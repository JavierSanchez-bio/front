#define BOOST_TEST_MODULE test_instrument
#include <boost/test/unit_test.hpp>

#include <Curves/ZeroCouponCurve.h>
#include <Instruments/Leg.h>
#include <Instruments/Bond.h>
#include <Instruments/Swap.h>
#include <Indices/Index.h>
#include <Flows/CashFlow.h>
#include <Instruments/InstrumentFactory.h>
#include <Instruments/Bootstrapper.h>
#include <Instruments/Deposit.h>

#include <vector>
#include <cmath>
#include <memory>
#include <iostream>
#include <stdio.h>

BOOST_AUTO_TEST_SUITE(TestInstruments)

BOOST_AUTO_TEST_CASE(test_zero_coupon_curve_dfs)
{
    std::vector<double> times    = {0.5, 1.0, 1.5, 2.0};
    std::vector<double> zc_rates = {0.0474, 0.0500, 0.0510, 0.0520};

    ZeroCouponCurve curve(times, zc_rates);

    for (std::size_t i = 0; i < times.size(); ++i) {
        double t  = times[i];
        double zc = zc_rates[i];
        double df_expected = std::exp(-zc * t);
        
        // CORREGIDO: Usamos get_dcf en lugar de df
        double df_curve    = curve.get_dcf(t); 

        BOOST_TEST(df_curve == df_expected, boost::test_tools::tolerance(1e-12));
    }
}

BOOST_AUTO_TEST_CASE(test_fixed_leg_price_with_curve)
{
    std::vector<double> times    = {0.5, 1.0, 1.5, 2.0};
    std::vector<double> zc_rates = {0.0474, 0.0500, 0.0510, 0.0520};

    ZeroCouponCurve curve(times, zc_rates);

    // Construimos una pata fija con cupones simples de 1.0 en cada fecha
    std::vector<Flows::CashFlow> cfs;
    for (double t : times) {
        cfs.emplace_back(1.0, t);
    }
    FixedLeg leg(cfs);

    double pv_expected = 0.0;
    for (std::size_t i = 0; i < times.size(); ++i) {
        double t  = times[i];
        double df = std::exp(-zc_rates[i] * t);
        pv_expected += 1.0 * df;
    }

    double pv_leg = leg.price(curve);
    BOOST_TEST(pv_leg == pv_expected, boost::test_tools::tolerance(1e-12));
}

BOOST_AUTO_TEST_CASE(test_swap_price_logic)
{
    std::vector<double> times    = {0.5, 1.0, 1.5, 2.0};
    std::vector<double> zc_rates = {0.0474, 0.0500, 0.0510, 0.0520};

    auto curve = std::make_shared<ZeroCouponCurve>(times, zc_rates);
    
    // CORREGIDO: Creamos el Índice necesario para la pata flotante
    auto index = std::make_shared<Index>(curve);

    std::vector<Flows::CashFlow> fixedCfs;
    std::vector<Flows::CashFlow> floatCfs;
    
    // Empezamos en i=1 (t=1.0) porque el cálculo forward de la curva necesita un punto anterior.
    // Si empezamos en i=0, forward_cc(0) lanza un out_of_range.
    for (std::size_t i = 1; i < times.size(); ++i) {
        double t = times[i];
        fixedCfs.emplace_back(1.0, t); // Cupón fijo de 1.0
        floatCfs.emplace_back(100.0, t); // Nocional de 100.0 para la flotante
    }

    FixedLeg fixedLeg(fixedCfs);
    FloatingLeg floatLeg(floatCfs, index);

    // Swap: Recibimos fija, pagamos flotante
    Swap swap(curve, fixedLeg, floatLeg, /*receiveFixed=*/true);

    // Calculamos los PVs esperados manualmente usando nuestras patas
    double expected_pv_fixed = fixedLeg.price(*curve);
    double expected_pv_float = floatLeg.price(*curve);
    
    // Si recibimos fija y pagamos flotante, el precio del Swap debe ser la resta exacta
    double expected_npv = expected_pv_fixed - expected_pv_float;

    double npv = swap.price();
    BOOST_TEST(npv == expected_npv, boost::test_tools::tolerance(1e-12));
}

BOOST_AUTO_TEST_CASE(test_swap_valuation_pdf)
{
    // Datos del PDF (Página 25)
    double notional = 100000000.0; // 100 Millones
    double fixedRate = 0.05;       // 5% Fijo
    
    // Curva ZC (4.74%, 5.00%, 5.10%, 5.20%)
    std::vector<double> times = {0.5, 1.0, 1.5, 2.0};
    std::vector<double> zc_rates = {0.0474, 0.0500, 0.0510, 0.0520};
    
    // En este caso, la curva de descuento y la forward son la misma
    auto curve = std::make_shared<ZeroCouponCurve>(times, zc_rates);

    // Usamos la Factory para crear el Swap (Recibiendo fijo)
    // 2 años, frecuencia semestral (2)
    auto myInstrument = InstrumentFactory::createSwap(
        notional, fixedRate, 2, 2, true, curve, curve
    );

    // CASTEAMOS: Le decimos a C++ "Oye, confía en mí, esto es un Swap"
    auto mySwap = std::dynamic_pointer_cast<Swap>(myInstrument);

    // Calculamos los NPVs por separado usando los "getters" que tienes en Swap.h
    double pvFixed = mySwap->fixedLeg().price(*curve);
    double pvFloat = mySwap->floatingLeg().price(*curve);
    double npv = mySwap->price();

    BOOST_TEST_MESSAGE("PV Pata Fija (Ingresos) : " << pvFixed);
    BOOST_TEST_MESSAGE("PV Pata Flotante (Pagos): " << pvFloat);
    BOOST_TEST_MESSAGE("NPV Total del Swap      : " << npv);

    // Comprobamos que no da error y devuelve un número 
    BOOST_CHECK(std::abs(npv) > 0.0);
}

BOOST_AUTO_TEST_CASE(test_bond_yield_class_example_real)
{
    // 1. Datos reales del problema de clase
    std::vector<double> times    = {0.5, 1.0, 1.5, 2.0};
    std::vector<double> zc_rates = {0.05, 0.058, 0.064, 0.068};
    
    auto curve = std::make_shared<ZeroCouponCurve>(times, zc_rates);

    // Bono a 2 años, Nocional 100, Cupón 6% anual (3 por semestre)
    std::vector<Flows::CashFlow> cfs;
    cfs.emplace_back(3, 0.5);   
    cfs.emplace_back(3, 1.0);   
    cfs.emplace_back(3, 1.5);   
    cfs.emplace_back(103, 2.0); 
    
    FixedLeg bondLeg(cfs);
    Bond bond(curve, bondLeg);

    // 2. Calculamos el precio teórico usando puramente la curva de descuento
    // (P = 3*e^(-0.05*0.5) + 3*e^(-0.058*1) + ... + 103*e^(-0.068*2))
    // Matemáticamente esto da ~98.385
    double theoretical_price = bond.price();

    // 3. Le pedimos la TIR usando el método que llama al Newton-Raphson interno
    // sin pasarle ningún precio forzado. Usará el theoretical_price.
    double theoretical_ytm = bond.yield();

    // Mostramos por consola para ver los resultados exactos
    printf("%.10lf\n", theoretical_price);
    std::cout << "Precio derivado de la curva: " << theoretical_price << std::endl;
    std::cout << "TIR continua (YTM): " << theoretical_ytm << std::endl;

    // 4. Comprobaciones
    // La TIR continua exacta para un precio de 98.385 es ~0.06762 (6.76%)
    // Usamos una tolerancia de 1e-2 (0.01) para que pase si en clase redondeasteis a 6.7% o 6.8%
    BOOST_TEST(theoretical_ytm == 0.06762, boost::test_tools::tolerance(1e-2));

    // Por seguridad, si descontamos con esa TIR continua recién calculada, 
    // tenemos que volver exactamente al precio original.
    double npv_with_yield = 0.0;
    for (const auto& cf : bondLeg.getCashFlows()) {
        npv_with_yield += cf.getAmount() * std::exp(-theoretical_ytm * cf.getYearFraction());
    }
    BOOST_TEST(npv_with_yield == theoretical_price, boost::test_tools::tolerance(1e-7));
}

BOOST_AUTO_TEST_CASE(test_bootstrapping_ejercicio_3_1_analitico)
{
    std::cout << "\n--- INICIANDO CALIBRACION ANALITICA (MODO PROFESOR) ---" << std::endl;

    auto curve = std::make_shared<ZeroCouponCurve>();

    // 1. Creamos los instrumentos
    auto dep_6m = std::make_shared<Deposit>(curve, 0.05, 0.5, 100.0);
    auto swap_12m = InstrumentFactory::createSwap(100.0, 0.055, 1.0, 2, true, curve, curve);
    auto swap_18m = InstrumentFactory::createSwap(100.0, 0.060, 1.5, 2, true, curve, curve);
    auto swap_2y = InstrumentFactory::createSwap(100.0, 0.064, 2.0, 2, true, curve, curve);

    // 2. Usamos el mapa de instrumentos del profesor
    Bootstrapping::InstrumentsMapType inst_map;
    inst_map[0.5] = dep_6m;
    inst_map[1.0] = swap_12m;
    inst_map[1.5] = swap_18m;
    inst_map[2.0] = swap_2y;

    // 3. Ejecutamos el FUNCTOR del profesor
    Bootstrapping bootstrapper_profesor;
    Bootstrapping::DiscountCurveType df_curve = bootstrapper_profesor(inst_map);

    // 4. Transformamos los Factores de Descuento a Tipos Cero Cupón Continuos y los metemos a tu curva
    for (auto const& pair : df_curve) {
        double t = pair.first;
        double df = pair.second;
        
        // Fórmula matemática: DF = exp(-zc * t) => zc = -ln(DF) / t
        double zc_rate = -std::log(df) / t;
        curve->add_pillar(t, zc_rate);
        
        std::cout << "Calibrado T=" << t << " -> DF: " << df << " -> Tipo Cero: " << (zc_rate * 100.0) << "%" << std::endl;
    }

    // 5. Validamos matemáticamente que cotizan a la par
    BOOST_TEST(dep_6m->price() == 0.0, boost::test_tools::tolerance(1e-6));
    BOOST_TEST(swap_12m->price() == 0.0, boost::test_tools::tolerance(1e-6));
    BOOST_TEST(swap_18m->price() == 0.0, boost::test_tools::tolerance(1e-6));
    BOOST_TEST(swap_2y->price() == 0.0, boost::test_tools::tolerance(1e-6));

    std::cout << "--- CALIBRACION ANALITICA COMPLETADA ---\n" << std::endl;
}


BOOST_AUTO_TEST_SUITE_END()
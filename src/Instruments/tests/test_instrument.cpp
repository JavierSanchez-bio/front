#define BOOST_TEST_MODULE test_instrument
#include <boost/test/unit_test.hpp>

#include <Instruments/ZeroCouponCurve.h>
#include <Instruments/Leg.h>
#include <Instruments/Swap.h>
#include <Instruments/Index.h>
#include <Flows/CashFlow.h>

#include <vector>
#include <cmath>
#include <memory>

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

BOOST_AUTO_TEST_SUITE_END()
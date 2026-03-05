#define BOOST_TEST_MODULE test_curves
#include <boost/test/unit_test.hpp>
#include "ZeroCouponCurve.h"

#include <vector>
#include <cmath>
#include <stdexcept>

BOOST_AUTO_TEST_SUITE(ZeroCouponCurve_Tests)

// 1. Test de inicialización y manejo de errores
BOOST_AUTO_TEST_CASE(test_curve_initialization_exceptions)
{
    std::vector<double> times_empty;
    std::vector<double> rates_empty;
    
    std::vector<double> times_ok = {1.0, 2.0};
    std::vector<double> rates_bad = {0.05}; // Diferente tamaño

    // Comprobamos que lance excepción si se pasan vectores de distinto tamaño
    BOOST_CHECK_THROW(ZeroCouponCurve(times_ok, rates_bad), std::invalid_argument);

    // Comprobamos que lance excepción si se inicializa con vectores vacíos
    BOOST_CHECK_THROW(ZeroCouponCurve(times_empty, rates_empty), std::invalid_argument);

    // Comprobamos que get_zc lance excepción si la curva construida por defecto está vacía
    ZeroCouponCurve empty_curve;
    BOOST_CHECK_THROW(empty_curve.get_zc(1.0), std::out_of_range);
}

// 2. Test de inserción dinámica de pilares (add_pillar)
BOOST_AUTO_TEST_CASE(test_curve_add_pillar)
{
    ZeroCouponCurve curve;
    
    // Añadimos pilares desordenados a propósito
    curve.add_pillar(2.0, 0.06);
    curve.add_pillar(0.5, 0.04);
    curve.add_pillar(1.0, 0.05);

    // Verificamos que los ha ordenado devolviendo los valores correctos
    BOOST_TEST(curve.get_zc(0.5) == 0.04, boost::test_tools::tolerance(1e-9));
    BOOST_TEST(curve.get_zc(1.0) == 0.05, boost::test_tools::tolerance(1e-9));
    BOOST_TEST(curve.get_zc(2.0) == 0.06, boost::test_tools::tolerance(1e-9));

    // Testeamos la actualización de un pilar existente (sobreescribir t=1.0)
    curve.add_pillar(1.0, 0.055);
    BOOST_TEST(curve.get_zc(1.0) == 0.055, boost::test_tools::tolerance(1e-9));
}

// 3. Test de Interpolación Lineal y Extrapolación Plana
BOOST_AUTO_TEST_CASE(test_curve_interpolation_and_extrapolation)
{
    ZeroCouponCurve curve;
    curve.add_pillar(1.0, 0.05); // 5% al año 1
    curve.add_pillar(2.0, 0.06); // 6% al año 2

    // a) Extrapolación Plana por la izquierda (t < 1.0)
    BOOST_TEST(curve.get_zc(0.5) == 0.05, boost::test_tools::tolerance(1e-9));
    
    // b) Extrapolación Plana por la derecha (t > 2.0)
    BOOST_TEST(curve.get_zc(3.0) == 0.06, boost::test_tools::tolerance(1e-9));

    // c) Interpolación Lineal exacta en el medio (t = 1.5)
    // El tipo debe ser exactamente la mitad entre 5% y 6% -> 5.5%
    BOOST_TEST(curve.get_zc(1.5) == 0.055, boost::test_tools::tolerance(1e-9));

    // d) Interpolación Lineal asimétrica (t = 1.25)
    // 25% del camino entre 5% y 6% -> 5.25%
    BOOST_TEST(curve.get_zc(1.25) == 0.0525, boost::test_tools::tolerance(1e-9));
}

// 4. Test del Factor de Descuento (Discount Factor)
BOOST_AUTO_TEST_CASE(test_curve_discount_factor)
{
    ZeroCouponCurve curve;
    curve.add_pillar(2.0, 0.06); // 6% continuo a 2 años

    // La fórmula es DF = exp(-r * t)
    double expected_df = std::exp(-0.06 * 2.0);
    
    BOOST_TEST(curve.get_dcf(2.0) == expected_df, boost::test_tools::tolerance(1e-9));
}

// 5. Test de Tipos Forward (Continuos y Simples)
BOOST_AUTO_TEST_CASE(test_curve_forward_rates)
{
    std::vector<double> times = {1.0, 2.0};
    std::vector<double> rates = {0.05, 0.06};
    ZeroCouponCurve curve(times, rates);

    // a) Forward continuo entre T1=1.0 y T2=2.0
    // F_cc = (R2*T2 - R1*T1) / (T2 - T1) = (0.06*2 - 0.05*1) / 1 = 0.12 - 0.05 = 0.07 (7%)
    double expected_fwd_cc = 0.07;
    BOOST_TEST(curve.forward_cc(1) == expected_fwd_cc, boost::test_tools::tolerance(1e-9));

    // b) Forward en t=0 (el primer elemento devuelve su propio zc_)
    BOOST_TEST(curve.forward_cc(0) == 0.05, boost::test_tools::tolerance(1e-9));

    // c) Forward simple (semianual m=2.0 por defecto) equivalente al forward continuo del 7%
    // F_simple = m * (exp(R_cc / m) - 1)
    double expected_fwd_simple = 2.0 * (std::exp(0.07 / 2.0) - 1.0);
    BOOST_TEST(curve.forward_simple(1) == expected_fwd_simple, boost::test_tools::tolerance(1e-9));
}

BOOST_AUTO_TEST_SUITE_END()
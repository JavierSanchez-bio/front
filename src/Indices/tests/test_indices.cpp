#define BOOST_TEST_MODULE test_indices
#include <boost/test/unit_test.hpp>

#include "Index.h"
#include "Curves/ZeroCouponCurve.h"
#include <memory>
#include <vector>

BOOST_AUTO_TEST_SUITE(Indices_Tests)

// 1. Test de inicialización segura
BOOST_AUTO_TEST_CASE(test_index_null_curve)
{
    // ¿Qué pasa si le pasamos un puntero nulo (curva vacía) por error?
    Index euribor(nullptr);
    
    // Debería ser robusto y devolver 0.0 sin colapsar el programa
    BOOST_TEST(euribor.get_fixing(1) == 0.0, boost::test_tools::tolerance(1e-9));
}

// 2. Test de cálculo de fixing (proyección forward)
BOOST_AUTO_TEST_CASE(test_index_forward_projection)
{
    // a) Creamos la curva (Nivel 0)
    std::vector<double> times = {1.0, 2.0};
    std::vector<double> rates = {0.04, 0.05}; // 4% a 1 año, 5% a 2 años
    auto fwd_curve = std::make_shared<ZeroCouponCurve>(times, rates);

    // b) Creamos el Índice (Nivel 1) inyectándole la curva
    Index euribor6m(fwd_curve);

    // c) Le pedimos al índice que proyecte el fixing para el periodo 1 (T=2.0)
    // Matemáticamente, el forward simple entre 1.0 y 2.0 con esos datos es aprox 6.09%
    // F_cc = (0.05*2 - 0.04*1)/1 = 0.06
    // F_simple (m=2) = 2 * (exp(0.06/2) - 1) = 0.060909...
    
    double expected_fixing = fwd_curve->forward_simple(1, 2.0); // El valor exacto
    double calculated_fixing = euribor6m.get_fixing(1, 2.0);

    BOOST_TEST(calculated_fixing == expected_fixing, boost::test_tools::tolerance(1e-9));
}

BOOST_AUTO_TEST_SUITE_END()
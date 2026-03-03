#define BOOST_TEST_MODULE test_maths
#include <boost/test/unit_test.hpp>
#include <Maths/NewtonRaphson.h>

BOOST_AUTO_TEST_SUITE(MathsSuite)
BOOST_AUTO_TEST_CASE(test_newton) {
    // Ejemplo: raíz de x^2 - 4 (es 2)
    auto f = [](double x) { return x*x - 4.0; };
    auto df = [](double x) { return 2.0*x; };
    double raiz = Maths::newtonRaphson(f, df, 1.0);
    BOOST_TEST(raiz == 2.0, boost::test_tools::tolerance(1e-5));
}
BOOST_AUTO_TEST_SUITE_END()
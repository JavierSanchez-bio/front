#define BOOST_TEST_MODULE test_maths
#include <boost/test/unit_test.hpp>
#include <boost/math/tools/roots.hpp>
#include <Maths/NewtonRaphson.h>

#include <cmath>
#include <utility>
#include <limits>
#include <cstdint>

BOOST_AUTO_TEST_SUITE(NewtonRaphson_Tests)

// Convergence test
// f(x) = x^3 * e^x - 8 * e^2
// f'(x) = x^2 * e^x * (x + 3)
// Exact root: x = 2
BOOST_AUTO_TEST_CASE(test_convergence)
{
    const double target_val = 8.0 * std::exp(2.0);
    
    auto f = [target_val](double x){ 
        return x * x * x * std::exp(x) - target_val; 
    };   
    auto df = [](double x){ 
        return x * x * std::exp(x) * (x + 3.0); 
    };

    double expected_root = 2.0;
    
    double root = Maths::newtonRaphson(f, df, 5.0);
    
    BOOST_TEST(root == expected_root, boost::test_tools::tolerance(1e-12));
}

// Division by zero
BOOST_AUTO_TEST_CASE(test_zero_division)
{
    auto f = [](double x) { return x * x - 1.0; };
    auto df = [](double x) { return 2.0 * x; };

    double result = Maths::newtonRaphson(f, df, 0.0);
    
    BOOST_TEST(result == 0.0, boost::test_tools::tolerance(1e-12));
}

// Comparation between boost and our method
BOOST_AUTO_TEST_CASE(test_with_boost)
{
    const double target_val = 8.0 * std::exp(2.0);

    auto f = [target_val](double x) { 
        return x * x * x * std::exp(x) - target_val; 
    };
    auto df = [](double x) { 
        return x * x * std::exp(x) * (x + 3.0); 
    };

    auto boost_func = [target_val](double x) {
        double fx = x * x * x * std::exp(x) - target_val;
        double dfx = x * x * std::exp(x) * (x + 3.0);
        return std::make_pair(fx, dfx);
    };

    double guess = 5.0;
    
    double root_custom = Maths::newtonRaphson(f, df, guess);

    const int digits = std::numeric_limits<double>::digits; 
    boost::uintmax_t max_iter = 100;
    
    double root_boost = boost::math::tools::newton_raphson_iterate(
        boost_func, guess, 0.0, 10.0, digits, max_iter);

    BOOST_TEST(root_custom == root_boost, boost::test_tools::tolerance(1e-13));
}

BOOST_AUTO_TEST_SUITE_END()
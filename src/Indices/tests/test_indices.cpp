#define BOOST_TEST_MODULE test_indices
#include <boost/test/unit_test.hpp>

#include "Index.h"
#include "Curves/ZeroCouponCurve.h"
#include <memory>
#include <vector>

BOOST_AUTO_TEST_SUITE(Index_Tests)

BOOST_AUTO_TEST_CASE(test_null_curve_pointer)
{
    std::shared_ptr<ZeroCouponCurve> null_curve = nullptr;
    Index index(null_curve);

    BOOST_TEST(index.get_fixing(1) == 0.0, boost::test_tools::tolerance(1e-12));
}

BOOST_AUTO_TEST_CASE(test_valid_fixing_delegation)
{
    std::vector<double> times = {1.0, 2.0};
    std::vector<double> rates = {0.05, 0.06};
    auto curve = std::make_shared<ZeroCouponCurve>(times, rates);
    
    Index index(curve);

    double expected_fixing = curve->forward_simple(1, 2.0);
    double actual_fixing = index.get_fixing(1); // m = 2.0 by default
    
    BOOST_TEST(actual_fixing == expected_fixing, boost::test_tools::tolerance(1e-12));
}

BOOST_AUTO_TEST_CASE(test_fixing_different_frequency)
{
    std::vector<double> times = {1.0, 2.0};
    std::vector<double> rates = {0.05, 0.06};
    auto curve = std::make_shared<ZeroCouponCurve>(times, rates);
    
    Index index(curve);

    double expected_fixing_quarterly = curve->forward_simple(1, 4.0);
    double actual_fixing_quarterly = index.get_fixing(1, 4.0);
    
    BOOST_TEST(actual_fixing_quarterly == expected_fixing_quarterly, boost::test_tools::tolerance(1e-12));
}

BOOST_AUTO_TEST_SUITE_END()
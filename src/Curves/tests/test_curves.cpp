#define BOOST_TEST_MODULE test_curves
#include <boost/test/unit_test.hpp>
#include "ZeroCouponCurve.h"

#include <cmath>
#include <vector>
#include <stdexcept>

BOOST_AUTO_TEST_SUITE(ZeroCouponCurve_Tests)

BOOST_AUTO_TEST_CASE(test_constructor_exceptions)
{
    std::vector<double> times = {1.0, 2.0};
    std::vector<double> rates_mismatch = {0.05};
    std::vector<double> empty_vec;

    BOOST_CHECK_THROW(ZeroCouponCurve(times, rates_mismatch), std::invalid_argument);
    
    BOOST_CHECK_THROW(ZeroCouponCurve(empty_vec, empty_vec), std::invalid_argument);

    ZeroCouponCurve empty_curve;
    BOOST_CHECK_THROW(empty_curve.get_zc(1.0), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(test_interpolation_and_extrapolation)
{
    std::vector<double> times = {1.0, 2.0, 3.0};
    std::vector<double> rates = {0.02, 0.03, 0.05};
    ZeroCouponCurve curve(times, rates);

    BOOST_TEST(curve.get_zc(0.5) == 0.02, boost::test_tools::tolerance(1e-12));

    BOOST_TEST(curve.get_zc(4.0) == 0.05, boost::test_tools::tolerance(1e-12));

    BOOST_TEST(curve.get_zc(1.0) == 0.02, boost::test_tools::tolerance(1e-12));
    BOOST_TEST(curve.get_zc(2.0) == 0.03, boost::test_tools::tolerance(1e-12));

    BOOST_TEST(curve.get_zc(1.5) == 0.025, boost::test_tools::tolerance(1e-12));
}

BOOST_AUTO_TEST_CASE(test_add_pillar)
{
    ZeroCouponCurve curve;
    
    curve.add_pillar(2.0, 0.04);
    BOOST_TEST(curve.get_zc(2.0) == 0.04, boost::test_tools::tolerance(1e-12));

    curve.add_pillar(1.0, 0.02);
    BOOST_TEST(curve.get_zc(1.5) == 0.03, boost::test_tools::tolerance(1e-12));

    curve.add_pillar(2.0, 0.06);
    BOOST_TEST(curve.get_zc(2.0) == 0.06, boost::test_tools::tolerance(1e-12));
    
    BOOST_TEST(curve.get_zc(1.5) == 0.04, boost::test_tools::tolerance(1e-12));
}

BOOST_AUTO_TEST_CASE(test_discount_factor)
{
    std::vector<double> times = {1.0, 2.0};
    std::vector<double> rates = {0.05, 0.06};
    ZeroCouponCurve curve(times, rates);

    double expected_df_1 = std::exp(-0.05 * 1.0);
    BOOST_TEST(curve.get_dcf(1.0) == expected_df_1, boost::test_tools::tolerance(1e-12));

    double expected_df_2 = std::exp(-0.055 * 1.5);
    BOOST_TEST(curve.get_dcf(1.5) == expected_df_2, boost::test_tools::tolerance(1e-12));
}

BOOST_AUTO_TEST_CASE(test_forward_rates)
{
    std::vector<double> times = {1.0, 2.0};
    std::vector<double> rates = {0.05, 0.06};
    ZeroCouponCurve curve(times, rates);

    BOOST_TEST(curve.forward_cc(0) == 0.05, boost::test_tools::tolerance(1e-12));

    double expected_fwd_cc = 0.07;
    BOOST_TEST(curve.forward_cc(1) == expected_fwd_cc, boost::test_tools::tolerance(1e-12));

    double expected_fwd_simple = 2.0 * (std::exp(expected_fwd_cc / 2.0) - 1.0);
    BOOST_TEST(curve.forward_simple(1, 2.0) == expected_fwd_simple, boost::test_tools::tolerance(1e-12));
}

BOOST_AUTO_TEST_SUITE_END()
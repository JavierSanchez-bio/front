#define BOOST_TEST_MODULE test_flows
#include <boost/test/unit_test.hpp>
#include <Flows/CashFlow.h>

#include <cmath>

BOOST_AUTO_TEST_SUITE(CashFlow_Tests)

BOOST_AUTO_TEST_CASE(test_initialization)
{
    Flows::CashFlow cf(100.0, 0.5);
    
    BOOST_TEST(cf.getAmount() == 100.0);
    BOOST_TEST(cf.getYearFraction() == 0.5);
}

BOOST_AUTO_TEST_CASE(test_present_value_standard)
{
    Flows::CashFlow cf(100.0, 2.0);
    double rate = 0.05;
    
    double expected_pv = 100.0 * std::exp(-rate * 2.0);
    
    BOOST_TEST(cf.presentValue(rate) == expected_pv, boost::test_tools::tolerance(1e-12));
}

BOOST_AUTO_TEST_CASE(test_present_value_zero_rate)
{
    Flows::CashFlow cf(100.0, 5.0);
    double rate = 0.0;
    
    BOOST_TEST(cf.presentValue(rate) == 100.0, boost::test_tools::tolerance(1e-12));
}

BOOST_AUTO_TEST_CASE(test_present_value_immediate)
{
    Flows::CashFlow cf(150.0, 0.0);
    double rate = 0.10;
    
    BOOST_TEST(cf.presentValue(rate) == 150.0, boost::test_tools::tolerance(1e-12));
}

BOOST_AUTO_TEST_CASE(test_present_value_negative_amount)
{
    Flows::CashFlow cf(-50.0, 1.0);
    double rate = 0.03;
    
    double expected_pv = -50.0 * std::exp(-rate * 1.0);
    
    BOOST_TEST(cf.presentValue(rate) == expected_pv, boost::test_tools::tolerance(1e-12));
}

BOOST_AUTO_TEST_SUITE_END()
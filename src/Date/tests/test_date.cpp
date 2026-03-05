#define BOOST_TEST_MODULE DateTests
#include <boost/test/unit_test.hpp>
#include "../DayCount.h"
#include <stdexcept>

BOOST_AUTO_TEST_SUITE(Date_DayCount_Tests)

// Easy case
BOOST_AUTO_TEST_CASE(test_basic)
{
    BOOST_TEST(Actual_360::compute_daycount("2023-01-01", "2023-01-02") == 1);
    BOOST_TEST(Thirty_360::compute_daycount("2023-01-01", "2023-01-02") == 1);

    BOOST_TEST(Actual_360::compute_daycount("2023-01-15", "2023-02-15") == 31);
    BOOST_TEST(Thirty_360::compute_daycount("2023-01-15", "2023-02-15") == 30);

    BOOST_TEST(Actual_360::compute_daycount("2023-01-10", "2024-01-10") == 365);
    BOOST_TEST(Thirty_360::compute_daycount("2023-01-10", "2024-01-10") == 360);
}

//End of month and bisiestos
BOOST_AUTO_TEST_CASE(test_actual360_month_year)
{
    BOOST_TEST(Actual_360::compute_daycount("2023-02-28", "2023-03-01") == 1);
    BOOST_TEST(Actual_360::compute_daycount("2024-01-28", "2024-03-01") == 2);
}

//30/360
BOOST_AUTO_TEST_CASE(test_thirty360_rules)
{
    BOOST_TEST(Thirty_360::compute_daycount("2023-01-31", "2023-03-31") == 60);
    BOOST_TEST(Thirty_360::compute_daycount("2023-01-30", "2023-01-31") == 0);
    BOOST_TEST(Thirty_360::compute_daycount("2023-02-28", "2023-03-01") == 3);
}

//Stress and robustness
BOOST_AUTO_TEST_CASE(test_thirty360_cross_year_and_reversed)
{
    BOOST_TEST(Thirty_360::compute_daycount("2023-11-15", "2024-02-15") == 90);

    BOOST_TEST(Actual_360::compute_daycount("2023-01-11", "2023-01-01") == -10);
    BOOST_TEST(Thirty_360::compute_daycount("2023-01-11", "2023-01-01") == -10);
}

//Error control
BOOST_AUTO_TEST_CASE(test_invalid_dates_and_dt_generation)
{
    BOOST_CHECK_THROW(DayCountCalculator::make_date("2023-02-29"), std::out_of_range);
    
    BOOST_CHECK_THROW(DayCountCalculator::make_date("2023-13-01"), std::out_of_range);

    BOOST_CHECK_THROW(DayCountCalculator::make_date("esto-es-basura"), std::exception);

    auto start = DayCountCalculator::make_date("2023-01-01");
    auto end   = DayCountCalculator::make_date("2024-01-01");
    
    Actual_360 act360;
    Thirty_360 th360;
    
    BOOST_TEST(act360(start, end) == 365.0 / 360.0, boost::test_tools::tolerance(1e-12));
    BOOST_TEST(th360(start, end) == 1.0, boost::test_tools::tolerance(1e-12));
}

BOOST_AUTO_TEST_SUITE_END()
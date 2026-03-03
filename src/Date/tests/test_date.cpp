#define BOOST_TEST_MODULE test_date
#include <boost/test/unit_test.hpp>
#include <Date/DayCount.h>

// ESTO ES LO QUE BUSCA EL SCRIPT DEL PROFESOR:
BOOST_AUTO_TEST_SUITE(DateSuite)

BOOST_AUTO_TEST_CASE(test_act360)
{
    double resultado = Date::DayCount::yearFraction(90.0, Date::Convention::ACT_360);
    BOOST_TEST(resultado == 0.25);
}

BOOST_AUTO_TEST_SUITE_END()
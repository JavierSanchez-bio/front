#define BOOST_TEST_MODULE test_flows
#include <boost/test/unit_test.hpp>
#include <Flows/CashFlow.h>

BOOST_AUTO_TEST_SUITE(FlowsSuite)

BOOST_AUTO_TEST_CASE(test_cashflow_pv)
{
    double nominal = 1000.0;
    double t = 2.0;    // 2 años
    double r = 0.05;   // 5% interés
    
    Flows::CashFlow cf(nominal, t);
    
    // PV esperado = 1000 / (1.05^2) = 907.029
    double esperado = 907.029478;
    double resultado = cf.presentValue(r);

    BOOST_TEST(resultado == esperado, boost::test_tools::tolerance(1e-6));
}

BOOST_AUTO_TEST_SUITE_END()
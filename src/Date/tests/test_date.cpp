#define BOOST_TEST_MODULE test_date
#include <boost/test/unit_test.hpp>
#include "DayCount.h" // Ajusta esta ruta si es necesario

// ESTO ES LO QUE BUSCA EL SCRIPT DEL PROFESOR:
BOOST_AUTO_TEST_SUITE(DateSuite)

// --- Pruebas para Actual/360 ---

BOOST_AUTO_TEST_CASE(test_act360_dias_basico)
{
    // Prueba usando strings (30 días exactos en enero)
    short dias = Actual_360::compute_daycount("2023-01-01", "2023-01-31");
    BOOST_TEST(dias == 30);
}

BOOST_AUTO_TEST_CASE(test_act360_bisiesto)
{
    // Prueba usando fechas reales de Boost (año bisiesto: 29 días en febrero)
    boost::gregorian::date d1(2024, 2, 1);
    boost::gregorian::date d2(2024, 3, 1);
    short dias = Actual_360::compute_daycount(d1, d2);
    
    BOOST_TEST(dias == 29);
}

BOOST_AUTO_TEST_CASE(test_act360_fraccion_ano)
{
    // Prueba del Functor (operator()) que devuelve el factor de descuento (double)
    Actual_360 act360_calc;
    boost::gregorian::date d1(2023, 1, 1);
    boost::gregorian::date d2(2023, 4, 1); // 90 días exactos en un año no bisiesto
    
    double resultado = act360_calc(d1, d2);
    
    // Al comparar doubles, es buena práctica usar una tolerancia
    BOOST_TEST(resultado == 0.25, boost::test_tools::tolerance(1e-6));
}

// --- Pruebas para 30/360 ---

BOOST_AUTO_TEST_CASE(test_thirty360_dias_basico)
{
    // De mediados de mes a mediados de mes (6 meses exactos -> 180 días)
    short dias = Thirty_360::compute_daycount("2023-01-15", "2023-07-15");
    BOOST_TEST(dias == 180);
}

BOOST_AUTO_TEST_CASE(test_thirty360_salto_ano)
{
    // Un año completo en 30/360 SIEMPRE son 360 días
    short dias = Thirty_360::compute_daycount("2023-01-01", "2024-01-01");
    BOOST_TEST(dias == 360);
}

BOOST_AUTO_TEST_CASE(test_thirty360_fraccion_ano)
{
    // Prueba del Functor para 30/360
    Thirty_360 thirty360_calc;
    boost::gregorian::date d1(2023, 1, 15);
    boost::gregorian::date d2(2023, 7, 15); // 180 días calculados en 30/360
    
    double resultado = thirty360_calc(d1, d2);
    
    BOOST_TEST(resultado == 0.50, boost::test_tools::tolerance(1e-6));
}

BOOST_AUTO_TEST_SUITE_END()
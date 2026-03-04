#define BOOST_TEST_MODULE test_instrument
#include <boost/test/unit_test.hpp>

#include <Instruments/ZeroCouponCurve.h>
#include <Instruments/Leg.h>
#include <Flows/CashFlow.h>

#include <vector>
#include <cmath>

BOOST_AUTO_TEST_SUITE(TestInstruments)

BOOST_AUTO_TEST_CASE(test_zero_coupon_curve_dfs)
{
    std::vector<double> times     = {0.5, 1.0, 1.5, 2.0};
    std::vector<double> zc_rates  = {0.0474, 0.0500, 0.0510, 0.0520};

    ZeroCouponCurve curve(times, zc_rates);

    for (std::size_t i = 0; i < times.size(); ++i) {
        double t   = times[i];
        double zc  = zc_rates[i];
        double df_expected = std::exp(-zc * t);
        double df_curve    = curve.df(t);

        BOOST_TEST(df_curve == df_expected, boost::test_tools::tolerance(1e-12));
    }
}

BOOST_AUTO_TEST_CASE(test_fixed_leg_price_with_curve)
{
    std::vector<double> times     = {0.5, 1.0, 1.5, 2.0};
    std::vector<double> zc_rates  = {0.0474, 0.0500, 0.0510, 0.0520};

    ZeroCouponCurve curve(times, zc_rates);

    // Construimos una pata fija con cupones simples de 1.0 en cada fecha
    std::vector<Flows::CashFlow> cfs;
    for (double t : times) {
        cfs.emplace_back(1.0, t);
    }
    FixedLeg leg(cfs);

    double pv_expected = 0.0;
    for (std::size_t i = 0; i < times.size(); ++i) {
        double t  = times[i];
        double df = std::exp(-zc_rates[i] * t);
        pv_expected += 1.0 * df;
    }

    double pv_leg = leg.price(curve);
    BOOST_TEST(pv_leg == pv_expected, boost::test_tools::tolerance(1e-12));
}

BOOST_AUTO_TEST_SUITE_END()


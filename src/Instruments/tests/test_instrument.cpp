#define BOOST_TEST_MODULE test_instrument
#include <boost/test/unit_test.hpp>

#include <Curves/ZeroCouponCurve.h>
#include <Instruments/Leg.h>
#include <Instruments/Bond.h>
#include <Instruments/Swap.h>
#include <Indices/Index.h>
#include <Flows/CashFlow.h>
#include <Instruments/InstrumentFactory.h>
#include <Instruments/Bootstrapper.h>
#include <Instruments/Deposit.h>

#include <vector>
#include <cmath>
#include <memory>
#include <iostream>
#include <iomanip>

BOOST_AUTO_TEST_SUITE(TestInstruments)

BOOST_AUTO_TEST_CASE(test_zero_coupon_curve_dfs)
{
    std::vector<double> times    = {0.5, 1.0, 1.5, 2.0};
    std::vector<double> zc_rates = {0.0474, 0.0500, 0.0510, 0.0520};

    ZeroCouponCurve curve(times, zc_rates);

    for (std::size_t i = 0; i < times.size(); ++i) {
        double t  = times[i];
        double zc = zc_rates[i];
        double df_expected = std::exp(-zc * t);
        
        double df_curve = curve.get_dcf(t); 

        BOOST_TEST(df_curve == df_expected, boost::test_tools::tolerance(1e-12));
    }
}

BOOST_AUTO_TEST_CASE(test_leg_basic_operations)
{
    Leg my_leg;
    
    BOOST_TEST(my_leg.size() == 0);

    my_leg.addCashFlow(Flows::CashFlow(100.0, 1.0));
    my_leg.addCashFlow(Flows::CashFlow(105.0, 2.0));

    BOOST_TEST(my_leg.size() == 2);
    BOOST_TEST(my_leg.getCashFlows()[0].getAmount() == 100.0);
    BOOST_TEST(my_leg.getCashFlows()[1].getYearFraction() == 2.0);
}

BOOST_AUTO_TEST_CASE(test_floating_leg_with_spread)
{
    std::vector<double> times = {1.0, 2.0};
    std::vector<double> rates = {0.05, 0.05};
    auto curve = std::make_shared<ZeroCouponCurve>(times, rates);
    auto index = std::make_shared<Index>(curve);

    std::vector<Flows::CashFlow> cfs = {
        Flows::CashFlow(100.0, 1.0),
        Flows::CashFlow(100.0, 2.0)
    };

    double spread = 0.01;
    FloatingLeg float_leg(cfs, index, spread);

    double rate1 = index->get_fixing(0) + spread;
    double expected_pmt1 = 100.0 * rate1 * 1.0;
    double expected_pv1 = expected_pmt1 * curve->get_dcf(1.0);

    double rate2 = index->get_fixing(1) + spread;
    double expected_pmt2 = 100.0 * rate2 * 1.0;
    double expected_pv2 = expected_pmt2 * curve->get_dcf(2.0);

    double total_expected_pv = expected_pv1 + expected_pv2;

    BOOST_TEST(float_leg.price(*curve) == total_expected_pv, boost::test_tools::tolerance(1e-12));
}

BOOST_AUTO_TEST_CASE(test_fixed_leg_price_with_curve)
{
    std::vector<double> times    = {0.5, 1.0, 1.5, 2.0};
    std::vector<double> zc_rates = {0.0474, 0.0500, 0.0510, 0.0520};

    ZeroCouponCurve curve(times, zc_rates);

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

BOOST_AUTO_TEST_CASE(test_deposit_discount_factors_and_annuity)
{
    Deposit dep(nullptr, 0.05, 0.5);

    double expected_df = 1.0 / 1.025;
    BOOST_TEST(dep.impliedDiscountFactor() == expected_df, boost::test_tools::tolerance(1e-8));

    double base_annuity = 10.0;
    double returned_df = dep.computeDiscountFactor(base_annuity);
    
    BOOST_TEST(returned_df == expected_df, boost::test_tools::tolerance(1e-8));
    
    double expected_annuity = 10.0 + (expected_df * 0.5);
    BOOST_TEST(base_annuity == expected_annuity, boost::test_tools::tolerance(1e-8));
}

BOOST_AUTO_TEST_CASE(test_deposit_price)
{
    std::vector<double> times = {0.5, 1.0};
    std::vector<double> rates = {0.04, 0.04}; 
    auto curve = std::make_shared<ZeroCouponCurve>(times, rates);
    
    Deposit dep(curve, 0.05, 0.5, 100.0);

    double future_cashflow = 102.5;
    
    double curve_df = std::exp(-0.04 * 0.5);
    
    double expected_price = (future_cashflow * curve_df) - 100.0;

    BOOST_TEST(dep.price() == expected_price, boost::test_tools::tolerance(1e-8));
}

BOOST_AUTO_TEST_CASE(test_swap_price_logic)
{
    std::vector<double> times    = {0.5, 1.0, 1.5, 2.0};
    std::vector<double> zc_rates = {0.0474, 0.0500, 0.0510, 0.0520};

    auto curve = std::make_shared<ZeroCouponCurve>(times, zc_rates);
    
    auto index = std::make_shared<Index>(curve);

    std::vector<Flows::CashFlow> fixedCfs;
    std::vector<Flows::CashFlow> floatCfs;
    
    for (std::size_t i = 1; i < times.size(); ++i) {
        double t = times[i];
        fixedCfs.emplace_back(1.0, t);
        floatCfs.emplace_back(100.0, t);
    }

    FixedLeg fixedLeg(fixedCfs);
    FloatingLeg floatLeg(floatCfs, index);

    Swap swap(curve, fixedLeg, floatLeg, true);

    double expected_pv_fixed = fixedLeg.price(*curve);
    double expected_pv_float = floatLeg.price(*curve);
    
    double expected_npv = expected_pv_fixed - expected_pv_float;

    double npv = swap.price();
    BOOST_TEST(npv == expected_npv, boost::test_tools::tolerance(1e-12));
}

BOOST_AUTO_TEST_CASE(test_swap_valuation_pdf)
{
    double notional = 100000000.0; 
    double fixedRate = 0.05;       
    
    std::vector<double> times = {0.5, 1.0, 1.5, 2.0};
    std::vector<double> zc_rates = {0.0474, 0.0500, 0.0510, 0.0520};
    
    auto curve = std::make_shared<ZeroCouponCurve>(times, zc_rates);

    auto myInstrument = InstrumentFactory::createSwap(
        notional, fixedRate, 2, 2, true, curve, curve
    );

    auto mySwap = std::dynamic_pointer_cast<Swap>(myInstrument);

    double pvFixed = mySwap->fixedLeg().price(*curve);
    double pvFloat = mySwap->floatingLeg().price(*curve);
    double npv = mySwap->price();

    BOOST_TEST_MESSAGE("PV Pata Fija (Ingresos) : " << pvFixed);
    BOOST_TEST_MESSAGE("PV Pata Flotante (Pagos): " << pvFloat);
    BOOST_TEST_MESSAGE("NPV Total del Swap      : " << npv);

    BOOST_CHECK(std::abs(npv) > 0.0);
}

BOOST_AUTO_TEST_CASE(test_bond_yield_class_example_real)
{
    std::vector<double> times    = {0.5, 1.0, 1.5, 2.0};
    std::vector<double> zc_rates = {0.05, 0.058, 0.064, 0.068};
    
    auto curve = std::make_shared<ZeroCouponCurve>(times, zc_rates);

    std::vector<Flows::CashFlow> cfs;
    cfs.emplace_back(3, 0.5);   
    cfs.emplace_back(3, 1.0);   
    cfs.emplace_back(3, 1.5);   
    cfs.emplace_back(103, 2.0); 
    
    FixedLeg bondLeg(cfs);
    Bond bond(curve, bondLeg);

    double theoretical_price = bond.price();
    double theoretical_ytm = bond.yield();

    std::cout << "Precio derivado de la curva: " << std::fixed << std::setprecision(6) << theoretical_price << std::endl;
    std::cout << "TIR continua (YTM): " << std::fixed << std::setprecision(8) << theoretical_ytm << std::endl;

    double expected_class_ytm = 0.06762282; 
    BOOST_TEST(theoretical_ytm == expected_class_ytm, boost::test_tools::tolerance(1e-4));

    double npv_with_yield = 0.0;
    for (const auto& cf : bondLeg.getCashFlows()) {
        npv_with_yield += cf.getAmount() * std::exp(-theoretical_ytm * cf.getYearFraction());
    }
    
    BOOST_TEST(npv_with_yield == theoretical_price, boost::test_tools::tolerance(1e-8));
}

BOOST_AUTO_TEST_CASE(test_instrument_factory_create_swap_structure)
{
    auto curve = std::make_shared<ZeroCouponCurve>();
    
    double notional = 1000.0;
    double fixedRate = 0.05;
    double years = 2.0;
    int frequency = 2; 
    
    auto instrument = InstrumentFactory::createSwap(
        notional, fixedRate, years, frequency, true, curve, curve
    );
    
    BOOST_REQUIRE(instrument != nullptr);
    auto swap = std::dynamic_pointer_cast<Swap>(instrument);
    BOOST_REQUIRE(swap != nullptr);
    
    BOOST_TEST(swap->fixedLeg().size() == 4);
    BOOST_TEST(swap->floatingLeg().size() == 4);
 
    const auto& fixed_cfs = swap->fixedLeg().getCashFlows();
    BOOST_TEST(fixed_cfs[0].getAmount() == 25.0);
    BOOST_TEST(fixed_cfs[0].getYearFraction() == 0.5);
    BOOST_TEST(fixed_cfs[3].getYearFraction() == 2.0);
    
    const auto& floating_cfs = swap->floatingLeg().getCashFlows();
    BOOST_TEST(floating_cfs[0].getAmount() == 1000.0);
    BOOST_TEST(floating_cfs[3].getAmount() == 1000.0);
}

BOOST_AUTO_TEST_CASE(test_bootstrapping_ejercicio_3_1_analitico)
{
    auto curve = std::make_shared<ZeroCouponCurve>();

    auto dep_6m = std::make_shared<Deposit>(curve, 0.05, 0.5, 100.0);
    auto swap_12m = InstrumentFactory::createSwap(100.0, 0.055, 1.0, 2, true, curve, curve);
    auto swap_18m = InstrumentFactory::createSwap(100.0, 0.060, 1.5, 2, true, curve, curve);
    auto swap_2y = InstrumentFactory::createSwap(100.0, 0.064, 2.0, 2, true, curve, curve);

    Bootstrapping::InstrumentsMapType inst_map;
    inst_map[0.5] = dep_6m;
    inst_map[1.0] = swap_12m;
    inst_map[1.5] = swap_18m;
    inst_map[2.0] = swap_2y;

    Bootstrapping bootstrapper;
    Bootstrapping::DiscountCurveType df_curve = bootstrapper(inst_map);

    for (auto const& pair : df_curve) {
        double t = pair.first;
        double df = pair.second;
        
        double zc_rate = -std::log(df) / t;
        curve->add_pillar(t, zc_rate);
    }

    BOOST_TEST(dep_6m->price() == 0.0, boost::test_tools::tolerance(1e-6));
    BOOST_TEST(swap_12m->price() == 0.0, boost::test_tools::tolerance(1e-6));
    BOOST_TEST(swap_18m->price() == 0.0, boost::test_tools::tolerance(1e-6));
    BOOST_TEST(swap_2y->price() == 0.0, boost::test_tools::tolerance(1e-6));
}


BOOST_AUTO_TEST_SUITE_END()
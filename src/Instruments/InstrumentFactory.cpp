#include "InstrumentFactory.h"
#include "Swap.h"
#include "Indices/Index.h"
#include "Flows/CashFlow.h"

std::shared_ptr<Instrument> InstrumentFactory::createSwap(
    double notional,
    double fixedRate,
    double years,
    int frequency,
    bool receiveFixed,
    std::shared_ptr<ZeroCouponCurve> discountCurve,
    std::shared_ptr<ZeroCouponCurve> forwardCurve)
{
    std::vector<Flows::CashFlow> fixedFlows;
    std::vector<Flows::CashFlow> floatingFlows;

    int totalPeriods = static_cast<int>(years * frequency);
    double dt = 1.0 / frequency;

    for (int i = 1; i <= totalPeriods; ++i) {
        double t = i * dt;

        fixedFlows.emplace_back(notional * fixedRate * dt, t);
        floatingFlows.emplace_back(notional, t);
    }

    FixedLeg fixedLeg(fixedFlows);
    auto euribor = std::make_shared<Index>(forwardCurve);
    FloatingLeg floatingLeg(floatingFlows, euribor, 0.0);

    return std::make_shared<Swap>(discountCurve, fixedLeg, floatingLeg, receiveFixed, fixedRate, dt);
}
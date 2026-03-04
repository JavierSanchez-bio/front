#include "InstrumentFactory.h"
#include "Swap.h"
#include "Index.h"
#include <Flows/CashFlow.h>

std::shared_ptr<Instrument> InstrumentFactory::createSwap(
    double notional,
    double fixedRate,
    int years,
    int frequency,
    bool receiveFixed,
    std::shared_ptr<ZeroCouponCurve> discountCurve,
    std::shared_ptr<ZeroCouponCurve> forwardCurve)
{
    std::vector<Flows::CashFlow> fixedFlows;
    std::vector<Flows::CashFlow> floatingFlows;

    int totalPeriods = years * frequency;
    double dt = 1.0 / frequency;

    for (int i = 1; i <= totalPeriods; ++i) {
        double t = i * dt;
        // Pata Fija: Nocional * TipoFijo * dt
        fixedFlows.emplace_back(notional * fixedRate * dt, t);
        // Pata Flotante: Pasamos el Nocional para que FloatingLeg lo multiplique por el índice
        floatingFlows.emplace_back(notional, t);
    }

    FixedLeg fixedLeg(fixedFlows);
    auto euribor = std::make_shared<Index>(forwardCurve);
    FloatingLeg floatingLeg(floatingFlows, euribor, 0.0);

    return std::make_shared<Swap>(discountCurve, fixedLeg, floatingLeg, receiveFixed);
}
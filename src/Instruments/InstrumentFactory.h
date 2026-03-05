#ifndef INSTRUMENT_FACTORY_H
#define INSTRUMENT_FACTORY_H

#include <memory>
#include <vector>

class Instrument;
class ZeroCouponCurve;

class InstrumentFactory {
public:
    static std::shared_ptr<Instrument> createSwap(
        double notional,
        double fixedRate,
        double years,
        int frequency,
        bool receiveFixed,
        std::shared_ptr<ZeroCouponCurve> discountCurve,
        std::shared_ptr<ZeroCouponCurve> forwardCurve);
};

#endif
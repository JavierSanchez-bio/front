#ifndef INSTRUMENT_FACTORY_H
#define INSTRUMENT_FACTORY_H

#include <memory>
#include <vector>

class Instrument;
class ZeroCouponCurve;

class InstrumentFactory {
public:
    static std::shared_ptr<Instrument> createVanillaSwap(
        double notional,
        double fixedRate,
        int years,
        int frequency,
        bool receiveFixed,
        std::shared_ptr<ZeroCouponCurve> discountCurve,
        std::shared_ptr<ZeroCouponCurve> forwardCurve);
};

#endif // INSTRUMENT_FACTORY_H
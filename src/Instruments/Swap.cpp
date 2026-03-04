#include "Swap.h"

Swap::Swap(std::shared_ptr<ZeroCouponCurve> curve,
           const FixedLeg& fixedLeg,
           const FloatingLeg& floatingLeg,
           bool receiveFixed)
    : Instrument(std::move(curve))
    , fixedLeg_(fixedLeg)
    , floatingLeg_(floatingLeg)
    , receiveFixed_(receiveFixed)
{
}

double Swap::price() const
{
    double pvFixed   = fixedLeg_.price(*discount_curve);
    double pvFloating = floatingLeg_.price(*discount_curve);

    if (receiveFixed_) {
        return pvFixed - pvFloating;
    }
    return pvFloating - pvFixed;
}


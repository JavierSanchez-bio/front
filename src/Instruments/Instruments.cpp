#include "Instruments.h"

Instrument::Instrument(std::shared_ptr<ZeroCouponCurve> curve) 
    : discount_curve(curve) {
}
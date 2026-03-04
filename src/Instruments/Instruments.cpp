#include "Instruments.h"

// Implementación del constructor. 
// Simplemente guardamos la curva que nos pasan para que Bond o Swap la usen después.
Instrument::Instrument(std::shared_ptr<ZeroCouponCurve> curve) 
    : discount_curve(curve) {
}
#include "Swap.h"

Swap::Swap(std::shared_ptr<ZeroCouponCurve> curve, const FixedLeg& fixed, const FloatingLeg& floating, bool receiveFixed, double fixedRate, double dt)
    : Instrument(std::move(curve)), fixedLeg_(fixed), floatingLeg_(floating), receiveFixed_(receiveFixed), fixedRate_(fixedRate), dt_(dt) {}

double Swap::computeDiscountFactor(double& annuity) const {
    // Despeje analítico del Factor de Descuento de un Swap cotizando a la par
    double df = (1.0 - fixedRate_ * annuity) / (1.0 + fixedRate_ * dt_);
    
    // Actualizamos la anualidad añadiendo este nuevo plazo para el Swap siguiente
    annuity += df * dt_;
    return df;
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


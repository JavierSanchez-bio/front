#ifndef SWAP_H
#define SWAP_H

#include <memory>

#include "Instruments.h"
#include "Leg.h"

// Swap de tipos de interés muy simplificado:
// - Una pata fija
// - Una pata flotante
// - Signo configurable (recibimos fija o flotante)
class Swap : public Instrument {
public:
    // Si receiveFixed == true, el valor es PV(Fixed) - PV(Floating)
    // Si receiveFixed == false, el valor es PV(Floating) - PV(Fixed)
    // Constructor nuevo
    Swap(std::shared_ptr<ZeroCouponCurve> curve, 
        const FixedLeg& fixed, 
        const FloatingLeg& floating, 
        bool receiveFixed, 
        double fixedRate = 0.0, 
        double dt = 0.0);

// Función del profesor
double computeDiscountFactor(double& annuity) const override;

    double price() const override;

    const FixedLeg&   fixedLeg() const { return fixedLeg_; }
    const FloatingLeg& floatingLeg() const { return floatingLeg_; }

private:
    FixedLeg   fixedLeg_;
    FloatingLeg floatingLeg_;
    bool       receiveFixed_;
    double fixedRate_;
    double dt_;
};

#endif // SWAP_H


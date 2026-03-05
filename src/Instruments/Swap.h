#ifndef SWAP_H
#define SWAP_H

#include <memory>

#include "Instruments.h"
#include "Leg.h"


class Swap : public Instrument {
public:

    Swap(std::shared_ptr<ZeroCouponCurve> curve, 
        const FixedLeg& fixed, 
        const FloatingLeg& floating, 
        bool receiveFixed, 
        double fixedRate = 0.0, 
        double dt = 0.0);

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

#endif


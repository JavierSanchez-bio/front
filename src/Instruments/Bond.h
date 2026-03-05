#ifndef BOND_H
#define BOND_H

#include "Instruments.h" 
#include "Leg.h"
#include <memory>

class Bond : public Instrument {
private:
    Leg leg_;

public:
    Bond(std::shared_ptr<ZeroCouponCurve> curve, const Leg& leg);

    ~Bond() override = default;

    double price() const override;

    double yield(double target_price) const;

    // Computes the YTM based on the current theoretical bond price
    double yield() const;
};

#endif
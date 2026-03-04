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
};

#endif // BOND_H
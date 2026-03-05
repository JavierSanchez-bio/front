#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <memory>

class ZeroCouponCurve; 

class Instrument {
protected:

    std::shared_ptr<ZeroCouponCurve> discount_curve;

public:

    Instrument(std::shared_ptr<ZeroCouponCurve> curve);

    virtual ~Instrument() = default;

    virtual double price() const = 0; 

    virtual double computeDiscountFactor(double& annuity) const { 
        return 0.0; 
    }
};

#endif
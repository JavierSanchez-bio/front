#ifndef DEPOSIT_H
#define DEPOSIT_H

#include "Instruments.h"
#include <memory>

class Deposit : public Instrument {
private:
    double rate_;         
    double yearFraction_; 
    double notional_;     

public:

    Deposit(std::shared_ptr<ZeroCouponCurve> curve, 
            double rate, 
            double yearFraction, 
            double notional = 100.0);

    ~Deposit() override = default;

    double price() const override;

    double impliedDiscountFactor() const;
    double computeDiscountFactor(double& annuity) const override;
    
    double getRate() const { return rate_; }
    double getYearFraction() const { return yearFraction_; }
};

#endif
#include "Deposit.h"
#include "Curves/ZeroCouponCurve.h"

Deposit::Deposit(std::shared_ptr<ZeroCouponCurve> curve, 
                 double rate, 
                 double yearFraction, 
                 double notional)
    : Instrument(std::move(curve)), 
      rate_(rate), 
      yearFraction_(yearFraction), 
      notional_(notional)
{
}

double Deposit::price() const
{
    if (!discount_curve) return 0.0;

    double df = discount_curve->get_dcf(yearFraction_); 
    
    double future_cashflow = notional_ * (1.0 + rate_ * yearFraction_);
    
    return future_cashflow * df - notional_;
}

double Deposit::impliedDiscountFactor() const
{
    return 1.0 / (1.0 + rate_ * yearFraction_);
}

double Deposit::computeDiscountFactor(double& annuity) const {

    double df = 1.0 / (1.0 + rate_ * yearFraction_);
    annuity += df * yearFraction_;

    return df;
}
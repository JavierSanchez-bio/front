#ifndef LEG_H
#define LEG_H

#include <vector>
#include <cstddef>
#include <memory>

#include "Flows/CashFlow.h"       
#include "Curves/ZeroCouponCurve.h"
#include "Indices/Index.h"        

class Leg {
protected:
    std::vector<Flows::CashFlow> cashflows_;

public:
    Leg() = default;
    explicit Leg(const std::vector<Flows::CashFlow>& cfs)
        : cashflows_(cfs) {}

    virtual ~Leg() = default;

    void addCashFlow(const Flows::CashFlow& cf)
    {
        cashflows_.push_back(cf);
    }

    std::size_t size() const { return cashflows_.size(); }

    virtual double price(const ZeroCouponCurve& curve) const
    {
        double pv = 0.0;
        for (const auto& cf : cashflows_) {
            double t  = cf.getYearFraction();
            
            
            double df = curve.get_dcf(t); 
            
            pv += cf.getAmount() * df;
        }
        return pv;
    }

    const std::vector<Flows::CashFlow>& getCashFlows() const {
        return cashflows_;
    }
};

class FixedLeg : public Leg {
public:
    using Leg::Leg; 
};

class FloatingLeg : public Leg {
private:
    std::shared_ptr<Index> index_;
    double spread_;

public:
    FloatingLeg(const std::vector<Flows::CashFlow>& cfs, std::shared_ptr<Index> index, double spread = 0.0)
        : Leg(cfs), index_(std::move(index)), spread_(spread) {}

    double price(const ZeroCouponCurve& curve) const override
    {
        double pv = 0.0;
        
        
        for (std::size_t i = 0; i < cashflows_.size(); ++i) {
            
            const auto& cf = cashflows_[i]; 
            
            double rate = index_->get_fixing(i) + spread_;
            
            double t = cf.getYearFraction();
            double t_prev = (i == 0) ? 0.0 : cashflows_[i - 1].getYearFraction();
            double dt = t - t_prev;
            
            double expected_payment = cf.getAmount() * rate * dt;
            
            double df = curve.get_dcf(t);
            pv += expected_payment * df;
        }
        return pv;
    }
};

#endif
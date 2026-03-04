#ifndef LEG_H
#define LEG_H

#include <vector>
#include <cstddef>

#include <Flows/CashFlow.h>
#include <Instruments/ZeroCouponCurve.h>

// Representa una pata genérica de un instrumento (lista de flujos).
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

    // Valor presente de la pata utilizando la curva de tipos cero cupón.
    virtual double price(const ZeroCouponCurve& curve) const
    {
        double pv = 0.0;
        for (const auto& cf : cashflows_) {
            double t  = cf.getYearFraction();
            double df = curve.df(t);
            pv += cf.getAmount() * df;
        }
        return pv;
    }
};

class FixedLeg : public Leg {
public:
    using Leg::Leg; // hereda constructores de Leg
};

class FloatingLeg : public Leg {
public:
    using Leg::Leg;
};

#endif // LEG_H



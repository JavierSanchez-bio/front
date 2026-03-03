#ifndef CASH_FLOW_H
#define CASH_FLOW_H

namespace Flows {
    class CashFlow {
    private:
        double amount;
        double yearFraction;

    public:
        CashFlow(double amt, double yf) : amount(amt), yearFraction(yf) {}
        double presentValue(double rate) const;
        double getAmount() const { return amount; }
    };
}

#endif
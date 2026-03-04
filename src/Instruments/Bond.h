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

    // Calcula el precio teórico usando la curva de descuento
    double price() const override;

    // Calcula la TIR (Yield) dado un precio de mercado objetivo.
    double yield(double target_price) const;

    // Sobrecarga: Calcula la TIR basada en el precio teórico actual del bono.
    double yield() const;
};

#endif // BOND_H
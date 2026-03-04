#ifndef DEPOSIT_H
#define DEPOSIT_H

#include "Instruments.h" // La clase base
#include <memory>

class Deposit : public Instrument {
private:
    double rate_;         // Tipo de interés del depósito (ej. 0.05 para 5%)
    double yearFraction_; // Fracción de año calculada con tu DayCount (dt)
    double notional_;     // Nocional (100.0 por defecto)

public:
    // Constructor
    Deposit(std::shared_ptr<ZeroCouponCurve> curve, 
            double rate, 
            double yearFraction, 
            double notional = 100.0);

    ~Deposit() override = default;

    // Calcula el NPV del depósito usando la curva de descuento
    double price() const override;

    // Extrae el factor de descuento IMPLÍCITO según las fórmulas del Tema 3.
    // Esto será vital para el Bootstrapping.
    double impliedDiscountFactor() const;
    double computeDiscountFactor(double& annuity) const override;
    
    // Getters
    double getRate() const { return rate_; }
    double getYearFraction() const { return yearFraction_; }
};

#endif // DEPOSIT_H
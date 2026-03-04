#include "Deposit.h"
#include "ZeroCouponCurve.h"

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

    // Un depósito consiste en entregar un Nocional hoy (-N) 
    // y recibir el Nocional + Intereses a vencimiento (N * (1 + R * dt))
    
    // Pedimos a la curva el factor de descuento para el vencimiento
    double df = discount_curve->get_dcf(yearFraction_); 
    
    double future_cashflow = notional_ * (1.0 + rate_ * yearFraction_);
    
    // NPV = Valor presente del flujo futuro menos la inversión inicial
    return future_cashflow * df - notional_;
}

double Deposit::impliedDiscountFactor() const
{
    // Ecuación matemática del PDF para sacar el factor de descuento a partir del depósito
    // P(t, T) = 1 / (1 + R * dt)
    return 1.0 / (1.0 + rate_ * yearFraction_);
}

double Deposit::computeDiscountFactor(double& annuity) const {
    // Fórmula del Depósito: DF = 1 / (1 + Rate * dt)
    double df = 1.0 / (1.0 + rate_ * yearFraction_);
    
    // El depósito suma la primera parte de la anualidad (DF * dt) para el Swap a 12m
    annuity += df * yearFraction_;
    return df;
}
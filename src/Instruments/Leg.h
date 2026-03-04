#ifndef LEG_H
#define LEG_H

#include <vector>
#include <cstddef>
#include <memory>

#include <Flows/CashFlow.h>
#include <Instruments/ZeroCouponCurve.h>
#include "Index.h" // Necesario para la FloatingLeg

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

    // Valor presente de la pata genérica (ideal para FixedLeg).
    virtual double price(const ZeroCouponCurve& curve) const
    {
        double pv = 0.0;
        for (const auto& cf : cashflows_) {
            double t  = cf.getYearFraction();
            
            // CORREGIDO: Usamos get_dcf() para coincidir con tu ZeroCouponCurve
            double df = curve.get_dcf(t); 
            
            pv += cf.getAmount() * df;
        }
        return pv;
    }
};

// Pata Fija: Hereda todo tal cual. 
// Asume que cf.getAmount() ya es el importe fijo a pagar.
class FixedLeg : public Leg {
public:
    using Leg::Leg; 
};

// Pata Flotante: Calcula sus flujos dinámicamente basándose en un índice.
class FloatingLeg : public Leg {
private:
    std::shared_ptr<Index> index_;
    double spread_; // Margen sobre el índice (ej: Euribor + 0.01)

public:
    // Constructor: Recibe los flujos (que guardarán el Nocional), el índice y el spread.
    FloatingLeg(const std::vector<Flows::CashFlow>& cfs, std::shared_ptr<Index> index, double spread = 0.0)
        : Leg(cfs), index_(std::move(index)), spread_(spread) {}

    // Sobreescribimos el cálculo del precio
    double price(const ZeroCouponCurve& curve) const override
    {
        double pv = 0.0;
        
        // Empezamos en i=1 porque el forward_cc de tu ZeroCurve lanza error si i=0.
        // iteramos hasta el tamaño de los flujos.
        for (std::size_t i = 1; i <= cashflows_.size(); ++i) {
            
            // Ajustamos a i-1 para acceder al vector correctamente (que empieza en 0)
            const auto& cf = cashflows_[i - 1]; 
            
            // 1. Calculamos la tasa del índice para este periodo
            double rate = index_->get_fixing(i) + spread_;
            
            // 2. Para la pata flotante, asumimos que getAmount() guarda el Principal/Nocional.
            // Pago esperado = Nocional * Tasa * Fracción de año (dt)
            double expected_payment = cf.getAmount() * rate * cf.getYearFraction();
            
            // 3. Descontamos el pago al presente usando la curva
            double t = cf.getYearFraction();
            double df = curve.get_dcf(t);
            
            pv += expected_payment * df;
        }
        return pv;
    }
};

#endif // LEG_H
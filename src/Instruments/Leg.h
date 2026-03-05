#ifndef LEG_H
#define LEG_H

#include <vector>
#include <cstddef>
#include <memory>

// Cambia tus includes actuales por estos:
#include "Flows/CashFlow.h"       // Dependencia de Nivel 0
#include "Curves/ZeroCouponCurve.h" // Dependencia de Nivel 0
#include "Indices/Index.h"        // Dependencia de Nivel 1

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
            
            
            double df = curve.get_dcf(t); 
            
            pv += cf.getAmount() * df;
        }
        return pv;
    }

    const std::vector<Flows::CashFlow>& getCashFlows() const {
        return cashflows_;
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
    double spread_; // (ej: Euribor + 0.01)

public:
    // Constructor: Recibe los flujos (que guardarán el Nocional), el índice y el spread.
    FloatingLeg(const std::vector<Flows::CashFlow>& cfs, std::shared_ptr<Index> index, double spread = 0.0)
        : Leg(cfs), index_(std::move(index)), spread_(spread) {}

    // Sobreescribimos el cálculo del precio
    double price(const ZeroCouponCurve& curve) const override
    {
        double pv = 0.0;
        
        
        for (std::size_t i = 0; i < cashflows_.size(); ++i) {
            
            const auto& cf = cashflows_[i]; 
            
            // 1. Calculamos la tasa del índice para este periodo (0, 1, 2, 3)
            double rate = index_->get_fixing(i) + spread_;
            
            // 2. Calculamos el dt (fracción de año del periodo)
            double t = cf.getYearFraction();
            double t_prev = (i == 0) ? 0.0 : cashflows_[i - 1].getYearFraction();
            double dt = t - t_prev;
            
            // 3. Pago esperado = Nocional * Tasa * Fracción de año (dt)
            double expected_payment = cf.getAmount() * rate * dt;
            
            // 4. Descontamos
            double df = curve.get_dcf(t);
            pv += expected_payment * df;
        }
        return pv;
    }
};

#endif // LEG_H
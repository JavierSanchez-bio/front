#ifndef BOOTSTRAPPER_H
#define BOOTSTRAPPER_H

#include "Instruments.h" // La clase base Instrument
#include "ZeroCouponCurve.h"
#include <vector>
#include <memory>
#include <utility>

class Bootstrapper {
private:
    std::shared_ptr<ZeroCouponCurve> curve_;
    
    // Almacenamos pares de <Vencimiento (años), Instrumento>
    std::vector<std::pair<double, std::shared_ptr<Instrument>>> instruments_;

public:
    explicit Bootstrapper(std::shared_ptr<ZeroCouponCurve> curve);

    // Añade un instrumento al motor de calibración indicando cuándo vence
    void add_instrument(double maturity_t, std::shared_ptr<Instrument> instrument);

    // Ejecuta el algoritmo de calibración y rellena la curva
    void build_curve();
};

#endif // BOOTSTRAPPER_H
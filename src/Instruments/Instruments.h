#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <memory>

// "Forward declaration": Le decimos al compilador que esta clase existe 
// para poder usarla como puntero, sin tener que incluir todo su código aquí.
class ZeroCouponCurve; 

class Instrument {
protected:
    // Según tu diagrama, Instrument se asocia con una curva de descuento.
    // Usamos shared_ptr por seguridad en la gestión de memoria en C++.
    std::shared_ptr<ZeroCouponCurve> discount_curve;

public:
    // Constructor
    Instrument(std::shared_ptr<ZeroCouponCurve> curve);

    // ¡MUY IMPORTANTE! Destructor virtual. 
    // Es obligatorio en C++ cuando usas herencia para evitar fugas de memoria.
    virtual ~Instrument() = default;

    // Método virtual puro (el "= 0" al final).
    // Esto hace que la clase sea abstracta. Nadie puede hacer un `new Instrument()`.
    // Obliga a Bond y Swap a escribir su propia versión de price().
    virtual double price() const = 0; 

    // Añadir en Instruments.h dentro de public:
    // Pasamos annuity por REFERENCIA (&) para que cada instrumento sume su parte
    virtual double computeDiscountFactor(double& annuity) const { 
        return 0.0; 
    }
};

#endif // INSTRUMENT_H
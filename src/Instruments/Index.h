#ifndef INDEX_H
#define INDEX_H

#include <memory>
#include "ZeroCouponCurve.h"

class Index {
private:
    // La curva "forward" que el índice usa para predecir las tasas
    std::shared_ptr<ZeroCouponCurve> forward_curve_;

public:
    // Constructor
    explicit Index(std::shared_ptr<ZeroCouponCurve> curve);

    // Obtiene la tasa del índice para el periodo 'i'. 
    // Usamos el m=2.0 por defecto (semianual) como tenías en tu curva.
    double get_fixing(std::size_t i, double m = 2.0) const;
};

#endif // INDEX_H
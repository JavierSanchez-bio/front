#include "Bond.h"

Bond::Bond(std::shared_ptr<ZeroCouponCurve> curve, const Leg& leg)
    : Instrument(std::move(curve)), leg_(leg)
{
}

double Bond::price() const
{
    // El precio del bono es simplemente el valor presente (PV) de su pata.
    // Usamos el puntero 'discount_curve' que heredamos de la clase Instrument
    // y lo desreferenciamos con '*' porque el método price de Leg pide una referencia.
    
    if (discount_curve) {
        return leg_.price(*discount_curve);
    }
    
    return 0.0; // Por seguridad, si no hay curva, devolvemos 0
}
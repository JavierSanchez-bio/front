#ifndef ZERO_COUPON_CURVE_H
#define ZERO_COUPON_CURVE_H

#include <vector>
#include <cstddef>

// Curva de tipos cero cupón continuamente compuestos.
// times (años) y zc_rates (cupones cero)
class ZeroCouponCurve {
public:
    ZeroCouponCurve(const std::vector<double>& times,
                    const std::vector<double>& zc_rates);

    //Zero cupon
    double get_zc(double t) const;
    
    // Factor de descuento
    double get_dcf(double t) const;

    // Tipo forward continuamente compuesto
    double forward_cc(std::size_t i) const;

    // Lo trasladamos a semianual
    double forward_simple(std::size_t i, double m = 2.0) const;

private:
    std::vector<double> times_;
    std::vector<double> zc_;
};

#endif // ZERO_COUPON_CURVE_H


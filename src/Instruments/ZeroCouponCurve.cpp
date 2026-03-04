#include "ZeroCouponCurve.h"

#include <cmath>
#include <stdexcept>

ZeroCouponCurve::ZeroCouponCurve(const std::vector<double>& times,
                                 const std::vector<double>& zc_rates)
    : times_(times), zc_(zc_rates)
{
    if (times_.size() != zc_.size() || times_.empty()) {
        throw std::invalid_argument("ZeroCouponCurve: tamaños inconsistentes o vacíos");
    }
}

void ZeroCouponCurve::add_pillar(double t, double zc_rate)
{
    // Buscamos la posición donde debería ir este tiempo para mantener el vector ordenado
    auto it = std::lower_bound(times_.begin(), times_.end(), t);
    auto index = std::distance(times_.begin(), it);

    // Si ya existe un pilar en ese mismo instante de tiempo (con un pequeño margen de error), lo actualizamos
    if (it != times_.end() && std::abs(*it - t) < 1e-10) {
        zc_[index] = zc_rate;
    } else {
        // Si no existe, insertamos el tiempo y el tipo cero en su posición
        times_.insert(it, t);
        zc_.insert(zc_.begin() + index, zc_rate);
    }
}

double ZeroCouponCurve::get_zc(double t) const
{
    if (times_.empty()) {
        throw std::out_of_range("ZeroCouponCurve::get_zc: La curva esta vacia");
    }

    if (t <= times_.front()) {
        return zc_.front();
    }
    
    if (t >= times_.back()) {
        return zc_.back();
    }

    for (std::size_t i = 1; i < times_.size(); ++i) {
        if (t <= times_[i]) {
            double t0 = times_[i - 1];
            double t1 = times_[i];
            double zc0 = zc_[i - 1];
            double zc1 = zc_[i];

            double weight = (t - t0) / (t1 - t0);
            return zc0 + weight * (zc1 - zc0);
        }
    }
    
    return 0.0;
}

double ZeroCouponCurve::get_dcf(double t) const
{
    double rate = get_zc(t); 
    return std::exp(-rate * t);
}

double ZeroCouponCurve::forward_cc(std::size_t i) const
{

    if (i == 0) {
        return zc_[0];
    }

    double Ti   = times_[i];
    double Ti_1 = times_[i - 1];
    double ZCi   = zc_[i];
    double ZCi_1 = zc_[i - 1];

    return (ZCi * Ti - ZCi_1 * Ti_1) / (Ti - Ti_1);
}

double ZeroCouponCurve::forward_simple(std::size_t i, double m) const
{
    double Rc = forward_cc(i);
    return m * (std::exp(Rc / m) - 1.0);
}


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

double ZeroCouponCurve::get_zc(double t) const
{
    for (std::size_t i = 0; i < times_.size(); ++i) {
        if (std::abs(times_[i] - t) < 1e-10) {
            return zc_[i];
        }
    }
    throw std::out_of_range("ZeroCouponCurve::get_zc: vencimiento no encontrado");
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


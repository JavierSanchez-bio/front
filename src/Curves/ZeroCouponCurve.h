#ifndef ZERO_COUPON_CURVE_H
#define ZERO_COUPON_CURVE_H

#include <vector>
#include <cstddef>

class ZeroCouponCurve {
public:
    ZeroCouponCurve() = default;

    ZeroCouponCurve(const std::vector<double>& times,
                    const std::vector<double>& zc_rates);

    void add_pillar(double t, double zc_rate);

    double get_zc(double t) const;
    
    double get_dcf(double t) const;

    double forward_cc(std::size_t i) const;

    double forward_simple(std::size_t i, double m = 2.0) const;

private:
    std::vector<double> times_;
    std::vector<double> zc_;
};

#endif
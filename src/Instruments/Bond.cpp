#include "Bond.h"
#include "../Maths/NewtonRaphson.h"

#include <cmath>
#include <functional>
#include <stdexcept>

Bond::Bond(std::shared_ptr<ZeroCouponCurve> curve, const Leg& leg)
    : Instrument(std::move(curve)), leg_(leg)
{
}

double Bond::price() const
{
    if (discount_curve) {
        return leg_.price(*discount_curve);
    }
    return 0.0;
}

double Bond::yield() const
{
    return yield(this->price());
}

double Bond::yield(double target_price) const
{
    const auto& cfs = leg_.getCashFlows();
    
    if (cfs.empty()) {
        throw std::logic_error("Bond::yield: No se puede calcular la TIR de un bono sin flujos.");
    }

    std::function<double(double)> f = [&cfs, target_price](double y) {
        double pv = 0.0;
        for (const auto& cf : cfs) {
            double t = cf.getYearFraction();
            pv += cf.getAmount() * std::exp(-y * t);
        }
        return pv - target_price;
    };

    std::function<double(double)> df = [&cfs](double y) {
        double dpv = 0.0;
        for (const auto& cf : cfs) {
            double t = cf.getYearFraction();
            dpv += cf.getAmount() * (-t) * std::exp(-y * t);
        }
        return dpv;
    };

    double initial_guess = 0.05;

    return Maths::newtonRaphson(f, df, initial_guess);
}
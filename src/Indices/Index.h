#ifndef INDEX_H
#define INDEX_H

#include <memory>
#include "Curves/ZeroCouponCurve.h"

class Index {
private:
    std::shared_ptr<ZeroCouponCurve> forward_curve_;

public:
    explicit Index(std::shared_ptr<ZeroCouponCurve> curve);
    double get_fixing(std::size_t i, double m = 2.0) const;
};

#endif
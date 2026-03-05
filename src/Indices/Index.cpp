#include "Index.h"

Index::Index(std::shared_ptr<ZeroCouponCurve> curve)
    : forward_curve_(std::move(curve))
{
}

double Index::get_fixing(std::size_t i, double m) const
{
    if (forward_curve_) {
        return forward_curve_->forward_simple(i, m);
    }
    return 0.0;
}
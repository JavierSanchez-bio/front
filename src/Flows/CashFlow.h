#include "CashFlow.h"
#include <cmath>

namespace Flows {
    double CashFlow::presentValue(double rate) const {
        return amount / std::pow(1.0 + rate, yearFraction);
    }
}
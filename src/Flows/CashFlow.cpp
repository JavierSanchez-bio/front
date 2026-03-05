#include "CashFlow.h"
#include <cmath>

namespace Flows {
    double CashFlow::presentValue(double rate) const {
        return amount * std::exp(-rate * yearFraction);
    }
}
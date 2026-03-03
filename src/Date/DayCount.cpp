#include "DayCount.h"

namespace Date {
    double DayCount::yearFraction(double days, Convention convention) {
        switch (convention) {
            case Convention::ACT_360:
                return days / 360.0;
            case Convention::THIRTY_360:
                return days / 360.0; // Aquí implementarás las reglas 30/360 del PDF
            default:
                return days / 365.0;
        }
    }
}
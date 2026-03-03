#ifndef DAY_COUNT_H
#define DAY_COUNT_H

namespace Date {
    enum class Convention { ACT_360, THIRTY_360 };

    class DayCount {
    public:
        static double yearFraction(double days, Convention convention);
    };
}

#endif
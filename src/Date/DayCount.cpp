#include "DayCount.h"
#include <algorithm>

short Actual_360::compute_daycount(const std::string& from, const std::string& to)
{
    // parse from string + build date class
    boost::gregorian::date from_date(boost::gregorian::from_string(from));
    boost::gregorian::date to_date(boost::gregorian::from_string(to));
    return compute_daycount(from_date, to_date);
}

short Actual_360::compute_daycount(const boost::gregorian::date & from,
                                   const boost::gregorian::date & to)
{
    return (to - from).days();
}

short Thirty_360::compute_daycount(const std::string& from, const std::string& to)
{
    // parse from string + build date class + get a structure date structure.
    auto from_date(boost::gregorian::date(boost::gregorian::from_string(from)));
    auto to_date(boost::gregorian::date(boost::gregorian::from_string(to)));
    return compute_daycount(from_date, to_date);
}

short Thirty_360::compute_daycount(const boost::gregorian::date & from,
                                   const boost::gregorian::date & to)
{
    auto from_date(from.year_month_day());
    auto to_date(to.year_month_day());
    auto years = to_date.year - from_date.year;
    auto months = to_date.month - from_date.month;
    return compute_daycount(years, months, from_date.day, to_date.day);
}

short Thirty_360::compute_daycount(const short years,
                                   const short months,
                                   const short days_from,
                                   const short days_to)
{
    return (360 * years) + 30 * (months -1) +
    std::max<short>(0, 30 - days_from) +
    std::min<short>(30, days_to);
}
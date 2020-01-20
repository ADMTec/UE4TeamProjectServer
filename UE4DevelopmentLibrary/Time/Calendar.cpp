#include "Calendar.hpp"
#include <array>


std::string Calendar::DateTime(const Clock& clock)
{
    return Calendar::ToString(clock.to_time_t(), Calendar::Format::date_time);
}

std::string Calendar::Date(const Clock& clock)
{
    return Calendar::ToString(clock.to_time_t(), Calendar::Format::date);
}

std::string Calendar::Today(Calendar::Format::type format)
{
    return std::string();
}

std::string Calendar::GetDay(const Clock& clock, int32_t relative_offset)
{
    return Calendar::ToString(
        clock.to_time_t() + (60 * 60 * 24 * relative_offset), Calendar::Format::date);
}

Calendar::DayOfTheWeek Calendar::DayOfWeek(const Clock& clock)
{
    tm time;
    time_t tick = clock.to_time_t();
    localtime_s(&time, &tick);
    return (DayOfTheWeek)time.tm_wday;
}

std::string Calendar::ToString(time_t tick, Calendar::Format::type format)
{
    std::array<char, 128> str;

    tm time;
    localtime_s(&time, &tick);
    strftime(str.data(), str.size(), format, &time);
    return str.data();
}

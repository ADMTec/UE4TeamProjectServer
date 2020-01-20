#pragma once
#include "Clock.hpp"
#include <string>

class Calendar
{
public:
	struct Format
	{
		using type = const char*;
		constexpr static const type date_time = "%Y-%m-%d %H:%M:%S";
		constexpr static const type date = "%Y-%m-%d";
		constexpr static const type time = "%H:%M:%S";
	};
	enum DayOfTheWeek : size_t
	{
		SUNDAY = 0,
		MONDAY = 1,
		TUESDAY = 2,
		WEDNESDAY = 3,
		THURSDAY = 4,
		FRIDAY = 5,
		SATURDAY = 6,
		count
	};
public:
	static std::string DateTime(const Clock& clock);
	static std::string Date(const Clock& clock);
	static std::string Today(Calendar::Format::type format);
	static std::string GetDay(const Clock& clock, int32_t relative_offset);
	static Calendar::DayOfTheWeek DayOfWeek(const Clock& clock);
private:
	static std::string ToString(time_t tick, Calendar::Format::type format);
};

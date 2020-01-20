#include "Clock.hpp"

Clock::Clock(const Clock& rhs)
{
    Clock::operator=(rhs);
}

void Clock::operator=(const Clock& rhs)
{
    time_point_ = rhs.time_point_;
}

Clock::Clock(Clock&& rhs) noexcept
{
    Clock::operator=(std::move(rhs));
}

void Clock::operator=(Clock&& rhs) noexcept
{
    time_point_ = std::move(rhs.time_point_);
}

Clock::Clock(uint64_t after_millisec)
{
    SetTime(after_millisec);
}

Clock::Clock(time_t time)
{
    SetTime(time);
}

Clock::Clock(const std::chrono::system_clock::time_point& time_point)
    : time_point_(time_point)
{
}

Clock::Clock()
{
    Update();
}

Clock::~Clock()
{
}

void Clock::Update()
{
    time_point_ = std::chrono::system_clock::now();
}

void Clock::SetTime(time_t time)
{
    time_point_ = std::chrono::system_clock::from_time_t(time);
}

void Clock::SetTime(uint64_t after_millisec)
{
    time_point_ = std::chrono::system_clock::now() + std::chrono::milliseconds(after_millisec);
}

void Clock::SetTime(const std::chrono::system_clock::time_point& time_point)
{
    time_point_ = time_point;
}

int64_t Clock::operator-(const Clock& other) const
{
    return std::chrono::duration_cast<
        std::chrono::milliseconds>(time_point_ - other.time_point_).count();
}

int64_t Clock::to_int64_t() const
{
    return std::chrono::duration_cast<
        std::chrono::milliseconds>(time_point_.time_since_epoch()).count();
}

time_t Clock::to_time_t() const
{
    return std::chrono::system_clock::to_time_t(time_point_);
}
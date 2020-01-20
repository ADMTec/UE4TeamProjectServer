#pragma once
#include <chrono>
#include <cstdint>

class Clock
{
public:
    Clock(const Clock& rhs);
    void operator=(const Clock& rhs);
    Clock(Clock&& rhs) noexcept;
    void operator=(Clock&& rhs) noexcept;
public:
    explicit Clock(time_t time);
    explicit Clock(uint64_t after_millisec);
    explicit Clock(const std::chrono::system_clock::time_point& time_point);
    Clock();
    ~Clock();

    void Update();
    void SetTime(time_t time);
    void SetTime(uint64_t after_millisec);
    void SetTime(const std::chrono::system_clock::time_point& time_point);

    int64_t operator-(const Clock& other) const;

    int64_t to_int64_t() const;
    time_t to_time_t() const;
private:
    std::chrono::system_clock::time_point time_point_;
};
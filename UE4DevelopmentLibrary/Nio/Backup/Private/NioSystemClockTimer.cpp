#include "NioSystemClockTimer.hpp"
#include "NioContext.hpp"
#include <iostream>
#include <sstream>


NioSystemClockTimer::NioSystemClockTimer(NioSystemClockTimer&& rhs) noexcept
    : timer_(std::move(rhs.timer_))
{
}

void NioSystemClockTimer::operator=(NioSystemClockTimer&& rhs) noexcept
{
    timer_ = std::move(rhs.timer_);
}

NioSystemClockTimer::NioSystemClockTimer(NioContext& nio_context)
    : timer_(nio_context.GetContext())
{
}

void NioSystemClockTimer::SetTimer(const std::function<void(void)>& func, std::chrono::milliseconds milli) {
    timer_.expires_from_now(milli);
    timer_.async_wait(
        [func = func](const boost::system::error_code& ec) {
        if (ec) {
            std::stringstream ss;
            ss << "NioSystemClockTimer Error[" << ec.value() << ":" << ec.message() << "]\n";
            std::cout << ss.str();
        } else {
            if (func) {
                func();
            }
        }
    });
}

void NioSystemClockTimer::Cancel()
{
    timer_.cancel();
}
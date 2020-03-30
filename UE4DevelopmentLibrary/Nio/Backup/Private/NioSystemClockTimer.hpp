#pragma once
#include "../NioBase.hpp"
#include "Time.hpp"
#include <functional>
#include <boost/asio.hpp>


class NioSystemClockTimer
{
public:
    NioSystemClockTimer(const NioSystemClockTimer&) = delete;
    void operator=(const NioSystemClockTimer&) = delete;
    NioSystemClockTimer(NioSystemClockTimer&& rhs) noexcept;
    void operator=(NioSystemClockTimer&& rhs) noexcept;

    NioSystemClockTimer(class NioContext& nio_context);
    void SetTimer(const std::function<void(void)>& func, std::chrono::milliseconds milli);
    void Cancel();
private:
    boost::asio::system_timer timer_;
};
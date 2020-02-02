#include "NioContext.hpp"


NioContext::NioContext()
    : guard_(boost::asio::make_work_guard(ioc_)), is_running_(false)
{
}

NioContext::~NioContext()
{
    Stop();
}

NioContext::NioContext(int concurrency_hint)
    : ioc_(concurrency_hint), guard_(boost::asio::make_work_guard(ioc_)), is_running_(false)
{
}

void NioContext::Run()
{
    bool expected{ false };
    if (is_running_.compare_exchange_strong(expected, true)) {
        ioc_.run();
    }
}

void NioContext::Stop()
{
    bool expected{ true };
    if (is_running_.compare_exchange_strong(expected, false)) {
        ioc_.stop();
    }
}

boost::asio::io_context& NioContext::GetContext()
{
    return ioc_;
}
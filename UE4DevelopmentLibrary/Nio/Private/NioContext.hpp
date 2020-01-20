#pragma once
#include "../NioBase.hpp"
#include "boost/asio.hpp"


class NioContext
{
public:
    NioContext();
    ~NioContext();
    explicit NioContext(int concurrency_hint);

    void Run();
    void Stop();
    boost::asio::io_context& GetContext();
private:
    std::atomic<bool> is_running_;
    boost::asio::io_context ioc_;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> guard_;
};
#pragma once
#include "NioBase.hpp"
#include <cstdint>
#include <atomic>
#include <memory>
#include <vector>
#include <thread>
#include <concurrent_queue.h>
#include <shared_mutex>
#include <sstream>

template<class NioSession>
class TNioServer
{
public:
    TNioServer(int32_t max_connection, uint16_t port, uint16_t num_thread)
        : max_connection_(max_connection), port_(port), num_thread_(num_thread)
        , acceptor_(ioc_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
    {
    }

    void Initialize()
    {
        std::unique_lock lock(session_pool_gaurd_);
        for (uint32_t i = 0; i < max_connection_; ++i)
        {
            session_id_queue_.push(i);
            session_pool_.emplace_back(std::make_shared<NioSession>(i, ioc_));
        }
    }

    void Run()
    {
        thread_group_.reserve(num_thread_);
        for (size_t i = 0; i < thread_group_.size(); ++i) {
            thread_group_.emplace_back(
                std::thread(
                    [](asio::io_context& ioc) {
                        try {
                            ioc.run();
                        } catch (const std::exception & e) {
                            std::stringstream ss;
                            ss << "io thread exit with unhandled exception - ";
                            ss << e.what();
                            NioSession::Handler::OnError(-1, ss.str().c_str());
                        }
                    }, std::ref(ioc_))
            );
        }
        bool expected{ false };
        if (is_running_.compare_exchange_strong(expected, true)) {
            Accept();
        }
    }

    void Accept()
    {
        int64_t id = -1;
        std::shared_ptr<asio::ip::tcp::socket> sock = std::shared_ptr<asio::ip::tcp::socket>(
            new asio::ip::tcp::socket(ioc_),
            [id, this](asio::ip::tcp::socket* ptr) {
                auto session = this->GetSession(id);
                if (session) {
                    session->OnClose();
                    delete ptr;
                    this->ReleaseSession(id);
                }
            }
        );
        std::shared_lock lock(session_pool_gaurd_);
        acceptor_.async_accept(
            *sock,
            [session = session_pool_[id], sock = sock, this](const error_code& ec) {
                if (!ec) {
                    session->SetSocket(sock);
                    session->OnConnect();
                } else {
                    session->OnError(ec);
                }
                this->Accept();
            }
        );
    }
    
    void Clear()
    {
        std::shared_lock lock(session_pool_gaurd_);
        for (auto& element : session_pool_)
        {
            if (element) {
                element->Close();
            }
        }
    }

    void Stop()
    {
        bool expected{ true };
        if (is_running_.compare_exchange_strong(expected, false)) {
            Clear();
        }
    }

    void Exit()
    {
        ioc_.stop();
        for (size_t i = 0; i < thread_group_.size(); ++i) {
            thread_group_[i].join();
        }
        thread_group_.clear();
    }

    void ReleaseSession(int64_t id)
    {
        if (is_running_) {
            session_id_queue_.push(id);
            Accept();
        }
    }

    bool IsRunning() const
    {
        return is_running_;
    }

    std::shared_ptr<NioSession> GetSession(int id)
    {
        std::shared_lock lock(session_pool_gaurd_);
        return session_pool_[id];
    }
private:
    asio::io_context ioc_;
    asio::ip::tcp::acceptor acceptor_;
    std::atomic<bool> is_running_;
    uint32_t max_connection_;
    uint16_t port_;
    uint16_t num_thread_;

    std::vector<std::thread> thread_group_;

    mutable std::shared_mutex session_pool_gaurd_;
    std::vector<std::shared_ptr<NioSession>> session_pool_;
    concurrency::concurrent_queue<int64_t> session_id_queue_;
};
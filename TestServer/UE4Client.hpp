#pragma once
#include "UE4DevelopmentLibrary/Utility.hpp"
#include "UE4DevelopmentLibrary/Nio.hpp"


class UE4Client
{
public:
    UE4Client(const UE4Client&) = delete;
    void operator=(const UE4Client&) = delete;
    UE4Client(UE4Client&&) noexcept = delete;
    void operator=(UE4Client&&) noexcept = delete;

public:
    explicit UE4Client(const std::shared_ptr<NioSession>& session);

    void Close();
    const __UUID& GetUUID() const;
    std::shared_ptr<NioSession> GetSession() const;
    float x;
    float y;
private:
    __UUID uuid_;
    std::shared_ptr<NioSession> session_;
};


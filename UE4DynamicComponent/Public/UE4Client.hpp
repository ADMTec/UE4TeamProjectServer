#pragma once
#include <optional>
#include "UE4DevelopmentLibrary/Utility.hpp"
#include "UE4DevelopmentLibrary/Nio.hpp"
#include "Base.hpp"



UE4_DLLEXPORT class UE4_DLL_CLASS __UUID;
UE4_DLLEXPORT template class UE4_DLL_CLASS std::optional<__UUID>;
UE4_DLLEXPORT template class UE4_DLL_CLASS std::shared_ptr<NioSession>;

class UE4_DLL_CLASS UE4Client
{
private:
    UE4Client(const UE4Client&);
    void operator=(const UE4Client&);
public:
    explicit UE4Client(const std::shared_ptr<NioSession>& session);
    ~UE4Client();

    void Close();
    const __UUID& GetUUID() const;
    std::shared_ptr<NioSession> GetSession() const;

    void SetContextUUID(uint32_t key, const __UUID& uuid);
    std::optional<__UUID> GetContextUUID(uint32_t key) const;
private:
    class ImplUE4Client* impl_;
};
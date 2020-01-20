#include "TestServer.hpp"
#include "UE4DevelopmentLibrary/Exception.hpp"
#include "UE4DevelopmentLibrary/Time.hpp"
#include "IoHandler.hpp"
#include <sstream>


bool ClientStorage::Insert(const std::string& uuid, const std::shared_ptr<UE4Client>& client)
{
    std::unique_lock lock(storage_mutex_);
    auto result = storage_.insert(std::make_pair(uuid, client));
    return result.second;
}

size_t ClientStorage::Erase(const std::string& uuid)
{
    std::unique_lock lock(storage_mutex_);
    return storage_.erase(uuid);
}

std::shared_ptr<UE4Client> ClientStorage::Find(const std::string& uuid) const
{
    std::shared_lock lock(storage_mutex_);
    auto iter = storage_.find(uuid);
    if (iter != storage_.end()) {
        return iter->second;
    }
    return std::shared_ptr<UE4Client>();
}

std::vector<std::shared_ptr<UE4Client>> ClientStorage::GetCopy() const
{
    std::vector<std::shared_ptr<UE4Client>> vec;

    std::shared_lock lock(storage_mutex_);
    vec.reserve(storage_.size());
    for (const auto& element : storage_) {
        vec.emplace_back(element.second);
    }
    return vec;
}


TestServer::TestServer()
    : eixt_flag_(false)
{

}

TestServer::~TestServer()
{
    worker_.join();
}
#include <iostream>
concurrency::concurrent_queue<std::pair<std::shared_ptr<UE4Client>, std::shared_ptr<NioInPacket>>> g_queue;


void TestServer::Initialize()
{
    NioServerBuilder builder;
    builder.SetPort(12221)
        .SetMaxConnection(10)
        .SetNioThreadCount(2)
        .SetNioInternalBufferSize(2048)
        .SetNioPacketCipher(std::shared_ptr<NioCipher>(new PacketCipher()))
        .SetNioEventHandler(std::shared_ptr<NioEventHandler>(new IoHandler()));
    io_server_ = builder.Build();
    io_server_->Initialize();

    worker_ = std::thread(
        [this]() {
            while (eixt_flag_ == false)
            {
                try {
                    std::pair<std::shared_ptr<UE4Client>, std::shared_ptr<NioInPacket>> pair;
                    if (g_queue.try_pop(pair))
                    {
                        std::cout << "PacketDump: " << pair.second->GetDebugString() << std::endl;
                        int16_t opcode = pair.second->ReadInt16();
                        switch (opcode)
                        {
                            case 0: // RequestSpawn
                                this->HandleSpawn(*pair.first, *pair.second);
                                this->HandlePossess(*pair.first, *pair.second);
                                break;
                            case 1: // NotifyPosition
                                this->HandleNotifiyPosition(*pair.first, *pair.second);
                            case 2:
                                this->HandleBroadCast(*pair.first, *pair.second);
                                break;
                        }
                    }
                }
                catch (const std::exception & e) {
                    std::cout << e.what();
                }
            }
        });
}

void TestServer::Run()
{
    io_server_->Run();
}

void TestServer::Stop()
{
    io_server_->Stop();
    eixt_flag_.store(true);
}

void TestServer::ActiveClient(NioSession& session)
{
    std::shared_ptr<UE4Client> client = std::make_shared<UE4Client>(session.shared_from_this());
    const auto& uuid = client->GetUUID().ToString();
    session.SetClinetKey(uuid);
    storage_.Insert(uuid, client);
    //this->OnActiveClient(*client);
}

void TestServer::CloseClient(const std::string& client_key)
{
    std::shared_ptr<UE4Client> client = storage_.Find(client_key);
    if (client) {
        //this->OnCloseClient(*client);
        client->Close();
        storage_.Erase(client_key);
    }
}

void TestServer::ProcessPacket(NioSession& session, const std::shared_ptr<NioInPacket>& in_packet)
{
    std::shared_ptr<UE4Client> client = storage_.Find(session.GetClientKey());
    if (!client) {
        std::stringstream ss;
        ss << "ip[" << session.GetRemoteAddress() << "]'s clinet is nullptr";
        throw StackTraceException(ExceptionType::kNullPointer, ss.str().c_str());
    }
    //this->OnProcessPacket(*client, in_packet);
    g_queue.push(std::make_pair(client, in_packet));
}

void TestServer::BroadCastPacket(NioOutPacket& outpacket)
{
    for (const auto& stored_client : this->GetClientAll()) {
        if (stored_client) {
            stored_client->GetSession()->AsyncSend(outpacket, false, true);
        }
    }
}

void TestServer::BroadCastPacket(NioOutPacket& outpacket, UE4Client* exclude)
{
    for (const auto& stored_client : this->GetClientAll()) {
        if (stored_client && stored_client.get() != exclude) {
                stored_client->GetSession()->AsyncSend(outpacket, false, true);
        }
    }
}

std::shared_ptr<UE4Client> TestServer::GetClient(const std::string& uuid) const
{
    return storage_.Find(uuid);
}

std::vector<std::shared_ptr<UE4Client>> TestServer::GetClientAll() const
{
    return storage_.GetCopy();
}
#include <random>
void TestServer::HandleSpawn(UE4Client& client, NioInPacket& input)
{
    OutPacket out;

    std::random_device rnd;
    std::mt19937 mt(rand());
    std::uniform_int_distribution<int> dist(-500, 500);

    float x = dist(mt);
    float y = dist(mt);
    client.x = x;
    client.y = y;
    out.WriteInt16(0);
    out << client.GetUUID();
    out << x;
    out << y;
    out.MakePacketHead();
    BroadCastPacket(out);
}

void TestServer::HandlePossess(UE4Client& client, NioInPacket& input)
{
    OutPacket out;
    out.WriteInt16(1);
    out << client.GetUUID();
    out.MakePacketHead();
    client.GetSession()->AsyncSend(out, false, false);
}

void TestServer::HandleNotifiyPosition(UE4Client& client, NioInPacket& input)
{
    float rox, roy, roz;
    float z;
    input >> client.x;
    input >> client.y;
    input >> z;
    input >> rox;
    input >> roy;
    input >> roz;
    // Rotation

    OutPacket out;
    out.WriteInt16(2);
    out << client.GetUUID();
    out << client.x;
    out << client.y;
    out << z;
    out << rox;
    out << roy;
    out << roz;
    out.MakePacketHead();
    this->BroadCastPacket(out, &client);
}

void TestServer::HandleBroadCast(UE4Client& client, NioInPacket& input)
{
    int length = input.GetBufferLength();

    char* buffer = &input.GetData()[4];

    OutPacket out;
    out.WriteInt16(3);
    out << client.GetUUID();
    out.WriteBytes(buffer, length - 4);
    out.MakePacketHead();
    this->BroadCastPacket(out);
}

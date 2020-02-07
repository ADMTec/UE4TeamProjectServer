#include "ZoneServer.hpp"
#include "ServerConstants.hpp"
#include "UE4DevelopmentLibrary/Exception.hpp"
#include "UE4DevelopmentLibrary/Database.hpp"
#include "UE4DevelopmentLibrary/Server/UE4PacketCipher.hpp"
#include "UE4DevelopmentLibrary/Server/UE4OutPacket.hpp"
#include "UE4DevelopmentLibrary/Server/RemoteSessionInfo.hpp"
#include "UE4DevelopmentLibrary/Server/RemoteServerInfo.hpp"
#include "UE4DevelopmentLibrary/Server/UE4EventHandler.hpp"
#include "Model/Character/Character.hpp"
#include "Packet/PacketGenerator.hpp"
#include "../InterServerOpcode.hpp"
#include "../NetworkOpcode.h"
#include "GameConstants.hpp"
#include "System/ZoneSystem.hpp"
#include "System/MatchingSystem.hpp"
#include "System/PythonScriptEngine.hpp"
#include <iostream>
#include <sstream>


ZoneServer* ZoneServer::static_instance_ = nullptr;

ZoneServer::ZoneServer()
    : print_log_(true)
{
    static_instance_ = this;
}

ZoneServer::~ZoneServer()
{
}

void ZoneServer::Initialize()
{
    io_server_.emplace(ServerConstant.max_connection, ServerConstant.port, ServerConstant.num_io_worker_);
    io_server_->Initialize();
    session_ = std::make_shared<IntermediateSession>(io_server_->GetContext());

    IntermediateSession::Handler::BindFunction(
        static_cast<int16_t>(IntermediateServerSendPacket::kNotifySessionAuthorityInfo),
        [this](IntermediateSession& session, IntermediateSession::InPacket& input) {
            std::string lobby_server_uuid = input.ReadString();
            std::string user_uuid = input.ReadString();
            RemoteSessionInfo info;
            input >> info;

            {
                std::lock_guard lock(session_authority_guard_);
                authority_map_.emplace(info.GetId(), info);
            }
            UE4OutPacket out;
            out.WriteInt16(static_cast<int16_t>(
                IntermediateServerReceivePacket::kReactSessionAuthorityInfo));
            out << lobby_server_uuid;
            out << user_uuid;
            out.MakePacketHead();
            session.Send(out, false, false);

            //nio_server_->CreateTimer(
            //    [this, info]() {
            //        std::lock_guard lock(session_authority_guard_);
            //        auto iter = authority_map_.find(info.GetId());
            //        if (iter != authority_map_.end()) {
            //            if (iter->second.GetClock().to_int64_t() == info.GetClock().to_int64_t()) {
            //                //authority_map_.erase(iter);
            //            }
            //        }
            //    }, std::chrono::milliseconds(3000));
        }
    );
}

void ZoneServer::Run()
{
    if (io_server_) {
        io_server_->Run();
        ConnectChannel();
        std::cout << "Input Command\n\
[1] : Print ZoneServer Debug State\n\
[2] : Print MatchSystem Debug State\n\
[3] : Print ZoneSystem Debug State\n\
[q] : Server Stop\n";
        std::string line;
        while (true)
        {
            std::cin >> line;
            try {
                if (line._Equal("q")) {
                    break;
                }
                int command = std::atoi(line.c_str());
                switch (command)
                {
                    case 1:
                        std::cout << this->GetDebugString();
                        break;
                    case 2:
                        std::cout << MatchSystem::Instance().GetDebugString();
                        break;
                    case 3:
                        std::cout << ZoneSystem::GetDebugString();
                        break;
                    default:
                        std::cout << "find command fail...\n";
                }
            } catch (const std::exception & e) {
                std::cout << e.what() << std::endl;
            }
        }
    }
}

void ZoneServer::Stop()
{
    if (io_server_) {
        io_server_->Stop();
    } else {
        throw StackTraceException(ExceptionType::kNullPointer, "nio server is nullptr");
    }
}

void ZoneServer::ConnectChannel()
{
    if (session_ && session_->IsOpen() == false) {
        if (session_->SyncConnect(ServerConstant.intermediate_server_ip.c_str(), ServerConstant.intermediate_server_port) == false) {
            std::stringstream ss;
            Clock clock;
            ss << '[' << Calendar::DateTime(clock) << "] connect to intermediate server(" 
                << ServerConstant.intermediate_server_ip << ":" << ServerConstant.intermediate_server_port << ") fail \n";
            std::cout << ss.str();
        } else {
            session_->OnConnect();

            RemoteServerInfo info;
            info.SetServerType(ServerType::kZoneServer);
            info.SetIP(ServerConstant.ip);
            info.SetPort(ServerConstant.port);
            info.SetCurrentConnection(0);
            info.SetMaxConnection(ServerConstant.max_connection);

            UE4OutPacket out;
            out.WriteInt16(static_cast<int16_t>(IntermediateServerReceivePacket::kRegisterRemoteServer));
            out << info;
            out.MakePacketHead();
            session_->Send(out, false, false);
        }
    }
}

void ZoneServer::OnActive(Session& session)
{
    std::shared_ptr<Client> client = std::make_shared<Client>(session.shared_from_this());
    const auto& uuid = client->GetUUID();
    {
        std::unique_lock lock(client_storage_guard_);
        client->SetState(ClientState::kNotConfirm);
        client_storage_.emplace(client->GetUUID().ToString(), client);
        session.SetClientKey(client->GetUUID().ToString());
    }
    if (print_log_) {
        std::stringstream ss;
        Clock clock;
        ss << "[" << Calendar::DateTime(clock) << "] "
            << client->GetSession()->GetRemoteAddress() << " connect...\n";
        std::cout << ss.str();
    }
    //GetNioServer()->CreateTimer([uuid = client.GetUUID(), this]() {
    //    auto client = this->GetClient(uuid.ToString());
    //    if (client && client->GetState() == ClientState::kNotConfirm) {
    //        this->CloseClient(uuid.ToString());
    //    }
    //}, std::chrono::milliseconds(1000));
}
\
void ZoneServer::OnClose(Session& session)
{
    std::shared_ptr<Client> client = nullptr;
    {
        std::unique_lock lock(client_storage_guard_);
        auto iter = client_storage_.find(session.GetClientKey());
        if (iter != client_storage_.end()) {
            client = iter->second;
            client_storage_.erase(iter);
        }
    }
    if (client)
    {
        if (client->GetState() == ClientState::kConfirm) {
            auto chr = client->GetCharacter();
            if (chr) {
                std::shared_ptr<Zone> zone = nullptr;
                {
                    std::lock_guard chr_lock(chr->mutex_);
                    zone = chr->GetZoneFromWeak();
                }
                if (zone) {
                    zone->RemoveCharacter(chr->GetObjectId());
                }
                chr->SetZone(nullptr);
                client->SetCharacter(nullptr);
            }
            std::shared_ptr<Match> match = client->GetMatchFromWeak();
            if (match) {
                match->Remove(client->GetUUID().ToString());
            }
        } else if (client->GetState() == ClientState::kNotConfirm) {

        }
        if (print_log_) {
            std::stringstream ss;
            Clock clock;
            ss << "[" << Calendar::DateTime(clock) << "] "
                << client->GetSession()->GetRemoteAddress() << " close...\n";
            std::cout << ss.str();
        }
        /*UE4OutPacket out;
        out.WriteInt16(static_cast<int16_t>(IntermediateServerReceivePacket::kNotifyUserLogout));
        out.WriteInt32(client.GetAccid());
        out.MakePacketHead();
        GetNioServer()->GetChannel().GetConnection(intermediate_server).Send(out);*/
    }
}
void ZoneServer::OnError(int ec, const char* message)
{
    std::stringstream ss;
    ss << "OnError[" << ec << ":" << message << "]" << '\n';
    std::cout << ss.str();
}
void ZoneServer::ProcessPacket(Session& session, const std::shared_ptr<Session::InPacket>& in_packet)
{
    auto client = this->GetClient(session.GetClientKey());
    if (!client) {
        std::stringstream ss;
        ss << "ip[" << session.GetRemoteAddress() << "]'s clinet is nullptr";
        throw StackTraceException(ExceptionType::kNullPointer, ss.str().c_str());
    }
    if (print_log_) {
        std::stringstream ss;
        ss << "Packet Dump: " << in_packet->GetDebugString() << '\n';
        std::cout << ss.str();
    }
    in_packet->SetAccesOffset(2);
    try {
        int16_t opcode = in_packet->ReadInt16();
        switch (static_cast<ENetworkCSOpcode>(opcode)) {
            case ENetworkCSOpcode::kZoneConrifmRequest:
                HandleConfirmRequest(*client, *in_packet);
                break;
            case ENetworkCSOpcode::kNotifyCurrentChrPosition:
                HandleChrPositionNotify(*client, *in_packet);
                break;
            case ENetworkCSOpcode::kRequestCharacterRolling:
                HandleRequestRolling(*client, *in_packet);
                break;
            case ENetworkCSOpcode::kNotifyMonsterAction:
                HandleMonsterActionNotify(*client, *in_packet);
                break;
            case ENetworkCSOpcode::kMatchingRequest:
                HandleMatchingRequest(*client, *in_packet);
                break;
            case ENetworkCSOpcode::kMatchingConfirmRequest:
                HandleMatchingComfirmRequest(*client, *in_packet);
                break;
            case ENetworkCSOpcode::kCharacterAttack:
                HandleCharacterAttackMotion(*client, *in_packet);
                break;
            case ENetworkCSOpcode::kCharcterHitSuccess:
                HandleCharacterHitSuccess(*client, *in_packet);
                break;
            case ENetworkCSOpcode::kMonsterHitCharacter:
                HandleMonsterAttack(*client, *in_packet);
                break;
            case ENetworkCSOpcode::kRequestCharacterWait:
                HandleCharacterWait(*client, *in_packet);
                break;
            case ENetworkCSOpcode::kRequestCharacterSprint:
                HandleCharacterSprint(*client, *in_packet);
                break;
            case ENetworkCSOpcode::kRequestChangeZone:
                HandleZoneChangeRequest(*client, *in_packet);
                break;
            case ENetworkCSOpcode::kInventoryUpdateRequest:
                HandleInventoryUpdate(*client, *in_packet);
                break;
        }
    } catch (const std::exception & e) {
        std::stringstream ss;
        Clock clock;
        std::string date = Calendar::DateTime(clock);
        ss << "[" << date << "] Exception: " << e.what() << '\n';
        date = ss.str();
        std::cout << date;
        client->Close();
    }
}

std::shared_ptr<Client> ZoneServer::GetClient(const std::string& uuid) const
{
    std::shared_lock lock(client_storage_guard_);
    auto iter = client_storage_.find(uuid);
    if (iter != client_storage_.end()) {
        return iter->second;
    }
    return nullptr;
}

std::string ZoneServer::GetDebugString() const
{
    std::unique_lock lock(client_storage_guard_);
    std::stringstream ss;
    ss << "-----------------------------------------------" << '\n';
    ss << "-----------------------------------------------" << '\n';
    ss << " Debug State [ZoneServer]" << '\n';
    for (const auto& client : client_storage_) {
        auto session = client.second->GetSession();
        auto chr = client.second->GetCharacter();
        ss << " Connected Client[";
        if (session) {
            ss << session->GetRemoteAddress();
        } else {
            ss << "NULL";
        }
        ss << "] Character Name[";
        if (chr) {
            std::lock_guard lock(chr->mutex_);
            ss << chr->GetName();
        } else {
            ss << "NULL";
        }
        ss << "] UUID: " << client.second->GetUUID().ToString() << '\n';
    }
    ss << "-----------------------------------------------" << '\n';
    ss << "-----------------------------------------------" << '\n';
    return ss.str();
}

void ZoneServer::HandleConfirmRequest(Client& client, Session::InPacket& in_packet)
{
    std::string id = in_packet.ReadString();
    const auto& ip = client.GetSession()->GetRemoteAddress();
    {
        bool result{ false };
        RemoteSessionInfo info;
        {
            std::lock_guard lock(session_authority_guard_);
            auto iter = authority_map_.find(id);
            if (iter != authority_map_.end() && iter->second.GetIp() == ip) {
                result = true;
                info = iter->second;
                authority_map_.erase(iter);
            }
        }
        if (result) {
            client.SetState(ClientState::kConfirm);
            client.SetAccid(info.GetAccid());
            client.SetCid(info.GetCid());
        } else {
            client.Close();
            return;
        }
    }
    std::shared_ptr<Character> chr = std::make_shared<Character>(client.GetAccid(), client.GetCid());
    {
        std::lock_guard chr_lock(chr->mutex_);
        chr->Initialize(ODBCConnectionPool::Instance().GetConnection());
        client.SetCharacter(chr);
        chr->SetWeakClient(client.shared_from_this());

    }
    ZoneSystem::GetTown()->SpawnCharacter(chr);
}

void ZoneServer::HandleChrPositionNotify(Client& client, Session::InPacket& in_packet)
{
    if (client.GetState() != kConfirm) {
        return;
    }
    int state = in_packet.ReadInt32();
    Location lo; lo.Read(in_packet);
    Rotation ro; ro.Read(in_packet);
    int64_t oid = -1;
    auto chr = client.GetCharacter();
    if (chr) {
        std::shared_ptr<Zone> zone = nullptr;
        {
            std::lock_guard lock(chr->mutex_);
            oid = chr->GetObjectId();
            zone = chr->GetZoneFromWeak();
            chr->GetLocation() = lo;
            chr->GetRotation() = ro;
        }
        if (zone) {
            UE4OutPacket out(UE4OutPacket::AllocBufferLength::k64Bytes);
            PacketGenerator::CharacterLocation(out, *chr, state);
            out.MakePacketHead();
            zone->BroadCast(out, oid);
        }
    }
}

void ZoneServer::HandleMonsterActionNotify(Client& client, Session::InPacket& in_packet)
{
    if (client.GetState() != kConfirm) {
        return;
    }
    int64_t mob_id = in_packet.ReadInt64();
    int state = in_packet.ReadInt32();
    Location lo; lo.Read(in_packet);
    Rotation ro; ro.Read(in_packet);

    auto chr = client.GetCharacter();
    if (chr) {
        int64_t chr_oid = -1;
        std::shared_ptr<Zone> zone = nullptr;
        {
            std::lock_guard lock(chr->mutex_);
            zone = chr->GetZoneFromWeak();
            chr_oid = chr->GetObjectId();
        }
        if (zone) {
            auto mob = zone->GetMonsterThreadSafe(mob_id);
            if (mob && zone->GetMobControllerOid() != Zone::MobControllerNullValue) {
                UE4OutPacket out;
                {
                    std::lock_guard lock(mob->mutex_);
                    mob->SetState(state);
                    mob->GetLocation() = lo;
                    mob->GetRotation() = ro;
                    PacketGenerator::UpdateMonsterAction(out, *mob);
                }
                out.MakePacketHead();
                zone->BroadCast(out, chr_oid);
            }
        }
    }
}

void ZoneServer::HandleMatchingRequest(Client& client, Session::InPacket& in_packet)
{
    auto match = client.GetMatchFromWeak();
    bool is_request = in_packet.ReadInt8();
    if (is_request) {
        int32_t mapid = in_packet.ReadInt32();
        if (match) {
            throw StackTraceException(ExceptionType::kRunTimeError, "Match Join) client already has matching instance");
        }
        match = MatchSystem::Instance().CreateOrFind(mapid);
        client.SetMatchWeak(match);
        match->Join(client.GetUUID().ToString());
    } else {
        if (!match) {
            throw StackTraceException(ExceptionType::kNullPointer, "Match Cancel) client has no matching instance");
        }
        match->Remove(client.GetUUID().ToString());
    }
}

void ZoneServer::HandleMatchingComfirmRequest(Client& client, Session::InPacket& in_packet)
{
    auto match = client.GetMatchFromWeak();
    if (match) {
        match->Confirm(client.GetUUID().ToString(), in_packet.ReadInt8());
    }
}

void ZoneServer::HandleCharacterAttackMotion(Client& client, Session::InPacket& in_packet)
{
    int32_t motion_id = in_packet.ReadInt32();
    FVector location; location.Read(in_packet);
    FVector rotation; rotation.Read(in_packet);

    auto chr = client.GetCharacter();
    std::shared_ptr<Zone> zone = nullptr;
    if (chr) {
        std::lock_guard lock(chr->mutex_);
        zone = chr->GetZoneFromWeak();
    } else {
        throw StackTraceException(ExceptionType::kNullPointer, "character is nullptr");
    };
    if (zone) {
            UE4OutPacket out;
            PacketGenerator::CharacterAttack(out, *chr, motion_id);
            out.MakePacketHead();
            zone->BroadCast(out, chr->GetObjectId());
    } else {
        throw StackTraceException(ExceptionType::kNullPointer, "zone is nullptr");
    }
}

void ZoneServer::HandleCharacterHitSuccess(Client& client, Session::InPacket& in_packet)
{
    int64_t character_oid = in_packet.ReadInt64();
    int64_t mob_oid = in_packet.ReadInt64();
    int32_t attack_id = in_packet.ReadInt32();
    FVector mob_lo; mob_lo.Read(in_packet);
    FVector mob_ro; mob_ro.Read(in_packet);

    auto chr = client.GetCharacter();
    std::shared_ptr<Zone> zone = nullptr;
    if (chr) {
        std::lock_guard lock(chr->mutex_);
        if (character_oid != chr->GetObjectId())
            return;
        zone = chr->GetZoneFromWeak();
    } else {
        throw StackTraceException(ExceptionType::kNullPointer, "character is nullptr");
    };
    if (zone) {
        auto mob = zone->GetMonsterThreadSafe(mob_oid);
        if (mob)
        {
            UE4OutPacket out;
            std::scoped_lock double_lock(chr->mutex_, mob->mutex_);
            PythonScript::Engine::Instance().ReloadExecute(
                PythonScript::Path::kCharacter,
                "AttackMonster",
                "Start",
                PYTHON_PASSING_BY_REFERENCE(*chr),
                PYTHON_PASSING_BY_REFERENCE(*mob),
                attack_id);
            if (mob->GetHP() == 0.0f) {
                mob->SetState(static_cast<int16_t>(Monster::State::kDead));
            }
            PacketGenerator::ActorDamaged(out, *chr, *mob, attack_id, mob->GetHP());
                out.MakePacketHead();
            zone->BroadCast(out);
        }
    } else {
        throw StackTraceException(ExceptionType::kNullPointer, "zone is nullptr");
    }
}

void ZoneServer::HandleMonsterAttack(Client& client, Session::InPacket& in_packet)
{
    int64_t attacker_mob_oid = in_packet.ReadInt64();
    int64_t attacked_chr_oid = in_packet.ReadInt64();
    int32_t attack_id = in_packet.ReadInt32();
    Location lo; lo.Read(in_packet);
    Rotation ro; ro.Read(in_packet);

    auto chr = client.GetCharacter();
    std::shared_ptr<Zone> zone = nullptr;
    if (chr) {
        std::lock_guard lock(chr->mutex_);
        if (chr->GetHP() == 0.0f) {
            return;
        }
        zone = chr->GetZoneFromWeak();
    } else {
        throw StackTraceException(ExceptionType::kNullPointer, "character is nullptr");
    };
    if (zone) {
        auto mob = zone->GetMonsterThreadSafe(attacker_mob_oid);
        if (mob)
        {
            std::scoped_lock double_lock(chr->mutex_, mob->mutex_);
            std::stringstream ss;
            ss << "damaged name: " << chr->GetName() << ", id: " << chr->GetObjectId() << '\n';
            std::cout << ss.str();
            PythonScript::Engine::Instance().ReloadExecute(
                PythonScript::Path::kMonster,
                "AttackCharacter",
                "Start",
                PYTHON_PASSING_BY_REFERENCE(*chr),
                PYTHON_PASSING_BY_REFERENCE(*mob),
                attack_id);
            UE4OutPacket out;
            PacketGenerator::ActorDamaged(out, *mob, *chr, attack_id, chr->GetHP());
            out.MakePacketHead();
            zone->BroadCast(out);

            if (chr->GetHP() == 0) {
                // Set Dead State

                UE4OutPacket remote;
                PacketGenerator::CharacterDead(remote, attacked_chr_oid);
                zone->BroadCast(remote, attacked_chr_oid);
            }
        }
    } else {
        throw StackTraceException(ExceptionType::kNullPointer, "zone is nullptr");
    }
}

void ZoneServer::HandleCharacterWait(Client& client, Session::InPacket& in_packet)
{
    int64_t cid = in_packet.ReadInt64();
    Location lo; lo.Read(in_packet);

    auto chr = client.GetCharacter();
    if (chr) {
        std::shared_ptr<Zone> zone = nullptr;
        UE4OutPacket out;
        {
            std::lock_guard lock(chr->mutex_);
            if (cid != chr->GetObjectId())
                return;
            chr->GetLocation() = lo;
            zone = chr->GetZoneFromWeak();
            PacketGenerator::CharacterWait(out, *chr);
        }
        if (zone)
        {
            out.MakePacketHead();
            zone->BroadCast(out, cid);
        }
    }
}

void ZoneServer::HandleCharacterSprint(Client& client, Session::InPacket& in_packet)
{
    int64_t cid = in_packet.ReadInt64();
    //Location lo; lo.Read(in_packet);

    auto chr = client.GetCharacter();
    if (chr) {
        std::shared_ptr<Zone> zone = nullptr;
        {
            std::lock_guard lock(chr->mutex_);
            if (cid != chr->GetObjectId()) {
                std::stringstream ss;
                ss << "-----------------------------\n";
                ss << "Client Send cid: " << cid << '\n';
                ss << "Target Read cid: " << chr->GetObjectId() << '\n';
                std::cout << ss.str();
                return;
            }
                
            zone = chr->GetZoneFromWeak();
            //chr->GetLocation() = lo;
        }
        if (zone)
        {
            UE4OutPacket out;
            PacketGenerator::CharacterSprint(out, *chr);
            out.MakePacketHead();
            zone->BroadCast(out, cid);
        }
    }
}

void ZoneServer::HandleZoneChangeRequest(Client& client, Session::InPacket& in_packet)
{
    auto chr = client.GetCharacter();
    if (chr) {
        std::shared_ptr<Zone> zone = nullptr;
        {
            std::lock_guard lock(chr->mutex_);
            zone = chr->GetZoneFromWeak();
        }
        if (zone) {
            if (zone->GetType() == Zone::Type::kTown) {
                throw StackTraceException(ExceptionType::kRunTimeError, " bad request");
            }
            zone->TryChangeZone();
        }
    }
}

// change two slot
// max overlap count = 200
void ZoneServer::HandleInventoryUpdate(Client& client, Session::InPacket& in_packet)
{
    // slot1 To slot2
    int32_t slot1 = in_packet.ReadInt32();
    int32_t slot2 = in_packet.ReadInt32();
    if (slot1 < 0 || slot1 >= Inventory::inventory_size || slot2 < 0 || slot2 >= Inventory::inventory_size) {
        throw StackTraceException(ExceptionType::kRunTimeError, " bad request");
    }

    auto chr = client.GetCharacter();
    if (chr) {
        UE4OutPacket out;
        std::lock_guard lock(chr->mutex_);
        chr->ChangeInventoryItemPosition(slot1, slot2);
        const auto& inventory = chr->GetInventory();
        bool slot1_has_item = inventory[slot1].operator bool();
        bool slot2_has_item = inventory[slot2].operator bool();

        PacketGenerator::InventoryUpdate(out,
            false, slot1,
            slot1_has_item ? inventory[slot1]->item.get() : nullptr,
            slot1_has_item ? inventory[slot1]->count : 0,
            false, slot2,
            slot2_has_item ? inventory[slot2]->item.get() : nullptr,
            slot2_has_item ? inventory[slot2]->count : 0);
        out.MakePacketHead();
        client.GetSession()->Send(out, true, false);
    }
}

void ZoneServer::HandleRequestRolling(Client& client, Session::InPacket& in_packet)
{
    int64_t ccid = in_packet.ReadInt64();
    Rotation ro; ro.Read(in_packet);

    auto chr = client.GetCharacter();
    if (chr) {
        int64_t cid = -1;
        std::shared_ptr<Zone> zone = nullptr;
        {
            std::lock_guard lock(chr->mutex_);
            cid = chr->GetObjectId();
            zone = chr->GetZoneFromWeak();
        }
        if ((ccid == cid) && zone) {
            UE4OutPacket out;
            out.WriteInt16(static_cast<int16_t>(ENetworkSCOpcode::kNotifyCharacterRolling));
            out.WriteInt64(cid);
            out.Write(ro);
            out.MakePacketHead();
            zone->BroadCast(out, cid);
        }
    }
}

#include "ExportZoneServer.hpp"
#include "Server/ZoneServer.hpp"
#include "ServerConstants.hpp"
#include "GameConstants.hpp"
#include "Provider/CSVDataProvider.hpp"
#include "Provider/ZoneDataProvider.hpp"
#include "Provider/MonsterTemplateProvider.hpp"
#include "UE4DevelopmentLibrary/Database.hpp"
#include "System/PythonScriptEngine.hpp"



#include "Model/Character/Character.hpp"

void ExportZoneServer::Initialize()
{
    const_cast<ServerConstants&>(ServerConstants::Instance()).Initialize();
    const_cast<GameConstants&>(GameConstants::Instance()).Initialize();
    const_cast<CSVDataProvider<EquipTableData>&>(CSVDataProvider<EquipTableData>::Instance()).Initialize();
    const_cast<CSVDataProvider<MonsterTableData>&>(CSVDataProvider<MonsterTableData>::Instance()).Initialize();
    const_cast<ZoneDataProvider&>(ZoneDataProvider::Instance()).Initialize();
    const_cast<MonsterTemplateProvider&>(MonsterTemplateProvider::Instance()).Initialize();
    PythonScript::Engine::Instance().Initialize();
    ODBCConnectionPool::Instance().Initialize(10,
        ServerConstant.odbc_name, ServerConstant.db_id, ServerConstant.db_pw);
    zone_server = new ZoneServer();
    zone_server->Initialize();
}

void ExportZoneServer::Run()
{
    zone_server->Run();
}

void ExportZoneServer::Stop()
{
    zone_server->Stop();
    PythonScript::Engine::Instance().Release();
}

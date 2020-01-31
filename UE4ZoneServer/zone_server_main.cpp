#include "ServerConstants.hpp"
#include "GameConstants.hpp"
#include "Provider/ZoneDataProvider.hpp"
#include "Provider/MonsterTemplateProvider.hpp"
#include "Provider/CSVDataProvider.hpp"
#include "Provider/CSV/EquipTableData.hpp"
#include "Provider/CSV/MonsterTableData.hpp"
#include "System/ZoneSystem.hpp"
#include "UE4DevelopmentLibrary/Database/ODBCConnectionPool.hpp"
#include "ZoneServer.hpp"
#include <iostream>



int main()
{
    try {
        const_cast<ServerConstants&>(ServerConstants::Instance()).Initialize();
        const_cast<GameConstants&>(GameConstants::Instance()).Initialize();
        const_cast<CSVDataProvider<EquipTableData>&>(CSVDataProvider<EquipTableData>::Instance()).Initialize();
        const_cast<CSVDataProvider<MonsterTableData>&>(CSVDataProvider<MonsterTableData>::Instance()).Initialize();
        const_cast<ZoneDataProvider&>(ZoneDataProvider::Instance()).Initialize();
        const_cast<MonsterTemplateProvider&>(MonsterTemplateProvider::Instance()).Initialize();
        ODBCConnectionPool::Instance().Initialize(10,
            ServerConstant.odbc_name, ServerConstant.db_id, ServerConstant.db_pw);
        ZoneSystem::Initialize();

        ZoneServer::Instance().Initialize();
        ZoneServer::Instance().Run();
        ZoneServer::Instance().Stop();
        ZoneSystem::Release();
    } catch (const std::exception & e) {
        std::cout << e.what();
    }
    return 0;
}
#include "ServerConstants.hpp"
#include "GameConstants.hpp"
#include "Provider/ZoneDataProvider.hpp"
#include "Provider/MonsterTemplateProvider.hpp"
#include "Provider/CSVDataProvider.hpp"
#include "Provider/CSV/EquipTableData.hpp"
#include "Provider/CSV/MonsterTableData.hpp"
#include "System/ZoneSystem.hpp"
#include "UE4DevelopmentLibrary/Database/ODBCConnectionPool.hpp"
#include <iostream>



int main()
{
    try {
        CSVDataProvider<EquipTableData>::Instance().Initialize();
        CSVDataProvider<MonsterTableData>::Instance().Initialize();
        const_cast<ServerConstants&>(ServerConstants::Instance()).Initialize();
        const_cast<GameConstants&>(GameConstants::Instance()).Initialize();
        const_cast<ZoneDataProvider&>(ZoneDataProvider::Instance()).Initialize();
        const_cast<MonsterTemplateProvider&>(MonsterTemplateProvider::Instance()).Initialize();
        ODBCConnectionPool::Instance().Initialize(10,
            ServerConstant.odbc_name, ServerConstant.db_id, ServerConstant.db_pw);

        ZoneSystem::Initialize();
    } catch (const std::exception & e) {
        std::cout << e.what();
    }
    return 0;
}
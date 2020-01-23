#include "Constants/ServerConstants.hpp"
#include "Constants/GameConstants.hpp"
#include "Data/ZoneDataProvider.hpp"
#include "Data/MonsterTemplateProvider.hpp"
#include "UE4DevelopmentLibrary/Database/ODBCConnectionPool.hpp"
#include <iostream>



int main()
{
    try {
        const_cast<ServerConstants&>(ServerConstants::Instance()).Initialize();
        const_cast<GameConstants&>(GameConstants::Instance()).Initialize();
        const_cast<ZoneDataProvider&>(ZoneDataProvider::Instance()).Initialize();
        const_cast<MonsterTemplateProvider&>(MonsterTemplateProvider::Instance()).Initialize();
        ODBCConnectionPool::Instance().Initialize(10,
            ServerConstant.odbc_name, ServerConstant.db_id, ServerConstant.db_pw);
    } catch (const std::exception & e) {
        std::cout << e.what();
    }



    return 0;
}
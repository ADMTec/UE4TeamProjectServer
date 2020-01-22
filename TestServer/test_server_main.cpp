#include "TestServer.hpp"
#include "UE4DevelopmentLibrary/Database.hpp"
#include <iostream>


int main()
{

    try {
        int cid = 1;
        ODBCConnectionPool::Instance().Initialize(3, L"UE4", L"root", L"root");
        auto con = ODBCConnectionPool::Instance().GetConnection();
        auto ps = con->GetPreparedStatement();
        ps->PrepareStatement(L"select characters.*, inventoryitems.itemid from characters, inventoryitems where characters.cid = inventoryitems.cid and inventoryitems.equipped = 1 and characters.cid = ?;");
        ps->SetInt32(1, &cid);
        auto rs = ps->Execute();

        int db_cid;
        int accid;
        int slot;
        char name[100];
        int level;
        int str;
        int dex;
        int intel;
        int job;
        int face;
        int hair;
        int gold;
        int zone;
        float x;
        float y;
        float z;
        int gender;
        int itemid;
        rs->BindInt32(1, &db_cid);
        rs->BindInt32(2, &accid);
        rs->BindInt32(3, &slot);
        rs->BindString(4, name, 100);
        rs->BindInt32(5, &level);
        rs->BindInt32(6, &str);
        rs->BindInt32(7, &dex);
        rs->BindInt32(8, &intel);
        rs->BindInt32(9, &job);
        rs->BindInt32(10, &face);
        rs->BindInt32(11, &hair);
        rs->BindInt32(12, &gold);
        rs->BindInt32(13, &zone);
        rs->BindFloat32(14, &x);
        rs->BindFloat32(15, &y);
        rs->BindFloat32(16, &z);
        rs->BindInt32(17, &gender);
        rs->BindInt32(18, &itemid);
        while (rs->Next()) {
            std::cout << db_cid << " " << slot << " " << name << " " << level << " " << itemid << std::endl;
        }
    } catch (const std::exception & e) {
        std::cout << e.what() << std::endl;
    }

    TestServer::Instance().Initialize();
    TestServer::Instance().Run();

    getchar();
    getchar();
    getchar();
    TestServer::Instance().Stop();

    return 0;
}
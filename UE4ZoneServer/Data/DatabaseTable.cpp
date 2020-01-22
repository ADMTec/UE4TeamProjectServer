#include "DatabaseTable.hpp"
#include "Database.hpp"


// ----------------------------------------------------------------------------------------------------------------------
const wchar_t* CharacterTable::GetSelectQueryFromCid()
{
    return L"select name, level, str, dex, intel, job, face, hair, gold, zone, x, y, z, gender from characters wherer cid = ?";
}

void CharacterTable::BindResultSet(ResultSet& rs, CharacterTable* data)
{
    rs.BindString(1, data->name, 100);
    rs.BindInt32(2, &data->level);
    rs.BindInt32(3, &data->str);
    rs.BindInt32(4, &data->dex);
    rs.BindInt32(5, &data->intel);
    rs.BindInt32(6, &data->job);
    rs.BindInt32(7, &data->face);
    rs.BindInt32(8, &data->hair);
    rs.BindInt32(9, &data->gold);
    rs.BindInt32(10, &data->zone_id);
    rs.BindFloat32(11, &data->x);
    rs.BindFloat32(12, &data->y);
    rs.BindFloat32(13, &data->z);
    rs.BindInt32(14, &data->gender);
}

const wchar_t* CharacterTable::GetUpdateQueryFromCid()
{
    return L"update characters set name = ?, level = ?, str = ?, dex = ?, intel = ?, job = ?, hair = ?, gold = ?, zone = ?, x = ?, y = ?, z = ?, gender = ? where cid = ?";
}

// ----------------------------------------------------------------------------------------------------------------------
const wchar_t* InventoryItemTable::GetSelectQueryFromCid()
{
    return L"select itemid, type, quantity, slot, str, dex, intel, equipped from inventoryitems wherer cid = ?";
}

void InventoryItemTable::BindResultSet(ResultSet& rs, InventoryItemTable* data)
{
    rs.BindInt32(1, &data->itemid);
    rs.BindInt32(2, &data->type);
    rs.BindInt32(3, &data->quantity);
    rs.BindInt32(4, &data->slot);
    rs.BindInt32(5, &data->str);
    rs.BindInt32(6, &data->dex);
    rs.BindInt32(7, &data->intel);
    rs.BindInt32(8, &data->equipped);
}

const wchar_t* InventoryItemTable::GetDeleteQueryFromCid()
{
    return L"delete from inventoryitems where cid = ?";
}
// ----------------------------------------------------------------------------------------------------------------------

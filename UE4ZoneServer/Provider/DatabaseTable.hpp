#pragma once
#include <cstdint>


struct CharacterTable
{
    char name[100];
    int32_t level;
    int32_t str;
    int32_t dex;
    int32_t intel;
    int32_t job;
    int32_t face;
    int32_t hair;
    int32_t gold;
    int32_t zone_id;
    float x;
    float y;
    float z;
    int32_t gender;

    static const wchar_t* GetSelectQueryFromCid();
    static void BindResultSet(class ResultSet& rs, CharacterTable* data);
    static const wchar_t* GetUpdateQueryFromCid();
};

struct InventoryItemTable
{
    int32_t itemid;
    int32_t type;
    int32_t quantity;
    int32_t slot;
    int32_t str;
    int32_t dex;
    int32_t intel;
    int32_t equipped;
    static const wchar_t* GetSelectQueryFromCid();
    static void BindResultSet(class ResultSet& rs, InventoryItemTable* data);
    static const wchar_t* GetDeleteQueryFromCid();
};
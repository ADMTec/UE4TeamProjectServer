#pragma once


#ifdef UE4MODEL_EXPORTS
#define UE4MODEL_DLLCLASS __declspec(dllexport)
#define UE4MODEL_DLLEXPORT
#else
#define UE4MODEL_DLLCLASS __declspec(dllimport)
#define UE4MODEL_DLLEXPORT extern
#endif


class UE4MODEL_DLLCLASS ExportZoneServer
{
public:
    void Initialize();
    void Run();
    void Stop();
private:
    class ZoneServer* zone_server;
};
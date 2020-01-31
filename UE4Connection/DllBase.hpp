#pragma once
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#ifdef UE4CONNECTION_EXPORTS
#define UE4CONNECTION_DLLCLASS __declspec(dllexport)
#define UE4CONNECTION_DLLEXPORT
#else
#define UE4CONNECTION_DLLCLASS __declspec(dllimport)
#define UE4CONNECTION_DLLEXPORT extern
#endif

#include <cstdint>
#include <string>
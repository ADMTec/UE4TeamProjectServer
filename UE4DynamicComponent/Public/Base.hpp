#pragma once

#ifdef UE4DYNAMICCOMPONENT_EXPORTS
#define UE4_DLL_CLASS __declspec(dllexport)
#define UE4_DLLEXPORT
#else
#define UE4_DLL_CLASS __declspec(dllimport)
#define UE4_DLLEXPORT extern
#endif
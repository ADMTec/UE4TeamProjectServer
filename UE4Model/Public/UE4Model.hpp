#pragma once
#include <cstdint>

#ifdef UE4MODEL_EXPORTS
#define UE4MODEL_DLLCLASS __declspec(dllexport)
#define UE4MODE_DLLEXPORT
#else
#define UE4MODEL_DLLCLASS __declspec(dllimport)
#define UE4MODE_DLLEXPORT extern
#endif

template<typename T>
constexpr int8_t ToInt8(T value) {
    return static_cast<int8_t>(value);
}

template<typename T>
constexpr int16_t ToInt16(T value) {
    return static_cast<int16_t>(value);
}

template<typename T>
constexpr int32_t ToInt32(T value) {
    return static_cast<int32_t>(value);
}

template<typename T>
constexpr int64_t ToInt64(T value) {
    return static_cast<int64_t>(value);
}
#pragma once
#include "UE4Model.hpp"
#include "UE4DevelopmentLibrary/Stream.hpp"

struct UE4MODEL_DLLCLASS FVector : public SerializeInterface
{
public:
    FVector(const FVector& rhs) {
        FVector::operator=(rhs);
    }
    void operator=(const FVector& rhs) {
        x = rhs.x;
        y = rhs.y;
        z = rhs.y;
    }
    FVector(float _x, float _y, float _z)
        : x(_x), y(_y), z(_z)
    {
    }
    FVector() : FVector(0.0, 0.0, 0.0)
    {

    }
    virtual void Write(OutputStream& output) const {
        output << x;
        output << y;
        output << z;
    }
    virtual void Read(InputStream& input) {
        input >> x;
        input >> y;
        input >> z;
    }
public:
    float x;
    float y;
    float z;
};

using Location = FVector;
using Rotation = FVector;
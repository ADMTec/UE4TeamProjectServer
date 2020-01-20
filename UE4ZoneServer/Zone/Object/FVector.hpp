#pragma once
#include "UE4DevelopmentLibrary/Stream.hpp"

struct FVector : public SerializeInterface
{
public:
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
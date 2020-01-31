#pragma once
#include "../DllBase.hpp"

class InputStream;
class OutputStream;

class UE4CONNECTION_DLLCLASS SerializeInterface
{
public:
    virtual void Write(OutputStream& output) const = 0;
    virtual void Read(InputStream& input) = 0;
};
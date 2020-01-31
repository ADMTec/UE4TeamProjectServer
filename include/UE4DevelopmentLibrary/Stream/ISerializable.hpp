#pragma once

class InputStream;
class OutputStream;

class SerializeInterface
{
public:
    virtual void Write(OutputStream& output) const = 0;
    virtual void Read(InputStream& input) = 0;
};
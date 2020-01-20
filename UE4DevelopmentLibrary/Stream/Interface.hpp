#pragma once

class InputStream;
class OutputStream;

class SerializeInterface
{
public:
    virtual void Write(OutputStream& outpacket) const = 0;
    virtual void Read(InputStream& inpacket) = 0;
};
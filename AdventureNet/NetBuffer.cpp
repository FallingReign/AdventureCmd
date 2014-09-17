#include <NetBuffer.hpp>

using namespace anet;

NetBuffer::NetBuffer()
: m_readpos(0)
{

}

// Write
NetBuffer& NetBuffer::operator<<(bool inData)
{
    writeData(&inData, sizeof(inData));
    return *this;
}

NetBuffer& NetBuffer::operator<<(UInt8 inData)
{
    writeData(&inData, sizeof(inData));
    return *this;
}

NetBuffer& NetBuffer::operator<<(Int8 inData)
{
    writeData(&inData, sizeof(inData));
    return *this;
}

NetBuffer& NetBuffer::operator<<(UInt16 inData)
{
    writeData(&inData, sizeof(inData));
    return *this;
}

NetBuffer& NetBuffer::operator<<(Int16 inData)
{
    writeData(&inData, sizeof(inData));
    return *this;
}

NetBuffer& NetBuffer::operator<<(UInt32 inData)
{
    writeData(&inData, sizeof(inData));
    return *this;
}

NetBuffer& NetBuffer::operator<<(Int32 inData)
{
    writeData(&inData, sizeof(inData));
    return *this;
}

NetBuffer& NetBuffer::operator<<(UInt64 inData)
{
    writeData(&inData, sizeof(inData));
    return *this;
}

NetBuffer& NetBuffer::operator<<(Int64 inData)
{
    writeData(&inData, sizeof(inData));
    return *this;
}

NetBuffer& NetBuffer::operator<<(Float32 inData)
{
    writeData(&inData, sizeof(inData));
    return *this;
}

NetBuffer& NetBuffer::operator<<(Float64 inData)
{
    writeData(&inData, sizeof(inData));
    return *this;
}

// Read
NetBuffer& NetBuffer::operator>>(bool& outData)
{
    readData(&outData, sizeof(outData));
    return *this;
}

NetBuffer& NetBuffer::operator>>(UInt8& outData)
{
    readData(&outData, sizeof(outData));
    return *this;
}

NetBuffer& NetBuffer::operator>>(Int8& outData)
{
    readData(&outData, sizeof(outData));
    return *this;
}

NetBuffer& NetBuffer::operator>>(UInt16& outData)
{
    readData(&outData, sizeof(outData));
    return *this;
}

NetBuffer& NetBuffer::operator>>(Int16& outData)
{
    readData(&outData, sizeof(outData));
    return *this;
}

NetBuffer& NetBuffer::operator>>(UInt32& outData)
{
    readData(&outData, sizeof(outData));
    return *this;
}

NetBuffer& NetBuffer::operator>>(Int32& outData)
{
    readData(&outData, sizeof(outData));
    return *this;
}

NetBuffer& NetBuffer::operator>>(UInt64& outData)
{
    readData(&outData, sizeof(outData));
    return *this;
}

NetBuffer& NetBuffer::operator>>(Int64& outData)
{
    readData(&outData, sizeof(outData));
    return *this;
}

NetBuffer& NetBuffer::operator>>(Float32& outData)
{
    readData(&outData, sizeof(outData));
    return *this;
}

NetBuffer& NetBuffer::operator>>(Float64& outData)
{
    readData(&outData, sizeof(outData));
    return *this;
}

UInt32 NetBuffer::getSize() const
{
    return m_buffer.size();
}

const std::vector<UInt8>& NetBuffer::getBuffer() const
{
    return m_buffer;
}

void NetBuffer::writeData(const void* inData, size_t inSize)
{
    unsigned int pos = m_buffer.size();
    m_buffer.resize(pos + inSize);
    memcpy(&m_buffer[pos], inData, inSize);
}

void NetBuffer::readData(void* outData, size_t inSize)
{
    if (m_readpos + inSize > m_buffer.size())
        return;
    memcpy(outData, &m_buffer[m_readpos], inSize);
    m_readpos += inSize;
}
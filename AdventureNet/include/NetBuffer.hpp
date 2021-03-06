#ifndef NETBUFFER_HPP
#define NETBUFFER_HPP

#include <vector>

#include <NetDefs.hpp>

namespace anet
{
    class NetBuffer
    {
        friend class UdpSocket;
        friend class TcpClient;
        friend class TcpListener;
    public:
        NetBuffer();

        // Write
        NetBuffer& operator<<(bool inData);

        NetBuffer& operator<<(UInt8 inData);
        NetBuffer& operator<<(Int8 inData);

        NetBuffer& operator<<(UInt16 inData);
        NetBuffer& operator<<(Int16 inData);

        NetBuffer& operator<<(UInt32 inData);
        NetBuffer& operator<<(Int32 inData);

        NetBuffer& operator<<(UInt64 inData);
        NetBuffer& operator<<(Int64 inData);

        NetBuffer& operator<<(Float32 inData);
        NetBuffer& operator<<(Float64 inData);

        // Read
        NetBuffer& operator>>(bool& outData);

        NetBuffer& operator>>(UInt8& outData);
        NetBuffer& operator>>(Int8& outData);

        NetBuffer& operator>>(UInt16& outData);
        NetBuffer& operator>>(Int16& outData);

        NetBuffer& operator>>(UInt32& outData);
        NetBuffer& operator>>(Int32& outData);

        NetBuffer& operator>>(UInt64& outData);
        NetBuffer& operator>>(Int64& outData);

        NetBuffer& operator>>(Float32& outData);
        NetBuffer& operator>>(Float64& outData);

        UInt32 getSize() const;
        const std::vector<UInt8>& getBuffer() const;
    private:
        std::vector<UInt8> m_buffer;
        std::vector<UInt8>::size_type m_readpos;

        void writeData(const void* inData, size_t inSize);
        void readData(void* outData, size_t inSize);
    };
}

#endif
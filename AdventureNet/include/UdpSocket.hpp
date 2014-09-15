#ifndef UDPSOCKET_HPP
#define UDPSOCKET_HPP

#include <NetSocket.hpp>
#include <NetAddress.hpp>

namespace anet
{
    class NetBuffer;
    class UdpSocket
        : public NetSocket
    {
    public:
        UdpSocket();
        ~UdpSocket();

        bool bind(UInt16 inPort);
        void unBind();

        int send(const NetBuffer& inBuffer, const NetAddress& inAddress);
        int receive(NetBuffer& outBuffer, NetAddress& outAddress);

        // Maximum theoretical size.
        // SOURCE: http://support.microsoft.com/kb/822061
        static const Int32 MAX_DGRAMSIZE = 65507;
    private:
    };
}

#endif
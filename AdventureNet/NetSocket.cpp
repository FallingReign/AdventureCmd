#include <NetSocket.hpp>
#include <WinSock2.h>

using namespace anet;

NetSocket::NetSocket(Type type)
    : m_type(type), m_sockid(INVALID_SOCKET)
{
}

NetSocket::~NetSocket()
{
}

void NetSocket::setBlocking(bool block)
{
    u_long doBlock = (block) ? 0 : 1;

    if (m_sockid != INVALID_SOCKET)
        ::ioctlsocket(m_sockid, FIONBIO, &doBlock);
}

void NetSocket::init()
{
    if (m_sockid == INVALID_SOCKET)
    {
        m_sockid = ::socket(AF_INET, (m_type == Type::TCP) ? SOCK_STREAM : SOCK_DGRAM, 0);

        // Allow broadcast
        if (m_type == Type::UDP)
        {
            int enabled = 1;
            ::setsockopt(m_sockid, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char*>(enabled), sizeof(enabled));
        }
    }
}

void NetSocket::uninit()
{
    if (m_sockid != INVALID_SOCKET)
    {
        closesocket(m_sockid);
        m_sockid = INVALID_SOCKET;
    }
}

UInt32 NetSocket::getSocketID() const
{
    return m_sockid;
}
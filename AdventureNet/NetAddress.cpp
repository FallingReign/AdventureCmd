#include <NetAddress.hpp>
#include <WinSock2.h>
#include <ws2tcpip.h>

using namespace anet;

NetAddress::NetAddress()
: m_address(0), m_port(0)
{
}

NetAddress::NetAddress(const std::string& inAddress, UInt16 inPort)
: m_address(resolve(inAddress)), m_port(inPort)
{

}

NetAddress::NetAddress(UInt32 inAddress, UInt16 inPort)
: m_address(inAddress), m_port(inPort)
{
}

sockaddr_in NetAddress::toAddress() const
{
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    addr.sin_addr.S_un.S_addr = m_address;

    return addr;
}

UInt16 NetAddress::getPort() const
{
    return m_port;
}

std::string NetAddress::getIP() const
{
    std::string ip;
    IN_ADDR addr;
    addr.S_un.S_addr = m_address;
    ip = inet_ntoa(addr);
    return ip;
}

UInt32 NetAddress::resolve(const std::string& inAddress)
{
    // Return broadcast if specified.
    if (inAddress == "255.255.255.255")
        return htonl(INADDR_BROADCAST);

    // First try to do it without a resolve.
    UInt32 addr = inet_addr(inAddress.c_str());
    
    // If it is valid then we are done.
    if (addr != INADDR_NONE)
        return addr;

    // Resolve otherwise.
    addrinfo inf;
    inf.ai_family = AF_INET;
    addrinfo* result = 0;

    // If == 0 then we got a valid result.
    if (::getaddrinfo(inAddress.c_str(), 0, &inf, &result) == 0)
    {
        if (result != 0)
        {
            sockaddr_in* iaddr = (sockaddr_in*)&result->ai_addr;
            addr = iaddr->sin_addr.S_un.S_addr;
            freeaddrinfo(result);
            return addr;
        }
    }

    return 0;
}
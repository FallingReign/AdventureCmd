#include <UdpSocket.hpp>
#include <NetBuffer.hpp>
#include <NetAddress.hpp>
#include <WinSock2.h>

using namespace anet;

UdpSocket::UdpSocket()
: NetSocket(Type::UDP)
{
}

UdpSocket::~UdpSocket()
{
    uninit();
}

bool UdpSocket::bind(UInt16 port)
{
    // Initialize Socket.
    init();

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.S_un.S_addr = INADDR_ANY;

    if (::bind(getSocketID(), (sockaddr*)&addr, sizeof(addr)) == -1)
    {
        return false;
    }

    return true;
}

void UdpSocket::unBind()
{
    uninit();
}

int UdpSocket::send(const NetBuffer& inBuffer, const NetAddress& inAddress)
{
    if (inBuffer.getSize() > MAX_DGRAMSIZE)
        return 0;

    const sockaddr_in addr = inAddress.toAddress();

    int bytesSent = ::sendto(getSocketID(), (char*)&inBuffer.getBuffer()[0], inBuffer.getSize(), 0, (sockaddr*)&addr, sizeof(addr));

    return bytesSent;
}

int UdpSocket::receive(NetBuffer& outBuffer, NetAddress& outAddress)
{
    //std::vector<UInt8> buffer;
    outBuffer.m_buffer.resize(MAX_DGRAMSIZE);

    sockaddr_in addr;
    int size = sizeof(addr);

    int bytesRecv = ::recvfrom(getSocketID(), (char*)&outBuffer.m_buffer[0], MAX_DGRAMSIZE, 0, (sockaddr*)&addr, &size);

    outAddress = NetAddress(addr.sin_addr.S_un.S_addr, ntohs(addr.sin_port));

    return bytesRecv;
}
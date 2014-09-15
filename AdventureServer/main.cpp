#include <iostream>

#include <NetBuffer.hpp>
#include <UdpSocket.hpp>

#include <algorithm>
#include <map>

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>

struct ClientHasher
{
    unsigned int operator()(const anet::NetAddress& k) const
    {
        using std::size_t;
        using std::hash;
        using std::string;

        // Compute individual hash values for first,
        // second and third and combine them using XOR
        // and bit shifting:
        return std::hash<std::string>()(k.getIP())
            ^ ((std::hash<anet::UInt8>()(k.getPort()) << 1) >> 1);
    }
};

void runServer()
{
    anet::UdpSocket serverSock;
    serverSock.bind(32002);

    std::map<unsigned int, anet::NetAddress> clientList;

    anet::NetBuffer buffer;
    anet::NetAddress addr;
    int bytes = serverSock.receive(buffer, addr);

    clientList.insert(std::pair<unsigned int, anet::NetAddress>(ClientHasher()(addr), addr));

    int x;
    buffer >> x;

    anet::NetAddress& ad = clientList[ClientHasher()(addr)];
    std::cout << "Hash: " << ClientHasher()(addr) << "\n";
    std::cout << "Client found: " << ad.getIP() << ":" << ad.getPort() << "\nData: " << x << "\n";
}

void runClient()
{
    anet::UdpSocket clientSock;

    // Bind to any available port.
    clientSock.bind(0);

    anet::NetAddress addr("127.0.0.1", 32002);
    anet::NetBuffer buffer;
    buffer << (int)10;
    clientSock.send(buffer, addr);
}

int main(int argc, char** argv)
{
    WSAData data;
    WSAStartup(MAKEWORD(2, 2), &data);

    std::cout << "Select 1 or 0: ";
    int choice;
    std::cin >> choice;
    std::cout << "\n";

    if (choice == 0)
    {
        runServer();
    }
    else
    {
        runClient();
    }

    WSACleanup();

    std::cin.sync();
    std::cin.get();

    return 0;
}
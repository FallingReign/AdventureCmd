#include <iostream>
#include <vector>

#include <NetBuffer.hpp>
#include <UdpSocket.hpp>

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>


void runServer()
{
    anet::UdpSocket serverSock;
    serverSock.bind(32002);

    anet::NetBuffer buffer;
    anet::NetAddress addr;
    int bytes = serverSock.receive(buffer, addr);
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
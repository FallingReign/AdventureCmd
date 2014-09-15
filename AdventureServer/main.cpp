#include <iostream>
#include <vector>

#include <NetBuffer.hpp>
#include <UdpSocket.hpp>

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>

#include "Server.hpp"

void runServer()
{
    Server server;

    bool quitting = false;
    
    server.Start();
    while (!quitting)
    {
        std::string cmd;
        std::getline(std::cin, cmd);

        if (cmd == "quit")
            quitting = true;
    }

    server.Stop();
}

void runClient()
{
    anet::UdpSocket clientSock;

    // Bind to any available port.
    clientSock.bind(0);

    anet::NetAddress addr("127.0.0.1", 32002);
    anet::NetBuffer buffer;
    buffer << (anet::UInt16)50322 << (anet::UInt8)0;
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
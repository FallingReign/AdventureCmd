#include <iostream>
#include <vector>

#include <NetBuffer.hpp>
#include <UdpSocket.hpp>

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>

#include "Server.hpp"
#include "Timer.hpp"

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

    Timer clientTimer;
    unsigned int accTime = 0;
    anet::Int32 x = 0, y = 0;
    while (true)
    {
        accTime += clientTimer.Restart();
        if (accTime > (1000 / 60))
        {
            x += 1;
            y += 2;

            anet::NetBuffer posBuffer;
            posBuffer << (anet::UInt16)50322 << (anet::UInt8)1 << (anet::Int32)x << (anet::Int32)y;
            clientSock.send(posBuffer, addr);

            accTime = 0;
            std::cout << "Sent Position!\n";
        }
    }
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
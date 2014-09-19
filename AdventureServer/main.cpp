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
    std::unordered_map<unsigned int, GameClient> clients;
    anet::UdpSocket clientSock;

    // Bind to any available port.
    clientSock.bind(0);

    anet::NetAddress addr("10.0.0.6", 32002);
    anet::NetBuffer buffer;
    buffer << (anet::UInt16)50322 << (anet::UInt8)0;
    clientSock.send(buffer, addr);

    anet::NetBuffer conBuffer;
    anet::NetAddress serverAddr;
    clientSock.receive(conBuffer, serverAddr);

    anet::UInt16 protocolID;
    anet::Int8 messageID;
    bool confirm;

    conBuffer >> protocolID >> messageID >> confirm;

    if (!confirm)
    {
        std::cout << "Failed to connect!\n";
        clientSock.unBind();
        return;
    }

    Timer clientTimer;
    unsigned int accTime = 0;
    anet::Int32 x = 0, y = 0;

    clientSock.setBlocking(false);
    bool running = true;
    while (running)
    {
        accTime += clientTimer.Restart();
        if (accTime >= (1000 / 60))
        {
            x += 1;
            y += 2;

            anet::NetBuffer posBuffer;
            posBuffer << (anet::UInt16)50322 << (anet::UInt8)3 << (anet::Int32)x << (anet::Int32)y;
            clientSock.send(posBuffer, addr);

            accTime = 0;
            //std::cout << "Sent Position!\n";

            anet::NetBuffer msgBuffer;
            anet::NetAddress serverAddr;
            int bytesRecv = clientSock.receive(msgBuffer, serverAddr);

            if (bytesRecv > 0)
            {
                anet::UInt16 protocolID;
                msgBuffer >> protocolID;
                if (protocolID == 50322)
                {
                    anet::UInt8 messageID;
                    msgBuffer >> messageID;

                    switch (messageID)
                    {
                    case 1: // Disconnection
                    {
                        anet::UInt8 dcCode;
                        msgBuffer >> dcCode;
                        if (dcCode == 1) // A different client disconnected.
                        {
                            anet::UInt32 hash;
                            msgBuffer >> hash;

                            auto it = clients.find(hash);
                            if (it != clients.end())
                            {
                                std::cout << "Client (" << hash << ") disconnected.\n";
                                clients.erase(it);
                            }
                        }
                        else
                        {
                            std::cout << "Disconnected from server.";
                            running = false;
                        }
                    }
                        break;
                    case 2: // Client listing.
                    {
                        unsigned int hash;
                        int x, y, room;

                        msgBuffer >> hash >> x >> y >> room;

                        std::cout << "New client found! [" << hash << ", (" << x << ", " << y << "), " << room << "]\n";

                        GameClient newClient;
                        newClient.x = x;
                        newClient.y = y;
                        newClient.roomid = room;

                        clients.insert(std::pair<unsigned int, GameClient>(hash, newClient));

                        break;
                    }
                    case 3: // Position
                    {
                        unsigned int hash;
                        int x, y;

                        msgBuffer >> hash >> x >> y;

                        clients[hash].x = x;
                        clients[hash].y = y;

                        break;
                    }
                    }
                }
            }
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
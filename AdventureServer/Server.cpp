#include "Server.hpp"
#include "Timer.hpp"
#include <iostream>
#include <functional>
#include <chrono>

#include <WinSock2.h>

Server::Server()
{
    m_isStarted = false;
}

Server::~Server()
{
    if (m_serverThread.joinable())
        m_serverThread.join();
}

void Server::Start()
{
    if (m_isStarted == true)
        return;

    m_isStarted = true;
    m_serverThread = std::thread(&Server::RunThread, this);
}

unsigned int Server::AddClient(const anet::NetAddress& addr)
{
    GameClient client;
    client.m_address = addr;
    client.m_timeout = 0;

    unsigned int hash = ClientHashFunc(addr);
    std::cout << "Hash: " << hash << "\n";
    
    if (m_clients.insert(std::pair<unsigned int, GameClient>(hash, client)).second == false)
        return -1;

    return hash;
}

void Server::RemoveClient()
{

}

unsigned int Server::ClientHashFunc(const anet::NetAddress& addr)
{
    using std::size_t;
    using std::hash;
    using std::string;
    return std::hash<std::string>()(addr.getIP())
        ^ ((std::hash<anet::UInt16>()(addr.getPort()) << 1) >> 1);
}

void Server::RunThread()
{
    if (m_socket.bind(32002))
        std::cout << "Server Started!" << std::endl;
    else
    {
        std::cout << "Error: " << WSAGetLastError() << std::endl;
        m_isStarted = false;
    }

    m_socket.setBlocking(false);

    Timer serverTimer;
    // Message loop
    while (m_isStarted)
    {
        unsigned int timeElapsed = serverTimer.Restart();

        anet::NetBuffer buffer;
        anet::NetAddress addr;

        int bytesRecv = m_socket.receive(buffer, addr);
        if (bytesRecv > 0)
        {
            // Get the protocol ID.
            anet::UInt16 protID;
            buffer >> protID;

            if (protID == Server::PROTOCOL_ID)
            {
                // Who sent this packet? Find out.
                int clientHash = ClientHashFunc(addr);

                // Get what message was sent.
                anet::UInt8 messageID;
                buffer >> messageID;

                MessageType type = static_cast<MessageType>(messageID);

                // Reset the timeout of the client that sent the message (if possible).
                auto c = m_clients.find(clientHash);
                if (c != m_clients.end())
                {
                    c->second.m_timeout = 0;
                }
                else if (c == m_clients.end() && type != MessageType::Connection) // Unknown with protocol ID. Refuse connection
                {
                    continue;
                }

                // Action determined by the returned ID.
                switch (type)
                {
                case MessageType::Connection: // New login
                {
                    unsigned int hash = AddClient(addr);
                    if (hash != -1)
                        std::cout << "New Client (" << hash << ") joined! (" << addr.getIP() << ":" << addr.getPort() << ")\n";
                    else
                        std::cout << "Duplicate client. Connection refused.\n";

                    // Forward new arrival to other clients.

                    // Forward the client listing to the new client.

                    break;
                }
                case MessageType::Position: // Position Reporting
                {
                    anet::Int32 x, y;
                    buffer >> x >> y;
                    //std::cout << "Client (" << clientHash << ") moved to: (" << x << ", " << y << ")\n";
                    m_clients[clientHash].x = x;
                    m_clients[clientHash].y = y;

                    // Forward to other clients.

                    break;
                }
                case MessageType::Room: // Room changed
                {
                    // Get the new room id.
                    anet::Int32 roomid;
                    buffer >> roomid;

                    m_clients[clientHash].roomid = roomid;

                    // Forward to other clients.


                    break;
                }
                }
            }
            else
            {
                std::cout << "Invalid protocol!\n";
            }
        }
        else if (bytesRecv == 0)
        {
            std::cout << "No Data.\n";
        }
        else if (bytesRecv < 0)
        {
            if (WSAGetLastError() != WSAEWOULDBLOCK)
            {
                std::cout << "Error code: " << WSAGetLastError() << "\n";
            }
        }

        // Timeouts
        auto it = m_clients.begin();
        while (it != m_clients.end())
        {
            // Increment the timeout.
            (*it).second.m_timeout += timeElapsed;
            if ((*it).second.m_timeout >= Server::MAX_TIMEOUT) // Disconnection
            {
                // Send out the disconnect packet just incase the client is lagging.
                //m_socket.send()
                std::cout << "Client Timed out.\n";
                // Remove the client.
                it = m_clients.erase(it);
                continue;
            }
            // Advance forward.
            ++it;
        }
    }

    std::cout << "Server stopped.\n";
    // Unbind the socket.
    m_socket.unBind();
}

void Server::Stop()
{
    m_isStarted = false;
    if (m_serverThread.joinable())
        m_serverThread.join();
}
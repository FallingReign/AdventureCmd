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
    m_clients.insert(std::pair<unsigned int, GameClient>(hash, client));

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

                // Action determined by the returned ID.
                switch (messageID)
                {
                case 0: // New login
                    std::cout << "New Client joined! (" << addr.getIP() << ":" << addr.getPort() << ")\n";
                    AddClient(addr);
                    break;
                }

                // Reset the timeout of the client that sent the message (if possible).
                try
                {
                    auto& client = m_clients.at(clientHash);
                    client.m_timeout = 0;
                }
                catch (std::out_of_range ex)
                {
                }
            }
            else
            {
                std::cout << "Invalid protocol!\n";
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
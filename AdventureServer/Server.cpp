#include "Server.hpp"
#include "Timer.hpp"
#include <iostream>
#include <functional>
#include <algorithm>
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

int Server::ForwardToClients(anet::NetBuffer& inBuffer, unsigned int inExclude)
{
    int totalBytes = 0;
    auto c = m_clients.begin();
    while (c != m_clients.end())
    {
        if (inExclude == 0 || (*c).first != inExclude)
            totalBytes += m_socket.send(inBuffer, (*c).second.m_address);

        ++c;
    }
    return totalBytes;
}

unsigned int Server::ClientHashFunc(const anet::NetAddress& addr)
{
    using std::size_t;
    using std::hash;
    using std::string;
    return std::hash<std::string>()(addr.getIP())
        ^ ((std::hash<anet::UInt16>()(addr.getPort()) << 1) >> 1);
}

void Server::HandleConnection(const anet::NetAddress& addr)
{
    unsigned int hash = AddClient(addr);

    anet::NetBuffer connBuffer;
    connBuffer << Server::PROTOCOL_ID << (anet::UInt8)MessageType::Connection;

    if (hash != -1) // Connection Accepted.
    {
        // Send a positive response.
        std::cout << "New Client (" << hash << ") joined! (" << addr.getIP() << ":" << addr.getPort() << ")\n";
        connBuffer << true;
        m_socket.send(connBuffer, addr);

        auto& newClient = m_clients[hash];

        // Send a listing of the present clients and the new client.
        auto c = m_clients.begin();
        while (c != m_clients.end())
        {
            auto& cl = (*c).second;

            if ((*c).first != hash)
            {
                // Send the client data to the new client.
                anet::NetBuffer listingBuffer;
                listingBuffer << Server::PROTOCOL_ID << (anet::UInt8)MessageType::ClientListing << (*c).first << cl.x << cl.y << cl.roomid;
                m_socket.send(listingBuffer, addr);

                // Send the new client data to the existing one.
                anet::NetBuffer addBuffer;
                addBuffer << Server::PROTOCOL_ID << (anet::UInt8)MessageType::ClientListing << hash << newClient.x << newClient.y << newClient.roomid;
                m_socket.send(addBuffer, cl.m_address);
            }

            // Increment.
            ++c;
        }
    }
    else // Connection refused.
    {
        connBuffer << false;
        std::cout << "Duplicate client. Connection refused.\n";
    }
}

void Server::HandlePosition(unsigned int clientHash, short x, short y)
{
    //std::cout << "Handling position: " << x << ", " << y << "\n";

    // Update local information.
	auto& c = m_clients[clientHash];
	c.x = x;
    c.y = y;

    // Forward to other clients.
    anet::NetBuffer posBuffer;
    posBuffer << Server::PROTOCOL_ID << (anet::UInt8)MessageType::Position << clientHash << x << y;
    ForwardToClients(posBuffer, clientHash);
}

void Server::HandleRoomChange(unsigned int clientHash, int roomid)
{
    // Update local information.
    //m_clients[clientHash].roomid = roomid;

    // Forward to other clients.
    anet::NetBuffer roomBuffer;
    roomBuffer << Server::PROTOCOL_ID << (anet::UInt8)MessageType::Room << clientHash << roomid;
    ForwardToClients(roomBuffer, clientHash);
}

void Server::RunThread()
{
	//32002
    if (m_socket.bind(33309))
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
			std::cout << "Recv: " << bytesRecv << "\n";
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
                    HandleConnection(addr);
                    break;
                }
                case MessageType::Position: // Position Reporting
                {
                    anet::Int16 x, y;
                    buffer >> x >> y;
                    HandlePosition(clientHash, x, y);
					std::cout << "Got position: " << x << ", " << y << "\n";
                    break;
                }
                case MessageType::Room: // Room changed
                {
                    // Get the new room id.
                    anet::Int32 roomid;
                    buffer >> roomid;
                    HandleRoomChange(clientHash, roomid);
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
            int errorCode = WSAGetLastError();
            if (errorCode != WSAEWOULDBLOCK && errorCode != WSAECONNRESET)
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
                anet::NetBuffer disconBuffer;
                disconBuffer << Server::PROTOCOL_ID << (anet::UInt8)MessageType::Disconnection << (anet::UInt8)0;
                m_socket.send(disconBuffer, (*it).second.m_address);
                
                std::cout << "Client (" << (*it).first << ") Timed out.\n";

                // Broadcast disconnection to others.
                anet::NetBuffer forwardDisconBuffer;
                forwardDisconBuffer << Server::PROTOCOL_ID << (anet::UInt8)MessageType::Disconnection << (anet::UInt8)1 << (*it).first;
                ForwardToClients(forwardDisconBuffer, (*it).first);

                // Remove the client from the map.
                it = m_clients.erase(it);
                continue;
            }
            // Advance forward.
            ++it;
        }
    }

    // Server stopped. Send disconnect messages.
    anet::NetBuffer disconBuffer;
    disconBuffer << Server::PROTOCOL_ID << (anet::UInt8)MessageType::Disconnection << (anet::UInt8)0;
    ForwardToClients(disconBuffer);

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
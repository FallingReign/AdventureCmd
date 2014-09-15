#include "Server.hpp"

Server::Server()
{
    m_isStarted = false;
}

Server::~Server()
{

}

bool Server::Start()
{
    if (m_isStarted)
        return true;

    m_serverThread = std::thread(&Server::RunThread, this);

    return false;
}

void Server::RunThread()
{
    m_socket.bind(32002);
    while (m_isStarted)
    {
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
                anet::UInt8 messageID;
                buffer >> messageID;


            }
        }
    }
    m_socket.unBind();
}

void Server::Stop()
{
    if (!m_isStarted)
    {
        m_isStarted = false;
        m_serverThread.join();
    }
}
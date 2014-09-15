#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <thread>
#include <atomic>

#include <NetBuffer.hpp>
#include <UdpSocket.hpp>

#include "GameClient.hpp"

class Server
{
public:
    Server();
    ~Server();

    bool Start();
    void Stop();

private:
    void RunThread();

private:
    std::vector<GameClient> m_clients;
    anet::UdpSocket m_socket;
    unsigned int m_lastFree{ 0 };
    std::atomic_bool m_isStarted;
    std::thread m_serverThread;

    const static anet::UInt16 PROTOCOL_ID = 50322;
};

#endif
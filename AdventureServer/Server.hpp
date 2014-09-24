#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <unordered_map>
#include <thread>
#include <atomic>

#include <NetBuffer.hpp>
#include <UdpSocket.hpp>

#include "GameClient.hpp"

enum class MessageType
    : anet::UInt8
{
    Connection,
    Disconnection,
    ClientListing,
    Position,
    Room
};

class Server
{
public:
    Server();
    ~Server();

    void Start();
    void Stop();

private:
    unsigned int AddClient(const anet::NetAddress& addr);
    void RemoveClient();
    
    void HandleConnection(const anet::NetAddress& addr);
    void HandlePosition(unsigned int clientHash, short x, short y);
    void HandleRoomChange(unsigned int clientHash, int worldID, short zoneX, short zoneY);
    
    void RunThread();

    int ForwardToClients(anet::NetBuffer& inBuffer, unsigned int inExclude = 0);

    static unsigned int ClientHashFunc(const anet::NetAddress& addr);

private:
    std::unordered_map<unsigned int, GameClient> m_clients;

    anet::UdpSocket m_socket;
    unsigned int m_lastFree{ 0 };
    std::atomic_bool m_isStarted;
    std::thread m_serverThread;

    static const anet::UInt16 PROTOCOL_ID = 50322;
    static const unsigned int MAX_CLIENTS = 2000;
    static const unsigned int MAX_TIMEOUT = 400;
};

#endif
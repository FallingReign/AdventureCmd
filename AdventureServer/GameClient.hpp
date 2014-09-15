#ifndef GAMECLIENT_HPP
#define GAMECLIENT_HPP

#include <NetAddress.hpp>

class GameClient
{
public:
    anet::NetAddress m_address;
    unsigned int m_timeout{ 0 };
};

#endif
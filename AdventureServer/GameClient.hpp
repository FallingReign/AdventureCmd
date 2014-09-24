#ifndef GAMECLIENT_HPP
#define GAMECLIENT_HPP

#include <NetAddress.hpp>

class GameClient
{
public:
    anet::NetAddress m_address;
    unsigned int m_timeout{ 0 };
	short x{ 0 }, y{ 0 };
	int roomid{ 0 };

	int worldid{ 0 }, zoneX{ 0 }, zoneY{ 0 };
};

#endif
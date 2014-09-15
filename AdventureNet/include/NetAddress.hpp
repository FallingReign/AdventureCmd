#pragma once
#ifndef NETADDRESS_HPP
#define NETADDRESS_HPP

#include <string>
#include <NetDefs.hpp>

struct sockaddr_in;

namespace anet
{
    class NetAddress
    {
    public:
        NetAddress();
        NetAddress(const std::string& inAddress, UInt16 inPort);
        NetAddress(UInt32 inAddress, UInt16 inPort);

        UInt16 getPort() const;
        std::string getIP() const;

        sockaddr_in toAddress() const;
    private:
        UInt32 m_address;
        UInt16 m_port;

        static UInt32 resolve(const std::string& inAddress);
    };
}

#endif
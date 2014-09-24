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

        friend bool operator==(const NetAddress& lhs, const NetAddress& rhs);
        friend bool operator!=(const NetAddress& lhs, const NetAddress& rhs);
    private:
        UInt32 m_address;
        UInt16 m_port;

        static UInt32 resolve(const std::string& inAddress);
    };

    inline bool operator==(const NetAddress& lhs, const NetAddress& rhs)
    {
        return lhs.m_address == rhs.m_address && lhs.m_port == rhs.m_port;
    }

    inline bool operator!=(const NetAddress& lhs, const NetAddress& rhs)
    {
        return !(lhs == rhs);
    }
}

#endif
#ifndef NETSOCKET_HPP
#define NETSOCKET_HPP

#include <NetDefs.hpp>

namespace anet
{
    class NetSocket
    {
    public:
        enum class Type
        {
            TCP,
            UDP
        };

        NetSocket(Type type);
        virtual ~NetSocket();

        void setBlocking(bool block = true);

        UInt32 getSocketID() const;

    protected:
        void init();
        void uninit();

    private:
        Type m_type;
        UInt32 m_sockid;
    };
}

#endif
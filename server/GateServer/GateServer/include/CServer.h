#pragma once

#include "const.h"
#include "HttpConnection.h"
#include "ConfigMgr.h"
#include "AsioIOServicePool.h"

class CServer : public std::enable_shared_from_this<CServer>
{
public:
    CServer(asio::io_context &ioc, unsigned short &port);
    void Start();

private:
    tcp::acceptor _acceptor;
    asio::io_context &_ioc;
    tcp::socket _socket;
};

#include "CServer.h"

CServer::CServer(boost::asio::io_context &ioc, unsigned short &port)
    : _ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), port)), _socket(ioc)
{
}

void CServer::Start()
{
    // 监听新连接
    auto self = shared_from_this();
    auto &io_context = AsioIOServicePool::GetInstance()->GetIOService();
    std::shared_ptr<HttpConnection> new_con = std::make_shared<HttpConnection>((tcp::socket)io_context);
    _acceptor.async_accept(new_con->GetSocket(),
                           [self, new_con](beast::error_code ec)
                           {
                               try
                               {
                                   // 若出现问题则重新开始，放弃这次连接
                                   if (ec)
                                   {
                                       self->Start();
                                       return;
                                   }
                                   // 处理新连接
                                   new_con->Start();
                                   // 继续监听
                                   self->Start();
                               }
                               catch (std::exception &exp)
                               {
                                   std::cout << "exception is " << exp.what() << std::endl;
                                   self->Start();
                               }
                           });
}
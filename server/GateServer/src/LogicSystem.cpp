#include "LogicSystem.h"
#include "HttpConnection.h"

LogicSystem::LogicSystem()
{
    RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection)
           {
               beast::ostream(connection->_response.body()) << "receive get_test req"<< std::endl;
               int i = 0;
               for (auto &elem : connection->_get_params)
               {
                   i++;
                   beast::ostream(connection->_response.body()) << "param" << i << " key is " << elem.first;
                   beast::ostream(connection->_response.body()) << ", " << " value is " << elem.second << std::endl;
               } });
}

LogicSystem::~LogicSystem()
{
}

bool LogicSystem::HandleGet(std::string url, std::shared_ptr<HttpConnection> conn)
{
    if (_get_handlers.find(url) == _get_handlers.end())
    {
        // 未找到对应的方法
        return false;
    }

    _get_handlers[url](conn);
    return true;
}

void LogicSystem::RegGet(std::string url, HttpHandler handler)
{
    // 添加url回调函数
    _get_handlers.insert(make_pair(url, handler));
}

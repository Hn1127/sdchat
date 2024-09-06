#pragma once

#include "const.h"
#include "Singleton.h"
#include <functional>
#include <map>

class HttpConnection;
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;

class LogicSystem : public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;

public:
    ~LogicSystem();
    bool HandleGet(std::string url, std::shared_ptr<HttpConnection> conn);
    void RegGet(std::string url, HttpHandler handler);

private:
    LogicSystem();
    // POST的方法
    std::map<std::string, HttpHandler> _post_handlers;
    // GET的方法
    std::map<std::string, HttpHandler> _get_handlers;
};
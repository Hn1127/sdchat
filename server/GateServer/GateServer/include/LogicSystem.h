#pragma once

#include "const.h"
#include "Singleton.h"
#include "VarifyGrpcClient.h"
#include "RedisMgr.h"
#include "MysqlMgr.h"
#include <functional>
#include <map>

class HttpConnection;
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;

class LogicSystem : public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;

public:
    ~LogicSystem();
    // 处理GET请求
    bool HandleGet(std::string url, std::shared_ptr<HttpConnection> conn);
    // 处理POST请求
    bool HandlePost(std::string url, std::shared_ptr<HttpConnection> conn);
    // 添加GET方法
    void RegGet(std::string url, HttpHandler handler);
    // 添加POST方法
    void RegPost(std::string url, HttpHandler handler);

private:
    LogicSystem();
    // GET的方法
    std::map<std::string, HttpHandler> _get_handlers;
    // POST的方法
    std::map<std::string, HttpHandler> _post_handlers;
};
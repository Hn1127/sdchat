#pragma once

#include <unordered_map>
#include "const.h"
#include "LogicSystem.h"

class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
    friend class LogicSystem;

public:
    HttpConnection(tcp::socket socket);
    void Start();

private:
    // 处理读好的请求
    void HandleReq();
    // 解析GET方法的参数
    void PreParseGetParam();
    // 将设置完毕的响应发送出去
    void WriteResponse();
    // 启动定时器,一定时间后关闭socket
    void CheckDeadline();

private:
    // 请求的url
    std::string _get_url;
    // get方法url的参数
    std::unordered_map<std::string, std::string> _get_params;

    // 网络连接套件
    tcp::socket _socket;
    beast::flat_buffer _buffer{8192};
    http::request<http::dynamic_body> _request;
    http::response<http::dynamic_body> _response;
    // 定时器
    asio::steady_timer deadline_{
        _socket.get_executor(), std::chrono::seconds(60)};
};
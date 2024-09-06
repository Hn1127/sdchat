#include "HttpConnection.h"

HttpConnection::HttpConnection(tcp::socket socket)
    : _socket(std::move(socket))
{
}

void HttpConnection::Start()
{
    auto self = shared_from_this();
    http::async_read(_socket, _buffer, _request,
                     [self](beast::error_code ec, std::size_t bytes_transferred)
                     {
                         // 读完毕后处理读到的数据
                         try
                         {
                             if (ec)
                             {
                                 std::cout << "http read err is " << ec.what() << std::endl;
                                 return;
                             }

                             // 处理读到的数据
                             boost::ignore_unused(bytes_transferred);
                             self->HandleReq();
                             self->CheckDeadline();
                         }
                         catch (std::exception &exp)
                         {
                             std::cout << "exception is " << exp.what() << std::endl;
                         }
                     });
}

void HttpConnection::HandleReq()
{
    // 处理请求
    // 设置版本
    _response.version(_request.version());
    // 非持久连接
    _response.keep_alive(false);

    if (_request.method() == http::verb::get)
    {
        // GET方法
        PreParseGetParam();
        bool success = LogicSystem::GetInstance()->HandleGet(_get_url, shared_from_this());
        if (!success)
        {
            // 处理GET失败
            _response.result(http::status::not_found);
            _response.set(http::field::content_type, "text/plain");
            beast::ostream(_response.body()) << "url not found!\r\n";
            WriteResponse();
            return;
        }

        _response.result(http::status::ok);
        _response.set(http::field::server, "GateServer");
        WriteResponse();
        return;
    }
}

void HttpConnection::PreParseGetParam()
{
    // 提取请求的URL
    auto url = _request.target();
    // 找到参数起始位置
    auto query_pos = url.find('?');
    if (query_pos == std::string::npos)
    {
        // 无参数
        _get_url = url;
        return;
    }

    // 提取真正的url
    _get_url = url.substr(0, query_pos);
    std::string query_string = url.substr(query_pos + 1);
    std::string key;
    std::string value;
    while ((query_pos = query_string.find('&')) != std::string::npos)
    {
        // pair = "key=value"
        auto pair = query_string.substr(0, query_pos);
        auto eq_pos = pair.find('=');
        if (eq_pos != std::string::npos)
        {
            key = pair.substr(0, eq_pos);
            value = pair.substr(eq_pos + 1);
            _get_params[key] = value;
        }
        query_string.erase(0, query_pos + 1);
    }
    // 处理最后一个参数对
    if (!query_string.empty())
    {
        size_t eq_pos = query_string.find('=');
        if (eq_pos != std::string::npos)
        {
            key = query_string.substr(0, eq_pos);
            value = query_string.substr(eq_pos + 1);
            _get_params[key] = value;
        }
    }
}

void HttpConnection::WriteResponse()
{
    // 处理响应
    auto self = shared_from_this();
    _response.content_length(_response.body().size());
    // http是短连接,发送完毕后直接断开发送端
    http::async_write(_socket, _response,
                      [self](beast::error_code ec, std::size_t)
                      {
                          self->_socket.shutdown(tcp::socket::shutdown_send, ec);
                          self->deadline_.cancel();
                      });
}

void HttpConnection::CheckDeadline()
{
    // 检测超时
    auto self = shared_from_this();

    deadline_.async_wait(
        [self](beast::error_code ec)
        {
            if (!ec)
            {
                self->_socket.close(ec);
            }
        });
}

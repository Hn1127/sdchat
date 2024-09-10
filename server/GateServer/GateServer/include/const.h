#pragma once

// boost
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
// jsonapp
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <iostream>

enum ErrorCodes
{
    Success = 0,
    Error_Json = 1001,  // Json解析错误
    RPCFailed = 1002,   // RPC请求错误
    VarifyExpired = 1003,      // 无对应验证码
    VarifyCodeErr = 1004,  // 验证码错误
    UserExist = 1005,    // 用户名已存在
};

namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace asio = boost::asio;     // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
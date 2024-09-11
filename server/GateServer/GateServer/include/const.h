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
    RPCFailed,   // RPC请求错误
    VarifyExpired,      // 无对应验证码
    VarifyCodeErr,  // 验证码错误
    UserExist,    // 用户名已存在
    EmailExist, // 邮箱已存在
    EmailNotMatch,   // 邮箱与用户名不匹配
    PasswdErr, // 密码错误
    PasswdUpFailed, // 密码更新失败
    ServerErr = 2001,    // 服务器错误
};

namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace asio = boost::asio;     // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
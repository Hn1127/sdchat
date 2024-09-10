#include "LogicSystem.h"
#include "HttpConnection.h"

LogicSystem::LogicSystem()
{
    // GET: /get_test
    RegGet("/get_test",
           [](std::shared_ptr<HttpConnection> conn)
           {
               beast::ostream(conn->_response.body()) << "receive get_test req" << std::endl;
               int i = 0;
               for (auto &elem : conn->_get_params)
               {
                   i++;
                   beast::ostream(conn->_response.body()) << "param" << i << " key is " << elem.first;
                   beast::ostream(conn->_response.body()) << ", " << " value is " << elem.second << std::endl;
               }
           });
    // POST: /get_varifycode
    RegPost("/get_varifycode",
            [](std::shared_ptr<HttpConnection> conn)
            {
                // 获取请求的消息体内容
                auto body_str = beast::buffers_to_string(conn->_request.body().data());
                conn->_response.set(http::field::content_type, "text/json");
                Json::Value root;
                Json::Reader reader;
                Json::Value src_root;
                // 解析消息体
                bool parse_success = reader.parse(body_str, src_root);
                // 解析是否成功
                if (!parse_success)
                {
                    std::cout << "Failed to parse JSON data!" << std::endl;
                    root["error"] = ErrorCodes::Error_Json;
                }
                else
                {
                    auto email = src_root["email"].asString();
                    std::cout << "email is " << email << std::endl;
                    GetVarifyRsp rsp = VarifyGrpcClient::GetInstance()->GetVarifyCode(email);
                    root["email"] = email;
                    root["error"] = rsp.error();
                }
                std::string jsonstr = root.toStyledString();
                beast::ostream(conn->_response.body()) << jsonstr;
                return true;
            });
    // POST: /user_register
    RegPost("/user_register",
        [](std::shared_ptr<HttpConnection> conn)
        {
            // 获取请求的消息体内容
            auto body_str = beast::buffers_to_string(conn->_request.body().data());
            conn->_response.set(http::field::content_type, "text/json");
            Json::Value root;
            Json::Reader reader;
            Json::Value src_root;
            // 解析消息体
            bool parse_success = reader.parse(body_str, src_root);
            // 解析是否成功
            if (!parse_success)
            {
                std::cout << "Failed to parse JSON data!" << std::endl;
                root["error"] = ErrorCodes::Error_Json;
                std::string jsonstr = root.toStyledString();
                beast::ostream(conn->_response.body()) << jsonstr;
                return true;
            }
            // 查找Redis中验证码是否一致
            std::string varify_code;
            bool b_get_varify = RedisMgr::GetInstance()->Get(src_root["email"].asString(), varify_code);
            // 不存在对应的验证码
            if (!b_get_varify) {
                std::cout << " get varify code expired" << std::endl;
                root["error"] = ErrorCodes::VarifyExpired;
                std::string jsonstr = root.toStyledString();
                beast::ostream(conn->_response.body()) << jsonstr;
                return true;
            }
            // 验证码不一致
            if (varify_code != src_root["varifycode"].asString()) {
                std::cout << " varify code error" << std::endl;
                root["error"] = ErrorCodes::VarifyCodeErr;
                std::string jsonstr = root.toStyledString();
                beast::ostream(conn->_response.body()) << jsonstr;
                return true;
            }
            
            // 判断用户是否已经存在
            // 首先访问redis查找
            bool b_usr_exist = RedisMgr::GetInstance()->ExistsKey(src_root["user"].asString());
            if (b_usr_exist) {
                std::cout << " user exist" << std::endl;
                root["error"] = ErrorCodes::UserExist;
                std::string jsonstr = root.toStyledString();
                beast::ostream(conn->_response.body()) << jsonstr;
                return true;
            }

            // 继续查找数据库判断用户是否存在
            root["error"] = 0;
            root["email"] = src_root["email"];
            root["user"] = src_root["user"].asString();
            root["passwd"] = src_root["passwd"].asString();
            root["confirm"] = src_root["confirm"].asString();
            root["varifycode"] = src_root["varifycode"].asString();
            std::string jsonstr = root.toStyledString();
            beast::ostream(conn->_response.body()) << jsonstr;
            return true;
        });
}

LogicSystem::~LogicSystem()
{
}

bool LogicSystem::HandleGet(std::string url, std::shared_ptr<HttpConnection> conn)
{
    std::cout << "get GET request:" << url << std::endl;
    if (_get_handlers.find(url) == _get_handlers.end())
    {
        // 未找到对应的方法
        return false;
    }

    _get_handlers[url](conn);
    return true;
}

bool LogicSystem::HandlePost(std::string url, std::shared_ptr<HttpConnection> conn)
{
    if (_post_handlers.find(url) == _post_handlers.end())
    {
        // 未找到对应的方法
        return false;
    }

    _post_handlers[url](conn);
    return true;
}

void LogicSystem::RegGet(std::string url, HttpHandler handler)
{
    // 添加url回调函数
    _get_handlers.insert(make_pair(url, handler));
}

void LogicSystem::RegPost(std::string url, HttpHandler handler)
{
    // 添加url回调函数
    _post_handlers.insert(make_pair(url, handler));
}
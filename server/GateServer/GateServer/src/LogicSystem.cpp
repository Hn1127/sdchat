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

            // 解析获得基本信息
            auto email = src_root["email"].asString();
            auto name = src_root["user"].asString();
            auto pwd = src_root["passwd"].asString();
            auto confirm = src_root["confirm"].asString();

            // 再次验证密码和确认密码
            if (pwd != confirm) {
                std::cout << "password err " << std::endl;
                root["error"] = ErrorCodes::PasswdErr;
                std::string jsonstr = root.toStyledString();
                beast::ostream(conn->_response.body()) << jsonstr;
                return true;
            }

            // 查找Redis中验证码是否一致
            std::string varify_code;
            bool b_get_varify = RedisMgr::GetInstance()->Get(src_root["email"].asString(), varify_code);
            // 不存在对应的验证码
            if (!b_get_varify) {
                std::cout << "get varify code expired" << std::endl;
                root["error"] = ErrorCodes::VarifyExpired;
                std::string jsonstr = root.toStyledString();
                beast::ostream(conn->_response.body()) << jsonstr;
                return true;
            }
            // 验证码不一致
            if (varify_code != src_root["varifycode"].asString()) {
                std::cout << "varify code error" << std::endl;
                root["error"] = ErrorCodes::VarifyCodeErr;
                std::string jsonstr = root.toStyledString();
                beast::ostream(conn->_response.body()) << jsonstr;
                return true;
            }

            // 判断用户是否已经存在
            // 首先访问redis查找
            bool b_usr_exist = RedisMgr::GetInstance()->ExistsKey(src_root["user"].asString());
            if (b_usr_exist) {
                std::cout << "user exist" << std::endl;
                root["error"] = ErrorCodes::UserExist;
                std::string jsonstr = root.toStyledString();
                beast::ostream(conn->_response.body()) << jsonstr;
                return true;
            }

            // 继续查找数据库判断用户是否存在
            int uid = MysqlMgr::GetInstance()->RegUser(name, email, pwd);
            if (uid == -1) {
                std::cout << "mysql error" << std::endl;
                root["error"] = ErrorCodes::ServerErr;
                std::string jsonstr = root.toStyledString();
                beast::ostream(conn->_response.body()) << jsonstr;
                return true;
            }
            if (uid == -11) {
                std::cout << "user exist" << std::endl;
                root["error"] = ErrorCodes::UserExist;
                std::string jsonstr = root.toStyledString();
                beast::ostream(conn->_response.body()) << jsonstr;
                return true;
            }
            if (uid == -12) {
                std::cout << "email exist" << std::endl;
                root["error"] = ErrorCodes::EmailExist;
                std::string jsonstr = root.toStyledString();
                beast::ostream(conn->_response.body()) << jsonstr;
                return true;
            }

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
    
    //重置回调逻辑
        RegPost("/reset_pwd",
            [](std::shared_ptr<HttpConnection> connection)
            {
                auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
                std::cout << "receive body is " << body_str << std::endl;
                connection->_response.set(http::field::content_type, "text/json");
                Json::Value root;
                Json::Reader reader;
                Json::Value src_root;
                bool parse_success = reader.parse(body_str, src_root);
                if (!parse_success) {
                    std::cout << "Failed to parse JSON data!" << std::endl;
                    root["error"] = ErrorCodes::Error_Json;
                    std::string jsonstr = root.toStyledString();
                    beast::ostream(connection->_response.body()) << jsonstr;
                    return true;
                }

                auto email = src_root["email"].asString();
                auto name = src_root["user"].asString();
                auto pwd = src_root["passwd"].asString();

                //先查找redis中email对应的验证码是否合理
                std::string  varify_code;
                bool b_get_varify = RedisMgr::GetInstance()->Get(src_root["email"].asString(), varify_code);
                if (!b_get_varify) {
                    // 不存在对应验证码
                    std::cout << " get varify code expired" << std::endl;
                    root["error"] = ErrorCodes::VarifyExpired;
                    std::string jsonstr = root.toStyledString();
                    beast::ostream(connection->_response.body()) << jsonstr;
                    return true;
                }

                if (varify_code != src_root["varifycode"].asString()) {
                    // 验证码错误
                    std::cout << " varify code error" << std::endl;
                    root["error"] = ErrorCodes::VarifyCodeErr;
                    std::string jsonstr = root.toStyledString();
                    beast::ostream(connection->_response.body()) << jsonstr;
                    return true;
                }

                //查询数据库判断用户名和邮箱是否匹配
                bool email_valid = MysqlMgr::GetInstance()->CheckEmail(name, email);
                if (!email_valid) {
                    // 用户名和邮箱不匹配
                    std::cout << " user email not match" << std::endl;
                    root["error"] = ErrorCodes::EmailNotMatch;
                    std::string jsonstr = root.toStyledString();
                    beast::ostream(connection->_response.body()) << jsonstr;
                    return true;
                }

                //更新密码为最新密码
                bool b_up = MysqlMgr::GetInstance()->UpdatePwd(name, pwd);
                if (!b_up) {
                    // 更新密码失败
                    std::cout << " update pwd failed" << std::endl;
                    root["error"] = ErrorCodes::PasswdUpFailed;
                    std::string jsonstr = root.toStyledString();
                    beast::ostream(connection->_response.body()) << jsonstr;
                    return true;
                }

                std::cout << "succeed to update password" << pwd << std::endl;
                root["error"] = 0;
                root["email"] = email;
                root["user"] = name;
                root["passwd"] = pwd;
                root["varifycode"] = src_root["varifycode"].asString();
                std::string jsonstr = root.toStyledString();
                beast::ostream(connection->_response.body()) << jsonstr;
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
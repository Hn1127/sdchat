#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>
#include <functional>
#include "QStyle"

// 服务器url路径
extern QString gate_url_prefix;

// 更新窗口动态属性
extern std::function<void(QWidget*)> repolish;

// http请求类型
enum ReqId {
    ID_GET_VARIFY_CODE = 1001, // 获取验证码
    ID_REG_USER = 1002,        // 注册用户
    ID_RESET_USER = 1003,      // 重置用户
};

// Lable状态
enum ClickLbState {
    Normal = 0,  // 普通状态
    Selected = 1 // 选中状态
};

// 错误代码信息
enum ErrorCodes {
    SUCCESS = 0,
    ERR_JSON = 1,      // Json解析失败
    ERR_NETWORK = 2,   // 网络错误
    Error_Json = 1001, // Json解析错误
    RPCFailed,         // RPC请求错误
    VarifyExpired,     // 无对应验证码
    VarifyCodeErr,     // 验证码错误
    UserExist,         // 用户名已存在
    EmailExist,        // 邮箱已存在
    EmailNotMatch,     // 邮箱与用户名不匹配
    PasswdErr,         // 密码错误
    PasswdUpFailed,    // 密码更新失败
    ServerErr = 2001,  // 服务器错误
};

// http请求属于的模块
enum Modules {
    REGISTERMOD = 0, // 注册模块
    LOGINMOD = 1,    // 登录模块
    RESETMOD = 2,    // 重置模块

};

#endif // GLOBAL_H

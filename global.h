#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>
#include <functional>
#include "QStyle"

// 更新窗口动态属性
extern std::function<void(QWidget*)> repolish;

// http请求类型
enum ReqId{
    ID_GET_VARIFY_CODE = 1001, //获取验证码
    ID_REG_USER = 1002, //注册用户
};

// 错误代码信息
enum ErrorCodes{
    SUCCESS = 0,
    ERR_JSON = 1, //Json解析失败
    ERR_NETWORK = 2,
};

// http请求属于的模块
enum Modules{
    REGISTERMOD = 0,
};

#endif // GLOBAL_H

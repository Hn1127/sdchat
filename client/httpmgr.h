#ifndef HTTPMGR_H
#define HTTPMGR_H

#include "singleton.h"
#include "global.h"
#include <QString>
#include <QUrl>
#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <memory>
#include <QJsonObject>
#include <QJsonDocument>

class HttpMgr:public QObject,public Singleton<HttpMgr>,
                public enable_shared_from_this<HttpMgr>
{
    Q_OBJECT
    friend class Singleton<HttpMgr>;

public:
    ~HttpMgr() = default;

public:
    // 指定模块请求指定url,携带json数据,请求编号为req_id
    void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);

private:
    HttpMgr();
    QNetworkAccessManager _manager;

signals:
    // http请求结束信号
    void sig_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
    // 发送给Register模块的完成信号
    void sig_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
    // 发送给Login模块的完成信号
    void sig_login_mod_finish(ReqId id, QString res, ErrorCodes err);
    // 发送给Reset模块的完成信号
    void sig_reset_mod_finish(ReqId id, QString res, ErrorCodes err);

private:
    // 接收sig_http_finish信号
    void slot_http_finish(ReqId id,QString res,ErrorCodes err,Modules mod);
};

#endif // HTTPMGR_H

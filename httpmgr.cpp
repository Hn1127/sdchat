#include "httpmgr.h"

HttpMgr::HttpMgr(){
    // 连接http请求完成信号
    connect(this,&HttpMgr::sig_http_finish,this,&HttpMgr::slot_http_finish);
}

void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod){
    // 创建一个POST请求,设置请求头和请求体
    QByteArray data = QJsonDocument(json).toJson();
    // 通过url构造请求
    QNetworkRequest request(url);
    // 指定消息体为json格式
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    // 指定消息体长度
    request.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(data.length()));
    // 发送请求
    QNetworkReply *reply=_manager.post(request,data);

    // 获取自身shared_ptr以构建伪闭包
    auto self = shared_from_this();
    QObject::connect(reply,&QNetworkReply::finished,[reply,self,req_id,mod](){
        QString res = "";
        bool success = true;    // 记录请求是否成功
        // 检查是否出现错误
        if(reply->error()==QNetworkReply::NoError){
            // 无错误则读取请求
            res = reply->readAll();
        }
        else{
            // 处理错误情况
            qDebug() << reply->errorString();
            success=false;
        }
        // 发送信号通知完成
        emit self->sig_http_finish(req_id, res, success?ErrorCodes::SUCCESS:ErrorCodes::ERR_NETWORK,mod);
        reply->deleteLater();
        return ;
    });
}

void HttpMgr::slot_http_finish(ReqId id,QString res,ErrorCodes err,Modules mod){
    // 根据mod发送给出不同的信号
    if(mod == Modules::REGISTERMOD){
        //发送信号通知指定模块http响应结束
        emit sig_reg_mod_finish(id, res, err);
    }
}

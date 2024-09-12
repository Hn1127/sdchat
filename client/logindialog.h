#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QJsonObject>
#include "global.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

signals:
    void switchRegister();
    void switchReset();

private:
    // 登录
    void on_login_btn_clicked();
    // HttpMgr完成登录请求的登录回包信号
    void slot_login_mod_finish(ReqId id, QString res, ErrorCodes err);

private:
    // 检查输入
    bool checkUserValid();
    bool checkPassValid();

    // 初始化回包逻辑
    void initHttpHandlers();

    // 启用/禁用窗口
    void startWindow();
    void stopWindow();

    // 信息显示
    void showTip(QString msg, bool ok);

private:
    QMap<ReqId, std::function<void(const QJsonObject &)>> _handlers;
    bool pass_vis;

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H

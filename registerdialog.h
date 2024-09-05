#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QRegularExpression>
#include "global.h"
#include "ui_registerdialog.h"
#include "httpmgr.h"

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

signals:
    void switchLogin();

private:
    // 处理HttpMgr的sig_reg_mod_finish信号
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
    // 点击"获取验证码"btn
    void on_get_code_clicked();
    // 显示信息
    void showTip(QString str,bool ok);
    // 初始化handlers
    void initHttpHandlers();

private:
    // 对不同的ReqId有不同的handler
    // 处理HTTP请求结果
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;

private:
    Ui::RegisterDialog *ui;
};

#endif // REGISTERDIALOG_H

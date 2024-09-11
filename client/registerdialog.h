#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QRegularExpression>
#include "global.h"
#include "ui_registerdialog.h"
#include <qtimer.h>

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

    void stopTimer();

signals:
    void switchLogin();

private:
    // 处理HttpMgr的sig_reg_mod_finish信号
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
    // 点击"获取验证码"btn
    void on_get_code_clicked();
    // 点击"注册"按钮
    void on_sure_btn_clicked();

private:
    // 显示信息
    void showTip(QString str,bool ok);

    // 初始化handlers
    void initHttpHandlers();

    // 检查各输入
    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkConfirmValid();
    bool checkVarifyValid();

    // 启用/禁用窗口
    void startWindow();
    void stopWindow();

    // 切换密码
    void changeVisible();

private:
    // 对不同的ReqId有不同的handler
    // 处理HTTP请求结果
    QMap<ReqId, std::function<void(const QJsonObject &)>> _handlers;
    QTimer *_codeTimer;
    int _codeCount;
    bool pass_vis;
    bool confirm_vis;

private:
    Ui::RegisterDialog *ui;
};

#endif // REGISTERDIALOG_H

#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <QDialog>
#include <QJsonObject>
#include <QRegularExpression>
#include "global.h"
#include "httpmgr.h"
#include <qtimer.h>

namespace Ui {
class ResetDialog;
}

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);
    ~ResetDialog();

private:
    // 处理HttpMgr的sig_reg_mod_finish信号
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
    // 获取验证码
    void on_get_code_clicked();
    // 重置密码
    void on_reset_code_clicked();

private:
    // 信息提示
    void showTip(QString msg, bool ok);

    // 初始化handlers
    void initHttpHandlers();

    // 检查各输入
    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkVarifyValid();

    // 启用/禁用窗口
    void startWindow();
    void stopWindow();

signals:
    void switchLogin();

private:
    QTimer *_codeTimer;
    int _codeCount;
    QMap<ReqId, std::function<void(const QJsonObject &)>> _handlers;
    bool pass_vis;

private:
    Ui::ResetDialog *ui;
};

#endif // RESETDIALOG_H

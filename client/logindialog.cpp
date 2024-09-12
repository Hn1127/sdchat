#include "logindialog.h"
#include "httpmgr.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    // 注册
    connect(ui->btn_reg, &QPushButton::clicked, this, &LoginDialog::switchRegister);
    // 重置密码
    connect(ui->btn_reset, &QPushButton::clicked, this, &LoginDialog::switchReset);
    // 登录
    connect(ui->btn_log, &QPushButton::clicked, this, &LoginDialog::on_login_btn_clicked);

    // 登录回包信号
    connect(HttpMgr::GetInstance().get(),
            &HttpMgr::sig_login_mod_finish,
            this,
            &LoginDialog::slot_login_mod_finish);

    // 密码可视化
    connect(ui->pass_visible, &QPushButton::clicked, this, [this]() {
        pass_vis = !pass_vis;
        ui->edit_pass->setEchoMode(pass_vis == true ? QLineEdit::Normal : QLineEdit::Password);
    });

    // 初始化回包逻辑
    initHttpHandlers();
}

void LoginDialog::on_login_btn_clicked()
{
    if (checkUserValid() && checkPassValid()) {
        auto user = ui->edit_user->text();
        auto pwd = ui->edit_pass->text();
        //发送http请求登录
        QJsonObject json_obj;
        json_obj["user"] = user;
        json_obj["passwd"] = pwd;
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/user_login"),
                                            json_obj,
                                            ReqId::ID_LOGIN_USER,
                                            Modules::LOGINMOD);
        // 禁用窗口
        showTip("登陆中...", true);
        stopWindow();
    }
}

void LoginDialog::slot_login_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    startWindow();
    if (err != ErrorCodes::SUCCESS) {
        showTip(tr("网络请求错误"), false);
        return;
    }

    // 解析 JSON 字符串,res需转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    //json解析错误
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        showTip(tr("json解析错误"), false);
        return;
    }

    //调用对应的逻辑,根据id回调。
    _handlers[id](jsonDoc.object());

    return;
}

bool LoginDialog::checkUserValid()
{
    auto user = ui->edit_user->text();
    if (user.isEmpty()) {
        showTip("用户名不能为空", false);
        return false;
    }
    return true;
}

bool LoginDialog::checkPassValid()
{
    auto pwd = ui->edit_pass->text();
    if (pwd.isEmpty()) {
        showTip("请输入密码", false);
        return false;
    }
    return true;
}

void LoginDialog::initHttpHandlers()
{
    //注册获取登录回包逻辑
    _handlers.insert(ReqId::ID_LOGIN_USER, [this](QJsonObject jsonObj) {
        int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            showTip(tr("参数错误"), false);
            return;
        }
        auto user = jsonObj["user"].toString();
        showTip(tr("登录成功"), true);
        qDebug() << "user is " << user;
    });
}

void LoginDialog::startWindow()
{
    QList<QWidget *> widgets = this->findChildren<QWidget *>();
    foreach (QWidget *widget, widgets) {
        widget->setEnabled(true);
    }
}

void LoginDialog::stopWindow()
{
    QList<QWidget *> widgets = this->findChildren<QWidget *>();
    foreach (QWidget *widget, widgets) {
        widget->setEnabled(false);
    }
    ui->label_icon->setEnabled(true);
}

void LoginDialog::showTip(QString msg, bool ok)
{
    if (ok) {
        ui->err_tip->setProperty("state", "normal");
    } else {
        ui->err_tip->setProperty("state", "error");
    }
    ui->err_tip->setText(msg);
    repolish(ui->err_tip);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

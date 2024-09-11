#include "resetdialog.h"
#include "ui_resetdialog.h"

ResetDialog::ResetDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ResetDialog)
    , pass_vis(false)
    , _codeCount(60)
{
    ui->setupUi(this);

    _codeTimer = new QTimer();

    // 返回
    connect(ui->btn_cancel, &QPushButton::clicked, this, &ResetDialog::switchLogin);
    // 验证码
    connect(ui->btn_getCode, &QPushButton::clicked, this, &ResetDialog::on_get_code_clicked);
    connect(_codeTimer, &QTimer::timeout, this, [this]() {
        // 定时器设置的时间
        ui->btn_getCode->setText(QString::number(--_codeCount));
        if (_codeCount <= 0) {
            startWindow();
            // 定时时间到，将button设为可用
            ui->btn_getCode->setEnabled(true);
            ui->btn_getCode->setText("获取验证码");
            _codeCount = 60;
            _codeTimer->stop();
        }
    });
    // 重置
    connect(ui->btn_reset, &QPushButton::clicked, this, &ResetDialog::on_reset_code_clicked);

    // 检查各个输入框的信息
    connect(ui->edit_user, &QLineEdit::editingFinished, this, &ResetDialog::checkUserValid);

    connect(ui->edit_email, &QLineEdit::editingFinished, this, &ResetDialog::checkEmailValid);

    connect(ui->edit_pass, &QLineEdit::editingFinished, this, &ResetDialog::checkPassValid);

    connect(ui->edit_code, &QLineEdit::editingFinished, this, &ResetDialog::checkVarifyValid);

    // 接收http请求结果
    connect(HttpMgr::GetInstance().get(),
            &HttpMgr::sig_reset_mod_finish,
            this,
            &ResetDialog::slot_reg_mod_finish);

    // 密码可视化
    connect(ui->pass_visible, &QPushButton::clicked, this, [this]() {
        pass_vis = !pass_vis;
        ui->edit_pass->setEchoMode(pass_vis == true ? QLineEdit::Normal : QLineEdit::Password);
    });

    initHttpHandlers();
}

bool ResetDialog::checkUserValid()
{
    if (this->ui->edit_user->text() == "") {
        showTip("用户名不能为空", false);
        return false;
    }
    return true;
}

bool ResetDialog::checkEmailValid()
{
    auto email = ui->edit_email->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if (!match) {
        //提示邮箱不正确
        showTip("邮箱地址不正确", false);
        return false;
    }
    return true;
}

bool ResetDialog::checkPassValid()
{
    auto pass = ui->edit_pass->text();
    if (pass.length() < 6 || pass.length() > 15) {
        //提示长度不准确
        showTip(tr("密码长度应为6~15"), false);
        return false;
    }
    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
    bool match = regExp.match(pass).hasMatch();
    if (!match) {
        //提示字符非法
        showTip(tr("密码长度应为6~15"), false);
        return false;
    }
    return true;
}

bool ResetDialog::checkVarifyValid()
{
    auto pass = ui->edit_code->text();
    if (pass.isEmpty()) {
        showTip(tr("验证码不能为空"), false);
        return false;
    }
    return true;
}

void ResetDialog::startWindow()
{
    QList<QWidget *> widgets = this->findChildren<QWidget *>();
    foreach (QWidget *widget, widgets) {
        widget->setEnabled(true);
    }
}

void ResetDialog::stopWindow()
{
    QList<QWidget *> widgets = this->findChildren<QWidget *>();
    foreach (QWidget *widget, widgets) {
        widget->setEnabled(false);
    }
}

ResetDialog::~ResetDialog()
{
    delete _codeTimer;
    delete ui;
}

void ResetDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    showTip("", true);
    startWindow();
    // 处理HTTP请求结果
    if (err != ErrorCodes::SUCCESS) {
        showTip("网络请求错误", false);
        _codeCount = 0;
        ui->btn_getCode->setText("获取验证码");
        return;
    }

    // 解析JSON字符串
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    // json为空
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        showTip(tr("json解析错误"), false);
        _codeCount = 0;
        ui->btn_getCode->setText("获取验证码");
        return;
    }

    switch (jsonDoc["error"].toInt()) {
    case ErrorCodes::VarifyExpired: {
        // 验证码过期
        showTip(tr("验证码已过期"), false);
        _codeCount = 0;
        ui->btn_getCode->setText("获取验证码");
        break;
    }
    case ErrorCodes::VarifyCodeErr: {
        // 验证码错误
        showTip(tr("验证码错误"), false);
        break;
    }
    case ErrorCodes::EmailNotMatch: {
        // 不匹配
        showTip(tr("用户名和邮箱不匹配"), false);
        _codeCount = 0;
        ui->btn_getCode->setText("获取验证码");
        break;
    }
    case ErrorCodes::PasswdUpFailed: {
        // 重置失败
        showTip(tr("密码重置失败"), false);
        break;
    }
    case ErrorCodes::ServerErr: {
        // 服务器错误
        showTip(tr("服务器错误"), false);
        break;
    }
    }

    // 处理json
    _handlers[id](jsonDoc.object());
    return;
}

void ResetDialog::on_get_code_clicked()
{
    // 验证邮箱是否正确
    if (checkEmailValid()) {
        // 设置定时器
        this->ui->btn_getCode->setEnabled(false);
        this->ui->btn_getCode->setText(QString::number(_codeCount));
        _codeTimer->start(1000);
        // 发送http请求获取验证码
        showTip("", true);
        QJsonObject json_obj;
        json_obj["email"] = ui->edit_email->text();
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/get_varifycode"),
                                            json_obj,
                                            ReqId::ID_GET_VARIFY_CODE,
                                            Modules::RESETMOD);
    } else {
        // 提示邮箱不正确
        showTip(tr("邮箱地址不正确"), false);
    }
}

void ResetDialog::on_reset_code_clicked()
{
    // 检查输入是否有效
    if (checkUserValid() && checkEmailValid() && checkVarifyValid() && checkPassValid()) {
        // 发送POST请求，重置密码
        QJsonObject json_obj;
        json_obj["user"] = ui->edit_user->text();
        json_obj["email"] = ui->edit_email->text();
        json_obj["passwd"] = ui->edit_pass->text();
        json_obj["varifycode"] = ui->edit_code->text();
        showTip("重置密码中...", true);
        stopWindow();
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/reset_pwd"),
                                            json_obj,
                                            ReqId::ID_RESET_USER,
                                            Modules::RESETMOD);
    }
}

void ResetDialog::showTip(QString msg, bool ok)
{
    if (ok) {
        ui->err_tip->setProperty("state", "normal");
    } else {
        ui->err_tip->setProperty("state", "error");
    }
    ui->err_tip->setText(msg);
    repolish(ui->err_tip);
}

void ResetDialog::initHttpHandlers()
{
    // 添加回包处理逻辑
    // 验证码获取
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](QJsonObject jsonObj) {
        int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            _codeCount = 0;
            ui->btn_getCode->setText("获取验证码");
            return;
        }
        showTip(tr("验证码已发送到邮箱，注意查收"), true);
    });
    // 重置密码
    _handlers.insert(ReqId::ID_RESET_USER, [this](QJsonObject jsonObj) {
        int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            return;
        }
        // 重置成功
        _codeCount = 0;
        ui->btn_getCode->setText("获取验证码");
        showTip(tr("密码重置成功"), true);
    });
}

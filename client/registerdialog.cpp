#include "registerdialog.h"
#include "httpmgr.h"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
    , pass_vis(false)
    , confirm_vis(false)
    , _codeCount(60)
{
    ui->setupUi(this);

    _codeTimer = new QTimer();

    // 切换登录界面
    connect(ui->btn_cancel, &QPushButton::clicked, this, &RegisterDialog::switchLogin);
    // 获取验证码
    connect(ui->btn_getCode, &QPushButton::clicked, this, &RegisterDialog::on_get_code_clicked);
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

    // 点击注册
    connect(ui->btn_reg, &QPushButton::clicked, this, &RegisterDialog::on_sure_btn_clicked);

    // 接收http请求结果
    connect(HttpMgr::GetInstance().get(),
            &HttpMgr::sig_reg_mod_finish,
            this,
            &RegisterDialog::slot_reg_mod_finish);

    // 检查各个输入框的信息
    connect(ui->edit_user, &QLineEdit::editingFinished, this, &RegisterDialog::checkUserValid);
    connect(ui->edit_email, &QLineEdit::editingFinished, this, &RegisterDialog::checkEmailValid);
    connect(ui->edit_pass, &QLineEdit::editingFinished, this, &RegisterDialog::checkPassValid);
    connect(ui->edit_confirm, &QLineEdit::editingFinished, this, &RegisterDialog::checkConfirmValid);
    connect(ui->edit_code, &QLineEdit::editingFinished, this, &RegisterDialog::checkVarifyValid);

    // 密码可视化
    connect(ui->pass_visible, &QPushButton::clicked, this, [this]() {
        pass_vis = !pass_vis;
        ui->edit_pass->setEchoMode(pass_vis == true ? QLineEdit::Normal : QLineEdit::Password);
    });
    connect(ui->confirm_visible, &QPushButton::clicked, this, [this]() {
        confirm_vis = !confirm_vis;
        ui->edit_confirm->setEchoMode(confirm_vis == true ? QLineEdit::Normal : QLineEdit::Password);
    });

    // 初始化handlers
    initHttpHandlers();
}

RegisterDialog::~RegisterDialog()
{
    stopTimer();
    delete ui;
}

void RegisterDialog::stopTimer()
{
    _codeTimer->stop();
}

void RegisterDialog::on_get_code_clicked()
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
                                            Modules::REGISTERMOD);
    } else {
        // 提示邮箱不正确
        showTip(tr("邮箱地址不正确"), false);
    }
}

void RegisterDialog::on_sure_btn_clicked()
{
    // 检查各输入框是否为空
    if (checkUserValid() && checkEmailValid() && checkPassValid() && checkPassValid()
        && checkConfirmValid() && checkVarifyValid()) {
        // 消息放入Json中发送HTTP请求
        QJsonObject json_obj;
        json_obj["user"] = ui->edit_user->text();
        json_obj["email"] = ui->edit_email->text();
        json_obj["passwd"] = ui->edit_pass->text();
        json_obj["confirm"] = ui->edit_confirm->text();
        json_obj["varifycode"] = ui->edit_code->text();
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/user_register"),
                                            json_obj,
                                            ReqId::ID_REG_USER,
                                            Modules::REGISTERMOD);
        showTip("注册中...", true);
        stopWindow();
    }
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err){
    startWindow();
    // 处理HTTP请求结果
    if(err !=ErrorCodes::SUCCESS){
        showTip("网络请求错误",false);
        _codeCount = 0;
        ui->btn_getCode->setText("获取验证码");
        return;
    }

    // 解析JSON字符串
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    // json为空
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        showTip(tr("json解析错误"),false);
        return;
    }

    switch (jsonDoc["error"].toInt()) {
    case ErrorCodes::VarifyExpired: {
        // 验证码不存在
        showTip(tr("验证码已过期"), false);
        break;
    }
    case ErrorCodes::VarifyCodeErr: {
        // 验证码不存在
        showTip(tr("验证码错误"), false);
        break;
    }
    case ErrorCodes::UserExist: {
        // 验证码不存在
        showTip(tr("用户名已存在"), false);
        break;
    }
    case ErrorCodes::EmailExist: {
        // 验证码不存在
        showTip(tr("邮箱已存在"), false);
        break;
    }
    case ErrorCodes::PasswdErr: {
        // 验证码不存在
        showTip(tr("密码和确认密码不匹配"), false);
        break;
    }
    case ErrorCodes::ServerErr: {
        // 验证码不存在
        showTip(tr("服务器错误"), false);
        break;
    }
    }

    // 处理json
    _handlers[id](jsonDoc.object());
    return;
}

void RegisterDialog::showTip(QString str,bool ok)
{
    if(ok)
        ui->err_tip->setProperty("state","normal");
    else
        ui->err_tip->setProperty("state", "error");
    ui->err_tip->setText(str);
    repolish(ui->err_tip);
}

void RegisterDialog::initHttpHandlers(){
    // 注册获取验证码逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](QJsonObject jsonObj) {
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            _codeCount = 0;
            ui->btn_getCode->setText("获取验证码");
            return;
        }
        showTip(tr("验证码已发送到邮箱，注意查收"), true);
    });
    // 注册逻辑
    _handlers.insert(ReqId::ID_REG_USER, [this](QJsonObject jsonObj) {
        int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESS) {
            return;
        }
        _codeCount = 0;
        ui->btn_getCode->setText("获取验证码");
        auto email = jsonObj["email"].toString();
        showTip(tr("注册成功"), true);
        // 移到登录界面
        emit(RegisterDialog::switchLogin());
        qDebug() << "user register success, email is " << email;
    });
}

bool RegisterDialog::checkUserValid()
{
    if (this->ui->edit_user->text() == "") {
        showTip("用户名不能为空", false);
        return false;
    }
    return true;
}

bool RegisterDialog::checkEmailValid()
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

bool RegisterDialog::checkPassValid()
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

bool RegisterDialog::checkConfirmValid()
{
    auto confirm = ui->edit_confirm->text();
    auto pass = ui->edit_pass->text();
    if (confirm != pass) {
        //提示长度不准确
        showTip(tr("确认密码应与密码一致"), false);
        return false;
    }
    return true;
}

bool RegisterDialog::checkVarifyValid()
{
    auto pass = ui->edit_code->text();
    if (pass.isEmpty()) {
        showTip(tr("验证码不能为空"), false);
        return false;
    }
    return true;
}

void RegisterDialog::startWindow()
{
    QList<QWidget *> widgets = this->findChildren<QWidget *>();
    foreach (QWidget *widget, widgets) {
        widget->setEnabled(true);
    }
}

void RegisterDialog::stopWindow()
{
    QList<QWidget *> widgets = this->findChildren<QWidget *>();
    foreach (QWidget *widget, widgets) {
        widget->setEnabled(false);
    }
}

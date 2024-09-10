#include "registerdialog.h"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    // 切换登录界面
    connect(ui->btn_cancel, &QPushButton::clicked, this, &RegisterDialog::switchLogin);
    // 获取验证码
    connect(ui->btn_getCode,&QPushButton::clicked,this,&RegisterDialog::on_get_code_clicked);
    // 点击注册
    connect(ui->btn_reg, &QPushButton::clicked, this, &RegisterDialog::on_sure_btn_clicked);
    // 接收http请求结果
    connect(HttpMgr::GetInstance().get(),&HttpMgr::sig_reg_mod_finish,this,&RegisterDialog::slot_reg_mod_finish);

    // 初始化handlers
    initHttpHandlers();
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_get_code_clicked()
{
    // 验证邮箱的地址正则表达式
    auto email = ui->edit_email->text();
    // 邮箱地址的正则表达式
    static QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(match){
        // 发送http请求获取验证码
        showTip("",true);
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/get_varifycode"),
                                            json_obj,
                                            ReqId::ID_GET_VARIFY_CODE,
                                            Modules::REGISTERMOD);
    }else{
        // 提示邮箱不正确
        showTip(tr("邮箱地址不正确"),false);
    }
}

void RegisterDialog::on_sure_btn_clicked()
{
    // 检查各输入框是否为空
    if (ui->edit_user->text() == "") {
        showTip(tr("用户名不能为空"), false);
        return;
    }

    if (ui->edit_email->text() == "") {
        showTip(tr("邮箱不能为空"), false);
        return;
    }

    if (ui->edit_pass->text() == "") {
        showTip(tr("密码不能为空"), false);
        return;
    }

    if (ui->edit_confirm->text() == "") {
        showTip(tr("确认密码不能为空"), false);
        return;
    }

    if (ui->edit_pass->text() != ui->edit_confirm->text()) {
        showTip(tr("密码和确认密码不匹配"), false);
        return;
    }

    if (ui->edit_code->text() == "") {
        showTip(tr("验证码不能为空"), false);
        return;
    }
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
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err){
    // 处理HTTP请求结果
    if(err !=ErrorCodes::SUCCESS){
        showTip("网络请求错误",false);
        return;
    }

    // 解析JSON字符串
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    // json为空
    if(jsonDoc.isNull()){
        showTip(tr("json解析错误"),false);
        return;
    }

    // json解析错误
    if(!jsonDoc.isObject()){
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
        ui->err_tip->setProperty("state","err");
    ui->err_tip->setText(str);
    repolish(ui->err_tip);
}

void RegisterDialog::initHttpHandlers(){
    // 注册获取验证码逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](QJsonObject jsonObj) {
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
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
        auto email = jsonObj["email"].toString();
        showTip(tr("注册成功"), true);
        // 移到登录界面
        emit(RegisterDialog::switchLogin());
        qDebug() << "user register success, email is " << email;
    });
}

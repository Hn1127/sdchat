#include "registerdialog.h"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    // 切换登录界面
    connect(ui->log_btn,&QPushButton::clicked,this,&RegisterDialog::switchLogin);
    // 获取验证码
    connect(ui->btn_getCode,&QPushButton::clicked,this,&RegisterDialog::on_get_code_clicked);
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
    auto email = ui->lineEdit_email->text();
    // 邮箱地址的正则表达式
    static QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(match){
        // 发送http请求获取验证码
        showTip("",true);
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpMgr::GetInstance()->PostHttpReq(QUrl("http://localhost:8080/get_varifycode"),
                                            json_obj,
                                            ReqId::ID_GET_VARIFY_CODE,
                                            Modules::REGISTERMOD);
    }else{
        // 提示邮箱不正确
        showTip(tr("邮箱地址不正确"),false);
    }
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
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }
        showTip(tr("验证码已发送到邮箱，注意查收"), true);
    });
}

#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    connect(ui->btn_reg, &QPushButton::clicked, this, &LoginDialog::switchRegister);
    connect(ui->btn_reset, &QPushButton::clicked, this, &LoginDialog::switchReset);

    // 密码可视化
    connect(ui->pass_visible, &QPushButton::clicked, this, [this]() {
        pass_vis = !pass_vis;
        ui->edit_pass->setEchoMode(pass_vis == true ? QLineEdit::Normal : QLineEdit::Password);
    });
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

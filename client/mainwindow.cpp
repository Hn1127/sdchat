#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _login_dlg = new LoginDialog();
    _reg_dlg = new RegisterDialog();
    _reset_dlg = new ResetDialog();
    // 展示登录界面
    setCentralWidget_(_login_dlg);
    _login_dlg->show();

    // 连接LoginDialog的switchRegister信号，登录->注册
    connect(_login_dlg,&LoginDialog::switchRegister,this,&MainWindow::SlotSwitchReg);
    // 忘记密码
    connect(_login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);
    // 连接RegisterDialog的switchLogin信号，注册->登录
    connect(_reg_dlg,&RegisterDialog::switchLogin,this,&MainWindow::SlotSwitchLog);
    // 连接ResetDialog的switchLogin信号，重置->登录
    connect(_reset_dlg, &ResetDialog::switchLogin, this, &MainWindow::SlotSwitchLog);
}

MainWindow::~MainWindow()
{
    delete _login_dlg;
    delete _reg_dlg;
    delete _reset_dlg;
    delete ui;
}

void MainWindow::setCentralWidget_(QWidget* w){
    takeCentralWidget();
    setCentralWidget(w);
}

// 登录界面切换至注册界面
void MainWindow::SlotSwitchReg(){
    setCentralWidget_(_reg_dlg);
    _login_dlg->hide();
    _reg_dlg->show();
}

// 注册界面切换至登录界面
void MainWindow::SlotSwitchLog(){
    // 停止定时器
    setCentralWidget_(_login_dlg);
    _login_dlg->show();
    _reg_dlg->hide();
}

// 登录界面切换至重置界面
void MainWindow::SlotSwitchReset()
{
    setCentralWidget_(_reset_dlg);
    _login_dlg->hide();
    _reset_dlg->show();
}

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _login_dlg = new LoginDialog();
    _reg_dlg = new RegisterDialog();
    // 展示登录界面
    setCentralWidget_(_login_dlg);
    _login_dlg->show();

    // 连接LoginDialog的switchRegister信号
    connect(_login_dlg,&LoginDialog::switchRegister,this,&MainWindow::SlotSwitchReg);
    // 连接RegisterDialog的switchLogin信号
    connect(_reg_dlg,&RegisterDialog::switchLogin,this,&MainWindow::SlotSwitchLog);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setCentralWidget_(QWidget* w){
    takeCentralWidget();
    setCentralWidget(w);
}

// 处理LoginDialog的"注册"
// 切换至注册界面
void MainWindow::SlotSwitchReg(){
    setCentralWidget_(_reg_dlg);
    _login_dlg->hide();
    _reg_dlg->show();
}

// 处理RegisterDialog的"取消"
// 切换至登录界面
void MainWindow::SlotSwitchLog(){
    setCentralWidget_(_login_dlg);
    _login_dlg->show();
    _reg_dlg->hide();
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "logindialog.h"
#include "registerdialog.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setCentralWidget_(QWidget* w);

private:
    void SlotSwitchReg();
    void SlotSwitchLog();

private:
    Ui::MainWindow *ui;
    LoginDialog* _login_dlg;
    RegisterDialog* _reg_dlg;
};
#endif // MAINWINDOW_H

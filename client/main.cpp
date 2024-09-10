#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QLocale>
#include <QSettings>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 获取服务器的url
    // 获取当前应用程序的路径
    QString app_path = QCoreApplication::applicationDirPath();
    // 拼接文件名
    QString fileName = "config.ini";
    QString config_path = QDir::toNativeSeparators(app_path + QDir::separator() + fileName);
    QSettings settings(config_path, QSettings::IniFormat);
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();
    gate_url_prefix = "http://" + gate_host + ":" + gate_port;

    // 加载qss文件
    QFile qss(":/style/stylesheet.qss");
    if( qss.open(QFile::ReadOnly))
    {
        qDebug("qssFile open success");
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }else{
        qDebug("qssFile Open failed");
    }

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "sdchat_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}

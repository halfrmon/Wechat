#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include "global.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile qss(":/style/stylesheet.qss");
    if(qss.open(QFile::ReadOnly))
    {
        qDebug("open success");
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);//根据读取到的qss文件来设置款式
        qss.close();
    }else{
        qDebug("Opne failed");
    }
    QString app_path = "/home/chen/AV_Development/5.29Project/Qtwidget/chat";
    // 拼接文件名
    QString fileName = "config.ini";
    QString config_path = QDir::toNativeSeparators(app_path +
                            QDir::separator() + fileName);
    QSettings settings(config_path, QSettings::IniFormat);
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();
    gate_url_prefix = "http://"+gate_host+":"+gate_port;
    qDebug()<<gate_url_prefix;
    MainWindow w;
    w.show();
    return a.exec();
}

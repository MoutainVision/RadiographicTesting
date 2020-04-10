#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>

#include "appconfig.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QTextCodec::setCodecForLocale(codec);

    Appconfig::InitAllDataPath();
    Appconfig::loadConfigFile();


    MainWindow w;
    w.showMaximized();

    return a.exec();
}

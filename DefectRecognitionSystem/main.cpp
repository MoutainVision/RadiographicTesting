#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>

#include "appconfig.h"

#include "logindlg.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QTextCodec::setCodecForLocale(codec);

    Appconfig::InitAllDataPath();
    Appconfig::loadConfigFile();


    LoginDlg dlg;
    MainWindow w;

    if (dlg.exec() == QDialog::Accepted)
    {
        w.showMaximized();
        return a.exec();
    }

    return 0;
}

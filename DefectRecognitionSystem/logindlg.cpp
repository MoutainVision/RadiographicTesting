#include "logindlg.h"
#include "ui_logindlg.h"

#include <QFile>
#include <QDesktopWidget>
#include <QFontDatabase>
#include <QMouseEvent>

#define MARGINS 2 //窗体边框
#define BORDERWIDTH 20


LoginDlg::LoginDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDlg)
{
    ui->setupUi(this);

//    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
//    setWindowFlags(Qt::FramelessWindowHint);
//    setAttribute(Qt::WA_TranslucentBackground);

//    ui->widget_head->setTitle(tr("Login"));

    QString fileName;
    fileName = ":/qss/login.qss";

    QFile file(fileName);

    if(file.open(QIODevice::ReadOnly))
    {
        ui->widget_back->setStyleSheet(file.readAll());
        file.close();
    }

    m_isMd5 = false;

    ui->label_msg->hide();


        ui->lineEdit_name->setFocus();


    connect(ui->lineEdit_name, SIGNAL(textChanged(QString)), this, SLOT(slot_editChange(QString)));
    connect(ui->lineEdit_password, SIGNAL(textChanged(QString)), this, SLOT(slot_passwordChange(QString)));

    connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(slot_btnOkClick()));
}

LoginDlg::~LoginDlg()
{
    delete ui;

//    this->reject();
}

//    QTimer::singleShot(500,this,[&]()
//    {
//        QPoint pt(0, 0);
//        QMouseEvent evt(QEvent::Leave, pt, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
//        qApp->sendEvent(ui->btnMenu_Close, &evt);
//    });

void LoginDlg::slot_btnOkClick()
{
    login();
}

void LoginDlg::slot_editChange(QString str)
{
    ui->label_msg->hide();
}

void LoginDlg::slot_passwordChange(QString str)
{
    m_isMd5 = false;
    ui->label_msg->hide();
}

bool LoginDlg::loginOperate(QString name, QString password, bool &isScuful, QString &errorStr)
{
//    if (name != g_userName)
    if (name != "admin")
    {
        isScuful = false;
        errorStr = tr("user doesn't exist.");
        return true;
    }

    if (password == "q")
    {
        isScuful = true;
        return true;
    }

//    if (password != g_password && g_appSysConfig.password != getMd5Str(password))
    if (password != "123456")
    {
        isScuful = false;
        errorStr = tr("Incorrect password.");
        return true;
    }

    isScuful = true;

    return true;
}

void LoginDlg::login()
{
    QString name = ui->lineEdit_name->text();
    QString password = ui->lineEdit_password->text();

//    if (name.isEmpty())
//    {
//        ui->label_msg->show();
//        ui->label_msg->setText(tr("User name can't be empty."));

//        return ;
//    }

    if (password.isEmpty())
    {
        ui->label_msg->show();
        ui->label_msg->setText(tr("Password can't be empty."));

        return ;
    }

    bool isSucful = true;
    QString errorStr;

    if (loginOperate(name, password, isSucful, errorStr))
    {
        if (isSucful)
        {
//            g_bIsLogin = true;

            this->accept();
        }
        else
        {
            ui->label_msg->show();
            ui->label_msg->setText(errorStr);

            return ;
        }
    }
}

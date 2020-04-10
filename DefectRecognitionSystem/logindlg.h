#ifndef LOGINDLG_H
#define LOGINDLG_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class LoginDlg;
}

//鼠标实现改变窗口大小
#define PADDING 6

class LoginDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDlg(QWidget *parent = 0);
    ~LoginDlg();

    bool loginOperate(QString name, QString password, bool &isScuful, QString &errorStr);

    void login();

private slots:
    void slot_editChange(QString str);

    void slot_passwordChange(QString str);

    void slot_btnOkClick();

private:
    Ui::LoginDlg *ui;

    bool            m_isMd5;
};

#endif // LOGINDLG_H

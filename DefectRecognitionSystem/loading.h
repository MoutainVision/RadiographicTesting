#ifndef LOADING_H
#define LOADING_H

#include <QWidget>
#include <QMovie>
#include <QLabel>
#include <QTimer>

class Loading : public QWidget
{
    Q_OBJECT
public:
    explicit Loading(QWidget *parent = 0);

    void showLoading();

    void closeLoading();

    void setWaitMsg(QString msg);

protected:
    void paintEvent(QPaintEvent *e);

public slots:
    void slot_timeOut();

private:
    bool    m_bFirst;

    QMovie*     m_movie;
    QLabel*     m_label;

    QString     m_msg;

    int         m_count;
    QString     m_showMsg;

    QTimer      *m_timer;

};

#endif // LOADING_H

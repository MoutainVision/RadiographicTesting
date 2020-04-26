#include "loading.h"

#include <QLayout>
#include <qDebug>
#include <QDesktopWidget>
#include <QApplication>

#include <QPainter>
#include <QVBoxLayout>

Loading::Loading(QWidget *parent) :
    QWidget(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    this->setAttribute(Qt::WA_TranslucentBackground);

    m_movie = new QMovie(":/imgs/load.gif");


    m_label = new QLabel();

    m_label->setMovie(m_movie);
    m_movie->start();

    QVBoxLayout* vlayout = new QVBoxLayout;
    vlayout->addWidget(m_label,0,Qt::AlignCenter);
    vlayout->setContentsMargins(0,0,0,0);

    setLayout(vlayout);

    QRect mLimitRect;
    if (QApplication::desktop()->screenCount() == 2)
    {
        QRect rect1 = QApplication::desktop()->availableGeometry(0);
        QRect rect2 = QApplication::desktop()->availableGeometry(1);

        mLimitRect.setX((rect1.x() < rect2.x() ? rect1.x():rect2.x()));
        mLimitRect.setY((rect1.y() > rect2.y() ? rect1.y():rect2.y()));
        mLimitRect.setWidth(rect1.width() + rect2.width());
        mLimitRect.setHeight((rect1.height() < rect2.height() ? rect1.height():rect2.height()));
    }
    else
    {
        QRect rectTemp = QApplication::desktop()->availableGeometry(0);

        mLimitRect.setX(rectTemp.x());
        mLimitRect.setY(rectTemp.y());
        mLimitRect.setWidth(rectTemp.width());
        mLimitRect.setHeight(rectTemp.height());
    }    

    setFixedSize(mLimitRect.width(),mLimitRect.height());

    m_timer = new QTimer;
    m_timer->start(200);

    m_bFirst = false;

    m_count = 0;

    m_msg = tr("Please Wait");

    connect(m_timer, SIGNAL(timeout()), this, SLOT(slot_timeOut()));
}

void Loading::setWaitMsg(QString msg)
{
    m_msg = msg;
}

void Loading::slot_timeOut()
{
    m_count ++;

    if (m_count==1)
        m_showMsg = m_msg;
    else if (m_count == 2)
        m_showMsg = m_msg + " .";
    else if (m_count == 3)
        m_showMsg = m_msg + " . .";
    else if (m_count == 4)
    {
        m_showMsg = m_msg + " . . .";
        m_count = 0;
    }

    update();
}

void Loading::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.fillRect(rect(),QColor(100,100,100,100));

    QPoint labelPos = m_label->pos();


    QPen pen;
    pen.setColor(QColor(255, 255, 255));

    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    font.setFamily("Arial");

    int l = m_msg.toLocal8Bit().length() - 9;

    QRect msgRect;
    msgRect = QRect(QPoint(labelPos.x() - l*5, labelPos.y()+80), QSize(250 + l*5, 50));

    painter.setPen(pen);
    painter.setFont(font);

    painter.drawText(msgRect, m_showMsg);

    return QWidget::paintEvent(e);
}

void Loading::showLoading()
{
    m_movie->start();
    this->showMaximized();
}

void Loading::closeLoading()
{
    m_movie->stop();
    this->close();
}











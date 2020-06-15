#ifndef BRIGHTCLBWDG_H
#define BRIGHTCLBWDG_H

#include <QWidget>
#include <QtMath>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QEvent>
#include <QKeyEvent>
#include <QPoint>

namespace Ui {
class BrightClbWdg;
}

class BrightClbWdg : public QWidget
{
    Q_OBJECT

public:
    explicit BrightClbWdg(QWidget *parent = 0);
    ~BrightClbWdg();

    void setHist(unsigned aF[], int size = 65536);

    int getYValue(int x, QPoint point1, QPoint point2);

private:
    bool eventFilter(QObject *, QEvent *);

private:
    Ui::BrightClbWdg *ui;


    unsigned      m_g[65536];


};

#endif // BRIGHTCLBWDG_H

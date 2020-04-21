#ifndef GEYIMGWDG_H
#define GEYIMGWDG_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QEvent>
#include <QPoint>


namespace Ui {
class GeyImgWdg;
}

class GeyImgWdg : public QWidget
{
    Q_OBJECT

public:
    explicit GeyImgWdg(QWidget *parent = nullptr);
    ~GeyImgWdg();

    void SetIntensityCurveValues(std::vector<unsigned short> aIntensity, int pixWidth);

    int getCurIndex(){return mCurIndex;}


signals:
    void sig_curGreayPos();



protected:
    void resizeEvent(QResizeEvent *e);

    bool eventFilter(QObject *obj, QEvent *e);

private:
    Ui::GeyImgWdg *ui;

    //
    float                        m_pixWidth;
    std::vector<unsigned short>  m_aIntensity;

    QPoint					     m_curPoint;
    int mCurIndex;
};

#endif // GEYIMGWDG_H

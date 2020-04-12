#ifndef RECOGNIZEWDG_H
#define RECOGNIZEWDG_H

#include <QWidget>
#include <QPainter>
#include <QRect>

#include <QScrollBar>


#include "appconfig.h"

#include "LibImgIO.h"
#include "LibImgProc.h"

#include "Base/FunctionTransfer.h"


#include <thread>

namespace Ui {
class RecognizeWdg;
}

class RecognizeWdg : public QWidget
{
    Q_OBJECT

public:
    explicit RecognizeWdg(QWidget *parent = nullptr);
    ~RecognizeWdg();

    void setDcmFileInfo(DcmFileNode dcmInfo);

    void reSize(int newW, int newH);

    void reSize(float scale);

    void showImg();

    void showScrollBar();

    void showScrollBar(bool status);

    void setScrollRanges(int maxX, int maxY);

    void shortImgToImage(unsigned short *pImg, int nW, int nH, QImage &img);

private slots:
    void slotBtnClick(bool bClick);

    void slot_scrollAreaXChange(int value);

    void slot_scrollAreaYChange(int value);

protected:
    bool eventFilter(QObject *obj, QEvent *e);


private:
    Ui::RecognizeWdg *ui;

    DcmFileNode mCurDcmFileInfo;
    DCMFile dmfile;

    int     mSourceX;
    int     mSourceY;
    QRect   mSourceRect;
    QImage  mPaintImg;



    QRect   mPaintRect;
};

#endif // RECOGNIZEWDG_H

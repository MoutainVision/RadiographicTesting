#ifndef RECOGNIZEWDG_H
#define RECOGNIZEWDG_H

#include <QWidget>
#include <QPainter>
#include <QRect>

#include <QScrollBar>
#include <QAbstractButton>
#include <QMouseEvent>

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

    //正常显示
    void showNomal();

    //适配屏幕
    void showAdapt();

    //恢复图像
    void resetImg();


    void showImg();

    void showImg(unsigned short *pImg, int nW, int nH);

    void showScrollBar();

    void showScrollBar(bool status);

    void setScrollRanges(int maxX, int maxY);

    void shortImgToImage(unsigned short *pImg, int nW, int nH, QImage &img);


    //批处理图像
    void delImg();

    //更新鼠标样式
    void updateCursor();

    void setScrollBarOffsetValues(int hValue, int vValue);

private slots:
    void slotBtnClick(bool bClick);

    void slot_scrollAreaXChange(int value);

    void slot_scrollAreaYChange(int value);

    void slot_sliderReleased();

    void slot_sliderWinValueChange(int value);

    //--tool widget--
    void slot_btnGroupClick(QAbstractButton *btn);


protected:
    bool eventFilter(QObject *obj, QEvent *e);


private:
    Ui::RecognizeWdg *ui;

    DcmFileNode mCurDcmFileInfo;
    DCMFile dmfile;

    DCMFile mSourceFile;

    int     mSourceX;
    int     mSourceY;
    QRect   mSourceRect;
    QImage  mPaintImg;

    unsigned short *m_pImgPro;  //处理图像
    unsigned short *m_pSrcImg;  //原始图像
    int     mSrcImgWidth;       //原始图像宽
    int     mSrcImgHeight;      //原始图像高


    bool    mBMeasureOpt;
    bool    mBDelImging;

    //当前图像大小
    int     mCurImgWidth;
    int     mCurImgHeight;

    float   mScale;  //缩放

    //
    bool    mBInvert;  //反相
    bool    mBFlip;    //翻转
    bool    mBMirror;  //镜像

    int     mWinCentre; //窗宽
    int     mWinWidth;  //窗位


    QRect   mPaintRect;

    //**绘制相关***
    //鼠标
    bool            m_bIsPress;
    QPoint          m_PressPt;


    //
    CurAction           m_ePreAction;
    CurAction           m_eCurAction;
    DrawStatus          m_eDrawStatus;
    DragDirection       m_eSldDragDirection;
};

#endif // RECOGNIZEWDG_H

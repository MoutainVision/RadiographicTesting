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

    //������ʾ
    void showNomal();

    //������Ļ
    void showAdapt();

    //�ָ�ͼ��
    void resetImg();


    void showImg();

    void showImg(unsigned short *pImg, int nW, int nH);

    void showScrollBar();

    void showScrollBar(bool status);

    void setScrollRanges(int maxX, int maxY);

    void shortImgToImage(unsigned short *pImg, int nW, int nH, QImage &img);


    //������ͼ��
    void delImg();

    //���������ʽ
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

    unsigned short *m_pImgPro;  //����ͼ��
    unsigned short *m_pSrcImg;  //ԭʼͼ��
    int     mSrcImgWidth;       //ԭʼͼ���
    int     mSrcImgHeight;      //ԭʼͼ���


    bool    mBMeasureOpt;
    bool    mBDelImging;

    //��ǰͼ���С
    int     mCurImgWidth;
    int     mCurImgHeight;

    float   mScale;  //����

    //
    bool    mBInvert;  //����
    bool    mBFlip;    //��ת
    bool    mBMirror;  //����

    int     mWinCentre; //����
    int     mWinWidth;  //��λ


    QRect   mPaintRect;

    //**�������***
    //���
    bool            m_bIsPress;
    QPoint          m_PressPt;


    //
    CurAction           m_ePreAction;
    CurAction           m_eCurAction;
    DrawStatus          m_eDrawStatus;
    DragDirection       m_eSldDragDirection;
};

#endif // RECOGNIZEWDG_H

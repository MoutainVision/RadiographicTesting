#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QEvent>

#include <QDirModel>
#include <QTreeView>
#include <QSplitter>
#include <QFileSystemModel>
#include <QSplitter>
#include <QFileDialog>
#include <qfileinfo.h>
#include <QDateTime>
#include <QMessageBox>

#include <QMutex>

#include <QPaintEvent>
#include <QPainter>
#include <QSplitter>

#include <thread>

#include "recognizewdg.h"
#include "recheckwdg.h"
#include "readdcmfile.h"

#include "appconfig.h"

#include "prewdg.h"

#include "DefectRecognition.h"


#define MAX_PRE_WIDGET_WIDTH 100
#define MIN_PRE_WIDGET_WIDTH 50


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setPreviewImg(QString filePath);

    void calcPreWdgPos();

    void deletePreWdgList();




    //
    void setDcmFileInfo();

    void reSize(int newW, int newH);

    void reSize(float scale);

    //正常显示
    void showNomal();

    //适配屏幕
    void showAdapt();

    //恢复图像
    void resetImg();

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

    //识别 插入特征
    void setRecognizeValue(int index, DefectFeat feat);

private slots:
    void slotBtnClick(bool bClick);

    void textChanged(QString text);

    void clicked(QModelIndex index);

    void  expanded(QModelIndex index);

    //
    void slot_scrollAreaXChange(int value);

    void slot_scrollAreaYChange(int value);

    void slot_sliderReleased();

    void slot_sliderWinValueChange(int value);

    //--tool widget--
    void slot_btnGroupClick(QAbstractButton *btn);

    void slot_tabCurrentChanged(int index);


protected:
    void resizeEvent(QResizeEvent *e);

    bool eventFilter(QObject *obj, QEvent *e);

private:
    Ui::MainWindow *ui;

    QSplitter *mSplitter;
    QDirModel *mModel;

    RecognizeWdg *mRecognizeWdg;
    ReCheckWdg *mReCheckWdg;

    QImage mPreviewImg;
    QPixmap mPreviewPixImg;

    QStringList   mDcmFileList;
    int mCurIndex;
    int mDcmCount;

    DcmFileNode mCurDcmFileInfo;
    QList<PreWdg *> mPreWdgList;

    QMutex     mDelImgLock;

    //缺陷
    vector<Defect> mADefectList;
    QList<QColor>  mDefectClassColor;
    bool           mBShowDefect;
    bool           mBShowCenter;

    //
    //**绘制相关***
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

    //鼠标
    bool            m_bIsPress;
    QPoint          m_PressPt;


    //
    CurAction           m_ePreAction;
    CurAction           m_eCurAction;
    DrawStatus          m_eDrawStatus;
    DragDirection       m_eSldDragDirection;

};

#endif // MAINWINDOW_H

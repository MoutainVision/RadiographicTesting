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

    //预览相关
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

    //滚动条相关
    void showScrollBar();

    void showScrollBar(bool status);

    void setScrollRanges(int maxX, int maxY);

    //鼠标移动， 修改滚动条，修改图像位置
    void setScrollBarOffsetValues(int hValue, int vValue);

    //图像转换
    void shortImgToImage(unsigned short *pImg, int nW, int nH, QImage &img);

    //显示图像
    void showImg(unsigned short *pImg, int nW, int nH);

    //批处理图像
    void delImg();

    //更新鼠标样式
    void updateCursor();

    //识别 插入特征
    void setRecognizeValue(int index, DefectFeat feat);

private slots:
    void slotBtnClick(bool bClick);

    void textChanged(QString text);

    //资源树点击
    void clicked(QModelIndex index);

    void  expanded(QModelIndex index);

    //滚动条X Y 改变
    void slot_scrollAreaXChange(int value);

    void slot_scrollAreaYChange(int value);

    //放大缩小
    void slot_sliderReleased();

    //窗宽窗位变化
    void slot_sliderWinValueChange(int value);

    //--tool widget--
    void slot_btnGroupClick(QAbstractButton *btn);

    //模块tab 变化
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

    //--图像处理相关=------
    int     mCurImgWidth;
    int     mCurImgHeight;

    float   mScale;  //缩放

    //
    bool    mBInvert;  //反相
    bool    mBFlip;    //翻转
    bool    mBMirror;  //镜像

    int     mWinCentre; //窗宽
    int     mWinWidth;  //窗位

    int     mRotate;     //角度
    int     mNeedRotate; //需要旋转的角度

    QRect   mPaintRect;

    bool    mBMeasureOpt;
    bool    mBDelImging;

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

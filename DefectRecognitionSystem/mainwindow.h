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
#include <QTabWidget>
#include <QHeaderView>

#include <QMenu>
#include <QAction>
#include <QPoint>

#include <thread>

#include<iostream>
#include<fstream>


#include "recognizewdg.h"
#include "recheckwdg.h"
#include "readdcmfile.h"

#include "appconfig.h"

#include "prewdg.h"

#include "DefectRecognition.h"
#include "DCMIndexingFile.h"
#include "ImageRetrieval.h"

//**绘图**
#include "rectitem.h"
#include "ellipseitem.h"
#include "lineitem.h"
#include "textitem.h"

#include "geyimgwdg.h"

#include "colorwdg.h"

#include "loading.h"

#include "Windows.h"

#include "aboutdlg.h"


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

    void showLoading(QString msg);

    void closeLoading();

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

    //识别缺陷
    void exeDefectImg();

    //清楚缺陷
    void clearDefect();

    //状态切换
    void actionActionChange();

    //----查重------
    void addRetrievalResultValues(vector<RetrievalResult> aRes);

//----灰度分布图------
    QPoint convertImgPt(QPoint wPt);

    QPoint convertWdgPt(QPoint iPt);

    //计算灰度分布
    void calcIntensityCurve(QPoint p1, QPoint p2);

    void calcCurGreyPos(int length);

    //设置灰度图像
    void setIntensityCurveValues(vector<unsigned short> aIntensity);

    //清空计算灰度的线
    void clearGreyLine();

    //关闭灰度窗口
    void closeGreyWdg();

    void getIntensity(QPoint curPt);


//----绘几何图形相关------
    ItemOperator *getPreOperator();
    ItemOperator *getNextOperator();

    void insetOperator(ItemOperator *itemOperator);
    void doPreOperator(ItemOperator *itemOperator);
    void doNextOperator(ItemOperator *itemOperator);

    //更新操作历史记录按钮组（redo， undo）
    void updateOperatorStatus();

    //清空item
    void clearItemSelected(int index);

    //--mouse--
    void mousePressArrowAction(QPoint pt);

    void mouseReleaseArrowAction();

    void mouseMoveArrowAction(QPoint pt);

    //获取拖拽点方向
    bool getDragDirection(QList<DragItem> &dragItemList,
                          QPoint pt,
                          DragDirection &dragDirection);

    QPoint getCurOffset();

//**测量表格相关**
    void updateMeasureTable();

    void addOneMeasure(int num, QString name,
                       QPoint beginPt,
                       QPoint endPt,
                       QPoint centerPt,
                       qreal width,
                       qreal height,
                       qreal majorAxis,
                       qreal minorAxis,
                       qreal angle,
                       qreal area,
                       qreal perimeter,
                       qreal intersity,
                       qreal mean,
                       qreal snr);

private slots:
    void slotBtnClick(bool bClick);

    void textChanged(QString text);

    void slot_tableCellClicked(int row, int col);

    //菜单
    void slot_menueToggle();
    void slot_showManu(QPoint pt);
    void slot_actionDelete();

    //资源树点击
    void clicked(QModelIndex index);

    void  expanded(QModelIndex index);

    //滚动条X Y 改变
    void slot_scrollAreaXChange(int value);

    void slot_scrollAreaYChange(int value);

    //放大缩小
    void slot_sliderValuechange(int value);

    //窗宽窗位变化
    void slot_sliderWinValueChange(int value);

    //--tool widget--
    void slot_btnGroupClick(QAbstractButton *btn);

    //模块tab 变化
    void slot_tabCurrentChanged(int index);

//工具栏
    //删除一个
    void slot_btnEraserToolClick();

    //撤销
    void slot_btnPreToolClick();

    //恢复
    void slot_btnNextToolClick();

    //清空
    void slot_btnDeleteToolClick();

    //参数值改变
    void slot_paramValueChange(int value);




protected:
    void resizeEvent(QResizeEvent *e);

    bool eventFilter(QObject *obj, QEvent *e);

private:
    Ui::MainWindow *ui;

    QMenu       *m_containerMenu;
    QAction     *m_actDelete;

    Loading *m_loadingDlg;

    HANDLE hEvent;

    //颜色
    ColorWdg *mColorWdg;

    //
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

    //标尺
    bool    mBReal;

    //查重
    QString mIndexFilePath;
    QString mIndexDataFilePath;
    ofstream mIndexFileOfs;

    PreWdg *mPreWdg;

    RectItem    *mRecheckRectItem;

    //检索数据
    DCMFileIndexingData mIndexData;



    //缺陷
    vector<Defect> mADefectList;
    QList<QColor>  mDefectClassColor;
    bool           mBShowDefect;
    bool           mBShowCenter;

    int            mCurDefectIndex;

    unsigned short *m_pImgDefect;  //处理图像

    RectItem    *mDefectRectItem;

    //灰度图像
    QPoint      m_iP1;
    QPoint      m_iP2;

    QLine       mGrayLine;
    QLine       mGrayLineTmp;
    QRect       mGreyRect;
    float       m_angle;

    GeyImgWdg   *mGeyImgWdg;

    RectItem    *mGreyRectItem;

    //--图像处理相关=------
    int     mCurImgWidth;
    int     mCurImgHeight;

    float   mScale;     //缩放

    //
    bool    mBInvert;  //反相
    bool    mBFlip;    //翻转
    bool    mBMirror;  //镜像
    bool    mBWind;  //窗宽
//    bool    mBContrast;  //对比度
//    int     mContrast; //对比度

    int     mWinCentre; //窗宽
    int     mWinWidth;  //窗位

    int     mRotate;     //角度
    int     mNeedRotate; //需要旋转的角度


    bool    mBMeasureOpt;
    bool    mBDelImging;
    QMutex     mDelImgLock;

    //
    //**绘制相关***
    DCMFile dmfile;

    DCMFile mSourceFile;

    QRect   mPaintRect;          //图像的Rect  (0, 0, nW, nH)
    QRect   mPaintRectReal;      //图像绘制的Rect

    int     mSourceX;
    int     mSourceY;
    QRect   mSourceRect;        //原图需要显示的Rect
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

    int                 m_selectedIndex;
    QPoint              m_curPosPt;

    QRectF              m_rectTmp;
    QLine               m_lineTmp;
    QPoint              m_beginPosPt;

    QLineEdit           *m_lineEditWdg;
    bool                m_bChangeText;

    GeometryItemBase            *m_geometryItemBase;
    QList<GeometryItemBase *>    m_geometryItemList;

    ItemOperator                *m_itemOperatorTmp;
    QList<ItemOperator *>        m_ItemOperatorList;
    int                          m_curOperatorIndex;

    bool             m_beginMove;
    QPointF          m_originPt;

    bool             m_beginChange;
    QRectF           m_originRect;
    QLineF           m_originLine;

};

#endif // MAINWINDOW_H

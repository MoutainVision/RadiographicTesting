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

//**��ͼ**
#include "rectitem.h"
#include "ellipseitem.h"
#include "lineitem.h"
#include "textitem.h"

#include "geyimgwdg.h"

#include "colorwdg.h"

#include "loading.h"

#include "Windows.h"



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

    //Ԥ�����
    void calcPreWdgPos();

    void deletePreWdgList();

    //
    void setDcmFileInfo();

    void reSize(int newW, int newH);

    void reSize(float scale);

    //������ʾ
    void showNomal();

    //������Ļ
    void showAdapt();

    //�ָ�ͼ��
    void resetImg();

    //���������
    void showScrollBar();

    void showScrollBar(bool status);

    void setScrollRanges(int maxX, int maxY);

    //����ƶ��� �޸Ĺ��������޸�ͼ��λ��
    void setScrollBarOffsetValues(int hValue, int vValue);

    //ͼ��ת��
    void shortImgToImage(unsigned short *pImg, int nW, int nH, QImage &img);

    //��ʾͼ��
    void showImg(unsigned short *pImg, int nW, int nH);

    //������ͼ��
    void delImg();

    //���������ʽ
    void updateCursor();

    //ʶ�� ��������
    void setRecognizeValue(int index, DefectFeat feat);

    //ʶ��ȱ��
    void exeDefectImg();

    //���ȱ��
    void clearDefect();

    //----����------
    void addRetrievalResultValues(vector<RetrievalResult> aRes);

//----�Ҷȷֲ�ͼ------
    QPoint convertImgPt(QPoint wPt);

    QPoint convertWdgPt(QPoint iPt);

    //����Ҷȷֲ�
    void calcIntensityCurve(QPoint p1, QPoint p2);

    void calcCurGreyPos(int length);

    //���ûҶ�ͼ��
    void setIntensityCurveValues(vector<unsigned short> aIntensity);

    //��ռ���Ҷȵ���
    void clearGreyLine();

    //�رջҶȴ���
    void closeGreyWdg();

    void getIntensity(QPoint curPt);


//----�漸��ͼ�����------
    ItemOperator *getPreOperator();
    ItemOperator *getNextOperator();

    void insetOperator(ItemOperator *itemOperator);
    void doPreOperator(ItemOperator *itemOperator);
    void doNextOperator(ItemOperator *itemOperator);

    //���²�����ʷ��¼��ť�飨redo�� undo��
    void updateOperatorStatus();

    //���item
    void clearItemSelected(int index);

    //--mouse--
    void mousePressArrowAction(QPoint pt);

    void mouseReleaseArrowAction();

    void mouseMoveArrowAction(QPoint pt);

    //��ȡ��ק�㷽��
    bool getDragDirection(QList<DragItem> &dragItemList,
                          QPoint pt,
                          DragDirection &dragDirection);

    QPoint getCurOffset();

//**����������**
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
                       qreal perimeter);

private slots:
    void slotBtnClick(bool bClick);

    void textChanged(QString text);

    void slot_tableCellClicked(int row, int col);

    //��Դ�����
    void clicked(QModelIndex index);

    void  expanded(QModelIndex index);

    //������X Y �ı�
    void slot_scrollAreaXChange(int value);

    void slot_scrollAreaYChange(int value);

    //�Ŵ���С
    void slot_sliderValuechange(int value);

    //����λ�仯
    void slot_sliderWinValueChange(int value);

    //--tool widget--
    void slot_btnGroupClick(QAbstractButton *btn);

    //ģ��tab �仯
    void slot_tabCurrentChanged(int index);

//������
    //ɾ��һ��
    void slot_btnEraserToolClick();

    //����
    void slot_btnPreToolClick();

    //�ָ�
    void slot_btnNextToolClick();

    //���
    void slot_btnDeleteToolClick();

    //����ֵ�ı�
    void slot_paramValueChange(int value);




protected:
    void resizeEvent(QResizeEvent *e);

    bool eventFilter(QObject *obj, QEvent *e);

private:
    Ui::MainWindow *ui;

    Loading *m_loadingDlg;

    HANDLE hEvent;

    //��ɫ
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

    //����
    QString mIndexFilePath;
    QString mIndexDataFilePath;
    ofstream mIndexFileOfs;

    PreWdg *mPreWdg;

    RectItem    *mRecheckRectItem;

    //��������
    DCMFileIndexingData mIndexData;



    //ȱ��
    vector<Defect> mADefectList;
    QList<QColor>  mDefectClassColor;
    bool           mBShowDefect;
    bool           mBShowCenter;

    int            mCurDefectIndex;

    unsigned short *m_pImgDefect;  //����ͼ��

    RectItem    *mDefectRectItem;

    //�Ҷ�ͼ��
    QPoint      m_iP1;
    QPoint      m_iP2;

    QLine       mGrayLine;
    QLine       mGrayLineTmp;
    QRect       mGreyRect;
    float       m_angle;

    GeyImgWdg   *mGeyImgWdg;

    RectItem    *mGreyRectItem;

    //--ͼ�������=------
    int     mCurImgWidth;
    int     mCurImgHeight;

    float   mScale;     //����

    //
    bool    mBInvert;  //����
    bool    mBFlip;    //��ת
    bool    mBMirror;  //����
    bool    mBWind;  //����
    bool    mBContrast;  //�Աȶ�

    int     mContrast; //�Աȶ�
    int     mWinCentre; //����
    int     mWinWidth;  //��λ

    int     mRotate;     //�Ƕ�
    int     mNeedRotate; //��Ҫ��ת�ĽǶ�


    bool    mBMeasureOpt;
    bool    mBDelImging;
    QMutex     mDelImgLock;

    //
    //**�������***
    DCMFile dmfile;

    DCMFile mSourceFile;

    QRect   mPaintRect;          //ͼ���Rect  (0, 0, nW, nH)
    QRect   mPaintRectReal;      //ͼ����Ƶ�Rect

    int     mSourceX;
    int     mSourceY;
    QRect   mSourceRect;        //ԭͼ��Ҫ��ʾ��Rect
    QImage  mPaintImg;

    unsigned short *m_pImgPro;  //����ͼ��
    unsigned short *m_pSrcImg;  //ԭʼͼ��
    int     mSrcImgWidth;       //ԭʼͼ���
    int     mSrcImgHeight;      //ԭʼͼ���

    //���
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

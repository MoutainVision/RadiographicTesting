#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <stdint.h>

#include <QColor>
#include <QString>
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QCryptographicHash>
#include <QFile>
#include <QString>
#include <QTranslator>
#include <QDir>
#include <QFont>
#include <QRectF>
#include <QRect>
#include <QLineF>
#include <QLine>
#include <QPolygon>
#include <QPolygonF>
#include <QPainterPath>
#include <QTransform>

class GeometryItemBase;

//class VideoPlayer_Main;
class PPTRecorder;
class VideoEditor_Main;
class RecordeHistory;
class DbOperator;
class VideoPlayer;

#ifndef DCM_FILENODE_H
#define DCM_FILENODE_H
struct DcmFileNode
{
    QString fileName;
    QString filePath;
    QString timeStr;

    QString patientName;

    int width;
    int height;

    int winCentre;  //窗位
    int windWidth;  //窗宽

    QString transFilePath;
    QString md5Str;
};
#endif

/**
 * @brief The MergeState enum  合并的阶段枚举
 */
enum MergeState
{
    NONE,
    TranscodeFile1, //转码文件1
    TranscodeFile2, //转码文件2
    DoingMerge      //正在合并
};

enum EexeMode
{
    E_MergeMode,    // 合并模式
    E_CutMode       //剪辑模式
};


//**比例尺设置**
#ifndef _SCALES_SETTING_
#define _SCALES_SETTING_
struct ScalesSetting
{
    bool    bShowRuler;
    int     horValue;       //水平的值
    int     verValue;       //垂直的值

    int     lineWidth;
    int     fontSize;

    QColor  color;
    int     segCount;       //segSection count
    int     showMode;       //0: horizon 1: vertical 2:horizon && vertical
    int     unitMode;       //0:um  1:mm  2:cm  3:pix

    QString unitModeStr;

    ScalesSetting()
    {
        bShowRuler  = true;
        horValue    = 10;
        verValue    = 10;

        lineWidth   = 2;
        fontSize    = 16;

        color       = QColor(255,0,0);
        segCount    = 5;

        showMode    = 2;
        unitMode    = 0;

        unitModeStr = "um";
    }
};
#endif

//**常规设置**
#ifndef _MEASURE_GENERAL_SETTING_
#define _MEASURE_GENERAL_SETTING_
struct SMeasureGeneralSet
{
    int     resultKeep;
    int     fontSize;
    QColor  color;
    bool    bDimension;

    SMeasureGeneralSet()
    {
        resultKeep = 2;
        fontSize = 16;
        color = QColor(255, 0, 0);
        bDimension = true;
    }
};
#endif

//**测量表格设置**
#ifndef _MEASURE_SHEET_SETTING_
#define _MEASURE_SHEET_SETTING_
struct SMeasureSheetSet
{
    bool    bIndex;
    bool    bName;
    bool    bStart;
    bool    bEnd;
    bool    bCenter;
    bool    bWidth;
    bool    bHeight;
    bool    bMajorAxis;
    bool    bMinorAxis;
    bool    bAngle;
    bool    bArea;
    bool    bPerimeter;

    SMeasureSheetSet()
    {
        bIndex  = true;
        bName   = true;
        bStart  = true;
        bEnd    = true;
        bCenter = true;
        bWidth  = true;
        bHeight = true;
        bMajorAxis = true;
        bMinorAxis = true;
        bAngle     = true;
        bArea      = true;
        bPerimeter = true;
    }
};
#endif

//**标尺类型**
#ifndef _LENGHT_UNIT_
#define _LENGTH_UNIT_
enum ELengthUnit
{
    PIXELUNIT = 0,
    NANOUNIT,
    MICUNIT,
    MILUNIT,
    CENTIUNIT
};
#endif

//**标注类型**
#ifndef _LABEL_TYPE_
#define _LABEL_TYPE_
enum LabelType
{
    NAMELABEL = 0,
    AREALABEL,
    PERIMETERLABEL,
    WIDTHANDHEIGHTLABEL,
    MAJORANDMINORAXISLABEL,
    LENGTHLABEL,
    NONELABEL
};
#endif

//**测量对象---
#ifndef _MEASURE_OBJECT_
#define _MEASURE_OBJECT_
struct MeasureObject
{
    int             lineWidth;
    QColor          lineColor;
    QString         penStyleStr;
    Qt::PenStyle    penStyle;
    bool            bFill;
    QColor          fillColor;

    QString         rectLabelType;
    QString         ellipseLabelType;
    QString         lineLabelType;

    MeasureObject()
    {
        bFill = false;
    }
};
#endif

//**测量设置**
#ifndef _MEASURE_CONFIG_
#define _MEASURE_CONFIG_
struct SMeasureConfig
{
    ScalesSetting       scaleSet;
    SMeasureGeneralSet  generalSet;
    SMeasureSheetSet    sheetSet;
    ELengthUnit         lengthUnit;

    MeasureObject       measureObjectSet;

    SMeasureConfig()
    {
        lengthUnit = PIXELUNIT;

        measureObjectSet.lineWidth = 2;
        measureObjectSet.lineColor = QColor(255, 0, 0);
        measureObjectSet.penStyle = Qt::SolidLine;
        measureObjectSet.penStyleStr = "Solid Line";
        measureObjectSet.bFill = false;
        measureObjectSet.fillColor = QColor(0, 255, 0);

        measureObjectSet.rectLabelType      = "Width/Height";
        measureObjectSet.ellipseLabelType   = "Area";
        measureObjectSet.lineLabelType      = "Length";
    }

    QString getUnitStr()
    {
        QString unitStr;

        switch (lengthUnit) {
        case PIXELUNIT:
            unitStr = "px";
            break;
        case NANOUNIT:
            unitStr = "nm";
            break;
        case MICUNIT:
            unitStr = "μm";
            break;
        case MILUNIT:
            unitStr = "mm";
            break;
        case CENTIUNIT:
            unitStr = "cm";
            break;
        default:
            break;
        }

        return unitStr;
    }
};
#endif


//**拖拽点方向**
#ifndef _DRAG_DIRECTION_
#define _DRAG_DIRECTION_
enum DragDirection
{
    DOWN,
    UP,
    NONEDIR,
    LEFTTOP,
    LEFTBOTTOM,
    RIGHTTOP,
    RIGHTBOTTOM,
    LEFT,
    RIGHT
};
#endif

//**几何图形的状态**
#ifndef _ITEM_STATUS_
#define _ITEM_STATUS_
enum ItemStatus
{
    NORMOAL = 1,
    SELECTEDMOVE,           // 选中移动
    SELECTEDNOMOVE,         //选中不移动
    EDIT,                   //编辑
    SELECTEDDRAG            //选中几何图形拖拽点
};
#endif

//**绘制状态**
#ifndef _DRAW_STATUS_
#define _DRAW_STATUS_
enum DrawStatus
{
    NODRAW,
    BEGINDRAW,
    ENDDRAW,
    DRAWING
};
#endif

//**当前动作**
#ifndef _CUR_ACTION_
#define _CUR_ACTION_
enum CurAction
{
    HANDACTION,
    ARROWACTION,
    ELLIPSEACTION,
    RECTACTION,
    LINEACTION,
    TEXTACTION,
    POLYGONACTION
};
#endif

//**操作类型**
#ifndef _ITEM_OPERATORTYPE_
#define _ITEM_OPERATORTYPE_
enum ItemOperatorType
{
    MOVEOPT,
    DELETEOPT,
    ADDOPT,
    CHANGEOPT,
    COLORCHAGNEOPT,
    LINEWIDTHCHANGEOPT,
    FONTCHANGEOPT,
    TEXTCHANGEOPT,
    PENSTYLEOPT,
    FILLCHANGEOPT,
    FILLCOLORCHANGEOPT,
    AOIOPT
};
#endif

#ifndef _ITEM_OPERATOR_
#define _ITEM_OPERATOR_
struct ItemOperator
{
    GeometryItemBase    *geometryItem;
    ItemOperatorType    type;
    QRectF              originalRect;       //原本矩形
    QRectF              changedRect;        //修改后的矩形

    QLineF              originLine;         //原本线
    QLineF              changeLine;         //修改后的线

    QPointF             originalPt;         //原先的位置信息
    QPointF             movePt;             //移动后的位置信息

    QColor              origiColor;         //原先的颜色
    QColor              changeColor;        //修改的颜色

    int                 origiLineWidth;     //原先的线宽
    int                 changeLineWidth;    //修改的线宽

    QFont               origiFont;          //原先字体
    QFont               changeFont;         //修改后字体

    QString             origiText;          //原先文字内容
    QString             changeText;         //修改后文字

    Qt::PenStyle        oriPenStyle;        //原先笔样式
    Qt::PenStyle        changePenStyle;        //修改笔样式

    bool                oriBFill;           //原先是否填充
    bool                changeBFill;        //修改是否填充

    QColor              oriFillColor;       //原先填充颜色
    QColor              changeFillColor;    //修改填充颜色

    ItemOperator()
    {
        geometryItem = NULL;

        origiText = "";
        changeText = "";
    }
};
#endif

#define PI 3.1412926

class Appconfig
{
public:
    Appconfig();


    static QString AppDataPath_Main; //程序数据主目录
    static QString AppDataPath_Data; //程序数据的data目录
    static QString AppDataPath_Tmp; //临时目录(程序退出时会清空此目录)
    static QString AppDataPath_TmpFile; //程序运行时 创建次文件，退出时删除此文件，用来判断程序是否正常退出
    static QString AppFilePath_Log; //日志目录
    static QString AppFilePath_LogFile; //日志文件
    static QString AppFilePath_EtcFile; //配置信息
    static QString AppFilePath_OpenFile; //默认路径
    static QString AppFilePath_Open_Pre_File; //默认路径


    //--
    static bool gHaveParam;  // 是否是通过传参数进来
    static QString AppFilePath_Video_Online; //视频文件保存目录(直播
    static QString AppFilePath_Video_Weike;  //视频文件保存目录（微课
    static int video_type;              //type = 0, online  type = 1, weike
    static QString video_filepath_name; //filepath name



    static bool gVideoKeepAspectRatio; //视频按比例播放


    static QString AppDataPath;

//    static VideoPlayer_Main *player;
    static PPTRecorder *pptRecorder;
    static VideoEditor_Main *videoEditor;
    static RecordeHistory * recordHistory;

    static VideoPlayer *player;

    static DbOperator *dbOperator;

    static QString bufferToString(QByteArray sendbuf);
    static QByteArray StringToBuffer(QString);

    static QString getFileMd5(QString filePath);

    static void mSleep(int mSecond);

    //配置文件
    static void loadConfigFile();
    static void saveConfigFile();

    /// 检测文件是否是在某个目录下
    static bool IsFileOnPath(QString filePath, QString dirPath);

    static bool deleteAppTempFile(QString filePath);

    static QStringList getDirList(const QDir &dir); //递归获取视频文件

    static void MakeDir(QString dirName);
    static void InitAllDataPath(); //初始化所有数据保存的路径

    ///写日志
    static void WriteLog(QString str);
    static void InitLogFile();

    static bool isAudio(QString suffix);

    static bool isVideo(QString suffix);

};

#endif // APPCONFIG_H

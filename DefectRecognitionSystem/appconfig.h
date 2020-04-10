#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <stdint.h>


#include <QString>
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QCryptographicHash>
#include <QFile>
#include <QString>
#include <QTranslator>
#include <QDir>


//class VideoPlayer_Main;
class PPTRecorder;
class VideoEditor_Main;
class RecordeHistory;
class DbOperator;
class VideoPlayer;

#ifndef FILENODE_H
#define FILENODE_H
struct FileNode
{
    int id;
    QString fileName;
    QString filePath;
    QString timeStr;

    QString transFilePath;

    QString md5Str;

    int type;   //type = 0, online  type = 1, weike
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

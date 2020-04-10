#include "appconfig.h"

#include <QTextStream>
#include <QProcess>
#include <QDesktopWidget>
#include <QDesktopServices>

#include <QByteArray>
#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QTranslator>
#include <QDateTime>
#include <QApplication>
#include <QMessageBox>

#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>

#include <QJsonDocument>
#include <QJsonObject>

#include <QDebug>

#if defined(WIN32)
#include <windows.h>
#include <direct.h>
#include <io.h> //C (Windows)    access
#else
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

void Sleep(long mSeconds)
{
    usleep(mSeconds * 1000);
}

#endif

QString Appconfig::AppDataPath = "";

PPTRecorder *Appconfig::pptRecorder = NULL;
//VideoPlayer_Main *Appconfig::player = NULL;
VideoEditor_Main *Appconfig::videoEditor = NULL;
RecordeHistory * Appconfig::recordHistory = NULL;
DbOperator *Appconfig::dbOperator = NULL;
VideoPlayer *Appconfig::player = NULL;

QString Appconfig::AppDataPath_Tmp; //临时目录(程序退出时会清空此目录)
QString Appconfig::AppFilePath_EtcFile;
QString Appconfig::AppDataPath_Main;
QString Appconfig::AppDataPath_Data;
QString Appconfig::AppDataPath_TmpFile;
QString Appconfig::AppFilePath_Log;
QString Appconfig::AppFilePath_LogFile;
QString Appconfig::AppFilePath_OpenFile;

bool Appconfig::gVideoKeepAspectRatio = true; //按比例显示

QString Appconfig::AppFilePath_Video_Weike;  //微课目录
QString Appconfig::AppFilePath_Video_Online; //直播目录
int Appconfig::video_type = 0;              //type = 0, online  type = 1, weike
QString Appconfig::video_filepath_name;     //filepath name

bool Appconfig::gHaveParam = false;

Appconfig::Appconfig()
{

}


void Appconfig::InitAllDataPath()
{

#if defined(WIN32)
    ///windows数据存储在C盘的数据目录下
    QFileInfo fileInfo(QCoreApplication::applicationFilePath());
    QString exeFileName = fileInfo.baseName(); //当前程序名字

    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
//    QString dataPath = QCoreApplication::applicationDirPath();

    if (dataPath.right(exeFileName.length()) == exeFileName)
    {
        dataPath.remove(dataPath.length() - exeFileName.length(), exeFileName.length());
    }

    if (!dataPath.endsWith("/"))
    {
        dataPath += "/";
    }

    dataPath += "DCMRecognize";

    if (dataPath.right(exeFileName.length()) != exeFileName)
    {
        if (!dataPath.endsWith("/"))
        {
            dataPath += "/";
        }

        dataPath += exeFileName;
    }

#else
    ///Linux则放在程序所在目录下的data目录下
    QFileInfo fileInfo(QCoreApplication::applicationFilePath());

    QString dataPath = fileInfo.absoluteDir().path();

    if (!dataPath.endsWith("/"))
    {
        dataPath += "/";
    }

#endif

    AppDataPath_Main = dataPath;

    AppDataPath_Data = AppDataPath_Main + "/data";


    AppFilePath_Video_Online = AppDataPath_Data + "/Live";
    MakeDir(AppFilePath_Video_Online);

    AppFilePath_Video_Weike = AppDataPath_Data + "/Vod";
    MakeDir(AppFilePath_Video_Weike);

    QString dirName = AppDataPath_Data + "/etc";
    MakeDir(dirName);

    AppFilePath_EtcFile = dirName + "/main.conf";

    AppDataPath_Tmp = AppDataPath_Data + "/tmp";
    AppFilePath_Log = AppDataPath_Data + "/log";

    AppDataPath_TmpFile = AppDataPath_Tmp + "/tmp.txt";

    AppFilePath_OpenFile = "";

    MakeDir(AppDataPath_Data);
    MakeDir(AppFilePath_Log);
    MakeDir(AppDataPath_Tmp);
//qDebug()<<AppDataPath_Main;
    InitLogFile();
}


void Appconfig::loadConfigFile()
{
    QFile file(Appconfig::AppFilePath_EtcFile);
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream fileOut(&file);
        fileOut.setCodec("GBK");  //unicode UTF-8  ANSI
        while (!fileOut.atEnd())
        {
            QString str = fileOut.readLine();
//            qDebug()<<str;

            str = str.remove(" ");

            if (str.isEmpty())
            {
                continue;
            }

            if (str.contains("opendir="))
            {
                str = str.remove("opendir=");
                str.replace("/","\\\\");
                Appconfig::AppFilePath_OpenFile = str;
            }

        }

        file.close();
    }
}

void Appconfig::saveConfigFile()
{
    QFile file(Appconfig::AppFilePath_EtcFile);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream fileOut(&file);
        fileOut.setCodec("GBK");  //unicode UTF-8  ANSI

        QString fileStr = QString("opendir=%1").arg(Appconfig::AppFilePath_OpenFile);
        fileOut<<fileStr;
        fileOut<<"\n";

        file.close();
    }
}

QStringList Appconfig::getDirList(const QDir &dir)
{
    QStringList filePathList;

    QFileInfoList flist = dir.entryInfoList();

    for(int i=0;i<flist.size();i++)
    {
        QFileInfo fileInfo = flist.at(i);

        if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
        {

        }
        else if (fileInfo.isDir())
        {
            qDebug()<<fileInfo.filePath();
            filePathList.append(getDirList(QDir(fileInfo.filePath())));
        }
        else
        {
            if (fileInfo.suffix().toLower() == "mp4" || fileInfo.suffix().toLower() == "flv")
            {
                filePathList.append(fileInfo.filePath());
            }
        }
    }

    return filePathList;
}

bool Appconfig::isAudio(QString suffix)
{
    if (suffix=="mp3" || suffix=="MP3" ||
            suffix=="wma" || suffix=="WMA" ||
            suffix=="wav" || suffix=="WAV")
    {
        return true;
    }

    return false;
}

bool Appconfig::isVideo(QString suffix)
{
    if (suffix=="flv" || suffix=="FLV" ||
            suffix=="rmvb" || suffix=="RMVB"  ||
            suffix=="avi" || suffix=="AVI" ||
            suffix=="MP4" || suffix == "mp4" ||
            suffix=="mkv" || suffix=="MKV"
            )
    {
        return true;
    }

    return false;
}

void Appconfig::InitLogFile()
{
    QDir dir(AppFilePath_Log);

    QFileInfoList fileInfoList = dir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList)
    {
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        if(fileInfo.isFile())
        {
            qint64 t1 = fileInfo.created().toMSecsSinceEpoch();
            qint64 t2 = QDateTime::currentMSecsSinceEpoch();

            qint64 t = (t2 - t1) / 1000; //文件创建到现在的时间（单位：秒）

            if (t >= (24*3600*3)) //删除3天前的日志文件
//            if (t >= (60*20))
            {
                QFile::remove(fileInfo.absoluteFilePath());
            }
        }
    }

    AppFilePath_LogFile = AppFilePath_Log + QString("/log_%1.txt").arg(QDate::currentDate().toString("yyyy-MM-dd"));
    WriteLog("\r\n=======================================\r\n=======================================\r\n[App Start...]\r\n\r\n");

}

void Appconfig::WriteLog(QString str)
{
    qDebug()<<__FUNCTION__<<str;

    QFile file(AppFilePath_LogFile);
    if (file.open(QIODevice::ReadWrite))
    {
        file.seek(file.size());
        QString tmpStr = QString("[%1] %2 \r\n")
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                .arg(str);

        QTextStream fileOut(&file);
        fileOut.setCodec("UTF-8");  //unicode UTF-8  ANSI
        fileOut<<tmpStr;
        file.close();
    }
}

void Appconfig::MakeDir(QString dirName)
{
    QDir dir;
    dir.mkpath(dirName);
}

bool Appconfig::deleteAppTempFile(QString filePath)
{
    QDir dir(filePath);
    if(!dir.exists()){
        return true;
    }

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
    QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
    foreach (QFileInfo file, fileList){ //遍历文件信息
        if (file.isFile()){ // 是文件，删除
            file.dir().remove(file.fileName());
        }else{ // 递归删除
            deleteAppTempFile(file.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath()); // 删除文件夹
}

QString Appconfig::bufferToString(QByteArray sendbuf)
{
    QString tmp;
    for (int k = 0; k < sendbuf.size(); k++)
    {
        QString str = QString("%1").arg(sendbuf[k] & 0xff, 2, 16, QLatin1Char('0'));
//        tmp += str + " ";
        tmp += str;
    }
    tmp = tmp.toUpper();
    return tmp;
}

QByteArray Appconfig::StringToBuffer(QString str)
{
    QString text = str.remove(" ");
    QByteArray sendbuf;
    while(!text.isEmpty())
    {
        QString str = text.left(2);

        if (str.length() == 1)
        {
            str = "0" + str;
        }

        text.remove(0,2);
        int x = str.left(1).toInt(0,16) << 4;
        x += str.right(1).toInt(0,16);
        QByteArray buf;
        buf.resize(1);
        buf[0] = x;
        sendbuf.append(buf);
    }

    return sendbuf;
}

void Appconfig::mSleep(int mSecond)
{
#if defined(WIN32)
    Sleep(mSecond);
#else
    usleep(mSecond * 1000);
#endif
}

QString Appconfig::getFileMd5(QString filePath)
{
    QFile localFile(filePath);

    if (!localFile.open(QFile::ReadOnly))
    {
//        qDebug() << "file open error.";
        return "";
    }

    QCryptographicHash ch(QCryptographicHash::Md5);

    while (!localFile.atEnd())
    {
        QByteArray buffer = localFile.read(1024*1024);
        ch.addData(buffer);
    }

    localFile.close();
    QByteArray md5 = ch.result();
    return Appconfig::bufferToString(md5).toLower();
}

bool Appconfig::IsFileOnPath(QString filePath,QString dirPath)
{
    QFileInfo fileInfo(filePath);
    QDir dir(dirPath);
//    qDebug()<<fileInfo.absolutePath()<<dir.absolutePath();
    if (fileInfo.absolutePath() == dir.absolutePath())
    {
        return true;
    }
    else
    {
        return false;
    }

}

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMessageBox>

#include "Base/FunctionTransfer.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    FunctionTransfer::init(QThread::currentThreadId());

    //分割屏幕
    mSplitter = new QSplitter(Qt::Orientation::Horizontal, ui->centralwidget);//水平
    mSplitter->setStyleSheet("QSplitter:handle{border-right:2px solid #000000;}");

    mSplitter->addWidget(ui->widget_left);
    mSplitter->addWidget(ui->widget_right);

    ui->centralwidget->layout()->addWidget(mSplitter);

    mSplitter->setStretchFactor(1, 3);//左右宽度比为4：1

    //资源树
    mModel  = new QDirModel;

    QStringList filters;
    filters << "*.dcm";

    mModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    mModel->setNameFilters(filters);

    ui->treeView->setModel(mModel);
    ui->treeView->setRootIndex(mModel->index(Appconfig::AppFilePath_OpenFile));

    ui->lineEdit_search->setText(Appconfig::AppFilePath_OpenFile);

    ui->widget_img->installEventFilter(this);
    ui->widget_pre->installEventFilter(this);


    mRecognizeWdg  = nullptr;
    mReCheckWdg = nullptr;

    connect(ui->pushButton_recognize, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_recheck, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_search, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->lineEdit_search, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));

    connect(ui->pushButton_open_pre, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->lineEdit_open_pre, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));

    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClicked(QModelIndex)));
}

void MainWindow::doubleClicked(QModelIndex index)
{
    QString filePath = mModel->filePath(index);

    QFileInfo info;
    info.setFile(filePath);

    if (info.isDir())
        return ;


    QString dateTimeStr = QDateTime::currentDateTime().toString("MM-dd hh-mm-ss-zzz");
    QString outFileName = QString(QStringLiteral("%1/%2.jpg")).arg(Appconfig::AppDataPath_Tmp).arg(dateTimeStr);

//    QString outFileName = "";
    std::thread([=] {
        std::string errorStr;
        //ReadDCMFile::readDCMFile(filePath.toLocal8Bit().toStdString(), outFileName.toLocal8Bit().toStdString(), errorStr);
        ReadDCMFile::readDCMFileLib(filePath.toLocal8Bit().toStdString(), outFileName.toLocal8Bit().toStdString(), errorStr);

        FunctionTransfer::runInMainThread([=]()
        {
            this->setPreviewImg(QString("%1").arg(outFileName));
        });

        std::cout << errorStr << std::endl;

    }).detach();


//    qDebug() <<  filePath << info.baseName() << info.filePath();


}

void MainWindow::textChanged(QString text)
{
//    qDebug() << __FUNCTION__ << text;lineEdit_search

    if (QObject::sender() == ui->lineEdit_search)
    {
        ui->treeView->reset();

        QModelIndex index = mModel->index(text);
        ui->treeView->setRootIndex(index);
    }
    else if (QObject::sender() == ui->lineEdit_open_pre)
    {

    }

}

void MainWindow::slotBtnClick(bool bClick)
{
    if (QObject::sender() == ui->pushButton_recognize)
    {
        if (nullptr == mRecognizeWdg)
        {
            mRecognizeWdg = new RecognizeWdg;
        }

        mRecognizeWdg->showMaximized();
    }
    else if (QObject::sender() == ui->pushButton_recheck)
    {
        if (nullptr == mReCheckWdg)
        {
            mReCheckWdg = new ReCheckWdg;
        }
        mReCheckWdg->showMaximized();
    }
    else if (QObject::sender() == ui->pushButton_search)
    {
        QString s = QFileDialog::getExistingDirectory(
            this, "open file dialog",
                "./");

        if (!s.isEmpty())
        {
            ui->lineEdit_search->setText(s);
        }
        else {
            ui->lineEdit_search->setText("");
        }

        Appconfig::AppFilePath_OpenFile = s;
    }
    else if (QObject::sender() == ui->pushButton_open_pre)
    {
        QString s = QFileDialog::getExistingDirectory(
            this, "open file dialog",
                Appconfig::AppFilePath_Open_Pre_File);

        if (!s.isEmpty())
        {
            ui->lineEdit_open_pre->setText(s);

            if (Appconfig::AppFilePath_Open_Pre_File != s)
            {
                deletePreWdgList();
            }

            Appconfig::AppFilePath_Open_Pre_File = s;

            QDir dir(s);
            QStringList dcmFileList = Appconfig::getDirList(dir);

            std::thread([=] {

                for (int i=0; i<dcmFileList.size(); i++)
                {
                    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));          //设置随机数种子
                    int rand = qrand() % 10000;

                    QString filePath = dcmFileList.at(i);
                    QString dateTimeStr = QDateTime::currentDateTime().toString("MM-dd hh-mm-ss-zzz");
                    QString outFileName = QString(QStringLiteral("%1/%2_%3.jpg")).arg(Appconfig::AppDataPath_Tmp).arg(dateTimeStr).arg(rand);

                    std::string errorStr;
                    ReadDCMFile::readDCMFileLib(filePath.toLocal8Bit().toStdString(), outFileName.toLocal8Bit().toStdString(), errorStr);

                    FunctionTransfer::runInMainThread([=]()
                    {
                        DcmFileNode info;
                        info.transFilePath = outFileName;

                        PreWdg *preWdg = new PreWdg;
                        preWdg->setParent(ui->widget_pre);
                        preWdg->setDCMFileInfo(info);

                        mPreWdgList.push_back(preWdg);

                        calcPreWdgPos();

                    });
                }

            }).detach();

//            for (QString filePath : dcmFileList)
//            {
//                qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));          //设置随机数种子
//                int rand = qrand() % 10000;

//                QString dateTimeStr = QDateTime::currentDateTime().toString("MM-dd hh-mm-ss-zzz");
//                QString outFileName = QString(QStringLiteral("%1/%2_%3.jpg")).arg(Appconfig::AppDataPath_Tmp).arg(dateTimeStr).arg(rand);

//                DcmFileNode info;
//                info.filePath = filePath;
//                info.transFilePath = outFileName;

//                PreWdg *preWdg = new PreWdg;
//                preWdg->setParent(ui->widget_pre);
//                preWdg->setDCMFileInfo(info);

//                mPreWdgList.push_back(preWdg);

//            }

//            calcPreWdgPos();


        }
    }
}

void MainWindow::deletePreWdgList()
{
    for (int i=0; i<mPreWdgList.size(); i++)
    {
        PreWdg *preWdg = mPreWdgList.at(i);
        delete preWdg;
        preWdg = NULL;
    }

    mPreWdgList.clear();
}

void MainWindow::calcPreWdgPos()
{
    int conWidth = ui->widget_pre->width();
//    int conHeight = ui->widget_pre->height();

    int res = conWidth % MAX_PRE_WIDGET_WIDTH;
    int cnt = conWidth / MAX_PRE_WIDGET_WIDTH;

    if (res > 0)
        cnt ++;

    int perWidth = conWidth / cnt;
    int perHeight = perWidth;

    int curRow = 0;
    int curCol = 0;
    int posX = 0;
    int posY = 0;

    qDebug() << __FUNCTION__ << cnt;

    int preWdgSize = mPreWdgList.size();

    for (int i=0; i<mPreWdgList.size(); i++)
    {
        PreWdg *preWdg = mPreWdgList.at(i);

        posX = perWidth * curCol;
        posY = perHeight * curRow;

        qDebug() << "curCol:" << curCol <<   "curRow:"  << curRow;
        qDebug() << "posX:" << posX <<   "posY:"  << posY;

        preWdg->setFixedSize(perWidth, perHeight);
        preWdg->setGeometry(posX, posY, perWidth, perHeight);
        preWdg->show();

        curCol ++;

        if (curCol == cnt)
        {
            curCol = 0;
            curRow ++;
        }
    }

    ui->widget_pre->setFixedHeight(curRow*perHeight + 100);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    return QMainWindow::resizeEvent(e);
}

void MainWindow::setPreviewImg(QString filePath)
{
    qDebug() << __FUNCTION__ << filePath;
    mPreviewPixImg.load(filePath);
    update();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == ui->widget_img)
    {
        if (e->type() == QEvent::Paint)
        {
            QPainter  p(ui->widget_img);

            if (!mPreviewPixImg.isNull())
            {
                int nX = 0;
                int nY = 0;
                int nWidth = 0;
                int nHeight = 0;
                int nW = mPreviewPixImg.width();
                int nH = mPreviewPixImg.height();

                double dRatioW = (double)(ui->widget_img->width()) / (double)nW;
                double dRatioH = (double)(ui->widget_img->height()) / (double)nH;
                double dRat = dRatioW > dRatioH? dRatioH : dRatioW;

                nWidth = dRat * nW;
                nHeight = dRat * nH;
                nX = dRatioW > dRatioH? ((ui->widget_img->width()) - nWidth) / 2 : 0;
                nY = dRatioW > dRatioH? 0 : ((ui->widget_img->height()) - nHeight) / 2;

                QRect imgRect(nX, nY, nWidth, nHeight);

                QBrush b(QColor("#000000"));
                p.setBrush(b);
                p.drawRect(ui->widget_img->rect());

                p.drawPixmap(imgRect, mPreviewPixImg);

                return true;
            }
        }
    }
    else if (obj == ui->widget_pre)
    {
        if (e->type() == QEvent::Resize)
        {
            calcPreWdgPos();
        }
    }

    return QMainWindow::eventFilter(obj, e);
}

MainWindow::~MainWindow()
{
    if (nullptr != mModel)
    {
        delete mModel;
        mModel = nullptr;
    }

    if (nullptr != mSplitter)
    {
        delete mSplitter;
        mSplitter = nullptr;
    }

    //save config
    Appconfig::saveConfigFile();

    //delete temp file
    Appconfig::deleteAppTempFile(Appconfig::AppDataPath_Tmp);

    delete ui;
}


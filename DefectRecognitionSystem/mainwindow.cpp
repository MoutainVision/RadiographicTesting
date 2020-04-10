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


    mModel  = new QDirModel;

    QStringList filters;
    filters << "*.dcm";

    mModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    mModel->setNameFilters(filters);

    ui->treeView->setModel(mModel);
    ui->treeView->setRootIndex(mModel->index(Appconfig::AppFilePath_OpenFile));

    ui->lineEdit_search->setText(Appconfig::AppFilePath_OpenFile);

    ui->widget_img->installEventFilter(this);


    mRecognizeWdg  = nullptr;
    mReCheckWdg = nullptr;

    connect(ui->pushButton_recognize, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_recheck, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_search, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->lineEdit_search, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));

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


    std::thread([=] {
        std::string errorStr;
        ReadDCMFile::readDCMFile(filePath.toLocal8Bit().toStdString(), outFileName.toLocal8Bit().toStdString(), errorStr);

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
//    qDebug() << __FUNCTION__ << text;

    ui->treeView->reset();

    QModelIndex index = mModel->index(text);
    ui->treeView->setRootIndex(index);
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

//                QBrush b(QColor("#FFFFFF"));
//                p.setBrush(b);
                p.drawRect(ui->widget_img->rect());

                p.drawPixmap(imgRect, mPreviewPixImg);

                return true;
            }
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

    //save config
    Appconfig::saveConfigFile();

    //delete temp file
    Appconfig::deleteAppTempFile(Appconfig::AppDataPath_Tmp);

    delete ui;
}

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

	mDcmFileList = Appconfig::getDirList(Appconfig::AppFilePath_OpenFile);
	mDcmCount = mDcmFileList.size();

    ui->widget_img->installEventFilter(this);
    ui->widget_pre->installEventFilter(this);

    ui->widget_img_pre->installEventFilter(this);

    ui->widget_tool->installEventFilter(this);
    ui->widget_tool->setMouseTracking(true);

    ui->horizontalScrollBar->installEventFilter(this);
    ui->horizontalScrollBar->setMouseTracking(true);

    ui->verticalScrollBar->installEventFilter(this);
    ui->verticalScrollBar->setMouseTracking(true);

    ui->verticalSlider_diameter->installEventFilter(this);
    ui->verticalSlider_diameter->setMouseTracking(true);

    ui->pushButton_cross->installEventFilter(this);
    ui->pushButton_cross->setMouseTracking(true);

    ui->pushButton_next_big->installEventFilter(this);
    ui->pushButton_next_big->setMouseTracking(true);

    ui->pushButton_pre_big->installEventFilter(this);
    ui->pushButton_pre_big->setMouseTracking(true);

    //
    mDefectClassColor.push_back(QColor("#f7acbc"));
    mDefectClassColor.push_back(QColor("#f47920"));
    mDefectClassColor.push_back(QColor("#726930"));
    mDefectClassColor.push_back(QColor("#426ab3"));
    mDefectClassColor.push_back(QColor("#b2d235"));
    mDefectClassColor.push_back(QColor("#843900"));
    mDefectClassColor.push_back(QColor("#d93a49"));
    mDefectClassColor.push_back(QColor("#007d65"));
    mDefectClassColor.push_back(QColor("#dea32c"));

    ui->widget_recognize_table->hide();
    ui->widget_measure->hide();
    ui->widget_tool->hide();

    ui->horizontalScrollBar->hide();
    ui->verticalScrollBar->hide();

    ui->tabWidget->setCurrentIndex(0);

    mRecognizeWdg  = nullptr;
    mReCheckWdg = nullptr;

    mCurIndex = 0;
    mDcmCount = 0;

    //
    m_pImgPro = nullptr;
    m_pSrcImg = nullptr;
    m_pImgDefect = nullptr;

    mBInvert = false;
    mBFlip   = false;
    mBMirror = false;

    mBShowDefect = true;
    mBShowCenter = true;

    mSourceX = 0;
    mSourceY = 0;

    m_eCurAction = ARROWACTION;
    mBMeasureOpt = false;

    mRotate = 0;

    ui->verticalSlider_diameter->setRange(5, 200);

    connect(ui->pushButton_search, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_pre_big, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_next_big, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));

    connect(ui->lineEdit_search, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));

    connect(ui->pushButton_open_pre, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->lineEdit_open_pre, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));

    //常规
    connect(ui->pushButton_measure, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_nomal, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_adapt, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_measure_table, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));

    //调整
    connect(ui->pushButton_reset, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_invert, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_Mirror, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_Flip, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_mag, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));

    connect(ui->pushButton_rotate, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));

    //
    connect(ui->pushButton_aoi, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_recog_show_table, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_exe, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_recog_clear, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_show_defect, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_show_defect_center, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));


    connect(ui->verticalSlider_WinCentre, SIGNAL(valueChanged(int)), this, SLOT(slot_sliderWinValueChange(int)));
    connect(ui->verticalSlider_WindWidth, SIGNAL(valueChanged(int)), this, SLOT(slot_sliderWinValueChange(int)));


    connect(ui->horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(slot_scrollAreaXChange(int)));
    connect(ui->verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(slot_scrollAreaYChange(int)));

    connect(ui->verticalSlider_diameter, SIGNAL(sliderReleased()), this, SLOT(slot_sliderReleased()));

    connect(ui->buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(slot_btnGroupClick(QAbstractButton*)));

    connect(ui->treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(clicked(QModelIndex)));
    connect(ui->treeView, SIGNAL(expanded(QModelIndex)), this, SLOT(expanded(QModelIndex)));

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slot_tabCurrentChanged(int)));



}

void MainWindow::slot_tabCurrentChanged(int index)
{
    if (index == 0 || index == 1)
    {
        ui->pushButton_pre_big->show();
        ui->pushButton_next_big->show();
    }
    else
    {
        ui->pushButton_pre_big->hide();
        ui->pushButton_next_big->hide();
    }
}

void MainWindow::slot_sliderReleased()
{
    int value = ui->verticalSlider_diameter->value();
    mScale = (float)value / 100;

    ui->lineEdit_diameter->setText(QString("%1").arg(value));

    delImg();
}


void MainWindow::slot_sliderWinValueChange(int value)
{
    if (QObject::sender() == ui->verticalSlider_WinCentre)
    {
        mWinCentre = value;
        ui->label_wincentre->setText(QString("%1").arg(value));
    }
    else if (QObject::sender() == ui->verticalSlider_WindWidth)
    {
        mWinWidth = value;
        ui->label_winwidth->setText(QString("%1").arg(value));
    }

//    delImg();
}

void MainWindow::setDcmFileInfo()
{
    ui->verticalSlider_WinCentre->blockSignals(true);
    ui->verticalSlider_WindWidth->blockSignals(true);


    ui->verticalSlider_WinCentre->blockSignals(false);
    ui->verticalSlider_WindWidth->blockSignals(false);

    //reset
    ui->pushButton_measure_table->setChecked(false);
    ui->tableWidget_measure->hide();

    ui->pushButton_recog_show_table->setChecked(false);
    ui->tableWidget_recognize->hide();

    ui->pushButton_measure->setChecked(false);
    ui->widget_tool->hide();

    ui->pushButton_invert->setChecked(false);
    mBInvert = false;

    ui->pushButton_Mirror->setChecked(false);
    mBMirror = false;

    ui->pushButton_Flip->setChecked(false);
    mBFlip = false;

    mRotate = 0;
    mNeedRotate = 0;

    //清空缺陷
    clearDefect();


    //load
    std::thread([=] {

        dmfile.Load(mCurDcmFileInfo.filePath.toLocal8Bit().toStdString().c_str());

        mCurDcmFileInfo.winCentre = dmfile.GetWindowCenter();
        mCurDcmFileInfo.windWidth = dmfile.GetWindowWidth();

        mCurDcmFileInfo.width = dmfile.GetWidth();
        mCurDcmFileInfo.height = dmfile.GetHeight();

        mWinCentre = mCurDcmFileInfo.winCentre;
        mWinWidth  = mCurDcmFileInfo.windWidth;

        qDebug() << mWinCentre <<":" << mWinWidth;

        FunctionTransfer::runInMainThread([=]()
        {
            ui->verticalSlider_WinCentre->setValue(mCurDcmFileInfo.winCentre);
            ui->verticalSlider_WindWidth->setValue(mCurDcmFileInfo.windWidth);

            m_pSrcImg = dmfile.GetBuffer();
            mSrcImgWidth = dmfile.GetWidth();
            mSrcImgHeight = dmfile.GetHeight();

            m_pImgPro = new unsigned short[mSrcImgWidth * mSrcImgHeight];
            memcpy(m_pImgPro, m_pSrcImg, mSrcImgWidth*mSrcImgHeight*sizeof(unsigned short));

            this->showAdapt();
        });

    }).detach();
}

void MainWindow::shortImgToImage(unsigned short *pImg, int nW, int nH, QImage &img)
{
    unsigned char *pImgMono8 = new unsigned char[nW*nH];
    long maxI, minI;
    if (Convert(pImgMono8, pImg, nW, nW, nH, maxI, minI))
    {
        QImage imageT(nW, nH, QImage::Format_RGB32);
        QRgb value;

        for (int y=0; y<nH; y++)
          for (int x=0; x<nW; x++)
          {
              value = qRgb(pImgMono8[y*nW+x], pImgMono8[y*nW+x], pImgMono8[y*nW+x]); // 0xffbd9527
              imageT.setPixel(x, y, value);
          }

        img = imageT.copy();
    }

    delete []pImgMono8;
}

void MainWindow::showAdapt()
{
    int wdgW = ui->widget_img_pre->width();
    int wdgH = ui->widget_img_pre->height();

    double dRatioW = (double)(wdgW) / (double)mSrcImgWidth;
    double dRatioH = (double)(wdgH) / (double)mSrcImgHeight;
    double dRat = dRatioW > dRatioH? dRatioH : dRatioW;

    mScale = dRat;

    ui->verticalSlider_diameter->setValue((int)(mScale*100));
    ui->lineEdit_diameter->setText(QString("%1").arg((int)(mScale*100)));

    delImg();
}


void MainWindow::resetImg()
{
    ui->pushButton_invert->setChecked(false);
    mBInvert = false;

    ui->pushButton_Mirror->setChecked(false);
    mBMirror = false;

    ui->pushButton_Flip->setChecked(false);
    mBFlip = false;

    mRotate = 0;
    mNeedRotate = 0;

    //适配显示
    showAdapt();
}

void MainWindow::showNomal()
{
    mScale = 1.0;

    ui->verticalSlider_diameter->setValue((int)(mScale*100));
    ui->lineEdit_diameter->setText(QString("%1").arg((int)(mScale*100)));

    delImg();
}

void MainWindow::reSize(int newW, int newH)
{
    mScale = mSrcImgWidth / newW;

    int srcW = mCurImgWidth;
    int srcH = mCurImgHeight;

    Resize(m_pImgPro, srcW, srcH, newW, newH);

    mCurImgWidth  = srcW;
    mCurImgHeight = srcH;
}

void MainWindow::reSize(float scale)
{
    mScale = scale;

    mCurImgWidth = scale * mSrcImgWidth;
    mCurImgHeight = scale * mSrcImgHeight;

    int srcW = mSrcImgWidth;
    int srcH = mSrcImgHeight;

    qDebug() << "pre:" << mCurImgWidth << mCurImgHeight;

    Resize(m_pImgPro, srcW, srcH, mCurImgWidth, mCurImgHeight);

    mCurImgWidth = srcW;
    mCurImgHeight = srcH;

    qDebug() << "end:" << mCurImgWidth << mCurImgHeight;
}

void  MainWindow::expanded(QModelIndex index)
{
    QString filePath = mModel->filePath(index);

//    mDcmFileList = Appconfig::getDirList(filePath);
//    mDcmCount = mDcmFileList.size();
}


void MainWindow::showImg(unsigned short *pImg, int nW, int nH)
{
    shortImgToImage(pImg, nW, nH, mPaintImg);

    mPaintRect.setRect(0, 0, nW, nH);

    showScrollBar();

    update();
}

void MainWindow::setScrollRanges(int maxX, int maxY)
{
    ui->horizontalScrollBar->setRange(30, maxX);
    ui->verticalScrollBar->setRange(30, maxY);
}


void MainWindow::showScrollBar(bool status)
{
    if (!status)
    {
        ui->horizontalScrollBar->hide();
        ui->verticalScrollBar->hide();
    }
    else
    {
        setScrollRanges(mPaintRect.width() - ui->widget_img_pre->width(), mPaintRect.height() - ui->widget_img_pre->height());

        if (mPaintRect.width() > ui->widget_img_pre->width())
        {
            ui->horizontalScrollBar->show();
            ui->horizontalScrollBar->setValue((mPaintRect.width()-ui->widget_img_pre->width())/2);
        }
        else
        {
            mSourceX = 0;
            ui->horizontalScrollBar->hide();
        }

        if (mPaintRect.height() > ui->widget_img_pre->height())
        {
            ui->verticalScrollBar->show();
            ui->verticalScrollBar->setValue((mPaintRect.height() - ui->widget_img_pre->height())/2);
        }
        else
        {
            mSourceY = 0;
            ui->horizontalScrollBar->hide();
        }
    }
}


void MainWindow::showScrollBar()
{
    if (mPaintRect.width() > ui->widget_img_pre->width()
            || mPaintRect.height() > ui->widget_img_pre->height())
    {
        showScrollBar(true);
    }
    else
        showScrollBar(false);
}

void MainWindow::slot_scrollAreaXChange(int value)
{
    if (!ui->horizontalScrollBar->isHidden())
    {
        int nW = ui->widget_img_pre->width();
        int nH = ui->widget_img_pre->height();

        mSourceX = value;

        if (mPaintRect.height() <= nH)
            mSourceRect = QRect(mSourceX, mSourceY, nW, mPaintRect.height());
        else
            mSourceRect = QRect(mSourceX, mSourceY, nW, nH);

        update();
    }
}

void MainWindow::slot_scrollAreaYChange(int value)
{
    if (!ui->verticalScrollBar->isHidden())
    {
        int nW = ui->widget_img_pre->width();
        int nH = ui->widget_img_pre->height();

        mSourceY = value;

        if (mPaintRect.width() <= nW)
            mSourceRect = QRect(mSourceX, mSourceY, mPaintRect.width(), nH);
        else
            mSourceRect = QRect(mSourceX, mSourceY, nW, nH);


        update();
    }
}


void MainWindow::clicked(QModelIndex index)
{
    QString filePath = mModel->filePath(index);

    mCurIndex = index.row();

    qDebug() << __FUNCTION__ << filePath;

    QFileInfo info;
    info.setFile(filePath);

	if (info.isDir())
		return;

    QString filePathParent = mModel->filePath(index.parent());

    if (!filePathParent.isEmpty())
    {
        mDcmFileList = Appconfig::getDirList(filePathParent);
        mDcmCount = mDcmFileList.size();
    }

    QString dateTimeStr = QDateTime::currentDateTime().toString("MM-dd hh-mm-ss-zzz");
    QString outFileName = QString(QStringLiteral("%1/%2.jpg")).arg(Appconfig::AppDataPath_Tmp).arg(dateTimeStr);


    mCurDcmFileInfo.filePath = filePath;
    mCurDcmFileInfo.transFilePath = outFileName;

    setDcmFileInfo();
}

void MainWindow::textChanged(QString text)
{
//    qDebug() << __FUNCTION__ << text;lineEdit_search

    if (QObject::sender() == ui->lineEdit_search)
    {
        mDcmFileList.clear();
        ui->treeView->reset();

        QModelIndex index = mModel->index(text);
        ui->treeView->setRootIndex(index);

        mDcmFileList = Appconfig::getDirList(text);
        mDcmCount = mDcmFileList.size();
    }
    else if (QObject::sender() == ui->lineEdit_open_pre)
    {

    }

}

void MainWindow::clearDefect()
{
    mADefectList.clear();
    ui->tableWidget_recognize->clearContents();
    ui->tableWidget_recognize->setRowCount(0);

    update();
}

void MainWindow::setRecognizeValue(int index, DefectFeat feat)
{
    int row = ui->tableWidget_recognize->rowCount();
    ui->tableWidget_recognize->setRowCount(row + 1);

    QTableWidgetItem *indexItem = new QTableWidgetItem;
    indexItem->setText(QString("%1").arg(index));
    indexItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *areaItem = new QTableWidgetItem;
    areaItem->setText(QString("%1").arg(feat.area));
    areaItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *periItem = new QTableWidgetItem;
    periItem->setText(QString("%1").arg(feat.peri));
    periItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *periAreaItem = new QTableWidgetItem;
    periAreaItem->setText(QString("%1").arg(feat.peri_area_ratio));
    periAreaItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *lengthItem = new QTableWidgetItem;
    lengthItem->setText(QString("%1").arg(feat.length));
    lengthItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *widthItem = new QTableWidgetItem;
    widthItem->setText(QString("%1").arg(feat.width));
    widthItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *rndnessItem = new QTableWidgetItem;
    rndnessItem->setText(QString("%1").arg(feat.rndness));
    rndnessItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *rectnessItem = new QTableWidgetItem;
    rectnessItem->setText(QString("%1").arg(feat.rectness));
    rectnessItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *aveGreyItem = new QTableWidgetItem;
    aveGreyItem->setText(QString("%1").arg(feat.aveGrey));
    aveGreyItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *greyContrastItem = new QTableWidgetItem;
    greyContrastItem->setText(QString("%1").arg(feat.grey_contrast));
    greyContrastItem->setTextAlignment(Qt::AlignCenter);


    //--additems--
    ui->tableWidget_recognize->setItem(row, 0, indexItem);
    ui->tableWidget_recognize->setItem(row, 1, areaItem);
    ui->tableWidget_recognize->setItem(row, 2, periItem);
    ui->tableWidget_recognize->setItem(row, 3, periAreaItem);
    ui->tableWidget_recognize->setItem(row, 4, lengthItem);
    ui->tableWidget_recognize->setItem(row, 5, widthItem);
    ui->tableWidget_recognize->setItem(row, 6, rndnessItem);
    ui->tableWidget_recognize->setItem(row, 7, rectnessItem);
    ui->tableWidget_recognize->setItem(row, 8, aveGreyItem);
    ui->tableWidget_recognize->setItem(row, 9, greyContrastItem);
}

void MainWindow::slotBtnClick(bool bClick)
{
    if (QObject::sender() == ui->pushButton_next_big)
    {
        if (mModel->rowCount() > 0)
        {
            int preIn = mCurIndex + 1;

            if (preIn < mDcmFileList.size())
            {
                QModelIndex preIndex = mModel->index(mDcmFileList.at(preIn));
                clicked(preIndex);
            }
        }
    }
    else if (QObject::sender() == ui->pushButton_pre_big)
    {
        if (mModel->rowCount() > 0)
        {
            int preIn = mCurIndex - 1;

            if (preIn >= 0 && preIn < mDcmFileList.size())
            {
                QModelIndex preIndex = mModel->index(mDcmFileList.at(preIn));
                clicked(preIndex);
            }
        }
    }
    else if (QObject::sender() == ui->pushButton_measure_table)
    {
        ui->widget_measure->setVisible(ui->pushButton_measure_table->isChecked());
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
        }
    }
    else if (QObject::sender() == ui->pushButton_measure)
    {
        mBMeasureOpt = ui->pushButton_measure->isChecked();
        ui->widget_tool->setVisible(mBMeasureOpt);
    }
    else if (QObject::sender() == ui->pushButton_adapt)
    {
        showAdapt();
    }
    else if (QObject::sender() == ui->pushButton_nomal)
    {
        showNomal();
    }
    else if (QObject::sender() == ui->pushButton_reset)
    {
        resetImg();
    }
    else if (QObject::sender() == ui->pushButton_invert)
    {
        mBInvert = ui->pushButton_invert->isChecked();
        delImg();
    }
    else if (QObject::sender() == ui->pushButton_Flip)
    {
        mBFlip = ui->pushButton_Flip->isChecked();
        delImg();
    }
    else if (QObject::sender() == ui->pushButton_Mirror)
    {
        mBMirror = ui->pushButton_Mirror->isChecked();
        delImg();
    }
    else if (QObject::sender() == ui->pushButton_rotate)
    {
        mRotate += 90;

        mRotate = mRotate % 360;

        if ((mBFlip && !mBMirror) || (!mBFlip && mBMirror))
        {
            mNeedRotate += 90;
            mNeedRotate = mNeedRotate % 360;
        }
        else
        {
            mNeedRotate = mRotate;
        }

        delImg();
    }
    else if (QObject::sender() == ui->pushButton_aoi)
    {

    }
    else if (QObject::sender() == ui->pushButton_recog_show_table)
    {
        ui->widget_recognize_table->setVisible(ui->pushButton_recog_show_table->isChecked());
    }
    else if (QObject::sender() == ui->pushButton_show_defect)
    {
        mBShowDefect = ui->pushButton_show_defect->isChecked();
        update();
    }
    else if (QObject::sender() == ui->pushButton_show_defect_center)
    {
        mBShowCenter = ui->pushButton_show_defect_center->isChecked();
        update();
    }
    else if (QObject::sender() == ui->pushButton_exe)
    {
        exeDefectImg();
    }
    else if (QObject::sender() == ui->pushButton_recog_clear)
    {
        clearDefect();
    }
}


void MainWindow::exeDefectImg()
{
    if (nullptr != m_pImgDefect)
    {
        delete []m_pImgDefect;
        m_pImgDefect = nullptr;
    }

    //拷贝
    m_pImgDefect = new unsigned short[mSrcImgWidth * mSrcImgHeight];
    memcpy(m_pImgDefect, m_pSrcImg, mSrcImgWidth*mSrcImgHeight*sizeof(unsigned short));

    int imgW  = mSrcImgWidth;
    int imgH = mSrcImgHeight;


    //反相
    if (mBInvert)
        Invert(m_pImgDefect, imgW, imgH);

    //镜像
    if (mBMirror)
        Mirror(m_pImgDefect, imgW, imgH);

    //翻转
    if (mBFlip)
        Flip(m_pImgDefect, imgW, imgH);

    //旋转
    if (mNeedRotate == 90)
        Rotate90(m_pImgDefect, imgW, imgH);
    else if (mNeedRotate == 180)
        Rotate180(m_pImgDefect, imgW, imgH);
    else if (mNeedRotate == 180)
        Rotate270(m_pImgDefect, imgW, imgH);

    DetectParam param;
    param.nGreyDiff     = ui->spinBox_GreyDiff->value();
    param.nConnectThr   = ui->spinBox_ConnectThr->value();
    param.nFilterRadius = ui->spinBox_FilterRadius->value();
    param.nMinDefectArea = ui->spinBox_MinDefectArea->value();

    clearDefect();

    ImageRect pRoi(50, 500, 50, 500);

    std::thread([&] {

        DetectDefect(mADefectList, m_pImgDefect, imgW, imgH, &pRoi, &param);

        FunctionTransfer::runInMainThread([=]()
        {
            for(int i=0; i<mADefectList.size(); i++)
            {
                setRecognizeValue(i, mADefectList.at(i).feat);
            }

            update();
        });

    }).detach();
}

void MainWindow::delImg()
{
//    std::thread([=] {
        //删除
        if (nullptr != m_pImgPro)
        {
            delete []m_pImgPro;
            m_pImgPro = NULL;
        }

        //拷贝
        m_pImgPro = new unsigned short[mSrcImgWidth * mSrcImgHeight];
        memcpy(m_pImgPro, m_pSrcImg, mSrcImgWidth*mSrcImgHeight*sizeof(unsigned short));

        mCurImgWidth  = mSrcImgWidth;
        mCurImgHeight = mSrcImgHeight;

        //反相
        if (mBInvert)
            Invert(m_pImgPro, mCurImgWidth, mCurImgHeight);

        //镜像
        if (mBMirror)
            Mirror(m_pImgPro, mCurImgWidth, mCurImgHeight);

        //翻转
        if (mBFlip)
            Flip(m_pImgPro, mCurImgWidth, mCurImgHeight);

        //窗宽窗位
//        WindowLevelTransform(m_pImgPro, mCurImgWidth, mCurImgHeight, mWinCentre, mWinWidth);

        //resize
        reSize(mScale);

        //旋转
        if (mNeedRotate == 90)
            Rotate90(m_pImgPro, mCurImgWidth, mCurImgHeight);
        else if (mNeedRotate == 180)
            Rotate180(m_pImgPro, mCurImgWidth, mCurImgHeight);
        else if (mNeedRotate == 180)
            Rotate270(m_pImgDefect, mCurImgWidth, mCurImgHeight);

        mBDelImging = false;

        FunctionTransfer::runInMainThread([=]()
        {
            //显示
            showImg(m_pImgPro, mCurImgWidth, mCurImgHeight);
        });

//    }).detach();

}


void MainWindow::slot_btnGroupClick(QAbstractButton *btn)
{
//    hideColorWdg();

    setCursor(Qt::ArrowCursor);

    if (btn == ui->pushButton_hand)
    {
        m_eCurAction = HANDACTION;
        setCursor(Qt::OpenHandCursor);
    }
    else if (btn == ui->pushButton_arrow)
    {
        m_eCurAction = ARROWACTION;
    }
    else if (btn == ui->pushButton_ellipse)
    {
        m_eCurAction = ELLIPSEACTION;
    }
    else if (btn == ui->pushButton_rect)
    {
        m_eCurAction = RECTACTION;
    }
    else if (btn == ui->pushButton_line)
    {
        m_eCurAction = LINEACTION;
    }
    else if (btn == ui->pushButton_text)
    {
        m_eCurAction = TEXTACTION;
    }

//    slot_btnColorClick(m_measureSetting.measureObjectSet.lineColor.name(), false);
//    ui->comboBox_linebold->setCurrentText(QString("%1").arg(m_measureSetting.measureObjectSet.lineWidth));

//    setPainterStyle();
}


void MainWindow::updateCursor()
{
    if (m_eCurAction == HANDACTION && mBMeasureOpt)
    {
        setCursor(Qt::OpenHandCursor);
    }
    else
    {
        if (mPaintRect.width() > ui->widget_img_pre->width() ||
            mPaintRect.height() > ui->widget_img_pre->height())
        {
            setCursor(Qt::OpenHandCursor);
        }
        else
            setCursor(Qt::ArrowCursor);
    }
}

void MainWindow::setScrollBarOffsetValues(int hValue, int vValue)
{
    if (ui->horizontalScrollBar->isVisible())
    {
        int value = ui->horizontalScrollBar->value() + hValue;

        if (value < 0)
            value = 0;

        if (value > ui->horizontalScrollBar->maximum())
            value = ui->horizontalScrollBar->maximum();

        ui->horizontalScrollBar->setValue(value);
    }

    if (ui->verticalScrollBar->isVisible())
    {
        int value = ui->verticalScrollBar->value() + vValue;

        if (value < 0)
            value = 0;

        if (value > ui->verticalScrollBar->maximum())
            value = ui->verticalScrollBar->maximum();

        ui->verticalScrollBar->setValue(value);
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

bool MainWindow::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == ui->widget_pre)
    {
        if (e->type() == QEvent::Resize)
        {
            calcPreWdgPos();
        }
    }
    else if (obj == ui->widget_img_pre)
    {
        if (e->type() == QEvent::Paint)
        {
            QPainter  p(ui->widget_img_pre);

            if (!mPaintImg.isNull())
            {
                int nHeight = ui->widget_img_pre->height();
                int nWidth = ui->widget_img_pre->width();

                p.save();
                QBrush b(QColor("#000000"));
                p.setBrush(b);
                p.drawRect(ui->widget_img_pre->rect());


                QRect paintRectTmp;

                if (mPaintRect.width() <= nWidth && mPaintRect.height() <= nHeight)
                {
                    paintRectTmp.setRect((nWidth-mPaintRect.width())/2,
                                         (nHeight-mPaintRect.height())/2,
                                         mPaintRect.width(),
                                         mPaintRect.height());

                    mSourceRect = QRect(0, 0, mPaintRect.width(), mPaintRect.height());

                    p.drawImage(paintRectTmp, mPaintImg);
                }
                else if (mPaintRect.width() <= nWidth && mPaintRect.height() > nHeight)
                {
                    paintRectTmp.setRect((nWidth-mPaintRect.width())/2, 0,
                                         mPaintRect.width(),
                                         nHeight);

                    p.drawImage(paintRectTmp, mPaintImg, mSourceRect);
                }
                else if (mPaintRect.width() > nWidth && mPaintRect.height() <= nHeight)
                {
                    paintRectTmp.setRect(0, (nHeight-mPaintRect.height())/2,
                                         nWidth,
                                         mPaintRect.height());
                    p.drawImage(paintRectTmp, mPaintImg, mSourceRect);
                }
                else
                {
                    paintRectTmp.setRect(0, 0, nWidth, nHeight);
                    p.drawImage(paintRectTmp, mPaintImg, mSourceRect);
                }

                if (mBShowDefect || mBShowCenter)
                {
                    for (int i=0; i<mADefectList.size(); i++)
                    {
                        int deClass = mADefectList.at(i).iClass;
                        QColor color = mDefectClassColor.at(deClass+1);

                        vector<POINT> aPt = mADefectList.at(i).aPt;


                        if (mBShowDefect)
                        {
                            for (int j=0; j<aPt.size(); j++)
                            {
                                QPoint pt(aPt.at(j).x * mScale-mSourceRect.x() + paintRectTmp.x(),
                                          aPt.at(j).y * mScale - mSourceRect.y() + paintRectTmp.y());

                                if (paintRectTmp.contains(pt))
                                {
                                    p.setPen(color);
                                    p.drawPoint(pt);
                                }
                            }
                        }


                        if (mBShowCenter)
                        {
                            //--center--
                            int length = 20 * mScale;

                            if (length > 20)
                                length = 20;
                            else if (length < 5)
                                length = 5;

                            QPoint centerPt = QPoint(mADefectList.at(i).center.x * mScale,
                                                     mADefectList.at(i).center.y * mScale);

                            p.setPen(QColor("#ff0000"));
                            p.drawLine(centerPt.x() - mSourceRect.x() -length + paintRectTmp.x(),
                                       centerPt.y() - mSourceRect.y() + paintRectTmp.y(),
                                       centerPt.x() - mSourceRect.x() + length + paintRectTmp.x(),
                                       centerPt.y() - mSourceRect.y() + paintRectTmp.y());

                            p.drawLine(centerPt.x() - mSourceRect.x() + paintRectTmp.x(),
                                       centerPt.y() - mSourceRect.y() - length + paintRectTmp.y(),
                                       centerPt.x() - mSourceRect.x() + paintRectTmp.x(),
                                       centerPt.y() - mSourceRect.y() + length + paintRectTmp.y());
                        }
                    }
                }


                p.setPen(QColor("#ffff00"));
                p.setFont(QFont("Microsoft YaHei UI", 20));
                p.drawText(QPoint(20, 40), mCurDcmFileInfo.filePath);

                p.drawText(QPoint(20, ui->widget_img_pre->rect().bottom()-30),
                           QString("[%1 x %2]   %3%")
                           .arg(mCurDcmFileInfo.width)
                           .arg(mCurDcmFileInfo.height)
                           .arg(QString::number(mScale*100,'f',1)));


                p.restore();

                p.setPen(QColor("#ff0000"));

                //几何图像
                QPen pen;
                for (int i=0; i<m_geometryItemList.size(); i++)
                {
                    GeometryItemBase *geometryItemTmp = m_geometryItemList.at(i);

                    if (NULL == geometryItemTmp)
                        continue;

                    if (RectItem *rectItemTmp = dynamic_cast<RectItem *> (geometryItemTmp))
                    {
//                        rectItemTmp->updateGeometry(QPoint(m_PaintRect.topLeft().x(), m_PaintRect.topLeft().y()),
//                                                            m_img.width(),
//                                                            m_img.height(), m_scaleRate,
//                                                            0, false, false);

                        QRectF rectTmp = rectItemTmp->getRect();
                        bool selectedFlag = false;

                        p.save();

                        if (rectItemTmp->getItemStatus() == SELECTEDMOVE
                                 || rectItemTmp->getItemStatus() == SELECTEDNOMOVE
                                 || rectItemTmp->getItemStatus() == SELECTEDDRAG)
                        {
            //                pen.setStyle(Qt::DashLine);
                            selectedFlag = true;
                        }

                        pen.setColor(rectItemTmp->getColor());
                        pen.setWidth(rectItemTmp->getLineWidht());
                        pen.setStyle(rectItemTmp->getPenStyle());

                        if (rectItemTmp->getFillStatus())
                            p.setBrush(QColor(rectItemTmp->getFillColor()));

                        p.setPen(pen);

                        p.drawRect(rectTmp);

                        p.restore();

                        p.save();
                        if (selectedFlag)
                        {
                            pen.setStyle(Qt::SolidLine);
                            pen.setColor(Qt::green);
                            pen.setWidth(2);

                            p.setPen(pen);

                            QList<QRect> rectList = rectItemTmp->getDragItemData().getDragItemRects();

                            for (int i=0; i<rectList.size(); i++)
                            {
                                p.drawEllipse(rectList.at(i));
                            }
                        }
                        else
                        {
//                            pen.setColor(m_measureSetting.generalSet.color);
                            QFont font;
//                            font.setPointSize(m_measureSetting.generalSet.fontSize);

                            p.setPen(pen);
                            p.setFont(font);

                            QString labelStr;
                            qreal value;

                            switch (rectItemTmp->getCurLabelType()) {
                            case NAMELABEL:

                                p.drawText(rectTmp.center(), rectItemTmp->getItemName());

                                break;
                            case AREALABEL:
                                value = rectItemTmp->getAreaUnit();

//                                if (m_measureSetting.generalSet.bDimension)
//                                    labelStr = QString("A:%1%2").arg(QString::number(value, 'f', m_measureSetting.generalSet.resultKeep))
//                                                               .arg(m_measureSetting.getUnitStr());
//                                else
//                                    labelStr = QString("A:%1").arg(QString::number(value, 'f', m_measureSetting.generalSet.resultKeep));
                                labelStr = QString("A:%1").arg(QString::number(value, 'f', 2));

                                p.drawText(rectTmp.center(), labelStr);

                                break;
                            case PERIMETERLABEL:
                                value = rectItemTmp->getCircumferenceUnit();

//                                if (m_measureSetting.generalSet.bDimension)
//                                    labelStr = QString("P:%1%2").arg(QString::number(value, 'f', m_measureSetting.generalSet.resultKeep))
//                                                               .arg(m_measureSetting.getUnitStr());
//                                else
//                                    labelStr = QString("P:%1").arg(QString::number(value, 'f', m_measureSetting.generalSet.resultKeep));

                                                                    labelStr = QString("P:%1").arg(QString::number(value, 'f', 2));

                                p.drawText(rectTmp.center(), labelStr);
                                break;
                            case WIDTHANDHEIGHTLABEL:
                            {
                                value = rectItemTmp->getOriWidthUnit();

//                                if (m_measureSetting.generalSet.bDimension)
//                                    labelStr = QString("W:%1%2").arg(QString::number(value, 'f', m_measureSetting.generalSet.resultKeep))
//                                                               .arg(m_measureSetting.getUnitStr());
//                                else
//                                    labelStr = QString("W:%1").arg(QString::number(value, 'f', m_measureSetting.generalSet.resultKeep));
                                labelStr = QString("W:%1").arg(QString::number(value, 'f', 2));

                                p.drawText(rectTmp.center(), labelStr);

                                //--
                                value = rectItemTmp->getOriHeightUnit();

//                                if (m_measureSetting.generalSet.bDimension)
//                                    labelStr = QString("H:%1%2").arg(QString::number(value, 'f', m_measureSetting.generalSet.resultKeep))
//                                                               .arg(m_measureSetting.getUnitStr());
//                                else
//                                    labelStr = QString("H:%1").arg(QString::number(value, 'f', m_measureSetting.generalSet.resultKeep));

                                labelStr = QString("H:%1").arg(QString::number(value, 'f', 2));

                                p.drawText(rectTmp.center().x(), rectTmp.center().y()+12+2, labelStr);
//                                p.drawText(rectTmp.center().x(), rectTmp.center().y()+m_measureSetting.generalSet.fontSize+2, labelStr);
            //                    p.drawText(rectTmp.right() - 30, rectTmp.center().y(), labelStr);
                            }
                                break;
                            default:
                                break;
                            }
                        }

                        p.restore();
                    }
                }

                switch (m_eCurAction) {
                case RECTACTION:
                {
                    if (m_eDrawStatus == BEGINDRAW)
                    {
                        p.drawRect(m_rectTmp);
                    }
                }
                    break;
                case ELLIPSEACTION:
                {
                    if (m_eDrawStatus == BEGINDRAW)
                    {
//                        painter->drawEllipse(m_rectTmp);
                    }
                }
                    break;
                case LINEACTION:
                {
                    if (m_eDrawStatus == BEGINDRAW)
                    {
//                        painter->drawLine(m_lineTmp);
                    }
                }
                    break;
                default:
                    break;
                }


                return true;
            }
        }
        else if (e->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);
            QPoint pt = mouseEvent->pos();

            if (!mBMeasureOpt)
            {
                m_bIsPress = true;
                m_PressPt = pt;

                if (mPaintRect.width() > ui->widget_img_pre->width() ||
                    mPaintRect.height() > ui->widget_img_pre->height())
                {
                    setCursor(Qt::ClosedHandCursor);
                }
            }
            else
            {
                //测量模式
//                hideColorWdg();

                if (mouseEvent->button() == Qt::LeftButton)
                {
                    switch (m_eCurAction) {
                    case RECTACTION:
                    case LINEACTION:
                    case ELLIPSEACTION:
                    case TEXTACTION:
                    {
//                        //清空测量窗口信息
//                        updateMesureDlgCtr(NULL);

                        //清空选中item
                        clearItemSelected(m_selectedIndex);

//                        //判断点击是否在图像区域内
//                        if (m_PaintRect.contains(wdgPtConvertToPaintPt(pt)))
//                        {
                            m_beginPosPt = pt;

                            m_eDrawStatus = BEGINDRAW;

//                            if (m_eCurAction == TEXTACTION)
//                            {
//                                m_bChangeText   = false;
//                                //--绘制文字--
//                                if (NULL == m_lineEditWdg)
//                                {
//                                    m_lineEditWdg = new QLineEdit(this);

//                                    connect(m_lineEditWdg, SIGNAL(editingFinished()), this, SLOT(slot_lineEditFinished()));
//                                }
//                                m_lineEditWdg->clear();
//                                m_lineEditWdg->move(pt);
//                                m_lineEditWdg->show();
//                                m_lineEditWdg->raise();
//                                m_lineEditWdg->setFocus();
//                            }
//                        }
                    }
                        break;
                    case ARROWACTION:
                    {
                        mousePressArrowAction(pt);
                    }
                        break;
                    case HANDACTION:
                    {
                        m_bIsPress = true;
                        m_PressPt = pt;

                        setCursor(Qt::ClosedHandCursor);
                    }
                        break;
                    default:
                        break;
                    }
                 }
            }

        }
        else if (e->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);
            QPoint pt = mouseEvent->pos();

            if (!mBMeasureOpt)
            {
                if (mPaintRect.width() > ui->widget_img_pre->width() ||
                    mPaintRect.height() > ui->widget_img_pre->height())
                {
                    setCursor(Qt::OpenHandCursor);
                    m_bIsPress = false;

                    update();
                }
            }
            else
            {
                if (mouseEvent->button() == Qt::LeftButton)
                {
                    switch (m_eCurAction) {
                    case RECTACTION:
                    {
                        if (m_rectTmp.isValid() && m_eDrawStatus == BEGINDRAW)
                        {
                            m_geometryItemBase = new RectItem(m_rectTmp);
//                            m_geometryItemBase->setColor(m_measureSetting.measureObjectSet.lineColor);
//                            m_geometryItemBase->setLineWidth(m_measureSetting.measureObjectSet.lineWidth);
//                            m_geometryItemBase->setPenStyle(m_measureSetting.measureObjectSet.penStyle);
//                            m_geometryItemBase->setCurLabelType(m_measureSetting.measureObjectSet.rectLabelType);

//                            m_geometryItemBase->setSpatialResolution(g_scanner.getSpatialResolution());
//                            m_geometryItemBase->setUnit(m_measureSetting.lengthUnit);

//                            m_geometryItemBase->calcOriGeometry(QPoint(m_PaintRect.topLeft().x(),
//                                                                       m_PaintRect.topLeft().y()),
//                                                                m_img.width(),
//                                                                m_img.height(), m_scaleRate,
//                                                                0, false, false);

//                            RectItem *rectItem = (RectItem *)m_geometryItemBase;
//                            rectItem->setFillStatus(m_measureSetting.measureObjectSet.bFill);
//                            rectItem->setFillColor(m_measureSetting.measureObjectSet.fillColor);

                            m_geometryItemList.push_back(m_geometryItemBase);

                            //置为选中态
                            m_selectedIndex = m_geometryItemList.count() - 1;
                            m_geometryItemBase->setItemStatus(SELECTEDNOMOVE);

                            //添加新增操作
                            m_itemOperatorTmp = new ItemOperator;
                            m_itemOperatorTmp->geometryItem = m_geometryItemBase;
                            m_itemOperatorTmp->type = ADDOPT;
                            insetOperator(m_itemOperatorTmp);

//                            updateMesureDlgCtr(m_geometryItemBase);

//                            updateMeasureTable();
                        }

                        m_rectTmp = QRectF();

                        m_eDrawStatus = ENDDRAW;
                    }
                        break;
                    case ELLIPSEACTION:
                    {
//                        if (m_rectTmp.isValid() && m_eDrawStatus == BEGINDRAW)
//                        {
//                            m_geometryItemBase = new EllipseItem(m_rectTmp);
//                            m_geometryItemBase->setColor(m_measureSetting.measureObjectSet.lineColor);
//                            m_geometryItemBase->setLineWidth(m_measureSetting.measureObjectSet.lineWidth);
//                            m_geometryItemBase->setPenStyle(m_measureSetting.measureObjectSet.penStyle);
//                            m_geometryItemBase->setCurLabelType(m_measureSetting.measureObjectSet.ellipseLabelType);

//                            m_geometryItemBase->setSpatialResolution(g_scanner.getSpatialResolution());
//                            m_geometryItemBase->setUnit(m_measureSetting.lengthUnit);

//                            m_geometryItemBase->calcOriGeometry(QPoint(m_PaintRect.topLeft().x(), m_PaintRect.topLeft().y()),
//                                                                m_img.width(),
//                                                                m_img.height(), m_scaleRate,
//                                                                0, false, false);

//                            EllipseItem *ellipseItem = (EllipseItem *)m_geometryItemBase;
//                            ellipseItem->setFillStatus(m_measureSetting.measureObjectSet.bFill);
//                            ellipseItem->setFillColor(m_measureSetting.measureObjectSet.fillColor);

//                            m_geometryItemList.push_back(m_geometryItemBase);

//                            //置为选中态
//                            m_selectedIndex = m_geometryItemList.count() - 1;
//                            m_geometryItemBase->setItemStatus(SELECTEDNOMOVE);

//                            //添加新增操作
//                            m_itemOperatorTmp = new ItemOperator;
//                            m_itemOperatorTmp->geometryItem = m_geometryItemBase;
//                            m_itemOperatorTmp->type = ADDOPT;
//                            insetOperator(m_itemOperatorTmp);

//                            updateMesureDlgCtr(m_geometryItemBase);

//                            updateMeasureTable();
//                        }

//                        m_rectTmp = QRectF();

//                        m_eDrawStatus = ENDDRAW;
                    }
                        break;
                    case ARROWACTION:
                    {
                        mouseReleaseArrowAction();
                    }
                        break;
                    case LINEACTION:
                    {
//                        if (!m_lineTmp.isNull() && m_eDrawStatus == BEGINDRAW)
//                        {
//                            m_geometryItemBase = new LineItem(m_lineTmp);
//                            m_geometryItemBase->setColor(m_measureSetting.measureObjectSet.lineColor);
//                            m_geometryItemBase->setLineWidth(m_measureSetting.measureObjectSet.lineWidth);
//                            m_geometryItemBase->setPenStyle(m_measureSetting.measureObjectSet.penStyle);
//                            m_geometryItemBase->setCurLabelType(m_measureSetting.measureObjectSet.lineLabelType);

//                            m_geometryItemBase->setSpatialResolution(g_scanner.getSpatialResolution());
//                            m_geometryItemBase->setUnit(m_measureSetting.lengthUnit);

//                            m_geometryItemBase->calcOriGeometry(QPoint(m_PaintRect.topLeft().x(), m_PaintRect.topLeft().y()),
//                                                                m_img.width(),
//                                                                m_img.height(), m_scaleRate,
//                                                                0, false, false);

//                            m_geometryItemList.push_back(m_geometryItemBase);

//                            //置为选中态
//                            m_selectedIndex = m_geometryItemList.count() - 1;
//                            m_geometryItemBase->setItemStatus(SELECTEDNOMOVE);

//                            //添加新增操作
//                            m_itemOperatorTmp = new ItemOperator;
//                            m_itemOperatorTmp->geometryItem = m_geometryItemBase;
//                            m_itemOperatorTmp->type = ADDOPT;
//                            insetOperator(m_itemOperatorTmp);

//                            updateMesureDlgCtr(m_geometryItemBase);

//                            updateMeasureTable();
//                        }

//                        m_lineTmp = QLine();

//                        m_eDrawStatus = ENDDRAW;
                    }
                        break;
                    case HANDACTION:
                    {
                        setCursor(Qt::OpenHandCursor);
                        m_bIsPress = false;

                        update();
                    }
                        break;
                    default:
                        break;
                    }
                }
            }

       }
       else if (e->type() == QEvent::MouseMove)
       {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);
            QPoint pt = mouseEvent->pos();

            if (!mBMeasureOpt)
            {
                if (mPaintRect.width() > ui->widget_img_pre->width() ||
                    mPaintRect.height() > ui->widget_img_pre->height())
                {
                    if (m_bIsPress)
                    {
                        //偏移距离转化为 （图像坐标中）
                        QPoint offsetPt = m_PressPt - pt;

                        m_PressPt = pt;

                        setScrollBarOffsetValues(offsetPt.x(), offsetPt.y());

                        update();
                    }
                }
            }
            else
            {
                QPoint imgPt = pt;

                switch (m_eCurAction) {
                case RECTACTION:
                {
                    if (m_eDrawStatus == BEGINDRAW)
                    {
                        int x, y , w, h;
                        x = m_beginPosPt.x();
                        y = m_beginPosPt.y();
                        w = imgPt.x() - x;
                        h = imgPt.y() - y;

                        m_rectTmp = QRect(x, y, w, h);

                        update();
                    }
                }
                    break;
                case ELLIPSEACTION:
                {
//                    if (m_eDrawStatus == BEGINDRAW)
//                    {
//                        int x, y , w, h;
//                        x = m_beginPosPt.x();
//                        y = m_beginPosPt.y();
//                        w = imgPt.x() - x;
//                        h = imgPt.y() - y;

//                        m_rectTmp = QRect(x, y, w, h);

//                        update(ui->widget_main->rect());
//                    }
                }
                    break;
                case LINEACTION:
                {
//                    if (m_eDrawStatus == BEGINDRAW)
//                    {
//                        QPoint p1 = m_beginPosPt;
//                        QPoint p2 = imgPt;

//                        m_lineTmp = QLine(p1, p2);

//                        update(ui->widget_main->rect());
//                    }
                }
                    break;
                case ARROWACTION:
                {
                    mouseMoveArrowAction(pt);
                }
                    break;
                case HANDACTION:
                {
                    if (m_bIsPress)
                    {
                        //偏移距离转化为 （图像坐标中）
                        QPoint offsetPt = m_PressPt - pt;

                        m_PressPt = pt;


                        setScrollBarOffsetValues(offsetPt.x(), offsetPt.y());

                        update();
                    }
                }
                    break;
                default:
                    break;
                }

            }
       }
        else if (e->type() == QEvent::Enter)
        {
//            if (mBMeasureOpt)
//                setCursor(Qt::OpenHandCursor);
//            else {
//                setCursor(Qt::ArrowCursor);
//            }

            if (mPaintRect.width() > ui->widget_img_pre->width() ||
                mPaintRect.height() > ui->widget_img_pre->height())
            {
                setCursor(Qt::OpenHandCursor);
            }
            else {
                setCursor(Qt::ArrowCursor);
            }

            return true;
        }
        else if (e->type() == QEvent::Leave)
        {
            setCursor(Qt::ArrowCursor);
            return true;
        }

    }
    else if(obj == ui->widget_tool)
    {
        if (e->type() == QEvent::Enter)
        {
            setCursor(Qt::ArrowCursor);

//            showColorWdgOpacity(1);
            return true;
        }
        else if(e->type() == QEvent::Leave)
        {
            updateCursor();

//            showColorWdgOpacity(0.5);
            return true;
        }
    }
    else if( obj == ui->horizontalScrollBar ||
             obj == ui->verticalScrollBar ||
             obj == ui->pushButton_cross ||
             obj == ui->verticalSlider_diameter ||
             obj == ui->pushButton_pre_big ||
             obj == ui->pushButton_next_big)
    {
        if (e->type() == QEvent::Enter)
        {
            setCursor(Qt::ArrowCursor);
            return true;
        }
        else if(e->type() == QEvent::Leave)
        {
            updateCursor();
            return true;
        }
    }
//    else if (m_colorWdg != NULL && obj == m_colorWdg)
//    {
//        if (e->type() == QEvent::Enter)
//        {
//            setCursor(Qt::ArrowCursor);

//            showColorWdgOpacity(1);
//            return true;
//        }
//        else if (e->type() == QEvent::Leave)
//        {
//            updateCursor();

//            showColorWdgOpacity(0.5);
//            return true;
//        }
//    }


    return QMainWindow::eventFilter(obj, e);
}

void MainWindow::insetOperator(ItemOperator *itemOperator)
{
    if (m_curOperatorIndex < m_ItemOperatorList.size() - 1)
    {
        //更新链表
        int popCount = m_ItemOperatorList.size() - m_curOperatorIndex - 1;

        for (int i=0; i<popCount; i++)
        {
            m_ItemOperatorList.pop_back();
        }
    }

    if (m_ItemOperatorList.size() >= 15)
    {
        //多出15个操作以后，扔掉之前的操作
        m_ItemOperatorList.pop_front();
    }

    m_ItemOperatorList.push_back(itemOperator);
    m_curOperatorIndex = m_ItemOperatorList.size() - 1;

    updateOperatorStatus();
}

void MainWindow::mousePressArrowAction(QPoint pt)
{
//    QPoint imgPt = convertToImgPt(pt);
    QPoint imgPt = pt;

//    slot_lineEditFinished();

    if (m_selectedIndex >= 0 && m_selectedIndex < m_geometryItemList.count())
    {
        GeometryItemBase *geometryItemTmp = m_geometryItemList.at(m_selectedIndex);

        if (RectItem *rectItemTmp = dynamic_cast<RectItem *> (geometryItemTmp))
        {
            QList<DragItem> dragItemList = rectItemTmp->getDragItemData().getDragItems();

            bool containtFlag;
            DragDirection curDragDirection;

            containtFlag = getDragDirection(dragItemList, imgPt, curDragDirection);

            if (containtFlag)
            {
                m_eSldDragDirection = curDragDirection;

                m_curPosPt = imgPt;
                m_geometryItemList.at(m_selectedIndex)->setItemStatus(SELECTEDDRAG);

                update();

                return;
            }
        }
        else if (TextItem *textItemTmp = dynamic_cast<TextItem *> (geometryItemTmp))
        {
            QList<DragItem> dragItemList = textItemTmp->getDragItemData().getDragItems();

            bool containtFlag;
            DragDirection curDragDirection;

            containtFlag = getDragDirection(dragItemList, imgPt, curDragDirection);

            if (containtFlag)
            {
                m_eSldDragDirection = curDragDirection;

                m_curPosPt = imgPt;
                m_geometryItemList.at(m_selectedIndex)->setItemStatus(SELECTEDDRAG);

                update();

                return;
            }
        }
        else if (EllipseItem *ellipseItemTmp = dynamic_cast<EllipseItem *> (geometryItemTmp))
        {
            QList<DragItem> dragItemList = ellipseItemTmp->getDragItemData().getDragItems();

            bool containtFlag;
            DragDirection curDragDirection;

            containtFlag = getDragDirection(dragItemList, imgPt, curDragDirection);

            if (containtFlag)
            {
                m_eSldDragDirection = curDragDirection;

                m_curPosPt = imgPt;
                m_geometryItemList.at(m_selectedIndex)->setItemStatus(SELECTEDDRAG);

                update();

                return;
            }
        }
        else if (LineItem *lineItemTmp = dynamic_cast<LineItem *> (geometryItemTmp))
        {
            QList<DragItem> dragItemList = lineItemTmp->getDragItemData().getDragItems();

            bool containtFlag;
            DragDirection curDragDirection;

            containtFlag = getDragDirection(dragItemList, imgPt, curDragDirection);

            if (containtFlag)
            {
                m_eSldDragDirection = curDragDirection;

                m_curPosPt = imgPt;
                m_geometryItemList.at(m_selectedIndex)->setItemStatus(SELECTEDDRAG);

                update();

                return;
            }
        }
    }

    m_selectedIndex = -1;
    m_eSldDragDirection = NONEDIR;

    for (int i=0; i<m_geometryItemList.size(); i++)
    {
        GeometryItemBase *geometryItemTmp = m_geometryItemList.at(i);

        if (RectItem *rectItemTmp = dynamic_cast<RectItem *> (geometryItemTmp))
        {
            QRectF rectTmp = rectItemTmp->getRect();

            //置为未选中态
            rectItemTmp->setItemStatus(NORMOAL);

            if (rectTmp.contains(imgPt))
            {
                m_selectedIndex = i;
                m_curPosPt = imgPt;
            }
        }
        else if (TextItem *textItemTmp = dynamic_cast<TextItem *> (geometryItemTmp))
        {
            QRectF rectTmp = textItemTmp->getRect();

            //置为未选中态
            textItemTmp->setItemStatus(NORMOAL);

            if (rectTmp.contains(imgPt))
            {
                m_selectedIndex = i;
                m_curPosPt = imgPt;
            }
        }
        else if (EllipseItem *ellipseItemTmp = dynamic_cast<EllipseItem *> (geometryItemTmp))
        {
            QRectF rectTmp = ellipseItemTmp->getRect();

            //置为未选中态
            ellipseItemTmp->setItemStatus(NORMOAL);

            if (rectTmp.contains(imgPt))
            {
                m_selectedIndex = i;
                m_curPosPt = imgPt;
            }
        }
        else if (LineItem *lineItemTmp = dynamic_cast<LineItem *> (geometryItemTmp))
        {

            QPolygonF polygonf = lineItemTmp->getBoundingPolygonf();

            //置为未选中态
            lineItemTmp->setItemStatus(NORMOAL);

//            if (point2Polygon(polygonf, polygonf.size(), imgPt) >= 0)
//            {
//                m_selectedIndex = i;
//                m_curPosPt = imgPt;
//            }
        }
     }

    if (m_selectedIndex >= 0 && m_selectedIndex < m_geometryItemList.count())
    {
        //置为选中态
        m_geometryItemList.at(m_selectedIndex)->setItemStatus(SELECTEDMOVE);

//        setToolStyles();

        setCursor(Qt::SizeAllCursor);

//        updateMesureDlgCtr(m_geometryItemList.at(m_selectedIndex));

        update();
    }
    else
    {
//        updateMesureDlgCtr();
    }
}

void MainWindow::clearItemSelected(int index)
{
    if (index >= 0 && index < m_geometryItemList.count())
    {
        m_geometryItemList.at(index)->setItemStatus(NORMOAL);

        m_selectedIndex = -1;
        m_eSldDragDirection = NONEDIR;
    }
}


void MainWindow::mouseReleaseArrowAction()
{
    if (m_selectedIndex >= 0 && m_selectedIndex < m_geometryItemList.count())
    {
        GeometryItemBase *geometryItemTmp = m_geometryItemList.at(m_selectedIndex);

        if (RectItem *rectItemTmp = dynamic_cast<RectItem *> (geometryItemTmp))
        {
            rectItemTmp->setItemStatus(SELECTEDNOMOVE);

            m_eSldDragDirection = NONEDIR;

            if (m_beginMove)
            {
                m_beginMove = false;

                //插入一个移动操作
                m_itemOperatorTmp = new ItemOperator;
                m_itemOperatorTmp->geometryItem = rectItemTmp;
                m_itemOperatorTmp->type = MOVEOPT;
                m_itemOperatorTmp->originalPt = m_originPt;
                m_itemOperatorTmp->movePt = rectItemTmp->getOriRect().topLeft();

                insetOperator(m_itemOperatorTmp);

//                updateMeasureTable();
            }

            if (m_beginChange)
            {
                m_beginChange = false;

                //插入一个修改操作
                m_itemOperatorTmp = new ItemOperator;
                m_itemOperatorTmp->geometryItem = rectItemTmp;
                m_itemOperatorTmp->type = CHANGEOPT;
                m_itemOperatorTmp->originalRect = m_originRect;
                m_itemOperatorTmp->changedRect = rectItemTmp->getOriRect();

                insetOperator(m_itemOperatorTmp);

//                updateMeasureTable();
            }
        }
        else if (TextItem *textItemTmp = dynamic_cast<TextItem *> (geometryItemTmp))
        {
            textItemTmp->setItemStatus(SELECTEDNOMOVE);

            m_eSldDragDirection = NONEDIR;

            if (m_beginMove)
            {
                m_beginMove = false;

                //插入一个移动操作
                m_itemOperatorTmp = new ItemOperator;
                m_itemOperatorTmp->geometryItem = textItemTmp;
                m_itemOperatorTmp->type = MOVEOPT;
                m_itemOperatorTmp->originalPt = m_originPt;
                m_itemOperatorTmp->movePt = textItemTmp->getOriRect().topLeft();

                insetOperator(m_itemOperatorTmp);
            }

            if (m_beginChange)
            {
                m_beginChange = false;

                //插入一个修改操作
                m_itemOperatorTmp = new ItemOperator;
                m_itemOperatorTmp->geometryItem = textItemTmp;
                m_itemOperatorTmp->type = CHANGEOPT;
                m_itemOperatorTmp->originalRect = m_originRect;
                m_itemOperatorTmp->changedRect = textItemTmp->getOriRect();

                insetOperator(m_itemOperatorTmp);
            }
        }
        else if (EllipseItem *ellipseItemTmp = dynamic_cast<EllipseItem *> (geometryItemTmp))
        {
            ellipseItemTmp->setItemStatus(SELECTEDNOMOVE);

            m_eSldDragDirection = NONEDIR;


            if (m_beginMove)
            {
                m_beginMove = false;

                //插入一个移动操作
                m_itemOperatorTmp = new ItemOperator;
                m_itemOperatorTmp->geometryItem = ellipseItemTmp;
                m_itemOperatorTmp->type = MOVEOPT;
                m_itemOperatorTmp->originalPt = m_originPt;
                m_itemOperatorTmp->movePt = ellipseItemTmp->getOriRect().topLeft();

                insetOperator(m_itemOperatorTmp);

//                updateMeasureTable();
            }

            if (m_beginChange)
            {
                m_beginChange = false;

                //插入一个修改操作
                m_itemOperatorTmp = new ItemOperator;
                m_itemOperatorTmp->geometryItem = ellipseItemTmp;
                m_itemOperatorTmp->type = CHANGEOPT;
                m_itemOperatorTmp->originalRect = m_originRect;
                m_itemOperatorTmp->changedRect = ellipseItemTmp->getOriRect();

                insetOperator(m_itemOperatorTmp);

//                updateMeasureTable();
            }
        }
        else if (LineItem *lineItemTmp = dynamic_cast<LineItem *> (geometryItemTmp))
        {
            lineItemTmp->setItemStatus(SELECTEDNOMOVE);

            m_eSldDragDirection = NONEDIR;

            if (m_beginMove)
            {
                m_beginMove = false;

                //插入一个移动操作
                m_itemOperatorTmp = new ItemOperator;
                m_itemOperatorTmp->geometryItem = lineItemTmp;
                m_itemOperatorTmp->type = MOVEOPT;
                m_itemOperatorTmp->originalPt = m_originPt;
                m_itemOperatorTmp->movePt = lineItemTmp->getOriLine().p1();

                insetOperator(m_itemOperatorTmp);

//                updateMeasureTable();
            }

            if (m_beginChange)
            {
                m_beginChange = false;

                //插入一个修改操作
                m_itemOperatorTmp = new ItemOperator;
                m_itemOperatorTmp->geometryItem = lineItemTmp;
                m_itemOperatorTmp->type = CHANGEOPT;
                m_itemOperatorTmp->originLine = m_originLine;
                m_itemOperatorTmp->changeLine = lineItemTmp->getOriLine();

                insetOperator(m_itemOperatorTmp);

//                updateMeasureTable();
            }

        }
    }
//    else
//    {
//        m_bMousePress = false;
//    }

    setCursor(Qt::ArrowCursor);
    update();
}


bool MainWindow::getDragDirection(QList<DragItem> &dragItemList,
                      QPoint pt,
                      DragDirection &dragDirection)
{
    bool containtFlag = false;

    for (int i=0; i<dragItemList.size(); i++)
    {
        DragItem dragItemTmp = dragItemList.at(i);
        if (dragItemTmp.getDragRect().contains(pt))
        {
            containtFlag = true;

            if (dragItemTmp.getDragDirection() == LEFTTOP)
            {
                setCursor(Qt::SizeFDiagCursor);
                dragDirection = LEFTTOP;

                break;
            }
            else if (dragItemTmp.getDragDirection() == LEFTBOTTOM)
            {
                setCursor(Qt::SizeBDiagCursor);
                dragDirection = LEFTBOTTOM;

                break;
            }
            else if (dragItemTmp.getDragDirection() == RIGHTTOP)
            {
                setCursor(Qt::SizeBDiagCursor);
                dragDirection = RIGHTTOP;

                break;
            }
            else if (dragItemTmp.getDragDirection() == RIGHTBOTTOM)
            {
                setCursor(Qt::SizeFDiagCursor);
                dragDirection = RIGHTBOTTOM;

                break;
            }
            else if (dragItemTmp.getDragDirection() == DragDirection::LEFT)
            {
                setCursor(Qt::SizeHorCursor);
                dragDirection = DragDirection::LEFT;

                break;
            }
            else if (dragItemTmp.getDragDirection() == DragDirection::RIGHT)
            {
                setCursor(Qt::SizeHorCursor);
                dragDirection = DragDirection::RIGHT;

                break;
            }
         }
    }

    return containtFlag;
}

void MainWindow::mouseMoveArrowAction(QPoint pt)
{
//    QPoint imgPt = convertToImgPt(pt);
    QPoint imgPt = pt;

    if (m_selectedIndex >= 0 && m_selectedIndex < m_geometryItemList.count())
    {
        GeometryItemBase *geometryItemTmp = m_geometryItemList.at(m_selectedIndex);

        if (RectItem *rectItemTmp = dynamic_cast<RectItem *> (geometryItemTmp))
        {
            QList<DragItem> dragItemList = rectItemTmp->getDragItemData().getDragItems();

            bool containtFlag = false;
            DragDirection curDragDirection;

            if (m_eSldDragDirection == NONEDIR)
            {
                containtFlag = getDragDirection(dragItemList, imgPt, curDragDirection);
            }
            else
            {
                containtFlag = true;
                curDragDirection = m_eSldDragDirection;
            }

            if (containtFlag)
            {
                if (rectItemTmp->getItemStatus() == SELECTEDDRAG)
                {
                    //拖拽点移动
                    if (!m_beginChange)
                    {
                        m_beginChange = true;
                        m_originRect  = rectItemTmp->getOriRect();
                    }

                    QPoint offsetPt = m_curPosPt - imgPt;
                    QRectF oldRect = rectItemTmp->getRect();
                    QRectF rectTmp;

                    switch(curDragDirection)
                    {
                    case LEFTTOP:
                        oldRect.setX(oldRect.x() - offsetPt.x());
                        oldRect.setY(oldRect.y() - offsetPt.y());
                        rectTmp = oldRect;
                        break;
                    case LEFTBOTTOM:
                        rectTmp = QRectF(oldRect.x() - offsetPt.x(),
                                        oldRect.y(),
                                        rectItemTmp->getWidth() + offsetPt.x(),
                                        rectItemTmp->getHeight() - offsetPt.y());

                        break;
                    case RIGHTTOP:
                        rectTmp = QRectF(oldRect.x(),
                                        oldRect.y() - offsetPt.y(),
                                        rectItemTmp->getWidth() - offsetPt.x(),
                                        rectItemTmp->getHeight() + offsetPt.y());
                        break;
                    case RIGHTBOTTOM:
                        rectTmp = QRectF(oldRect.x(),
                                        oldRect.y(),
                                        rectItemTmp->getWidth() - offsetPt.x(),
                                        rectItemTmp->getHeight() - offsetPt.y());
                        break;
                    default :
                        break;
                    }

                    rectItemTmp->setRect(rectTmp);
//                    rectItemTmp->calcOriGeometry(QPoint(m_PaintRect.topLeft().x(), m_PaintRect.topLeft().y()),
//                                                 m_img.width(),
//                                                 m_img.height(), m_scaleRate,
//                                                 0, false, false);

                    m_curPosPt = imgPt;

//                    updateMesureDlgCtr(geometryItemTmp);

//                    updateMeasureTable();

                    update();

                    return ;
                }
            }
            else
            {

//                QPoint imgPt = convertToImgPt(pt);
                //设置鼠标状态
                if (rectItemTmp->getBoundingRectF().contains(imgPt)){
                    setCursor(Qt::SizeAllCursor);
                }
                else{
                    setCursor(Qt::ArrowCursor);
                }

                if (rectItemTmp->getItemStatus() == SELECTEDMOVE)
                {
                    //ITEM移动
                    if (!m_beginMove)
                    {
                        m_beginMove = true;
                        m_originPt = rectItemTmp->getOriRect().topLeft();
                    }

                    QPoint offsetPt = m_curPosPt - imgPt;
                    QRectF oldRect = rectItemTmp->getRect();

                    QRectF rectTmp = QRectF(oldRect.x() - offsetPt.x(),
                                          oldRect.y() - offsetPt.y(),
                                          rectItemTmp->getWidth(),
                                          rectItemTmp->getHeight());

                    rectItemTmp->setRect(rectTmp);
//                    rectItemTmp->calcOriGeometry(QPoint(m_PaintRect.topLeft().x(), m_PaintRect.topLeft().y()),
//                                                 m_img.width(),
//                                                 m_img.height(), m_scaleRate,
//                                                 0, false, false);

                    m_curPosPt = imgPt;

//                    updateMesureDlgCtr(geometryItemTmp);

//                    updateMeasureTable();

                    update();

                    return ;
                }
            }

        }
        else if (TextItem *textItemTmp = dynamic_cast<TextItem *> (geometryItemTmp))
        {
//            QList<DragItem> dragItemList = textItemTmp->getDragItemData().getDragItems();

//            bool containtFlag = false;
//            DragDirection curDragDirection;

//            if (m_eSldDragDirection == NONEDIR)
//            {
//                containtFlag = getDragDirection(dragItemList, imgPt, curDragDirection);
//            }
//            else
//            {
//                containtFlag = true;
//                curDragDirection = m_eSldDragDirection;
//            }

//            if (containtFlag)
//            {
//                if (textItemTmp->getItemStatus() == SELECTEDDRAG)
//                {
//                    //拖拽点移动
//                    if (!m_beginChange)
//                    {
//                        m_beginChange = true;
//                        m_originRect  = textItemTmp->getOriRect();
//                    }

//                    QPoint offsetPt = m_curPosPt - imgPt;
//                    QRectF oldRect = textItemTmp->getRect();
//                    QRectF rectTmp;

//                    switch(curDragDirection)
//                    {
//                    case LEFTTOP:
//                        oldRect.setX(oldRect.x() - offsetPt.x());
//                        oldRect.setY(oldRect.y() - offsetPt.y());
//                        rectTmp = oldRect;
//                        break;
//                    case LEFTBOTTOM:
//                        rectTmp = QRectF(oldRect.x() - offsetPt.x(),
//                                        oldRect.y(),
//                                        textItemTmp->getWidth() + offsetPt.x(),
//                                        textItemTmp->getHeight() - offsetPt.y());

//                        break;
//                    case RIGHTTOP:
//                        rectTmp = QRectF(oldRect.x(),
//                                        oldRect.y() - offsetPt.y(),
//                                        textItemTmp->getWidth() - offsetPt.x(),
//                                        textItemTmp->getHeight() + offsetPt.y());
//                        break;
//                    case RIGHTBOTTOM:
//                        rectTmp = QRectF(oldRect.x(),
//                                        oldRect.y(),
//                                        textItemTmp->getWidth() - offsetPt.x(),
//                                        textItemTmp->getHeight() - offsetPt.y());
//                        break;
//                    default :
//                        break;
//                    }

////                    textItemTmp->updateRect(rectTmp);
//                    textItemTmp->setText(rectTmp, textItemTmp->getText());
//                    textItemTmp->calcOriGeometry(QPoint(m_PaintRect.topLeft().x(), m_PaintRect.topLeft().y()),
//                                                 m_img.width(),
//                                                 m_img.height(), m_scaleRate,
//                                                 0, false, false);

//                    textItemTmp->reCalcFont();

//                    m_curPosPt = imgPt;

//                    updateMeasureTable();

//                    update(ui->widget_main->rect());

//                    return ;
//                }
//            }
//            else
//            {

////                QPoint imgPt = convertToImgPt(pt);
//                //设置鼠标状态
//                if (textItemTmp->getBoundingRectF().contains(imgPt)){
//                    setCursor(Qt::SizeAllCursor);
//                }
//                else{
//                    setCursor(Qt::ArrowCursor);
//                }

//                if (textItemTmp->getItemStatus() == SELECTEDMOVE)
//                {
//                    //ITEM移动
//                    if (!m_beginMove)
//                    {
//                        m_beginMove = true;
//                        m_originPt = textItemTmp->getOriRect().topLeft();
//                    }

//                    QPoint offsetPt = m_curPosPt - imgPt;
//                    QRectF oldRect = textItemTmp->getRect();

//                    QRectF rectTmp = QRectF(oldRect.x() - offsetPt.x(),
//                                          oldRect.y() - offsetPt.y(),
//                                          textItemTmp->getWidth(),
//                                          textItemTmp->getHeight());

//                    textItemTmp->setText(rectTmp, textItemTmp->getText());
////                    textItemTmp->updateRect(rectTmp);
//                    textItemTmp->calcOriGeometry(QPoint(m_PaintRect.topLeft().x(), m_PaintRect.topLeft().y()),
//                                                 m_img.width(),
//                                                 m_img.height(), m_scaleRate,
//                                                 0, false, false);
//                    textItemTmp->reCalcFont();

//                    m_curPosPt = imgPt;

//                    updateMeasureTable();

//                    update(ui->widget_main->rect());

//                    return ;
//                }
//            }

        }
        else if (EllipseItem *ellipseItemTmp = dynamic_cast<EllipseItem *> (geometryItemTmp))
        {
//            QList<DragItem> dragItemList = ellipseItemTmp->getDragItemData().getDragItems();

//            bool containtFlag = false;
//            DragDirection curDragDirection;

//            if (m_eSldDragDirection == NONEDIR)
//            {
//                containtFlag = getDragDirection(dragItemList, imgPt, curDragDirection);
//            }
//            else
//            {
//                containtFlag = true;
//                curDragDirection = m_eSldDragDirection;
//            }

//            if (containtFlag)
//            {
//                if (ellipseItemTmp->getItemStatus() == SELECTEDDRAG)
//                {
//                    //拖拽点移动
//                    if (!m_beginChange)
//                    {
//                        m_beginChange = true;
//                        m_originRect  = ellipseItemTmp->getOriRect();
//                    }

//                    QPoint offsetPt = m_curPosPt - imgPt;
//                    QRectF oldRect = ellipseItemTmp->getRect();
//                    QRectF rectTmp;

//                    switch(curDragDirection)
//                    {
//                    case LEFTTOP:
//                        oldRect.setX(oldRect.x() - offsetPt.x());
//                        oldRect.setY(oldRect.y() - offsetPt.y());
//                        rectTmp = oldRect;
//                        break;
//                    case LEFTBOTTOM:
//                        rectTmp = QRectF(oldRect.x() - offsetPt.x(),
//                                        oldRect.y(),
//                                        ellipseItemTmp->getWidth() + offsetPt.x(),
//                                        ellipseItemTmp->getHeight() - offsetPt.y());

//                        break;
//                    case RIGHTTOP:
//                        rectTmp = QRectF(oldRect.x(),
//                                        oldRect.y() - offsetPt.y(),
//                                        ellipseItemTmp->getWidth() - offsetPt.x(),
//                                        ellipseItemTmp->getHeight() + offsetPt.y());
//                        break;
//                    case RIGHTBOTTOM:
//                        rectTmp = QRectF(oldRect.x(),
//                                        oldRect.y(),
//                                        ellipseItemTmp->getWidth() - offsetPt.x(),
//                                        ellipseItemTmp->getHeight() - offsetPt.y());
//                        break;
//                    default :
//                        break;
//                    }

//                    ellipseItemTmp->setRect(rectTmp);
//                    ellipseItemTmp->calcOriGeometry(QPoint(m_PaintRect.topLeft().x(), m_PaintRect.topLeft().y()),
//                                                    m_img.width(),
//                                                    m_img.height(), m_scaleRate,
//                                                    0, false, false);

//                    m_curPosPt = imgPt;

//                    updateMesureDlgCtr(geometryItemTmp);

//                    updateMeasureTable();

//                    update(ui->widget_main->rect());

//                    return ;
//                }
//            }
//            else
//            {
//                //设置鼠标状态
//                if (ellipseItemTmp->getBoundingRectF().contains(imgPt)){
//                    setCursor(Qt::SizeAllCursor);
//                }
//                else{
//                    setCursor(Qt::ArrowCursor);
//                }

//                if (ellipseItemTmp->getItemStatus() == SELECTEDMOVE)
//                {
//                    //ITEM移动
//                    if (!m_beginMove)
//                    {
//                        m_beginMove = true;
//                        m_originPt = ellipseItemTmp->getOriRect().topLeft();
//                    }

//                    QPoint offsetPt = m_curPosPt - imgPt;
//                    QRectF oldRect = ellipseItemTmp->getRect();

//                    QRectF rectTmp = QRectF(oldRect.x() - offsetPt.x(),
//                                          oldRect.y() - offsetPt.y(),
//                                          ellipseItemTmp->getWidth(),
//                                          ellipseItemTmp->getHeight());


//                    ellipseItemTmp->setRect(rectTmp);
//                    ellipseItemTmp->calcOriGeometry(QPoint(m_PaintRect.topLeft().x(), m_PaintRect.topLeft().y()),
//                                                     m_img.width(),
//                                                     m_img.height(), m_scaleRate,
//                                                     0, false, false);

//                    m_curPosPt = imgPt;

//                    updateMesureDlgCtr(geometryItemTmp);

//                    updateMeasureTable();

//                    update(ui->widget_main->rect());

//                    return ;
//                }
//            }
        }
        else if (LineItem *lineItemTmp = dynamic_cast<LineItem *> (geometryItemTmp))
        {
//            QList<DragItem> dragItemList = lineItemTmp->getDragItemData().getDragItems();

//            bool containtFlag = false;
//            DragDirection curDragDirection;

//            if (m_eSldDragDirection == NONEDIR)
//            {
//                containtFlag = getDragDirection(dragItemList, imgPt, curDragDirection);
//            }
//            else
//            {
//                containtFlag = true;
//                curDragDirection = m_eSldDragDirection;
//            }

//            if (containtFlag)
//            {
//                if (lineItemTmp->getItemStatus() == SELECTEDDRAG)
//                {
//                    //拖拽点移动
//                    if (!m_beginChange)
//                    {
//                        m_beginChange = true;
//                        m_originLine  = lineItemTmp->getOriLine();
//                    }

//                    QLineF lineTmp = lineItemTmp->getLine();

//                    switch(curDragDirection)
//                    {
//                    case DragDirection::LEFT:
//                        lineTmp.setP1(imgPt);
//                        break;
//                    case DragDirection::RIGHT:
//                        lineTmp.setP2(imgPt);
//                        break;
//                    default :
//                        break;
//                    }

//                    lineItemTmp->setLine(lineTmp);
//                    lineItemTmp->calcOriGeometry(QPoint(m_PaintRect.topLeft().x(), m_PaintRect.topLeft().y()),
//                                                 m_img.width(),
//                                                 m_img.height(), m_scaleRate,
//                                                 0, false, false);

//                    m_curPosPt = imgPt;

//                    updateMesureDlgCtr(geometryItemTmp);

//                    updateMeasureTable();

//                    update(ui->widget_main->rect());

//                    return ;
//                }
//            }
//            else
//            {
//                QPolygonF polygonf = lineItemTmp->getBoundingPolygonf();

//                //设置鼠标状态
//                if (point2Polygon(polygonf, polygonf.size(), imgPt) >= 0){
//                    setCursor(Qt::SizeAllCursor);
//                }
//                else{
//                    setCursor(Qt::ArrowCursor);
//                }


//                if (lineItemTmp->getItemStatus() == SELECTEDMOVE)
//                {
//                    //ITEM移动
//                    if (!m_beginMove)
//                    {
//                        m_beginMove = true;
//                        m_originPt = lineItemTmp->getOriLine().p1();
//                    }

//                    QPoint offsetPt = m_curPosPt - imgPt;

//                    QLineF oldLine = lineItemTmp->getLine();
//                    QLineF lineTmp = QLineF(oldLine.p1()-offsetPt, oldLine.p2()-offsetPt);

//                    lineItemTmp->setLine(lineTmp);
//                    lineItemTmp->calcOriGeometry(QPoint(m_PaintRect.topLeft().x(), m_PaintRect.topLeft().y()),
//                                                 m_img.width(),
//                                                 m_img.height(), m_scaleRate,
//                                                 0, false, false);

//                    m_curPosPt = imgPt;

//                    updateMesureDlgCtr(geometryItemTmp);

//                    updateMeasureTable();

//                    update(ui->widget_main->rect());

//                    return ;
//                }
//            }
        }

        return ;
    }
}

void MainWindow::updateOperatorStatus()
{
    if (m_ItemOperatorList.size() <= 0)
    {
        ui->pushButton_pre->setEnabled(false);
        ui->pushButton_next->setEnabled(false);
    }
    else
    {
        if (m_curOperatorIndex < 0)
        {
            ui->pushButton_pre->setEnabled(false);
            ui->pushButton_next->setEnabled(true);
        }
        else if (m_curOperatorIndex>=0 && m_curOperatorIndex==m_ItemOperatorList.size()-1)
        {
            ui->pushButton_pre->setEnabled(true);
            ui->pushButton_next->setEnabled(false);
        }
        else if (m_curOperatorIndex>0 && m_curOperatorIndex<m_ItemOperatorList.size()-1)
        {
            ui->pushButton_pre->setEnabled(true);
            ui->pushButton_next->setEnabled(true);
        }

    }
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

    if (dmfile.IsValid())
        dmfile.Release();

    if (nullptr != m_pImgPro)
    {
        delete []m_pImgPro;
        m_pImgPro = NULL;
    }

    if (nullptr != m_pImgDefect)
    {
        delete []m_pImgDefect;
        m_pImgDefect = nullptr;
    }

    //save config
    Appconfig::saveConfigFile();

    //delete temp file
    Appconfig::deleteAppTempFile(Appconfig::AppDataPath_Tmp);

    delete ui;
}


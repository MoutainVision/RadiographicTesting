#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMessageBox>

#include "Base/FunctionTransfer.h"

int point2Polygon(QPolygonF pts, int nPt, QPointF pt)
{
    //calc the outer rectangle
    long left, right, top, bottom;
    left   = pts[0].x();
    right  = pts[0].x();
    top    = pts[0].y();
    bottom = pts[0].y();
    for (int i=1; i<nPt; i++)
    {
        if (pts[i].x() < left)
            left = pts[i].x();
        if (pts[i].x() > right)
            right = pts[i].x();
        if (pts[i].y() < bottom)
            bottom = pts[i].y();
        if (pts[i].y() > top)
            top = pts[i].y();
    }

    if (pt.x() < left || pt.x() > right || pt.y() < bottom || pt.y() > top)
    {
        //the point is out of the polygon
        return -1;
    }
    else
    {
        //number of intersection
        int nIntersection = 0;

        const double eps = 1.0e-8;

        for (int i=0; i<nPt; i++)//for each edge
        {
            long x1 = pts[i].x();
            long y1 = pts[i].y();
            long x2, y2;
            if (i < nPt-1)
            {
                x2 = pts[i+1].x();
                y2 = pts[i+1].y();
            }
            else
            {
                x2 = pts[0].x();
                y2 = pts[0].y();
            }

            long maxx = x1 > x2 ? x1 : x2;
            long maxy = y1 > y2 ? y1 : y2;
            long minx = x1 < x2 ? x1 : x2;
            long miny = y1 < y2 ? y1 : y2;

            if (pt.x() > maxx || pt.y() > maxy || pt.y() < miny)
            {
                //no intersection with the edge
                continue;
            }

            if (maxy == miny)
            {
                if (pt.x() >= minx && pt.x() <=maxx)
                {
                    //the point is on the polygon
                    return 0;
                }
                else
                {
                    //ignor the horizontal edge, no counting for intersection
                    continue;
                }
            }
            else
            {
                double x0 = x1 + (double)(pt.y() - y1) * (x1 - x2) / (y1 - y2);
                if (fabs(x0-pt.x()) < eps)
                {
                    //the point is on the polygon
                    return 0;
                }
                else
                {
                    if (x0 < pt.x())
                    {
                        //no intersection with the edge
                        continue;
                    }
                    else
                    {
                        if (pt.y() == miny)
                        {
                            //intersection with the lower endpoint, ignored. no counting
                            continue;
                        }
                        else
                        {
                            nIntersection++;
                        }
                    }
                }
            }
        }

        if (nIntersection % 2 == 0)
        {
            //the point is out of the polygon
            return -1;
        }
        else
        {
            //the point is in the polygon
            return 1;
        }
    }
}

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
    ui->widget_img_pre->setMouseTracking(true);

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

    ui->widget_recheck->hide();

    ui->tableWidget_recheck->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);


    ui->horizontalScrollBar->hide();
    ui->verticalScrollBar->hide();

    ui->tabWidget->removeTab(1);
    ui->tabWidget->setCurrentIndex(0);


    ui->label_connectThr_recheck->hide();
    ui->spinBox_ConnectThr_recheck->hide();

    ui->label_connectThr->hide();
    ui->spinBox_ConnectThr->hide();


   ui->checkBox_contrast->hide();
   ui->spinBox_level->hide();
   ui->doubleSpinBox_power->hide();


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
    mBContrast = false;
    mBWind = true;

    mBShowDefect = true;
    mBShowCenter = true;

    mSourceX = 0;
    mSourceY = 0;

    m_eCurAction = HANDACTION;
    mBMeasureOpt = false;
    m_bIsPress = false;

    mCurDefectIndex = -1;

    mGeyImgWdg = NULL;
    mGreyRectItem = NULL;

    mDefectRectItem = NULL;

    mRecheckRectItem = NULL;

    m_loadingDlg = NULL;

    mPreWdg = NULL;

    mIndexFilePath = Appconfig::AppDataPath_Data + "index.idx";
    mIndexDataFilePath = Appconfig::AppDataPath_Data + "DcmIndex.dat";

    mIndexFileOfs.open(mIndexFilePath.toStdString(), ios::app | ios::out);

    QFile indexDataFile(mIndexDataFilePath);
    if (!indexDataFile.exists())
    {
        DCMIndexingFile::Create(mIndexDataFilePath.toStdString().c_str());
    }

    QFile indexListFile(mIndexFilePath);
    if (indexListFile.exists())
    {
        GetIndexList(mAIdxList, mIndexFilePath.toStdString().c_str());
    }

    //人工， 初始化有信号
    hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);

    mRotate = 0;

    ui->verticalSlider_diameter->setRange(5, 200);

    ui->pushButton_pre->hide();
    ui->pushButton_next->hide();

    ui->widget_ruler_cali->hide();

    ui->checkBox_ruler_cali->hide();

    mBReal = false;

//    ui->widget_wind->hide();

    ui->tableWidget_recognize->setContextMenuPolicy(Qt::CustomContextMenu);
    m_containerMenu = NULL;

    connect(ui->tableWidget_recognize, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slot_showManu(QPoint)));


    //
    mColorWdg = new ColorWdg();
    mColorWdg->hide();
    mColorWdg->installEventFilter(this);
    mColorWdg->setMouseTracking(true);

    connect(mColorWdg, &ColorWdg::sig_btnColorClick, [=](){
        QString style = "background-color: "+ mColorWdg->getCurColor().name() + ";";
        ui->pushButton_color->setStyleSheet(style);
    });

	connect(ui->checkBox_gray_mesure, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_search, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_pre_big, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_next_big, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));

    connect(ui->lineEdit_search, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));

    connect(ui->pushButton_open_pre, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->lineEdit_open_pre, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));

    //tool widget
    connect(ui->pushButton_color, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));


    //常规
    connect(ui->pushButton_measure, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_nomal, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_adapt, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->checkBox_measure_table, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));


    //查重
    connect(ui->pushButton_adapt_recheck, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_nomal_recheck, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));

    //标尺
    connect(ui->checkBox_ruler_cali, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->checkBox_ruler_apply, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));


    //调整
    connect(ui->pushButton_reset, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_invert, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_Mirror, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_Flip, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
//    connect(ui->pushButton_mag, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));

    connect(ui->pushButton_rotate, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));

    //
    connect(ui->pushButton_aoi, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->checkBox_recog_show_table, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_exe, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_recog_clear, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->checkBox_show_defect, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->checkBox_show_defect_center, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->checkBox_wind, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->checkBox_contrast, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_add_db, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_recheck, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));

    connect(ui->pushButton_reset_wind, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));

    connect(ui->pushButton_aoi_recheck, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_clear_recheck, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));

//    connect(ui->horizontalSlider_contrast, SIGNAL(valueChanged(int)), this, SLOT(slot_sliderWinValueChange(int)));

    connect(ui->verticalSlider_WinCentre, SIGNAL(valueChanged(int)), this, SLOT(slot_sliderWinValueChange(int)));
    connect(ui->verticalSlider_WindWidth, SIGNAL(valueChanged(int)), this, SLOT(slot_sliderWinValueChange(int)));


    connect(ui->horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(slot_scrollAreaXChange(int)));
    connect(ui->verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(slot_scrollAreaYChange(int)));

    connect(ui->verticalSlider_diameter, SIGNAL(valueChanged(int)), this, SLOT(slot_sliderValuechange(int)));

    connect(ui->buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(slot_btnGroupClick(QAbstractButton*)));
    connect(ui->pushButton_pre, SIGNAL(clicked()), this, SLOT(slot_btnPreToolClick()));
    connect(ui->pushButton_next, SIGNAL(clicked()), this, SLOT(slot_btnNextToolClick()));
    connect(ui->pushButton_delete, SIGNAL(clicked()), this, SLOT(slot_btnDeleteToolClick()));
    connect(ui->pushButton_eraser, SIGNAL(clicked()), this, SLOT(slot_btnEraserToolClick()));

    connect(ui->treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(clicked(QModelIndex)));
    connect(ui->treeView, SIGNAL(expanded(QModelIndex)), this, SLOT(expanded(QModelIndex)));

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slot_tabCurrentChanged(int)));

    connect(ui->tableWidget_recognize, SIGNAL(cellClicked(int, int)), this, SLOT(slot_tableCellClicked(int, int)));

    connect(ui->tableWidget_recheck, SIGNAL(cellClicked(int, int)), this, SLOT(slot_tableCellClicked(int, int)));

    connect(ui->tableWidget_measure, SIGNAL(cellClicked(int, int)), this, SLOT(slot_tableCellClicked(int, int)));

    ui->spinBox_GreyDiff->setValue(Appconfig::gDetectParam.nGreyDiff);
    ui->spinBox_ConnectThr->setValue(Appconfig::gDetectParam.nConnectThr);
    ui->spinBox_FilterRadius->setValue(Appconfig::gDetectParam.nFilterRadius);
    ui->spinBox_MinDefectArea->setValue(Appconfig::gDetectParam.nMinDefectArea);

    ui->spinBox_GreyDiff_recheck->setValue(Appconfig::gRecheckDetectParam.nGreyDiff);
    ui->spinBox_ConnectThr_recheck->setValue(Appconfig::gRecheckDetectParam.nConnectThr);
    ui->spinBox_FilterRadius_recheck->setValue(Appconfig::gRecheckDetectParam.nFilterRadius);
    ui->spinBox_MinDefectArea_recheck->setValue(Appconfig::gRecheckDetectParam.nMinDefectArea);



    connect(ui->spinBox_GreyDiff, SIGNAL(valueChanged(int)), this, SLOT(slot_paramValueChange(int)));
    connect(ui->spinBox_ConnectThr, SIGNAL(valueChanged(int)), this, SLOT(slot_paramValueChange(int)));
    connect(ui->spinBox_FilterRadius, SIGNAL(valueChanged(int)), this, SLOT(slot_paramValueChange(int)));
    connect(ui->spinBox_MinDefectArea, SIGNAL(valueChanged(int)), this, SLOT(slot_paramValueChange(int)));

    connect(ui->spinBox_GreyDiff_recheck, SIGNAL(valueChanged(int)), this, SLOT(slot_paramValueChange(int)));
    connect(ui->spinBox_ConnectThr_recheck, SIGNAL(valueChanged(int)), this, SLOT(slot_paramValueChange(int)));
    connect(ui->spinBox_FilterRadius_recheck, SIGNAL(valueChanged(int)), this, SLOT(slot_paramValueChange(int)));
    connect(ui->spinBox_MinDefectArea_recheck, SIGNAL(valueChanged(int)), this, SLOT(slot_paramValueChange(int)));

    connect(ui->action_about, SIGNAL(triggered()), this, SLOT(slot_menueToggle()));

    connect(ui->action_open, SIGNAL(triggered()), this, SLOT(slot_menueToggle()));
    connect(ui->action_save, SIGNAL(triggered()), this, SLOT(slot_menueToggle()));
    connect(ui->action_exit, SIGNAL(triggered()), this, SLOT(slot_menueToggle()));

    connect(ui->action_invert, SIGNAL(triggered()), this, SLOT(slot_menueToggle()));
    connect(ui->action_rotate, SIGNAL(triggered()), this, SLOT(slot_menueToggle()));
    connect(ui->action_mirror, SIGNAL(triggered()), this, SLOT(slot_menueToggle()));
    connect(ui->action_flip, SIGNAL(triggered()), this, SLOT(slot_menueToggle()));
    connect(ui->action_detect_param, SIGNAL(triggered()), this, SLOT(slot_menueToggle()));
    connect(ui->action_recheck_param, SIGNAL(triggered()), this, SLOT(slot_menueToggle()));

    connect(ui->action_delete_db, SIGNAL(triggered()), this, SLOT(slot_menueToggle()));
}


bool MainWindow::GetIndexList(vector<DCMFileIndex> &aIdxList, const char*szIndexFile)
{
    fstream ifs(szIndexFile, ios::in);
    if (ifs)
    {
		string str;
		unsigned nOff, nLen;
		string strDef;

		while (ifs >> str >> nOff >> nLen >> strDef)
		{
			aIdxList.push_back(DCMFileIndex(str, nOff, nLen, strDef));
		}

        return true;
    }

    return false;
}

void MainWindow::slot_showManu(QPoint pt)
{
    if (m_containerMenu)
    {
        delete m_containerMenu;
        m_containerMenu = NULL;
    }

    m_containerMenu = new QMenu(ui->tableWidget_recognize);


    m_actDelete = new QAction(ui->tableWidget_recognize);
    m_actDelete->setText(QStringLiteral("删除"));

    m_containerMenu->addAction(m_actDelete);

    connect(m_actDelete, SIGNAL(triggered()), this, SLOT(slot_actionDelete()));

    m_containerMenu->exec(QCursor::pos());
}

void MainWindow::slot_actionDelete()
{
    int curIndex = ui->tableWidget_recognize->currentRow();

    if (curIndex != -1)
    {
        ui->tableWidget_recognize->removeRow(curIndex);

        mADefectList.erase(mADefectList.begin() + curIndex);

        update();
    }
}

void MainWindow::slot_menueToggle()
{

    if (QObject::sender() == ui->action_about)
    {
        AboutDlg aboutdlg;

        aboutdlg.exec();
    }
    else if (QObject::sender() == ui->action_exit)
    {
        this->close();
    }
    else if (QObject::sender() == ui->action_delete_db)
    {
        bool bRemove = true;

        int result = QMessageBox::information(this, (QStringLiteral("提示")),  (QStringLiteral("是否确定删除数据库，删除后将无法恢复？")), QMessageBox::Yes | QMessageBox::No);

        switch (result)
        {
        case QMessageBox::Yes:
            bRemove = true;
            break;
        case QMessageBox::No:
            bRemove = false;
            break;
        default:
            bRemove = false;
            break;
        }

        if (bRemove)
        {
            //清空列表
            mAIdxList.clear();
            mIndexFileOfs.close();

            //删除
            bool bRemoveIndex = QFile::remove(mIndexFilePath);
            bool bRemoveData  = QFile::remove(mIndexDataFilePath);

            if (bRemoveIndex && bRemoveData)
            {
                QMessageBox::information(this, (QStringLiteral("提示")),  (QStringLiteral("删除成功.")));
            }

            //新建
            mIndexFileOfs.open(mIndexFilePath.toStdString(), ios::app | ios::out);

            QFile indexDataFile(mIndexDataFilePath);
            if (!indexDataFile.exists())
            {
                DCMIndexingFile::Create(mIndexDataFilePath.toStdString().c_str());
            }
        }
    }
    else if (QObject::sender() == ui->action_open)
    {
        QString s = QFileDialog::getOpenFileName(
             this, QStringLiteral("打开文件"),
             Appconfig::AppFilePath_OpenFile,//初始目录
             "dcm files (*.dcm)");

        if (!s.isEmpty())
        {
            QString dateTimeStr = QDateTime::currentDateTime().toString("MM-dd hh-mm-ss-zzz");
            QString outFileName = QString(QStringLiteral("%1/%2.jpg")).arg(Appconfig::AppDataPath_Tmp).arg(dateTimeStr);

            mCurDcmFileInfo.filePath = s;
            mCurDcmFileInfo.transFilePath = outFileName;

            setDcmFileInfo();
        }

    }
    else if (QObject::sender() == ui->action_save)
    {
        QFileInfo info(mCurDcmFileInfo.filePath);


        QString s = QFileDialog::getSaveFileName(
                   this, QStringLiteral("保存文件"),
                       info.absolutePath() +"//"+ info.baseName() + ".bmp",//初始目录
                    "bmp files (*.bmp)");

       if (!s.isEmpty())
       {
           unsigned short *pImgSave;  //处理图像

           pImgSave = new unsigned short[mSrcImgWidth * mSrcImgHeight];
           memcpy(pImgSave, m_pSrcImg, mSrcImgWidth*mSrcImgHeight*sizeof(unsigned short));


           //窗宽窗位
           if (mBWind)
           {
                if (mWinCentre>1 && mWinWidth>1)
                    WindowLevelTransform(pImgSave, mSrcImgWidth, mSrcImgHeight, mWinCentre, mWinWidth);
           }

           //反相
           if (mBInvert)
           {
               Invert(pImgSave, mSrcImgWidth, mSrcImgHeight);
           }

            QImage img;
            shortImgToImage(pImgSave, mSrcImgWidth, mSrcImgHeight, img);

            if (img.save(s, "bmp"))
            {
                QMessageBox::information(this, (QStringLiteral("提示")),  (QStringLiteral("保存成功.")));
            }
       }
       else
           QMessageBox::information(this, (QStringLiteral("提示")),  (QStringLiteral("文件名不可为空.")));

    }
    else if (QObject::sender() == ui->action_invert)
    {
        mBInvert = ui->action_invert->isChecked();
        ui->pushButton_invert->setChecked(mBInvert);

        delImg();
    }
    else if (QObject::sender() == ui->action_rotate)
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
    else if (QObject::sender() == ui->action_mirror)
    {
        mBMirror = ui->action_mirror->isChecked();
        ui->pushButton_Mirror->setChecked(mBMirror);
        delImg();
    }
    else if (QObject::sender() == ui->action_flip)
    {
        mBFlip = ui->action_flip->isChecked();
        ui->pushButton_Flip->setChecked(mBFlip);

        delImg();
    }
    else if (QObject::sender() == ui->action_detect_param)
    {
        MyDetectParam param;

        ui->spinBox_GreyDiff->setValue(param.nGreyDiff);
        ui->spinBox_ConnectThr->setValue(param.nConnectThr);
        ui->spinBox_FilterRadius->setValue(param.nFilterRadius);
        ui->spinBox_MinDefectArea->setValue(param.nMinDefectArea);
    }
    else if (QObject::sender() == ui->action_recheck_param)
    {
        MyDetectParam param;

        ui->spinBox_GreyDiff_recheck->setValue(param.nGreyDiff);
        ui->spinBox_ConnectThr_recheck->setValue(param.nConnectThr);
        ui->spinBox_FilterRadius_recheck->setValue(param.nFilterRadius);
        ui->spinBox_MinDefectArea_recheck->setValue(param.nMinDefectArea);
    }


}

void MainWindow::slot_paramValueChange(int value)
{
    Appconfig::gDetectParam.nGreyDiff      = ui->spinBox_GreyDiff->value();
    Appconfig::gDetectParam.nConnectThr    = ui->spinBox_ConnectThr->value();
    Appconfig::gDetectParam.nFilterRadius  = ui->spinBox_FilterRadius->value();
    Appconfig::gDetectParam.nMinDefectArea = ui->spinBox_MinDefectArea->value();

    Appconfig::gRecheckDetectParam.nGreyDiff      = ui->spinBox_GreyDiff_recheck->value();
    Appconfig::gRecheckDetectParam.nConnectThr    = ui->spinBox_ConnectThr_recheck->value();
    Appconfig::gRecheckDetectParam.nFilterRadius  = ui->spinBox_FilterRadius_recheck->value();
    Appconfig::gRecheckDetectParam.nMinDefectArea = ui->spinBox_MinDefectArea_recheck->value();

}

void MainWindow::showLoading(QString msg)
{
    if (NULL == m_loadingDlg)
    {
        m_loadingDlg = new Loading(this);
    }

    m_loadingDlg->setWaitMsg(msg);

    m_loadingDlg->showLoading();
}

void MainWindow::closeLoading()
{
    if (NULL != m_loadingDlg)
        m_loadingDlg->closeLoading();
}

void MainWindow::slot_tabCurrentChanged(int index)
{
    if (index != 2)
    {
        if (NULL != mRecheckRectItem)
        {
            delete mRecheckRectItem;
            mRecheckRectItem = NULL;
        }
    }

    if (index == 0)
    {
        ui->pushButton_pre_big->show();
        ui->pushButton_next_big->show();

        ui->widget_recheck->hide();
    }
    else if (index == 2)
    {
        //--查重--
        ui->pushButton_pre_big->hide();
        ui->pushButton_next_big->hide();

        //测量
        ui->checkBox_measure_table->setChecked(false);
        ui->widget_measure->hide();

        ui->pushButton_measure->setChecked(false);
        ui->widget_tool->hide();

        //灰度测量
        mGrayLine = QLine();
        if (NULL != mGeyImgWdg)
            mGeyImgWdg->close();

        if (NULL != mGreyRectItem)
        {
            delete mGreyRectItem;
            mGreyRectItem = NULL;
        }

        //识别
        ui->pushButton_aoi->setChecked(false);
        ui->pushButton_aoi_recheck->setChecked(false);

        ui->widget_recognize_table->hide();

        //
        ui->widget_recheck->show();
    }
    else
    {
        ui->pushButton_pre_big->hide();
        ui->pushButton_next_big->hide();

        ui->widget_recheck->hide();
    }
}

void MainWindow::slot_sliderValuechange(int value)
{
//    int value = ui->verticalSlider_diameter->value();
    mScale = (float)value / 100;

    ui->lineEdit_diameter->setText(QString("%1").arg(value));

    delImg();
}

void MainWindow::slot_tableCellClicked(int row, int col)
{
    if (QObject::sender() == ui->tableWidget_recognize)
    {
        mCurDefectIndex = row;

        update();
    }
    else if (QObject::sender() == ui->tableWidget_measure)
    {
        clearItemSelected(m_selectedIndex);

        m_selectedIndex = row;

        if (m_selectedIndex < m_geometryItemList.size())
            m_geometryItemList.at(m_selectedIndex)->setItemStatus(SELECTEDMOVE);

        update();
    }
    else if (QObject::sender() == ui->tableWidget_recheck)
    {
        QString filePath = ui->tableWidget_recheck->item(row, 2)->text();
        std::thread([=] {

            qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));          //设置随机数种子
            int rand = qrand() % 10000;

            QString dateTimeStr = QDateTime::currentDateTime().toString("MM-dd hh-mm-ss-zzz");
            QString outFileName = QString(QStringLiteral("%1/%2_%3.jpg")).arg(Appconfig::AppDataPath_Tmp).arg(dateTimeStr).arg(rand);

            std::string errorStr;
            ReadDCMFile::readDCMFileLib(filePath.toLocal8Bit().toStdString(), outFileName.toLocal8Bit().toStdString(), errorStr);

            FunctionTransfer::runInMainThread([=]()
            {
                DcmFileNode info;
                info.transFilePath = outFileName;

                if (NULL == mPreWdg)
                    mPreWdg = new PreWdg;

                mPreWdg->setParent(ui->widget_recheck_pre);
                mPreWdg->setDCMFileInfo(info);

                int perWidth  = ui->widget_recheck_pre->width();
                int perHeight = ui->widget_recheck_pre->height();

                mPreWdg->setFixedSize(perWidth, perHeight);
                mPreWdg->setGeometry(0, 0, perWidth, perHeight);
                mPreWdg->show();

            });

        }).detach();
    }
}

void MainWindow::slot_sliderWinValueChange(int value)
{
    if (QObject::sender() == ui->verticalSlider_WinCentre)
    {
        mWinCentre = value;
        ui->label_wincentre->setText(QString("%1").arg(value));

        delImg();
    }
    else if (QObject::sender() == ui->verticalSlider_WindWidth)
    {
        mWinWidth = value;
        ui->label_winwidth->setText(QString("%1").arg(value));

        delImg();
    }
//    else if (QObject::sender() == ui->horizontalSlider_contrast)
//    {
//        mContrast = value;
//        ui->label_contrast->setText(QString("%1").arg(value));

//        if (mBContrast)
//            delImg();
//    }
}

void MainWindow::setDcmFileInfo()
{
    ui->verticalSlider_WinCentre->blockSignals(true);
    ui->verticalSlider_WindWidth->blockSignals(true);


    ui->verticalSlider_WinCentre->blockSignals(false);
    ui->verticalSlider_WindWidth->blockSignals(false);

    //reset
    ui->checkBox_measure_table->setChecked(false);
    ui->widget_measure->hide();

    ui->checkBox_recog_show_table->setChecked(false);
    ui->widget_recognize_table->hide();

    ui->pushButton_measure->setChecked(false);
    ui->widget_tool->hide();

    ui->pushButton_invert->setChecked(false);
    mBInvert = false;

    ui->pushButton_Mirror->setChecked(false);
    mBMirror = false;

    ui->pushButton_Flip->setChecked(false);
    mBFlip = false;

    ui->checkBox_wind->setChecked(true);
    mBWind = true;

    ui->checkBox_contrast->setChecked(false);
    mBContrast = false;

    mRotate = 0;
    mNeedRotate = 0;

    //清空缺陷
    clearDefect();

    if (NULL != mDefectRectItem)
    {
        delete mDefectRectItem;
        mDefectRectItem = NULL;
    }

    if (NULL != mRecheckRectItem)
    {
        delete mRecheckRectItem;
        mRecheckRectItem = NULL;
    }

    //清空几何图像
    slot_btnDeleteToolClick();

    //清空查重表格
    ui->widget_recheck->hide();
    ui->tableWidget_recheck->clearContents();
    ui->tableWidget_recheck->setRowCount(0);

    //灰度测量
    mGrayLine = QLine();
    if (NULL != mGeyImgWdg)
        mGeyImgWdg->close();

    if (NULL != mGreyRectItem)
    {
        delete mGreyRectItem;
        mGreyRectItem = NULL;
    }

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
            m_pSrcImg = dmfile.GetBuffer();
            mSrcImgWidth = dmfile.GetWidth();
            mSrcImgHeight = dmfile.GetHeight();

            mImgProLock.lock();

            if (nullptr != m_pImgPro)
            {
                delete []m_pImgPro;
                m_pImgPro = NULL;
            }

            m_pImgPro = new unsigned short[mSrcImgWidth * mSrcImgHeight];
            memcpy(m_pImgPro, m_pSrcImg, mSrcImgWidth*mSrcImgHeight*sizeof(unsigned short));

            mImgProLock.unlock();

			ui->verticalSlider_WinCentre->setValue(mCurDcmFileInfo.winCentre);
			ui->verticalSlider_WindWidth->setValue(mCurDcmFileInfo.windWidth);


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


    float b = floor(dRat * 1000.000f + 0.5) / 1000.000f;

    mScale = b;

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

    ui->checkBox_wind->setChecked(true);
    mBWind = true;

    ui->checkBox_contrast->setChecked(false);
    mBContrast = false;

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

    mImgProLock.lock();
    Resize(m_pImgPro, srcW, srcH, newW, newH);
    mImgProLock.unlock();

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

    mImgProLock.lock();
    Resize(m_pImgPro, srcW, srcH, mCurImgWidth, mCurImgHeight);
    mImgProLock.unlock();

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
//    shortImgToImage(pImg, nW, nH, mPaintImg);

//    mDelImgLock.lock();
//    if (!mBDelImging)
//    {
//        mDelImgLock.unlock();
//        SetEvent(hEvent);
//        return ;
//    }
//    mDelImgLock.unlock();

//	SetEvent(hEvent);

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

        mSourceRect = QRect(0, 0, mPaintRect.width(), mPaintRect.height());
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
           // qDebug() << __FUNCTION__ << "setValue";

            ui->verticalScrollBar->show();
            ui->verticalScrollBar->setValue((mPaintRect.height() - ui->widget_img_pre->height())/2);
        }
        else
        {
            mSourceY = 0;
            ui->horizontalScrollBar->hide();
        }

        int nW = ui->widget_img_pre->width();
        int nH = ui->widget_img_pre->height();

        if (mPaintRect.width() <= nW)
            mSourceRect = QRect(mSourceX, mSourceY, mPaintRect.width(), nH);
        else
            mSourceRect = QRect(mSourceX, mSourceY, nW, nH);


        update();
    }
}


void MainWindow::showScrollBar()
{
//    qDebug() << __FUNCTION__ << mPaintRect.width() << mPaintRect.height()
//                 << ui->widget_img_pre->width() << ui->widget_img_pre->height();

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

//        qDebug() << __FUNCTION__ << value << nH << mPaintRect.width();

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

void MainWindow::actionActionChange()
{
    if (mBMeasureOpt)
    {
        //灰度测量
        mGrayLine = QLine();
        if (NULL != mGeyImgWdg)
            mGeyImgWdg->close();

        if (NULL != mGreyRectItem)
        {
            delete mGreyRectItem;
            mGreyRectItem = NULL;
        }

        ui->checkBox_gray_mesure->setChecked(false);

        //缺陷
        ui->pushButton_aoi->setChecked(false);

        //
        ui->pushButton_hand->setChecked(true);
        m_eCurAction = HANDACTION;

        ui->pushButton_measure->setChecked(true);
    }
    else
    {
        //测量
        ui->checkBox_measure_table->setChecked(false);
        ui->widget_measure->hide();

        ui->pushButton_measure->setChecked(false);
        ui->widget_tool->hide();

        if (m_eCurAction == GREAYACTION || m_eCurAction == AOIACTION)
        {
            if (NULL != mRecheckRectItem)
            {
                delete mRecheckRectItem;
                mRecheckRectItem = NULL;
            }
        }

        if (m_eCurAction == GREAYACTION)
        {
            ui->pushButton_aoi->setChecked(false);
            ui->pushButton_aoi_recheck->setChecked(false);

        }
        else if (m_eCurAction == AOIACTION)
        {
            //灰度测量
            mGrayLine = QLine();
            if (NULL != mGeyImgWdg)
                mGeyImgWdg->close();

            if (NULL != mGreyRectItem)
            {
                delete mGreyRectItem;
                mGreyRectItem = NULL;
            }

			ui->checkBox_gray_mesure->setChecked(false);

        }
        else if (m_eCurAction == AOIRECHECKACTION)
        {
            //灰度测量
            mGrayLine = QLine();
            if (NULL != mGeyImgWdg)
                mGeyImgWdg->close();

            if (NULL != mGreyRectItem)
            {
                delete mGreyRectItem;
                mGreyRectItem = NULL;
            }

            //缺陷
            ui->checkBox_recog_show_table->setChecked(false);
            ui->widget_recognize_table->hide();

        }
    }

    update();
}

void MainWindow::clearDefect()
{
    mCurDefectIndex = -1;
    mADefectList.clear();
    ui->tableWidget_recognize->clearContents();
    ui->tableWidget_recognize->setRowCount(0);

    update();
}

QPoint MainWindow::convertImgPt(QPoint wPt)
{
    return QPoint(wPt.x() - mPaintRectReal.x() + mSourceRect.x(),
                  wPt.y() - mPaintRectReal.y() + mSourceRect.y());
}

QPoint MainWindow::convertWdgPt(QPoint iPt)
{
    return QPoint(iPt.x() + mPaintRectReal.x() - mSourceRect.x(),
                  iPt.y() + mPaintRectReal.y() - mSourceRect.y());
}

QPoint MainWindow::getCurOffset()
{
    return QPoint(mPaintRectReal.x() - mSourceRect.x(),
                 mPaintRectReal.y() - mSourceRect.y());
}

void MainWindow::calcIntensityCurve(QPoint p1, QPoint p2)
{
    QPoint iP1 = convertImgPt(p1);
    QPoint iP2 = convertImgPt(p2);

    m_iP1 = iP1;
    m_iP2 = iP2;

//    mImgProLock.lock();
    vector<unsigned short> aIntensity;
//    GetIntensityCurve(aIntensity, m_pImgPro, mCurImgWidth, mCurImgHeight, iP1.x(),
//                      iP1.y(), iP2.x(), iP2.y());

//    mImgProLock.unlock();

    GetIntensityCurve(aIntensity, m_pSrcImg, mSrcImgWidth, mSrcImgHeight, iP1.x() / mScale,
                      iP1.y() / mScale, iP2.x() / mScale, iP2.y() / mScale);


    float ab = iP2.x() - iP1.x();
    float bc = iP2.y() - iP1.y();

    m_angle = atan(ab / bc);

    setIntensityCurveValues(aIntensity);
}

void MainWindow::calcCurGreyPos(int length)
{

}

void MainWindow::setIntensityCurveValues(vector<unsigned short> aIntensity)
{
    if (nullptr == mGeyImgWdg)
    {
        mGeyImgWdg = new GeyImgWdg;

        connect(mGeyImgWdg, &GeyImgWdg::sig_curGreayPos, [=](){

            int length = mGeyImgWdg->getCurIndex();

            float abx = length * cos(m_angle) * mScale;
            float bcx = length * sin(m_angle) * mScale;

            QPoint tempIp;
            QPoint tempWp;

            if (m_iP1.x() < m_iP2.x())
            {
                if (m_angle < 0)
                    tempIp = QPoint(m_iP1.x() - bcx, m_iP1.y() - abx);
                else
                    tempIp = QPoint(m_iP1.x() + bcx, m_iP1.y() + abx);
            }
            else
            {
                if (m_angle < 0)
                    tempIp = QPoint(m_iP1.x() + bcx, m_iP1.y() + abx);
                else
                    tempIp = QPoint(m_iP1.x() - bcx, m_iP1.y() - abx);
            }

            tempWp = convertWdgPt(tempIp);
       //     tempWp = convertWdgPt(QPoint(tempIp.x() / mScale, tempIp.y() / mScale));

            if (NULL != mGreyRectItem)
            {
                delete mGreyRectItem;
                mGreyRectItem = NULL;
            }

            mGreyRectItem = new RectItem(QRectF(QPointF(tempWp.x()-7, tempWp.y()-7), QSizeF(14, 14)));

            mGreyRectItem->calcOriGeometry(getCurOffset(),
                                           mCurImgWidth,
                                           mCurImgHeight,
                                           mScale,
                                           mNeedRotate, mBFlip, mBMirror);

            update();
        });
    }

    mGeyImgWdg->SetIntensityCurveValues(aIntensity, aIntensity.size());
    mGeyImgWdg->move(QPoint(this->width() - mGeyImgWdg->width(), this->height() - mGeyImgWdg->height()));
    mGeyImgWdg->show();


}

void MainWindow::clearGreyLine()
{
    mGrayLine = QLine();
}

void MainWindow::closeGreyWdg()
{
    if (nullptr != mGeyImgWdg)
    {
        mGeyImgWdg->close();
    }
}

void MainWindow::getIntensity(QPoint curPt)
{
    if (nullptr != m_pSrcImg)
	{
		QPoint imgPt = convertImgPt(curPt);

        /*if (imgPt.x() > 0 && imgPt.x()<mPaintRectReal.width()
                && imgPt.y() > 0 && imgPt.y()<mPaintRectReal.height())*/
        if (imgPt.x() > 0 && (imgPt.x() / mScale) < mSrcImgWidth
            && imgPt.y() > 0 && (imgPt.y() / mScale) < mSrcImgHeight)
        {
          //  mDelImgLock.lock();

            unsigned short intensity = 0;
            GetIntensity(intensity, m_pSrcImg, mSrcImgWidth, mSrcImgHeight,
                imgPt.x() / mScale, imgPt.y()/ mScale);

           // mDelImgLock.unlock();

            ui->label_X_Value->setText(QString("%1").arg((int)(imgPt.x() / mScale)));
            ui->label_Y_Value->setText(QString("%1").arg((int)(imgPt.y() / mScale)));

            ui->label_intensity->setText(QString("%1").arg(intensity));
        }
        else
        {
            ui->label_X_Value->clear();
            ui->label_Y_Value->clear();
            ui->label_intensity->clear();
        }
	}   
}

void MainWindow::addRetrievalResultValues(vector<RetrievalResult> aRes)
{
    for (size_t k = 0; k != aRes.size(); ++k)
    {
        int row = ui->tableWidget_recheck->rowCount();
        ui->tableWidget_recheck->setRowCount(row + 1);

        QTableWidgetItem *indexItem = new QTableWidgetItem;
        indexItem->setText(QString("%1").arg(k));
        indexItem->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *dbIndexItem = new QTableWidgetItem;
        dbIndexItem->setText(QString("%1").arg(aRes[k].index));
        dbIndexItem->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *pathItem = new QTableWidgetItem;
        pathItem->setText(QString("%1").arg(aRes[k].strMatchedFile.c_str()));
        pathItem->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *similarItem = new QTableWidgetItem;
        similarItem->setText(QString("%1%").arg(aRes[k].dSimilarity*100));
        similarItem->setTextAlignment(Qt::AlignCenter);

        ui->tableWidget_recheck->setItem(row, 0, indexItem);
        ui->tableWidget_recheck->setItem(row, 1, dbIndexItem);
        ui->tableWidget_recheck->setItem(row, 2, pathItem);
        ui->tableWidget_recheck->setItem(row, 3, similarItem);


    }

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

//    QTableWidgetItem *rectnessItem = new QTableWidgetItem;
//    rectnessItem->setText(QString("%1").arg(feat.rectness));
//    rectnessItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *aveGreyItem = new QTableWidgetItem;
    aveGreyItem->setText(QString("%1").arg(feat.aveGrey));
    aveGreyItem->setTextAlignment(Qt::AlignCenter);

//    QTableWidgetItem *greyContrastItem = new QTableWidgetItem;
//    greyContrastItem->setText(QString("%1").arg(feat.grey_contrast));
//    greyContrastItem->setTextAlignment(Qt::AlignCenter);


    //--additems--
    ui->tableWidget_recognize->setItem(row, 0, indexItem);
    ui->tableWidget_recognize->setItem(row, 1, areaItem);
    ui->tableWidget_recognize->setItem(row, 2, periItem);
    ui->tableWidget_recognize->setItem(row, 3, periAreaItem);
    ui->tableWidget_recognize->setItem(row, 4, lengthItem);
    ui->tableWidget_recognize->setItem(row, 5, widthItem);
    ui->tableWidget_recognize->setItem(row, 6, rndnessItem);
//    ui->tableWidget_recognize->setItem(row, 7, rectnessItem);
    ui->tableWidget_recognize->setItem(row, 7, aveGreyItem);
//    ui->tableWidget_recognize->setItem(row, 9, greyContrastItem);
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

                ui->treeView->setCurrentIndex(preIndex);
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

                ui->treeView->setCurrentIndex(preIndex);
            }
        }
    }
    else if (QObject::sender() == ui->checkBox_measure_table)
    {
        ui->widget_measure->setVisible(ui->checkBox_measure_table->isChecked());
    }
    else if (QObject::sender() == ui->pushButton_search)
    {
        QString s = QFileDialog::getExistingDirectory(
            this, "open file dialog",
                Appconfig::AppFilePath_OpenFile);

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

        actionActionChange();
    }
    else if (QObject::sender() == ui->checkBox_ruler_cali)
    {
        ui->widget_ruler_cali->setVisible(ui->checkBox_ruler_cali->isChecked());
    }
    else if (QObject::sender() == ui->checkBox_ruler_apply)
    {
        mBReal = ui->checkBox_ruler_apply->isChecked();
        ui->doubleSpinBox_real_mm->setEnabled(!mBReal);

        double value = ui->doubleSpinBox_real_mm->value();

        for (int i=0; i<m_geometryItemList.size(); i++)
        {
            if (mBReal)
                m_geometryItemList.at(i)->setUnit(MICUNIT);
            else
                m_geometryItemList.at(i)->setUnit(PIXELUNIT);

            m_geometryItemList.at(i)->setSpatialResolution(value);
            m_geometryItemList.at(i)->changeToUnitType();
        }

        updateMeasureTable();
    }
    else if (QObject::sender() == ui->pushButton_adapt || QObject::sender() == ui->pushButton_adapt_recheck)
    {
        showAdapt();
    }
    else if (QObject::sender() == ui->pushButton_nomal || QObject::sender() == ui->pushButton_nomal_recheck)
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
        ui->action_invert->setChecked(mBInvert);

        delImg();
    }
    else if (QObject::sender() == ui->pushButton_Flip)
    {
        mBFlip = ui->pushButton_Flip->isChecked();
        ui->action_flip->setChecked(mBFlip);

        delImg();
    }
    else if (QObject::sender() == ui->pushButton_Mirror)
    {
        mBMirror = ui->pushButton_Mirror->isChecked();
        ui->action_mirror->setChecked(mBMirror);

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
        if (ui->pushButton_aoi->isChecked())
        {
//            m_ePreAction = m_eCurAction;

            m_eCurAction = AOIACTION;
//            ui->widget_tool->setEnabled(false);
        }
        else
        {
            m_eCurAction = HANDACTION;
//            ui->widget_tool->setEnabled(true);
        }

        mBMeasureOpt = false;

        actionActionChange();
    }
    else if (QObject::sender() == ui->checkBox_recog_show_table)
    {
        ui->widget_recognize_table->setVisible(ui->checkBox_recog_show_table->isChecked());
    }
    else if (QObject::sender() == ui->checkBox_wind)
    {
        mBWind = ui->checkBox_wind->isChecked();
        ui->widget_wind->setVisible(mBWind);

        delImg();
    }
    else if (QObject::sender() == ui->pushButton_reset_wind)
    {
        ui->verticalSlider_WinCentre->setValue(mCurDcmFileInfo.winCentre);
        ui->verticalSlider_WindWidth->setValue(mCurDcmFileInfo.windWidth);

        delImg();
    }
    else if (QObject::sender() == ui->checkBox_contrast)
    {
        mBContrast = ui->checkBox_contrast->isChecked();
        mLevel     = ui->spinBox_level->value();
        mPower     = ui->doubleSpinBox_power->value();

        delImg();
    }
    else if (QObject::sender() == ui->checkBox_show_defect)
    {
        mBShowDefect = ui->checkBox_show_defect->isChecked();
        update();
    }
    else if (QObject::sender() == ui->checkBox_show_defect_center)
    {
        mBShowCenter = ui->checkBox_show_defect_center->isChecked();
        update();
    }
    else if (QObject::sender() == ui->pushButton_exe)
    {
        m_eCurAction = m_ePreAction;
        ui->widget_tool->setEnabled(true);
        ui->pushButton_aoi->setChecked(false);

        exeDefectImg();
    }
    else if (QObject::sender() == ui->pushButton_add_db)
    {

        //if (mADefectList.size() > 0 && dmfile.IsValid())
        if (dmfile.IsValid())
        {
            QFileInfo curFile(mCurDcmFileInfo.filePath);

            bool isExist = false;
            int isExistCount = 0;
            for (int i=0; i<mAIdxList.size(); i++)
            {
                QFileInfo idFile(mAIdxList.at(i).strFullPath.c_str());

                if (idFile.fileName() == curFile.fileName())
                {
                    isExist = true;
                    isExistCount++;
                   // break;
                }
            }

            bool needAdd = true;
            if (isExist)
            {
                int result = QMessageBox::information(this, (QStringLiteral("提示")),  (QStringLiteral("数据库中已经存在，是否继续写入？")), QMessageBox::Yes | QMessageBox::No);

                switch (result)
                {
                case QMessageBox::Yes:
                    needAdd = true;
                    break;
                case QMessageBox::No:
                    needAdd = false;
                    break;
                default:
                    needAdd = false;
                    break;
                }
            }

            if (needAdd)
            {
                QString dbFilePath = QString("%1//%2").arg(Appconfig::AppFilePath_Img_Online).arg(curFile.fileName());

                //copy src img to data
                if (!isExist)
                {
                    QFile copyFile(mCurDcmFileInfo.filePath);
                    copyFile.copy(dbFilePath);
                }


                //index file open
                if (!mIndexFileOfs.is_open())
                    mIndexFileOfs.open(mIndexFilePath.toStdString(), ios::app | ios::out);

                mIndexData.strFullPath = dbFilePath.toLocal8Bit().toStdString();
                mIndexData.fileFeat = dmfile.getFileFeature();

                for (size_t n = 0; n != mADefectList.size(); ++n)
                {
                    mIndexData.aDefectList.push_back(mADefectList.at(n).feat);
                }


                QString defFileStrT = QString("%1//%2_%3.def").arg(Appconfig::AppFilePath_Img_Online).arg(curFile.fileName()).arg(isExistCount);

                string strDefFile = defFileStrT.toLocal8Bit().toStdString();

                if (mADefectList.size() > 0)
                {
                    //保存缺陷文件
                    SaveDefect(mADefectList, strDefFile.c_str());
                }

                DCMFileIndex idx;
                DCMIndexingFile::Write(idx, mIndexDataFilePath.toStdString().c_str(), mIndexData);

                //把当前文件的索引信息写入所以文件中
    //            mIndexFileOfs << mCurDcmFileInfo.filePath.toStdString() << "\t" << idx.nOffset << "\t" << idx.nLength << "\n";
                mIndexFileOfs << dbFilePath.toStdString() << "\t" << idx.nOffset << "\t" << idx.nLength  << "\t" << strDefFile << "\n";
                mIndexFileOfs.close();

                //添加到列表中
                DCMFileIndex idxT;
                idxT.strFullPath = dbFilePath.toStdString();
                idxT.nOffset = idx.nOffset;
                idxT.nLength = idx.nLength;
                idxT.strDefFile = strDefFile;

                mAIdxList.push_back(idxT);

                QMessageBox::information(this, (QStringLiteral("提示")),  (QStringLiteral("加入数据库成功。")));
            }
        }
    }
    else if (QObject::sender() == ui->pushButton_aoi_recheck)
    {
        if (ui->pushButton_aoi_recheck->isChecked())
        {
//            m_ePreAction = m_eCurAction;

            m_eCurAction = AOIRECHECKACTION;
//            ui->widget_tool->setEnabled(false);
        }
        else
        {

            m_eCurAction = HANDACTION;
//            m_eCurAction = m_ePreAction;
//            ui->widget_tool->setEnabled(true);
        }

        mBMeasureOpt = false;
        actionActionChange();
    }
    else if (QObject::sender() == ui->pushButton_recheck)
    {
        if (dmfile.IsValid())
        {
            showLoading(QStringLiteral("正在查重，请稍等"));

            std::thread([&] {

                //查重
                vector<RetrievalResult> aRes;

                DetectParam param;
                param.nGreyDiff     = ui->spinBox_GreyDiff_recheck->value();
                param.nConnectThr   = ui->spinBox_ConnectThr_recheck->value();
                param.nFilterRadius = ui->spinBox_FilterRadius_recheck->value();
                param.nMinDefectArea = ui->spinBox_MinDefectArea_recheck->value();

                ImageRect pRoi;
                if (NULL != mRecheckRectItem)
                {
                    QRectF rect = mRecheckRectItem->getOriRect();
                    pRoi.xs = rect.left();
                    pRoi.xe = rect.right();
                    pRoi.ys = rect.top();
                    pRoi.ye = rect.bottom();
                }

                Search(aRes, dmfile, pRoi, param, mIndexFilePath.toStdString().c_str(), mIndexDataFilePath.toStdString().c_str());

                FunctionTransfer::runInMainThread([=]()
                {
                    ui->tableWidget_recheck->clearContents();
                    ui->tableWidget_recheck->setRowCount(0);

                    addRetrievalResultValues(aRes);

                    closeLoading();
                });

            }).detach();
        }
    }
    else if (QObject::sender() == ui->pushButton_clear_recheck)
    {
        if (NULL != mRecheckRectItem)
        {
            delete mRecheckRectItem;
            mRecheckRectItem = NULL;
        }

        //清空查重表格
        ui->tableWidget_recheck->clearContents();
        ui->tableWidget_recheck->setRowCount(0);

        update();
    }
    else if (QObject::sender() == ui->pushButton_recog_clear)
    {
        clearDefect();

        if (NULL != mDefectRectItem)
        {
            delete mDefectRectItem;
            mDefectRectItem = NULL;
        }

        update();
    }
    else if (QObject::sender() == ui->checkBox_gray_mesure)
    {
        if (ui->checkBox_gray_mesure->isChecked())
        {
//            m_ePreAction = m_eCurAction;

            m_eCurAction = GREAYACTION;/*
            ui->widget_tool->setEnabled(false);

            ui->pushButton_hand->setEnabled(false);
            ui->pushButton_arrow->setEnabled(false);
            ui->pushButton_ellipse->setEnabled(false);
            ui->pushButton_rect->setEnabled(false);
            ui->pushButton_line->setEnabled(false);*/
        }
        else
        {


            mGrayLine = QLine();
			if (NULL != mGeyImgWdg)
				mGeyImgWdg->close();

            if (NULL != mGreyRectItem)
            {
                delete mGreyRectItem;
                mGreyRectItem = NULL;
            }

            m_eCurAction = HANDACTION;
//            m_eCurAction = m_ePreAction;
//            ui->widget_tool->setEnabled(true);

//            ui->pushButton_hand->setEnabled(true);
//            ui->pushButton_arrow->setEnabled(true);
//            ui->pushButton_ellipse->setEnabled(true);
//            ui->pushButton_rect->setEnabled(true);
//            ui->pushButton_line->setEnabled(true);

            update();
        }

        mBMeasureOpt = false;

        actionActionChange();
    }
    else if (QObject::sender() == ui->pushButton_color)
    {
        mColorWdg->show();

        QPoint GlobalPoint(ui->pushButton_color->mapToGlobal(QPoint(13, 0)));
        mColorWdg->move(GlobalPoint.x()-mColorWdg->width()/2, GlobalPoint.y()-mColorWdg->height()-10);

        mColorWdg->setFocus();
    }
}


void MainWindow::exeDefectImg()
{
    showLoading(QStringLiteral("正在查找缺陷，请稍等"));


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

    //镜像
    if (mBMirror)
        Mirror(m_pImgDefect, imgW, imgH);

    //翻转
    if (mBFlip)
        Flip(m_pImgDefect, imgW, imgH);

//    //窗宽窗位
//    if (mBWind)
//    {
//        if (mWinCentre>1 && mWinWidth>1)
//            WindowLevelTransform(m_pImgDefect, mSrcImgWidth, mSrcImgHeight, mWinCentre, mWinWidth);
//    }

//    //对比度
//    if (mBContrast)
//    {
//        IPFuncMUSICA(m_pImgDefect, mCurImgWidth, mCurImgHeight, mLevel, mPower);

//        //ContrastEnhancement(m_pImgPro, mCurImgWidth, mCurImgHeight, mContrast);
//    }

//    //反相
//    if (mBInvert)
//        Invert(m_pImgDefect, imgW, imgH);


    //旋转
    if (mNeedRotate == 90)
        Rotate90(m_pImgDefect, imgW, imgH);
    else if (mNeedRotate == 180)
        Rotate180(m_pImgDefect, imgW, imgH);
    else if (mNeedRotate == 270)
        Rotate270(m_pImgDefect, imgW, imgH);

    DetectParam param;
    param.nGreyDiff     = ui->spinBox_GreyDiff->value();
    param.nConnectThr   = ui->spinBox_ConnectThr->value();
    param.nFilterRadius = ui->spinBox_FilterRadius->value();
    param.nMinDefectArea = ui->spinBox_MinDefectArea->value();

    clearDefect();

    ImageRect pRoi;
    if (NULL != mDefectRectItem)
    {
        QRectF rect = mDefectRectItem->getOriRect();
        pRoi.xs = rect.left();
        pRoi.xe = rect.right();
        pRoi.ys = rect.top();
        pRoi.ye = rect.bottom();
    }

    std::thread([&] {

        DetectDefect(mADefectList, m_pImgDefect, imgW, imgH, &pRoi, &param);

        FunctionTransfer::runInMainThread([=]()
        {
            for(int i=0; i<mADefectList.size(); i++)
            {
                setRecognizeValue(i, mADefectList.at(i).feat);
            }

            closeLoading();

            update();
        });

    }).detach();
}

void MainWindow::delImg()
{
    if (!dmfile.IsValid())
        return ;

    mDelImgLock.lock();
    mBDelImging = false;
    mDelImgLock.unlock();

    DWORD dReturn = WaitForSingleObject(hEvent,INFINITE);

    if (WAIT_OBJECT_0 == dReturn)
    {
        ResetEvent(hEvent);

        mDelImgLock.lock();
        mBDelImging = true;
        mDelImgLock.unlock();
    }

    std::thread([=] {

        mImgProLock.lock();
        //删除
        if (nullptr != m_pImgPro)
        {
            delete []m_pImgPro;
            m_pImgPro = NULL;
        }
        mImgProLock.unlock();


        mDelImgLock.lock();
        if (!mBDelImging)
        {
            mDelImgLock.unlock();
            SetEvent(hEvent);
            return ;
        }
        mDelImgLock.unlock();

        if (mSrcImgWidth <= 0 || mSrcImgHeight <= 0)
            return ;

        mImgProLock.lock();
        //拷贝
        m_pImgPro = new unsigned short[mSrcImgWidth * mSrcImgHeight];
        memcpy(m_pImgPro, m_pSrcImg, mSrcImgWidth*mSrcImgHeight*sizeof(unsigned short));
        mImgProLock.unlock();


        mCurImgWidth  = mSrcImgWidth;
        mCurImgHeight = mSrcImgHeight;

        //镜像
        if (mBMirror)
        {
            mImgProLock.lock();
            Mirror(m_pImgPro, mCurImgWidth, mCurImgHeight);
            mImgProLock.unlock();
        }

        mDelImgLock.lock();
        if (!mBDelImging)
        {
            mDelImgLock.unlock();
            SetEvent(hEvent);
            return ;
        }
        mDelImgLock.unlock();

        //翻转
        if (mBFlip)
        {
            mImgProLock.lock();
            Flip(m_pImgPro, mCurImgWidth, mCurImgHeight);
            mImgProLock.unlock();
        }

        mDelImgLock.lock();
        if (!mBDelImging)
        {
            mDelImgLock.unlock();
            SetEvent(hEvent);
            return ;
        }
        mDelImgLock.unlock();

        //窗宽窗位
        if (mBWind)
        {
            if (mWinCentre>1 && mWinWidth>1)
            {
                mImgProLock.lock();
                WindowLevelTransform(m_pImgPro, mCurImgWidth, mCurImgHeight, mWinCentre, mWinWidth);
                mImgProLock.unlock();
            }
        }

        mDelImgLock.lock();
        if (!mBDelImging)
        {
            mDelImgLock.unlock();
            SetEvent(hEvent);
            return ;
        }
        mDelImgLock.unlock();

        //对比度
        if (mBContrast)
        {
            mImgProLock.lock();
            IPFuncMUSICA(m_pImgPro, mCurImgWidth, mCurImgHeight, mLevel, mPower);
            mImgProLock.unlock();

            //ContrastEnhancement(m_pImgPro, mCurImgWidth, mCurImgHeight, mContrast);
        }

        mDelImgLock.lock();
        if (!mBDelImging)
        {
            mDelImgLock.unlock();
            SetEvent(hEvent);
            return ;
        }
        mDelImgLock.unlock();

        //反相
        if (mBInvert)
        {
            mImgProLock.lock();
            Invert(m_pImgPro, mCurImgWidth, mCurImgHeight);
            mImgProLock.unlock();
        }

        mDelImgLock.lock();
        if (!mBDelImging)
        {
            mDelImgLock.unlock();
            SetEvent(hEvent);
            return ;
        }
        mDelImgLock.unlock();

        //resize
        reSize(mScale);

        mDelImgLock.lock();
        if (!mBDelImging)
        {
            mDelImgLock.unlock();
            SetEvent(hEvent);
            return ;
        }
        mDelImgLock.unlock();

        mImgProLock.lock();
        //旋转
        if (mNeedRotate == 90)
            Rotate90(m_pImgPro, mCurImgWidth, mCurImgHeight);
        else if (mNeedRotate == 180)
            Rotate180(m_pImgPro, mCurImgWidth, mCurImgHeight);
        else if (mNeedRotate == 270)
            Rotate270(m_pImgPro, mCurImgWidth, mCurImgHeight);
        mImgProLock.unlock();

        mDelImgLock.lock();
        if (!mBDelImging)
        {
            mDelImgLock.unlock();
            SetEvent(hEvent);
            return ;
        }
        mDelImgLock.unlock();

        mImgProLock.lock();
        shortImgToImage(m_pImgPro, mCurImgWidth, mCurImgHeight, mPaintImg);
        mImgProLock.unlock();

        mDelImgLock.lock();
        if (!mBDelImging)
        {
            mDelImgLock.unlock();
            SetEvent(hEvent);
            return ;
        }
        mDelImgLock.unlock();


        FunctionTransfer::runInMainThread([=]()
        {
            //显示
            showImg(m_pImgPro, mCurImgWidth, mCurImgHeight);
        });

        SetEvent(hEvent);

    }).detach();

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

    mColorWdg->hide();

//    slot_btnColorClick(m_measureSetting.measureObjectSet.lineColor.name(), false);
//    ui->comboBox_linebold->setCurrentText(QString("%1").arg(m_measureSetting.measureObjectSet.lineWidth));

//    setPainterStyle();
}


void MainWindow::updateCursor()
{
    if (mBMeasureOpt)
    {
        if (m_eCurAction == HANDACTION)
        {
            if (mPaintRect.width() > ui->widget_img_pre->width() ||
                mPaintRect.height() > ui->widget_img_pre->height())
            {
                setCursor(Qt::OpenHandCursor);
            }
            else {
                setCursor(Qt::ArrowCursor);
            }
        }
        else {
            setCursor(Qt::ArrowCursor);
        }
    }
    else
    {
        if (mPaintRect.width() > ui->widget_img_pre->width() ||
            mPaintRect.height() > ui->widget_img_pre->height())
        {
            if (m_eCurAction == HANDACTION)
                setCursor(Qt::OpenHandCursor);
            else
                setCursor(Qt::ArrowCursor);
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
    else if (obj == mColorWdg)
    {
        if (e->type() == QEvent::FocusOut)
        {
            if (mColorWdg->hasFocus())
            {
            }
            else
            {
                mColorWdg->hide();
            }
        }
        else if (e->type() == QEvent::Enter)
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

                if (mPaintRect.width() <= nWidth && mPaintRect.height() <= nHeight)
                {
                    mPaintRectReal.setRect((nWidth-mPaintRect.width())/2,
                                         (nHeight-mPaintRect.height())/2,
                                         mPaintRect.width(),
                                         mPaintRect.height());

                    mSourceRect = QRect(0, 0, mPaintRect.width(), mPaintRect.height());

                    p.drawImage(mPaintRectReal, mPaintImg);
                }
                else if (mPaintRect.width() <= nWidth && mPaintRect.height() > nHeight)
                {
                    mPaintRectReal.setRect((nWidth-mPaintRect.width())/2, 0,
                                         mPaintRect.width(),
                                         nHeight);

                    p.drawImage(mPaintRectReal, mPaintImg, mSourceRect);
                }
                else if (mPaintRect.width() > nWidth && mPaintRect.height() <= nHeight)
                {
                    mPaintRectReal.setRect(0, (nHeight-mPaintRect.height())/2,
                                         nWidth,
                                         mPaintRect.height());
                    p.drawImage(mPaintRectReal, mPaintImg, mSourceRect);
                }
                else
                {
                    mPaintRectReal.setRect(0, 0, nWidth, nHeight);
                    p.drawImage(mPaintRectReal, mPaintImg, mSourceRect);
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
                                QPoint pt(aPt.at(j).x * mScale-mSourceRect.x() + mPaintRectReal.x(),
                                          aPt.at(j).y * mScale - mSourceRect.y() + mPaintRectReal.y());

                                if (mPaintRectReal.contains(pt))
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


                            if (mCurDefectIndex == i)
                            {
                                QPen pen;
                                pen.setWidth(3);
                                pen.setColor(QColor("#0000ff"));
                                p.setPen(pen);
                            }
                            else
                                p.setPen(QColor("#ff0000"));

                            p.drawLine(centerPt.x() - mSourceRect.x() -length + mPaintRectReal.x(),
                                       centerPt.y() - mSourceRect.y() + mPaintRectReal.y(),
                                       centerPt.x() - mSourceRect.x() + length + mPaintRectReal.x(),
                                       centerPt.y() - mSourceRect.y() + mPaintRectReal.y());

                            p.drawLine(centerPt.x() - mSourceRect.x() + mPaintRectReal.x(),
                                       centerPt.y() - mSourceRect.y() - length + mPaintRectReal.y(),
                                       centerPt.x() - mSourceRect.x() + mPaintRectReal.x(),
                                       centerPt.y() - mSourceRect.y() + length + mPaintRectReal.y());
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
                        rectItemTmp->updateGeometry(getCurOffset(),
                                                        mCurImgWidth,
                                                        mCurImgHeight, mScale,
                                                        mNeedRotate, mBFlip, mBMirror);

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
//                                    labelStr = QString("A:%1%2").arg(QString::number(value, 'f', 2))
//                                                               .arg(m_measureSetting.getUnitStr());
//                                else
//                                    labelStr = QString("A:%1").arg(QString::number(value, 'f', 2));
                                labelStr = QString("A:%1").arg(QString::number(value, 'f', 2));

                                p.drawText(rectTmp.center(), labelStr);

                                break;
                            case PERIMETERLABEL:
                                value = rectItemTmp->getCircumferenceUnit();

//                                if (m_measureSetting.generalSet.bDimension)
//                                    labelStr = QString("P:%1%2").arg(QString::number(value, 'f', 2))
//                                                               .arg(m_measureSetting.getUnitStr());
//                                else
//                                    labelStr = QString("P:%1").arg(QString::number(value, 'f', 2));

                                                                    labelStr = QString("P:%1").arg(QString::number(value, 'f', 2));

                                p.drawText(rectTmp.center(), labelStr);
                                break;
                            case WIDTHANDHEIGHTLABEL:
                            {
                                value = rectItemTmp->getOriWidthUnit();

//                                if (m_measureSetting.generalSet.bDimension)
//                                    labelStr = QString("W:%1%2").arg(QString::number(value, 'f', 2))
//                                                               .arg(m_measureSetting.getUnitStr());
//                                else
//                                    labelStr = QString("W:%1").arg(QString::number(value, 'f', 2));
                                labelStr = QString("W:%1").arg(QString::number(value, 'f', 2));

                                p.drawText(rectTmp.center(), labelStr);

                                //--
                                value = rectItemTmp->getOriHeightUnit();

//                                if (m_measureSetting.generalSet.bDimension)
//                                    labelStr = QString("H:%1%2").arg(QString::number(value, 'f', 2))
//                                                               .arg(m_measureSetting.getUnitStr());
//                                else
//                                    labelStr = QString("H:%1").arg(QString::number(value, 'f', 2));

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
                    else if (EllipseItem *ellipseItemTmp = dynamic_cast<EllipseItem *> (geometryItemTmp))
                    {
                        ellipseItemTmp->updateGeometry(getCurOffset(),
                                                         mCurImgWidth,
                                                         mCurImgHeight, mScale,
                                                         mNeedRotate, mBFlip, mBMirror);

                        QRectF rectTmp = ellipseItemTmp->getRect();
                        bool selectedFlag = false;

                        p.save();

                        if (ellipseItemTmp->getItemStatus() == SELECTEDMOVE
                                 || ellipseItemTmp->getItemStatus() == SELECTEDNOMOVE
                                 || ellipseItemTmp->getItemStatus() == SELECTEDDRAG)
                        {
                            selectedFlag = true;
                        }

                        pen.setStyle(ellipseItemTmp->getPenStyle());
                        pen.setColor(ellipseItemTmp->getColor());
                        pen.setWidth(ellipseItemTmp->getLineWidht());

                        if (ellipseItemTmp->getFillStatus())
                            p.setBrush(QColor(ellipseItemTmp->getFillColor()));

                        p.setPen(pen);

                        p.drawEllipse(rectTmp);

                        p.restore();

                        p.save();
                        if (selectedFlag)
                        {
                            //线边框
                            pen.setStyle(Qt::DashLine);
                            pen.setColor(Qt::green);
                            pen.setWidth(2);

                            QRectF rectF = ellipseItemTmp->getBoundingRectF();
                            p.setPen(pen);
                            p.drawRect(rectF);

                            //拖拽点
                            pen.setStyle(Qt::SolidLine);
                            p.setPen(pen);

                            QList<QRect> rectList = ellipseItemTmp->getDragItemData().getDragItemRects();

                            for (int i=0; i<rectList.size(); i++)
                            {
                                p.drawEllipse(rectList.at(i));
                            }
                        }
                        else
                        {
//                            pen.setColor(m_measureSetting.generalSet.color);
                            QFont font;
                            //font.setPixelSize(m_measureSetting.generalSet.fontSize);
//                            font.setPointSize(m_measureSetting.generalSet.fontSize);

                            p.setPen(pen);
                            p.setFont(font);

                            QString labelStr;
                            qreal value;

                            switch (ellipseItemTmp->getCurLabelType()) {
                            case NAMELABEL:

                                p.drawText(rectTmp.center(), ellipseItemTmp->getItemName());

                                break;
                            case AREALABEL:
                                value = ellipseItemTmp->getAreaUnit();

//                                if (m_measureSetting.generalSet.bDimension)
//                                    labelStr = QString("A:%1%2").arg(QString::number(value, 'f', m_measureSetting.generalSet.resultKeep))
//                                                               .arg(m_measureSetting.getUnitStr());
//                                else
//                                    labelStr = QString("A:%1").arg(QString::number(value, 'f', m_measureSetting.generalSet.resultKeep));

                                labelStr = QString("A:%1").arg(QString::number(value, 'f', 2));
                                p.drawText(rectTmp.center(), labelStr);

                                break;
                            case PERIMETERLABEL:
                                value = ellipseItemTmp->getCircumferenceUnit();

//                                if (m_measureSetting.generalSet.bDimension)
//                                    labelStr = QString("P:%1%2").arg(QString::number(value, 'f', m_measureSetting.generalSet.resultKeep))
//                                                               .arg(m_measureSetting.getUnitStr());
//                                else
//                                    labelStr = QString("P:%1").arg(QString::number(value, 'f', m_measureSetting.generalSet.resultKeep));

                                labelStr = QString("P:%1").arg(QString::number(value, 'f', 2));

                                p.drawText(rectTmp.center(), labelStr);
                                break;
                            case MAJORANDMINORAXISLABEL:
                            {
                                value = ellipseItemTmp->getSemiMajorAxisUnit();

//                                if (m_measureSetting.generalSet.bDimension)
//                                    labelStr = QString("Maj:%1%2").arg(QString::number(value, 'f', m_measureSetting.generalSet.resultKeep))
//                                                               .arg(m_measureSetting.getUnitStr());
//                                else
//                                    labelStr = QString("Maj:%1").arg(QString::number(value, 'f', m_measureSetting.generalSet.resultKeep));

            //                    p.drawText(rectTmp.center().x(),rectTmp.top()-2, labelStr);

                                labelStr = QString("Maj:%1").arg(QString::number(value, 'f', 2));
                                p.drawText(rectTmp.center(), labelStr);

                                //--
                                value = ellipseItemTmp->getSemiMinorAxisUnit();

//                                if (m_measureSetting.generalSet.bDimension)
//                                    labelStr = QString("Min:%1%2").arg(QString::number(value, 'f', m_measureSetting.generalSet.resultKeep))
//                                                               .arg(m_measureSetting.getUnitStr());
//                                else
//                                    labelStr = QString("Min:%1").arg(QString::number(value, 'f', m_measureSetting.generalSet.resultKeep));

                                labelStr = QString("Min:%1").arg(QString::number(value, 'f', 2));

            //                    p.drawText(rectTmp.right() - 30, rectTmp.center().y(), labelStr);
                                p.drawText(rectTmp.center().x(), rectTmp.center().y()+12+2, labelStr);
                            }
                                break;
                            default:
                                break;
                            }
                        }

                        p.restore();
                    }
                    else if (LineItem *lineItemTmp = dynamic_cast<LineItem *> (geometryItemTmp))
                    {
                        if (NULL == lineItemTmp)
                            continue;

                        lineItemTmp->updateGeometry(getCurOffset(),
                                                    mCurImgWidth,
                                                    mCurImgHeight, mScale,
                                                    mNeedRotate, mBFlip, mBMirror);

                        QLineF line = lineItemTmp->getLine();
                        bool selectedFlag = false;

                        p.save();

                        pen.setStyle(lineItemTmp->getPenStyle());
                        pen.setColor(lineItemTmp->getColor());
                        pen.setWidth(lineItemTmp->getLineWidht());

                        if (lineItemTmp->getItemStatus() == SELECTEDMOVE
                             || lineItemTmp->getItemStatus() == SELECTEDNOMOVE
                             || lineItemTmp->getItemStatus() == SELECTEDDRAG)
                        {
                            selectedFlag = true;
                        }

                        p.setPen(pen);
                        p.drawLine(line);

                        p.restore();

                        p.save();
                        if (selectedFlag)
                        {
                            //线边框
                            pen.setStyle(Qt::DashLine);
                            pen.setColor(Qt::green);
                            pen.setWidth(2);

                            QPolygonF polygonf = lineItemTmp->getBoundingPolygonf();
                            p.setPen(pen);
                            p.drawPolygon(polygonf);

                            //拖拽点
                            pen.setStyle(Qt::SolidLine);
                            p.setPen(pen);

                            QList<QRect> rectList = lineItemTmp->getDragItemData().getDragItemRects();

                            for (int i=0; i<rectList.size(); i++)
                            {
                                p.drawEllipse(rectList.at(i));
                            }
                        }
                        else
                        {
//                            pen.setColor(m_measureSetting.generalSet.color);
                            QFont font;
                            //font.setPixelSize(m_measureSetting.generalSet.fontSize);
//                            font.setPointSize(m_measureSetting.generalSet.fontSize);

                            p.setPen(pen);
                            p.setFont(font);

                            QString labelStr;
                            qreal value;

                            switch (lineItemTmp->getCurLabelType()) {
                            case NAMELABEL:

                                p.drawText(line.p1().x() + (line.p2().x()-line.p1().x())/2,
                                                  line.p1().y() + (line.p2().y()-line.p1().y())/2,
                                                  lineItemTmp->getItemName());

                                break;
                            case LENGTHLABEL:
                                value = lineItemTmp->getLengthUnit();

//                                if (m_measureSetting.generalSet.bDimension)
//                                    labelStr = QString("L:%1%2").arg(QString::number(value, 'f', m_measureSetting.generalSet.resultKeep))
//                                                               .arg(m_measureSetting.getUnitStr());
//                                else
//                                    labelStr = QString("L:%1").arg(QString::number(value, 'f', m_measureSetting.generalSet.resultKeep));

                                labelStr = QString("L:%1").arg(QString::number(value, 'f', 2));

                                p.drawText(line.p1().x() + (line.p2().x()-line.p1().x())/2,
                                                  line.p1().y() + (line.p2().y()-line.p1().y())/2,
                                                  labelStr);

                                break;
                            default:
                                break;
                            }
                        }

                        p.restore();
                    }

                }


                //灰色计算
                if (!mGrayLine.isNull())
                    p.drawLine(mGrayLine);

                if (!mGrayLine.isNull())
                {
                    if (NULL != mGreyRectItem)
                    {
                        mGreyRectItem->updateGeometry(getCurOffset(),
                                                      mCurImgWidth,
                                                      mCurImgHeight, mScale,
                                                      mNeedRotate, mBFlip, mBMirror);

                        QRectF rectTmp = mGreyRectItem->getRect();

                        p.drawLine(rectTmp.topLeft(), rectTmp.bottomRight());
                        p.drawLine(rectTmp.bottomLeft(), rectTmp.topRight());
                    }
                }

                //
                if (NULL != mDefectRectItem)
                {
                    mDefectRectItem->updateGeometry(getCurOffset(),
                                                  mCurImgWidth,
                                                  mCurImgHeight, mScale,
                                                  mNeedRotate, mBFlip, mBMirror);

                    QRectF rectTmp = mDefectRectItem->getRect();

                    p.save();
                    p.setPen(mDefectRectItem->getColor());
                    p.drawRect(rectTmp);

                    float value = mDefectRectItem->getOriWidth();
                    QString labelStr = QString("W:%1").arg(QString::number(value, 'f', 2));
                    p.drawText(rectTmp.topRight().x()+2, rectTmp.topRight().y(), labelStr);

                    //--
                    value = mDefectRectItem->getOriHeight();
                    labelStr = QString("H:%1").arg(QString::number(value, 'f', 2));
                    p.drawText(rectTmp.topRight().x()+2, rectTmp.topRight().y()+12+2, labelStr);

                    //--Intensity--
                    value = mDefectRectItem->getIntensity();
                    labelStr = QString("I:%1").arg(QString::number(value, 'f', 2));
                    p.drawText(rectTmp.topRight().x()+2, rectTmp.topRight().y()+26+2, labelStr);

                    //--SNR--
                    double dMean, dStd, dSNR;
                    mDefectRectItem->getSNR(dMean, dStd, dSNR);

                    labelStr = QString("Mean:%1").arg(QString::number(dMean, 'f', 2));
                    p.drawText(rectTmp.topRight().x()+2, rectTmp.topRight().y()+40+2, labelStr);

                    labelStr = QString("Std:%1").arg(QString::number(dStd, 'f', 2));
                    p.drawText(rectTmp.topRight().x()+2, rectTmp.topRight().y()+54+2, labelStr);

                    labelStr = QString("SNR:%1").arg(QString::number(dSNR, 'f', 2));
                    p.drawText(rectTmp.topRight().x()+2, rectTmp.topRight().y()+68+2, labelStr);

                    p.restore();
                }

                //
                if (NULL != mRecheckRectItem)
                {
                    mRecheckRectItem->updateGeometry(getCurOffset(),
                                                  mCurImgWidth,
                                                  mCurImgHeight, mScale,
                                                  mNeedRotate, mBFlip, mBMirror);

                    QRectF rectTmp = mRecheckRectItem->getRect();

                    p.save();
                    p.setPen(mRecheckRectItem->getColor());
                    p.drawRect(rectTmp);

                    float value = mRecheckRectItem->getOriWidth();

                    QString labelStr = QString("W:%1").arg(QString::number(value, 'f', 2));

                    p.drawText(rectTmp.topRight().x()+2, rectTmp.topRight().y(), labelStr);

                    //--
                    value = mRecheckRectItem->getOriHeight();

                    labelStr = QString("H:%1").arg(QString::number(value, 'f', 2));

                    p.drawText(rectTmp.topRight().x()+2, rectTmp.topRight().y()+12+2, labelStr);


                    //--Intensity--
                    value = mRecheckRectItem->getIntensity();
                    labelStr = QString("I:%1").arg(QString::number(value, 'f', 2));
                    p.drawText(rectTmp.topRight().x()+2, rectTmp.topRight().y()+26+2, labelStr);

                    //--SNR--
                    double dMean, dStd, dSNR;
                    mRecheckRectItem->getSNR(dMean, dStd, dSNR);

                    labelStr = QString("Mean:%1").arg(QString::number(dMean, 'f', 2));
                    p.drawText(rectTmp.topRight().x()+2, rectTmp.topRight().y()+40+2, labelStr);

                    labelStr = QString("Std:%1").arg(QString::number(dStd, 'f', 2));
                    p.drawText(rectTmp.topRight().x()+2, rectTmp.topRight().y()+54+2, labelStr);

                    labelStr = QString("SNR:%1").arg(QString::number(dSNR, 'f', 2));
                    p.drawText(rectTmp.topRight().x()+2, rectTmp.topRight().y()+68+2, labelStr);

                    p.restore();
                }

                //临时几何图像
                switch (m_eCurAction) {
                case RECTACTION:
                case AOIACTION:
                case AOIRECHECKACTION:
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
                        p.drawEllipse(m_rectTmp);
                    }
                }
                    break;
                case LINEACTION:
                {
                    if (m_eDrawStatus == BEGINDRAW)
                    {
                        p.drawLine(m_lineTmp);
                    }
                }
                    break;
                case GREAYACTION:
                {
                    if (m_eDrawStatus == BEGINDRAW)
                    {
                        p.drawLine(mGrayLineTmp);
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

            if (mBMeasureOpt || m_eCurAction==GREAYACTION || m_eCurAction==AOIACTION || m_eCurAction == AOIRECHECKACTION)
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
                    case GREAYACTION:
                    case AOIACTION:
                    case AOIRECHECKACTION:
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
            else if (!mBMeasureOpt)
            {
                m_bIsPress = true;
                m_PressPt = pt;

                if (mPaintRect.width() > ui->widget_img_pre->width() ||
                    mPaintRect.height() > ui->widget_img_pre->height())
                {
                    setCursor(Qt::ClosedHandCursor);
                }
            }

        }
        else if (e->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);
            QPoint pt = mouseEvent->pos();

            if (mBMeasureOpt || m_eCurAction==GREAYACTION || m_eCurAction==AOIACTION || m_eCurAction == AOIRECHECKACTION)
            {
                if (mouseEvent->button() == Qt::LeftButton)
                {
                    switch (m_eCurAction) {
                    case RECTACTION:
                    {
                        if (m_rectTmp.isValid() && m_eDrawStatus == BEGINDRAW && m_rectTmp.width() > 2 && m_rectTmp.height() > 2)
                        {
                            m_geometryItemBase = new RectItem(m_rectTmp);
                            m_geometryItemBase->setColor(mColorWdg->getCurColor());
                            m_geometryItemBase->setLineWidth(ui->comboBox_linebold->currentText().toInt());
//                            m_geometryItemBase->setPenStyle(m_measureSetting.measureObjectSet.penStyle);
//                            m_geometryItemBase->setCurLabelType(m_measureSetting.measureObjectSet.rectLabelType);

//                            m_geometryItemBase->setSpatialResolution(g_scanner.getSpatialResolution());
//                            m_geometryItemBase->setUnit(m_measureSetting.lengthUnit);

                            m_geometryItemBase->calcOriGeometry(getCurOffset(),
                                                                mCurImgWidth,
                                                                mCurImgHeight,
                                                                mScale,
                                                                mNeedRotate, mBFlip, mBMirror);

                            RectItem *rectItem = (RectItem *)m_geometryItemBase;
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

                            QRectF rectTmp = rectItem->getOriRect();

                            //--Intensity--
                            if (nullptr != m_pSrcImg)
                            {
                                QPoint topLeft = rectTmp.topLeft().toPoint();
                                QPoint bottomRight = rectTmp.bottomRight().toPoint();

                                unsigned short intensity = 0;
                                GetIntensity(intensity, m_pSrcImg, mSrcImgWidth, mSrcImgHeight,
                                    topLeft.x(), topLeft.y(), bottomRight.x(), bottomRight.y());

                                rectItem->setIntensity(intensity);
                            }

                            //--SNR--
                            if (nullptr != m_pSrcImg)
                            {
                                QPoint topLeft = rectTmp.topLeft().toPoint();
                                QPoint bottomRight = rectTmp.bottomRight().toPoint();

                                double dMean, dStd, dSNR;
                                GetSNR(dMean, dStd, dSNR, m_pSrcImg, mSrcImgWidth, mSrcImgHeight,
                                    topLeft.x(), topLeft.y(), bottomRight.x(), bottomRight.y());

                                rectItem->setSNR(dMean, dStd, dSNR);
                            }

                            updateMeasureTable();
                        }

                        m_rectTmp = QRectF();

                        m_eDrawStatus = ENDDRAW;
                    }
                        break;
                    case AOIACTION:
                    {
                        if (m_rectTmp.isValid() && m_eDrawStatus == BEGINDRAW && m_rectTmp.width() > 2 && m_rectTmp.height() > 2)
                        {
                            if (NULL != mDefectRectItem)
                            {
                                delete mDefectRectItem;
                                mDefectRectItem = NULL;
                            }

                            mDefectRectItem = new RectItem(m_rectTmp);
                            mDefectRectItem->setColor(QColor("#0000ff"));
                            mDefectRectItem->setLineWidth(ui->comboBox_linebold->currentText().toInt());
//                            m_geometryItemBase->setPenStyle(m_measureSetting.measureObjectSet.penStyle);
//                            m_geometryItemBase->setCurLabelType(m_measureSetting.measureObjectSet.rectLabelType);

//                            m_geometryItemBase->setSpatialResolution(g_scanner.getSpatialResolution());
//                            m_geometryItemBase->setUnit(m_measureSetting.lengthUnit);

                            mDefectRectItem->calcOriGeometry(getCurOffset(),
                                                                mCurImgWidth,
                                                                mCurImgHeight,
                                                                mScale,
                                                                mNeedRotate, mBFlip, mBMirror);


                            //置为选中态
                            mDefectRectItem->setItemStatus(SELECTEDNOMOVE);

                            QRectF rectTmp = mDefectRectItem->getOriRect();

                            //--Intensity--
                            if (nullptr != m_pSrcImg)
                            {
                                QPoint topLeft = rectTmp.topLeft().toPoint();
                                QPoint bottomRight = rectTmp.bottomRight().toPoint();

                                unsigned short intensity = 0;
                                GetIntensity(intensity, m_pSrcImg, mSrcImgWidth, mSrcImgHeight,
                                    topLeft.x(), topLeft.y(), bottomRight.x(), bottomRight.y());

                                mDefectRectItem->setIntensity(intensity);
                            }

                            //--SNR--
                            if (nullptr != m_pSrcImg)
                            {
                                QPoint topLeft = rectTmp.topLeft().toPoint();
                                QPoint bottomRight = rectTmp.bottomRight().toPoint();

                                double dMean, dStd, dSNR;
                                GetSNR(dMean, dStd, dSNR, m_pSrcImg, mSrcImgWidth, mSrcImgHeight,
                                    topLeft.x(), topLeft.y(), bottomRight.x(), bottomRight.y());

                                mDefectRectItem->setSNR(dMean, dStd, dSNR);
                            }

                        }

                        m_rectTmp = QRectF();

                        m_eDrawStatus = ENDDRAW;
                    }
                        break;
                    case AOIRECHECKACTION:
                    {
                        if (m_rectTmp.isValid() && m_eDrawStatus == BEGINDRAW && m_rectTmp.width() > 2 && m_rectTmp.height() > 2)
                        {
                            if (NULL != mRecheckRectItem)
                            {
                                delete mRecheckRectItem;
                                mRecheckRectItem = NULL;
                            }

                            mRecheckRectItem = new RectItem(m_rectTmp);
                            mRecheckRectItem->setColor(QColor("#00ff00"));
                            mRecheckRectItem->setLineWidth(2);

                            mRecheckRectItem->calcOriGeometry(getCurOffset(),
                                                                mCurImgWidth,
                                                                mCurImgHeight,
                                                                mScale,
                                                                mNeedRotate, mBFlip, mBMirror);


                            //置为选中态
                            mRecheckRectItem->setItemStatus(SELECTEDNOMOVE);

                            QRectF rectTmp = mRecheckRectItem->getOriRect();

                            //--Intensity--
                            if (nullptr != m_pSrcImg)
                            {
                                QPoint topLeft = rectTmp.topLeft().toPoint();
                                QPoint bottomRight = rectTmp.bottomRight().toPoint();

                                unsigned short intensity = 0;
                                GetIntensity(intensity, m_pSrcImg, mSrcImgWidth, mSrcImgHeight,
                                    topLeft.x(), topLeft.y(), bottomRight.x(), bottomRight.y());

                                mRecheckRectItem->setIntensity(intensity);
                            }

                            //--SNR--
                            if (nullptr != m_pSrcImg)
                            {
                                QPoint topLeft = rectTmp.topLeft().toPoint();
                                QPoint bottomRight = rectTmp.bottomRight().toPoint();

                                double dMean, dStd, dSNR;
                                GetSNR(dMean, dStd, dSNR, m_pSrcImg, mSrcImgWidth, mSrcImgHeight,
                                    topLeft.x(), topLeft.y(), bottomRight.x(), bottomRight.y());

                                mRecheckRectItem->setSNR(dMean, dStd, dSNR);
                            }
                        }

                        m_rectTmp = QRectF();

                        m_eDrawStatus = ENDDRAW;
                    }
                        break;
                    case ELLIPSEACTION:
                    {
                        if (m_rectTmp.isValid() && m_eDrawStatus == BEGINDRAW && m_rectTmp.width() > 2 && m_rectTmp.height() > 2)
                        {
                            m_geometryItemBase = new EllipseItem(m_rectTmp);
                            m_geometryItemBase->setColor(mColorWdg->getCurColor());
                            m_geometryItemBase->setLineWidth(ui->comboBox_linebold->currentText().toInt());
//                            m_geometryItemBase->setPenStyle(m_measureSetting.measureObjectSet.penStyle);
//                            m_geometryItemBase->setCurLabelType(m_measureSetting.measureObjectSet.ellipseLabelType);

//                            m_geometryItemBase->setSpatialResolution(g_scanner.getSpatialResolution());
//                            m_geometryItemBase->setUnit(m_measureSetting.lengthUnit);

                            m_geometryItemBase->calcOriGeometry(getCurOffset(),
                                                                mCurImgWidth,
                                                                mCurImgHeight,
                                                                mScale,
                                                                mNeedRotate, mBFlip, mBMirror);

                            EllipseItem *ellipseItem = (EllipseItem *)m_geometryItemBase;
//                            ellipseItem->setFillStatus(m_measureSetting.measureObjectSet.bFill);
//                            ellipseItem->setFillColor(m_measureSetting.measureObjectSet.fillColor);

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

                            QRectF rectTmp = ellipseItem->getOriRect();

                            //--Intensity--
                            if (nullptr != m_pSrcImg)
                            {
                                QPoint topLeft = rectTmp.topLeft().toPoint();
                                QPoint bottomRight = rectTmp.bottomRight().toPoint();

                                unsigned short intensity = 0;
                                GetIntensity(intensity, m_pSrcImg, mSrcImgWidth, mSrcImgHeight,
                                    topLeft.x(), topLeft.y(), bottomRight.x(), bottomRight.y());

                                ellipseItem->setIntensity(intensity);
                            }

                            //--SNR--
                            if (nullptr != m_pSrcImg)
                            {
                                QPoint topLeft = rectTmp.topLeft().toPoint();
                                QPoint bottomRight = rectTmp.bottomRight().toPoint();

                                double dMean, dStd, dSNR;
                                GetSNR(dMean, dStd, dSNR, m_pSrcImg, mSrcImgWidth, mSrcImgHeight,
                                    topLeft.x(), topLeft.y(), bottomRight.x(), bottomRight.y());

                                ellipseItem->setSNR(dMean, dStd, dSNR);
                            }

                            updateMeasureTable();
                        }

                        m_rectTmp = QRectF();

                        m_eDrawStatus = ENDDRAW;
                    }
                        break;
                    case ARROWACTION:
                    {
                        mouseReleaseArrowAction();
                    }
                        break;
                    case LINEACTION:
                    {
                        if (!m_lineTmp.isNull() && m_eDrawStatus == BEGINDRAW)
                        {
                            m_geometryItemBase = new LineItem(m_lineTmp);
                            m_geometryItemBase->setColor(mColorWdg->getCurColor());
                            m_geometryItemBase->setLineWidth(ui->comboBox_linebold->currentText().toInt());
//                            m_geometryItemBase->setPenStyle(m_measureSetting.measureObjectSet.penStyle);
//                            m_geometryItemBase->setCurLabelType(m_measureSetting.measureObjectSet.lineLabelType);

//                            m_geometryItemBase->setSpatialResolution(g_scanner.getSpatialResolution());
//                            m_geometryItemBase->setUnit(m_measureSetting.lengthUnit);

                            m_geometryItemBase->calcOriGeometry(getCurOffset(),
                                                                mCurImgWidth,
                                                                mCurImgHeight,
                                                                mScale,
                                                                mNeedRotate, mBFlip, mBMirror);

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

                            updateMeasureTable();
                        }

                        m_lineTmp = QLine();

                        m_eDrawStatus = ENDDRAW;
                    }
                        break;
                    case GREAYACTION:
                    {
                        if (!mGrayLineTmp.isNull() && m_eDrawStatus == BEGINDRAW)
                        {
                            mGrayLine = mGrayLineTmp;
                        }

                        mGrayLineTmp = QLine();

                        m_eDrawStatus = ENDDRAW;

                        calcIntensityCurve(mGrayLine.p1(), mGrayLine.p2());
                    }
                        break;
                    case HANDACTION:
                    {
                        setCursor(Qt::OpenHandCursor);
                        m_bIsPress = false;
                    }
                        break;
                    default:
                        break;
                    }

                    update();
                }
            }
            else if (!mBMeasureOpt)
            {
                if (mPaintRect.width() > ui->widget_img_pre->width() ||
                    mPaintRect.height() > ui->widget_img_pre->height())
                {
                    setCursor(Qt::OpenHandCursor);
                    m_bIsPress = false;

                    update();
                }
            }

       }
       else if (e->type() == QEvent::MouseMove)
       {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);
            QPoint pt = mouseEvent->pos();

			if (mPaintRectReal.contains(pt))
			{
				//灰度
				getIntensity(pt);
			}
			else
			{
				ui->label_X_Value->clear();
				ui->label_Y_Value->clear();
				ui->label_intensity->clear();
			}

            if (mBMeasureOpt || m_eCurAction==GREAYACTION || m_eCurAction==AOIACTION || m_eCurAction==AOIRECHECKACTION)
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
                case AOIACTION:
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
                case AOIRECHECKACTION:
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
                case LINEACTION:
                {
                    if (m_eDrawStatus == BEGINDRAW)
                    {
                        QPoint p1 = m_beginPosPt;
                        QPoint p2 = imgPt;

                        m_lineTmp = QLine(p1, p2);

                        update();
                    }
                }
                    break;
                case GREAYACTION:
                {
                    if (m_eDrawStatus == BEGINDRAW)
                    {
                        QPoint p1 = m_beginPosPt;
                        QPoint p2 = imgPt;

                        mGrayLineTmp = QLine(p1, p2);

                        update();
                    }
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
            else if (!mBMeasureOpt)
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

            return true;
        }
        else if (e->type() == QEvent::Enter)
        {
            updateCursor();

            return true;
        }
        else if (e->type() == QEvent::Leave)
        {
            setCursor(Qt::ArrowCursor);
            return true;
        }
        else if (e->type() == QEvent::Resize)
        {
            if (dmfile.IsValid())
                delImg();


//            qDebug() << __FUNCTION__  << "resize.";
            return true;
        }
    }
    else if(obj == ui->widget_tool)
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


void MainWindow::doPreOperator(ItemOperator *itemOperator)
{
    GeometryItemBase *geometryItemTmp = itemOperator->geometryItem;

    if (NULL == geometryItemTmp)
        return;

    if (RectItem *rectItemTmp = dynamic_cast<RectItem *> (geometryItemTmp))
    {
        QRectF rectTmp;

        rectItemTmp->setItemStatus(NORMOAL);
        switch (itemOperator->type) {
        case ADDOPT:
        {
            m_geometryItemList.removeOne(rectItemTmp);
        }
            break;
        case MOVEOPT:
        {
            QPointF pt = itemOperator->originalPt;
            rectTmp = QRectF(pt.x(),
                            pt.y(),
                            rectItemTmp->getOriWidth(),
                            rectItemTmp->getOriHeight());

            rectItemTmp->setRect(rectTmp);

            rectItemTmp->calcOriGeometry(QPoint(0, 0),
                                         mSrcImgWidth,
                                         mSrcImgHeight,
                                         1,
                                         0, false, false);
        }
            break;
        case DELETEOPT:
            m_geometryItemList.push_back(rectItemTmp);
            break;
        case CHANGEOPT:
            rectTmp = itemOperator->originalRect;
            rectItemTmp->setRect(rectTmp);

            rectItemTmp->calcOriGeometry(QPoint(0, 0),
                                         mCurImgWidth,
                                         mCurImgHeight,
                                         1,
                                         mNeedRotate, mBFlip, mBMirror);
            break;
        case COLORCHAGNEOPT:
        {
            rectItemTmp->setColor(itemOperator->origiColor);
        }
            break;
        case LINEWIDTHCHANGEOPT:
        {
            rectItemTmp->setLineWidth(itemOperator->origiLineWidth);
        }
            break;
        case PENSTYLEOPT:
        {
            rectItemTmp->setPenStyle(itemOperator->oriPenStyle);
        }
            break;
        case FILLCHANGEOPT:
        {
            rectItemTmp->setFillStatus(itemOperator->oriBFill);
        }
            break;
        case FILLCOLORCHANGEOPT:
        {
            rectItemTmp->setFillColor(itemOperator->oriFillColor);
        }
            break;
        default:
            break;
        }
    }
    else if (TextItem *textItemTmp = dynamic_cast<TextItem *> (geometryItemTmp))
    {

    }
    else if (EllipseItem *ellipseItemTmp = dynamic_cast<EllipseItem *> (geometryItemTmp))
    {
//        QRectF rectTmp;

//        ellipseItemTmp->setItemStatus(NORMOAL);
//        switch (itemOperator->type) {
//        case ADDOPT:
//        {
//            m_geometryItemList.removeOne(ellipseItemTmp);
//        }
//            break;
//        case MOVEOPT:
//        {
//            QPointF pt = itemOperator->originalPt;
//            rectTmp = QRectF(pt.x(),
//                            pt.y(),
//                            ellipseItemTmp->getOriWidth(),
//                            ellipseItemTmp->getOriHeight());

//            ellipseItemTmp->setRect(rectTmp);

//            ellipseItemTmp->calcOriGeometry(QPoint(0, 0),
//                                            m_img.width(),m_img.height(),
//                                            1, 0, false, false);
//        }
//            break;
//        case DELETEOPT:
//            m_geometryItemList.push_back(ellipseItemTmp);
//            break;
//        case CHANGEOPT:
//            rectTmp = itemOperator->originalRect;
//            ellipseItemTmp->setRect(rectTmp);
//            ellipseItemTmp->calcOriGeometry(QPoint(0, 0),
//                                            m_img.width(),m_img.height(),
//                                            1, 0, false, false);
//            break;
//        case COLORCHAGNEOPT:
//        {
//            ellipseItemTmp->setColor(itemOperator->origiColor);
//        }
//            break;
//        case LINEWIDTHCHANGEOPT:
//        {
//            ellipseItemTmp->setLineWidth(itemOperator->origiLineWidth);
//        }
//            break;
//        case PENSTYLEOPT:
//        {
//            ellipseItemTmp->setPenStyle(itemOperator->oriPenStyle);
//        }
//            break;
//        case FILLCHANGEOPT:
//        {
//            ellipseItemTmp->setFillStatus(itemOperator->oriBFill);
//        }
//            break;
//        case FILLCOLORCHANGEOPT:
//        {
//            ellipseItemTmp->setFillColor(itemOperator->oriFillColor);
//        }
//            break;
//        default:
//            break;
//        }
    }
    else if (LineItem *lineItemTmp = dynamic_cast<LineItem *> (geometryItemTmp))
    {
//        QLineF lineTmp;

//        lineItemTmp->setItemStatus(NORMOAL);
//        switch (itemOperator->type) {
//        case ADDOPT:
//        {
//            m_geometryItemList.removeOne(lineItemTmp);
//        }
//            break;
//        case MOVEOPT:
//        {
//            QPointF pt = itemOperator->originalPt;
//            QPointF nPt = lineItemTmp->getOriLine().p1();

//            QPointF offsetPt = nPt - pt;

//            lineTmp = QLineF(pt, lineItemTmp->getOriLine().p2()-offsetPt);

//            lineItemTmp->setLine(lineTmp);
//            lineItemTmp->calcOriGeometry(QPoint(0, 0),
//                                         m_img.width(),m_img.height(),
//                                         1, 0, false, false);
//        }
//            break;
//        case DELETEOPT:
//            m_geometryItemList.push_back(lineItemTmp);
//            break;
//        case CHANGEOPT:
//            lineTmp = itemOperator->originLine;
//            lineItemTmp->setLine(lineTmp);
//            lineItemTmp->calcOriGeometry(QPoint(0, 0),
//                                         m_img.width(),m_img.height(),
//                                         1, 0, false, false);
//            break;
//        case COLORCHAGNEOPT:
//        {
//            lineItemTmp->setColor(itemOperator->origiColor);
//        }
//            break;
//        case LINEWIDTHCHANGEOPT:
//        {
//            lineItemTmp->setLineWidth(itemOperator->origiLineWidth);
//        }
//            break;
//        case PENSTYLEOPT:
//        {
//            lineItemTmp->setPenStyle(itemOperator->oriPenStyle);
//        }
//            break;
//        default:
//            break;
//        }
    }

    updateMeasureTable();

    updateOperatorStatus();

    update();
}


void MainWindow::doNextOperator(ItemOperator *itemOperator)
{
    GeometryItemBase *geometryItemTmp = itemOperator->geometryItem;

    if (NULL == geometryItemTmp)
        return;

    if (RectItem *rectItemTmp = dynamic_cast<RectItem *> (geometryItemTmp))
    {
        QRectF rectTmp;

        rectItemTmp->setItemStatus(NORMOAL);
        switch (itemOperator->type) {
        case ADDOPT:
        {
            m_geometryItemList.push_back(rectItemTmp);
        }
            break;
        case MOVEOPT:
        {
            QPointF pt = itemOperator->movePt;
            rectTmp = QRectF(pt.x(),
                             pt.y(),
                             rectItemTmp->getOriWidth(),
                             rectItemTmp->getOriHeight());

            rectItemTmp->setRect(rectTmp);

            rectItemTmp->calcOriGeometry(getCurOffset(),
                                         mCurImgWidth,
                                         mCurImgHeight,
                                         mScale,
                                         mNeedRotate, mBFlip, mBMirror);
        }
            break;
        case DELETEOPT:
            m_geometryItemList.removeOne(rectItemTmp);
            break;
        case CHANGEOPT:
            rectTmp = itemOperator->changedRect;
            rectItemTmp->setRect(rectTmp);

            rectItemTmp->calcOriGeometry(getCurOffset(),
                                         mCurImgWidth,
                                         mCurImgHeight,
                                         mScale,
                                         mNeedRotate, mBFlip, mBMirror);
            break;
        case COLORCHAGNEOPT:
        {
            rectItemTmp->setColor(itemOperator->changeColor);
        }
            break;
        case LINEWIDTHCHANGEOPT:
        {
            rectItemTmp->setLineWidth(itemOperator->changeLineWidth);
        }
            break;
        case PENSTYLEOPT:
        {
            rectItemTmp->setPenStyle(itemOperator->changePenStyle);
        }
            break;
        case FILLCHANGEOPT:
        {
            rectItemTmp->setFillStatus(itemOperator->changeBFill);
        }
            break;
        case FILLCOLORCHANGEOPT:
        {
            rectItemTmp->setFillColor(itemOperator->changeFillColor);
        }
            break;
        default:
            break;
        }
    }
    else if (TextItem *textItemTmp = dynamic_cast<TextItem *> (geometryItemTmp))
    {

    }
    else if (EllipseItem *ellipseItemTmp = dynamic_cast<EllipseItem *> (geometryItemTmp))
    {
//        QRectF rectTmp;

//        ellipseItemTmp->setItemStatus(NORMOAL);
//        switch (itemOperator->type) {
//        case ADDOPT:
//        {
//            m_geometryItemList.push_back(ellipseItemTmp);
//        }
//            break;
//        case MOVEOPT:
//        {
//            QPointF pt = itemOperator->movePt;
//            rectTmp = QRectF(pt.x(),
//                            pt.y(),
//                            ellipseItemTmp->getOriWidth(),
//                            ellipseItemTmp->getOriHeight());

//            ellipseItemTmp->setRect(rectTmp);

//            ellipseItemTmp->calcOriGeometry(QPoint(0, 0),
//                                             m_img.width(),m_img.height(),
//                                             1, 0, false, false);
//        }
//            break;
//        case DELETEOPT:
//            m_geometryItemList.removeOne(ellipseItemTmp);
//            break;
//        case CHANGEOPT:
//            rectTmp = itemOperator->changedRect;
//            ellipseItemTmp->setRect(rectTmp);

//            ellipseItemTmp->calcOriGeometry(QPoint(0, 0),
//                                             m_img.width(),m_img.height(),
//                                             1, 0, false, false);
//            break;
//        case COLORCHAGNEOPT:
//        {
//            ellipseItemTmp->setColor(itemOperator->changeColor);
//        }
//            break;
//        case LINEWIDTHCHANGEOPT:
//        {
//            ellipseItemTmp->setLineWidth(itemOperator->changeLineWidth);
//        }
//            break;
//        case PENSTYLEOPT:
//        {
//            ellipseItemTmp->setPenStyle(itemOperator->changePenStyle);
//        }
//            break;
//        case FILLCHANGEOPT:
//        {
//            ellipseItemTmp->setFillStatus(itemOperator->changeBFill);
//        }
//            break;
//        case FILLCOLORCHANGEOPT:
//        {
//            ellipseItemTmp->setFillColor(itemOperator->changeFillColor);
//        }
//            break;
//        default:
//            break;
//        }
    }
    else if (LineItem *lineItemTmp = dynamic_cast<LineItem *> (geometryItemTmp))
    {
//        QLineF lineTmp;

//        lineItemTmp->setItemStatus(NORMOAL);
//        switch (itemOperator->type) {
//        case ADDOPT:
//        {
//            m_geometryItemList.push_back(lineItemTmp);
//        }
//            break;
//        case MOVEOPT:
//        {
//            QPointF pt = itemOperator->movePt;
//            QPointF nPt = lineItemTmp->getOriLine().p1();

//            QPointF offsetPt = nPt - pt;

//            lineTmp = QLineF(pt, lineItemTmp->getOriLine().p2()-offsetPt);

//            lineItemTmp->setLine(lineTmp);

//            lineItemTmp->calcOriGeometry(QPoint(0, 0),
//                                         m_img.width(),m_img.height(),
//                                         1, 0, false, false);
//        }
//            break;
//        case DELETEOPT:
//            m_geometryItemList.removeOne(lineItemTmp);
//            break;
//        case CHANGEOPT:
//            lineTmp = itemOperator->changeLine;
//            lineItemTmp->setLine(lineTmp);
//            lineItemTmp->calcOriGeometry(QPoint(0, 0),
//                                         m_img.width(),m_img.height(),
//                                         1, 0, false, false);
//            break;
//        case COLORCHAGNEOPT:
//        {
//            lineItemTmp->setColor(itemOperator->changeColor);
//        }
//            break;
//        case LINEWIDTHCHANGEOPT:
//        {
//            lineItemTmp->setLineWidth(itemOperator->changeLineWidth);
//        }
//            break;
//        case PENSTYLEOPT:
//        {
//            lineItemTmp->setPenStyle(itemOperator->changePenStyle);
//        }
//            break;
//        default:
//            break;
//        }
    }

    updateMeasureTable();

    updateOperatorStatus();

    update();
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

            if (point2Polygon(polygonf, polygonf.size(), imgPt) >= 0)
            {
                m_selectedIndex = i;
                m_curPosPt = imgPt;
            }
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

                updateMeasureTable();
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

                updateMeasureTable();
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

                updateMeasureTable();
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

                updateMeasureTable();
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

                updateMeasureTable();
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

                updateMeasureTable();
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


ItemOperator *MainWindow::getPreOperator()
{
    ItemOperator *preOperator = NULL;

    if (m_curOperatorIndex >= 0 &&
        m_curOperatorIndex < m_ItemOperatorList.size())
    {
        preOperator = m_ItemOperatorList.at(m_curOperatorIndex);
        m_curOperatorIndex -- ;
    }

    return preOperator;
}

ItemOperator *MainWindow::getNextOperator()
{
    ItemOperator *preOperator = NULL;

    int operatorIndexTmp = m_curOperatorIndex + 1;
    if (operatorIndexTmp >= 0 &&
        operatorIndexTmp < m_ItemOperatorList.size())
    {
        preOperator = m_ItemOperatorList.at(operatorIndexTmp);
        m_curOperatorIndex ++;
    }

    return preOperator;
}

void MainWindow::slot_btnPreToolClick()
{
    ItemOperator *itemOperator = getPreOperator();

    if (NULL != itemOperator)
    {
        doPreOperator(itemOperator);
    }
}

void MainWindow::slot_btnNextToolClick()
{
    ItemOperator *itemOperator = getNextOperator();

    if (NULL != itemOperator)
    {
        doNextOperator(itemOperator);
    }
}

void MainWindow::slot_btnDeleteToolClick()
{
    m_selectedIndex = -1;
    m_eSldDragDirection = NONEDIR;

    m_ItemOperatorList.clear();

    m_geometryItemList.clear();

//    updateMesureDlgCtr(NULL);

    updateMeasureTable();

    updateOperatorStatus();

    update();
}

void MainWindow::slot_btnEraserToolClick()
{
    if (m_selectedIndex >= 0 && m_selectedIndex < m_geometryItemList.count())
    {
       GeometryItemBase *geometryItemBaseTmp = m_geometryItemList.at(m_selectedIndex);
       geometryItemBaseTmp->setItemStatus(NORMOAL);
       m_geometryItemList.removeOne(geometryItemBaseTmp);

       m_selectedIndex = -1;
       m_eSldDragDirection = NONEDIR;

       //插入一个删除操作
       m_itemOperatorTmp = new ItemOperator;
       m_itemOperatorTmp->geometryItem = geometryItemBaseTmp;
       m_itemOperatorTmp->type = DELETEOPT;
       insetOperator(m_itemOperatorTmp);

//       updateMesureDlgCtr(NULL);

       updateMeasureTable();

       updateOperatorStatus();

       update();
    }
}


void MainWindow::updateMeasureTable()
{
    ui->tableWidget_measure->clearContents();
    ui->tableWidget_measure->setRowCount(0);

    for (int i=0; i<m_geometryItemList.size(); i++)
    {
        GeometryItemBase *geometryItemTmp = m_geometryItemList.at(i);

        if (RectItem *rectItemTmp = dynamic_cast<RectItem *> (geometryItemTmp))
        {
            QRectF rect = rectItemTmp->getOriRect();

            //--SNR--
            double dMean, dStd, dSNR;
            rectItemTmp->getSNR(dMean, dStd, dSNR);

            addOneMeasure(i, QStringLiteral("矩形"), rectItemTmp->getItemName(), rect.topLeft().toPoint(), rect.bottomRight().toPoint(),
                          rect.center().toPoint(), rectItemTmp->getOriHeightUnit(), rectItemTmp->getOriWidthUnit(),
                          0, 0, 0,
                          rectItemTmp->getAreaUnit(),
                          rectItemTmp->getCircumferenceUnit(),
                          rectItemTmp->getIntensity(),
                          dMean, dSNR);
        }
        else if (EllipseItem *ellipseItemTmp = dynamic_cast<EllipseItem *> (geometryItemTmp))
        {
            QRectF rect = ellipseItemTmp->getOriRect();
            //--SNR--
            double dMean, dStd, dSNR;
            ellipseItemTmp->getSNR(dMean, dStd, dSNR);

            addOneMeasure(i, QStringLiteral("椭圆"), ellipseItemTmp->getItemName(), rect.topLeft().toPoint(), rect.bottomRight().toPoint(),
                          rect.center().toPoint(),
                          ellipseItemTmp->getOriWidthUnit(), ellipseItemTmp->getOriHeightUnit(),
                          ellipseItemTmp->getSemiMajorAxisUnit(), ellipseItemTmp->getSemiMinorAxisUnit(),
                          0,
                          ellipseItemTmp->getAreaUnit(),
                          ellipseItemTmp->getCircumferenceUnit(),
                          ellipseItemTmp->getIntensity(),
                          dMean, dSNR);
        }
        else if (LineItem *lineItemTmp = dynamic_cast<LineItem *> (geometryItemTmp))
        {
            QLineF line = lineItemTmp->getOriLine();

            addOneMeasure(i, QStringLiteral("直线"),lineItemTmp->getItemName(), line.p1().toPoint(), line.p2().toPoint(),
                          lineItemTmp->getCenterPt(),
                          lineItemTmp->getLengthUnit(), 0,
                          0,0,
                          lineItemTmp->getAngle(),
                          0,
                          0,
                          0,
                          0,
                          0);
        }
    }
}

void MainWindow::addOneMeasure(int num,
                   QString name, QString typeName,
                   QPoint beginPt,
                   QPoint endPt,
                   QPoint centerPt,
                   qreal width,
                   qreal height, qreal majorAxis, qreal minorAxis, qreal angle,
                   qreal area,
                   qreal perimeter,
                   qreal intersity,
                   qreal mean,
                   qreal snr)
{
    ui->tableWidget_measure->setRowCount(ui->tableWidget_measure->rowCount() + 1);

    int row = ui->tableWidget_measure->rowCount() - 1;

    //--items--
    QTableWidgetItem *noItem = new QTableWidgetItem;
    noItem->setText(QString("%1").arg(num));
    noItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *nameItem = new QTableWidgetItem;
    nameItem->setText(name);
    nameItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *beginItem = new QTableWidgetItem;
//    beginItem->setText(QString("(%1, %2)").arg(beginPt.x()).arg(beginPt.y()));
    if (typeName == tr("Line") )
        beginItem->setText(QString("-"));
    else
        beginItem->setText(QString("%1").arg(intersity));
    beginItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *endItem = new QTableWidgetItem;
//    endItem->setText(QString("(%1, %2)").arg(endPt.x()).arg(endPt.y()));
    if (typeName == tr("Line") )
        endItem->setText(QString("-"));
    else
        endItem->setText(QString("%1").arg(QString::number(mean, 'f', 2)));
    endItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *centerItem = new QTableWidgetItem;
//    centerItem->setText(QString("(%1, %2)").arg(centerPt.x()).arg(centerPt.y()));
    if (typeName == tr("Line") )
        centerItem->setText(QString("-"));
    else
        centerItem->setText(QString("%1").arg(QString::number(snr, 'f', 2)));
    centerItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *widthItem = new QTableWidgetItem;
    widthItem->setText(QString("%1").arg(QString::number(width, 'f', 2)));
    widthItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *heightItem = new QTableWidgetItem;
    if (typeName == tr("Line") )
        heightItem->setText(QString("-"));
    else
        heightItem->setText(QString("%1").arg(QString::number(height, 'f', 2)));

    heightItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *majorItem = new QTableWidgetItem;
    if (typeName == tr("Rectangle") || typeName == tr("Line") )
        majorItem->setText(QString("-"));
    else
        majorItem->setText(QString("%1").arg(QString::number(majorAxis, 'f', 2)));
    majorItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *minorItem = new QTableWidgetItem;
    if (typeName == tr("Rectangle") || typeName == tr("Line" ))
        minorItem->setText(QString("-"));
    else
        minorItem->setText(QString("%1").arg(QString::number(minorAxis, 'f', 2)));
    minorItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *angleItem = new QTableWidgetItem;
    if (typeName == tr("Rectangle") || typeName == tr("Ellipse") )
        angleItem->setText(QString("-"));
    else
        angleItem->setText(QString("%1").arg(QString::number(angle, 'f', 2)));
    angleItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *areaItem = new QTableWidgetItem;
    if (typeName == tr("Line"))
        areaItem->setText(QString("-"));
    else
        areaItem->setText(QString("%1").arg(QString::number(area, 'f', 2)));

    areaItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *perimeterItem = new QTableWidgetItem;
    if (typeName == tr("Line"))
        perimeterItem->setText(QString("-"));
    else
        perimeterItem->setText(QString("%1").arg(QString::number(perimeter, 'f', 2)));

    perimeterItem->setTextAlignment(Qt::AlignCenter);

    //--additems--
    ui->tableWidget_measure->setItem(row, 0, noItem);
    ui->tableWidget_measure->setItem(row, 1, nameItem);
    ui->tableWidget_measure->setItem(row, 2, beginItem);
    ui->tableWidget_measure->setItem(row, 3, endItem);
    ui->tableWidget_measure->setItem(row, 4, centerItem);
    ui->tableWidget_measure->setItem(row, 5, widthItem);
    ui->tableWidget_measure->setItem(row, 6, heightItem);
    ui->tableWidget_measure->setItem(row, 7, majorItem);
    ui->tableWidget_measure->setItem(row, 8, minorItem);
    ui->tableWidget_measure->setItem(row, 9, angleItem);
    ui->tableWidget_measure->setItem(row, 10, areaItem);
    ui->tableWidget_measure->setItem(row, 11, perimeterItem);
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
//    QPoint imgPt = convertImgPt(pt);
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
                    rectItemTmp->calcOriGeometry(getCurOffset(),
                                                 mCurImgWidth,
                                                 mCurImgHeight,
                                                 mScale,
                                                 mNeedRotate, mBFlip, mBMirror);

                    m_curPosPt = imgPt;

//                    updateMesureDlgCtr(geometryItemTmp);

                    updateMeasureTable();

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
                    rectItemTmp->calcOriGeometry(getCurOffset(),
                                                 mCurImgWidth,
                                                 mCurImgHeight,
                                                 mScale,
                                                 mNeedRotate, mBFlip, mBMirror);

                    m_curPosPt = imgPt;

//                    updateMesureDlgCtr(geometryItemTmp);

                    updateMeasureTable();

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
            QList<DragItem> dragItemList = ellipseItemTmp->getDragItemData().getDragItems();

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
                if (ellipseItemTmp->getItemStatus() == SELECTEDDRAG)
                {
                    //拖拽点移动
                    if (!m_beginChange)
                    {
                        m_beginChange = true;
                        m_originRect  = ellipseItemTmp->getOriRect();
                    }

                    QPoint offsetPt = m_curPosPt - imgPt;
                    QRectF oldRect = ellipseItemTmp->getRect();
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
                                        ellipseItemTmp->getWidth() + offsetPt.x(),
                                        ellipseItemTmp->getHeight() - offsetPt.y());

                        break;
                    case RIGHTTOP:
                        rectTmp = QRectF(oldRect.x(),
                                        oldRect.y() - offsetPt.y(),
                                        ellipseItemTmp->getWidth() - offsetPt.x(),
                                        ellipseItemTmp->getHeight() + offsetPt.y());
                        break;
                    case RIGHTBOTTOM:
                        rectTmp = QRectF(oldRect.x(),
                                        oldRect.y(),
                                        ellipseItemTmp->getWidth() - offsetPt.x(),
                                        ellipseItemTmp->getHeight() - offsetPt.y());
                        break;
                    default :
                        break;
                    }

                    ellipseItemTmp->setRect(rectTmp);
                    ellipseItemTmp->calcOriGeometry(getCurOffset(),
                                                    mCurImgWidth,
                                                    mCurImgHeight,
                                                    mScale,
                                                    mNeedRotate, mBFlip, mBMirror);

                    m_curPosPt = imgPt;

//                    updateMesureDlgCtr(geometryItemTmp);

                    updateMeasureTable();

                    update();

                    return ;
                }
            }
            else
            {
                //设置鼠标状态
                if (ellipseItemTmp->getBoundingRectF().contains(imgPt)){
                    setCursor(Qt::SizeAllCursor);
                }
                else{
                    setCursor(Qt::ArrowCursor);
                }

                if (ellipseItemTmp->getItemStatus() == SELECTEDMOVE)
                {
                    //ITEM移动
                    if (!m_beginMove)
                    {
                        m_beginMove = true;
                        m_originPt = ellipseItemTmp->getOriRect().topLeft();
                    }

                    QPoint offsetPt = m_curPosPt - imgPt;
                    QRectF oldRect = ellipseItemTmp->getRect();

                    QRectF rectTmp = QRectF(oldRect.x() - offsetPt.x(),
                                          oldRect.y() - offsetPt.y(),
                                          ellipseItemTmp->getWidth(),
                                          ellipseItemTmp->getHeight());


                    ellipseItemTmp->setRect(rectTmp);
                    ellipseItemTmp->calcOriGeometry(getCurOffset(),
                                                    mCurImgWidth,
                                                    mCurImgHeight,
                                                    mScale,
                                                    mNeedRotate, mBFlip, mBMirror);

                    m_curPosPt = imgPt;

//                    updateMesureDlgCtr(geometryItemTmp);

                    updateMeasureTable();

                    update();

                    return ;
                }
            }
        }
        else if (LineItem *lineItemTmp = dynamic_cast<LineItem *> (geometryItemTmp))
        {
            QList<DragItem> dragItemList = lineItemTmp->getDragItemData().getDragItems();

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
                if (lineItemTmp->getItemStatus() == SELECTEDDRAG)
                {
                    //拖拽点移动
                    if (!m_beginChange)
                    {
                        m_beginChange = true;
                        m_originLine  = lineItemTmp->getOriLine();
                    }

                    QLineF lineTmp = lineItemTmp->getLine();

                    switch(curDragDirection)
                    {
                    case DragDirection::LEFT:
                        lineTmp.setP1(imgPt);
                        break;
                    case DragDirection::RIGHT:
                        lineTmp.setP2(imgPt);
                        break;
                    default :
                        break;
                    }

                    lineItemTmp->setLine(lineTmp);
                    lineItemTmp->calcOriGeometry(getCurOffset(),
                                                 mCurImgWidth,
                                                 mCurImgHeight,
                                                 mScale,
                                                 mNeedRotate, mBFlip, mBMirror);

                    m_curPosPt = imgPt;

//                    updateMesureDlgCtr(geometryItemTmp);

                    updateMeasureTable();

                    update();

                    return ;
                }
            }
            else
            {
                QPolygonF polygonf = lineItemTmp->getBoundingPolygonf();

                //设置鼠标状态
                if (point2Polygon(polygonf, polygonf.size(), imgPt) >= 0){
                    setCursor(Qt::SizeAllCursor);
                }
                else{
                    setCursor(Qt::ArrowCursor);
                }


                if (lineItemTmp->getItemStatus() == SELECTEDMOVE)
                {
                    //ITEM移动
                    if (!m_beginMove)
                    {
                        m_beginMove = true;
                        m_originPt = lineItemTmp->getOriLine().p1();
                    }

                    QPoint offsetPt = m_curPosPt - imgPt;

                    QLineF oldLine = lineItemTmp->getLine();
                    QLineF lineTmp = QLineF(oldLine.p1()-offsetPt, oldLine.p2()-offsetPt);

                    lineItemTmp->setLine(lineTmp);
                    lineItemTmp->calcOriGeometry(getCurOffset(),
                                                 mCurImgWidth,
                                                 mCurImgHeight,
                                                 mScale,
                                                 mNeedRotate, mBFlip, mBMirror);

                    m_curPosPt = imgPt;

//                    updateMesureDlgCtr(geometryItemTmp);

                    updateMeasureTable();

                    update();

                    return ;
                }
            }
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
    CloseHandle(hEvent);

    if (nullptr != mModel)
    {
        delete mModel;
        mModel = nullptr;
    }

    if (m_containerMenu)
    {
        delete m_containerMenu;

        m_containerMenu = NULL;
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

    if (NULL != mGeyImgWdg)
    {
        delete mGeyImgWdg;
        mGeyImgWdg = NULL;
    }

	if (NULL != mColorWdg)
	{
		delete mColorWdg;
		mColorWdg = NULL;
	}

    if (NULL != m_loadingDlg)
    {
        delete m_loadingDlg;
        m_loadingDlg = NULL;
    }

    mIndexFileOfs.close();


    //save config
    Appconfig::saveConfigFile();

    //delete temp file
    Appconfig::deleteAppTempFile(Appconfig::AppDataPath_Tmp);

    delete ui;
}


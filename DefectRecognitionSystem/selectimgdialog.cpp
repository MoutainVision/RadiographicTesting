#include "selectimgdialog.h"
#include "ui_selectimgdialog.h"

SelectImgDialog::SelectImgDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectImgDialog)
{
    ui->setupUi(this);

    mSrcWdg = NULL;
    mSimilarWdg = NULL;

    mCurIndex = 0;

    ui->tableWidget_recheck->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->tableWidget_recheck, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(slot_tableCellClicked(int, int)));


    connect(ui->pushButton_back, &QPushButton::clicked, [=](){

        ui->stackedWidget->setCurrentIndex(0);

    });
}

void SelectImgDialog::slot_tableCellClicked(int row, int col)
{
    QString filePath = ui->tableWidget_recheck->item(row, 2)->text();

    std::thread([=] {

        //db file
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));          //设置随机数种子
        int rand = qrand() % 10000;

        QString dateTimeStr = QDateTime::currentDateTime().toString("MM-dd hh-mm-ss-zzz");
        QString outFileName = QString(QStringLiteral("%1/%2_%3.jpg")).arg(Appconfig::AppDataPath_Tmp).arg(dateTimeStr).arg(rand);

        std::string errorStr;
        ReadDCMFile::readDCMFileLib(filePath.toLocal8Bit().toStdString(), outFileName.toLocal8Bit().toStdString(), errorStr);

        mDbFileInfo.filePath = filePath;
        mDbFileInfo.transFilePath = outFileName;


        ReadDCMFile::readDCMFileLib(mCurFileInfo.filePath.toLocal8Bit().toStdString(),
                                    mCurFileInfo.transFilePath.toLocal8Bit().toStdString(),
                                    errorStr);


        FunctionTransfer::runInMainThread([=]()
        {
            ui->stackedWidget->setCurrentIndex(1);

            setDCMFileInfo(mCurFileInfo, mDbFileInfo);
        });


    }).detach();
}


void SelectImgDialog::setCurrentInfo(DcmFileNode curInfo)
{
    mCurFileInfo = curInfo;
}

DcmFileNode SelectImgDialog::getFileInfo()
{
    if (mCurIndex == 0)
        return mCurFileInfo;
    else if (mCurIndex == 1)
        return mDbFileInfo;
}

SelectImgDialog::~SelectImgDialog()
{
    delete ui;
}

void SelectImgDialog::setSimilarValues(vector<RetrievalResult> aRes)
{
    ui->tableWidget_recheck->clearContents();
    ui->tableWidget_recheck->setRowCount(0);

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

void SelectImgDialog::setDCMFileInfo(DcmFileNode srcInfo, DcmFileNode similarInfo)
{
    if (NULL == mSrcWdg)
    {
        mSrcWdg = new PreWdg;
        mSrcWdg->setSelectStatus(true);

        connect(mSrcWdg, &PreWdg::sig_clickItem, [=](){
            mSrcWdg->setSelectStatus(true);

            mCurIndex = 0;

            if (NULL != mSimilarWdg)
            {
                mSimilarWdg->setSelectStatus(false);
            }
        });
    }

    mSrcWdg->setParent(ui->widget_src);
    mSrcWdg->setDCMFileInfo(srcInfo);


    int perWidth  = ui->widget_src->width();
    int perHeight = ui->widget_src->height();

    mSrcWdg->setFixedSize(perWidth, perHeight);
    mSrcWdg->setGeometry(0, 0, perWidth, perHeight);

    mSrcWdg->show();

//------similar--
    if (NULL == mSimilarWdg)
    {
        mSimilarWdg = new PreWdg;

        connect(mSimilarWdg, &PreWdg::sig_clickItem, [=](){
            mSimilarWdg->setSelectStatus(true);

            mCurIndex = 1;

            if (NULL != mSrcWdg)
            {
                mSrcWdg->setSelectStatus(false);
            }
        });
    }

    mSimilarWdg->setParent(ui->widget_repead);
    mSimilarWdg->setDCMFileInfo(similarInfo);
    perWidth  = ui->widget_repead->width();
    perHeight = ui->widget_repead->height();

    mSimilarWdg->setFixedSize(perWidth, perHeight);
    mSimilarWdg->setGeometry(0, 0, perWidth, perHeight);

    mSimilarWdg->show();
}

#include "prewdg.h"
#include "ui_prewdg.h"

#include <QDebug>

PreWdg::PreWdg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PreWdg)
{
    ui->setupUi(this);

//    FunctionTransfer::init(QThread::currentThreadId());


    ui->widget_img->installEventFilter(this);
}

PreWdg::~PreWdg()
{
    delete ui;
}

void PreWdg::setDCMFileInfo(DcmFileNode info)
{
    mInfo = info;

    mPreviewPixImg.load(info.transFilePath);

    update();

//    std::thread([&] {
//        std::string errorStr;

//        ReadDCMFile::readDCMFileLib(mInfo.filePath.toLocal8Bit().toStdString(), mInfo.transFilePath.toLocal8Bit().toStdString(), errorStr);

//        FunctionTransfer::runInMainThread([=]()
//        {
//            mPreviewPixImg.load(mInfo.transFilePath);

//            update();
//        });

//    }).detach();
}

void PreWdg::reLoad()
{
    if (mPreviewPixImg.isNull())
    {
        qDebug() << mInfo.transFilePath;
//        mPreviewPixImg.load(mInfo.transFilePath);
    }
}

bool PreWdg::eventFilter(QObject *obj, QEvent *e)
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
        else if (e->type() == QEvent::Resize)
        {
            reLoad();
            update();
        }
    }

    return QWidget::eventFilter(obj, e);
}

#include "recognizewdg.h"
#include "ui_recognizewdg.h"

#include <QFile>
#include <QDebug>

RecognizeWdg::RecognizeWdg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecognizeWdg)
{
    ui->setupUi(this);

    ui->widget_measure->hide();
    ui->widget_tool->hide();

    ui->widget_img_pre->installEventFilter(this);


    mSourceX = 0;
    mSourceY = 0;


    connect(ui->pushButton_measure, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_invert, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));

    connect(ui->horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(slot_scrollAreaXChange(int)));
    connect(ui->verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(slot_scrollAreaYChange(int)));

}

RecognizeWdg::~RecognizeWdg()
{
    dmfile.Release();
    delete ui;
}

void RecognizeWdg::setDcmFileInfo(DcmFileNode dcmInfo)
{
    mCurDcmFileInfo = dcmInfo;

    std::thread([=] {

        dmfile.Load(dcmInfo.filePath.toLocal8Bit().toStdString().c_str());

        FunctionTransfer::runInMainThread([=]()
        {
//            qDebug() << "1111";
            this->showImg();
        });

    }).detach();
}

void RecognizeWdg::shortImgToImage(unsigned short *pImg, int nW, int nH, QImage &img)
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

void RecognizeWdg::reSize(int newW, int newH)
{
//    LIBIMGPROC_API bool Resize(unsigned short *&pImg,	//圖像數據指針，既為輸入也為輸出
//        int &nW,					//圖像寬度，既為輸入也為輸出
//        int &nH,					//圖像高度，既為輸入也為輸出
//        int nNewW,				//輸出圖像寬度
//        int nNewH);				//輸出圖像高度

//    Resize(dmfile.GetBuffer())
}

void RecognizeWdg::reSize(float scale)
{

}

void RecognizeWdg::showImg()
{
    int w = dmfile.GetWidth();
    int h = dmfile.GetHeight();

    qDebug() << "3333";
    shortImgToImage(dmfile.GetBuffer(), w, h, mPaintImg);

    mPaintRect.setRect(0, 0, w, h);

    showScrollBar();

    update();
}

void RecognizeWdg::setScrollRanges(int maxX, int maxY)
{
    ui->horizontalScrollBar->setRange(30, maxX);
    ui->verticalScrollBar->setRange(30, maxY);
}


void RecognizeWdg::showScrollBar(bool status)
{
    if (!status)
    {
        ui->horizontalScrollBar->hide();
        ui->verticalScrollBar->hide();
    }
    else
    {
        setScrollRanges(mPaintRect.width() - ui->widget_img_pre->width() + 30, mPaintRect.height() - ui->widget_img_pre->height() + 30);

        if (mPaintRect.width() > ui->widget_img_pre->width())
        {
            ui->horizontalScrollBar->show();
            ui->horizontalScrollBar->setValue(mPaintRect.width() / 2);
        }
        else
            ui->horizontalScrollBar->hide();

        if (mPaintRect.height() > ui->widget_img_pre->height())
        {
            ui->verticalScrollBar->show();
            ui->verticalScrollBar->setValue(mPaintRect.height() / 2);
        }
        else
            ui->horizontalScrollBar->hide();
    }
}

void RecognizeWdg::showScrollBar()
{
    if (mPaintRect.width() > ui->widget_img_pre->width()
            || mPaintRect.height() > ui->widget_img_pre->height())
    {
        showScrollBar(true);
    }
    else
        showScrollBar(false);
}

void RecognizeWdg::slot_scrollAreaXChange(int value)
{
    if (!ui->horizontalScrollBar->isHidden())
    {
        int nW = ui->widget_img_pre->width();
        int nH = ui->widget_img_pre->height();

        mSourceX = value;

        mSourceRect = QRect(mSourceX, mSourceY, nW, nH);

        update();
    }
}

void RecognizeWdg::slot_scrollAreaYChange(int value)
{
    if (!ui->verticalScrollBar->isHidden())
    {
        int nW = ui->widget_img_pre->width();
        int nH = ui->widget_img_pre->height();

        mSourceY = value;

        mSourceRect = QRect(mSourceX, mSourceY, nW, nH);

        update();
    }
}

void RecognizeWdg::slotBtnClick(bool bClick)
{
    if (QObject::sender() == ui->pushButton_measure)
    {
        ui->widget_tool->setVisible(ui->pushButton_measure->isChecked());
    }
}

bool RecognizeWdg::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == ui->widget_img_pre)
    {
        if (e->type() == QEvent::Paint)
        {
            QPainter  p(ui->widget_img_pre);

            if (!mPaintImg.isNull())
            {
                int nHeight = ui->widget_img_pre->height();
                int nWidth = ui->widget_img_pre->width();

                QBrush b(QColor("#000000"));
                p.setBrush(b);
                p.drawRect(ui->widget_img_pre->rect());



//                QRectF target(10.0, 20.0, 80.0, 60.0);
//                QRectF source(0.0, 0.0, 70.0, 40.0);
//                QImage image(":/images/myImage.png");

//                QPainter painter(this);
//                painter.drawImage(target, image, source);

                //p.drawImage(QRect(0,0,nWidth, nHeight), mPaintImg);

//                QRect target()


                p.drawImage(QRect(0, 0, nWidth, nHeight), mPaintImg, mSourceRect);



                p.setPen(QColor("#ffff00"));
                p.setFont(QFont("Microsoft YaHei UI", 20));
                p.drawText(QPoint(20, 40), mCurDcmFileInfo.filePath);

                p.drawText(QPoint(20, ui->widget_img_pre->rect().bottom()-30), QString("[%1 x %2]").arg(mCurDcmFileInfo.width).arg(mCurDcmFileInfo.height));

                return true;
            }
        }
    }


    return QWidget::eventFilter(obj, e);
}


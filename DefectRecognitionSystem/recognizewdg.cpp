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

    mBInvert = false;

    mSourceX = 0;
    mSourceY = 0;

    ui->verticalSlider_diameter->setRange(5, 100);


    connect(ui->pushButton_measure, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_invert, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));

    connect(ui->horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(slot_scrollAreaXChange(int)));
    connect(ui->verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(slot_scrollAreaYChange(int)));

    connect(ui->verticalSlider_diameter, SIGNAL(sliderReleased()), this, SLOT(slot_sliderReleased()));
}

void RecognizeWdg::slot_sliderReleased()
{
    int value = ui->verticalSlider_diameter->value();
    mScale = (float)value / 100;

    ui->lineEdit_diameter->setText(QString("%1").arg(value));

    delImg();
}

RecognizeWdg::~RecognizeWdg()
{
    dmfile.Release();

    delete []m_pImgPro;
    m_pImgPro = NULL;

    delete ui;
}

void RecognizeWdg::setDcmFileInfo(DcmFileNode dcmInfo)
{
    mCurDcmFileInfo = dcmInfo;

    std::thread([=] {

        dmfile.Load(dcmInfo.filePath.toLocal8Bit().toStdString().c_str());


        FunctionTransfer::runInMainThread([=]()
        {
            m_pSrcImg = dmfile.GetBuffer();
            mSrcImgWidth = dmfile.GetWidth();
            mSrcImgHeight = dmfile.GetHeight();

            m_pImgPro = new unsigned short[mSrcImgWidth * mSrcImgHeight];
            memcpy(m_pImgPro, m_pSrcImg, mSrcImgWidth*mSrcImgHeight*sizeof(unsigned short));

            this->showNomal();
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

void RecognizeWdg::showNomal()
{
    int wdgW = ui->widget_img_pre->width();
    int wdgH = ui->widget_img_pre->height();

    int nX = 0;
    int nY = 0;
    int nWidth = 0;
    int nHeight = 0;

    double dRatioW = (double)(wdgW) / (double)mSrcImgWidth;
    double dRatioH = (double)(wdgH) / (double)mSrcImgHeight;
    double dRat = dRatioW > dRatioH? dRatioH : dRatioW;

    mCurImgWidth = dRat * mSrcImgWidth;
    mCurImgHeight = dRat * mSrcImgHeight;

    mScale = dRat;

    ui->verticalSlider_diameter->setValue((int)(mScale*100));
    ui->lineEdit_diameter->setText(QString("%1").arg((int)(mScale*100)));

    int srcW = mSrcImgWidth;
    int srcH = mSrcImgHeight;
    Resize(m_pImgPro, srcW, srcH, mCurImgWidth, mCurImgHeight);

    showImg(m_pImgPro, srcW, srcH);
}

void RecognizeWdg::reSize(int newW, int newH)
{
    mScale = mSrcImgWidth / newW;

    int srcW = mCurImgWidth;
    int srcH = mCurImgHeight;

    Resize(m_pImgPro, srcW, srcH, newW, newH);

    mCurImgWidth  = srcW;
    mCurImgHeight = srcH;
}

void RecognizeWdg::reSize(float scale)
{
    mScale = scale;

    mCurImgWidth = scale * mSrcImgWidth;
    mCurImgHeight = scale * mSrcImgHeight;

    int srcW = mSrcImgWidth;
    int srcH = mSrcImgHeight;

    Resize(m_pImgPro, srcW, srcH, mCurImgWidth, mCurImgHeight);

    mCurImgWidth = srcW;
    mCurImgHeight = srcH;
}

void RecognizeWdg::showImg()
{
    int w = dmfile.GetWidth();
    int h = dmfile.GetHeight();


    shortImgToImage(dmfile.GetBuffer(), w, h, mPaintImg);

    mPaintRect.setRect(0, 0, w, h);

    showScrollBar();

    update();
}

void RecognizeWdg::showImg(unsigned short *pImg, int nW, int nH)
{
    shortImgToImage(pImg, nW, nH, mPaintImg);

    mPaintRect.setRect(0, 0, nW, nH);

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
    else if (QObject::sender() == ui->pushButton_invert)
    {
        mBInvert = ui->pushButton_invert->isChecked();

        delImg();
    }
}

void RecognizeWdg::delImg()
{
    //É¾³ý
    delete []m_pImgPro;
    m_pImgPro = NULL;

    //¿½±´
    m_pImgPro = new unsigned short[mSrcImgWidth * mSrcImgHeight];
    memcpy(m_pImgPro, m_pSrcImg, mSrcImgWidth*mSrcImgHeight*sizeof(unsigned short));

    mCurImgWidth  = mSrcImgWidth;
    mCurImgHeight = mSrcImgHeight;

    //·´Ïà
    if (mBInvert)
        Invert(m_pImgPro, mCurImgWidth, mCurImgHeight);

    //resize
    reSize(mScale);

    //ÏÔÊ¾
    showImg(m_pImgPro, mCurImgWidth, mCurImgHeight);
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

                if (mPaintRect.width() <= nWidth || mPaintRect.height() <= nHeight)
                {
                    p.drawImage(QRect((nWidth-mPaintRect.width())/2,
                                      (nHeight-mPaintRect.height())/2,
                                      mPaintRect.width(),
                                      mPaintRect.height()), mPaintImg);
                }
                else
                    p.drawImage(QRect(0, 0, nWidth, nHeight), mPaintImg, mSourceRect);

                p.setPen(QColor("#ffff00"));
                p.setFont(QFont("Microsoft YaHei UI", 20));
                p.drawText(QPoint(20, 40), mCurDcmFileInfo.filePath);

                p.drawText(QPoint(20, ui->widget_img_pre->rect().bottom()-30),
                           QString("[%1 x %2]   %3%")
                           .arg(mCurDcmFileInfo.width)
                           .arg(mCurDcmFileInfo.height)
                           .arg(QString::number(mScale*100,'f',1)));

                return true;
            }
        }
    }


    return QWidget::eventFilter(obj, e);
}


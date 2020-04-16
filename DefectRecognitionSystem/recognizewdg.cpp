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

    mBInvert = false;
    mBFlip   = false;
    mBMirror = false;

    mSourceX = 0;
    mSourceY = 0;

    m_eCurAction = ARROWACTION;
    mBMeasureOpt = false;

    ui->verticalSlider_diameter->setRange(5, 200);


    //常规
    connect(ui->pushButton_measure, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_nomal, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_adapt, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));

    //调整
    connect(ui->pushButton_reset, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_invert, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_Mirror, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_Flip, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));
    connect(ui->pushButton_mag, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));


    connect(ui->verticalSlider_WinCentre, SIGNAL(valueChanged(int)), this, SLOT(slot_sliderWinValueChange(int)));
    connect(ui->verticalSlider_WindWidth, SIGNAL(valueChanged(int)), this, SLOT(slot_sliderWinValueChange(int)));


    connect(ui->pushButton_next_step, SIGNAL(clicked(bool)), this, SLOT(slotBtnClick(bool)));

    connect(ui->horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(slot_scrollAreaXChange(int)));
    connect(ui->verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(slot_scrollAreaYChange(int)));

    connect(ui->verticalSlider_diameter, SIGNAL(sliderReleased()), this, SLOT(slot_sliderReleased()));

    connect(ui->buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(slot_btnGroupClick(QAbstractButton*)));
}

void RecognizeWdg::slot_sliderReleased()
{
    int value = ui->verticalSlider_diameter->value();
    mScale = (float)value / 100;

    ui->lineEdit_diameter->setText(QString("%1").arg(value));

    delImg();
}

void RecognizeWdg::slot_sliderWinValueChange(int value)
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

    ui->verticalSlider_WinCentre->blockSignals(true);
    ui->verticalSlider_WindWidth->blockSignals(true);

    ui->verticalSlider_WinCentre->setValue(dcmInfo.winCentre);
    ui->verticalSlider_WindWidth->setValue(dcmInfo.windWidth);

    mWinCentre = dcmInfo.winCentre;
    mWinWidth  = dcmInfo.windWidth;


    qDebug() << mWinCentre <<":" << mWinWidth;

    ui->verticalSlider_WinCentre->blockSignals(false);
    ui->verticalSlider_WindWidth->blockSignals(false);


    std::thread([=] {

        dmfile.Load(dcmInfo.filePath.toLocal8Bit().toStdString().c_str());

        FunctionTransfer::runInMainThread([=]()
        {
            m_pSrcImg = dmfile.GetBuffer();
            mSrcImgWidth = dmfile.GetWidth();
            mSrcImgHeight = dmfile.GetHeight();

            m_pImgPro = new unsigned short[mSrcImgWidth * mSrcImgHeight];
            memcpy(m_pImgPro, m_pSrcImg, mSrcImgWidth*mSrcImgHeight*sizeof(unsigned short));

            this->showAdapt();
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

void RecognizeWdg::showAdapt()
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

void RecognizeWdg::resetImg()
{
    ui->pushButton_invert->setChecked(false);
    mBInvert = false;

    ui->pushButton_Mirror->setChecked(false);
    mBMirror = false;

    ui->pushButton_Flip->setChecked(false);
    mBFlip = false;

    //适配显示
    showAdapt();
}

void RecognizeWdg::showNomal()
{
    mScale = 1.0;

    ui->verticalSlider_diameter->setValue((int)(mScale*100));
    ui->lineEdit_diameter->setText(QString("%1").arg((int)(mScale*100)));

    delImg();
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

    qDebug() << "pre:" << mCurImgWidth << mCurImgHeight;

    Resize(m_pImgPro, srcW, srcH, mCurImgWidth, mCurImgHeight);

    mCurImgWidth = srcW;
    mCurImgHeight = srcH;

    qDebug() << "end:" << mCurImgWidth << mCurImgHeight;
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

		if (mPaintRect.height() <= nH)
			mSourceRect = QRect(mSourceX, mSourceY, nW, mPaintRect.height());
		else
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

		if (mPaintRect.width() <= nW)
			mSourceRect = QRect(mSourceX, mSourceY, mPaintRect.width(), nH);
		else 
			mSourceRect = QRect(mSourceX, mSourceY, nW, nH);


        update();
    }
}

void RecognizeWdg::slotBtnClick(bool bClick)
{
    if (QObject::sender() == ui->pushButton_measure)
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
    else if (QObject::sender() == ui->pushButton_next_step)
    {
        ui->tabWidget->setCurrentIndex(1);
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
}

void RecognizeWdg::delImg()
{
//    std::thread([=] {
        //删除
        delete []m_pImgPro;
        m_pImgPro = NULL;

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

        mBDelImging = false;

        FunctionTransfer::runInMainThread([=]()
        {
            //显示
            showImg(m_pImgPro, mCurImgWidth, mCurImgHeight);
        });

//    }).detach();

}


void RecognizeWdg::slot_btnGroupClick(QAbstractButton *btn)
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

void RecognizeWdg::updateCursor()
{
    if (m_eCurAction == HANDACTION && mBMeasureOpt)
    {
        setCursor(Qt::OpenHandCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
}

void RecognizeWdg::setScrollBarOffsetValues(int hValue, int vValue)
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


                if (mPaintRect.width() <= nWidth && mPaintRect.height() <= nHeight)
                {
                    p.drawImage(QRect((nWidth-mPaintRect.width())/2,
                                      (nHeight-mPaintRect.height())/2,
                                      mPaintRect.width(),
                                      mPaintRect.height()), mPaintImg);
                }
                else if (mPaintRect.width() <= nWidth && mPaintRect.height() > nHeight)
                {
                    p.drawImage(QRect((nWidth-mPaintRect.width())/2, 0,
                                      mPaintRect.width(),
                                      nHeight), mPaintImg, mSourceRect);
                }
                else if (mPaintRect.width() > nWidth && mPaintRect.height() <= nHeight)
                {
                    p.drawImage(QRect(0, (nHeight-mPaintRect.height())/2,
                                      nWidth,
                                      mPaintRect.height()), mPaintImg, mSourceRect);
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
        else if (e->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(e);
            QPoint pt = mouseEvent->pos();

            if (!mBMeasureOpt)
            {

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

//                        //清空选中item
//                        clearItemSelected(m_selectedIndex);

//                        //判断点击是否在图像区域内
//                        if (m_PaintRect.contains(wdgPtConvertToPaintPt(pt)))
//                        {
//                            m_beginPosPt = pt;

//                            m_eDrawStatus = BEGINDRAW;

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
//                        mousePressArrowAction(pt);
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

            }
            else
            {
                if (mouseEvent->button() == Qt::LeftButton)
                {
                    switch (m_eCurAction) {
                    case RECTACTION:
                    {
//                        if (m_rectTmp.isValid() && m_eDrawStatus == BEGINDRAW)
//                        {
//                            m_geometryItemBase = new RectItem(m_rectTmp);
//                            m_geometryItemBase->setColor(m_measureSetting.measureObjectSet.lineColor);
//                            m_geometryItemBase->setLineWidth(m_measureSetting.measureObjectSet.lineWidth);
//                            m_geometryItemBase->setPenStyle(m_measureSetting.measureObjectSet.penStyle);
//                            m_geometryItemBase->setCurLabelType(m_measureSetting.measureObjectSet.rectLabelType);

//                            m_geometryItemBase->setSpatialResolution(g_scanner.getSpatialResolution());
//                            m_geometryItemBase->setUnit(m_measureSetting.lengthUnit);

//                            m_geometryItemBase->calcOriGeometry(QPoint(m_PaintRect.topLeft().x(), m_PaintRect.topLeft().y()),
//                                                                m_img.width(),
//                                                                m_img.height(), m_scaleRate,
//                                                                0, false, false);

//                            RectItem *rectItem = (RectItem *)m_geometryItemBase;
//                            rectItem->setFillStatus(m_measureSetting.measureObjectSet.bFill);
//                            rectItem->setFillColor(m_measureSetting.measureObjectSet.fillColor);

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
//                        mouseReleaseArrowAction();
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

            }
            else
            {
                QPoint imgPt = pt;

                switch (m_eCurAction) {
                case RECTACTION:
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
//                    mouseMoveArrowAction(pt);
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
            if (mBMeasureOpt)
                setCursor(Qt::OpenHandCursor);
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
             obj == ui->verticalSlider_diameter)
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



    return QWidget::eventFilter(obj, e);
}


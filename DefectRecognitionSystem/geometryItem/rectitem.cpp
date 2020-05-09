#include "rectitem.h"

#include <QDebug>

RectItem::RectItem()
{
    m_eItemStatus = NORMOAL;

    m_fillColor = QColor(100,200,255);
    m_bFill  = false;

    m_labelTypList.append(NAMELABEL);
    m_labelTypList.append(AREALABEL);
    m_labelTypList.append(PERIMETERLABEL);
    m_labelTypList.append(WIDTHANDHEIGHTLABEL);
    m_labelTypList.append(NONELABEL);

    m_itemName = QObject::tr("Rectangle");
}

RectItem::~RectItem()
{

}

void RectItem::setFillStatus(bool bFill)
{
    m_bFill = bFill;
}

bool RectItem::getFillStatus()
{
    return m_bFill;
}

void RectItem::setFillColor(QColor color)
{
    m_fillColor = color;
}

QColor RectItem::getFillColor()
{
    return m_fillColor;
}

RectItem::RectItem(QRectF rect)
{
    m_eItemStatus = NORMOAL;

    setBoundingRect(rect);

    m_rect = rect;
    m_rectDragItemData.setRect(&m_rect.toRect());

    m_width = rect.width();
    m_height = rect.height();

    //
    m_fillColor = QColor(100,200,255);
    m_bFill  = false;

    m_labelTypList.append(NAMELABEL);
    m_labelTypList.append(AREALABEL);
    m_labelTypList.append(PERIMETERLABEL);
    m_labelTypList.append(WIDTHANDHEIGHTLABEL);
    m_labelTypList.append(NONELABEL);

    m_itemName = QObject::tr("Rectangle");
}

//
void RectItem::setRect(QRectF rect)
{
    setBoundingRect(rect);

    m_rect = rect;
    m_rectDragItemData.setRect(&m_rect.toRect());

    m_width = rect.width();
    m_height = rect.height();
}

void RectItem::changeToUnitType()
{
    if (m_lengthUnit == PIXELUNIT)
    {
        m_oriWidthUnit = m_oriWidth;
        m_oriHeightUnit = m_oriHeight;
    }
    else if (m_lengthUnit == NANOUNIT)
    {
        m_oriWidthUnit = m_oriWidth*m_spatialResolution*1000;
        m_oriHeightUnit = m_oriHeight*m_spatialResolution*1000;
    }
    else if (m_lengthUnit == MICUNIT)
    {
        m_oriWidthUnit = m_oriWidth*m_spatialResolution;
        m_oriHeightUnit = m_oriHeight*m_spatialResolution;
    }
    else if (m_lengthUnit == MILUNIT)
    {
        m_oriWidthUnit = m_oriWidth*m_spatialResolution/1000;
        m_oriHeightUnit = m_oriHeight*m_spatialResolution/1000;
    }
    else if (m_lengthUnit == CENTIUNIT)
    {
        m_oriWidthUnit = m_oriWidth*m_spatialResolution/10000;
        m_oriHeightUnit = m_oriHeight*m_spatialResolution/10000;
    }

    m_circumferenceUnit = 2 * (m_oriWidthUnit + m_oriHeightUnit);
    m_areaUnit = m_oriWidthUnit * m_oriHeightUnit;
}

void RectItem::calcOriGeometry(QPoint oriPt, int imgW, int imgH, float scale, int rotate, bool bFlip, bool bMirror)
{
    if (PRINT_DEBUG)
        qDebug() << "==================";
    //平移变换
    m_rectOri.setLeft(m_rect.left() - oriPt.x());
    m_rectOri.setRight(m_rect.right() - oriPt.x());
    m_rectOri.setTop(m_rect.top() - oriPt.y());
    m_rectOri.setBottom(m_rect.bottom() - oriPt.y());

    if (PRINT_DEBUG)
        qDebug() << "m_rect:" << m_rect << "m_rectOri" << m_rectOri << "oriPt" << oriPt;

    //缩放变换
    m_rectOri.setLeft((float)m_rectOri.left() / scale);
    m_rectOri.setRight((float)m_rectOri.right() / scale);
    m_rectOri.setTop((float)m_rectOri.top() / scale);
    m_rectOri.setBottom((float)m_rectOri.bottom() / scale);


    if (PRINT_DEBUG)
        qDebug() << "sssss:" << m_rectOri;


//--------------------------------------
    QRectF rectTempCpy;

    rectTempCpy = m_rectOri;
    int type = rotate / 90;

    //坐标变换， 图像中心坐标
    int imgCenterW = imgW / 2;
    int imgCenterH = imgH / 2;

    rectTempCpy.setLeft(rectTempCpy.left() - imgCenterW / scale);
    rectTempCpy.setRight(rectTempCpy.right() - imgCenterW / scale);
    rectTempCpy.setTop(rectTempCpy.top() - imgCenterH / scale);
    rectTempCpy.setBottom(rectTempCpy.bottom() - imgCenterH / scale);

    if (PRINT_DEBUG)
        qDebug() << "rectTempCpy:" << rectTempCpy << "imgCenterH" << imgCenterH / scale << "imgCenterW" << imgCenterW / scale;

    QRectF rectT;

    if (bMirror)
    {
        rectT = rectTempCpy;

        rectTempCpy.setLeft(-rectT.right());
        rectTempCpy.setRight(-rectT.left());
        rectTempCpy.setTop(rectT.top());
        rectTempCpy.setBottom(rectT.bottom());
    }

    if (bFlip)
    {
        rectT = rectTempCpy;

        rectTempCpy.setLeft(rectT.left());
        rectTempCpy.setRight(rectT.right());
        rectTempCpy.setTop(-rectT.bottom());
        rectTempCpy.setBottom(-rectT.top());
    }

    //旋转变换
    if (type == 1)
    {
        rectT = rectTempCpy;

        qDebug() << "topLeft:" << rectTempCpy.topLeft() << "bottomRight:"  <<  rectTempCpy.bottomRight();

        rectTempCpy.setLeft(-rectT.bottom());
        rectTempCpy.setRight(-rectT.top());
        rectTempCpy.setTop(rectT.left());
        rectTempCpy.setBottom(rectT.right());


        qDebug() << "topLeft:" << rectTempCpy.topLeft() << "bottomRight:"  <<  rectTempCpy.bottomRight();
    }
    else if (type == 2)
    {
        rectT = rectTempCpy;

        rectTempCpy.setLeft(-rectT.right());
        rectTempCpy.setRight(-rectT.left());
        rectTempCpy.setTop(-rectT.bottom());
        rectTempCpy.setBottom(-rectT.top());
    }
    else if (type == 3)
    {
        rectT = rectTempCpy;

        rectTempCpy.setLeft(-rectT.bottom());
        rectTempCpy.setRight(-rectT.top());
        rectTempCpy.setTop(rectT.left());
        rectTempCpy.setBottom(rectT.right());
    }

    if (PRINT_DEBUG)
        qDebug() << "rrrrrrrr:" << rectTempCpy;

    //转化为图像0点坐标
    if (type == 1 || type == 3)
    {
        rectTempCpy.setLeft(rectTempCpy.left() + imgCenterH / scale);
        rectTempCpy.setRight(rectTempCpy.right() + imgCenterH / scale);
        rectTempCpy.setTop(rectTempCpy.top() + imgCenterW / scale);
        rectTempCpy.setBottom(rectTempCpy.bottom() + imgCenterW / scale);
    }
    else {
        rectTempCpy.setLeft(rectTempCpy.left() + imgCenterW / scale);
        rectTempCpy.setRight(rectTempCpy.right() + imgCenterW / scale);
        rectTempCpy.setTop(rectTempCpy.top() + imgCenterH / scale);
        rectTempCpy.setBottom(rectTempCpy.bottom() + imgCenterH / scale);
    }

    m_rectOri = rectTempCpy;

    if (PRINT_DEBUG)
        qDebug() << "000000000:" << rectTempCpy;

    //更新、计算相关信息
    m_oriWidth = m_rectOri.width() / scale;
    m_oriHeight = m_rectOri.height()  / scale;

    m_circumference = 2 * (m_oriWidth + m_oriHeight);
    m_area = m_oriWidth * m_oriHeight;

    changeToUnitType();
}

void RectItem::updateGeometry(QPoint curOriPt, int imgW, int imgH, float scale,
                              int rotate, bool bFlip, bool bMirror)
{
    QRectF rectTempCpy;

    rectTempCpy = m_rectOri;

    if (PRINT_DEBUG)
        qDebug() << "-----------------------------------";
    if (PRINT_DEBUG)
        qDebug() << "m_rectOri:" << m_rectOri << "curOriPt" << curOriPt;

    //坐标变换， 图像中心坐标
    int imgCenterW = imgW / 2;
    int imgCenterH = imgH / 2;

    QRectF rectT;

    rectT = rectTempCpy;

    int type = rotate / 90;
    if (type == 1 || type == 3)
    {
        rectTempCpy.setLeft(rectTempCpy.left() - imgCenterH / scale);
        rectTempCpy.setRight(rectTempCpy.right() - imgCenterH / scale);
        rectTempCpy.setTop(rectTempCpy.top() - imgCenterW / scale);
        rectTempCpy.setBottom(rectTempCpy.bottom() - imgCenterW / scale);
    }
    else {
        rectTempCpy.setLeft(rectTempCpy.left() - imgCenterW / scale);
        rectTempCpy.setRight(rectTempCpy.right() - imgCenterW / scale);
        rectTempCpy.setTop(rectTempCpy.top() - imgCenterH / scale);
        rectTempCpy.setBottom(rectTempCpy.bottom() - imgCenterH / scale);
    }

    if (PRINT_DEBUG)
        qDebug() << "rectTempCpy:" << rectTempCpy<< "imgCenterW:" << imgCenterW / scale<< "imgCenterH:" << imgCenterH / scale;

    if (bMirror)
    {
        rectT = rectTempCpy;

        rectTempCpy.setLeft(-rectT.right());
        rectTempCpy.setRight(-rectT.left());
        rectTempCpy.setTop(rectT.top());
        rectTempCpy.setBottom(rectT.bottom());
    }

    if (bFlip)
    {
        rectT = rectTempCpy;

        rectTempCpy.setLeft(rectT.left());
        rectTempCpy.setRight(rectT.right());
        rectTempCpy.setTop(-rectT.bottom());
        rectTempCpy.setBottom( -rectT.top());
    }

    //旋转变换
    if (type == 1)
    {
        rectT = rectTempCpy;

        rectTempCpy.setLeft(rectT.top());
        rectTempCpy.setRight(rectT.bottom());
        rectTempCpy.setTop(-rectT.right());
        rectTempCpy.setBottom(-rectT.left());
    }
    else if (type == 2)
    {
        rectT = rectTempCpy;

        rectTempCpy.setLeft(-rectT.right());
        rectTempCpy.setRight(-rectT.left());
        rectTempCpy.setTop(-rectT.bottom());
        rectTempCpy.setBottom(-rectT.top());
    }
    else if (type == 3)
    {
        rectT = rectTempCpy;

        rectTempCpy.setLeft(-rectT.bottom());
        rectTempCpy.setRight(-rectT.top());
        rectTempCpy.setTop(rectT.left());
        rectTempCpy.setBottom(rectT.right());
    }

    if (PRINT_DEBUG)
        qDebug() << "rotate rectTempCpy:" << rectTempCpy;

    //转化为图像0点坐标
    rectTempCpy.setLeft(rectTempCpy.left()      + imgCenterW / scale);
    rectTempCpy.setRight(rectTempCpy.right()    + imgCenterW / scale);
    rectTempCpy.setTop(rectTempCpy.top()        + imgCenterH / scale);
    rectTempCpy.setBottom(rectTempCpy.bottom()  + imgCenterH / scale);

    if (PRINT_DEBUG)
        qDebug() << "00000 rectTempCpy:" << rectTempCpy;

    //把图像坐标系转窗口坐标系 ，需要注意的是 curOriPt 已经经过放大缩小
    QRectF rectTemp;

    //平移
    rectTemp.setLeft(rectTempCpy.left()     + curOriPt.x()/scale);
    rectTemp.setRight(rectTempCpy.right()   + curOriPt.x()/scale);
    rectTemp.setTop(rectTempCpy.top()       + curOriPt.y()/scale);
    rectTemp.setBottom(rectTempCpy.bottom() + curOriPt.y()/scale);

    if (PRINT_DEBUG)
        qDebug() << "mmove rectTemp:" << rectTemp;

    //缩放变换
    rectTemp.setLeft((float)rectTemp.left() * scale);
    rectTemp.setRight((float)rectTemp.right() * scale);
    rectTemp.setTop((float)rectTemp.top() * scale);
    rectTemp.setBottom((float)rectTemp.bottom() * scale);

    if (PRINT_DEBUG)
        qDebug() << "scale rectTemp:" << rectTemp << "scale:" << scale;

    //刷新矩阵
    setRect(rectTemp);
}

//get
float RectItem::getWidth()
{
    return m_width;
}

float RectItem::getHeight()
{
    return m_height;
}

QRectF RectItem::getRect()
{
    return m_rect;
}

QRectF RectItem::getOriRect()
{
    return m_rectOri;
}

float RectItem::getCircumference()
{
    return m_circumference;
}

float RectItem::getArea()
{
    return m_area;
}

float RectItem::getOriWidth()
{
    return m_oriWidth;
}

float RectItem::getOriHeight()
{
    return m_oriHeight;
}

float RectItem::getCircumferenceUnit()
{
    return m_circumferenceUnit;
}

float RectItem::getAreaUnit()
{
    return m_areaUnit;
}

float RectItem::getOriWidthUnit()
{
    return m_oriWidthUnit;
}

float RectItem::getOriHeightUnit()
{
    return m_oriHeightUnit;
}

RectDragItemData RectItem::getDragItemData()
{
    return m_rectDragItemData;
}

#include "ellipseitem.h"

EllipseItem::EllipseItem()
{
    m_eItemStatus = NORMOAL;

    m_fillColor = QColor(100,200,255);
    m_bFill  = false;

    m_labelTypList.append(NAMELABEL);
    m_labelTypList.append(AREALABEL);
    m_labelTypList.append(PERIMETERLABEL);
    m_labelTypList.append(MAJORANDMINORAXISLABEL);
    m_labelTypList.append(NONELABEL);

    m_itemName = QObject::tr("Ellipse");
}

EllipseItem::~EllipseItem()
{

}

void EllipseItem::setFillStatus(bool bFill)
{
    m_bFill = bFill;
}

bool EllipseItem::getFillStatus()
{
    return m_bFill;
}

void EllipseItem::setFillColor(QColor color)
{
    m_fillColor = color;
}

QColor EllipseItem::getFillColor()
{
    return m_fillColor;
}

EllipseItem::EllipseItem(QRectF rect)
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
    m_labelTypList.append(MAJORANDMINORAXISLABEL);
    m_labelTypList.append(NONELABEL);

    m_itemName = QObject::tr("Ellipse");
}

//
void EllipseItem::setRect(QRectF rect)
{
    setBoundingRect(rect);

    m_rect = rect;
    m_rectDragItemData.setRect(&m_rect.toRect());

    m_width = rect.width();
    m_height = rect.height();
}

void EllipseItem::changeToUnitType()
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

    if (m_oriWidthUnit > m_oriHeightUnit)
    {
        m_majorUnit = (float)m_oriWidthUnit / 2.0;
        m_minorUnit = (float)m_oriHeightUnit / 2.0;
    }
    else
    {
        m_majorUnit = (float)m_oriHeightUnit / 2.0;
        m_minorUnit = (float)m_oriWidthUnit / 2.0;
    }

    m_circumferenceUnit = 2 * PI * m_minorUnit + 4 * (m_majorUnit - m_minorUnit);
    m_areaUnit = PI * m_majorUnit * m_minorUnit;
}

void EllipseItem::calcOriGeometry(QPoint oriPt, int imgW, int imgH, float scale, int rotate, bool bFlip, bool bMirror)
{
    //平移变换
    m_rectOri.setLeft(m_rect.left() - oriPt.x());
    m_rectOri.setRight(m_rect.right() - oriPt.x());
    m_rectOri.setTop(m_rect.top() - oriPt.y());
    m_rectOri.setBottom(m_rect.bottom() - oriPt.y());


    //转化为图像的坐标系统，需要注意的是oriPt 已经经过放大缩小
    //缩放变换
    m_rectOri.setLeft((float)m_rectOri.left() / scale);
    m_rectOri.setRight((float)m_rectOri.right() / scale);
    m_rectOri.setTop((float)m_rectOri.top() / scale);
    m_rectOri.setBottom((float)m_rectOri.bottom() / scale);

//--------------------------------------
    QRectF rectTempCpy;

    rectTempCpy = m_rectOri;
    //坐标变换， 图像中心坐标
    int imgCenterW = imgW / 2;
    int imgCenterH = imgH / 2;

    rectTempCpy.setLeft(rectTempCpy.left() - imgCenterW / scale);
    rectTempCpy.setRight(rectTempCpy.right() - imgCenterW / scale);
    rectTempCpy.setTop(rectTempCpy.top() - imgCenterH / scale);
    rectTempCpy.setBottom(rectTempCpy.bottom() - imgCenterH / scale);


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
    int type = rotate / 90;
    if (type == 1)
    {
        rectT = rectTempCpy;

        rectTempCpy.setLeft(-rectT.bottom());
        rectTempCpy.setRight(-rectT.top());
        rectTempCpy.setTop(rectT.left());
        rectTempCpy.setBottom(rectT.right());

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


    //转化为图像0点坐标
    rectTempCpy.setLeft(rectTempCpy.left() + imgCenterW / scale);
    rectTempCpy.setRight(rectTempCpy.right() + imgCenterW / scale);
    rectTempCpy.setTop(rectTempCpy.top() + imgCenterH / scale);
    rectTempCpy.setBottom(rectTempCpy.bottom() + imgCenterH / scale);

    m_rectOri = rectTempCpy;

    //更新、计算相关信息
    m_oriWidth  = m_rectOri.width() / scale;
    m_oriHeight = m_rectOri.height()  / scale;

    changeToUnitType();
}

void EllipseItem::updateGeometry(QPoint curOriPt, int imgW, int imgH, float scale, int rotate, bool bFlip, bool bMirror)
{
    //把图像坐标系转窗口坐标系 ，需要注意的是 curOriPt 已经经过放大缩小
    QRectF rectTempCpy;

    rectTempCpy = m_rectOri;

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

    //转化为图像0点坐标
    rectTempCpy.setLeft(rectTempCpy.left()      + imgCenterW / scale);
    rectTempCpy.setRight(rectTempCpy.right()    + imgCenterW / scale);
    rectTempCpy.setTop(rectTempCpy.top()        + imgCenterH / scale);
    rectTempCpy.setBottom(rectTempCpy.bottom()  + imgCenterH / scale);

    QRectF rectTemp;

    //平移
    rectTemp.setLeft(rectTempCpy.left()     + curOriPt.x()/scale);
    rectTemp.setRight(rectTempCpy.right()   + curOriPt.x()/scale);
    rectTemp.setTop(rectTempCpy.top()       + curOriPt.y()/scale);
    rectTemp.setBottom(rectTempCpy.bottom() + curOriPt.y()/scale);

    //缩放变换
    rectTemp.setLeft((float)rectTemp.left() * scale);
    rectTemp.setRight((float)rectTemp.right() * scale);
    rectTemp.setTop((float)rectTemp.top() * scale);
    rectTemp.setBottom((float)rectTemp.bottom() * scale);

    //刷新矩阵
    setRect(rectTemp);
}

float EllipseItem::getSemiMajorAxisUnit()
{
    return m_majorUnit;
}

float EllipseItem::getSemiMinorAxisUnit()
{
    return m_minorUnit;
}

float EllipseItem::getOriWidthUnit()
{
    return m_oriWidthUnit;
}

float EllipseItem::getOriHeightUnit()
{
    return m_oriHeightUnit;
}

QRectF EllipseItem::getOriRect()
{
    return m_rectOri;
}

float EllipseItem::getCircumferenceUnit()
{
    return m_circumferenceUnit;
}

float EllipseItem::getAreaUnit()
{
    return m_areaUnit;
}

float EllipseItem::getOriWidth()
{
    return m_oriWidth;
}

float EllipseItem::getOriHeight()
{
    return m_oriHeight;
}

//get
float EllipseItem::getWidth()
{
    return m_width;
}

float   EllipseItem::getHeight()
{
    return m_height;
}

QRectF EllipseItem::getRect()
{
    return m_rect;
}

RectDragItemData EllipseItem::getDragItemData()
{
    return m_rectDragItemData;
}

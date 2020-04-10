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
    //转化为图像的坐标系统，需要注意的是oriPt 已经经过放大缩小
    //缩放变换
    m_rectOri.setLeft((float)m_rect.left() / scale);
    m_rectOri.setRight((float)m_rect.right() / scale);
    m_rectOri.setTop((float)m_rect.top() / scale);
    m_rectOri.setBottom((float)m_rect.bottom() / scale);

    //平移变换
    m_rectOri.setLeft(m_rectOri.left() - oriPt.x());
    m_rectOri.setRight(m_rectOri.right() - oriPt.x());
    m_rectOri.setTop(m_rectOri.top() - oriPt.y());
    m_rectOri.setBottom(m_rectOri.bottom() - oriPt.y());

    //更新、计算相关信息
    m_oriWidth  = m_rectOri.width();
    m_oriHeight = m_rectOri.height();

    changeToUnitType();
}

void EllipseItem::updateGeometry(QPoint curOriPt, int imgW, int imgH, float scale, int rotate, bool bFlip, bool bMirror)
{
    //把图像坐标系转窗口坐标系 ，需要注意的是 curOriPt 已经经过放大缩小
    QRectF rectTemp;

    //平移
    rectTemp.setLeft(m_rectOri.left() + curOriPt.x());
    rectTemp.setRight(m_rectOri.right() + curOriPt.x());
    rectTemp.setTop(m_rectOri.top() + curOriPt.y());
    rectTemp.setBottom(m_rectOri.bottom() + curOriPt.y());

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

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

    /*
//--------------------------------------
//    QRect rectTempCpy;

//    rectTempCpy = m_rectOri;
//    //坐标变换， 图像中心坐标
//    int imgCenterW = imgW / 2;
//    int imgCenterH = imgH / 2;

//    rectTempCpy.left   -= imgCenterW / scale;
//    rectTempCpy.right  -= imgCenterW / scale;
//    rectTempCpy.top	   -= imgCenterH / scale;
//    rectTempCpy.bottom -= imgCenterH / scale;

//    QRect rectT;

//    if (bFlip)
//    {
//        rectT = rectTempCpy;

//        rectTempCpy.left   =  -rectT.right;
//        rectTempCpy.right  =  -rectT.left;
//        rectTempCpy.top	   =  rectT.top;
//        rectTempCpy.bottom =  rectT.bottom;
//    }

//    if (bMirror)
//    {
//        rectT = rectTempCpy;

//        rectTempCpy.left   =  rectT.left;
//        rectTempCpy.right  =  rectT.right;
//        rectTempCpy.top	   =  -rectT.bottom;
//        rectTempCpy.bottom =  -rectT.top;
//    }

//    //旋转变换
//    int type = rotate / 90;
//    if (type == 1)
//    {
//        rectT = rectTempCpy;

//        rectTempCpy.left   =  rectT.top;
//        rectTempCpy.right  =  rectT.bottom;
//        rectTempCpy.top	   =  -rectT.right;
//        rectTempCpy.bottom =  -rectT.left;
//    }
//    else if (type == 2)
//    {
//        rectT = rectTempCpy;

//        rectTempCpy.left   =  -rectT.right;
//        rectTempCpy.right  =  -rectT.left;
//        rectTempCpy.top	   =  -rectT.bottom;
//        rectTempCpy.bottom =  -rectT.top;
//    }
//    else if (type == 3)
//    {
//        rectT = rectTempCpy;

//        rectTempCpy.left   =  -rectT.bottom;
//        rectTempCpy.right  =  -rectT.top;
//        rectTempCpy.top	   =  rectT.left;
//        rectTempCpy.bottom =  rectT.right;
//    }

//    //转化为图像0点坐标
//    rectTempCpy.left	+= imgCenterW / scale;
//    rectTempCpy.right  += imgCenterW / scale;
//    rectTempCpy.top	   += imgCenterH / scale;
//    rectTempCpy.bottom += imgCenterH / scale;

    //
//    m_rectOri = rectTempCpy;
    */

    //更新、计算相关信息
    m_oriWidth = m_rectOri.width();
    m_oriHeight = m_rectOri.height();

    m_circumference = 2 * (m_oriWidth + m_oriHeight);
    m_area = m_oriWidth * m_oriHeight;

    changeToUnitType();
}

void RectItem::updateGeometry(QPoint curOriPt, int imgW, int imgH, float scale, int rotate, bool bFlip, bool bMirror)
{
   /*
     QRect rectTempCpy;

    rectTempCpy = m_rectOri;

    //坐标变换， 图像中心坐标
    int imgCenterW = imgW / 2;
    int imgCenterH = imgH / 2;

    rectTempCpy.left	-= imgCenterW / scale;
    rectTempCpy.right   -= imgCenterW / scale;
    rectTempCpy.top		-= imgCenterH / scale;
    rectTempCpy.bottom  -= imgCenterH / scale;

    QRect rectT;

    if (bFlip)
    {
        rectT = rectTempCpy;

        rectTempCpy.left	  =  -rectT.right;
        rectTempCpy.right     =  -rectT.left;
        rectTempCpy.top	      =  rectT.top;
        rectTempCpy.bottom    =  rectT.bottom;
    }

    if (bMirror)
    {
        rectT = rectTempCpy;

        rectTempCpy.left	  =  rectT.left;
        rectTempCpy.right     =  rectT.right;
        rectTempCpy.top	      =  -rectT.bottom;
        rectTempCpy.bottom    =  -rectT.top;
    }

    //旋转变换
    int type = rotate / 90;
    if (type == 1)
    {
        rectT = rectTempCpy;

        rectTempCpy.left	=  -rectT.bottom;
        rectTempCpy.right   =  -rectT.top;
        rectTempCpy.top		=  rectT.left;
        rectTempCpy.bottom  =  rectT.right;
    }
    else if (type == 2)
    {
        rectT = rectTempCpy;

        rectTempCpy.left   =  -rectT.right;
        rectTempCpy.right  =  -rectT.left;
        rectTempCpy.top	   =  -rectT.bottom;
        rectTempCpy.bottom =  -rectT.top;
    }
    else if (type == 3)
    {
        rectT = rectTempCpy;

        rectTempCpy.left	=  rectT.top;
        rectTempCpy.right  =  rectT.bottom;
        rectTempCpy.top	   =  -rectT.right;
        rectTempCpy.bottom =  -rectT.left;
    }

    //转化为图像0点坐标
    rectTempCpy.left	+= imgCenterW / scale;
    rectTempCpy.right   += imgCenterW / scale;
    rectTempCpy.top	    += imgCenterH / scale;
    rectTempCpy.bottom  += imgCenterH / scale;
    */

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

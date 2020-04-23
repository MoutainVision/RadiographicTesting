#include "lineitem.h"
#include <QPolygonF>

LineItem::LineItem()
{
    m_eItemStatus = NORMOAL;

    m_labelTypList.append(NAMELABEL);
    m_labelTypList.append(LENGTHLABEL);
    m_labelTypList.append(NONELABEL);

    m_itemName = QObject::tr("Line");
}

LineItem::~LineItem()
{

}

LineItem::LineItem(QLineF line)
{
    m_eItemStatus = NORMOAL;

    m_labelTypList.append(NAMELABEL);
    m_labelTypList.append(LENGTHLABEL);
    m_labelTypList.append(NONELABEL);

    calcBoundingRect(line);

    m_line = line;
    m_lineDragItemData.setLine(&m_line);

    m_itemName = QObject::tr("Line");
}

void LineItem::changeToUnitType()
{
    if (m_lengthUnit == PIXELUNIT)
    {
        m_lengthUunit = m_length;
    }
    else if (m_lengthUnit == NANOUNIT)
    {
        m_lengthUunit = m_length*m_spatialResolution*1000;
    }
    else if (m_lengthUnit == MICUNIT)
    {
        m_lengthUunit = m_length*m_spatialResolution;
    }
    else if (m_lengthUnit == MILUNIT)
    {
        m_lengthUunit = m_length*m_spatialResolution/1000;
    }
    else if (m_lengthUnit == CENTIUNIT)
    {
        m_lengthUunit = m_length*m_spatialResolution/10000;
    }
}

void LineItem::calcOriGeometry(QPoint oriPt, int imgW, int imgH, float scale, int rotate, bool bFlip, bool bMirror)
{
    int type = rotate / 90;

    //平移变换
    m_lineOri.setP1(QPointF(m_line.p1().x() - oriPt.x(), m_line.p1().y() - oriPt.y()));
    m_lineOri.setP2(QPointF(m_line.p2().x() - oriPt.x(), m_line.p2().y() - oriPt.y()));



    //缩放变换
    m_lineOri.setP1(m_lineOri.p1() / scale);
    m_lineOri.setP2(m_lineOri.p2() / scale);

    //转化为图像的坐标系统，需要注意的是oriPt 已经经过放大缩小
    QLineF              lineTemp;
    lineTemp = m_lineOri;
    //坐标变换， 图像中心坐标
    int imgCenterW = imgW / 2;
    int imgCenterH = imgH / 2;

//    if (type == 1 || type == 3)
//    {
//    lineTemp.setP1(QPointF(lineTemp.p1().x() - imgCenterH / scale,
//                           lineTemp.p1().y() - imgCenterW / scale));

//    lineTemp.setP2(QPointF(lineTemp.p2().x() - imgCenterH / scale,
//                           lineTemp.p2().y() - imgCenterW / scale));
//    }
//    else {
        lineTemp.setP1(QPointF(lineTemp.p1().x() - imgCenterW / scale,
                               lineTemp.p1().y() - imgCenterH / scale));

        lineTemp.setP2(QPointF(lineTemp.p2().x() - imgCenterW / scale,
                               lineTemp.p2().y() - imgCenterH / scale));
//    }


    if (bMirror)
    {
        lineTemp.setP1(QPointF(lineTemp.p1().x(),
                               -lineTemp.p1().y()));

        lineTemp.setP2(QPointF(lineTemp.p2().x(),
                               -lineTemp.p2().y()));
    }

    if (bFlip)
    {
        lineTemp.setP1(QPointF(-lineTemp.p1().x(),
                               lineTemp.p1().y()));

        lineTemp.setP2(QPointF(-lineTemp.p2().x(),
                               lineTemp.p2().y()));
    }


    if (type == 1)
    {
        lineTemp.setP1(QPointF(-lineTemp.p2().y(),
                               lineTemp.p1().x()));

        lineTemp.setP2(QPointF(-lineTemp.p1().y(),
                               lineTemp.p2().x()));

    }
    else if (type == 2)
    {
        lineTemp.setP1(QPointF(-lineTemp.p1().x(),
                               -lineTemp.p1().y()));

        lineTemp.setP2(QPointF(-lineTemp.p2().x(),
                               -lineTemp.p2().y()));
    }
    else if (type == 3)
    {
        lineTemp.setP1(QPointF(-lineTemp.p1().y(),
                               lineTemp.p1().x()));

        lineTemp.setP2(QPointF(-lineTemp.p2().y(),
                               lineTemp.p2().x()));
    }

    if (type == 1 || type == 3)
    {
        lineTemp.setP1(QPointF(lineTemp.p1().x() + imgCenterH / scale,
                               lineTemp.p1().y() + imgCenterW / scale));

        lineTemp.setP2(QPointF(lineTemp.p2().x() + imgCenterH / scale,
                               lineTemp.p2().y() + imgCenterW / scale));
    }
    else {
        lineTemp.setP1(QPointF(lineTemp.p1().x() + imgCenterW / scale,
                               lineTemp.p1().y() + imgCenterH / scale));

        lineTemp.setP2(QPointF(lineTemp.p2().x() + imgCenterW / scale,
                               lineTemp.p2().y() + imgCenterH / scale));
    }


    m_lineOri = lineTemp;

    //更新、计算相关信息
    m_length = m_lineOri.length() / scale;
    m_angle  = m_lineOri.angle() / scale;

    changeToUnitType();
}

void LineItem::updateGeometry(QPoint curOriPt, int imgW, int imgH, float scale, int rotate, bool bFlip, bool bMirror)
{
    QLineF lineTempCpy;

    lineTempCpy = m_lineOri;

    //坐标变换， 图像中心坐标
    int imgCenterW = imgW / 2;
    int imgCenterH = imgH / 2;

    int type = rotate / 90;
    if (type == 1 || type == 3)
    {
        lineTempCpy.setP1(QPointF(lineTempCpy.p1().x() - imgCenterH / scale,
                               lineTempCpy.p1().y() - imgCenterW / scale));

        lineTempCpy.setP2(QPointF(lineTempCpy.p2().x() - imgCenterH / scale,
                               lineTempCpy.p2().y() - imgCenterW / scale));

    }
    else {
        lineTempCpy.setP1(QPointF(lineTempCpy.p1().x() - imgCenterW / scale,
                               lineTempCpy.p1().y() - imgCenterH / scale));

        lineTempCpy.setP2(QPointF(lineTempCpy.p2().x() - imgCenterW / scale,
                               lineTempCpy.p2().y() - imgCenterH / scale));

    }

    if (bFlip)
    {
        lineTempCpy.setP1(QPointF(lineTempCpy.p1().x(),
                               -lineTempCpy.p1().y()));

        lineTempCpy.setP2(QPointF(lineTempCpy.p2().x(),
                               -lineTempCpy.p2().y()));
    }

    if (bMirror)
    {
        lineTempCpy.setP1(QPointF(-lineTempCpy.p1().x(),
                               lineTempCpy.p1().y()));

        lineTempCpy.setP2(QPointF(-lineTempCpy.p2().x(),
                               lineTempCpy.p2().y()));
    }

    if (type == 1)
    {
        lineTempCpy.setP1(QPointF(lineTempCpy.p1().y(),
                               -lineTempCpy.p1().x()));

        lineTempCpy.setP2(QPointF(lineTempCpy.p2().y(),
                               -lineTempCpy.p2().x()));

    }
    else if (type == 2)
    {
        lineTempCpy.setP1(QPointF(-lineTempCpy.p1().x(),
                               -lineTempCpy.p1().y()));

        lineTempCpy.setP2(QPointF(-lineTempCpy.p2().x(),
                               -lineTempCpy.p2().y()));
    }
    else if (type == 3)
    {
        lineTempCpy.setP1(QPointF(-lineTempCpy.p1().y(),
                               lineTempCpy.p1().x()));

        lineTempCpy.setP2(QPointF(-lineTempCpy.p2().y(),
                               lineTempCpy.p2().x()));
    }

    lineTempCpy.setP1(QPointF(lineTempCpy.p1().x() + imgCenterW / scale,
                           lineTempCpy.p1().y() + imgCenterH / scale));

    lineTempCpy.setP2(QPointF(lineTempCpy.p2().x() + imgCenterW / scale,
                           lineTempCpy.p2().y() + imgCenterH / scale));


    //把图像坐标系转窗口坐标系 ，需要注意的是 curOriPt 已经经过放大缩小
    QLineF lineTemp;

    //平移
    lineTemp.setP1(QPointF(lineTempCpy.p1().x() + curOriPt.x()/scale,
                           lineTempCpy.p1().y() + curOriPt.y()/scale));

    lineTemp.setP2(QPointF(lineTempCpy.p2().x() + curOriPt.x()/scale,
                           lineTempCpy.p2().y() + curOriPt.y()/scale));

    //缩放变换
    lineTemp.setP1(lineTemp.p1() * scale);
    lineTemp.setP2(lineTemp.p2() * scale);

    //刷新矩阵
    setLine(lineTemp);
}

void LineItem::calcBoundingRect(QLineF &line)
{
    QPolygonF poligonf;

    int dX = line.dx();
    int dY = line.dy();

    int a , b;
    a = dX >= dY ? dX : dY;
    b = dX <= dY ? dX : dY;

    float smallAngle = atan2((double)b, (double)a) ;
    float bigAngle = PI/2 - smallAngle;

//    角度的对边=斜边*sin角度
//    角度邻边=斜边*cos角度

    //第一个三角形
    float nearSide1 = DISTANCE;                         //邻边
    float slopeSide1 = DISTANCE / cos(smallAngle);      //斜边
    float faceSide1 = slopeSide1 * sin(smallAngle);     //角度的对边

    //第二个三角形
    float slopeSide2 = DISTANCE - faceSide1;
    float nearSide2 = slopeSide2 * cos(bigAngle);
    float faceSide2 = slopeSide2 * sin(bigAngle);

    if (dX >= dY)
    {
        QPointF aPt((float)(line.p1().x()-faceSide2), (float)(line.p1().y()-(slopeSide1 + nearSide2)));
        QPointF bPt((float)(line.p1().x()-(slopeSide1 + nearSide2)), (float)(line.p1().y()+faceSide2));
        QPointF cPt((float)(line.p2().x()+faceSide2), (float)(line.p2().y()+slopeSide1 + nearSide2));
        QPointF dPt((float)(line.p2().x()+slopeSide1 + nearSide2), (float)(line.p2().y()-faceSide2));

        poligonf << aPt << bPt << cPt << dPt;
    }
    else
    {
        QPointF aPt((float)(line.p1().x()+faceSide2), (float)(line.p1().y()-(slopeSide1 + nearSide2)));
        QPointF bPt((float)(line.p1().x()-(slopeSide1 + nearSide2)), (float)(line.p1().y()-faceSide2));
        QPointF cPt((float)(line.p2().x()-faceSide2), (float)(line.p2().y()+slopeSide1 + nearSide2));
        QPointF dPt((float)(line.p2().x()+slopeSide1 + nearSide2), (float)(line.p2().y()+faceSide2));

        poligonf << aPt << bPt << cPt << dPt;
    }

    setBoundingPolygonf(poligonf);
}

//
void LineItem::setLine(QLineF line)
{
    calcBoundingRect(line);

    m_line = line;
    m_lineDragItemData.setLine(&m_line);
}

//
QLineF LineItem::getLine()
{
    return m_line;
}

QLineF LineItem::getOriLine()
{
    return m_lineOri;
}

float LineItem::getLength()
{
    return m_length;
}

float LineItem::getLengthUnit()
{
    return m_lengthUunit;
}

QPoint LineItem::getCenterPt()
{
    QPointF pt1 = m_lineOri.p1();
    QPointF pt2 = m_lineOri.p2();

    return QPoint(pt1.x() + (pt2.x()-pt1.x())/2.0, pt1.y() + (pt2.y()-pt1.y())/2.0);
}

float LineItem::getAngle()
{
    return m_angle;
}

LineDragItemData LineItem::getDragItemData()
{
    return m_lineDragItemData;
}


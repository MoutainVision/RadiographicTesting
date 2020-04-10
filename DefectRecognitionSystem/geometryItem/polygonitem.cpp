#include "polygonitem.h"

#include <QDebug>

PolygonItem::PolygonItem()
{
    m_eItemStatus = NORMOAL;
}

PolygonItem::~PolygonItem()
{

}

PolygonItem::PolygonItem(QPolygonF polygonf)
{
    m_eItemStatus = NORMOAL;

    setBoundingRect(polygonf.boundingRect());

    m_dPolygonf = polygonf;

    m_rect = polygonf.boundingRect();

    m_rectDragItemData.setRect(&m_rect.toRect());

    m_width = polygonf.boundingRect().width();
    m_height = polygonf.boundingRect().height();
}

//
void PolygonItem::setPolygonF(QPolygonF polygonf)
{
    setBoundingRect(polygonf.boundingRect());

    m_dPolygonf = polygonf;

    m_rect = polygonf.boundingRect();

    m_rectDragItemData.setRect(&m_rect.toRect());

    m_width = polygonf.boundingRect().width();
    m_height = polygonf.boundingRect().height();
}

void PolygonItem::updateBoundingRect(QRectF rect)
{
//    m_rectDragItemData.setRect(&m_dPolygonf.boundingRect());
//    m_rectDragItemData.setRect(&rect);

    m_width = rect.width();
    m_height = rect.height();

    m_rect = rect;

    m_rectDragItemData.setRect(&m_rect.toRect());

    QRectF preRect = getBoundingRectF();

    QPainterPath path;
    path.addPolygon(m_dPolygonf);

    //重新计算
    qreal xScale = (float)rect.width() / (float)preRect.width();
    qreal yScale = (float)rect.height() / (float)preRect.height();

    if (xScale <= 0.05 || yScale <= 0.05)
        return ;

    QTransform t;
    t.scale(xScale, yScale);

//    qDebug() << "xScale:" <<xScale << "yScale: "<<yScale << "m_dPolygonf:" << m_dPolygonf;


    m_dPolygonf = path.toFillPolygon(t);

    QRectF rectTmp = m_dPolygonf.boundingRect();

    qreal sx = rectTmp.x() - rect.x();
    qreal sy = rectTmp.y() - rect.y();

    QTransform t1;
    t1.translate(-sx, -sy);

    QPainterPath path1;
    path1.addPolygon(m_dPolygonf);
    m_dPolygonf = path1.toFillPolygon(t1);

//     qDebug() << "m_dPolygonf:" << m_dPolygonf;


    setBoundingRect(m_dPolygonf.boundingRect());

//    emit sig_updateInfo();
}

//get
float  PolygonItem::getWidth()
{
    return m_width;
}

float PolygonItem::getHeight()
{
    return m_height;
}

QPolygonF PolygonItem::getDataPolygonf()
{
    return m_dPolygonf;
}

RectDragItemData PolygonItem::getDragItemData()
{
    return m_rectDragItemData;
}

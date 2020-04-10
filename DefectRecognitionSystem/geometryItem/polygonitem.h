#ifndef TEXTITEM_H
#define TEXTITEM_H

#include "rectdragitemdata.h"
#include "scanglobal.h"
#include "geometryitembase.h"

#include <QPolygonF>


#include "messagedlg.h"

class PolygonItem : public GeometryItemBase
{
public:
    explicit PolygonItem();
    explicit PolygonItem(QPolygonF polygonf);

    ~PolygonItem();

    //
    void setPolygonF(QPolygonF polygonf);

    void updateBoundingRect(QRectF rect);

    //get
    QPolygonF getDataPolygonf();
    RectDragItemData getDragItemData();
    float   getWidth();
    float   getHeight();

private:
    QPolygonF           m_dPolygonf;
    RectDragItemData    m_rectDragItemData;

    QRectF              m_rect;

    float                 m_width;
    float                 m_height;
};

#endif // TEXTITEM_H

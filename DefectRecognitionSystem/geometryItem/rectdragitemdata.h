#ifndef RECTDRAGITEMDATA_H
#define RECTDRAGITEMDATA_H

#include <QPoint>
#include <QRect>
#include <QList>

#include "dragitem.h"

#include "messagedlg.h"

class RectDragItemData
{
public:
    RectDragItemData();
    ~RectDragItemData();

    void setRect(QRect *rect);
//    void setRect(QRectF *rectf);

    //**
    QList<QRect> getDragItemRects();
    QList<DragItem> getDragItems();

    void updateDragItem();

private:
    DragItem    m_dragItem[4];

    QRect       *m_rect;

};

#endif // RECTDRAGITEMDATA_H

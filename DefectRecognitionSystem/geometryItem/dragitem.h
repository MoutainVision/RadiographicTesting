#ifndef DRAGITEM_H
#define DRAGITEM_H

#include <QPoint>
#include <QRect>

#include "scanglobal.h"

class DragItem
{
public:
    explicit DragItem();
    ~DragItem();

    void setDragPt(QPoint &pt);
    void setDragDirection(DragDirection direction);

    DragDirection getDragDirection();
    QPoint getDragPt();
    QRect getDragRect();

private:
    QPoint              m_dragPt;
    QRect               m_dragRect;

    DragDirection       m_eDragDirection;

};

#endif // DRAGITEM_H

#include "dragitem.h"

DragItem::DragItem()
{
    m_eDragDirection = NONEDIR;
}

DragItem::~DragItem()
{

}

void DragItem::setDragDirection(DragDirection direction)
{
    m_eDragDirection = direction;
}

DragDirection DragItem::getDragDirection()
{
    return m_eDragDirection;
}

void DragItem::setDragPt(QPoint &pt)
{
    m_dragPt = pt;

    if (g_ctrMag == 1)
        m_dragRect = QRect(m_dragPt.x()-4, m_dragPt.y()-4, 8, 8);
    else
        m_dragRect = QRect(m_dragPt.x()-8, m_dragPt.y()-8, 16, 16);
}

QPoint DragItem::getDragPt()
{
    return m_dragPt;
}

QRect DragItem::getDragRect()
{
    return m_dragRect;
}



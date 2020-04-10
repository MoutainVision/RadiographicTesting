#include "linedragitemdata.h"

LineDragItemData::LineDragItemData()
{

}

LineDragItemData::~LineDragItemData()
{

}

void LineDragItemData::setLine(QLineF *line)
{
    m_line = line;


    if (m_line->p1().x() <= m_line->p2().x())
    {
        m_dragItem[0].setDragPt(m_line->p1().toPoint());
        m_dragItem[0].setDragDirection(DragDirection::LEFT);

        m_dragItem[1].setDragPt(m_line->p2().toPoint());
        m_dragItem[1].setDragDirection(DragDirection::RIGHT);
    }
    else
    {
        m_dragItem[0].setDragPt(m_line->p2().toPoint());
        m_dragItem[0].setDragDirection(DragDirection::RIGHT);

        m_dragItem[1].setDragPt(m_line->p1().toPoint());
        m_dragItem[1].setDragDirection(DragDirection::LEFT);
    }
}

//
QList<QRect> LineDragItemData::getDragItemRects()
{
    QList<QRect> rectList;

    rectList.push_back(m_dragItem[0].getDragRect());
    rectList.push_back(m_dragItem[1].getDragRect());

    return rectList;
}

QList<DragItem> LineDragItemData::getDragItems()
{
    QList<DragItem> dragItems;

    dragItems.push_back(m_dragItem[0]);
    dragItems.push_back(m_dragItem[1]);

    return dragItems;
}

//
void LineDragItemData::updateDragItem()
{
    m_dragItem[0].setDragPt(m_line->p1().toPoint());
    m_dragItem[1].setDragPt(m_line->p2().toPoint());
}


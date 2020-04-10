#include "rectdragitemdata.h"

RectDragItemData::RectDragItemData()
{

}

RectDragItemData::~RectDragItemData()
{

}

//void RectDragItemData::setRect(QRectF *rectf)
//{
//    //error api
////    m_rect = &rectf->toRect();
//    m_rect = rectf->toRect();

//    m_dragItem[0].setDragPt(m_rect->topLeft());
//    m_dragItem[0].setDragDirection(LEFTTOP);

//    m_dragItem[1].setDragPt(m_rect->topRight());
//    m_dragItem[1].setDragDirection(RIGHTTOP);

//    m_dragItem[2].setDragPt(m_rect->bottomLeft());
//    m_dragItem[2].setDragDirection(LEFTBOTTOM);

//    m_dragItem[3].setDragPt(m_rect->bottomRight());
//    m_dragItem[3].setDragDirection(RIGHTBOTTOM);
//}

void RectDragItemData::setRect(QRect *rect)
{
    m_rect = rect;

    m_dragItem[0].setDragPt(m_rect->topLeft());
    m_dragItem[0].setDragDirection(LEFTTOP);

    m_dragItem[1].setDragPt(m_rect->topRight());
    m_dragItem[1].setDragDirection(RIGHTTOP);

    m_dragItem[2].setDragPt(m_rect->bottomLeft());
    m_dragItem[2].setDragDirection(LEFTBOTTOM);

    m_dragItem[3].setDragPt(m_rect->bottomRight());
    m_dragItem[3].setDragDirection(RIGHTBOTTOM);
}

void RectDragItemData::updateDragItem()
{
    m_dragItem[0].setDragPt(m_rect->topLeft());
    m_dragItem[1].setDragPt(m_rect->topRight());
    m_dragItem[2].setDragPt(m_rect->bottomLeft());
    m_dragItem[3].setDragPt(m_rect->bottomRight());
}

QList<DragItem> RectDragItemData::getDragItems()
{
    QList<DragItem> dragItems;

    dragItems.push_back(m_dragItem[0]);
    dragItems.push_back(m_dragItem[1]);
    dragItems.push_back(m_dragItem[2]);
    dragItems.push_back(m_dragItem[3]);

    return dragItems;
}

QList<QRect> RectDragItemData::getDragItemRects()
{
    QList<QRect> rectList;

    rectList.push_back(m_dragItem[0].getDragRect());
    rectList.push_back(m_dragItem[1].getDragRect());
    rectList.push_back(m_dragItem[2].getDragRect());
    rectList.push_back(m_dragItem[3].getDragRect());

    return rectList;
}


#ifndef LINEDRAGITEMDATA_H
#define LINEDRAGITEMDATA_H

#include <QPoint>
#include <QRect>
#include <QList>

#include "dragitem.h"

class LineDragItemData
{
public:
    LineDragItemData();
    ~LineDragItemData();

    void setLine(QLineF *line);

    //
    QList<QRect> getDragItemRects();
    QList<DragItem> getDragItems();

    //
    void updateDragItem();

private:
    DragItem     m_dragItem[2];

    QLineF        *m_line;
};

#endif // LINEDRAGITEMDATA_H

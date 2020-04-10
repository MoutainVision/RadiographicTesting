#ifndef LINEITEM_H
#define LINEITEM_H

#include <qmath.h>

#include "scanglobal.h"
#include "geometryitembase.h"
#include "linedragitemdata.h"

#define WIDTHTIMES  5
#define DISTANCE    5

class LineItem : public GeometryItemBase
{
public:
    explicit LineItem();
    explicit LineItem(QLineF line);

    ~LineItem();

    void calcBoundingRect(QLineF &line);

    void changeToUnitType();

    void calcOriGeometry(QPoint oriPt, int imgW, int imgH, float scale, int rotate, bool bFlip, bool bMirror);

    void updateGeometry(QPoint curOriPt, int imgW, int imgH, float scale, int rotate, bool bFlip, bool bMirror);

    //
    void setLine(QLineF line);

    //
    QLineF getLine();
    LineDragItemData getDragItemData();

    QLineF getOriLine();

    float getLength();     
    float getAngle();
    QPoint getCenterPt();

    float getLengthUnit();

private:
    QLineF              m_line;

    LineDragItemData    m_lineDragItemData;

    QLineF              m_lineOri; 

    float               m_length;
    float               m_angle;

    float               m_lengthUunit;
};

#endif // LINEITEM_H

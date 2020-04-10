#ifndef ELLIPSEITEM_H
#define ELLIPSEITEM_H

#include "rectdragitemdata.h"
#include "scanglobal.h"
#include "geometryitembase.h"

class EllipseItem : public GeometryItemBase
{
public:
    explicit EllipseItem();
    explicit EllipseItem(QRectF rect);

    ~EllipseItem();

    void setFillStatus(bool bFill);
    bool getFillStatus();

    void setFillColor(QColor color);
    QColor getFillColor();

    void changeToUnitType();

    //
    void setRect(QRectF rect);

    //设置坐标系（计算原始几何图形）
    void calcOriGeometry(QPoint oriPt, int imgW, int imgH, float scale, int rotate, bool bFlip, bool bMirror);

    //做完变换刷新几何图形（获得当前几何图形）
    void updateGeometry(QPoint curOriPt, int imgW, int imgH, float scale, int rotate, bool bFlip, bool bMirror);

    //
    QRectF getRect();
    RectDragItemData getDragItemData();
    float   getWidth();
    float getHeight();

    QRectF getOriRect();

    float getOriWidth();		//宽
    float getOriHeight();       //高

    float getCircumferenceUnit();	//周长
    float getAreaUnit();			//面积
    float getSemiMajorAxisUnit();	//长半轴
    float getSemiMinorAxisUnit();	//短半轴
    float getOriWidthUnit();		//宽
    float getOriHeightUnit();       //高

private:
    bool                m_bFill;
    QColor              m_fillColor;

    QRectF               m_rect;
    RectDragItemData     m_rectDragItemData;

    float               m_width;
    float               m_height;

    QRectF				m_rectOri;	//原始矩形

    float               m_oriWidth;
    float               m_oriHeight;

    float				m_majorUnit;
    float				m_minorUnit;
    float				m_circumferenceUnit;
    float				m_areaUnit;
    float               m_oriWidthUnit;
    float               m_oriHeightUnit;
};

#endif // ELLIPSEITEM_H

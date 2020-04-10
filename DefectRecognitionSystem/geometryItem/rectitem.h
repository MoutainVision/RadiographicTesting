#ifndef RECTITEM_H
#define RECTITEM_H

#include "rectdragitemdata.h"
#include "scanglobal.h"
#include "geometryitembase.h"

class RectItem : public GeometryItemBase
{
public:
    explicit RectItem();
    explicit RectItem(QRectF rect);

    ~RectItem();

    void setFillStatus(bool bFill);
    bool getFillStatus();

    void setFillColor(QColor color);
    QColor getFillColor();

    virtual void changeToUnitType();

    //
    void setRect(QRectF rect);

    //设置坐标系（计算原始几何图形）
    void calcOriGeometry(QPoint oriPt, int imgW, int imgH, float scale, int rotate, bool bFlip, bool bMirror);

    //做完变换刷新几何图形（获得当前几何图形）
    void updateGeometry(QPoint curOriPt, int imgW, int imgH, float scale, int rotate, bool bFlip, bool bMirror);

    //get
    QRectF getRect();
    RectDragItemData getDragItemData();
    float   getWidth();
    float   getHeight();

    QRectF getOriRect();

    float getCircumference();	//周长
    float getArea();			//面积
    float getOriWidth();		//宽
    float getOriHeight();       //高

    float getCircumferenceUnit();	//周长
    float getAreaUnit();			//面积
    float getOriWidthUnit();		//宽
    float getOriHeightUnit();       //高

private:
    //
    QRectF				m_rectOri;	//原始矩形

    bool                m_bFill;
    QColor              m_fillColor;

    QRectF              m_rect;
    RectDragItemData    m_rectDragItemData;

    float               m_width;
    float               m_height;

    float				m_circumference;
    float				m_area;
    float               m_oriWidth;
    float               m_oriHeight;

    float				m_circumferenceUnit;
    float				m_areaUnit;
    float               m_oriWidthUnit;
    float               m_oriHeightUnit;
};

#endif // RECTITEM_H

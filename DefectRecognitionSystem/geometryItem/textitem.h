#ifndef TEXTITEM_H
#define TEXTITEM_H

#include "rectdragitemdata.h"
#include "scanglobal.h"
#include "geometryitembase.h"

class TextItem : public GeometryItemBase
{
public:
    explicit TextItem();
    explicit TextItem(QRectF rect, QString text);

    ~TextItem();

    void calcOriGeometry(QPoint oriPt, int imgW, int imgH, float scale, int rotate, bool bFlip, bool bMirror);

    void updateGeometry(QPoint curOriPt, int imgW, int imgH, float scale, int rotate, bool bFlip, bool bMirror);

    //
    void setText(QRectF rect, QString text);

//    void updateRect(QRectF rect);

    void updateFont(QFont font, float scaleValue);

    void updateText(QString text, float scaleValue);

    void reCalcFont();

    //get
    QString getText();

    QRectF getRect();
    RectDragItemData getDragItemData();
    float   getWidth();
    float   getHeight();

    QFont   getTempFont();

    QRectF getOriRect();
    float getOriWidth();
    float getOriHeight();

private:
    QRectF              m_rect;
    RectDragItemData    m_rectDragItemData;

    QString             m_text;

    float               m_width;
    float               m_height;

    QFont               m_fontTmp;
    QRectF				m_rectOri;
    float               m_oriWidth;
    float               m_oriHeight;
};

#endif // TEXTITEM_H

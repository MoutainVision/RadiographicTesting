#include "textitem.h"

#include <QDebug>

TextItem::TextItem()
{
    m_eItemStatus = NORMOAL;

    m_text = "";
}

TextItem::~TextItem()
{

}

TextItem::TextItem(QRectF rect, QString text)
{
    m_eItemStatus = NORMOAL;

    setBoundingRect(rect);

    m_text = text;
    m_rect = rect;

    m_rectDragItemData.setRect(&m_rect.toRect());

    m_width = rect.width();
    m_height = rect.height();
}

//
void TextItem::setText(QRectF rect, QString text)
{
    setBoundingRect(rect);

    m_text = text;
    m_rect = rect;

    m_rectDragItemData.setRect(&m_rect.toRect());

    m_width = rect.width();
    m_height = rect.height();

    //计算font temp size
    int size1 = (int)m_width / ceil((float)m_text.toLocal8Bit().size()/2.0);
    int size2 = m_height;

    int pixelSize = size1 < size2 ? size1 : size2;

    QFont font = getFont();
    font.setPixelSize(pixelSize);

    m_fontTmp = font;
}

void TextItem::calcOriGeometry(QPoint oriPt, int imgW, int imgH, float scale, int rotate, bool bFlip, bool bMirror)
{
    //转化为图像的坐标系统，需要注意的是oriPt 已经经过放大缩小
    //缩放变换
    m_rectOri.setLeft((float)m_rect.left() / scale);
    m_rectOri.setRight((float)m_rect.right() / scale);
    m_rectOri.setTop((float)m_rect.top() / scale);
    m_rectOri.setBottom((float)m_rect.bottom() / scale);

    //平移变换
    m_rectOri.setLeft(m_rectOri.left() - oriPt.x());
    m_rectOri.setRight(m_rectOri.right() - oriPt.x());
    m_rectOri.setTop(m_rectOri.top() - oriPt.y());
    m_rectOri.setBottom(m_rectOri.bottom() - oriPt.y());

    //更新、计算相关信息
    m_oriWidth = m_rectOri.width();
    m_oriHeight = m_rectOri.height();
}

void TextItem::updateGeometry(QPoint curOriPt, int imgW, int imgH, float scale, int rotate, bool bFlip, bool bMirror)
{
    //把图像坐标系转窗口坐标系 ，需要注意的是 curOriPt 已经经过放大缩小
    QRectF rectTemp;

    //平移
    rectTemp.setLeft(m_rectOri.left() + curOriPt.x());
    rectTemp.setRight(m_rectOri.right() + curOriPt.x());
    rectTemp.setTop(m_rectOri.top() + curOriPt.y());
    rectTemp.setBottom(m_rectOri.bottom() + curOriPt.y());

    //缩放变换
    rectTemp.setLeft((float)rectTemp.left() * scale);
    rectTemp.setRight((float)rectTemp.right() * scale);
    rectTemp.setTop((float)rectTemp.top() * scale);
    rectTemp.setBottom((float)rectTemp.bottom() * scale);

    //刷新矩阵
    setText(rectTemp, m_text);
}

void TextItem::reCalcFont()
{
    qreal width = m_rectOri.width();
    qreal height = m_rectOri.height();

    //重新计算font size
    int size1 = (int)width / ceil((float)m_text.toLocal8Bit().size()/2.0);
    int size2 = height;

    int pixelSize = size1 < size2 ? size1 : size2;

    QFont font = getFont();
    font.setPixelSize(pixelSize);

    setFont(font);
}

//void TextItem::updateRect(QRectF rect)
//{
//    m_rect = rect;

//    setBoundingRect(rect);

//    m_rect = rect;

//    m_rectDragItemData.setRect(&m_rect.toRect());

//    m_width = rect.width();
//    m_height = rect.height();

//    //重新计算font size
//    int size1 = (int)m_width / ceil((float)m_text.toLocal8Bit().size()/2.0);
//    int size2 = m_height;

//    int pixelSize = size1 < size2 ? size1 : size2;

//    QFont font = getFont();
//    font.setPixelSize(pixelSize);

//    setFont(font);
//}

void TextItem::updateText(QString text, float scaleValue)
{
    m_text = text;

    int pixelSize = m_font.pixelSize();

    m_rect = QRectF(m_rect.x(),
                    m_rect.y(),
                    pixelSize*ceil((float)text.toLocal8Bit().size()/2.0)*scaleValue,
                    pixelSize*scaleValue);
}

void TextItem::updateFont(QFont font, float scaleValue)
{
   m_font = font;

   //重新计算文字边框
   int pixelSize = m_font.pixelSize();
   float w, h;
   w = pixelSize * ceil((float)m_text.toLocal8Bit().size()/2.0) * scaleValue;
   h = pixelSize * scaleValue;

   m_rect = QRectF(m_rect.x(), m_rect.y(), w, h);
}

//get
QFont TextItem::getTempFont()
{
    return m_fontTmp;
}

QRectF TextItem::getOriRect()
{
    return m_rectOri;
}

float TextItem::getOriWidth()
{
    return m_oriWidth;
}

float TextItem::getOriHeight()
{
    return m_oriHeight;
}

float TextItem::getWidth()
{
    return m_width;
}

float TextItem::getHeight()
{
    return m_height;
}

QString TextItem::getText()
{
    return m_text;
}

QRectF TextItem::getRect()
{
    return m_rect;
}

RectDragItemData TextItem::getDragItemData()
{
    return m_rectDragItemData;
}

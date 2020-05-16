#ifndef GEOMETRYITEMBASE_H
#define GEOMETRYITEMBASE_H

#include "appconfig.h"

//class GeometryItemBase : public QObject
class GeometryItemBase
{
//    Q_OBJECT
public:
    GeometryItemBase();
    ~GeometryItemBase();

    virtual void dummy();

    //calc original geometry(trans image coordinate)
    virtual void calcOriGeometry(QPoint oriPt, int imgW,
                                 int imgH, float scale, int rotate,
                                 bool bFlip, bool bMirror);

    //update geometry (get current geometry)
    virtual void updateGeometry(QPoint curOriPt, int imgW,
                                int imgH, float scale, int rotate,
                                bool bFlip, bool bMirror);

    virtual void setCurLabelType(LabelType type = NONELABEL);
    virtual void setCurLabelType(QString typeStr);
    virtual LabelType getCurLabelType();
    virtual QString getLabelTypeStr(LabelType type);

    virtual void setPenStyle(Qt::PenStyle type = Qt::SolidLine);
    virtual Qt::PenStyle getPenStyle();

    static LabelType getLabelType(QString typeStr);

    virtual QList<LabelType> getLabelTypes();
    virtual QStringList getLabelTypeNames();

    virtual QString getItemName();

    virtual void setUnit(ELengthUnit unit);

    virtual void setSpatialResolution(double resolution);

    virtual void changeToUnitType();

    //
    void setBoundingRect(QRect boundingRect);
    void setBoundingRect(QRectF boundingRectF);

    void setItemStatus(ItemStatus status);
    void setLineWidth(int lineWidth);
    void setColor(QColor color);
    void setFont(QFont font);

    //
    QRect getBoundingRect();
    QRectF getBoundingRectF();
    ItemStatus getItemStatus();
    int  getLineWidht();
    QColor getColor();
    QFont   getFont();

    void setBoundingPolygonf(QPolygonF polygonf);
    QPolygonF getBoundingPolygonf();

//signals:
//    void sig_updateInfo();

    void setIntensity(float value){mIntensity = value;}
    float getIntensity() {return mIntensity;}

    void setSNR(double dMean, double dStd, double dSNR){mMean = dMean; mStd = dStd;  mSNR = dSNR; }
    void getSNR(double &dMean, double &dStd, double &dSNR){dMean =mMean;  dStd = mStd; dSNR = mSNR;}


public:
    QRect               m_boundingRect;
    QRectF              m_boundingRectF;
    ItemStatus          m_eItemStatus;
    int                 m_lineWidth;
    QFont               m_font;
    QColor              m_color;

    QString             m_itemName;

    QPolygonF           m_polygonf;

    Qt::PenStyle        m_penStyle;

    QList<LabelType>    m_labelTypList;
    LabelType           m_curLabelType;

    ELengthUnit         m_lengthUnit;
    double              m_spatialResolution;


    float               mIntensity;
    double              mMean;
    double              mStd;
    double              mSNR;
};

#endif // GEOMETRYITEMBASE_H

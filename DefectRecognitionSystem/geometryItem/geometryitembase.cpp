#include "geometryitembase.h"

GeometryItemBase::GeometryItemBase()
{
    m_eItemStatus = NORMOAL;
    m_lineWidth = 1;
    m_color = QColor(Qt::red);

    m_curLabelType = NONELABEL;
    m_penStyle = Qt::SolidLine;

    m_lengthUnit = PIXELUNIT;
    m_spatialResolution = 1.0;
}

GeometryItemBase::~GeometryItemBase()
{

}

void GeometryItemBase::dummy()
{

}

void GeometryItemBase::calcOriGeometry(QPoint oriPt, int imgW, int imgH, float scale, int rotate, bool bFlip, bool bMirror)
{

}

void GeometryItemBase::updateGeometry(QPoint curOriPt, int imgW, int imgH, float scale, int rotate, bool bFlip, bool bMirror)
{

}

QStringList GeometryItemBase::getLabelTypeNames()
{
    QStringList labelTypeList;

    for (int i=0; i<m_labelTypList.size(); i++)
    {
        LabelType labelType = m_labelTypList.at(i);

        switch (labelType) {
        case NAMELABEL:
            labelTypeList.append(QObject::tr("Name"));
            break;
        case AREALABEL:
            labelTypeList.append(QObject::tr("Area"));
            break;
        case PERIMETERLABEL:
            labelTypeList.append(QObject::tr("Perimeter"));
            break;
        case WIDTHANDHEIGHTLABEL:
            labelTypeList.append(QObject::tr("Width/Height"));
            break;
        case MAJORANDMINORAXISLABEL:
            labelTypeList.append(QObject::tr("Major/MinorAxis"));
            break;
        case LENGTHLABEL:
            labelTypeList.append(QObject::tr("Length"));
            break;
        case NONELABEL:
            labelTypeList.append(QObject::tr("None"));
            break;
        default:
            break;
        }
    }

    return labelTypeList;
}

QList<LabelType>  GeometryItemBase::getLabelTypes()
{
    return m_labelTypList;
}

void GeometryItemBase::setCurLabelType(LabelType type)
{
    m_curLabelType = type;
}

void GeometryItemBase::setCurLabelType(QString typeStr)
{
    m_curLabelType = getLabelType(typeStr);
}

LabelType GeometryItemBase::getCurLabelType()
{
    return m_curLabelType;
}

QString GeometryItemBase::getItemName()
{
    return m_itemName;
}

void GeometryItemBase::setUnit(ELengthUnit unit)
{
    m_lengthUnit = unit;
}

void GeometryItemBase::setSpatialResolution(double resolution)
{
    m_spatialResolution = resolution;
}

void GeometryItemBase::changeToUnitType()
{

}

LabelType GeometryItemBase::getLabelType(QString typeStr)
{
    LabelType labelTypeTmp;

    if (typeStr == QObject::tr("Name"))
        labelTypeTmp = NAMELABEL;
    else if (typeStr == QObject::tr("Area"))
        labelTypeTmp = AREALABEL;
    else if (typeStr == QObject::tr("Perimeter"))
        labelTypeTmp = PERIMETERLABEL;
    else if (typeStr == QObject::tr("Width/Height"))
        labelTypeTmp = WIDTHANDHEIGHTLABEL;
    else if (typeStr == QObject::tr("Major/MinorAxis"))
        labelTypeTmp = MAJORANDMINORAXISLABEL;
    else if (typeStr == QObject::tr("Length"))
        labelTypeTmp = LENGTHLABEL;
    else if (typeStr == QObject::tr("None"))
        labelTypeTmp = NONELABEL;

    return labelTypeTmp;
}

QString GeometryItemBase::getLabelTypeStr(LabelType type)
{
    QString labelTypeStr;

    switch (type) {
    case NAMELABEL:
        labelTypeStr = QObject::tr("Name");
        break;
    case AREALABEL:
        labelTypeStr = QObject::tr("Area");
        break;
    case PERIMETERLABEL:
        labelTypeStr = QObject::tr("Perimeter");
        break;
    case WIDTHANDHEIGHTLABEL:
        labelTypeStr = QObject::tr("Width/Height");
        break;
    case MAJORANDMINORAXISLABEL:
        labelTypeStr = QObject::tr("Major/MinorAxis");
        break;
    case LENGTHLABEL:
        labelTypeStr = QObject::tr("Length");
        break;
    case NONELABEL:
        labelTypeStr = QObject::tr("None");
        break;
    default:
        break;
    }

    return labelTypeStr;
}

void GeometryItemBase::setPenStyle(Qt::PenStyle type)
{
    m_penStyle = type;
}

Qt::PenStyle GeometryItemBase::getPenStyle()
{
    return m_penStyle;
}

//
void GeometryItemBase::setBoundingRect(QRect boundingRect)
{
    m_boundingRect = boundingRect;

    QPolygonF polygonf;
    polygonf.push_back(boundingRect.topLeft());
    polygonf.push_back(boundingRect.bottomLeft());
    polygonf.push_back(boundingRect.bottomRight());
    polygonf.push_back(boundingRect.topRight());

    setBoundingPolygonf(polygonf);
}

void GeometryItemBase::setBoundingRect(QRectF boundingRectF)
{
    m_boundingRectF = boundingRectF;

    QPolygonF polygonf;
    polygonf.push_back(boundingRectF.topLeft());
    polygonf.push_back(boundingRectF.bottomLeft());
    polygonf.push_back(boundingRectF.bottomRight());
    polygonf.push_back(boundingRectF.topRight());

    setBoundingPolygonf(polygonf);
}

void GeometryItemBase::setItemStatus(ItemStatus status)
{
    m_eItemStatus = status;
}

void GeometryItemBase::setLineWidth(int lineWidth)
{
    m_lineWidth = lineWidth;
}

void GeometryItemBase::setFont(QFont font)
{
    m_font = font;
}

void GeometryItemBase::setColor(QColor color)
{
    m_color = color;
}

void GeometryItemBase::setBoundingPolygonf(QPolygonF polygonf)
{
    m_polygonf = polygonf;
}

QPolygonF GeometryItemBase::getBoundingPolygonf()
{
    return m_polygonf;
}

//
QRect GeometryItemBase::getBoundingRect()
{
    return m_boundingRect;
}

QRectF GeometryItemBase::getBoundingRectF()
{
    return m_boundingRectF;
}

ItemStatus GeometryItemBase::getItemStatus()
{
    return m_eItemStatus;
}

int GeometryItemBase::getLineWidht()
{
    return m_lineWidth;
}

QFont GeometryItemBase::getFont()
{
    return m_font;
}

QColor GeometryItemBase::getColor()
{
    return m_color;
}


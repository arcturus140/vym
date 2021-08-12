#include <QDebug>

#include "flagobj.h"

/////////////////////////////////////////////////////////////////
// FlagObj
/////////////////////////////////////////////////////////////////
FlagObj::FlagObj(QGraphicsItem *parent) : MapObj(parent)
{
    // qDebug() << "Const FlagObj  this=" << this;
    init();
}

FlagObj::~FlagObj()
{
    // qDebug() << "Destr FlagObj  this=" << this << "  " << name;
    if (imageObj)
        delete (imageObj);
}

void FlagObj::init()
{
    imageObj = new ImageObj(parentItem());
    avis = true;
}

void FlagObj::move(double x, double y)
{
    MapObj::move(x, y);
    imageObj->setPos(QPointF(x, y));
    positionBBox();
}

void FlagObj::moveBy(double x, double y)
{
    move(x + absPos.x(), y + absPos.y());
}

void FlagObj::setZValue(double z) { imageObj->setZValue(z); }

void FlagObj::setVisibility(bool v)
{
    MapObj::setVisibility(v);
    if (v)
        imageObj->setVisibility(true);
    else
        imageObj->setVisibility(false);
}

void FlagObj::loadImage(ImageObj *io)
{
    prepareGeometryChange();

    imageObj->copy(io); // Creates deep copy of pixmap or svg!
    calcBBoxSize();
    positionBBox();
}

void FlagObj::setUuid(const QUuid &id) { uid = id; }

const QUuid FlagObj::getUuid() { return uid; }

void FlagObj::setAlwaysVisible(bool b) { avis = b; }

bool FlagObj::isAlwaysVisible() { return avis; }

void FlagObj::positionBBox()
{
    bbox.moveTopLeft(absPos);
    clickPoly = QPolygonF(bbox);
}

void FlagObj::calcBBoxSize()
{
    if (visible)
        bbox.setSize(QSizeF(imageObj->boundingRect().width(),
                            imageObj->boundingRect().height()));
    else
        bbox.setSize(QSizeF(0, 0));
    clickPoly = QPolygonF(bbox);
}

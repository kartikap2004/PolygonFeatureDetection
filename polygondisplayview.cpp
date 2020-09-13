#include "polygondisplayview.h"
#include "polygongraphicsitem.h"
#include <math.h>
#include <QDebug>
#include <QGraphicsItem>
#include <QPointF>
#include <QPolygonF>
#include <QScrollBar>
#include <QVector>

PolygonDisplayView::PolygonDisplayView(QWidget *parent)
    : QGraphicsView(parent)
{
    mPolygonGraphicsItem = new PolygonGraphicsItem(this->sceneRect(), this);
    mScene = new QGraphicsScene(this);

    setScene(mScene);
    mScene->addItem(mPolygonGraphicsItem);

    mPolygonGraphicsItem->setScale(0.5);

    mScene->update();
    this->update();
};

PolygonDisplayView::~PolygonDisplayView()
{
    // delete mPolygonGraphicsItem;
    mScene->clear();
}

void PolygonDisplayView::setPolyPoints(QSharedPointer<QVector<QPointF>> aPointsList)
{
    mPolygonGraphicsItem->clearData();
    mPolygonGraphicsItem->setPolyPoints(aPointsList);
    mScene->update();
}

void PolygonDisplayView::refreshGeometry()
{
    // mPolygonGraphicsItem->refreshPolygon();
    mScene->update();
}

void PolygonDisplayView::setScale(const double aScale)
{
    mPolygonGraphicsItem->setItemScaling(aScale);
    QRectF lGraphicsRect = mPolygonGraphicsItem->getBoundingRect();

    double lMinX = -99;
    double lMinY = -99;
    double lMaxX = 99;
    double lMaxY = 99;

    mPolygonGraphicsItem->getMinMax(lMinX, lMaxX, lMinY, lMaxY);
    int lScrollClearance = std::max(abs(lMaxX - lMinX), abs(lMaxY - lMinY));

    QScrollBar *hbar = horizontalScrollBar();
    hbar->setRange(int(lGraphicsRect.left()) - lScrollClearance,
                   int(lGraphicsRect.right() + lScrollClearance));

    QScrollBar *vbar = verticalScrollBar();
    vbar->setRange(int(lGraphicsRect.top()) - lScrollClearance,
                   int(lGraphicsRect.bottom() + lScrollClearance));

    mScene->update();
}

void PolygonDisplayView::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "PolygonDisplayView::mousePressEvent()";
    mPolygonGraphicsItem->mousePressEvent(event);
}

void PolygonDisplayView::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "PolygonDisplayView::mouseMoveEvent()";
    mPolygonGraphicsItem->mouseMoveEvent(event);
}

void PolygonDisplayView::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "PolygonDisplayView::mouseReleaseEvent()";
    mPolygonGraphicsItem->mouseReleaseEvent(event);
}

void PolygonDisplayView::decompose_polygon(const bool aFindEdges)
{
    mPolygonGraphicsItem->decompose_polygon(aFindEdges);
    mScene->update();
}

void PolygonDisplayView::setLineTolerance(const double aDeviationVal)
{
    mPolygonGraphicsItem->setLineTolerance(aDeviationVal);
}

void PolygonDisplayView::setArcTolerance(const double aDeviationVal)
{
    mPolygonGraphicsItem->setArcTolerance(aDeviationVal);
}

void PolygonDisplayView::setSplineTolerance(const double aDeviationVal)
{
    mPolygonGraphicsItem->setSplineTolerance(aDeviationVal);
}

void PolygonDisplayView::setSharpAngleTolerance(const double aTolerance)
{
    mPolygonGraphicsItem->setSharpAngleTolerance(aTolerance);
}

void PolygonDisplayView::setCheckLineTol(const bool aCheck)
{
    mPolygonGraphicsItem->setCheckLineTol(aCheck);
}

void PolygonDisplayView::setCheckArcTol(const bool aSetTol)
{
    mPolygonGraphicsItem->setCheckArcTol(aSetTol);
}

void PolygonDisplayView::setCheckSplineTol(const bool aCheck)
{
    mPolygonGraphicsItem->setCheckSplineTol(aCheck);
}

void PolygonDisplayView::setCheckSharpEdges(const bool aCheckSharpEdges)
{
    mPolygonGraphicsItem->setCheckSharpAngleTol(aCheckSharpEdges);
}

void PolygonDisplayView::setMarkerDisplay(const bool aShow)
{
    mPolygonGraphicsItem->setMarkerDisplay(aShow);
}

PolygonGraphicsItem *PolygonDisplayView::getPolyGraphicsItem()
{
    return mPolygonGraphicsItem;
}

void PolygonDisplayView::updateScene()
{
    mScene->setSceneRect(this->rect());
    mPolygonGraphicsItem->setRect(this->rect());

    mScene->removeItem(mPolygonGraphicsItem);
    mScene->addItem(mPolygonGraphicsItem);

    mScene->update();
}

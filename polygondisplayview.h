#ifndef POLGONDISPLAYWIDGET_H
#define POLGONDISPLAYWIDGET_H

#include "polygongraphicsitem.h"
#include <QGraphicsView>
#include <QPainter>
#include <QPainterPath>
#include <QPointF>
#include <QPolygonF>
#include <QSharedPointer>
#include <QStyleOptionGraphicsItem>
#include <QVector>
#include <QWidget>

class QGraphicsItem;

class PolygonDisplayView : public QGraphicsView
{
    Q_OBJECT
public:
    PolygonDisplayView(QWidget *parent = nullptr);
    ~PolygonDisplayView() override;

    virtual void updateScene();

    void setScale(const double aScale);
    void setPolyPoints(QSharedPointer<QVector<QPointF>> aPointsList);
    void refreshGeometry();

    void decompose_polygon(const bool aFindEdges);
    PolygonGraphicsItem *getPolyGraphicsItem();

    void setLineTolerance(const double aDeviationVal);
    void setArcTolerance(const double aDeviationVal);
    void setSplineTolerance(const double aDeviationVal);
    void setSharpAngleTolerance(const double aDeviationVal);

    void setCheckLineTol(const bool aSetTol);
    void setCheckArcTol(const bool aSetTol);
    void setCheckSplineTol(const bool aSetTol);
    void setCheckSharpEdges(const bool aCheckSharpEdges);

    void setMarkerDisplay(const bool aShow);

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

private:
    PolygonGraphicsItem *mPolygonGraphicsItem;
    QGraphicsScene *mScene;
};

#endif // POLGONDISPLAYWIDGET_H

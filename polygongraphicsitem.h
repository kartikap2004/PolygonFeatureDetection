#ifndef POLYGONGRAPHICSITEM_H
#define POLYGONGRAPHICSITEM_H

#include <memory.h>
#include <polyfeaturedetection.h>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QPainter>
#include <QPainterPath>
#include <QSharedPointer>
#include <QWidget>

const int DATAMARKERSIZE = 1;
const int LINE_WIDTH = 1;
const double POINT_CLICK_TOLERANCE = 10.0;

class PolygonGraphicsItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    PolygonGraphicsItem(const QRectF &aRect, QGraphicsView *parent);
    ~PolygonGraphicsItem() override;

    void decompose_polygon(const bool aFindEdges);
    void setPolyPoints(QSharedPointer<QVector<QPointF>> aPointsList);
    void setRect(const QRectF &aRect);
    void setItemScaling(qreal aScaleFactor);
    QPolygonF recalcPolygon(const QPolygonF &aPolygon);
    // virtual QPainterPath shape() const override;
    void fitCurve();
    void clearData();

    void setLineTolerance(const double aDeviationVal);
    void setArcTolerance(const double aDeviationVal);
    void setSplineTolerance(const double aDeviationVal);
    void setSharpAngleTolerance(const double aTolerance);

    void setCheckLineTol(const bool aCheck);
    void setCheckArcTol(const bool aCheck);
    void setCheckSplineTol(const bool aCheck);
    void setCheckSharpAngleTol(const bool aCheck);

    // virtual bool event(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    QRectF getBoundingRect() const;
    void setMarkerDisplay(const bool aShow);

    void getMinMax(double &aMinX, double &aMinY, double &aMaxX, double &aMaxY);

protected:
    virtual void paint(QPainter *aPainter,
                       const QStyleOptionGraphicsItem *aOptions,
                       QWidget *aWidget) override;
    virtual QRectF boundingRect() const override;

public:
signals:
    void updateStatusBarText(const QString &aText);

public slots:

private:
    QRectF m_rect;
    QSharedPointer<QVector<QPointF>> mPointsList;
    PolyFeatureDetection *mPolyFeatureDetection;
    QList<PolygonEdge *> mPolyEdgeList;
    QGraphicsView *mParent;
    QPointF mPointOffset;
    double mSharpAngleTolerance, mLineTolerance, mArcTolerance, mSplineTolerance;
    bool mDecomposing, mShowMarkers;
    bool mCheckSharpEdges, mCheckLines, mCheckArcs, mCheckSplines;
};

#endif // POLYGONGRAPHICSITEM_H

#ifndef POLYFEATUREDETECTION_H
#define POLYFEATUREDETECTION_H

#include "polygonedge.h"
#include <QList>
#include <QPolygonF>
#include <QSharedPointer>
#include <QVector>

class PolyFeatureDetection : public QObject
{
    Q_OBJECT
public:
    PolyFeatureDetection(QSharedPointer<QVector<QPointF>> &aPointsList);

    void createEdgeList(QList<PolygonEdge *> &aEdgeList);

    void getMinMax(QList<PolygonEdge *> &aEdgeList,
                   double &aMinX,
                   double &aMinY,
                   double &aMaxX,
                   double &aMaxY);

    int sharpAngleToleranceCheck(QList<PolygonEdge *> &aEdgeList, const double aAngleTol);

    int lineToleranceCheck(QList<PolygonEdge *> &aEdgeList,
                           const double aTolerance,
                           const bool aSharpAngleCheck,
                           const double aSharpAngleTol);

    int arcToleranceCheck(QList<PolygonEdge *> &aEdgeList,
                          const double aTolerance,
                          const bool aSharpAngleCheck,
                          const double aSharpAngleTol);

    int splineToleranceCheck(QList<PolygonEdge *> &aEdgeList,
                             const double aTolerance,
                             const bool aSharpAngleCheck,
                             const double aSharpAngleTol);

    void getListOfSharpFeatures(QList<PolygonEdge *> &aEdgeList,
                                const double aAngleTol,
                                QList<QSharedPointer<QList<PolygonEdge *>>> &aSharpFeatures);
private:
    bool calculateArcParameters(const PolygonEdge *aCurrentEdge,
                                const PolygonEdge *aNextEdge,
                                double &aCenterX,
                                double &aCenterY,
                                double &aRadius);

    void calcSplineApprox_recursive(QVector<QPointF> &aInputPts,
                                    QList<PolygonEdge *> &aEdgeCandidateList,
                                    const double aTolerance);

    void calcSpline(QVector<QPointF> &aInputPts, QVector<QPointF> &aSplineCurvePts);

    void identifySplineErrors(QVector<QPointF> &aSplineCurvePts,
                              QList<PolygonEdge *> &aEdgeList,
                              const double aTolerance);

    bool removeEdgeswithSplineErrors(QList<PolygonEdge *> &aEdgeList,
                                     QVector<QPointF> &aCandidatePts,
                                     const double aTolerance);

    QSharedPointer<QVector<QPointF>> mPolyPoints;
};

#endif // POLYFEATUREDETECTION_H

#include "polyfeaturedetection.h"
#include <CurveFitter.h>
#include <limits.h>
#include <math.h>
#include <QLineF>
#include <QPainterPath>
#include <QPointF>

PolyFeatureDetection::PolyFeatureDetection(QSharedPointer<QVector<QPointF>> &aPointsList)
{
    mPolyPoints = aPointsList;
}

void PolyFeatureDetection::createEdgeList(QList<PolygonEdge *> &aEdgeList)
{
    // recreate edge list
    aEdgeList.clear();
    QPointF lPrev, lCurrent;
    for (int i = 1; i < mPolyPoints->count(); i++) {
        lPrev = mPolyPoints->at(i - 1);
        lCurrent = mPolyPoints->at(i);
        aEdgeList.append(new PolygonEdge(lPrev, lCurrent));
    }
}

void PolyFeatureDetection::getMinMax(
    QList<PolygonEdge *> &aEdgeList, double &aMinX, double &aMinY, double &aMaxX, double &aMaxY)
{
    aMinX = 99999.0;
    aMinY = 99999.0;
    aMaxX = -99999.0;
    aMaxY = -99999.0;

    if (aEdgeList.count() > 0) {
        for (auto lEdge : aEdgeList) {
            double x1 = lEdge->getPoint1().x();
            double y1 = lEdge->getPoint1().y();
            if (x1 < aMinX) {
                aMinX = x1;
            }
            if (x1 > aMaxX) {
                aMaxX = x1;
            }
            if (y1 < aMinY) {
                aMinY = y1;
            }
            if (y1 > aMaxY) {
                aMaxY = y1;
            }
        }
        // check endpoint of last edge.
        double x2 = aEdgeList.at(aEdgeList.count() - 1)->getPoint2().x();
        double y2 = aEdgeList.at(aEdgeList.count() - 1)->getPoint2().y();
        if (x2 < aMinX) {
            aMinX = x2;
        }
        if (x2 > aMaxX) {
            aMaxX = x2;
        }
        if (y2 < aMinY) {
            aMinY = y2;
        }
        if (y2 > aMaxY) {
            aMaxY = y2;
        }
    } else {
        aMinX = -99999.0;
        aMinY = -99999.0;
        aMaxX = 99999.0;
        aMaxY = 99999.0;
    }
}

bool PolyFeatureDetection::calculateArcParameters(const PolygonEdge *aCurrentEdge,
                                                  const PolygonEdge *aNextEdge,
                                                  double &aCenterX,
                                                  double &aCenterY,
                                                  double &aRadius)
{
    bool lArcOk = false;
    if (aCurrentEdge != nullptr && aNextEdge != nullptr) {
        double x1 = aCurrentEdge->getPoint1().x();
        double y1 = aCurrentEdge->getPoint1().y();
        double x2 = aCurrentEdge->getPoint2().x();
        double y2 = aCurrentEdge->getPoint2().y();
        double x3 = aNextEdge->getPoint2().x();
        double y3 = aNextEdge->getPoint2().y();

        // Calculate center and radius of arc formed by 3 previous points
        double centerY_denom = (y1 - y2) / (x1 - x2) - (y2 - y3) / (x2 - x3);
        double centerY_num = ((x1 * x1 + y1 * y1 - x2 * x2 - y2 * y2) / (2 * (x1 - x2)))
                             - ((x2 * x2 + y2 * y2 - x3 * x3 - y3 * y3) / (2 * (x2 - x3)));
        aCenterY = centerY_num / centerY_denom;
        aCenterX = ((x1 * x1 + y1 * y1 - x2 * x2 - y2 * y2) / (2 * (x1 - x2)))
                   - (aCenterY * (y1 - y2) / (x1 - x2));
        aRadius = sqrt((aCenterX - x1) * (aCenterX - x1) + (aCenterY - y1) * (aCenterY - y1));
        lArcOk = true;
    }
    return lArcOk;
}

void PolyFeatureDetection::getListOfSharpFeatures(
    QList<PolygonEdge *> &aEdgeList,
    const double aAngleTol,
    QList<QSharedPointer<QList<PolygonEdge *>>> &aSharpFeaturesList)
{
    sharpAngleToleranceCheck(aEdgeList, aAngleTol);

    if (aEdgeList.count() > 0) {
        QSharedPointer<QList<PolygonEdge *>> lCurrentSharpEdgeList
            = QSharedPointer<QList<PolygonEdge *>>(new QList<PolygonEdge *>());
        lCurrentSharpEdgeList->append(aEdgeList.at(0));
        aSharpFeaturesList.append(lCurrentSharpEdgeList);

        int i = 1;
        while (i <= aEdgeList.count() - 1) {
            PolygonEdge *lPrevEdge = aEdgeList.at(i - 1);
            int lSharpEdgeID = lPrevEdge->getSharpEdgeID();
            PolygonEdge *lCurrentEdge = aEdgeList.at(i);
            if (lCurrentEdge->getSharpEdgeID() == lSharpEdgeID) {
                lCurrentSharpEdgeList->append(lCurrentEdge);
            } else {
                lCurrentSharpEdgeList = QSharedPointer<QList<PolygonEdge *>>(
                    new QList<PolygonEdge *>());
                lCurrentSharpEdgeList->append(lCurrentEdge);
                aSharpFeaturesList.append(lCurrentSharpEdgeList);
            }
            i++;
        }
    }
}

int PolyFeatureDetection::sharpAngleToleranceCheck(QList<PolygonEdge *> &aEdgeList,
                                                   const double aAngleTol)
{
    // Reset all sharp edge IDs
    for (auto lEdge : aEdgeList) {
        lEdge->setSharpEdgeID(DEFAULT_SHARPEDGE_ID);
    }

    int lSharpEdgeCount = DEFAULT_SHARPEDGE_ID + 1;
    if (aEdgeList.count() >= 2) {
        PolygonEdge *firstEdge = aEdgeList.at(0);
        firstEdge->setSharpEdgeID(lSharpEdgeCount);
        bool lCurrentAngleDir = false;
        bool lPrevAngleDir = (firstEdge->getAngle() >= 0);

        int i = 0;
        while (i <= aEdgeList.count() - 2) {
            PolygonEdge *lCurrentEdge = aEdgeList.at(i);
            PolygonEdge *lNextEdge = aEdgeList.at(i + 1);
            double lAngleDiff = abs(lNextEdge->getAngle() - lCurrentEdge->getAngle());
            lCurrentAngleDir = (lAngleDiff >= 0) & (lPrevAngleDir);
            if ((lAngleDiff <= aAngleTol)) {
                lNextEdge->setSharpEdgeID(lSharpEdgeCount);
            } else {
                lSharpEdgeCount++;
                lNextEdge->setSharpEdgeID(lSharpEdgeCount);
            }
            lPrevAngleDir = lCurrentAngleDir;
            i++;
        } // while (i <= aEdgeList.count() - 2)

        // check angle between first and last edge.
        PolygonEdge *lastEdge = aEdgeList.at(aEdgeList.count() - 1);
        double lAngleDiff = (lastEdge->getAngle()) - (firstEdge->getAngle());
        lCurrentAngleDir = (lAngleDiff >= 0) & (lPrevAngleDir);

        if (lAngleDiff <= aAngleTol) {
            // get all the edges tagged with same ID as first edge
            // change the IDs of all of the above edges.
            for (int i = 0; i <= aEdgeList.count() - 1; i++) {
                PolygonEdge *lEdge = aEdgeList.at(i);
                if (lEdge->getSharpEdgeID() == DEFAULT_SHARPEDGE_ID) {
                    lEdge->setSharpEdgeID(lSharpEdgeCount);
                } else {
                    break;
                }
            }
            i = 0;
            lSharpEdgeCount = DEFAULT_SHARPEDGE_ID;
            while (i <= aEdgeList.count() - 2) {
                PolygonEdge *lCurrentEdge = aEdgeList.at(i);
                PolygonEdge *lNextEdge = aEdgeList.at(i + 1);
                double lAngleDiff = lNextEdge->getAngle() - lCurrentEdge->getAngle();
                lCurrentAngleDir = (lAngleDiff >= 0) & (lPrevAngleDir);
                if ((lAngleDiff <= aAngleTol)) {
                    lNextEdge->setSharpEdgeID(lCurrentEdge->getSharpEdgeID());

                } else {
                    lNextEdge->setSharpEdgeID(lCurrentEdge->getSharpEdgeID() + 1);
                    lSharpEdgeCount++;
                }
                lPrevAngleDir = lCurrentAngleDir;
                i++;
            } // while (i <= aEdgeList.count() - 2)
        }
    } // if (aEdgeList.count() >= 2)

    return lSharpEdgeCount;
}

int PolyFeatureDetection::lineToleranceCheck(QList<PolygonEdge *> &aEdgeList,
                                             const double aTolerance,
                                             const bool aSharpAngleCheck,
                                             const double aSharpAngleTol)
{
    // Poly Edges that do NOT pass the tolerance check will be marked with feature
    // ID "0" or a special feature ID "99999" indicating ---- NOT IMPLEMENTED
    long lFeatureID = LINE_FEATURE_ID;

    QList<QSharedPointer<QList<PolygonEdge *>>> lSharpEdges;
    if (aSharpAngleCheck) {
        getListOfSharpFeatures(aEdgeList, aSharpAngleTol, lSharpEdges);
    } else {
        QSharedPointer<QList<PolygonEdge *>> lEdgeListPtr = QSharedPointer<QList<PolygonEdge *>>(
            new QList<PolygonEdge *>());
        for (auto lEdge : aEdgeList) {
            lEdgeListPtr->append(lEdge);
        }
        lSharpEdges.append(lEdgeListPtr);
    }

    double lMinX, lMinY, lMaxX, lMaxY;
    getMinMax(aEdgeList, lMinX, lMinY, lMaxX, lMaxY);
    double lNormalizeFactor = std::max((lMaxY - lMinY), (lMaxX - lMinX));

    for (int j = 0; j <= lSharpEdges.count() - 1; j++) {
        QSharedPointer<QList<PolygonEdge *>> lCandidateEdgeListPtr = lSharpEdges.at(j);
        QList<PolygonEdge *> lCandidateEdgeList = *(lCandidateEdgeListPtr.data());
        lFeatureID++;

        // Remove all edges tagged as SPLINEs
        QList<PolygonEdge *> lCandidateLineEdges;
        for (auto lEdge : lCandidateEdgeList) {
            if (lEdge->getFeatureID() < ARC_FEATURE_ID) {
                lCandidateLineEdges.append(lEdge);
            }
        }

        if (lCandidateLineEdges.count() > 1) {
            PolygonEdge *lFirstEdge = lCandidateLineEdges.at(0);
            lFirstEdge->setFeatureID(lFeatureID);

            for (int i = 1; i <= lCandidateLineEdges.count() - 1; i++) {
                PolygonEdge *lCurrentEdge = lCandidateLineEdges.at(i);
                PolygonEdge *lPrevEdge = lCandidateLineEdges.at(i - 1);

                double lPrevSlope, lCurrentSlope;
                double lPrevAngle = lPrevEdge->getAngle();
                if (fabs(lPrevAngle - 90) <= EPSILON) {
                    lPrevSlope = 1.0;
                } else {
                    lPrevSlope = tan(lPrevAngle);
                }

                double lCurrentAngle = lCurrentEdge->getAngle();
                if (fabs(lCurrentAngle - 90) <= EPSILON) {
                    lCurrentSlope = 1.0;
                } else {
                    lCurrentSlope = tan(lCurrentAngle);
                }

                double lSlopeDiff = fabs(lCurrentSlope - lPrevSlope) / lNormalizeFactor;
                if (lSlopeDiff <= aTolerance) {
                    lCurrentEdge->setFeatureID(lPrevEdge->getFeatureID());
                } else {
                    lFeatureID++;
                    lCurrentEdge->setFeatureID(lFeatureID);
                }

            } // for i

            // calculate slope between last edge and first edge
        }
    } // for j

    return (lFeatureID - LINE_FEATURE_ID);
}

int PolyFeatureDetection::arcToleranceCheck(QList<PolygonEdge *> &aEdgeList,
                                            const double aTolerance,
                                            const bool aSharpAngleCheck,
                                            const double aSharpAngleTol)
{
    // Step 1: Calculate center of curvature and radius of arc passing through
    // non-collinear pts 1-2-3 -
    // For non-collinearity : check FEATURE ID of edges 1-2 and 2-3, these should
    // be different IDs (because of line tolerance check that runs before arc
    // tolerance check)
    // Step 2: Calculate if pt 4 lies on this arc,
    // if YES in Step 2, mark 1-2-3-4 as ARC ID "ARC_FEATURE_ID+XXX" and keep
    // repeating for subsequeent points if NO in step 2, then start new arc for
    // points 2-3-4.  //
    // Edge 1-2 will be not be tagged (will retain feature ID from line tolerance
    // checks)
    long lFeatureID = ARC_FEATURE_ID;

    QList<QSharedPointer<QList<PolygonEdge *>>> lSharpEdges;
    if (aSharpAngleCheck) {
        getListOfSharpFeatures(aEdgeList, aSharpAngleTol, lSharpEdges);
    } else {
        QSharedPointer<QList<PolygonEdge *>> lEdgeListPtr = QSharedPointer<QList<PolygonEdge *>>(
            new QList<PolygonEdge *>());
        for (auto lEdge : aEdgeList) {
            lEdgeListPtr->append(lEdge);
        }
        lSharpEdges.append(lEdgeListPtr);
    }

    for (int j = 0; j <= lSharpEdges.count() - 1; j++) {
        QSharedPointer<QList<PolygonEdge *>> lCandidateEdgeListPtr = lSharpEdges.at(j);
        QList<PolygonEdge *> lCandidateArcEdges = *(lCandidateEdgeListPtr.data());
        long lFirstEdgeFeatID = 0;

        // Get all edges that are NOT tagged as SPLINEs
        lFeatureID++;
        lCandidateArcEdges.at(0)->setFeatureID(0);

        int lCurrentEdgeIdx = 1;
        while (lCurrentEdgeIdx <= lCandidateArcEdges.count() - 1) {
            double lMinX, lMinY, lMaxX, lMaxY;
            getMinMax(lCandidateArcEdges, lMinX, lMinY, lMaxX, lMaxY);
            double lNormalizeFactor = std::max((lMaxY - lMinY), (lMaxX - lMinX));

            // Calculate center and radius of arc formed by 3 previous points
            double lCenterX, lCenterY, lArcRad;
            PolygonEdge *lCurrentEdge = lCandidateArcEdges.at(lCurrentEdgeIdx);
            PolygonEdge *lPreviousEdge = lCandidateArcEdges.at(lCurrentEdgeIdx - 1);
            lCurrentEdge->setFeatureID(lPreviousEdge->getFeatureID());
            calculateArcParameters(lPreviousEdge, lCurrentEdge, lCenterX, lCenterY, lArcRad);

            // Calculate distance of next edge end point to center of circle
            PolygonEdge *lNextEdge;
            if (lCurrentEdgeIdx == lCandidateArcEdges.count() - 1) {
                lNextEdge = lCandidateArcEdges.at(0);
            } else {
                lNextEdge = lCandidateArcEdges.at(lCurrentEdgeIdx + 1);
            }
            double lDist = sqrt(pow((lNextEdge->getPoint2().y() - lCenterY), 2)
                                + pow(lNextEdge->getPoint2().x() - lCenterX, 2));
            if ((fabs(lDist - lArcRad) / lNormalizeFactor) <= aTolerance) {
                // tag this Edge (and previous & next edge) with arc feature ID
                lPreviousEdge->setFeatureID(lFeatureID);
                lCurrentEdge->setFeatureID(lFeatureID);
                lNextEdge->setFeatureID(lFeatureID);
            } else {
                // edge is connected to previous, but has a different radius from
                // previous arc
                // calculate new ARC parameters.
                // lPreviousEdge->setFeatureID(0);
                lCurrentEdge->setFeatureID(0);
                // lNextEdge->setFeatureID(0);
                calculateArcParameters(lCurrentEdge, lNextEdge, lCenterX, lCenterY, lArcRad);
                lFeatureID++;
            }

            lCurrentEdgeIdx++;
        } // while (i <= lCandidateArcEdges.count() - 2)

    } // for j

    return (lFeatureID - ARC_FEATURE_ID);
}

// QList<PolygonEdge *> &lineToleranceCheck(const QPolygonF& aPolygon);
int PolyFeatureDetection::splineToleranceCheck(QList<PolygonEdge *> &aEdgeList,
                                               const double aTolerance,
                                               const bool aSharpAngleCheck,
                                               const double aSharpAngleTol)
{
    // Find candidates for splines : edges with feature IDs < (ARC_FEATURE_ID)??
    // Step 1 : check if the complete list of points can be
    // approximated by 1 spline
    // Step 2: If yes, mark all edges with a unique
    // feature id for spline (SPLINE_FEATURE_ID).
    // Step 3 : If no, find the first
    // point with spline error, break the list at this point, and recalculate
    // spline for remaining points in list. ALl entities upto the error point are
    // tagged with new feature ID (SPLINE_FEATURE_ID+XXX)
    // Step 4 : Repeat step 3
    // until all points with spline errors have been removed OR approximated with
    // new splines
    long lFeatureID = SPLINE_FEATURE_ID;
    QList<QSharedPointer<QList<PolygonEdge *>>> lSharpEdges;
    if (aSharpAngleCheck) {
        getListOfSharpFeatures(aEdgeList, aSharpAngleTol, lSharpEdges);
    } else {
        QSharedPointer<QList<PolygonEdge *>> lEdgeListPtr = QSharedPointer<QList<PolygonEdge *>>(
            new QList<PolygonEdge *>());
        for (auto lEdge : aEdgeList) {
            lEdgeListPtr->append(lEdge);
        }
        lSharpEdges.append(lEdgeListPtr);
    }

    for (int k = 0; k <= lSharpEdges.count() - 1; k++) {
        QSharedPointer<QList<PolygonEdge *>> lCandidateEdgeListPtr = lSharpEdges.at(k);
        QList<PolygonEdge *> lCandidateEdgeList = *(lCandidateEdgeListPtr.data());

        lFeatureID++;
        if (lCandidateEdgeList.count() >= 3) {
            QVector<QPointF> lInputPoints;
            // append first point
            lInputPoints << lCandidateEdgeList.at(0)->getPoint1();
            int j;
            for (j = 0; j < lCandidateEdgeList.count() - 1; j++) {
                PolygonEdge *lCurrEdge = lCandidateEdgeList.at(j);
                PolygonEdge *lNextEdge = lCandidateEdgeList.at(j + 1);
                lInputPoints << lNextEdge->getPoint1();
            }
            // append last point - why is this required?
            lInputPoints << lCandidateEdgeList.at(lCandidateEdgeList.count() - 1)->getPoint2();

            // recursively calculate splines
            double lMinX, lMinY, lMaxX, lMaxY;
            getMinMax(lCandidateEdgeList, lMinX, lMinY, lMaxX, lMaxY);
            double lNormalizeFactor = std::max((lMaxY - lMinY), (lMaxX - lMinX));
            // double lSplineTolerance = aTolerance/lNormalizeFactor;
            calcSplineApprox_recursive(lInputPoints, lCandidateEdgeList, aTolerance);

            // At this point all the spline candidates have been identified, and
            // spline error set for each edge. Set a unique feature ID, starting
            // with SPLINE_FEATURE_ID+1 for every new set of points that constitute
            // a spline if spline approximation fails, feature ID stays unchanged.
            PolygonEdge *lPrevEdge = lCandidateEdgeList.at(0);
            lPrevEdge->setFeatureID(lFeatureID);

            int i = 1;
            while (i <= lCandidateEdgeList.count() - 1) {
                lPrevEdge = lCandidateEdgeList.at(i - 1);
                PolygonEdge *lCurrEdge = lCandidateEdgeList.at(i);
                if (lPrevEdge->isSplineCandidate(aTolerance)
                    && lCurrEdge->isSplineCandidate(aTolerance)) {
                    lCurrEdge->setFeatureID(lPrevEdge->getFeatureID());
                } else if (!lPrevEdge->isSplineCandidate(aTolerance)
                           && lCurrEdge->isSplineCandidate(aTolerance)) {
                    lFeatureID++;
                    lCurrEdge->setFeatureID(lFeatureID);
                } else if (!lCurrEdge->isSplineCandidate(aTolerance)) {
                    lCurrEdge->setFeatureID(0);
                }
                i++;
            } // while (i <= lCandidateEdgeList.count()-1)

        } // if (lCandidateEdgeList.count() >= 3)

    } // for (int k = 0; k <= lSharpEdges.count()-1; k++)

    return (lFeatureID - SPLINE_FEATURE_ID);
}

// int PolyFeatureDetection::splineToleranceCheck_method2(QList<PolygonEdge *>
// &aEdgeList,
//                                                       const double
//                                                       aTolerance)
//{
// Step 1 : start by calculating spline going through 3 edges
// Step 2 : if error < tolerance for all 3 edges, add another edge,
// recalculate spline for set of 4 edges
// Step 3 : If error > tolerance then remove pt 1,
// mark edge 1 as non- spline. then add another point and recalculate
// spline for the set of 3 points (back to Step 1)
// Repeat above steps till you reach the last edge in input list.
// int lNumSplines = 0;
// return lNumSplines;
//}

void PolyFeatureDetection::calcSplineApprox_recursive(QVector<QPointF> &aInputPts,
                                                      QList<PolygonEdge *> &aEdgeCandidateList,
                                                      const double aTolerance)
{
    QVector<QPointF> lSplinePoints;
    calcSpline(aInputPts, lSplinePoints);

    aInputPts.clear();
    aInputPts << lSplinePoints; // create new input polygon
    // Tag edges as spline candidates if they qualify for spline approximation
    identifySplineErrors(lSplinePoints, aEdgeCandidateList, aTolerance);
    // remove any edges with spline errors > tolerance and
    // formulate new list of points that are spline candidates
    bool lEdgeListModified = removeEdgeswithSplineErrors(aEdgeCandidateList, aInputPts, aTolerance);
    if (lEdgeListModified && aEdgeCandidateList.count() >= 3) {
        calcSplineApprox_recursive(aInputPts, aEdgeCandidateList, aTolerance);
    }
}

void PolyFeatureDetection::calcSpline(QVector<QPointF> &aInputPts, QVector<QPointF> &aSplineCurvePts)
{
    // aSplineCurvePts is an output parameter, will be recreated every time
    SplineCurveFitter lCurveFitter;
    lCurveFitter.setSplineSize(100 * mPolyPoints->count());

    QPolygonF lInputPoly(aInputPts);
    QPolygonF lSplineCurve = lCurveFitter.fitCurve(lInputPoly);

    aSplineCurvePts.clear();
    aSplineCurvePts << lSplineCurve;
}

bool PolyFeatureDetection::removeEdgeswithSplineErrors(QList<PolygonEdge *> &aEdgeList,
                                                       QVector<QPointF> &aCandidatePts,
                                                       const double aTolerance)
{
    bool aEdgeListModified = false;
    // find index of first non-spline entity.
    int lSplineStartIndex = -1;
    for (int j = 0; j < aEdgeList.count(); j++) {
        PolygonEdge *lEdge = aEdgeList.at(j);
        if (j >= 3 && lEdge->getSplineError() > aTolerance) {
            lSplineStartIndex = j;
            break;
        }
    }
    if (lSplineStartIndex >= 3) {
        // remove all entities upto lSplineStartIndex
        int k = 0;
        while (k >= lSplineStartIndex && aEdgeList.count() >= 3) {
            aEdgeList.removeAt(0);
            aEdgeListModified = true;
            k++;
        }
    }
    // populate new list of points based on removed edges : aCandidatePts
    aCandidatePts.clear(); // aCandidatePts is OUTPUT list
    for (int j = 0; j < aEdgeList.count(); j++) {
        PolygonEdge *lEdge = aEdgeList.at(j);
        aCandidatePts << lEdge->getPoint1();
        if (j == aEdgeList.count() - 1) {
            aCandidatePts << lEdge->getPoint2();
        }
    }

    return aEdgeListModified;
}

void PolyFeatureDetection::identifySplineErrors(QVector<QPointF> &aSplineCurvePts,
                                                QList<PolygonEdge *> &aEdgeList,
                                                const double aTolerance)
{
    for (int j = 0; j < aEdgeList.count(); j++) {
        PolygonEdge *lEdge = aEdgeList.at(j);
        double lDistToSpline = 99999;

        for (int k = 0; k <= aSplineCurvePts.count() - 1; k++) {
            QPointF lSplinePt = aSplineCurvePts.at(k);
            // calculate error/distance of spline point from lEdge
            QPointF p1 = lEdge->getPoint1();
            QPointF p2 = lEdge->getPoint2();
            QPointF lMidPoint((p1.x() + p2.x()) / 2, (p1.y() + p2.y()) / 2);

            qreal lDist1 = sqrt(pow(lSplinePt.x() - p1.x(), 2) + pow(lSplinePt.y() - p1.y(), 2));
            qreal lDist2 = sqrt(pow(lSplinePt.x() - p2.x(), 2) + pow(lSplinePt.y() - p2.y(), 2));
            qreal lDist3 = sqrt(pow(lSplinePt.x() - lMidPoint.x(), 2)
                                + pow(lSplinePt.y() - lMidPoint.y(), 2));

            qreal leastSqrError = sqrt(lDist1 * lDist1 + lDist2 * lDist2 + lDist3 * lDist3);
            if (leastSqrError < lDistToSpline) {
                lDistToSpline = leastSqrError;
            }
        } // for k

        aEdgeList.at(j)->setSplineError(lDistToSpline);
    } // for j
}

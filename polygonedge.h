#ifndef POLYGONEDGE_H
#define POLYGONEDGE_H

#include <QObject>
#include <QPointF>

const int LINE_FEATURE_ID = 99;
const int ARC_FEATURE_ID = 49999;
const int SPLINE_FEATURE_ID = 99999;

const int DEFAULT_SHARPEDGE_ID = 0;
const double EPSILON = 1E-6;

const double DEFAULT_LINE_TOL = 0.001;
const double DEFAULT_ARC_TOL = 0.01;
const double DEFAULT_SPLINE_TOL = 0.05;
const double DEFAULT_SHARP_ANGLE_TOL = 10.0; // degrees

class PolygonEdge : public QObject
{
    Q_OBJECT
public:
    PolygonEdge(const QPointF &ap1, const QPointF &ap2);

    void setPoint1(const QPointF &ap1) { mp1 = ap1; }
    void setPoint2(const QPointF &ap2) { mp2 = ap2; }
    const QPointF &getPoint1() const { return mp1; }
    const QPointF &getPoint2() const { return mp2; }

    void setFeatureID(const long aFeatureID) { mFeatureID = aFeatureID; }
    long getFeatureID() const { return mFeatureID; }

    void setSplineError(const double aSplineError) { mSplineError = aSplineError; }
    double getSplineError() const { return mSplineError; }

    bool isSplineCandidate(const double aSplineTol) const;

    void setHash(const QString &aHashKey) { mHashKey = aHashKey; }
    const QString &getHash(size_t aSeed);

    double getAngle() const;

    long getSharpEdgeID() const { return mSharpEdgeID; }
    void setSharpEdgeID(const long aSharpEdgeID) { mSharpEdgeID = aSharpEdgeID; }

private:
    QPointF mp1, mp2;
    long mFeatureID;
    long mSharpEdgeID;
    QString mHashKey;
    double mSplineError;
};

#endif // POLYGONEDGE_H

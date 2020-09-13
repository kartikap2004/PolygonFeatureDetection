#include "polygonedge.h"
#include <limits.h>
#include <math.h>

PolygonEdge::PolygonEdge(const QPointF &ap1, const QPointF &ap2)
{
    mp1 = ap1;
    mp2 = ap2;
    mHashKey = "";
    mFeatureID = 0;
    mSharpEdgeID = 0;
    mSplineError = 0.0;
}

const QString &PolygonEdge::getHash(size_t aSeed)
{
    if (mHashKey.isEmpty()) {
        QString lKeyStr = "EdgeKey:";
        lKeyStr += "x1{" + QString::number(mp1.x(), 'g', 4) + "}";
        lKeyStr += "y1{" + QString::number(mp1.y(), 'g', 4) + "}";
        lKeyStr += "x2{" + QString::number(mp2.x(), 'g', 4) + "}";
        lKeyStr += "y2{" + QString::number(mp2.y(), 'g', 4) + "}";

        // TO-DO : Also need a way to consider rounding errors for floating point
        // numbers for eg : 0.00001 and 0.0000099 should probably be considered
        // equal - is it possible to generate the same hash keys for doubles
        // within a certain tolerance value? hash_combine(aSeed, lKeyStr); return
        // aSeed;
        mHashKey = lKeyStr;
    }
    return mHashKey;
}

double PolygonEdge::getAngle() const
{
    double y = mp2.y() - mp1.y();
    double x = mp2.x() - mp1.x();
    double lAngle = 0;
    if (fabs(x) <= EPSILON) {
        lAngle = 90;
    } else {
        lAngle = (atan2(y, x)) * (180 / 3.14);
    }
    return lAngle;
}

bool PolygonEdge::isSplineCandidate(const double aSplineTol) const
{
    if (mSplineError <= aSplineTol) {
        return true;
    } else {
        return false;
    }
}

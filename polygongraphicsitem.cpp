#include "polygongraphicsitem.h"
#include <datamarker.h>
#include <math.h>
#include <polygondisplayview.h>
#include <QCursor>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPointF>
#include <QPolygonF>
#include <QSharedPointer>
#include <QStyleOptionGraphicsItem>
#include <QVector>

PolygonGraphicsItem::PolygonGraphicsItem(const QRectF &aRect, QGraphicsView *parent)
    : mParent(parent)
    , mDecomposing(false)
    , mCheckSharpEdges(false)
    , mCheckLines(false)
    , mCheckArcs(false)
    , mCheckSplines(false)
    , mShowMarkers(false)
    , mLineTolerance(DEFAULT_LINE_TOL)
    , mArcTolerance(DEFAULT_ARC_TOL)
    , mSplineTolerance(DEFAULT_SPLINE_TOL)
    , mSharpAngleTolerance(DEFAULT_SHARP_ANGLE_TOL)
    , mPolyFeatureDetection(NULL)
{
    mPointOffset = QPointF(0, 0);
    m_rect = aRect;
    mPointsList = QSharedPointer<QVector<QPointF>>(new QVector<QPointF>());
    setAcceptHoverEvents(true);
    // setAcceptedMouseButtons({Qt::MouseButton::LeftButton,
    // Qt::MouseButton::RightButton});
}

PolygonGraphicsItem::~PolygonGraphicsItem()
{
    mPointsList->clear();
}

void PolygonGraphicsItem::setRect(const QRectF &aRect)
{
    m_rect = aRect;
    update();
}

void PolygonGraphicsItem::setItemScaling(qreal aScaleFactor)
{
    prepareGeometryChange();
    setScale(aScaleFactor);
    update();
}

void PolygonGraphicsItem::setLineTolerance(const double aDeviationVal)
{
    mLineTolerance = aDeviationVal;
}

void PolygonGraphicsItem::setArcTolerance(const double aDeviationVal)
{
    mArcTolerance = aDeviationVal;
}

void PolygonGraphicsItem::setSplineTolerance(const double aDeviationVal)
{
    mSplineTolerance = aDeviationVal;
}

void PolygonGraphicsItem::setSharpAngleTolerance(const double aTolerance)
{
    mSharpAngleTolerance = aTolerance;
}

QPolygonF PolygonGraphicsItem::recalcPolygon(const QPolygonF &aPolygon)
{
    QPointF lGeoCenter = aPolygon.boundingRect().center();
    QPointF lWidgetCenter = m_rect.center();

    prepareGeometryChange();

    QPolygonF lTranslatedPoly = aPolygon.translated(lWidgetCenter - lGeoCenter);
    return lTranslatedPoly;
}

void PolygonGraphicsItem::fitCurve()
{
    /*QPolygonF path;
for (auto lEntry : m_values) {
path << QPointF(lEntry.first, lEntry.second);
}

SplineCurveFitter lCurveFitter;
lCurveFitter.setSplineSize(100
* m_values.size()); // Also used in
SplineChart::recalculateRange()

mFittedCurvePoints = lCurveFitter.fitCurve(path);

for (auto lFittedCurvePt : mFittedCurvePoints) {
QPointF lpositionF = positionForValue(lFittedCurvePt.x(),
lFittedCurvePt.y()); mFittedCurvePts_Mapped << lpositionF;
}
m_fitCurve = true;*/
}

void PolygonGraphicsItem::setPolyPoints(QSharedPointer<QVector<QPointF>> aPointsList)
{
    prepareGeometryChange();

    if (aPointsList != nullptr) {
        if (mPointsList) {
            mPointsList->clear();
        }

        QPolygonF lOrigPolygon;
        lOrigPolygon << *(aPointsList->data());

        // translate to center of the scene
        mPointOffset = m_rect.center() - lOrigPolygon.boundingRect().center();

        for (int i = 0; i < aPointsList->size(); i++) {
            QPointF lPolyPoint = aPointsList->at(i);
            QPointF lTranslatedPt = lPolyPoint + mPointOffset;
            mPointsList->append(QPointF(lTranslatedPt));
        }
        if (mPointsList->count() >= 3) {
            mPointsList->append(aPointsList->at(0) + +mPointOffset);
        }
    }

    if (mPolyFeatureDetection == nullptr) {
        mPolyFeatureDetection = new PolyFeatureDetection(mPointsList);
        mPolyFeatureDetection->createEdgeList(mPolyEdgeList);
    }
    update();
}

void PolygonGraphicsItem::setCheckLineTol(const bool aCheck)
{
    mCheckLines = aCheck;
    if (!mCheckLines) {
        for (auto lEdge : mPolyEdgeList) {
            if (lEdge->getSharpEdgeID() > LINE_FEATURE_ID) {
                lEdge->setFeatureID(LINE_FEATURE_ID);
            }
        }
    }
}

void PolygonGraphicsItem::setCheckArcTol(const bool aCheck)
{
    mCheckArcs = aCheck;
    if (!mCheckArcs) {
        for (auto lEdge : mPolyEdgeList) {
            if (lEdge->getSharpEdgeID() > ARC_FEATURE_ID
                && lEdge->getSharpEdgeID() < LINE_FEATURE_ID) {
                lEdge->setFeatureID(ARC_FEATURE_ID);
            }
        }
    }
}

void PolygonGraphicsItem::setCheckSplineTol(const bool aCheck)
{
    mCheckSplines = aCheck;
    if (!mCheckSplines) {
        for (auto lEdge : mPolyEdgeList) {
            if (lEdge->getSharpEdgeID() > SPLINE_FEATURE_ID
                && lEdge->getSharpEdgeID() < ARC_FEATURE_ID) {
                lEdge->setFeatureID(SPLINE_FEATURE_ID);
            }
        }
    }
}

void PolygonGraphicsItem::setCheckSharpAngleTol(const bool aCheck)
{
    mCheckSharpEdges = aCheck;
    if (!mCheckSharpEdges) {
        // reset Sharp Angle Edge IDs
        for (auto lEdge : mPolyEdgeList) {
            lEdge->setSharpEdgeID(DEFAULT_SHARPEDGE_ID);
        }
    }
}

void PolygonGraphicsItem::paint(QPainter *aPainter,
                                const QStyleOptionGraphicsItem *aOptions,
                                QWidget *aWidget)
{
    aPainter->setRenderHint(QPainter::Antialiasing, true);
    aPainter->setPen(QPen(lightgreen, 0.5));
    if (mPointsList && mPointsList->count() > 0) {
        if (mDecomposing) {
            aPainter->setPen(QPen(Qt::GlobalColor::lightGray, 0.5));
            if (mPolyFeatureDetection != nullptr) {
                int numLines = 0;
                int numArcs = 0;
                int numSplines = 0;
                int numSharpEdges = 0;

                // Reset feature IDs and sharp-edge IDs
                for (auto lEdge : mPolyEdgeList) {
                    lEdge->setFeatureID(0);
                    lEdge->setSharpEdgeID(0);
                }

                double lSharpAngleTol = DEFAULT_SHARP_ANGLE_TOL;
                if (mCheckSplines) {
                    numSplines = mPolyFeatureDetection->splineToleranceCheck(mPolyEdgeList, mSplineTolerance,
                                                                             mCheckSharpEdges, lSharpAngleTol);
                }
                if (mCheckArcs) {
                    numArcs = mPolyFeatureDetection->arcToleranceCheck(mPolyEdgeList, mArcTolerance,
                                                                       mCheckSharpEdges, lSharpAngleTol);
                }
                if (mCheckLines) {
                    numLines = mPolyFeatureDetection->lineToleranceCheck(mPolyEdgeList, mLineTolerance, 
                                                                         mCheckSharpEdges, lSharpAngleTol);
                }
                
                if (mCheckSharpEdges && 
                        !mCheckLines && !mCheckArcs && !mCheckSplines) {
                    numSharpEdges = mPolyFeatureDetection
                                        ->sharpAngleToleranceCheck(mPolyEdgeList,
                                                                   mSharpAngleTolerance);
                    lSharpAngleTol = mSharpAngleTolerance;
                }


                // Draw edges
                for (int i = 0; i < mPolyEdgeList.count(); i++) {
                    PolygonEdge *lPolyEdge = mPolyEdgeList.at(i);
                    long lFeatureID = lPolyEdge->getFeatureID();
                    long lSharpEdgeID = lPolyEdge->getSharpEdgeID();

                    if (lFeatureID > SPLINE_FEATURE_ID) {
                        aPainter->setPen(
                            QPen(Qt::GlobalColor((lFeatureID % 10) + Qt::GlobalColor::red), 0.5));
                    } else if (lFeatureID > ARC_FEATURE_ID) {
                        aPainter->setPen(
                            QPen(Qt::GlobalColor((lFeatureID % 10) + Qt::GlobalColor::green), 0.5));
                    } else if (lFeatureID > LINE_FEATURE_ID) {
                        aPainter->setPen(
                            QPen(Qt::GlobalColor((lFeatureID % 10) + Qt::GlobalColor::blue), 0.5));
                    } else if (lSharpEdgeID > DEFAULT_SHARPEDGE_ID) {
                        aPainter->setPen(
                            QPen(Qt::GlobalColor((lSharpEdgeID % 10) + Qt::GlobalColor::cyan), 0.5));
                    }
                    aPainter->drawLine(lPolyEdge->getPoint1(), lPolyEdge->getPoint2());
                }
            }
            // Draw markers/symbols
            if (mShowMarkers) {
                aPainter->save();
                for (int i = 0; i < mPointsList->count(); i++) {
                    double x2 = mPointsList->at(i).x();
                    double y2 = mPointsList->at(i).y();
                    // draw symbol/marker
                    DataMarker lMarker(QPointF(x2, y2), SymbolType::Plus, grey);
                    lMarker.paint(aPainter, aOptions, aWidget);
                }
                aPainter->restore();
            }
        } else { // if (!mDecomposing)
            QPolygonF lPolygon;
            lPolygon << *mPointsList;
            // QPolygonF lPolygon = recalcPolygon(lOrigPolygon);
            aPainter->save();
            aPainter->setPen(QPen(lightgray, 0.5));
            aPainter->drawPolygon(lPolygon);
            aPainter->restore();

            // draw symbols/markers
            if (mShowMarkers) {
                aPainter->save();
                for (int i = 0; i < mPointsList->count(); i++) {
                    double x1 = mPointsList->at(i).x();
                    double y1 = mPointsList->at(i).y();
                    DataMarker lMarker(QPointF(x1, y1), SymbolType::Plus, grey);
                    lMarker.paint(aPainter, aOptions, aWidget);
                }
                aPainter->restore();
            }
        }
    }
    aPainter->setRenderHint(QPainter::Antialiasing, false);
}

QRectF PolygonGraphicsItem::boundingRect() const
{
    return m_rect;
}

void PolygonGraphicsItem::mousePressEvent(QMouseEvent *event)
{
    QPointF lCursorPos = (event->pos());
    bool lVertexClicked = false;

    for (auto lPoint : *mPointsList) {
        QPointF lMappedPolyPt = lPoint; // mapToScene() ??
        double lDist = sqrt(pow(lCursorPos.y() - lMappedPolyPt.y(), 2)
                            + pow(lCursorPos.x() - lMappedPolyPt.x(), 2));
        if (lDist <= POINT_CLICK_TOLERANCE) {
            QString lStatusBarText = "On vertex click : Translated : " + QString::number(lPoint.x())
                                     + " , " + QString::number(lPoint.y());
            lStatusBarText += "Original : " + QString::number(lPoint.x() - mPointOffset.x()) + " , "
                              + QString::number(lPoint.y() - mPointOffset.y());
            emit updateStatusBarText(lStatusBarText);
            lVertexClicked = true;
            break;
        }
    }

    if (!lVertexClicked) {
        QString lStatusBarText = "On mouse click cursor coordinates : "
                                 + QString::number(lCursorPos.x()) + " , "
                                 + QString::number(lCursorPos.y());
        lStatusBarText += "Original position : "
                          + QString::number(lCursorPos.x() - mPointOffset.x()) + " , "
                          + QString::number(lCursorPos.y() - mPointOffset.y());
        emit updateStatusBarText(lStatusBarText);
    }

    event->accept();
}

QRectF PolygonGraphicsItem::getBoundingRect() const
{
    QPolygonF lPolygon;
    lPolygon << *mPointsList;
    QRectF lRect = lPolygon.boundingRect();
    QPointF lGraphicsScenePt1 = mapToScene(QPointF(lRect.top(), lRect.left()));
    QPointF lGraphicsScenePt2 = mapToScene(QPointF(lRect.bottom(), lRect.right()));
    return QRectF(lGraphicsScenePt1, lGraphicsScenePt2);
}

void PolygonGraphicsItem::setMarkerDisplay(const bool aShow)
{
    mShowMarkers = aShow;
}

void PolygonGraphicsItem::getMinMax(double &aMinX, double &aMinY, double &aMaxX, double &aMaxY)
{
    if (mPolyFeatureDetection != nullptr) {
        mPolyFeatureDetection->getMinMax(mPolyEdgeList, aMinX, aMinY, aMaxX, aMaxY);
    }
}

void PolygonGraphicsItem::mouseMoveEvent(QMouseEvent *event)
{
    QPointF lCursorPos = event->pos();
    QString lStatusBarText = "Mouse Move position : X : " + QString::number(lCursorPos.x())
                             + " Y : " + QString::number(lCursorPos.y());
    event->accept();
    emit updateStatusBarText(lStatusBarText);
}

void PolygonGraphicsItem::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
}

void PolygonGraphicsItem::decompose_polygon(const bool aFindEdges)
{
    mDecomposing = aFindEdges;
    prepareGeometryChange();
    update();
}

void PolygonGraphicsItem::clearData()
{
    mPointsList->clear();
    mPolyEdgeList.clear();
    delete mPolyFeatureDetection;
    mPolyFeatureDetection = NULL;
}

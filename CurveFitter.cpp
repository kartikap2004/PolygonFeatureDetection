#include "CurveFitter.h"
#include "Spline.h"
#include <qmath.h>
#include <qstack.h>
#include <qvector.h>

const int MAX_SPLINE_SIZE = 50000;

CurveFitter::CurveFitter() {}

CurveFitter::~CurveFitter() {}

class SplineCurveFitter::PrivateData {
public:
  PrivateData() : fitMode(SplineCurveFitter::FitAuto), splineSize(250) {}

  Spline spline;
  SplineCurveFitter::FitMode fitMode;
  int splineSize;
};

SplineCurveFitter::SplineCurveFitter() { d_data = new PrivateData; }

SplineCurveFitter::~SplineCurveFitter() { delete d_data; }

void SplineCurveFitter::setSpline(const Spline &spline) {
  d_data->spline = spline;
  d_data->spline.reset();
}

const Spline &SplineCurveFitter::spline() const { return d_data->spline; }

Spline &SplineCurveFitter::spline() { return d_data->spline; }

void SplineCurveFitter::setSplineSize(int splineSize) {
  d_data->splineSize = qMax(splineSize, 10);
  if (d_data->splineSize > MAX_SPLINE_SIZE) {
    d_data->splineSize = MAX_SPLINE_SIZE;
  }
}

int SplineCurveFitter::splineSize() const { return d_data->splineSize; }

QPolygonF SplineCurveFitter::fitCurve(const QPolygonF &points) const {
  const int size = points.size();
  if (size <= 2)
    return points;
  return fitSpline(points);
}

QPolygonF SplineCurveFitter::fitSpline(const QPolygonF &points) const {

  d_data->spline.setPoints(points);

  if (!d_data->spline.isValid())
    return points;

  QPolygonF fittedPoints(d_data->splineSize);

  const double x1 = points[0].x();
  const double x2 = points[int(points.size() - 1)].x();
  const double dx = x2 - x1;
  const double delta = dx / (d_data->splineSize - 1);

  for (int i = 0; i < d_data->splineSize; i++) {
    QPointF &p = fittedPoints[i];

    const double v = x1 + i * delta;
    const double sv = d_data->spline.value(v);

    p.setX(v);
    p.setY(sv);
  }

  d_data->spline.reset();
  return fittedPoints;
}

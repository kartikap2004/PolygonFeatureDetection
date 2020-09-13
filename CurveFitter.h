#ifndef QWT_CURVE_FITTER_H
#define QWT_CURVE_FITTER_H

#include "qglobal.h"
#include <qpolygon.h>
#include <qrect.h>

class Spline;

class CurveFitter {
public:
  virtual ~CurveFitter();

  virtual QPolygonF fitCurve(const QPolygonF &polygon) const = 0;

protected:
  CurveFitter();

private:
  CurveFitter(const CurveFitter &);
  CurveFitter &operator=(const CurveFitter &);
};

class SplineCurveFitter : public CurveFitter {
public:
  enum FitMode { FitAuto, FitSpline, FitParametricSpline };

  SplineCurveFitter();
  virtual ~SplineCurveFitter();

  void setSpline(const Spline &);
  const Spline &spline() const;
  Spline &spline();

  void setSplineSize(int);
  int splineSize() const;

  virtual QPolygonF fitCurve(const QPolygonF &) const;

private:
  QPolygonF fitSpline(const QPolygonF &) const;

  class PrivateData;
  PrivateData *d_data;
};

#endif

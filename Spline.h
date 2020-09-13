#ifndef SPLINE_H
#define SPLINE_H

#include "EigenLibrary/Dense"
#include <qglobal.h>
#include <qpolygon.h>
#include <qvector.h>

class Spline {
public:
  Spline();
  Spline(const Spline &);

  ~Spline();

  Spline &operator=(const Spline &);

  bool setPoints(const QPolygonF &points);
  QPolygonF points() const;

  void reset();

  bool isValid() const;
  double value(double x) const;

protected:
  bool buildNaturalSpline(const QPolygonF &);

private:
  class PrivateData;
  PrivateData *d_data;
};

class Spline::PrivateData {
public:
  // coefficient vectors
  Eigen::RowVectorXd coefficientsA;
  Eigen::RowVectorXd coefficientsB;
  Eigen::RowVectorXd coefficientsC;

  // control points
  QPolygonF points;
};

#endif

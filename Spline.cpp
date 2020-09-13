#include "Spline.h"
#include <qmath.h>

static int lookup(double x, const QPolygonF &values) {
  int i1;
  const int size = values.size();

  if (x <= values[0].x())
    i1 = 0;
  else if (x >= values[size - 2].x())
    i1 = size - 2;
  else {
    i1 = 0;
    int i2 = size - 2;
    int i3 = 0;

    while (i2 - i1 > 1) {
      i3 = i1 + ((i2 - i1) >> 1);

      if (values[i3].x() > x)
        i2 = i3;
      else
        i1 = i3;
    }
  }
  return i1;
}

Spline::Spline() { d_data = new PrivateData; }

Spline::Spline(const Spline &other) { d_data = new PrivateData(*other.d_data); }

Spline &Spline::operator=(const Spline &other) {
  *d_data = *other.d_data;
  return *this;
}

Spline::~Spline() { delete d_data; }

/*Calculate the spline coefficients
  This function will determine the coefficients for a natural spline and store
  them internally*/
bool Spline::setPoints(const QPolygonF &points) {
  const int size = points.size();
  if (size <= 2) {
    reset();
    return false;
  }

  d_data->points = points;

  d_data->coefficientsA.resize(size - 1);
  d_data->coefficientsB.resize(size - 1);
  d_data->coefficientsC.resize(size - 1);

  bool ok = buildNaturalSpline(points);
  if (!ok)
    reset();

  return ok;
}

/*Return points, that have been set by setPoints() */
QPolygonF Spline::points() const { return d_data->points; }

// Free allocated memory and set size to 0
void Spline::reset() {
  d_data->coefficientsA.resize(0);
  d_data->coefficientsB.resize(0);
  d_data->coefficientsC.resize(0);
  d_data->points.resize(0);
}

// True if valid
bool Spline::isValid() const { return d_data->coefficientsA.size() > 0; }

/* Calculate the interpolated function value corresponding
  to a given argument x.*/
double Spline::value(double x) const {
  if (d_data->coefficientsA.size() == 0)
    return 0.0;

  const int i = lookup(x, d_data->points);

  const double delta = x - d_data->points[i].x();
  return (
      (((d_data->coefficientsA[i] * delta) + d_data->coefficientsB[i]) * delta +
       d_data->coefficientsC[i]) *
          delta +
      d_data->points[i].y());
}

/* Determines the coefficients for a natural spline
return true if successful */
bool Spline::buildNaturalSpline(const QPolygonF &points) {
  int i;

  const QPointF *p = points.data();
  const int size = points.size();

  double *aCoeff = d_data->coefficientsA.data();
  double *bCoeff = d_data->coefficientsB.data();
  double *cCoeff = d_data->coefficientsC.data();

  //  set up tridiagonal equation system; use coefficient
  //  vectors as temporary buffers
  QVector<double> h(size - 1);
  for (i = 0; i < size - 1; i++) {
    h[i] = p[i + 1].x() - p[i].x();
    if (h[i] <= 0)
      return false;
  }

  QVector<double> d(size - 1);
  double dy1 = (p[1].y() - p[0].y()) / h[0];
  for (i = 1; i < size - 1; i++) {
    bCoeff[i] = cCoeff[i] = h[i];
    aCoeff[i] = 2.0 * (h[i - 1] + h[i]);

    const double dy2 = (p[i + 1].y() - p[i].y()) / h[i];
    d[i] = 6.0 * (dy1 - dy2);
    dy1 = dy2;
  }

  // solve the above equation

  // L-U Factorization
  for (i = 1; i < size - 2; i++) {
    cCoeff[i] /= aCoeff[i];
    aCoeff[i + 1] -= bCoeff[i] * cCoeff[i];
  }

  // forward elimination
  QVector<double> s(size);
  s[1] = d[1];
  for (i = 2; i < size - 1; i++)
    s[i] = d[i] - cCoeff[i - 1] * s[i - 1];

  // backward elimination
  s[size - 2] = -s[size - 2] / aCoeff[size - 2];
  for (i = size - 3; i > 0; i--)
    s[i] = -(s[i] + bCoeff[i] * s[i + 1]) / aCoeff[i];
  s[size - 1] = s[0] = 0.0;

  // Finally, determine the spline coefficients
  for (i = 0; i < size - 1; i++) {
    aCoeff[i] = (s[i + 1] - s[i]) / (6.0 * h[i]);
    bCoeff[i] = 0.5 * s[i];
    cCoeff[i] =
        (p[i + 1].y() - p[i].y()) / h[i] - (s[i + 1] + 2.0 * s[i]) * h[i] / 6.0;
  }

  return true;
}

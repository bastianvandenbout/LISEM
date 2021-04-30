#ifndef LINEFIT_H
#define LINEFIT_H

#include <vector>
#include <math.h>

struct Point {
  double _x, _y;
};
struct Line {
  double _slope, _yInt;
  double getYforX(double x) {
    return _slope*x + _yInt;
  }
  // Construct line from points
  bool fitPoints(const std::vector<Point> &pts) {
    int nPoints = pts.size();
    if( nPoints < 2 ) {
      // Fail: infinitely many lines passing through this single point
      return false;
    }
    double sumX=0, sumY=0, sumXY=0, sumX2=0;
    for(int i=0; i<nPoints; i++) {
      sumX += pts[i]._x;
      sumY += pts[i]._y;
      sumXY += pts[i]._x * pts[i]._y;
      sumX2 += pts[i]._x * pts[i]._x;
    }
    double xMean = sumX / nPoints;
    double yMean = sumY / nPoints;
    double denominator = sumX2 - sumX * xMean;
    // You can tune the eps (1e-7) below for your specific task
    if( std::fabs(denominator) < 1e-7 ) {
      // Fail: it seems a vertical line
      return false;
    }
    _slope = (sumXY - sumX * yMean) / denominator;
    _yInt = yMean - _slope * xMean;
    return true;
  }
};



#endif // LINEFIT_H

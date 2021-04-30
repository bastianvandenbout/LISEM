#ifndef SHAPEALGEBRA_H
#define SHAPEALGEBRA_H

#include "limits"
#include "math.h"

inline static void GetClosestPointOnLine(double Ax, double Ay, double Bx, double By, double Px, double Py, double *xret, double *yret)
{
  double APx = Px - Ax;
  double APy = Py - Ay;
  double ABx = Bx - Ax;
  double ABy = By - Ay;
  double magAB2 = ABx*ABx + ABy*ABy;
  double ABdotAP = ABx*APx + ABy*APy;
  double t = ABdotAP / magAB2;

  if ( t < 0.0)
  {
        *xret = Ax;
        *yret = Ay;
  }
  else if (t > 1.0)
  {
        *xret = Bx;
        *yret = By;
  }
  else
  {
      if(!std::isnan(t) && std::isfinite(t))
      {
          *xret = Ax + ABx*t;
          *yret = Ay + ABy*t;
      }else {
          *xret = (Ax + Bx)*0.5;
          *yret = (Ay + By)*0.5;
      }
  }

}






#endif // SHAPEALGEBRA_H

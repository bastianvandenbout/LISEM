#ifndef GEOMETRYBASE_H
#define GEOMETRYBASE_H

#include "linear/lsm_vector4.h"
#include "linear/lsm_vector2.h"

#define INFINITYFLTLSM __builtin_inff()

inline LSMVector3 Geometry_ClosestPointOnLine(const LSMVector3 &A, const LSMVector3 &B, const LSMVector3 &C) {
    LSMVector3 d = (C - B).Normalize();
    LSMVector3 v = A - B;
    double t = v.dot(d);
    LSMVector3 P = B + d*t;
    return P;
}

inline LSMVector2 Geometry_ClosestPointOnLine2D(LSMVector2 A, LSMVector2 B, LSMVector2 C) {
    LSMVector2 d = (C - B).Normalize();
    LSMVector2 v = A - B;
    double t = v.dot(d);
    LSMVector2 P = B + d*t;
    return P;
}

inline bool Geometry_PointsOnSameSideLine(LSMVector2 P1, LSMVector2 P2, const LSMVector2 LP1, const LSMVector2 LP2)
{

    LSMVector2 P1L = Geometry_ClosestPointOnLine2D(P1,LP1,LP2);
    LSMVector2 P2L = Geometry_ClosestPointOnLine2D(P2,LP1,LP2);

    if((P1 - P1L).dot(P2-P2L) > 0)
    {
        return true;
    }else
    {
        return false;
    }
}



// LINE/LINE
inline static bool Geometry_lineLine(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, LSMVector2 &p) {

    if(((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1)) == 0 || ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1)) == 0)
    {
        return false;
    }

  // calculate the direction of the lines
  float uA = ((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
  float uB = ((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));


  // if uA and uB are between 0-1, lines are colliding
  if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
  //if (uA >= 0 && uB <= 1) {

    // optionally, draw a circle where the lines meet
    float intersectionX = x1 + (uA * (x2-x1));
    float intersectionY = y1 + (uA * (y2-y1));

    p = LSMVector2(intersectionX,intersectionY);

    return true;
  }
  return false;
}

inline static float Geometry_direction(LSMVector2 pi, LSMVector2 pj, LSMVector2 pk) {
    return (pk.x - pi.x) * (pj.y - pi.y) - (pj.x - pi.x) * (pk.y - pi.y);
}

inline static bool Geometry_onSegment(LSMVector2 pi, LSMVector2 pj, LSMVector2 pk) {
    if (std::min(pi.x, pj.x) <= pk.x && pk.x <= std::max(pi.x, pj.x)
        && std::min(pi.y, pj.y) <= pk.y && pk.y <= std::max(pi.y, pj.y)) {
        return true;
    } else {
        return false;
    }
}

inline static bool Geometry_segmentIntersect(LSMVector2 p1, LSMVector2 p2, LSMVector2 p3, LSMVector2 p4) {
    float d1 = Geometry_direction(p3, p4, p1);
    float d2 = Geometry_direction(p3, p4, p2);
    float d3 = Geometry_direction(p1, p2, p3);
    float d4 = Geometry_direction(p1, p2, p4);

    if (((d1 > 0.0 && d2 <  0.0) || (d1 <  0.0 && d2 >  0.0)) && ((d3 >  0.0 && d4 <  0.0) || (d3 <  0.0 && d4 >  0.0))) {
        return true;
    } else if (d1 ==  0.0 && Geometry_onSegment(p3, p4, p1)) {
        return true;
    } else if (d2 ==  0.0 && Geometry_onSegment(p3, p4, p2)) {
        return true;
    } else if (d3 ==  0.0 && Geometry_onSegment(p1, p2, p3)) {
        return true;
    } else if (d4 ==  0.0 && Geometry_onSegment(p1, p2, p4)) {
        return true;
    } else {
        return false;
    }
}

inline static bool Geometry_PolygonContaintsPoint(std::vector<double> &x, std::vector<double> &y, double px, double py)
{
    int vcount = x.size();

    if(x.size() < 2)
    {
        return false;
    }

    double minx = 0.0;
    double miny = 0.0;
    for(int i = 0; i < vcount; i++)
    {
        if(i == 0)
        {
            minx = x[i];
            miny = y[i];
        }else {
            minx = std::min(minx,x[i]);
            miny = std::min(miny,y[i]);
        }
    }

    LSMVector2 point = LSMVector2(px,py);
     //create a ray (segment) starting from the given point,
    //and to the point outside of polygon.
    LSMVector2 outside(minx - 1.0, miny);
    int intersections = 0;
    //check intersections between the ray and every side of the polygon.

    //check for intersections with internal ring
    for (int i = 0; i < vcount - 1; ++i) {
    if (Geometry_segmentIntersect(point, outside,LSMVector2(x[i],y[i]), LSMVector2(x[i+1],y[i+1]))) {
            intersections++;
        }
    }
    //check the last line
    if (Geometry_segmentIntersect(point, outside,LSMVector2(x[vcount-1],y[vcount-1]), LSMVector2(x[0],y[0]))) {
        intersections++;
    }

    return (intersections % 2 != 0);

}

inline static bool Geometry_PolygonIntersectsLine(std::vector<double> &x, std::vector<double> &y, double px1, double py1, double px2, double py2)
{
    int vcount = x.size();

    if(x.size() < 2)
    {
        return false;
    }

    double minx = 0.0;
    double miny = 0.0;
    for(int i = 0; i < vcount; i++)
    {
        if(i == 0)
        {
            minx = x[i];
            miny = y[i];
        }else {
            minx = std::min(minx,x[i]);
            miny = std::min(miny,y[i]);
        }
    }


    LSMVector2 point = LSMVector2(px1,py1);

     //create a ray (segment) starting from the given point,
    //and to the point outside of polygon.
    LSMVector2 outside(px2,py2);
    int intersections = 0;
    //check intersections between the ray and every side of the polygon.

    //check for intersections with internal ring
    for (int i = 0; i < vcount - 1; ++i) {
    if (Geometry_segmentIntersect(point, outside,LSMVector2(x[i],y[i]), LSMVector2(x[i+1],y[i+1]))) {
            intersections++;
        }
    }
    //check the last line
    if (Geometry_segmentIntersect(point, outside,LSMVector2(x[vcount-1],y[vcount-1]), LSMVector2(x[0],y[0]))) {
        intersections++;
    }

    return (intersections > 0);

}




#endif // GEOMETRYBASE_H

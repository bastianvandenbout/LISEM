#pragma once

#include "linear/lsm_vector4.h"
#include "linear/lsm_vector2.h"
#include "geometrybase.h"
#include "pcrtypes.h"

#define INFINITYFLTLSM __builtin_inff()



// LINE/LINE
inline static bool RectLine_lineLine(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, LSMVector2 &p) {

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

// LINE/RECTANGLE
inline static bool lineRectUnion(float x1, float y1, float x2, float y2, float rx, float ry, float rw, float rh, LSMVector2 & p1, LSMVector2 &p2) {
    // check if the line has hit any of the rectangle's sides
    // uses the Line/Line function below
    LSMVector2 p;
    int n = 0;
    bool left =   RectLine_lineLine(x1,y1,x2,y2, rx,ry,rx, ry+rh,p);
    if(left)
    {
        p1 = p;
        n++;
    }
    bool right =  RectLine_lineLine(x1,y1,x2,y2, rx+rw,ry, rx+rw,ry+rh,p);
    if(right)
    {
        if(n > 0)
        {
          p2 = p;
          return true;
        }else
        {
            p1 = p;
        }
        n++;
    }
    bool top =    RectLine_lineLine(x1,y1,x2,y2, rx,ry, rx+rw,ry,p);
    if(top)
    {
        if(n > 0)
        {
          p2 = p;
          return true;
        }else
        {
            p1 = p;
        }
        n++;
    }
    bool bottom = RectLine_lineLine(x1,y1,x2,y2, rx,ry+rh, rx+rw,ry+rh,p);
    if(bottom)
    {
        if(n > 0)
        {
          p2 = p;
          return true;
        }else
        {
            p1 = p;
        }
        n++;
    }

    std::cout << "n_intersect "  << n << std::endl;

    if(n == 2)
    {
        //just return, double intersection

    }else if(n == 1)
    {
        if(x1 >= rx && x1 <= rx + rw && y1 >= ry && y1 <= ry + rh)
        {
            p2 = LSMVector2(x1,y1);
        }else {
            p2 = LSMVector2(x2,y2);
        }

    }else // n == 0
    {
        //set both to zero
        p1 = LSMVector2(x1,y1);
        p2 = LSMVector2(x2,y2);
    }

    // if ANY of the above are true, the line
    // has hit the rectangle
    if (left || right || top || bottom) {
      return true;
    }
    return false;

}

// LINE/RECTANGLE
inline static bool lineRect(float x1, float y1, float x2, float y2, float rx, float ry, float rw, float rh, LSMVector2 & p1, LSMVector2 &p2) {

  // check if the line has hit any of the rectangle's sides
  // uses the Line/Line function below
  LSMVector2 p;
  int n = 0;
  bool left =   RectLine_lineLine(x1,y1,x2,y2, rx,ry,rx, ry+rh,p);
  if(left)
  {
      p1 = p;
      n++;
  }
  bool right =  RectLine_lineLine(x1,y1,x2,y2, rx+rw,ry, rx+rw,ry+rh,p);
  if(right)
  {
      if(n > 0)
      {
        p2 = p;
        return true;
      }else
      {
          p1 = p;
      }
      n++;
  }
  bool top =    RectLine_lineLine(x1,y1,x2,y2, rx,ry, rx+rw,ry,p);
  if(top)
  {
      if(n > 0)
      {
        p2 = p;
        return true;
      }else
      {
          p1 = p;
      }
      n++;
  }
  bool bottom = RectLine_lineLine(x1,y1,x2,y2, rx,ry+rh, rx+rw,ry+rh,p);
  if(bottom)
  {
      if(n > 0)
      {
        p2 = p;
        return true;
      }else
      {
          p1 = p;
      }
      n++;
  }

  // if ANY of the above are true, the line
  // has hit the rectangle
  if (left || right || top || bottom) {
    return true;
  }
  return false;
}






inline double TriangleArea(LSMVector3 vpr0,LSMVector3 vpr1,LSMVector3 vpr2)
{

    double a = (vpr0-vpr1).length();
    double b = (vpr2-vpr1).length();
    double c = (vpr2-vpr0).length();

    double s = 0.5 * (a+b+c);

    double areasq = s*(s-a)*(s-b)*(s-c);

    return (areasq > 0.0?std::sqrt(areasq):0.0);
}

inline double DistancePointLine(const LSMVector3 &A, const LSMVector3 &B, const LSMVector3 &C) {
    LSMVector3 d = (C - B).Normalize();
    LSMVector3 v = A - B;
    double t = v.dot(d);
    LSMVector3 P = B + d*t;
    return (P-A).length();
}

static inline bool further(const LSMVector2 &p1, const LSMVector2 &p2, const LSMVector2 &p3)
{
    float dis1 = (p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y);
    float dis2 = (p3.x - p1.x) * (p3.x - p1.x) + (p3.y - p1.y) * (p3.y - p1.y);
    return dis1 > dis2;
}
static inline float sign (const LSMVector2 &p1, const LSMVector2 &p2, const LSMVector2 &p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

static inline bool TTO_PointInTriangle (const LSMVector2 &pt, const LSMVector2 &v1, const LSMVector2 &v2, const LSMVector2 &v3)
{
    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = sign(pt, v1, v2);
    d2 = sign(pt, v2, v3);
    d3 = sign(pt, v3, v1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}


// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
static inline int TTO_orientation(const LSMVector2 &p, const LSMVector2 &q, const LSMVector2 &r)
{
    // See https://www.geeksforgeeks.org/orientation-3-ordered-points/
    // for details of below formula.
    int val = (q.y - p.y) * (r.x - q.x) -
              (q.x - p.x) * (r.y - q.y);

    if (val == 0) return 0;  // colinear

    return (val > 0)? 1: 2; // clock or counterclock wise
}


static inline LSMVector2 TTO_LineLine_Intersect(const LSMVector2 &p1, const LSMVector2 &p2, const LSMVector2 &p3, const LSMVector2 &p4)
{
    LSMVector2 nullres;
    pcr::setMV(nullres.x);

    // Store the values for fast access and easy
    // equations-to-code conversion
    float x1 = p1.x, x2 = p2.x, x3 = p3.x, x4 = p4.x;
    float y1 = p1.y, y2 = p2.y, y3 = p3.y, y4 = p4.y;

    float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    // If d is zero, there is no intersection
    if (d == 0) return nullres;

    // Get the x and y
    float pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
    float x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
    float y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;

    // Check if the x and y coordinates are within both lines
    if ( x < std::min(x1, x2) || x > std::max(x1, x2) ||
    x < std::min(x3, x4) || x > std::max(x3, x4) ) return nullres;
    if ( y < std::min(y1, y2) || y > std::max(y1, y2) ||
    y < std::min(y3, y4) || y > std::max(y3, y4) ) return nullres;

    // Return the point of intersection
    LSMVector2 ret;
    ret.x = x;
    ret.y = y;
    return ret;

}


static inline float TTO_AreaOfTriangle(const LSMVector2 &p1, const LSMVector2 &p2, const LSMVector2 &p3)
{

    double dArea = ((p2.x - p1.x)*(p3.y - p1.y) - (p3.x - p1.x)*(p2.y - p1.y))/2.0;
    return (dArea > 0.0) ? dArea : -dArea;
}


//find the overlapping area between two triangles
//does not do axis-aligned bounding box check for collision
//this algorithm might be terribly inefficient, but couldnt really find information on a faster one (except for generic polygon clipping libraries like ClipperLib or GeometryTools
//This specific functions aims only at two triangles with clockwise winding order.
//It functions by first finding all intersections
//then, start at a vertex and keep following the triangles edges untill we hit an intersection.
//in case of an intersection, check if we need to switch to the other triangle
//repeat untill we find a point which was already checked.
//finally, calculate area of convex polygon (between 3 and 6 vertices) by simple triangulation from some point within the polygon
static inline float TTO_Triangle_Overlap(LSMVector2 t1_p1,LSMVector2 t1_p2,LSMVector2 t1_p3,LSMVector2 t2_p1,LSMVector2 t2_p2,LSMVector2 t2_p3)
{


    //9 combinations
    //there should be at most 6 intersections
    LSMVector2 res_11 =TTO_LineLine_Intersect(t1_p1,t1_p2,t2_p1,t2_p2);
    LSMVector2 res_12 =TTO_LineLine_Intersect(t1_p1,t1_p2,t2_p2,t2_p3);
    LSMVector2 res_13 =TTO_LineLine_Intersect(t1_p1,t1_p2,t2_p3,t2_p1);
    LSMVector2 res_21 =TTO_LineLine_Intersect(t1_p2,t1_p3,t2_p1,t2_p2);
    LSMVector2 res_22 =TTO_LineLine_Intersect(t1_p2,t1_p3,t2_p2,t2_p3);
    LSMVector2 res_23 =TTO_LineLine_Intersect(t1_p2,t1_p3,t2_p3,t2_p1);
    LSMVector2 res_31 =TTO_LineLine_Intersect(t1_p3,t1_p1,t2_p1,t2_p2);
    LSMVector2 res_32 =TTO_LineLine_Intersect(t1_p3,t1_p1,t2_p2,t2_p3);
    LSMVector2 res_33 =TTO_LineLine_Intersect(t1_p3,t1_p1,t2_p3,t2_p1);

    int intersect_1 = (pcr::isMV(res_11.x)?0:1) + (pcr::isMV(res_12.x)?0:1) + (pcr::isMV(res_13.x)?0:1);
    int intersect_2 = (pcr::isMV(res_21.x)?0:1) + (pcr::isMV(res_22.x)?0:1) + (pcr::isMV(res_23.x)?0:1);
    int intersect_3 = (pcr::isMV(res_31.x)?0:1) + (pcr::isMV(res_32.x)?0:1) + (pcr::isMV(res_33.x)?0:1);

    int intersect_21 = (pcr::isMV(res_11.x)?0:1) + (pcr::isMV(res_21.x)?0:1) + (pcr::isMV(res_31.x)?0:1);
    int intersect_22 = (pcr::isMV(res_12.x)?0:1) + (pcr::isMV(res_22.x)?0:1) + (pcr::isMV(res_32.x)?0:1);
    int intersect_23 = (pcr::isMV(res_13.x)?0:1) + (pcr::isMV(res_23.x)?0:1) + (pcr::isMV(res_33.x)?0:1);

    //total intersections should be betweeen 2 and 6
    //3 checks of wether vertices of triangle 1 are inside triangle 2

    //std::cout  << intersect_1 << "  " << intersect_2 << "  " << intersect_3 << std::endl;
    std::vector<LSMVector2> pointlist;

    bool is_in = TTO_PointInTriangle(t1_p1,t2_p1,t2_p2,t2_p3);

    //one of the triangles is completely within the other
    if((intersect_1 + intersect_2 + intersect_3 )< 2)
    {
        if(is_in == 0)
        {
            return TTO_AreaOfTriangle(t2_p1,t2_p2,t2_p3);
        }else
        {
            return TTO_AreaOfTriangle(t1_p1,t1_p2,t1_p3);
        }
    }


    //start looping over triangle 1
    int index_current = 0;
    int triangle_current = 1;//1 or 2;
    int intersect_l1[3] = {intersect_1,intersect_2,intersect_3};
    int intersect_l2[3] = {intersect_21,intersect_22,intersect_23};

    LSMVector2 * res_t1[3] = {&t1_p1,&t1_p2,&t1_p3};
    LSMVector2 * res_t2[3] = {&t2_p1,&t2_p2,&t2_p3};

    LSMVector2 * res_intersect[9] = {&res_11,&res_12,&res_13,&res_21,&res_22,&res_23,&res_31,&res_32,&res_33};
    LSMVector2 * res_intersect2[9] = {&res_11,&res_21,&res_31,&res_12,&res_22,&res_32,&res_13,&res_23,&res_33};

    bool resd_intersect[9] = {false,false,false,false,false,false,false,false,false};
    bool resd_intersect2[9] = {false,false,false,false,false,false,false,false,false};

    int intersect1_oi[9] = {0,3,6,1,4,7,2,5,8};
    int intersect2_oi[9] = {0,3,6,1,4,7,2,5,8};

    bool done_t1[3] = {false,false,false};
    bool done_t2[3] = {false,false,false};


    bool skip_one = false;

    while(true)
    {

        bool skip_one_this = skip_one;
        bool skip_next = false;
        skip_one = false;

        //std::cout  << "triangle current " << triangle_current << " " << index_current << "   in: "<< is_in << std::endl;

        if(triangle_current == 1)
        {
            if(is_in)
            {
                pointlist.push_back(*res_t1[index_current]);

            }
            done_t1[index_current] = true;

            //std::cout  << "number of intersections " << intersect_l1[index_current] << "  "  << skip_one_this <<  std::endl;


            if((intersect_l1[index_current] == 1 && !skip_one_this) || (intersect_l1[index_current] == 2 && skip_one_this))
            {
                const LSMVector2 *tempp1 = res_intersect[index_current*3 + 0];
                const LSMVector2 *tempp2 = res_intersect[index_current*3 + 1];
                const LSMVector2 *tempp3 = res_intersect[index_current*3 + 2];

                bool donep1 = resd_intersect[index_current*3 + 0];
                bool donep2 = resd_intersect[index_current*3 + 1];
                bool donep3 = resd_intersect[index_current *3+ 2];

                int ndone = (donep1? 1:0) + (donep2?1:0) + (donep3?1:0);


                if((intersect_l1[index_current] == 1 && !skip_one_this) && ndone == 1)
                {
                    //std::cout  << "all interceptions done, break" << std::endl;
                    break;
                }else if(ndone == 2)
                {
                    //std::cout  << "all interceptions done, break" << std::endl;
                    break;
                }


                int index_switch = 0;
                if(!pcr::isMV(tempp1->x) && !donep1)
                {
                    pointlist.push_back(*tempp1);
                    index_switch = 0;
                    resd_intersect[index_current*3 + 0] = true;
                    resd_intersect2[intersect1_oi[index_current*3 + 0]] = true;

                }
                if(!pcr::isMV(tempp2->x) && !donep2)
                {
                    pointlist.push_back(*tempp2);
                    index_switch = 1;
                    resd_intersect[index_current*3 + 1] = true;
                    resd_intersect2[intersect1_oi[index_current*3 + 1]] = true;

                }
                if(!pcr::isMV(tempp3->x) &&!donep3)
                {
                    pointlist.push_back(*tempp3);
                    index_switch = 2;
                    resd_intersect[index_current*3 + 2] = true;
                    resd_intersect2[intersect1_oi[index_current*3 + 2]] = true;
                }


                if(intersect_l1[index_current] == 1)
                {
                    is_in = !is_in;
                }

                //if we moved out of the other triangle, switch witch triangle we follow
                if(!is_in)
                {
                    triangle_current = 2;
                    index_current = index_switch;
                    skip_next = true;
                    skip_one = true;

                    //std::cout  << "switch" << std::endl;


                }

            }else if(intersect_l1[index_current] == 2)
            {
                //this must mean the line to the next vertex moves into and out of the other triangle
                //we add both to the point list, and must continue with the other triangle
                const LSMVector2 *tempp1 = res_intersect[index_current*3 + 0];
                const LSMVector2 *tempp2 = res_intersect[index_current*3 + 1];
                const LSMVector2 *tempp3 = res_intersect[index_current*3 + 2];

                bool donep1 = resd_intersect[index_current*3 + 0];
                bool donep2 = resd_intersect[index_current*3 + 1];
                bool donep3 = resd_intersect[index_current*3 + 2];


                int ndone = (donep1? 1:0) + (donep2?1:0) + (donep3?1:0);

                if(ndone == 2)
                {
                    //std::cout  << "all interceptions done, break" << std::endl;
                    break;
                }

                is_in = false;

                if(pcr::isMV(tempp1->x))
                {
                    bool fur = further(*res_t1[index_current],*tempp2,*tempp3);
                    //std::cout  << "intercept 2 and 3" << std::endl;
                    if(!fur)
                    {
                        //add closer one, then further one
                        pointlist.push_back(*tempp2);
                        pointlist.push_back(*tempp3);

                        resd_intersect[index_current*3 + 1] = true;
                        resd_intersect[index_current*3 + 2] = true;
                        resd_intersect2[intersect1_oi[index_current*3 + 1]] = true;
                        resd_intersect2[intersect1_oi[index_current*3 + 2]] = true;

                        triangle_current = 2;
                        index_current = 2;
                        skip_next = true;
                        skip_one = true;

                        //std::cout  << "switch" << std::endl;
                    }else
                    {
                        //add closer one, then further one
                        pointlist.push_back(*tempp3);
                        pointlist.push_back(*tempp2);

                        resd_intersect[index_current*3 + 1] = true;
                        resd_intersect[index_current*3 + 2] = true;
                        resd_intersect2[intersect1_oi[index_current*3 + 1]] = true;
                        resd_intersect2[intersect1_oi[index_current*3 + 2]] = true;

                        triangle_current = 2;
                        index_current = 1;
                        skip_next = true;
                        skip_one = true;
                        //std::cout  << "switch" << std::endl;

                    }

                }else if(pcr::isMV(tempp2->x))
                {
                    //std::cout  << "intercept 1 and 3" << std::endl;
                    bool fur = further(*res_t1[index_current],*tempp1,*tempp3);
                    if(!fur)
                    {
                        //add closer one, then further one
                        pointlist.push_back(*tempp1);
                        pointlist.push_back(*tempp3);

                        resd_intersect[index_current*3 + 0] = true;
                        resd_intersect[index_current*3 + 2] = true;
                        resd_intersect2[intersect1_oi[index_current*3 + 0]] = true;
                        resd_intersect2[intersect1_oi[index_current*3 + 2]] = true;

                        triangle_current = 2;
                        index_current = 2;
                        skip_next = true;
                        skip_one = true;
                        //std::cout  << "switch" << std::endl;

                    }else
                    {
                        //add closer one, then further one
                        pointlist.push_back(*tempp3);
                        pointlist.push_back(*tempp1);

                        resd_intersect[index_current*3 + 0] = true;
                        resd_intersect[index_current*3 + 2] = true;
                        resd_intersect2[intersect1_oi[index_current*3 + 0]] = true;
                        resd_intersect2[intersect1_oi[index_current*3 + 2]] = true;

                        triangle_current = 2;
                        index_current = 0;
                        skip_next = true;
                        skip_one = true;

                        //std::cout  << "switch" << std::endl;

                    }
                }else
                {
                    bool fur = further(*res_t1[index_current],*tempp1,*tempp2);
                    //std::cout  << "intercept 1 and 2" << std::endl;
                    if(!fur)
                    {
                        //add closer one, then further one
                        pointlist.push_back(*tempp1);
                        pointlist.push_back(*tempp2);

                        resd_intersect[index_current*3 + 0] = true;
                        resd_intersect[index_current*3 + 1] = true;
                        resd_intersect2[intersect1_oi[index_current*3 + 0]] = true;
                        resd_intersect2[intersect1_oi[index_current*3 + 1]] = true;

                        triangle_current = 2;
                        index_current = 1;
                        skip_next = true;
                        skip_one = true;
                        //std::cout  << "switch" << std::endl;

                    }else
                    {
                        //add closer one, then further one
                        pointlist.push_back(*tempp2);
                        pointlist.push_back(*tempp1);

                        resd_intersect[index_current*3 + 0] = true;
                        resd_intersect[index_current*3 + 1] = true;
                        resd_intersect2[intersect1_oi[index_current*3 + 0]] = true;
                        resd_intersect2[intersect1_oi[index_current*3 + 1]] = true;

                        triangle_current = 2;
                        index_current = 0;
                        skip_next = true;
                        skip_one = true;

                        //std::cout  << "switch" << std::endl;
                    }

                }
            }

            if(intersect_l1[index_current] == 1 && skip_one_this)
            {
                is_in = !is_in;
            }
        }else
        {

            if(is_in)
            {
                pointlist.push_back(*res_t2[index_current]);
            }
            done_t2[index_current] = true;

            //std::cout  << "number of intersections " << intersect_l2[index_current] << "  "  << skip_one_this <<  std::endl;

            if((intersect_l2[index_current] == 1 && !skip_one_this) || (intersect_l2[index_current] == 2 && skip_one_this))
            {

                const LSMVector2 *tempp1 = res_intersect2[index_current*3 + 0];
                const LSMVector2 *tempp2 = res_intersect2[index_current*3 + 1];
                const LSMVector2 *tempp3 = res_intersect2[index_current*3 + 2];

                bool donep1 = resd_intersect2[index_current*3 + 0];
                bool donep2 = resd_intersect2[index_current*3 + 1];
                bool donep3 = resd_intersect2[index_current*3 + 2];

                int ndone = (donep1? 1:0) + (donep2?1:0) + (donep3?1:0);


                if((intersect_l2[index_current] == 1 && !skip_one_this) && ndone == 1)
                {
                    //std::cout  << "all interceptions done, break" << std::endl;
                    break;
                }else if(ndone == 2)
                {
                    //std::cout  << "all interceptions done, break" << std::endl;
                    break;
                }


                int index_switch = 0;
                if(!pcr::isMV(tempp1->x) && !donep1)
                {
                    pointlist.push_back(*tempp1);
                    index_switch = 0;
                    resd_intersect2[index_current*3 + 0] = true;
                    resd_intersect[intersect2_oi[index_current*3 + 0]] = true;
                    //std::cout  << "intersect with egde 0" << std::endl;
                }
                if(!pcr::isMV(tempp2->x) && !donep2)
                {
                    pointlist.push_back(*tempp2);
                    index_switch = 1;
                    resd_intersect2[index_current*3 + 1] = true;
                    resd_intersect[intersect2_oi[index_current*3 + 1]] = true;
                    //std::cout  << "intersect with egde 1" << std::endl;
                }
                if(!pcr::isMV(tempp3->x) && !donep3)
                {
                    pointlist.push_back(*tempp3);
                    index_switch = 2;
                    resd_intersect2[index_current*3 + 2] = true;
                    resd_intersect[intersect2_oi[index_current*3 + 2]] = true;
                    //std::cout  << "intersect with egde 2" << std::endl;
                }


                if(intersect_l2[index_current] == 1)
                {
                    is_in = !is_in;
                }

                //if we moved into the other triangle, switch witch triangle we follow
                if(!is_in)
                {
                    triangle_current = 1;
                    index_current = index_switch;
                    skip_next = true;
                    skip_one = true;

                    //std::cout  << "switch" << std::endl;
                }

            }else if(intersect_l2[index_current] == 2)
            {
                //this must mean the line to the next vertex moves into and out of the other triangle
                //we add both to the point list, and must continue with the other triangle
                const LSMVector2 *tempp1 = res_intersect2[index_current*3 + 0];
                const LSMVector2 *tempp2 = res_intersect2[index_current*3 + 1];
                const LSMVector2 *tempp3 = res_intersect2[index_current*3 + 2];

                bool donep1 = resd_intersect2[index_current*3 + 0];
                bool donep2 = resd_intersect2[index_current*3 + 1];
                bool donep3 = resd_intersect2[index_current*3 + 2];


                int ndone = (donep1? 1:0) + (donep2?1:0) + (donep3?1:0);

                if(ndone == 2)
                {
                    //std::cout  << "all interceptions done, break" << std::endl;
                    break;
                }
                is_in = false;

                if(pcr::isMV(tempp1->x))
                {
                    bool fur = further(*res_t2[index_current],*tempp2,*tempp3);

                    if(!fur)
                    {
                        //add closer one, then further one
                        pointlist.push_back(*tempp2);
                        pointlist.push_back(*tempp3);

                        resd_intersect2[index_current*3 + 1] = true;
                        resd_intersect2[index_current*3 + 2] = true;
                        resd_intersect[intersect2_oi[index_current*3 + 1]] = true;
                        resd_intersect[intersect2_oi[index_current*3 + 2]] = true;

                        triangle_current = 1;
                        index_current = 2;
                        skip_next = true;
                        skip_one = true;

                        //std::cout  << "switch" << std::endl;

                    }else
                    {
                        //add closer one, then further one
                        pointlist.push_back(*tempp3);
                        pointlist.push_back(*tempp2);

                        resd_intersect2[index_current*3 + 1] = true;
                        resd_intersect2[index_current*3 + 2] = true;
                        resd_intersect[intersect2_oi[index_current*3 + 1]] = true;
                        resd_intersect[intersect2_oi[index_current*3 + 2]] = true;

                        triangle_current = 1;
                        index_current = 1;
                        skip_next = true;
                        skip_one = true;

                        //std::cout  << "switch" << std::endl;

                    }

                }else if(pcr::isMV(tempp2->x))
                {
                    bool fur = further(*res_t2[index_current],*tempp1,*tempp3);
                    if(!fur)
                    {
                        //add closer one, then further one
                        pointlist.push_back(*tempp1);
                        pointlist.push_back(*tempp3);

                        resd_intersect2[index_current*3 + 0] = true;
                        resd_intersect2[index_current*3 + 2] = true;
                        resd_intersect[intersect2_oi[index_current*3 + 0]] = true;
                        resd_intersect[intersect2_oi[index_current*3 + 2]] = true;

                        triangle_current = 1;
                        index_current = 2;
                        skip_next = true;
                        skip_one = true;

                        //std::cout  << "switch" << std::endl;

                    }else
                    {
                        //add closer one, then further one
                        pointlist.push_back(*tempp3);
                        pointlist.push_back(*tempp1);

                        resd_intersect2[index_current*3+ 0] = true;
                        resd_intersect2[index_current*3+ 2] = true;
                        resd_intersect[intersect2_oi[index_current*3 + 0]] = true;
                        resd_intersect[intersect2_oi[index_current*3 + 2]] = true;

                        triangle_current = 1;
                        index_current = 0;
                        skip_next = true;
                        skip_one = true;

                        //std::cout  << "switch" << std::endl;

                    }
                }else
                {
                    bool fur = further(*res_t2[index_current],*tempp1,*tempp2);
                    if(!fur)
                    {
                        //add closer one, then further one
                        pointlist.push_back(*tempp1);
                        pointlist.push_back(*tempp2);

                        resd_intersect2[index_current*3 + 0] = true;
                        resd_intersect2[index_current*3 + 1] = true;
                        resd_intersect[intersect2_oi[index_current*3 + 0]] = true;
                        resd_intersect[intersect2_oi[index_current*3 + 1]] = true;

                        triangle_current = 1;
                        index_current = 1;
                        skip_next = true;
                        skip_one = true;

                        //std::cout  << "switch" << std::endl;

                    }else
                    {
                        //add closer one, then further one
                        pointlist.push_back(*tempp2);
                        pointlist.push_back(*tempp1);

                        resd_intersect2[index_current*3 + 0] = true;
                        resd_intersect2[index_current*3 + 1] = true;
                        resd_intersect[intersect2_oi[index_current*3 + 0]] = true;
                        resd_intersect[intersect2_oi[index_current*3 + 1]] = true;

                        triangle_current = 1;
                        index_current = 0;
                        skip_next = true;
                        skip_one = true;


                        //std::cout  << "switch" << std::endl;

                    }

                }
            }

            if(intersect_l2[index_current] == 1 && skip_one_this)
            {
                is_in = !is_in;
            }
        }

        if(!skip_next)
        {
            //std::cout  << "no intersections, next point" << std::endl;
            if(triangle_current == 1)
            {
                done_t1[index_current] = true;
                index_current ++;
                if(index_current > 2)
                {
                    index_current = 0;
                }
                if(done_t1[index_current] == true)
                {
                    //std::cout  << "next point done, break" << std::endl;
                    break;
                }


            }else
            {
                done_t2[index_current] = true;
                index_current ++;
                if(index_current > 2)
                {
                    index_current = 0;
                }
                if(done_t2[index_current] == true)
                {
                    //std::cout  << "next point done, break" << std::endl;
                    break;
                }
            }
        }
    }

    //resulting polygon with anywhere between 3 and 6 vertices

    //std::cout  << " pointlist size " << pointlist.size()<< std::endl;
    if(pointlist.size() < 3)
    {
        return 0.0;
    }
    //find any point within this polygon
    LSMVector2 insidep = LSMVector2((pointlist[0].x + pointlist[1].x)*0.5f,(pointlist[0].y + pointlist[1].y)*0.5f);

    //calculate area
    //for the intersection (which is a convex polygon, this can be done by making a fan of triangles relative to any point within the polygon

    float size = 0.0;
    for(int i = 0; i < pointlist.size(); i++)
    {
          size += TTO_AreaOfTriangle(insidep,pointlist[i],i == pointlist.size()-1? pointlist[0]:pointlist[i+1]);
    }

    return size;
}



struct Ray
{

    LSMVector3 orig;
    LSMVector3 dir;

    inline Ray(LSMVector3 begin, LSMVector3 end)
    {
        orig = begin;
        dir = end - begin;
    }
};

/* Ray-triangle intersection routine */

static inline float rayTriangleIntersect(Ray r, const LSMVector3 &v0, const LSMVector3 &v1, const LSMVector3 &v2)
{
  LSMVector3 v0v1 = v1-v0;
  LSMVector3 v0v2 = v2-v0;

  LSMVector3 pvec = LSMVector3::CrossProduct(r.dir, v0v2);

  float det = v0v1.dot(pvec);

  if (det < 1e-20)
  {
    return -INFINITYFLTLSM;
  }

  float invDet = 1.0 / det;

  LSMVector3 tvec = r.orig-v0;

  float u = tvec.dot(pvec) * invDet;

  if (u < 0.0 || u > 1.0)
  {
    return -INFINITYFLTLSM;
  }

  LSMVector3 qvec = LSMVector3::CrossProduct(tvec, v0v1);

  float v = r.dir.dot(qvec) * invDet;

  if (v < 0.0 || u + v > 1.0)
  {
    return -INFINITYFLTLSM;
  }

  return v0v2.dot(qvec) * invDet;
}


typedef struct TSquare
{
    LSMVector3 p1;
    LSMVector3 p2;
    LSMVector3 p3;
    LSMVector3 p4;
    inline TSquare(LSMVector3 pi1,LSMVector3 pi2,LSMVector3 pi3,LSMVector3 pi4)
    {
        p1 = pi1;
        p2 = pi2;
        p3 = pi3;
        p4 = pi4;
    }
} TSquare;

typedef struct LSMStructTriangle
{
    LSMVector3 p1;
    LSMVector3 p2;
    LSMVector3 p3;
    inline LSMStructTriangle(LSMVector3 pi1,LSMVector3 pi2,LSMVector3 pi3)
    {
        p1 = pi1;
        p2 = pi2;
        p3 = pi3;
    }
} LSMStructTriangle;


inline LSMVector3 TriangleLineIntersect(LSMVector3 p1, LSMVector3 p2, LSMStructTriangle &sq)
{
    Ray r(p1,p2);
    float t1 = rayTriangleIntersect(r,sq.p1,sq.p2,sq.p3);
    if(t1 != -INFINITYFLTLSM && t1 >= 0.0 && t1 <= 1.0)
    {
        return r.orig + r.dir * t1;
    }
    t1 = rayTriangleIntersect(r,sq.p1,sq.p3,sq.p2);
    if(t1 != -INFINITYFLTLSM && t1 >= 0.0 && t1 <= 1.0)
    {
        return r.orig + r.dir * t1;
    }
    LSMVector3 ret;
    ret.x = -INFINITYFLTLSM;
    return ret;
}

inline LSMVector3 SquareLineIntersect(LSMVector3 p1, LSMVector3 p2, TSquare &sq)
{
    Ray r(p1,p2);
    float t1 = rayTriangleIntersect(r,sq.p1,sq.p2,sq.p3);
    if(t1 != -INFINITYFLTLSM && t1 >= 0.0 && t1 <= 1.0)
    {
        return r.orig + r.dir * t1;
    }

    float t2 = rayTriangleIntersect(r,sq.p1,sq.p3,sq.p2);
    if(t2 != -INFINITYFLTLSM && t2 >= 0.0 && t2 <= 1.0)
    {
        return r.orig + r.dir * t2;
    }

    t1 = rayTriangleIntersect(r,sq.p2,sq.p4,sq.p3);
    if(t1 != -INFINITYFLTLSM && t1 >= 0.0 && t1 <= 1.0)
    {
        return r.orig + r.dir * t1;
    }

    t2 = rayTriangleIntersect(r,sq.p2,sq.p3,sq.p4);
    if(t2 != -INFINITYFLTLSM && t2 >= 0.0 && t2 <= 1.0)
    {
        return r.orig + r.dir * t2;
    }
    LSMVector3 ret;
    ret.x = -INFINITYFLTLSM;
    return ret;
}

inline double TetrahedronVolume(LSMVector3 vt,LSMVector3 v0,LSMVector3 v1,LSMVector3 v2)
{
    return (1.0/6.0) * std::fabs(LSMVector3::CrossProduct((v0-vt),(v1-vt)).dot(v2-vt));
}

inline double TriangleWaterFlowIntersect(LSMVector3 p1, LSMVector3 p2, LSMVector3 p3, BoundingBox extent, double h, double dhdx, double dhdy, double z, double dzdx, double dzdy, double &vol, double &area_proj, LSMVector3 &com, LSMVector3 vel_dir, double &vel_proj_area, LSMVector3 hydro_cof, double &hydro_total, std::vector<LSMVector3> &corners)
{
    if(!BoundingBox(p1,p2,p3).Overlaps(extent))
    {
        //std::cout << "no overlap" << std::endl;
        vol = 0.0;
        return 0.0;
    }


    vel_dir = vel_dir.Normalize();
    LSMVector3 vel_axis1 = vel_dir;
    LSMVector3 vel_axis2 = LSMVector3::CrossProduct(vel_axis1,LSMVector3(0.0,1.0,0.0));

    LSMStructTriangle triangle(p1,p2,p3);
    LSMVector3 normal = LSMVector3::CrossProduct(triangle.p2 - triangle.p1,triangle.p3 - triangle.p1).Normalize();
    if(normal.dot(LSMVector3(0.0,1.0,0.0)) >= 0.0)
    {
       LSMVector3 temp = p1;
       p1 = p2;
       p2 = temp;
       triangle = LSMStructTriangle(p1,p2,p3);
    }

    std::vector<LSMVector3> points;
    LSMVector3 triangle_lines_p1[3] = {p1,p2,p3};
    LSMVector3 triangle_lines_p2[3] = {p2,p3,p1};

    //LSMVector3 normal = LSMVector3::CrossProduct(p2-p1,p3-p1).Normalize();

    float xmin = extent.GetMinX();
    float xmax = extent.GetMaxX();
    float zmin = extent.GetMinY();
    float zmax = extent.GetMaxY();

    float y1b = z + dzdx *(xmin - extent.GetCenterX()) + dzdy *(zmin - extent.GetCenterY())-1000.0;
    float y2b = z + dzdx *(xmax - extent.GetCenterX()) + dzdy *(zmin - extent.GetCenterY())-1000.0;
    float y3b = z + dzdx *(xmin - extent.GetCenterX()) + dzdy *(zmax - extent.GetCenterY())-1000.0;
    float y4b = z + dzdx *(xmax - extent.GetCenterX()) + dzdy *(zmax - extent.GetCenterY())-1000.0;

    float y1t = h + z + (dhdx + dzdx) *(xmin - extent.GetCenterX()) + (dhdy + dzdy) *(zmin - extent.GetCenterY());
    float y2t = h + z + (dhdx + dzdx) *(xmax - extent.GetCenterX()) + (dhdy + dzdy) *(zmin - extent.GetCenterY());
    float y3t = h + z + (dhdx + dzdx) *(xmin - extent.GetCenterX()) + (dhdy + dzdy) *(zmax - extent.GetCenterY());
    float y4t = h + z + (dhdx + dzdx) *(xmax - extent.GetCenterX()) + (dhdy + dzdy) *(zmax - extent.GetCenterY());

    //check for all three points if they are inside

    float yp1 = h + z + (dhdx + dzdx) *(p1.x - extent.GetCenterX()) + (dhdy + dzdy) *(p1.z - extent.GetCenterY());
    float yp2 = h + z + (dhdx + dzdx) *(p2.x - extent.GetCenterX()) + (dhdy + dzdy) *(p2.z - extent.GetCenterY());
    float yp3 = h + z + (dhdx + dzdx) *(p3.x - extent.GetCenterX()) + (dhdy + dzdy) *(p3.z - extent.GetCenterY());

    if(p1.y <= yp1 && extent.Contains(p1.x,p1.z))
    {
        points.push_back(p1);
    }
    if(p2.y <= yp2 && extent.Contains(p2.x,p2.z))
    {
        points.push_back(p2);
    }
    if(p3.y <= yp3 && extent.Contains(p3.x,p3.z))
    {
        points.push_back(p3);
    }

    //if there are three points of the triangle within the flow region, no intersectinos can occur
    if(!(points.size() == 3))
    {

        //std::cout << "check flow surface edges " << std::endl;
        //5 rectangles, with 3 lines for intersection
        //std::cout << "top side " << std::endl;
        //top side
        TSquare sq(LSMVector3(xmin,y1t,zmin),LSMVector3(xmax,y2t,zmin),LSMVector3(xmax,y4t,zmax),LSMVector3(xmin,y3t,zmax));
        for(int i =0; i < 3; i++)
        {
            LSMVector3 intersect = SquareLineIntersect(triangle_lines_p1[i],triangle_lines_p2[i],sq);
            if(intersect.x != -INFINITYFLTLSM)
            {
                points.push_back(intersect);
            }
        }

        //std::cout << "xmin side " << std::endl;
        //xmin side
        sq = TSquare(LSMVector3(xmin,y1b,zmin),LSMVector3(xmin,y1t,zmin),LSMVector3(xmin,y3t,zmax),LSMVector3(xmin,y3b,zmax));
        for(int i =0; i < 3; i++)
        {
            LSMVector3 intersect = SquareLineIntersect(triangle_lines_p1[i],triangle_lines_p2[i],sq);
            if(intersect.x != -INFINITYFLTLSM)
            {
                points.push_back(intersect);
            }
        }

        //std::cout << "xmax side " << std::endl;
        //xmax side
        sq = TSquare(LSMVector3(xmax,y2b,zmin),LSMVector3(xmax,y2t,zmin),LSMVector3(xmax,y4t,zmax),LSMVector3(xmax,y4b,zmax));
        for(int i =0; i < 3; i++)
        {
            LSMVector3 intersect = SquareLineIntersect(triangle_lines_p1[i],triangle_lines_p2[i],sq);
            if(intersect.x != -INFINITYFLTLSM)
            {
                points.push_back(intersect);
            }
        }


        //std::cout << "zmin side " << std::endl;
        //zmin side
        sq = TSquare(LSMVector3(xmin,y1b,zmin),LSMVector3(xmin,y1t,zmin),LSMVector3(xmax,y2t,zmin),LSMVector3(xmax,y2b,zmin));
        for(int i =0; i < 3; i++)
        {
            LSMVector3 intersect = SquareLineIntersect(triangle_lines_p1[i],triangle_lines_p2[i],sq);
            if(intersect.x != -INFINITYFLTLSM)
            {
                points.push_back(intersect);
            }
        }
        //std::cout << "zmax side " << std::endl;
        //zmax side
        sq = TSquare(LSMVector3(xmin,y3b,zmax),LSMVector3(xmin,y3t,zmax),LSMVector3(xmax,y4t,zmax),LSMVector3(xmax,y4b,zmax));
        for(int i =0; i < 3; i++)
        {
            LSMVector3 intersect = SquareLineIntersect(triangle_lines_p1[i],triangle_lines_p2[i],sq);
            if(intersect.x != -INFINITYFLTLSM)
            {
                points.push_back(intersect);
            }
        }

        //std::cout << "check flow surface edges " << std::endl;
        //8 lines with one triangle for intersection
        //upward edges (4 pieces)
        LSMVector3 intersect = TriangleLineIntersect(LSMVector3(xmin,y1b,zmin),LSMVector3(xmin,y1t,zmin),triangle);
        if(intersect.x != -INFINITYFLTLSM)
        {
            points.push_back(intersect);
        }
        intersect = TriangleLineIntersect(LSMVector3(xmax,y2b,zmin),LSMVector3(xmax,y2t,zmin),triangle);
        if(intersect.x != -INFINITYFLTLSM)
        {
            points.push_back(intersect);
        }
        intersect = TriangleLineIntersect(LSMVector3(xmin,y4b,zmax),LSMVector3(xmin,y4t,zmax),triangle);
        if(intersect.x != -INFINITYFLTLSM)
        {
            points.push_back(intersect);
        }
        intersect = TriangleLineIntersect(LSMVector3(xmax,y4b,zmax),LSMVector3(xmax,y4t,zmax),triangle);
        if(intersect.x != -INFINITYFLTLSM)
        {
            points.push_back(intersect);
        }

        //edges of top face
        intersect = TriangleLineIntersect(LSMVector3(xmin,y1t,zmin),LSMVector3(xmax,y2t,zmin),triangle);
        if(intersect.x != -INFINITYFLTLSM)
        {
            points.push_back(intersect);
        }
        intersect = TriangleLineIntersect(LSMVector3(xmax,y2t,zmin),LSMVector3(xmax,y4t,zmax),triangle);
        if(intersect.x != -INFINITYFLTLSM)
        {
            points.push_back(intersect);
        }
        intersect = TriangleLineIntersect(LSMVector3(xmin,y1t,zmin),LSMVector3(xmin,y3t,zmax),triangle);
        if(intersect.x != -INFINITYFLTLSM)
        {
            points.push_back(intersect);
        }
        intersect = TriangleLineIntersect(LSMVector3(xmin,y3t,zmax),LSMVector3(xmax,y4t,zmax),triangle);
        if(intersect.x != -INFINITYFLTLSM)
        {
            points.push_back(intersect);
        }
    }



    if(points.size() == 0)
    {
        //std::cout << "did not find points" << std::endl;
        vol = 0.0;
        return 0.0;
    }

    if(points.size() < 3)
    {

        vol = 0.0;
        return 0.0;

    }

    std::vector<LSMVector2> points_proj;
    std::vector<LSMVector4> angle_proj;

    //project to barycentric coordinates
    LSMVector3 v0 = p2 - p1, v1 = LSMVector3::CrossProduct(p2-p1,normal);
    for(int i = 0; i < points.size(); i++)
    {
        LSMVector3  v2 = points.at(i) - p1;
        float u = v2.dot(v0);
        float v = v2.dot(v1);
        points_proj.push_back(LSMVector2(u,v));
    }



    //get reference point (lowest point)

    int index = 0;
    int vmax = points_proj.at(0).y;
    for(int i = 1; i < points_proj.size(); i++)
    {
        if(points_proj.at(i).y < vmax)
        {
            vmax = points_proj.at(i).y;
            index = i;
        }
    }
    LSMVector3 pref = points.at(index);
    LSMVector2 pref_proj = points_proj.at(index);

    //calculate angle to all others
    for(int i = 0; i < points_proj.size(); i++)
    {
        if(i != index)
        {
            LSMVector2 pproj =points_proj.at(i);
            float angle = atan2(pproj.x - pref_proj.x,pproj.y - pref_proj.y);
            angle_proj.push_back(LSMVector4(points.at(i).x,points.at(i).y,points.at(i).z,angle));
        }else
        {
            angle_proj.push_back(LSMVector4(points.at(i).x,points.at(i).y,points.at(i).z,-999999.9));
        }
    }

    //sort by angle lowest to highest

    std::sort(angle_proj.begin(),angle_proj.end(),[](LSMVector4 a, LSMVector4 b){return a.w < b.w;});

    //now get the surface total

    double area = 0.0f;
    double vol_total = 0.0;

    hydro_cof = LSMVector3(0.0,0.0,0.0);
    hydro_total = 0.0;

    com = LSMVector3(0.0,0.0,0.0);
    area_proj = 0.0;
    //std::cout << "found " << points.size() << " points " << std::endl;
    //std::cout << "point list " << std::endl;
    for (int i = 0; i < angle_proj.size(); i++) {
        //std::cout << angle_proj.at(i).xyz().x << " ," << angle_proj.at(i).xyz().y << " ," << angle_proj.at(i).xyz().z << std::endl;
    }

    corners.push_back(pref);

    for (int i = 1; i < angle_proj.size()-1; i++) {
        // Get vertices of triangle i.
        LSMVector3 v0 = pref;
        LSMVector3 v1 = angle_proj.at(i).xyz();
        LSMVector3 v2 = angle_proj.at(i+1).xyz();

        corners.push_back(angle_proj.at(i).xyz());
        if(i == angle_proj.size()-1)
        {
             corners.push_back(angle_proj.at(i+1).xyz());
        }
        LSMVector3 normali = LSMVector3::CrossProduct(v1 - v0,v2-v0).Normalize();
        if((normali.dot(LSMVector3(0.0,1.0,0.0)) <= 0.0))
        {
                LSMVector3 temp = v1;
                v1 = v2;
                v2 = temp;
        }

        double area_thissubtriangle = 0.0;
        {
            double a = (v0-v1).length();
            double b = (v2-v1).length();
            double c = (v2-v0).length();

            double s = 0.5 * (a+b+c);

            double areasq = s*(s-a)*(s-b)*(s-c);
            area += areasq > 0.0?std::sqrt(areasq):0.0;
            area_thissubtriangle = areasq > 0.0?std::sqrt(areasq):0.0;
        }

        //signed submerged volume:

        float vyp1 = h + z + (dhdx + dzdx) *(v0.x - extent.GetCenterX()) + (dhdy + dzdy) *(v0.z - extent.GetCenterY());
        float vyp2 = h + z + (dhdx + dzdx) *(v1.x - extent.GetCenterX()) + (dhdy + dzdy) *(v1.z - extent.GetCenterY());
        float vyp3 = h + z + (dhdx + dzdx) *(v2.x - extent.GetCenterX()) + (dhdy + dzdy) *(v2.z - extent.GetCenterY());

        //submerged volume consists of three tetrahedra

        LSMVector3 v4 = LSMVector3(v0.x,vyp1,v0.z);
        LSMVector3 v5 = LSMVector3(v1.x,vyp2,v1.z);
        LSMVector3 v6 = LSMVector3(v2.x,vyp3,v2.z);

        float thvol1 = TetrahedronVolume(v0,v6,v5,v4);
        float thvol2 = TetrahedronVolume(v6,v0,v1,v2);
        float thvol3 = TetrahedronVolume(v0,v5,v6,v1);
        float volthis =thvol1 +thvol2 + thvol3;

        vol_total += volthis;

        com = com + ((v0+v6+v5+v4)*(1.0/4.0)*thvol1 + (v6+v0+v1+v2)*(1.0/4.0)*thvol2 + (v0+v5+v6+v1)*(1.0/4.0)*thvol3);
        //projected surface

        /*LSMVector3 vpr0 = LSMVector3(v0.x,vyp1,v0.z);
        LSMVector3 vpr1 = LSMVector3(v1.x,vyp2,v1.z);
        LSMVector3 vpr2 = LSMVector3(v2.x,vyp3,v2.z);

        {
            double a = (vpr0-vpr1).length();
            double b = (vpr2-vpr1).length();
            double c = (vpr2-vpr0).length();

            double s = 0.5 * (a+b+c);

            double areasq = s*(s-a)*(s-b)*(s-c);

            area_proj +=  (normal.dot(LSMVector3(0.0,1.0,0.0)) <= 0.0? -1.0:1.0)*(areasq > 0.0?std::sqrt(areasq):0.0);
        }*/

        //velocity projected area

        /*LSMVector3 vprv0 = vel_axis1 * v0.dot(vel_axis1)+LSMVector3(0.0,v0.y,0.0)+vel_axis2 * v0.dot(vel_axis2);
        LSMVector3 vprv1 = vel_axis1 * v1.dot(vel_axis1)+LSMVector3(0.0,v1.y,0.0)+vel_axis2 * v1.dot(vel_axis2);
        LSMVector3 vprv2 = vel_axis1 * v2.dot(vel_axis1)+LSMVector3(0.0,v2.y,0.0)+vel_axis2 * v2.dot(vel_axis2);

        {
            double a = (vpr0-vpr1).length();
            double b = (vpr2-vpr1).length();
            double c = (vpr2-vpr0).length();

            double s = 0.5 * (a+b+c);

            double areasq = s*(s-a)*(s-b)*(s-c);

            vel_proj_area +=  (normal.dot(vel_dir) <= 0.0? -1.0:1.0)*(areasq > 0.0?std::sqrt(areasq):0.0);
        }*/

        //hydrostatic force center of application
        //split into two triangles with horizontal base.

        LSMVector3 hy_A = v0.y > v1.y ? (v0.y > v2.y ? v0: v2):(v1.y > v2.y ? v1: v2);
        LSMVector3 hy_C = v0.y < v1.y ? (v0.y < v2.y ? v0: v2):(v1.y < v2.y ? v1: v2);
        LSMVector3 hy_B = (v0.y < hy_A.y  && v0.y > hy_C.y )? v0 : ((v1.y < hy_A.y  && v1.y > hy_C.y )? v1 : v2);
        LSMVector3 hy_AC = (hy_C- hy_A);
        LSMVector3 hy_D = hy_A - hy_AC * (hy_A.y - hy_B.y);

        //two triangles with horizontal base -> ADB and BDC
        float baselength = (hy_D- hy_B).length();
        float height_top = (hy_A.y - hy_B.y);
        float height_bottom = (hy_B.y - hy_C.y);

        float h_water_av_top = (1.0/3.0) * (vyp1 + vyp2 + vyp3) - (1.0/3.0) * (hy_A.y+ hy_B.y + hy_C.y);
        float h_water_av_bottom = (1.0/3.0) * (vyp1 + vyp2 + vyp3) - (1.0/3.0) * (hy_B.y+ hy_D.y + hy_C.y);
        float tc_top = (4.0 * h_water_av_top  + 3.0 * height_top)/(6.0 * h_water_av_top  + 4.0 * height_top);
        float tc_bottom = (2.0 * h_water_av_bottom  + 1.0 * height_bottom)/(6.0 * h_water_av_bottom  + 2.0 * height_bottom);

        hydro_cof = hydro_cof + (hy_A + ((hy_D+hy_B)*0.5 - hy_A)* tc_top)*h_water_av_top *(0.5 * height_top * baselength)  + (hy_C + ((hy_D+hy_B)*0.5-hy_C) *tc_bottom)*h_water_av_bottom*(0.5 * height_bottom * baselength);
        hydro_total += h_water_av_top*(0.5 * height_top * baselength) + h_water_av_bottom* area_thissubtriangle*(0.5 * height_bottom * baselength);
    }
    if(hydro_total > 1e-10)
    {
        hydro_cof = hydro_cof/hydro_total;


    }
    vel_proj_area = normal.dot(vel_dir)* area;
    area_proj = normal.dot(LSMVector3(0.0,1.0,0.0)) * area;
    if(std::fabs(vol_total) > 1e-12)
    {
        com = com/vol_total;
    }
    vol = -(normal.dot(LSMVector3(0.0,1.0,0.0)) <= 0.0? 1.0:-1.0)*vol_total;




    //std::cout << " area " << area << " vol " << vol << std::endl;
    return area;

}

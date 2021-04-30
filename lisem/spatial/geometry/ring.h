#pragma once

#include "defines.h"
#include <vector>
#include "linear/lsm_vector2.h"
#include "geometry/shapealgebra.h"
#include "iostream"
#include <algorithm>

// LINE/LINE
inline static bool PolyLine_lineLine(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, LSMVector2 &p) {

    if(((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1)) == 0 || ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1)) == 0)
    {
        return false;
    }

  // calculate the direction of the lines
  float uA = ((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
  float uB = ((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));


  // if uA and uB are between 0-1, lines are colliding
  if (uA > 0.0 && uA < 1.0 && uB > 0.0 && uB < 1.0) {
  //if (uA >= 0 && uB <= 1) {

    // optionally, draw a circle where the lines meet
    float intersectionX = x1 + (uA * (x2-x1));
    float intersectionY = y1 + (uA * (y2-y1));

    p = LSMVector2(intersectionX,intersectionY);

    return true;
  }
  return false;
}

class LISEM_API Ring
{

    std::vector<double> vertices;

   public:
    inline void AddVertex(double x, double y)
    {
        vertices.push_back(x);
        vertices.push_back(y);
    }

    inline double * GetVertexData()
    {
        return vertices.data();
    }

    inline int GetVertexCount()
    {
        return vertices.size()/2;
    }

    inline void RemoveVertex(int i)
    {
        vertices.erase(vertices.begin() + i);
        vertices.erase(vertices.begin() + i);
    }
    inline void SetAsCircle(LSMVector2 centre, float radius, int divide)
    {

        for(int i = 0; i < divide; i++)
        {
            float x = centre.x + cos(2.0 * M_PI * (float(i)))/(float(divide));
            float y = centre.y + sin(2.0 * M_PI * (float(i)))/(float(divide));
            AddVertex(x,y);
        }

    }
    inline int size()
    {
        return GetVertexCount();
    }

    inline void CloseIfNot()
    {
        double* pts = GetVertexData();
        size_t nPts = GetVertexCount()-1;

        if(nPts > 2)
        {
            if((pts[nPts*2] !=  pts[0]) || (pts[nPts*2+1] != pts[1]))
            {
                vertices.push_back(pts[0]);
                vertices.push_back(pts[1]);
            }
        }
    }

    inline bool approx_equal(double x, double y)
    {
       const double EPSILON = 1E-14;
       if (x == 0) return fabs(y) <= EPSILON;
       if (y == 0) return fabs(x) <= EPSILON;
       return fabs(x - y) / std::max(fabs(x), fabs(y)) <= EPSILON;
    }

    inline bool RemoveSelfIntersection()
    {

        bool found = true;
        bool found_any = false;
        while(found)
        {
            found = false;


            for(int i = 0; i < vertices.size(); i += 2)
            {
                if(vertices.size() < 7)
                {
                    break;
                }

                bool found_inner = false;
                double lx1 = vertices[i +0];
                double ly1 = vertices[i +1];


                for(int j = 0; j < vertices.size() ; j += 2)
                {

                    if(vertices.size() < 7)
                    {
                        break;
                    }

                    if(j == i)
                    {
                        continue;
                    }


                    double l2x1 = vertices[j +0];
                    double l2y1 = vertices[j +1];



                    if(approx_equal(l2x1,lx1) && approx_equal(l2y1,ly1))
                    {
                        vertices.erase(vertices.begin() + (j + 0));
                        vertices.erase(vertices.begin() + (j + 0));

                        found = true;
                        found_inner = true;
                        found_any = true;
                        break;
                    }


                }

                if(found_inner)
                {
                    break;
                }
            }

            if(found)
            {
               break;
            }

            for(int i = 0; i < vertices.size() - 2; i += 2)
            {
                if(vertices.size() < 7)
                {
                    break;
                }

                bool found_inner = false;
                double lx1 = vertices[i +0];
                double lx2 = vertices[i +2];
                double ly1 = vertices[i +1];
                double ly2 = vertices[i +3];


                for(int j = 0; j < vertices.size() - 2; j += 2)
                {
                    if(vertices.size() < 7)
                    {
                        break;
                    }
                    double l2x1 = vertices[j +0];
                    double l2x2 = vertices[j +2];
                    double l2y1 = vertices[j +1];
                    double l2y2 = vertices[j +3];

                    LSMVector2 p;

                    //check line-line intersection
                    if( PolyLine_lineLine(lx1,ly1,lx2,ly2,l2x1,l2y1,l2x2,l2y2,p))
                    {
                        //remove a point from second line, that has its vertex closest to the edge points of the other line.
                        //this prioritizes the removal of small intersection loops, and leaves the larger structure intact
                        //keep doing this untill there is no intersection anymore
                        double dist_11 = (l2x1 - lx1)*(l2x1 - lx1) + (l2y1 - ly1)*(l2y1 - ly1);
                        double dist_22 = (l2x2 - lx2)*(l2x2 - lx2) + (l2y2 - ly2)*(l2y2 - ly2);
                        double dist_12 = (l2x1 - lx2)*(l2x1 - lx2) + (l2y1 - ly2)*(l2y1 - ly2);
                        double dist_21 = (l2x2 - lx1)*(l2x2 - lx1) + (l2y2 - ly1)*(l2y2 - ly1);
                        if(std::min(dist_11,dist_12) < std::min(dist_22,dist_21))
                        {
                            vertices.erase(vertices.begin() + (j + 0));
                            vertices.erase(vertices.begin() + (j + 0));
                        }else {
                            vertices.erase(vertices.begin() + (j + 2));
                            vertices.erase(vertices.begin() + (j + 2));
                        }
                        found = true;
                        found_inner = true;
                        found_any = true;
                        break;
                    }
                }
                if(found_inner)
                {
                    break;
                }
            }
        }

        return found_any;

    }

    inline LSMVector2 GetVertex(int i)
    {
        return LSMVector2(vertices.at(i* 2),vertices.at(i* 2 + 1));

    }

    inline void GetVertexP(int i, double ** x , double **y)
    {
        if( i < GetVertexCount())
        {

            *x = &vertices.at(i* 2);
            *y = &vertices.at(i* 2 + 1);
            return;
        }
        *x = nullptr;
        *y = nullptr;
    }

    inline float SignedArea()
    {

        double* pts = GetVertexData();
        size_t nPts = GetVertexCount();
        LSMVector2 off = {(float)pts[0],(float)pts[1]};
        float twicearea = 0;
        LSMVector2 p1, p2;
        float f;
        for (int i = 0, j = nPts - 1; i < nPts; j = i++) {
            p1 = LSMVector2((float)pts[2*i],(float)pts[2*i+1]);
            p2 = LSMVector2((float)pts[2*j],(float)pts[2*j+1]);
            f = (p1.x - off.x) * (p2.y - off.y) - (p2.x - off.x) * (p1.y - off.y);
            twicearea += f;
        }

        return twicearea /2.0;
    }


    inline bool Is_ClockWise()
    {
        double signed_area = SignedArea();

        return signed_area > 0;
    }

    inline void Reverse()
    {
        std::reverse(vertices.begin(),vertices.end());
    }

    LSMVector2 GetClosestEdgeLocationP(LSMVector2 pos, double **x1 = nullptr, double **y1 = nullptr, double**x2 = nullptr, double **y2 = nullptr)
    {
        if(x1 != nullptr)
        {
            *x1 = nullptr;
        }
        if(x2 != nullptr)
        {
            *x2 = nullptr;
        }
        if(y1 != nullptr)
        {
            *y1 = nullptr;
        }
        if(y2 != nullptr)
        {
            *y2 = nullptr;
        }


        LSMVector2 closest = LSMVector2(1e31,1e31);
        double mindist = 1e31;
        for(int i = 0; i < vertices.size() - 2; i += 2)
        {
            double lx1 = vertices[i +0];
            double lx2 = vertices[i +2];
            double ly1 = vertices[i +1];
            double ly2 = vertices[i +3];

            double retx =1e31;
            double rety =1e31;

            GetClosestPointOnLine(lx1,ly1,lx2,ly2,pos.x,pos.y, &retx, &rety);

            float distance = LSMVector2(pos - LSMVector2(retx,rety)).length();

            if(i == 0 || distance < mindist)
            {
                closest =LSMVector2(retx,rety);
                mindist = distance;
                if(x1 != nullptr)
                {
                    *x1 = &vertices[i +0];
                }
                if(x2 != nullptr)
                {
                    *x2 = &vertices[i +2];
                }
                if(y1 != nullptr)
                {
                    *y1 = &vertices[i +1];
                }
                if(y2 != nullptr)
                {
                    *y2 = &vertices[i +3];
                }

            }
        }

        return closest;
    }

    LSMVector2 GetClosestVertexP(LSMVector2 pos, double **x1 = nullptr, double**y1 = nullptr)
    {
        LSMVector2 closest = LSMVector2(1e31,1e31);
        float mindist = 1e31;

        if(x1 != nullptr)
        {
            *x1 = nullptr;
        }
        if(y1 != nullptr)
        {
            *y1 = nullptr;
        }

        for(int i = 0; i < vertices.size()-1; i += 2)
        {
            double lx1 = vertices[i +0];
            double ly1 = vertices[i +1];
            float distance = LSMVector2(pos - LSMVector2(lx1,ly1)).length();
            if(i == 0 || distance < mindist)
            {
                mindist = distance;
                closest = LSMVector2(lx1,ly1);
                if(x1 != nullptr)
                {
                    *x1 = &vertices[i +0];
                }
                if(y1 != nullptr)
                {
                    *y1 = &vertices[i +1];
                }

            }
        }

        return closest;
    }

    double Length()
    {
        double length = 0.0;

        for(int i = 0; i < vertices.size() - 2; i += 2)
        {
            double x1 = vertices[i +0];
            double x2 = vertices[i +2];
            double y1 = vertices[i +1];
            double y2 = vertices[i +3];

            length += std::sqrt((x2-x1) *(x2-x1) + (y2-y1) *(y2-y1));

        }
        return length;
    }
    virtual void MoveVertex(double *x, double *y, LSMVector2 d)
    {

        for(int i = 0; i < ((int)vertices.size())-1; i += 2)
        {
            if(&vertices[i +0] == x)
            {
                vertices[i +0] += d.x;
            }
            if(&vertices[i +1] == y)
            {
                vertices[i +1] += d.y;
            }
        }
    }

    inline void Move(LSMVector2 d)
    {
        for(int i = 0; i < ((int)vertices.size())-1; i += 2)
        {
            vertices[i +0] += d.x;
            vertices[i +1] += d.y;
        }
    }

    LSMVector2 GetClosestEdgeLocation(LSMVector2 pos)
    {
        LSMVector2 closest = LSMVector2(1e31,1e31);
        double mindist = 1e31;
        for(int i = 0; i < vertices.size() - 2; i += 2)
        {
            double x1 = vertices[i +0];
            double x2 = vertices[i +2];
            double y1 = vertices[i +1];
            double y2 = vertices[i +3];

            double retx =1e31;
            double rety =1e31;

            GetClosestPointOnLine(x1,y1,x2,y2,pos.x,pos.y, &retx, &rety);

            float distance = LSMVector2(pos - LSMVector2(retx,rety)).length();

            if(i== 0||distance < mindist)
            {
                closest =LSMVector2(retx,rety);
                mindist = distance;
            }
        }

        return closest;
    }

    LSMVector2 GetClosestVertex(LSMVector2 pos)
    {
        LSMVector2 closest = LSMVector2(1e31,1e31);
        float mindist = 1e31;

        for(int i = 0; i < vertices.size()-1; i += 2)
        {
            double x1 = vertices[i +0];
            double y1 = vertices[i +1];
            float distance = LSMVector2(pos - LSMVector2(x1,y1)).length();
            if(i == 0)
            {
                mindist = distance;
                closest = LSMVector2(x1,y1);
            }else if(distance < mindist)
            {
                mindist = distance;
                closest = LSMVector2(x1,y1);
            }
        }

        return closest;
    }

    inline Ring * Copy()
    {
        Ring * r = new Ring();

        r->vertices = this->vertices;
        return r;
    }
};


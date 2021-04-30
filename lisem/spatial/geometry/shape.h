#pragma once
#include "defines.h"
#include "ogrsf_frmts.h"
#include "gdal.h"
#include "ogr_api.h"

#include "boundingbox.h"

#include "linear/lsm_vector2.h"
#include "linear/lsm_vector3.h"
#include "linear/lsm_vector4.h"

#include "functional"

#define LISEM_SHAPE_UNKNOWN 0
#define LISEM_SHAPE_POINT 1
#define LISEM_SHAPE_LINE 2
#define LISEM_SHAPE_TRIANGLE 3
#define LISEM_SHAPE_POLYGON 4
#define LISEM_SHAPE_MULTIPOINT 5
#define LISEM_SHAPE_MULTILINE 6
#define LISEM_SHAPE_MULTIPOLYGON 7
#define LISEM_SHAPE_MULTITRIANGLE 8

class LISEM_API LSMShape
{

private:

    int m_Type;

    //virtual double GetArea();
    //virtual double GetCentroid();
    //virtual int GetType();

public:

    inline LSMShape()
    {
        m_Type = LISEM_SHAPE_UNKNOWN;
    }

    inline LSMShape(int type)
    {
        m_Type = type;
    }

    inline int GetType()
    {
        return m_Type;
    }

    virtual LSMShape * Copy()
    {
        return new LSMShape(LISEM_SHAPE_UNKNOWN);
    }


    virtual BoundingBox GetBoundingBox()
    {

        return BoundingBox(0,0,0,0);
    }

    virtual bool Contains(LSMVector2 pos, double tolerence)
    {
        return false;
    }

    virtual LSMVector2 GetClosestEdgeLocationP(LSMVector2 pos, double **x1 = nullptr, double **y1 = nullptr, double **x2 = nullptr, double **y2 = nullptr)
    {



        return LSMVector2();
    }

    virtual LSMVector2 GetClosestVertexP(LSMVector2 pos, double **x = nullptr, double **y = nullptr)
    {
        return LSMVector2();
    }

    virtual LSMVector2 GetClosestEdgeLocation(LSMVector2 pos)
    {



        return LSMVector2();
    }

    virtual LSMVector2 GetClosestVertex(LSMVector2 pos)
    {
        return LSMVector2();
    }

    virtual int GetVertexCount()
    {
        return 0;
    }
    virtual LSMVector2 GetVertex(int i)
    {
        return LSMVector2(0.0,0.0);
    }
    virtual void GetVertexP(int i, double ** x , double **y)
    {

    }

    virtual void RemoveVertex(int i)
    {

    }

    //input is polygon (provided in non-closed form)
    virtual bool Overlaps(std::vector<double> & x,std::vector<double> & y)
    {
        return false;
    }
    virtual QList<LSMShape *> Split(std::vector<double> & x,std::vector<double> & y)
    {
        QList<LSMShape *> l;

        return l;
    }



    virtual void MoveVertex(double *x, double *y, LSMVector2 d)
    {

    }

    virtual void MoveVertex(LSMVector2 d)
    {

    }

    virtual void Move(LSMVector2 pos)
    {

    }

    virtual double Area()
    {
        return 0.0;
    }

    virtual double Length()
    {
        return 0.0;
    }


    virtual LSMShape *Buffer(double buf_dist)
    {

        return nullptr;
    }

    virtual LSMVector2 GetCentroid()
    {
        return LSMVector2(0.0,0.0);
    }


    virtual void Destroy()
    {


    }




};



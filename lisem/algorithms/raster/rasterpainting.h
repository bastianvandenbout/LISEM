#ifndef RASTERPAINTING_H
#define RASTERPAINTING_H

#include "vector"
#include "geo/raster/map.h"
#include "geometry/shape.h"
#include "boundingbox.h"
#include "geometry/line.h"
#include "geometry/sphtriangle.h"
#include "raster/rastercommon.h"

typedef struct RasterCoordinateList
{
    std::vector<int> m_R;
    std::vector<int> m_C;
    std::vector<float> m_A;

    inline BoundingBox GetBoundingBox() const
    {
        BoundingBox b;
        if(m_R.size() > 0)
        {
            b.SetAs(LSMVector2(m_R.at(0),m_C.at(0)));
        }
        for(int i = 1; i < m_R.size(); i++)
        {
            b.MergeWith(LSMVector2(m_R.at(i),m_C.at(i)));
        }

        return b;
    }

} RasterCoordinateList;

typedef struct RasterChangeList
{
    cTMap * target;
    std::vector<int> m_R;
    std::vector<int> m_C;
    std::vector<float> m_Pre;
    std::vector<float> m_Post;

    inline BoundingBox GetBoundingBox() const
    {
        BoundingBox b;
        if(m_R.size() > 0)
        {
            b.SetAs(LSMVector2(m_R.at(0),m_C.at(0)));
        }
        for(int i = 1; i < m_R.size(); i++)
        {
            b.MergeWith(LSMVector2(m_R.at(i),m_C.at(i)));
        }

        return b;
    }

} RasterChangeList;

typedef double (*RasterAlterFunc)(cTMap *,int,int,float,float);

static inline double AddToRasterCell(cTMap * m,int r, int c, float val, float val2)
{
    return m->data[r][c] + val;
}

static inline double PaintToRasterCell(cTMap * m,int r, int c, float val, float val2)
{
    return val;
}

static inline double ConvergeRasterCell(cTMap * m,int r, int c, float val, float val2)
{
    return m->data[r][c] + val2*(val -m->data[r][c]);
}

static inline double MutiplyToRasterCell(cTMap * m,int r, int c, float val, float val2)
{
    return m->data[r][c] *val;
}

static inline double BlurRasterCell(cTMap * m,int r, int c, float val, float val2)
{
    return GetMapValue_3x3Av(m,r,c);
}


static inline double LiquifyRasterCell(cTMap * m,int r, int c, float val, float val2)
{
    //val is dr
    //val2 = dc


    //for each pixel within the dragged domain, we sample linearly from originating location
    if((val == 0.0) && (val2 == 0.0))
    {
        return m->data[r][c];
    }else {

        double x = m->west() + (((float)(c)) - val2) * m->cellSizeX();
        double y = m->north() + (((float)(r)) - val) * m->cellSizeY();

        if((x > m->west()) && (x < m->west() + ((float)(m->nrCols()))*  m->cellSizeX()))
        {
            if(m->cellSizeX() > 0)
            {
                x= std::max(m->west(),std::min(m->west() + ((double(m->nrCols()))*  m->cellSizeX()),x));
            }else {
                x= std::min(m->west(),std::max(m->west() + ((double(m->nrCols()))*  m->cellSizeX()),x));
            }
        }
        if((y > m->north()) && (y < m->north() + ((float)(m->nrRows()))*  m->cellSizeY()))
        {
            if(m->cellSizeX() > 0)
            {
                y= std::max(m->north(),std::min(m->north() + ((double(m->nrRows()))*  m->cellSizeY()),y));
            }else {
                y= std::min(m->north(),std::max(m->north() + ((double(m->nrRows()))*  m->cellSizeY()),y));
            }
        }
        return m->SampleLinear(x,y);
    }
}


static inline RasterCoordinateList RasterizeRectangle(cTMap * m, double cx, double cy, double sx, double sy, double featherfac)
{

    RasterCoordinateList rl;


    float xmin = cx - 0.5 * std::max(sy,sx);
    float xmax = cx + 0.5 * std::max(sy,sx);
    float ymin = cy - 0.5 * std::max(sy,sx);
    float ymax = cy + 0.5 * std::max(sy,sx);

    int cmin = static_cast<int>((xmin - m->west())/m->cellSizeX());
    int cmax = static_cast<int>((xmax - m->west())/m->cellSizeX());
    int rmin = static_cast<int>((ymin - m->north())/m->cellSizeY());
    int rmax = static_cast<int>((ymax - m->north())/m->cellSizeY());

    if(cmin > cmax)
    {
        int temp = cmax;
        cmax = cmin;
        cmin =  temp;
    }
    if(rmin > rmax)
    {
        int temp = rmax;
        rmax = rmin;
        rmin =  temp;
    }

    rmin = std::max(0,std::min(m->nrRows()-1,rmin));
    rmax = std::max(0,std::min(m->nrRows()-1,rmax));
    cmin = std::max(0,std::min(m->nrCols()-1,cmin));
    cmax = std::max(0,std::min(m->nrCols()-1,cmax));

    for(int r = rmin; r < rmax + 1; r++)
    {
        for(int c = cmin; c < cmax + 1; c++)
        {
            float celly = ((m->north() + r * m->cellSizeY()));
            float cellx = ((m->west() + c * m->cellSizeX()));
            if(abs(cellx - cx + 0.5 * m->cellSizeX()) < sx * 0.5 && abs(celly - cy + 0.5 * m->cellSizeY()) < sy * 0.5)
            {
                float alphaf = 2.0* std::max(abs(cellx - cx)/ sx,abs(celly - cy)/sy);

                rl.m_C.push_back(c);
                rl.m_R.push_back(r);

                if(featherfac > 0.0)
                {
                    alphaf = std::min(1.0,std::max(0.0,pow(1.0-alphaf,featherfac)));
                }else
                {
                    alphaf = 1.0;
                }
                rl.m_A.push_back(alphaf);
            }
        }
    }

    return rl;

}

static inline RasterCoordinateList RasterizeLines(cTMap * m, float * x, float * y, int length, double size, double featherfac)
{

    RasterCoordinateList rl;


    BoundingBox b;


    LSMLine * l = new LSMLine();
    for(int i = 0; i < length; i++)
    {
        l->AddVertex(x[i],y[i]);
    }

    b = l->GetBoundingBox();


    float xmin = b.GetMinX()- size;
    float xmax = b.GetMaxX()+ size;
    float ymin = b.GetMinY()- size;
    float ymax = b.GetMaxY()+ size;

    int cmin = static_cast<int>((xmin - m->west())/m->cellSizeX());
    int cmax = static_cast<int>((xmax - m->west())/m->cellSizeX());
    int rmin = static_cast<int>((ymin - m->north())/m->cellSizeY());
    int rmax = static_cast<int>((ymax - m->north())/m->cellSizeY());


    if(cmin > cmax)
    {
        int temp = cmax;
        cmax = cmin;
        cmin =  temp;
    }
    if(rmin > rmax)
    {
        int temp = rmax;
        rmax = rmin;
        rmin =  temp;
    }

    rmin = std::max(0,std::min(m->nrRows()-1,rmin));
    rmax = std::max(0,std::min(m->nrRows()-1,rmax));
    cmin = std::max(0,std::min(m->nrCols()-1,cmin));
    cmax = std::max(0,std::min(m->nrCols()-1,cmax));
    for(int r = rmin; r < rmax + 1; r++)
    {
        for(int c = cmin; c < cmax + 1; c++)
        {
            float celly = ((m->north() + r * m->cellSizeY()));
            float cellx = ((m->west() + c * m->cellSizeX()));

            float dis = l->GetDistanceToPoint(LSMVector2(cellx,celly));

            if(dis < size)
            {
                float alphaf =dis/size;

                rl.m_C.push_back(c);
                rl.m_R.push_back(r);

                if(featherfac > 0.0)
                {
                    alphaf = std::min(1.0,std::max(0.0,pow(1.0-alphaf,featherfac)));
                }else
                {
                    alphaf = 1.0;
                }
                rl.m_A.push_back(alphaf);
            }
        }
    }

    l->Destroy();
    delete l;

    return rl;

}

static inline RasterCoordinateList RasterizeEllipsoid(cTMap * m, double cx, double cy, double sx, double sy, double featherfac)
{

    RasterCoordinateList rl;


    float xmin = cx - 0.5 * std::max(sy,sx);
    float xmax = cx + 0.5 * std::max(sy,sx);
    float ymin = cy - 0.5 * std::max(sy,sx);
    float ymax = cy + 0.5 * std::max(sy,sx);

    int cmin = static_cast<int>((xmin - m->west())/m->cellSizeX());
    int cmax = static_cast<int>((xmax - m->west())/m->cellSizeX());
    int rmin = static_cast<int>((ymin - m->north())/m->cellSizeY());
    int rmax = static_cast<int>((ymax - m->north())/m->cellSizeY());

    if(cmin > cmax)
    {
        int temp = cmax;
        cmax = cmin;
        cmin =  temp;
    }
    if(rmin > rmax)
    {
        int temp = rmax;
        rmax = rmin;
        rmin =  temp;
    }

    rmin = std::max(0,std::min(m->nrRows()-1,rmin));
    rmax = std::max(0,std::min(m->nrRows()-1,rmax));
    cmin = std::max(0,std::min(m->nrCols()-1,cmin));
    cmax = std::max(0,std::min(m->nrCols()-1,cmax));

    for(int r = rmin; r < rmax + 1; r++)
    {
        for(int c = cmin; c < cmax + 1; c++)
        {
            float celly = ((m->north() + r * m->cellSizeY()));
            float cellx = ((m->west() + c * m->cellSizeX()));
            if(abs(cellx - cx + 0.5 * m->cellSizeX()) < sx * 0.5 && abs(celly - cy + 0.5 * m->cellSizeY()) < sy * 0.5)
            {
                float distx = 2.0*abs(cellx - cx + 0.5 * m->cellSizeX())/sx;
                float disty = 2.0*abs(celly - cy + 0.5 * m->cellSizeY())/sy;

                float distr = std::sqrt(distx * distx + disty*disty);

                float alphaf = distr;
                if(alphaf > 0.0)
                {
                    rl.m_C.push_back(c);
                    rl.m_R.push_back(r);

                    if(featherfac > 0.0)
                    {
                        alphaf = std::min(1.0,std::max(0.0,pow(1.0-alphaf,featherfac)));
                    }else
                    {
                        alphaf = 1.0;
                    }
                    rl.m_A.push_back(alphaf);
                }

            }
        }
    }

    return rl;


}



static inline RasterCoordinateList RasterizeTriangles(cTMap * m, float * x, float * y, int length)
{

    RasterCoordinateList rl;

    length = 3*(length / 3);
    if(!(length > 3))
    {
        return rl;
    }

    for(int i = 0; i < length; i = i+3)
    {
        SPHTriangle *t = new SPHTriangle(x[i],y[i],x[i+1],y[i+1],x[i+2],y[i+2]);

        BoundingBox b = t->GetBoundingBox();

        float xmin = b.GetMinX();
        float xmax = b.GetMaxX();
        float ymin = b.GetMinY();
        float ymax = b.GetMaxY();

        int cmin = static_cast<int>((xmin - m->west())/m->cellSizeX());
        int cmax = static_cast<int>((xmax - m->west())/m->cellSizeX());
        int rmin = static_cast<int>((ymin - m->north())/m->cellSizeY());
        int rmax = static_cast<int>((ymax - m->north())/m->cellSizeY());

        rmin = std::max(0,std::min(m->nrRows()-1,rmin));
        rmax = std::max(0,std::min(m->nrRows()-1,rmax));
        cmin = std::max(0,std::min(m->nrCols()-1,cmin));
        cmax = std::max(0,std::min(m->nrCols()-1,cmax));

        if(cmin > cmax)
        {
            int temp = cmax;
            cmax = cmin;
            cmin =  temp;
        }
        if(rmin > rmax)
        {
            int temp = rmax;
            rmax = rmin;
            rmin =  temp;
        }


        for(int r = rmin; r < rmax + 1; r++)
        {
            for(int c = cmin; c < cmax + 1; c++)
            {
                float celly = ((m->north() + r * m->cellSizeY()));
                float cellx = ((m->west() + c * m->cellSizeX()));

                if(t->Contains(LSMVector2(cellx,celly),m->cellSize()))
                {
                    rl.m_R.push_back(r);
                    rl.m_C.push_back(c);
                    rl.m_A.push_back(1.0f);
                }
            }
        }

        t->Destroy();
        delete t;
    }


    return rl;

}

static inline RasterCoordinateList RasterizeShape(cTMap * m, LSMShape * s)
{

    RasterCoordinateList rl;
    BoundingBox b = s->GetBoundingBox();

    float xmin = b.GetMinX();
    float xmax = b.GetMaxX();
    float ymin = b.GetMinY();
    float ymax = b.GetMaxY();

    int cmin = static_cast<int>((xmin - m->west())/m->cellSizeX());
    int cmax = static_cast<int>((xmax - m->west())/m->cellSizeX());
    int rmin = static_cast<int>((ymin - m->north())/m->cellSizeY());
    int rmax = static_cast<int>((ymax - m->north())/m->cellSizeY());

    rmin = std::max(0,std::min(m->nrRows()-1,rmin));
    rmax = std::max(0,std::min(m->nrRows()-1,rmax));
    cmin = std::max(0,std::min(m->nrCols()-1,cmin));
    cmax = std::max(0,std::min(m->nrCols()-1,cmax));

    if(cmin > cmax)
    {
        int temp = cmax;
        cmax = cmin;
        cmin =  temp;
    }
    if(rmin > rmax)
    {
        int temp = rmax;
        rmax = rmin;
        rmin =  temp;
    }


    for(int r = rmin; r < rmax + 1; r++)
    {
        for(int c = cmin; c < cmax + 1; c++)
        {
            float celly = ((m->north() + r * m->cellSizeY()));
            float cellx = ((m->west() + c * m->cellSizeX()));

            if(s->Contains(LSMVector2(cellx,celly),m->cellSize()))
            {
                rl.m_R.push_back(r);
                rl.m_C.push_back(c);
                rl.m_A.push_back(1.0f);
            }
        }
    }

    return rl;


}


static inline RasterChangeList RasterPaint(cTMap * m, RasterCoordinateList l, RasterAlterFunc f, float val, float val2, bool overwrite_mv = false, bool paint_mv = false, bool mult_feather = false)
{

    RasterChangeList rcl;

    rcl.target = m;

    for(int i = 0; i < l.m_C.size();i++)
    {
        int r = l.m_R.at(i);
        int c = l.m_C.at(i);
        float alpha = l.m_A.at(i);

        float val_t = val;
        float val2_t = val2;
        if(mult_feather)
        {
            val_t = val * (1.0-alpha);
            val2_t = val2 *(1.0-alpha);
        }
        float val_old = m->data[r][c];
        if(!paint_mv)
        {
            if(pcr::isMV(val_old))
            {
                if(overwrite_mv)
                {
                    float val_new = f(m,r,c,val,val2);

                    rcl.m_C.push_back(c);
                    rcl.m_R.push_back(r);
                    rcl.m_Pre.push_back(m->data[r][c]);
                    rcl.m_Post.push_back(val_new);
                }
            }else {

                float val_new = f(m,r,c,val,val2);

                rcl.m_C.push_back(c);
                rcl.m_R.push_back(r);
                rcl.m_Pre.push_back(m->data[r][c]);
                rcl.m_Post.push_back(alpha * (val_new) + (1.0-alpha) * m->data[r][c]);
            }
        }else {

            float val_new =0.0;

            pcr::setMV(val_new);
            rcl.m_C.push_back(c);
            rcl.m_R.push_back(r);
            rcl.m_Pre.push_back(m->data[r][c]);
            rcl.m_Post.push_back(val_new);

        }



    }

    for(int i = 0; i < rcl.m_C.size();i++)
    {
        int r = rcl.m_R.at(i);
        int c = rcl.m_C.at(i);

        float val_new = rcl.m_Post.at(i);
        m->data[r][c] = val_new;
    }

    return rcl;
}

static inline void RasterUndo(cTMap * m, RasterChangeList rcl)
{
    for(int i = 0; i < rcl.m_C.size();i++)
    {
        int r = rcl.m_R.at(i);
        int c = rcl.m_C.at(i);

        float val_new = rcl.m_Pre.at(i);
        m->data[r][c] = val_new;
    }

}

#include "geo/shapes/shapefile.h"

static inline cTMap * AS_PaintShapeFile(cTMap * m,ShapeFile * s, double value, double value2, int type)
{
     cTMap * target = m->GetCopy();
     RasterAlterFunc f;
     if(type == 0)
     {
         f =PaintToRasterCell;
     }else if(type == 1)
     {
         f = AddToRasterCell;
     }else if(type == 2)
     {
         f = AddToRasterCell;
     }else if(type == 3)
     {
         f = MutiplyToRasterCell;
     }else if(type == 4)
     {
         f = MutiplyToRasterCell;
     }else if(type == 5)
     {
         f = ConvergeRasterCell;
     }else if(type == 6)
     {
         f = BlurRasterCell;
     }

     for(int i = 0; i < s->GetLayerCount(); i++)
     {
        ShapeLayer * l = s->GetLayer(i);
        for(int j = 0; j < l->GetFeatureCount(); j++)
        {
            Feature * fe = l->GetFeature(j);
            for(int k = 0; k < fe->GetShapeCount(); k++)
            {
                LSMShape * shape = fe->GetShape(k);

                RasterCoordinateList l = RasterizeShape(target,shape);
                RasterPaint(target,l,f,value,value2);
            }
        }
     }

     return target;
}

static inline cTMap * AS_PaintShape(cTMap * m,LSMShape * s, double value, double value2, int type)
{
    cTMap * target = m->GetCopy();
     RasterAlterFunc f;
     if(type == 0)
     {
         f =PaintToRasterCell;
     }else if(type == 1)
     {
         f = AddToRasterCell;
     }else if(type == 2)
     {
         f = AddToRasterCell;
     }else if(type == 3)
     {
         f = MutiplyToRasterCell;
     }else if(type == 4)
     {
         f = MutiplyToRasterCell;
     }else if(type == 5)
     {
         f = ConvergeRasterCell;
     }else if(type == 6)
     {
         f = BlurRasterCell;
     }

     RasterCoordinateList l = RasterizeShape(target,s);
     RasterPaint(target,l,f,value,value2);

     return target;
}
static inline cTMap * AS_PaintPolygon(cTMap * m, std::vector<LSMVector2> points, double value, double value2, int type)
{
    int count = points.size();

    cTMap * target = m->GetCopy();
    std::vector<float> x;
    std::vector<float> y;
    for(int i = 0; i < points.size(); i++)
    {
        x.push_back((((LSMVector2)points.at(i))).x);
        y.push_back((((LSMVector2)points.at(i))).y);
    }
    if(count > 0)
    {
        x.push_back(x[0]);
        y.push_back(y[0]);
    }

    LSMPolygon * p = new LSMPolygon();
    Ring * r = new Ring();
    for(int i = 0; i < x.size(); i++)
    {
        r->AddVertex(x[i],y[i]);
    }
    p->SetExtRing(r);
    LSMMultiPolygon * m_PolygonFill = p->Triangulate();

    std::vector<float> m_PolygonFillX;
    std::vector<float> m_PolygonFillY;


    RasterAlterFunc f;
    if(type == 0)
    {
        f =PaintToRasterCell;
    }else if(type == 1)
    {
        f = AddToRasterCell;
    }else if(type == 2)
    {
        f = AddToRasterCell;
    }else if(type == 3)
    {
        f = MutiplyToRasterCell;
    }else if(type == 4)
    {
        f = MutiplyToRasterCell;
    }else if(type == 5)
    {
        f = ConvergeRasterCell;
    }else if(type == 6)
    {
        f = BlurRasterCell;
    }

    int tcount = 0;
    for(int i = 0; i < m_PolygonFill->GetPolygonCount(); i++)
    {
        LSMPolygon * p = m_PolygonFill->GetPolygon(i);
        if(p->GetExtRing()->GetVertexCount() > 2)
        {
            double * data = p->GetExtRing()->GetVertexData();
            m_PolygonFillX.push_back(data[0]);
            m_PolygonFillY.push_back(data[1]);
            m_PolygonFillX.push_back(data[2]);
            m_PolygonFillY.push_back(data[3]);
            m_PolygonFillX.push_back(data[4]);
            m_PolygonFillY.push_back(data[5]);

            tcount ++;
        }
    }

    RasterCoordinateList l = RasterizeTriangles(target,x.data(),y.data(),tcount);
    RasterPaint(target,l,f,value,value2);


    delete p;
    delete r;
    delete m_PolygonFill;
    return target;

}
static inline cTMap * AS_PaintLines(cTMap * m, std::vector<LSMVector2> points, double size, double value, double value2, double feather, int type)
{
    int count = points.size();

    cTMap * target = m->GetCopy();
    std::vector<float> x;
    std::vector<float> y;
    for(int i = 0; i < points.size(); i++)
    {
        x.push_back((((LSMVector2)points.at(i))).x);
        y.push_back((((LSMVector2)points.at(i))).y);
    }

    RasterAlterFunc f;
    if(type == 0)
    {
        f =PaintToRasterCell;
    }else if(type == 1)
    {
        f = AddToRasterCell;
    }else if(type == 2)
    {
        f = AddToRasterCell;
    }else if(type == 3)
    {
        f = MutiplyToRasterCell;
    }else if(type == 4)
    {
        f = MutiplyToRasterCell;
    }else if(type == 5)
    {
        f = ConvergeRasterCell;
    }else if(type == 6)
    {
        f = BlurRasterCell;
    }

    RasterCoordinateList l = RasterizeLines(target,x.data(),y.data(),count,size,feather);
    RasterPaint(target,l,f,value,value2);

    return target;
}
static inline cTMap * AS_PaintEllipsoid(cTMap * m, double x, double y, double sx, double sy, double value, double value2,double feather, int type)
{

    cTMap * target = m->GetCopy();
     RasterAlterFunc f;
     if(type == 0)
     {
         f =PaintToRasterCell;
     }else if(type == 1)
     {
         f = AddToRasterCell;
     }else if(type == 2)
     {
         f = AddToRasterCell;
     }else if(type == 3)
     {
         f = MutiplyToRasterCell;
     }else if(type == 4)
     {
         f = MutiplyToRasterCell;
     }else if(type == 5)
     {
         f = ConvergeRasterCell;
     }else if(type == 6)
     {
         f = BlurRasterCell;
     }

     RasterCoordinateList l = RasterizeEllipsoid(target,x,y,sx,sy,feather);
     RasterPaint(target,l,f,value,value2);

     return target;
}
static inline cTMap * AS_PaintRectangle(cTMap * m, double x, double y, double sx, double sy, double value, double value2,double feather, int type)
{
    cTMap * target = m->GetCopy();
     RasterAlterFunc f;
     if(type == 0)
     {
         f =PaintToRasterCell;
     }else if(type == 1)
     {
         f = AddToRasterCell;
     }else if(type == 2)
     {
         f = AddToRasterCell;
     }else if(type == 3)
     {
         f = MutiplyToRasterCell;
     }else if(type == 4)
     {
         f = MutiplyToRasterCell;
     }else if(type == 5)
     {
         f = ConvergeRasterCell;
     }else if(type == 6)
     {
         f = BlurRasterCell;
     }

     RasterCoordinateList l = RasterizeRectangle(target,x,y,sx,sy,feather);
     RasterPaint(target,l,f,value,value2);

     return target;
}


#endif // RASTERPAINTING_H

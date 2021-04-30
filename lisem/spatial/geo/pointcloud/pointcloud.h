#ifndef POINTCLOUD_H
#define POINTCLOUD_H


#include "geo/geoobject.h"
#include "error.h"
#include "vector"
#include "functional"

typedef struct PointCloudPoint
{

    double m_x;
    double m_y;
    double m_z;
    double m_t;
    float m_r;
    float m_g;
    float m_b;
    int m_class;
    int m_nreturns;
    int m_returnn;
} PointCloudPoint;

class PointCloud : public GeoObject
{
    int m_PointCount = 0;


    bool m_HasColor = false;
    bool m_HasTime = false;

    double m_Xmin = 0.0;
    double m_Xmax = 0.0;
    double m_Ymin = 0.0;
    double m_Ymax = 0.0;
    double m_Zmin = 0.0;
    double m_Zmax = 0.0;
public:

    std::vector<double> m_PosX;
    std::vector<double> m_PosY;
    std::vector<double> m_PosZ;
    std::vector<double> m_PosT;
    std::vector<float> m_ColorR;
    std::vector<float> m_ColorG;
    std::vector<float> m_ColorB;
    std::vector<int> m_Class;
    std::vector<int> m_NReturns;
    std::vector<int> m_ReturnNo;
    std::vector<int> m_ClassTypes;
    std::vector<QString> m_ClassNames;


    inline PointCloud()
    {
        m_BoundingBox = BoundingBox(0.0,0.0,0.0,0.0);
    }

    //is default copy constructible as inherited form std::vector
    PointCloud              (PointCloud const& other)=default;
    PointCloud               (PointCloud&& other)=default;
    ~PointCloud              ()=default;



    inline int GetPointCount()
    {

        return m_PointCount;
    }

    inline void CalculateBoundingBox()
    {
        double xmin = 1e31;
        double xmax = -1e31;
        double ymin = 1e31;
        double ymax = -1e31;
        double zmin = 1e31;
        double zmax = -1e31;

        for(int i = 0; i < m_PointCount; i++)
        {
            xmin = std::min(m_PosX.at(i),xmin);
            xmax = std::max(m_PosX.at(i),xmax);
            ymin = std::min(m_PosY.at(i),ymin);
            ymax = std::max(m_PosY.at(i),ymax);
            zmin = std::min(m_PosZ.at(i),zmin);
            zmax = std::max(m_PosZ.at(i),zmax);
        }

        m_Xmax = xmax;
        m_Xmin = xmin;
        m_Ymax = ymax;
        m_Ymin = ymin;
        m_Zmax = zmax;
        m_Zmin = zmin;

        m_BoundingBox = BoundingBox(m_Xmax,m_Xmin,m_Ymin,m_Ymax);
    }

    inline void Resize(int count)
    {
        //allocate arrays
        m_PosX.resize(count,0.0);
        m_PosY.resize(count,0.0);
        m_PosZ.resize(count,0.0);
        if(m_HasTime)
        {
            m_PosT.resize(count,0.0);
        }
        if(m_HasColor)
        {
            m_ColorR.resize(count,0.0);
            m_ColorG.resize(count,0.0);
            m_ColorB.resize(count,0.0);
        }
        m_Class.resize(count,0.0);
        m_NReturns.resize(count,1.0);
        m_ReturnNo.resize(count,0.0);


        //update point count
        m_PointCount = count;

    }
    inline void SetHasTime(bool x)
    {

        if(!x)
        {
            m_PosT.clear();
        }else if(x && !m_HasTime)
        {
            m_PosT.resize(m_PointCount,0.0);
        }
        m_HasTime = x;

    }
    inline void SetHasColor(bool x)
    {
        if(!x)
        {
            m_ColorR.clear();
            m_ColorG.clear();
            m_ColorB.clear();
        }else if(x && !m_HasColor)
        {
            m_ColorR.resize(m_PointCount,0.0);
            m_ColorG.resize(m_PointCount,0.0);
            m_ColorB.resize(m_PointCount,0.0);

        }
        m_HasColor = x;
    }

    inline bool HasTime()
    {
        return m_HasTime;
    }

    inline bool HasColor()
    {
        return m_HasColor;
    }

    inline void AddClass(int pclass, QString name)
    {
        m_ClassTypes.push_back(pclass);
        m_ClassNames.push_back(name);
    }

    inline void SetPoint(int index, PointCloudPoint p)
    {
        SetPoint(index,p.m_x,p.m_y,p.m_z,p.m_t,p.m_r,p.m_g,p.m_b,p.m_class,p.m_nreturns,p.m_returnn);
    }
    inline void SetPoint(int index, double x, double y, double z, double t, float r, float g, float b, int pointclass, int nreturns, int returnn)
    {
        SetPointPosition(index,x,y,z);
        SetPointTime(index,t);
        SetPointColor(index,r,g,b);
        SetPointClass(index,pointclass);
        SetPointReturnInfo(index,nreturns,returnn);
    }

    inline PointCloudPoint GetPoint(int i)
    {
        if(i > -1 && i <GetPointCount())
        {
            PointCloudPoint p;

            p.m_x = m_PosX.at(i);
            p.m_y = m_PosY.at(i);
            p.m_z = m_PosZ.at(i);
            if(m_HasColor)
            {
                p.m_r = m_ColorR.at(i);
                p.m_g = m_ColorG.at(i);
                p.m_b = m_ColorB.at(i);
            }
            if(m_HasTime)
            {
                p.m_t = m_PosT.at(i);
            }
            p.m_class = m_Class.at(i);
            p.m_returnn = m_ReturnNo.at(i);
            p.m_nreturns = m_NReturns.at(i);

            return p;

        }
        return PointCloudPoint();
    }

    inline void SetPointTime(int index, double time)
    {
        if(m_HasTime)
        {
            if(index > -1 && index < m_PointCount)
            {
                m_PosT[index] = time;
            }
        }

    }

    inline void SetPointPosition(int index, double x, double y, double z)
    {
        if(index > -1 && index < m_PointCount)
        {
            m_PosX[index] = x;
            m_PosY[index] = y;
            m_PosZ[index] = z;
        }
    }
    inline void SetPointColor(int index, float r, float g, float b)
    {
        if(m_HasColor)
        {
            if(index > -1 && index < m_PointCount)
            {
                m_ColorR[index] = r;
                m_ColorG[index] = g;
                m_ColorB[index] = b;
            }
        }
    }
    inline void SetPointReturnInfo(int index, int nreturns, int return_no)
    {
        if(index > -1 && index < m_PointCount)
        {
            m_ReturnNo[index] = return_no;
            m_NReturns[index] = nreturns;
        }
    }

    inline void SetPointClass(int index, int pointclass)
    {
        if(index > -1 && index < m_PointCount)
        {
            m_Class[index] = pointclass;
        }
    }

    inline void CopyFrom(PointCloud * p)
    {
        m_PointCount = p->m_PointCount;

        m_PosX = p->m_PosX;
        m_PosY = p->m_PosY;
        m_PosZ = p->m_PosZ;
        m_PosT = p->m_PosT;
        m_ColorR = p->m_ColorR;
        m_ColorG = p->m_ColorG;
        m_ColorB = p->m_ColorB;
        m_Class = p->m_Class;
        m_NReturns = p->m_NReturns;
        m_ReturnNo = p->m_ReturnNo;
        m_ClassTypes = p->m_ClassTypes;
        m_ClassNames = p->m_ClassNames;

        m_HasColor = p->m_HasColor;
        m_HasTime = p->m_HasTime;
    }


//Angelscript related functions
public:

    QString        AS_FileName          = "";
    bool           AS_writeonassign     = false;
    std::function<void(PointCloud *,QString)> AS_writefunc;
    std::function<PointCloud *(QString)> AS_readfunc;
    bool           AS_Created           = false;
    int            AS_refcount          = 1;
    void           AS_AddRef            ();
    void           AS_ReleaseRef        ();
    PointCloud*    AS_Assign            (PointCloud*);

};

//Angelscript related functionality

inline void PointCloud::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void PointCloud::AS_ReleaseRef()
{
    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {
        delete this;
    }

}

inline PointCloud* PointCloud::AS_Assign(PointCloud* sh)
{
    if(AS_writeonassign)
    {
        AS_writefunc(sh,AS_FileName);
    }
    this->AS_writeonassign = false;
    CopyFrom(sh);
    return this;
}

inline PointCloud* PointCloudFactory()
{
    return new PointCloud();

}

#endif // POINTCLOUD_H

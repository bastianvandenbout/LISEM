#ifndef FIELD_H
#define FIELD_H

#include "defines.h"
#include "geo/geoobject.h"
#include "geo/geoprojection.h"
#include <vector>
#include "map.h"

class LISEM_API Field : public GeoObject
{
public:

    QString m_ParName = "";
    QString m_TimeVal = "";
    std::vector<cTMap*> m_Maps;
    double m_ZStart = 0.0;
    double m_dz = 1.0;


    inline Field()
    {



    }

    inline ~Field()
    {

        Clear();

    }

    inline Field(std::vector<cTMap*> maps, double z_start, double dz, bool copy = true)
    {

        SetFromMapList(maps,z_start,dz,copy);


    }



    inline void CopyFrom(Field * f)
    {

        m_ZStart = f->m_ZStart;
        m_dz = f->m_dz;
        m_Maps.clear();
        m_ParName = f->m_ParName;
        m_TimeVal = f->m_TimeVal;
        for(int i = 0; i < f->m_Maps.size(); i++)
        {
            m_Maps.push_back(f->m_Maps.at(i)->GetCopy());
        }
        if(m_Maps.size() > 0)
        {

            this->m_BoundingBox = m_Maps.at(0)->GetBoundingBox();
            this->m_Projection = m_Maps.at(0)->GetProjection();
        }
    }

    inline void CopyFrom0(Field * f)
    {

        m_ZStart = f->m_ZStart;
        m_dz = f->m_dz;
        m_Maps.clear();
        m_ParName = f->m_ParName;
        m_TimeVal = f->m_TimeVal;
        for(int i = 0; i < f->m_Maps.size(); i++)
        {
            m_Maps.push_back(f->m_Maps.at(i)->GetCopy0());
        }
        if(m_Maps.size() > 0)
        {

            this->m_BoundingBox = m_Maps.at(0)->GetBoundingBox();
            this->m_Projection = m_Maps.at(0)->GetProjection();
        }
    }

    inline Field * GetCopy()
    {

        Field * F = new Field();
        F->CopyFrom(this);

        return F;
    }

    inline Field * GetCopy0()
    {

        Field * F = new Field();
        F->CopyFrom0(this);

        return F;
    }


    inline void Clear()
    {
        for(int i = 0; i < m_Maps.size(); i++)
        {
            delete m_Maps.at(i);
        }
        m_Maps.clear();
        m_ZStart = 0.0;
        m_dz = 1.0;
    }

    inline void SetFromMapList(std::vector<cTMap*> maps, double z_start, double dz, bool copy = true)
    {
        this->m_BoundingBox = maps.at(0)->GetBoundingBox();
        this->m_Projection = maps.at(0)->GetProjection();

        if(copy)
        {
            for(int  i = 0; i < maps.size(); i++)
            {
                maps.at(i) = maps.at(i)->GetCopy();
            }
        }
        m_Maps = maps;
        m_ZStart = z_start;
        m_dz = dz;

    }
    inline float GetWest()
    {
        if(m_Maps.size() > 0)
        {
            return m_Maps.at(0)->west();
        }else
        {
            return 0.0;
        }
    }

    inline float &ValueAt(int l, int r, int c)
    {
        return m_Maps.at(l)->data[r][c];
    }

    inline float GetSizeX()
    {
        if(m_Maps.size() > 0)
        {
            return m_Maps.at(0)->cellSizeX() * ((float)(m_Maps.at(0)->nrCols()));
        }else
        {
            return 0.0;
        }

    }
    inline float GetSizeY()
    {

        if(m_Maps.size() > 0)
        {
            return m_Maps.at(0)->cellSizeY() * ((float)(m_Maps.at(0)->nrRows()));
        }else
        {
            return 0.0;
        }
    }
    inline float GetSizeZ()
    {

        return this->m_dz * ((float)(m_Maps.size()));

    }

    inline float GetNorth()
    {
        if(m_Maps.size() > 0)
        {
            return m_Maps.at(0)->north();
        }else
        {
            return 0.0;
        }
    }
    inline float GetBottom()
    {
        return m_ZStart;

    }


    inline float cellSizeZ()
    {
        return m_dz;
    }



    inline float cellSizeY()
    {
        if(m_Maps.size() > 0)
        {
            return m_Maps.at(0)->cellSizeY();
        }else
        {
            return 0.0;
        }
    }



    inline float cellSizeX()
    {
        if(m_Maps.size() > 0)
        {
            return m_Maps.at(0)->cellSizeX();
        }else
        {
            return 0.0;
        }
    }



    inline void SetCellSizeX(double dx)
    {
        for(int i = 0; i < m_Maps.size(); i++)
        {
            m_Maps.at(i)->setcellSizeX(dx);
        }

    }
    inline void SetCellSizeY(double dy)
    {
        for(int i = 0; i < m_Maps.size(); i++)
        {
            m_Maps.at(i)->setcellSizeY(dy);
        }

    }
    inline void SetCellSizeZ(double dz)
    {
        this->m_dz = dz;

    }

    inline void SetWest(double x)
    {
        for(int i = 0; i < m_Maps.size(); i++)
        {
            m_Maps.at(i)->setwest(x);
        }

    }
    inline void SetNorth(double y)
    {
        for(int i = 0; i < m_Maps.size(); i++)
        {
            m_Maps.at(i)->setnorth(y);
        }

    }
    inline void SetBottom(double z)
    {
        this->m_ZStart = z;
    }
    inline BoundingBox3D GetAABB()
    {
        return BoundingBox3D(GetWest(),GetWest() + GetSizeX(),GetBottom(),GetBottom()+GetSizeZ(),GetNorth(),GetSizeY());
    }
    inline cTMap * at(int i)
    {
        return m_Maps.at(i);
    }
    inline std::vector<cTMap*> GetMapList()
    {
        return m_Maps;
    }

    inline int nrLevels()
    {
        return m_Maps.size();
    }

    inline int nrRows()
    {
        if(m_Maps.size() > 0)
        {
            return m_Maps.at(0)->nrRows();
        }
        return 0;

    }
    inline int nrCols()
    {
        if(m_Maps.size() > 0)
        {
            return m_Maps.at(0)->nrCols();
        }

        return 0;
    }

    inline void GetMinMax(float * mini, float *maxi, float * avgi)
    {
        float min = 1e31;
        float max = -1e31;
        double avg = 0.0;
        double avgn = 0.0;
        for(int i = 0; i < m_Maps.size(); i++)
        {
            for(int r = 0; r < m_Maps.at(i)->nrRows(); r++)
            {
                for(int c = 0; c < m_Maps.at(i)->nrCols(); c++)
                {
                    if(!pcr::isMV(m_Maps.at(i)->data[r][c]))
                    {
                        min = std::min(m_Maps.at(i)->data[r][c],min);
                        max = std::max(m_Maps.at(i)->data[r][c],max);
                        avg +=m_Maps.at(i)->data[r][c];
                        avgn += 1.0;
                    }

                }
            }
        }

        if(avgn > 0.5)
        {
            avg = avg/avgn;
        }
        *mini = min;
        *maxi = max;
        *avgi = avg;
    }


    QString    AS_FileName          = "";
    bool           AS_writeonassign     = false;

    int AS_refcount = 1;
    Field *        Assign            (Field *other);
    Field *        Assign            (float other);
    void           AS_AddRef            ();
    void           AS_ReleaseRef        ();

    std::function<void(Field *,QString)> AS_writefunc;
    std::function<Field *(QString)> AS_readfunc;


    Field *        OpAdd             (Field *other);
    Field *        OpMul             (Field *other);

    Field *        OpNeg             ();
    Field *        OpSub             (Field *other);
    Field *        OpDiv             (Field *other);
    Field *        OpPow             (Field *other);
    Field *        OpAddAssign       (Field *other);
    Field *        OpSubAssign       (Field *other);
    Field *        OpMulAssign       (Field *other);
    Field *        OpDivAssign       (Field *other);
    Field *        OpPowAssign       (Field *other);

    Field *        OpAdd             (float other);
    Field *        OpMul             (float other);

    Field *        OpSub             (float other);
    Field *        OpDiv             (float other);
    Field *        OpPow             (float other);
    Field *        OpAddAssign       (float other);
    Field *        OpSubAssign       (float other);
    Field *        OpMulAssign       (float other);
    Field *        OpDivAssign       (float other);
    Field *        OpPowAssign       (float other);

    Field *        OpModAssign       (Field *other);
    Field *        OpModAssign       (float other);
    Field *        OpMod             (Field *other);
    Field *        OpMod             (float other);

    Field *        EqualTo           (Field *other);
    Field *        LargerThen        (Field *other);
    Field *        SmallerThen       (Field *other);
    Field *        NotEqualTo        (Field *other);
    Field *        LargerEqualThen   (Field *other);
    Field *        SmallerEqualThen  (Field *other);

    Field *        EqualTo           (float other);
    Field *        LargerThen        (float other);
    Field *        SmallerThen       (float other);
    Field *        NotEqualTo        (float other);
    Field *        LargerEqualThen   (float other);
    Field *        SmallerEqualThen  (float other);

    Field *        And               (Field *other);
    Field *        Or                (Field *other);
    Field *        Xor               (Field *other);

    Field *        And               (float other);
    Field *        Or                (float other);
    Field *        Xor               (float other);

    Field *        AndAssign         (Field *other);
    Field *        OrAssign          (Field *other);
    Field *        XorAssign         (Field *other);

    Field *        AndAssign         (float other);
    Field *        OrAssign          (float other);
    Field *        XorAssign         (float other);

    Field *        Negate            ();

    Field *        OpMod_r           (float other);
    Field *        OpAdd_r           (float other);
    Field *        OpMul_r           (float other);
    Field *        OpSub_r           (float other);
    Field *        OpDiv_r           (float other);
    Field *        OpPow_r           (float other);
    Field *        LargerThen_r      (float other);
    Field *        SmallerThen_r     (float other);
    Field *        NotEqualTo_r      (float other);
    Field *        LargerEqualThen_r (float other);
    Field *        SmallerEqualThen_r(float other);
    Field *        EqualTo_r         (float other);
    Field *        And_r             (float other);
    Field *        Or_r              (float other);
    Field *        Xor_r             (float other);

};

inline Field* Field::Assign(Field * other)
{
    if(this->AS_writeonassign)
    {
        this->AS_writefunc(other,AS_FileName);
    }
    {
        this->CopyFrom(other);
    }
    return this;
}

inline Field* Field::Assign(float other)
{
    for(int i = 0; i < m_Maps.size(); i++)
    {
        for(int r = 0; r < m_Maps.at(i)->nrRows(); r++)
        {
            for(int c = 0; c < m_Maps.at(i)->nrCols(); c++)
            {
                m_Maps.at(i)->data[r][c] = other;

            }
        }
    }
    return this;
}

inline void Field::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

    std::cout << "add ref " << AS_refcount << " " << this << std::endl;
}

inline void Field::AS_ReleaseRef()
{

    AS_refcount = AS_refcount - 1;

    std::cout << "release ref " << AS_refcount << " " << this <<  std::endl;

    if(AS_refcount == 0)
    {

        std::cout << "delete " << this << std::endl;
        delete this;
    }

}


inline static Field * FieldFactory()
{
    return new Field();
}

inline static Field * FieldFactory2(int levels, int rows, int cols, float z0, float dz, float y0, float dy, float x0, float dx)
{
    Field * f = new Field();

    std::vector<cTMap *> maps;

    for(int i = 0; i < levels; i++)
    {
        MaskedRaster<float> raster_data(rows,cols,y0,x0,dx,dy);
        cTMap * m = new cTMap(std::move(raster_data),"","");
        maps.push_back(m);
    }
    f->SetFromMapList(maps,z0,dz,false);
    return f;
}


inline static Field * FieldFromMap(cTMap * m, int layers, float z_start, float dz)
{
    std::vector<cTMap *> maps;

    for(int i = 0; i < layers; i++)
    {
        maps.push_back(m->GetCopy());
    }

    Field * f = new Field();

    f->SetFromMapList(maps,z_start,dz,false);

    return f;

}

inline static Field * FieldFromMapList(std::vector<cTMap *> maps, float z_start, float dz)
{

    Field * f = new Field();

    f->SetFromMapList(maps,z_start,dz,true);

    return f;
}

inline static std::vector<cTMap *> MapListFromField(Field * f)
{
    std::vector<cTMap*> ret;
    std::vector<cTMap*> ml = f->GetMapList();

    for(int i = 0; i < ml.size(); i++)
    {
        ret.push_back(ml.at(i)->GetCopy());
    }

    return ret;
}


#endif // FIELD_H

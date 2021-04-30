#pragma once

#include "geo/shapes/shapelayer.h"
#include "geo/geoobject.h"
#include "error.h"
#include <iostream>
#include "ogr_api.h"
#include "ogrsf_frmts.h"
#include "gdal.h"
#include <QList>
#include "functional"
#include "geometry/feature.h"
#include "matrixtable.h"
class ShapeFile : public GeoObject, MatrixTableParent
{

public:

    ShapeFile()=default;

    inline ShapeFile(const ShapeFile &other)
    {
        this->CopyFrom(&other);
    }

private:
    QList<ShapeLayer*> m_Layers;

public:

    ShapeFile               (ShapeFile&& other)=default;
    ~ShapeFile              ()=default;
    ShapeFile&         operator=           (ShapeFile const& other)=delete;
    ShapeFile&         operator=           (ShapeFile&& other)=default;

    inline void AddLayer(ShapeLayer* l)
    {
        m_Layers.append(l);
    }

    inline ShapeLayer * GetLayer(int i) const
    {
        return m_Layers.at(i);
    }
    inline int GetLayerCount() const
    {
        return m_Layers.length();
    }
    inline int GetShapeCount()
    {
        int count = 0;
        for(int i =0;i < m_Layers.length(); i++)
        {
            count += m_Layers.at(i)->GetShapeCount();
        }
        return count;
    }

    inline ShapeFile * GetCopy()
    {
        ShapeFile * s = new ShapeFile();
        s->CopyFrom(this,false);
        return s;
    }
    inline void CopyFrom(const ShapeFile * s, bool copy_filename = true)
    {

        AS_Created = s->AS_Created;
        if(copy_filename)
        {
            AS_FileName = s->AS_FileName;
        }

        for(int i =0; i < s->GetLayerCount(); i++)
        {
            this->AddLayer(s->GetLayer(i)->Copy());
        }

        m_Projection = s->m_Projection;
        m_BoundingBox = s->m_BoundingBox;
    }

    inline void Destroy()
    {
        for(int i =0; i < m_Layers.length(); i++)
        {
            ShapeLayer * l = m_Layers.at(i);
            if(l != nullptr)
            {
                l->Destroy();
                delete l;
            }
        }
        m_Layers.clear();
    }

    inline BoundingBox GetBoundingBox()
    {
        return m_BoundingBox;
    }

    inline BoundingBox GetAndCalcBoundingBox()
    {
        BoundingBox b;
        if(m_Layers.length() > 0)
        {
            b = m_Layers.at(0)->GetAndCalcBoundingBox();

            for(int i = 1; i < m_Layers.length(); i++)
            {
                b.MergeWith(m_Layers.at(i)->GetAndCalcBoundingBox());
            }
        }
        m_BoundingBox = b;

        return b;
    }

    //presupposed that the input is in the same CRS as the native data from this layer
    //additionaly assumes that the pos vector is also in this CRS (transform before calling this function)
    inline QList<Feature *> GetFeaturesAt(LSMVector2 pos, double tolerence)
    {
        QList<Feature *> result;

        //for each layer get the features that math the location and add to list
        for(int i = 0; i < m_Layers.length(); i++)
        {
            QList<Feature *> layerresult = m_Layers.at(i)->GetFeaturesAt(pos, tolerence);

            result.append(layerresult);
        }

        return result;
    }

    inline MatrixTable * GetFeatureAttributes(Feature * f)
    {
        MatrixTable * t = new MatrixTable();

        //for each layer get the features that math the location and add to list
        for(int i = 0; i < m_Layers.length(); i++)
        {
            int index = m_Layers.at(i)->HasFeature(f);
            if(index > -1)
            {
                int n = m_Layers.at(i)->GetNumberOfAttributes();
                t->SetSize(n,2);

                for(int j = 0; j < m_Layers.at(i)->GetNumberOfAttributes(); j++)
                {

                    t->SetValue(j,0,m_Layers.at(i)->GetAttributeName(j));
                    t->SetValue(j,1,m_Layers.at(i)->GetAttributeList(j)->GetValueAsString(index));

                }
                return t;
            }
        }



        return t;
    }

    LSMVector2 GetClosestEdgeLocation(LSMVector2 pos, ShapeLayer ** l_ret = nullptr, Feature ** f_ret = nullptr, LSMShape ** s_ret = nullptr)
    {
        LSMVector2 closest = LSMVector2(1e31,1e31);
        float mindist = 1e31;
        if(s_ret != nullptr)
        {
            *s_ret = nullptr;
        }if(f_ret != nullptr)
        {
            *f_ret = nullptr;
        }if(l_ret != nullptr)
        {
            *l_ret = nullptr;
        }

        for(int j = 0; j < m_Layers.length(); j++)
        {
            Feature ** f_retj = new (Feature *)();
            LSMShape ** s_retj  = new (LSMShape *)();
            LSMVector2 p = m_Layers.at(j)->GetClosestEdgeLocation(pos,f_retj,s_retj);
            float distance = LSMVector2(pos - p).length();
            if(std::isfinite(distance))
            {
                if(j == 0)
                {
                    mindist = distance;
                    closest = p;
                    if(s_ret != nullptr && s_retj != nullptr)
                    {
                        *s_ret = *s_retj;
                    }if(f_ret != nullptr && f_retj != nullptr)
                    {
                        *f_ret = *f_retj;
                    }if(l_ret != nullptr)
                    {
                        *l_ret =m_Layers.at(j);
                    }
                }else if(distance < mindist)
                {
                    mindist = distance;
                    closest = p;
                    if(s_ret != nullptr && s_retj != nullptr)
                    {
                        *s_ret = *s_retj;
                    }if(f_ret != nullptr && f_retj != nullptr)
                    {
                        *f_ret =  *f_retj;
                    }if(l_ret != nullptr)
                    {
                        *l_ret =m_Layers.at(j);
                    }
                }
            }
            delete f_retj;
            delete s_retj;
        }

        return closest;
    }

    LSMVector2 GetClosestVertex(LSMVector2 pos, ShapeLayer ** l_ret = nullptr, Feature ** f_ret = nullptr, LSMShape ** s_ret = nullptr)
    {
        LSMVector2 closest = LSMVector2(1e31,1e31);
        float mindist = 1e31;
        if(s_ret != nullptr)
        {
            *s_ret = nullptr;
        }if(f_ret != nullptr)
        {
            *f_ret = nullptr;
        }if(l_ret != nullptr)
        {
            *l_ret = nullptr;
        }

        for(int j = 0; j < m_Layers.length(); j++)
        {
            Feature ** f_retj = new (Feature *)();
            LSMShape ** s_retj  = new (LSMShape *)();
            LSMVector2 p = m_Layers.at(j)->GetClosestVertex(pos,f_retj,s_retj);
            float distance = LSMVector2(pos - p).length();
            if(std::isfinite(distance))
            {
                if(j == 0)
                {
                    mindist = distance;
                    closest = p;
                    if(s_ret != nullptr && s_retj != nullptr)
                    {
                        *s_ret = *s_retj;
                    }if(f_ret != nullptr && f_retj != nullptr)
                    {
                        *f_ret = *f_retj;
                    }if(l_ret != nullptr)
                    {
                        *l_ret =m_Layers.at(j);
                    }
                }else if(distance < mindist)
                {
                    mindist = distance;
                    closest = p;
                    if(s_ret != nullptr && s_retj != nullptr)
                    {
                        *s_ret = *s_retj;
                    }if(f_ret != nullptr && f_retj != nullptr)
                    {
                        *f_ret =  *f_retj;
                    }if(l_ret != nullptr)
                    {
                        *l_ret =m_Layers.at(j);
                    }
                }
            }
            delete f_retj;
            delete s_retj;
        }

        return closest;
    }




    inline virtual void ReplaceValues(MatrixTable * t, QString name) override
    {

        //from this table, we will either replace or create a new attribute
        //the location for the copy-in
        if(t->GetNumberOfCols() > 0)
        {
            if(m_Layers.length()>0)
            {
                if(name.length() > 0)
                {
                    ShapeLayer *l = this->GetLayer(0);

                    //try to find the attribute in the layer
                    int attribindex = l->GetAttributeListIndex(name);
                    // if we found it, update it
                    if(attribindex > 0)
                    {

                        //else, make a new attribute
                    }else
                    {
                        attribindex = l->AddAttribute(name,t->GetColumnType(0));
                    }

                    //try to get a value for all the features
                    for(int i = 0; i < l->GetFeatureCount(); i++)
                    {
                        l->SetFeatureAttribute(i,name,t->GetValueStringQ(i,0));
                    }
                }else
                {
                    this->GetLayer(0)->SetAttributeTable(t);
                }

            }

        }
    }

    inline virtual void UpdateParent() override
    {
        if(AS_writeonassign)
        {
            AS_writefunc(this,AS_FileName);
        }
        this->AS_writeonassign = false;




    }




    inline bool Overlaps(std::vector<double> & x,std::vector<double> & y)
    {

        for(int i = 0; i < m_Layers.length(); i++)
        {
            if(m_Layers.at(i)->Overlaps(x,y))
            {
                return true;
            }
        }

        return false;
    }

    inline QList<Feature*> GetOverlappingFeatures(std::vector<double> & x,std::vector<double> & y)
    {
        QList<Feature*> l;
        for(int i = 0; i < m_Layers.length(); i++)
        {
             l.append(m_Layers.at(i)->GetOverlappingFeatures(x,y));

        }
        return l;
    }

    inline QList<std::pair<LSMShape*,Feature*>> GetOverlappingShapes(std::vector<double> & x,std::vector<double> & y)
    {
        QList<std::pair<LSMShape*,Feature*>> l;
        for(int i = 0; i < m_Layers.length(); i++)
        {
             l.append(m_Layers.at(i)->GetOverlappingShapes(x,y));

        }
        return l;
    }
    inline QList<std::tuple<double*,double*,LSMShape*,Feature*>> GetOverlappingVertices(std::vector<double> & x,std::vector<double> & y)
    {
        QList<std::tuple<double*,double*,LSMShape*,Feature*>> l;
        for(int i = 0; i < m_Layers.length(); i++)
        {
             l.append(m_Layers.at(i)->GetOverlappingVertices(x,y));

        }
        return l;
    }




//Angelscript related functions
public:

    QString    AS_FileName          = "";
    bool           AS_writeonassign     = false;
    std::function<void(ShapeFile *,QString)> AS_writefunc;
    std::function<ShapeFile *(QString)> AS_readfunc;
    bool           AS_Created           = false;
    int            AS_refcount          = 1;
    void           AS_AddRef            ();
    void           AS_ReleaseRef        ();
    ShapeFile*     AS_Assign            (ShapeFile*);

    /*ShapeFile *    AS_And               (ShapeFile * other);
    ShapeFile *    AS_Or                (ShapeFile * other);
    ShapeFile *    AS_Xor               (ShapeFile * other);
    ShapeFile *    AS_Add               (ShapeFile * other);
    ShapeFile *    AS_Sub               (ShapeFile * other);
    */

    void           AS_SetAttributeValue (QString name, int index, QString value);
    MatrixTable *  AS_GetTable          ();
    MatrixTable *  AS_GetTable          (QString name);
    void           AS_SetTable          (MatrixTable*t);
    ShapeLayer * AS_GetLayer            (int index);
    int AS_GetLayerCount                ();


    //LSMVector2 AS_GetCentroid();
    //bool AS_Contains(LSMVector2 v);
    //double AS_GetLength();
    //double AS_GetArea();
    //void AS_Move(LSMVector2 m);
    //LSMVector2 AS_GetClosestVertex(LSMVector2 v);
    //LSMVector2 AS_GetClosestEdgePoint(LSMVector2 v);
    //BoundingBox AS_GetBoundingBox();

};

//Angelscript related functionality

inline void ShapeFile::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void ShapeFile::AS_ReleaseRef()
{
    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {
        delete this;
    }

}

inline ShapeFile* ShapeFile::AS_Assign(ShapeFile* sh)
{
    if(AS_writeonassign)
    {

        AS_writefunc(sh,AS_FileName);
    }
    Destroy();
    this->AS_writeonassign = false;
    CopyFrom(sh);
    return this;
}

inline ShapeFile* ShapesFactory()
{
    return new ShapeFile();

}

inline ShapeFile* FromGDALDataset(GDALDataset * d)
{
    ShapeFile * ret = new ShapeFile();

    int nlayers = d->GetLayerCount();
    for(int i = 0; i < nlayers; i++)
    {
        ShapeLayer * l = FromGDALLayer(d->GetLayer(i));
        ret->AddLayer(l);
    }

    return ret;
}

inline GDALDataset * ToGDALDataset(GDALDataset * to, ShapeFile* shapefile)
{
    //return new ShapeFile();

    int nlayers = shapefile->GetLayerCount();

    for(int i = 0; i < nlayers;i++)
    {
        int count_pre = shapefile->GetLayer(i)->GetFeatureCount();
        OGRLayer *poLayer = ToGDALLayer(to,shapefile->GetLayer(i),shapefile->GetProjection());
        int count_post = poLayer->GetFeatureCount();

    }

    return to;
}

inline void ShapeFile::AS_SetAttributeValue (QString name, int index, QString value)
{
    if(m_Layers.length() > 0)
    {
        m_Layers.at(0)->AS_SetFeatureAttribute(index,name,value);
    }

}

inline MatrixTable *  ShapeFile::AS_GetTable()
{
    if(AS_writeonassign)
    {
        ShapeFile*target;
        target = AS_readfunc(AS_FileName);
        this->CopyFrom(target,false);
        target->Destroy();
        delete target;
    }

    if(m_Layers.length() > 0)
    {
        MatrixTable * ret = m_Layers.at(0)->GetAttributeTable();
        ret->MTParent = this;
        ret->HasParent = true;
        ret->Parent_sr = 0;
        ret->Parent_sc = 0;
        ret->Parent_scname = "";
        return ret;
    }else {
        return new MatrixTable();
    }
}

inline void ShapeFile::AS_SetTable(MatrixTable * T)
{
    if(GetLayerCount() > 0)
    {
        this->GetLayer(0)->SetAttributeTable(T);
    }

}

inline MatrixTable *  ShapeFile::AS_GetTable(QString name)
{
    if(AS_writeonassign)
    {
        ShapeFile*target;
        target = AS_readfunc(AS_FileName);
        this->CopyFrom(target,false);
        target->Destroy();
        delete target;
    }

    if(m_Layers.length() > 0)
    {
        int index = m_Layers.at(0)->GetAttributeListIndex(name);
        /*if(index < 0)
        {
            m_Layers.at(0)->AddAttribute(name,ATTRIBUTE_TYPE_STRING);
        }*/
        MatrixTable * ret = m_Layers.at(0)->GetAttributeTable(name);
        ret->MTParent = this;
        ret->HasParent = true;
        ret->Parent_sr = 0;
        ret->Parent_sc = 0;
        ret->Parent_scname = name;
        return ret;
    }
    return new MatrixTable();
}

inline ShapeLayer * ShapeFile::AS_GetLayer(int index)
{
    if(index < 0 || index >= m_Layers.size())
    {
        LISEMS_ERROR("Shapefile::GetLayer(int) out of bounds, index does not represent a valid layer");
        throw 1;
    }
    return m_Layers.at(index);

}

inline int ShapeFile::AS_GetLayerCount                ()
{
    return m_Layers.size();
}

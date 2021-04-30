#pragma once

#include "openglcltexture.h"
#include "geo/geoprojection.h"
#include "geo/geocoordtransform.h"
#include "openglclmanager.h"
#include "layer/geowindowstate.h"

class WorldGLTransform
{
    GeoProjection m_Pfrom;
    GeoProjection m_Pto;

    OpenGLCLTexture * m_TextureX = nullptr;
    OpenGLCLTexture * m_TextureY = nullptr;
    double * p_x;
    double * p_y;
    float * pf_x;
    float * pf_y;

    bool m_IsGeneric = false;
    bool m_Created = false;
    int m_Count = 0;

    int m_x;
    int m_y;
    int m_xorg;
    int m_yorg;

    GeoCoordTransformer * transformer;

    BoundingBox m_BoundingBox;
public:
    inline void Destroy(OpenGLCLManager *m)
    {

        if(m_Created)
        {
            if(!m_IsGeneric)
            {
                m->DestroyMCLGLTexture(m_TextureX);
                m->DestroyMCLGLTexture(m_TextureY);

                delete[] p_x;
                delete[] p_y;
                delete[] pf_x;
                delete[] pf_y;
            }

            delete transformer;
            m_Created = false;
        }

        m_TextureX = nullptr;
        m_TextureY = nullptr;

    }

    inline OpenGLCLTexture * GetTextureX()
    {
        return m_TextureX;
    }

    inline OpenGLCLTexture * GetTextureY()
    {
        return m_TextureY;
    }

    inline bool IsGeneric()
    {
        return m_IsGeneric;
    }
    inline bool IsCreated()
    {
        return m_Created;
    }

    inline GeoProjection GetProjectionFrom()
    {
        return m_Pfrom;
    }
    inline GeoProjection GetProjectionTo()
    {
        return m_Pto;
    }
    inline void Create(OpenGLCLManager *m, int x, int y, GeoProjection from, GeoProjection to)
    {

        m_xorg =x;
        m_yorg =y;
        m_x = std::max(10,x/25);
        m_y = std::max(10,y/25);
        transformer = GeoCoordTransformer::GetCoordTransformer(from,to);

        if(!transformer->IsGeneric())
        {
            int count = m_x * m_y;

            p_x =(double*) new double[count];
            p_y =(double*) new double[count];
            pf_x =(float*) new float[count];
            pf_y =(float*) new float[count];

            if(p_x == nullptr || p_y == nullptr)
            {
                m_IsGeneric = true;

            }else {


                m_TextureX= m->GetMCLGLLinearTexture(m_x,m_y,0.0,true);
                m_TextureY= m->GetMCLGLLinearTexture(m_x,m_y,0.0,true);
            }


        }else {
            m_IsGeneric = true;
        }


        m_Pfrom =from;
        m_Pto = to;



        m_Created = true;
        m_Count ++;


    }

    inline void UpdateFromWindowState(OpenGLCLManager *m, GeoWindowState s)
    {
        if(!m_IsGeneric)
        {
            double pix_dx = (s.brx-s.tlx)/((double)(m_x));
            double pix_dy = (s.bry-s.tly)/((double)(m_y));

            double tlx = s.tlx;
            double tly = s.tly;

            for(int r = 0; r < m_y; r++)
            {
                for(int c = 0; c < m_x; c++)
                {
                    int i = c + r*m_x;
                    double x= c;
                    double y= r;
                    p_x[i] = tlx + (x+0.5) * pix_dx;
                    p_y[i] = tly + (y+0.5) * pix_dy;
                }
            }

            transformer->Transform(m_x*m_y,p_x,p_y);

            float xmin = 1e31;
            float xmax = -1e31;
            float ymin = 1e31;
            float ymax = -1e31;

            for(int r = 0; r < m_y; r++)
            {
                for(int c = 0; c < m_x; c++)
                {
                    int i = c + r*m_x;
                    pf_x[i] = float(p_x[i]);
                    pf_y[i] = float(p_y[i]);

                    xmin = std::min(xmin,pf_x[i]);
                    xmax = std::max(xmax,pf_x[i]);
                    ymin = std::min(ymin,pf_y[i]);
                    ymax = std::max(ymax,pf_y[i]);
                }
            }

            m_BoundingBox.Set(xmin,xmax,ymin,ymax);
            m_TextureX->FillData2DRF32(pf_x);
            m_TextureY->FillData2DRF32(pf_y);
        }
    }
    inline BoundingBox GetBoundingBox()
    {
        return m_BoundingBox;
    }
    inline void SetRef(int x)
    {
        m_Count = x;
    }
    inline int GetRefs()
    {
        return m_Count;
    }
    inline void AddRef()
    {
        m_Count ++;
    }

    inline bool RemoveRef()
    {
        m_Count --;

        if(m_Count == 0)
        {
            return true;
        }else {
            return false;
        }
    }
};

class World3DGLTransform
{








};





class WorldGLTransformManager
{

private:


    OpenGLCLManager *m_OpenGLCLManager;

    QList<WorldGLTransform *> m_TransformList;

public:

    inline WorldGLTransformManager(OpenGLCLManager * m)
    {
        m_OpenGLCLManager =m;
    }


    inline void RemoveAll()
    {
        for(int i = 0; i < m_TransformList.length(); i++)
        {
            WorldGLTransform *t = m_TransformList.at(i);
            t->Destroy(m_OpenGLCLManager);
        }
        m_TransformList.clear();
    }

    inline void ResetRefsZero()
    {
        for(int i = 0; i < m_TransformList.length(); i++)
        {
             WorldGLTransform *t = m_TransformList.at(i);
             t->SetRef(0);
        }
    }

    inline void RemoveWithRefsZero()
    {
        for(int i = m_TransformList.length()-1; i >-1; i--)
        {
             WorldGLTransform *t = m_TransformList.at(i);
             if(t->GetRefs() == 0)
             {
                t->Destroy(m_OpenGLCLManager);
                m_TransformList.removeAt(i);
             }
        }
    }

    inline void Remove(GeoProjection from, GeoProjection to)
    {
        WorldGLTransform * r = Get(from, to);
        for(int i = 0; i < m_TransformList.length(); i++)
        {
            WorldGLTransform *t = m_TransformList.at(i);
            if(t == r)
            {
                if(t->RemoveRef())
                {
                    t->Destroy(m_OpenGLCLManager);
                    m_TransformList.removeAt(i);
                    break;
                }
            }
        }
    }

    inline bool Add(int x, int y,GeoProjection from, GeoProjection to)
    {
        WorldGLTransform * t =Get(from,to);

        if(t == nullptr)
        {

            WorldGLTransform * t = new WorldGLTransform();
            t->Create(m_OpenGLCLManager,x,y,from,to);
            m_TransformList.append(t);


            return true;
        }else {
            t->AddRef();
            return false;
        }
    }

    inline WorldGLTransform *Get(GeoProjection from, GeoProjection to)
    {
        for(int i = 0; i < m_TransformList.length(); i++)
        {
            WorldGLTransform *t = m_TransformList.at(i);
            if(t->GetProjectionTo().IsEqualTo(to) && t->GetProjectionFrom().IsEqualTo(from))
            {
                return t;
            }

        }

        return nullptr;


    }



    inline void UpdateFromWindowState(GeoWindowState s)
    {
        for(int i = 0; i < m_TransformList.length(); i++)
        {
            WorldGLTransform *t = m_TransformList.at(i);
            t->UpdateFromWindowState(m_OpenGLCLManager,s);
        }
    }
};

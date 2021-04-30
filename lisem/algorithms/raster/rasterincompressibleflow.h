#ifndef RASTERINCOMPRESSIBLEFLOW_H
#define RASTERINCOMPRESSIBLEFLOW_H

#include "QString"
#include "geo/raster/map.h"
#include "rasterconstructors.h"
#include "linear/lsm_vector3.h"
#include "raster/rastercommon.h"
#include "rasterderivative.h"
#include "navierstokes/fluid2p2d.h"
#include "navierstokes/fluid2p3d.h"


inline std::vector<cTMap *> AS_IncompressibleWave(cTMap * M, cTMap * VX, cTMap * VY,cTMap * P, cTMap * LS, cTMap * Block, cTMap * BlockUX, cTMap * BlockUY, float visc, float _dt, float courant, float b0)
{


    if(!(M->data.nr_rows() == VX->data.nr_rows() && M->data.nr_cols() == VX->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for VX");
       throw -1;
    }
    if(!(M->data.nr_rows() == VY->data.nr_rows() && M->data.nr_cols() == VY->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for VY");
       throw -1;
    }
    if(!(M->data.nr_rows() == P->data.nr_rows() && M->data.nr_cols() == P->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for P");
       throw -1;
    }
    if(!(M->data.nr_rows() == Block->data.nr_rows() && M->data.nr_cols() == Block->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for P");
       throw -1;
    }
    if(!(M->data.nr_rows() == BlockUX->data.nr_rows() && M->data.nr_cols() == BlockUX->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for P");
       throw -1;
    }
    if(!(M->data.nr_rows() == BlockUY->data.nr_rows() && M->data.nr_cols() == BlockUY->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for P");
       throw -1;
    }
    if(!(M->data.nr_rows() == LS->data.nr_rows() && M->data.nr_cols() == LS->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for P");
       throw -1;
    }

    /*if(!(M->data.nr_rows()*4 == px->data.nr_rows() && M->data.nr_cols()*4 == px->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for px (should be four times mass resolution)");
       throw -1;
    }
    if(!(M->data.nr_rows()*4 == py->data.nr_rows() && M->data.nr_cols()*4 == py->data.nr_cols()))
    {
       LISEMS_ERROR("Numbers of rows and column do not match for py (should be four times mass resolution)");
       throw -1;
    }*/

    cTMap * HN = M->GetCopy();
    cTMap * VXN = VX->GetCopy();
    cTMap * VYN = VY->GetCopy();
    cTMap * PN = P->GetCopy();
    cTMap * LSN = LS->GetCopy();
    Fluid2P2D f(HN->nrRows(),HN->nrCols(),std::fabs(HN->cellSizeX()),std::fabs(HN->cellSizeY()),b0);

    f.InitializeFromData(M,VX,VY,P,LS, Block);

    f.TimeStep(_dt);

    f.StoreToData(HN,VXN,VYN,PN,LSN);

    f.Destroy();

    return {HN,VXN,VYN,PN,LSN};

}

inline static QList<cTMap *> CopyQML(QList<cTMap*> in)
{
     QList<cTMap *> out;
     for(int i = 0; i < in.size(); i++)
     {
         out.append(in.at(i)->GetCopy0());
     }
     return out;
}

inline std::vector<cTMap *> AS_IncompressibleFlow3D(std::vector<cTMap *> maps,std::vector<cTMap *> Block, std::vector<cTMap *> Blockux,std::vector<cTMap *> Blockuy,std::vector<cTMap *> Blockuz,float csz, float visc, float diff, float _dt, float courant)
{

    if(maps.size() < 12)
    {
        LISEMS_ERROR("Unable to work with less than 2 vertical layers");
        throw -1;
    }

    if(!(maps.size() % 6 == 0))
    {
        LISEMS_ERROR("number of maps must be multiple of 6 (dens, vx,vy,vz,p,ls, dens,vx,vy,vz,p,ls,... from bottom to top)");
        throw -1;
    }


    QList<cTMap*> DENS;
    QList<cTMap*> VX;
    QList<cTMap*> VY;
    QList<cTMap*> VZ;
    QList<cTMap*> P;
    QList<cTMap*> LS;

    for(int i = 0; i < maps.size(); i++)
    {
        if(!(maps.at(i)->data.nr_rows() == maps.at(0)->data.nr_rows() && maps.at(i)->data.nr_cols() == maps.at(0)->data.nr_cols()))
        {
           LISEMS_ERROR("Numbers of rows and column do not match for dem and input state");
           throw -1;
        }

        if(i % 6 == 0)
        {
            DENS.append(maps.at(i));

        }
        if(i % 6 == 1)
        {
            VX.append(maps.at(i));
        }
        if(i % 6 == 2)
        {
            VY.append(maps.at(i));
        }
        if(i % 6 == 3)
        {
            VZ.append(maps.at(i));
        }
        if(i % 6 == 4)
        {
            P.append(maps.at(i));
        }
        if(i % 6 == 5)
        {
            LS.append(maps.at(i));
        }
    }

    cTMap * HN =DENS.at(0);

    Fluid2P3D f(HN->nrRows(),HN->nrCols(),DENS.length(),std::fabs(HN->cellSizeX()),std::fabs(HN->cellSizeY()),std::fabs(csz));

    f.InitializeFromData(DENS,VX,VY,VZ,P,LS);

    f.TimeStep(_dt);


    QList<cTMap*> DENSN = CopyQML(DENS);
    QList<cTMap*> VXN = CopyQML(VX);
    QList<cTMap*> VYN = CopyQML(VY);
    QList<cTMap*> VZN = CopyQML(VZ);
    QList<cTMap*> PN = CopyQML(P);
    QList<cTMap*> LSN = CopyQML(LS);


    f.StoreToData(DENSN, VXN,VYN,VZN,PN,LSN);

    std::vector<cTMap*> ret;


    for(int i = 0; i < DENSN.length(); i++)
    {

        ret.push_back(DENSN.at(i));
        ret.push_back(VXN.at(i));
        ret.push_back(VYN.at(i));
        ret.push_back(VZN.at(i));
        ret.push_back(PN.at(i));
        ret.push_back(LSN.at(i));
    }

    return ret;

}


#endif // RASTERINCOMPRESSIBLEFLOW_H

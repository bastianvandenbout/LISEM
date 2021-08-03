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
#include "geo/raster/field.h"


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

    f.InitializeFromData(M,VX,VY,P,LS, Block, BlockUX, BlockUY);

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

inline std::vector<Field *> AS_IncompressibleFlow3D(std::vector<Field *> maps,Field* Block, Field* Blockux,Field* Blockuy,Field* Blockuz,float csz, float visc, float diff, float _dt, float courant)
{


    if((maps.size() != 6 ))
    {
        LISEMS_ERROR("number of fields must be 6 (density, velocity-x, velocity-y, velocity-z, pressure, level-set)");
        throw -1;
    }


    std::vector<cTMap*> DENS = maps.at(0)->GetMapList();
    std::vector<cTMap*> VX = maps.at(1)->GetMapList();
    std::vector<cTMap*> VY = maps.at(2)->GetMapList();
    std::vector<cTMap*> VZ = maps.at(3)->GetMapList();
    std::vector<cTMap*> P = maps.at(4)->GetMapList();
    std::vector<cTMap*> LS = maps.at(5)->GetMapList();

    for(int i = 0; i < maps.size(); i++)
    {
        if(!(maps.at(i)->nrLevels() == maps.at(0)->nrLevels() && maps.at(i)->nrRows() == maps.at(0)->nrRows() && maps.at(i)->nrCols() == maps.at(0)->nrCols()))
        {
           LISEMS_ERROR("Numbers of rows and column do not match for dem and input state");
           throw -1;
        }
    }

    cTMap * HN =DENS.at(0);

    Fluid2P3D f(HN->nrRows(),HN->nrCols(),DENS.size(),std::fabs(HN->cellSizeX()),std::fabs(HN->cellSizeY()),std::fabs(csz));

    f.InitializeFromData(DENS,VX,VY,VZ,P,LS, Block->GetMapList(), Blockux->GetMapList(), Blockuy->GetMapList(), Blockuz->GetMapList());

    f.TimeStep(_dt);

    Field *DENSN = maps.at(0)->GetCopy();
    Field *VXN = maps.at(1)->GetCopy();
    Field *VYN = maps.at(2)->GetCopy();
    Field *VZN = maps.at(3)->GetCopy();
    Field *PN= maps.at(4)->GetCopy();
    Field *LSN = maps.at(5)->GetCopy();

    f.StoreToData(DENSN->GetMapList(), VXN->GetMapList(),VYN->GetMapList(),VZN->GetMapList(),PN->GetMapList(),LSN->GetMapList());


    return {DENSN, VXN,VYN,VZN,PN,LSN};

}


#endif // RASTERINCOMPRESSIBLEFLOW_H

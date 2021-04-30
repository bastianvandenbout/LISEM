
#include "gdal.h"
#include "ogr_spatialref.h"
#include "ogr_api.h"
#include "gdal_utils.h"
#include "geo/shapes/shapefile.h"
#include "geo/shapes/shapelayer.h"
#include <QString>
#include "error.h"
#include <iostream>
#include "ogr_api.h"
#include "ogrsf_frmts.h"
#include "ogr_spatialref.h"
#include "gdal_utils.h"
#include "gdal.h"
#include "geo/raster/map.h"
#include "mathfunction.h"
#include "raster/rasterconstructors.h"


inline static cTMap * AS_MapFunction(cTMap * inmap, MathFunction f)
{
    if(f.GetParameterCount() != 1)
    {
        LISEMS_ERROR("Could not run math-expression with only one parameter, function has " + QString::number(f.GetParameterCount()));
        throw 1;
    }

    MaskedRaster<float> raster_data(inmap->data.nr_rows(), inmap->data.nr_cols(), inmap->data.north(), inmap->data.west(), inmap->data.cell_size(),inmap->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),inmap->projection(),"");


    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(inmap->data[r][c]))
            {

                map->data[r][c] = f.AS_Int_Evaluate({inmap->data[r][c]});

            }else {

                pcr::setMV(map->data[r][c]);
            }
        }
    }


    return map;
}

inline static cTMap * AS_MapFunction(std::vector<cTMap*> maps, MathFunction f)
{
    std::cout << f.AS_GetExpression().toStdString() << std::endl;

    if(maps.size() != f.GetParameterCount())
    {
        LISEMS_ERROR("Could not run math-expression with "+QString::number(maps.size()) +  " parameters, function has " + QString::number(f.GetParameterCount()));
        throw 1;
    }

    if(maps.size() == 0)
    {
        return MapFactory((float)f.AS_Int_Evaluate({}));
    }

    cTMap * inmap = maps.at(0);

    for(int i = 1; i < maps.size(); i++)
    {
        if(maps.at(i)->nrCols() != inmap->nrCols() || maps.at(i)->nrRows() != inmap->nrRows())
        {
            LISEMS_ERROR("Could not run math-expression with input maps of different size, at index " + QString::number(i));
            throw 1;
        }
    }

    MaskedRaster<float> raster_data(inmap->data.nr_rows(), inmap->data.nr_cols(), inmap->data.north(), inmap->data.west(), inmap->data.cell_size(),inmap->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),inmap->projection(),"");



    f =MathFunction(f.Expression);

    std::vector<double> vals;
    for(int i = 0; i < maps.size(); i++)
    {
        vals.push_back(0.0);
    }
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {

            bool ismv = false;
            for(int i = 0; i < maps.size(); i++)
            {
                if(pcr::isMV(maps.at(i)->data[r][c]))
                {
                    ismv = true;
                    break;
                }else {
                    vals[i] = maps.at(i)->data[r][c];
                }
            }
            if(!ismv)
            {

                map->data[r][c] = f.AS_Int_Evaluate(vals);

            }else {

                pcr::setMV(map->data[r][c]);
            }
        }
    }


    return map;
}

/*inline static MatrixTable* AS_MapFunctionT(MatrixTable * map, MathFunction * f)
{


    return new MatrixTable();
}

inline static MatrixTable* AS_MapFunctionT(CScriptArray * maps, MathFunction * f)
{


    return new MatrixTable();
}*/




inline static cTMap * AS_MapFunction(cTMap * map, asIScriptFunction *f /* double f(double)*/)
{

    asIScriptContext *cmpContext = 0;
    bool isNested = false;
    {
        // Try to reuse the active context
        cmpContext = asGetActiveContext();
        if( cmpContext )
        {
            if( cmpContext->GetEngine() == f->GetEngine() && cmpContext->PushState() >= 0 )
                isNested = true;
            else
                cmpContext = 0;
        }
        if( cmpContext == 0 )
        {
            // TODO: Ideally this context would be retrieved from a pool, so we don't have to
            //       create a new one everytime. We could keep a context with the array object
            //       but that would consume a lot of resources as each context is quite heavy.
            cmpContext = f->GetEngine()->CreateContext();
        }
    }

    asIScriptContext *ctx = cmpContext;


    // TODO: Add proper error handling
    int r = ctx->Prepare(f);

    //ctx->SetArgDouble();
    r = ctx->Execute();

    double val = 0.0;
    if( r == asEXECUTION_FINISHED )
    {
        val =  ctx->GetReturnDouble();
    }


    if( cmpContext )
    {
        if( isNested )
        {
            asEContextState state = cmpContext->GetState();
            cmpContext->PopState();
            if( state == asEXECUTION_ABORTED )
                cmpContext->Abort();
        }
        else
            cmpContext->Release();
    }

    return new cTMap();
}

inline static cTMap * AS_MapFunction(CScriptArray * maps, asIScriptFunction * f)
{


    return new cTMap();
}

inline static MatrixTable* AS_MapFunctionT(MatrixTable * map, asIScriptFunction * f)
{


    return new MatrixTable();
}

/*inline static MatrixTable* AS_MapFunctionT(CScriptArray * maps, asIScriptFunction *f)
{


    return new MatrixTable();
}*/

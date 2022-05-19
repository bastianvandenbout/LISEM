
#include "geo/raster/map.h"
//#include "scriptarray.h"


cTMap * cTMap::Assign(cTMap * other)
{

    if(AS_writeonassign)
    {
        AS_writefunc(other,AS_FileName);
    }
    this->AS_writeonassign = false;
    this->AS_IsLDD = other->AS_IsLDD;
    this->data = other->data;
    this->AS_IsSingleValue = other->AS_IsSingleValue;
    this->m_Projection = other->m_Projection;
    this->m_BoundingBox = other->m_BoundingBox;


    return this;
}

cTMap *        cTMap::Assign            (float other)
{

    cTMap * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    if(this->data.nr_rows() > 0 && this->data.nr_cols() > 0)
    {
        for(int r = 0; r < this->data.nr_rows();r++)
        {
            for(int c = 0; c < this->data.nr_cols();c++)
            {
                this->data[r][c] = other;
            }
        }
    }else
    {
        this->AS_IsSingleValue = true;
        MaskedRaster<float> raster_data(1,1,0.0f,0.0f,1.0f);
        this->data = std::move(raster_data);
        for(int r = 0; r < this->data.nr_rows();r++)
        {
            for(int c = 0; c < this->data.nr_cols();c++)
            {
                this->data[r][c] = other;
            }
        }
    }


    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;

}


cTMap * cTMap::GetBand(int i)
{
    std::cout << "Get Band 1:" << AS_BandFilePath.toStdString() << " 2:" << AS_FileName.toStdString() << " " << AS_IsBandLoadable << " " <<  i << "  " << AS_TotalNBands <<  std::endl;


    if(AS_IsBandLoadable)
    {
        if(i == 0)
        {
            cTMap * ret = this->GetCopy();
            ret->AS_FileName = AS_FileName;
            ret->AS_BandFilePath = AS_BandFilePath;
            ret->AS_TotalNBands = AS_TotalNBands;
            ret->AS_IsBand = true;
            ret->AS_Bandn = i;
            ret->AS_writefunc = AS_writefunc;
            ret->AS_writeonassign = AS_writeonassign;
            ret->AS_writebandfunc = AS_writebandfunc;
            ret->AS_writefunclist = AS_writefunclist;
            ret->AS_readallfunc = AS_readallfunc;
            ret->AS_readfunc = AS_readfunc;
            ret->AS_readbandfunc = AS_readbandfunc;
            ret->AS_readbandabsfunc = AS_readbandabsfunc;
            ret->AS_IsBandLoadable = AS_IsBandLoadable;
            ret->AS_refcount = 1;

            ret->AS_writefunc = std::bind(ret->AS_writebandfunc,std::placeholders::_1,std::placeholders::_2,i);
            ret->AS_readfunc = std::bind(ret->AS_readbandfunc,std::placeholders::_1,i);

            return ret;
        }
        //Is this a file-related map
        //Does the original file have this many maps?

        if(AS_writeonassign)
        {
            if(!AS_BandFilePath.isEmpty() && AS_TotalNBands > i)
            {
                //load a particular band
                cTMap * ret = GetCopy();
                //if we have a saving map, continue that
                ret->AS_FileName = AS_FileName;
                ret->AS_BandFilePath = AS_BandFilePath;
                ret->AS_TotalNBands = AS_TotalNBands;
                ret->AS_IsBand = true;
                ret->AS_Bandn = i;
                ret->AS_writefunc = AS_writefunc;
                ret->AS_writeonassign = AS_writeonassign;
                ret->AS_writebandfunc = AS_writebandfunc;
                ret->AS_writefunclist = AS_writefunclist;
                ret->AS_readallfunc = AS_readallfunc;
                ret->AS_readfunc = AS_readfunc;
                ret->AS_readbandfunc = AS_readbandfunc;
                ret->AS_readbandabsfunc = AS_readbandabsfunc;
                ret->AS_IsBandLoadable = AS_IsBandLoadable;
                ret->AS_refcount = 1;


                ret->AS_writefunc = std::bind(ret->AS_writebandfunc,std::placeholders::_1,std::placeholders::_2,i);
                ret->AS_readfunc = std::bind(ret->AS_readbandfunc,std::placeholders::_1,i);

                return ret;

            }else {
                LISEMS_ERROR("Could not load band from file: " + AS_BandFilePath + " band n: " + QString::number(i));
                throw 1;
            }

        }else {
            if(!AS_BandFilePath.isEmpty() && AS_TotalNBands > i)
            {
                std::cout << "load band " << AS_readbandfunc.operator bool() << std::endl;
                //load a particular band
                cTMap * ret = AS_readbandabsfunc(AS_BandFilePath,i);
                //if we have a saving map, continue that
                ret->AS_FileName = AS_FileName;
                ret->AS_BandFilePath = AS_BandFilePath;
                ret->AS_TotalNBands = AS_TotalNBands;
                ret->AS_IsBand = true;
                ret->AS_Bandn = i;
                ret->AS_writefunc = AS_writefunc;
                ret->AS_writeonassign = AS_writeonassign;
                ret->AS_writebandfunc = AS_writebandfunc;
                ret->AS_writefunclist = AS_writefunclist;
                ret->AS_readallfunc = AS_readallfunc;
                ret->AS_readfunc = AS_readfunc;
                ret->AS_readbandfunc = AS_readbandfunc;
                ret->AS_readbandabsfunc = AS_readbandabsfunc;
                ret->AS_IsBandLoadable = AS_IsBandLoadable;
                ret->AS_refcount = 1;


                ret->AS_writefunc = std::bind(ret->AS_writebandfunc,std::placeholders::_1,std::placeholders::_2,i);
                ret->AS_readfunc = std::bind(ret->AS_readbandfunc,std::placeholders::_1,i);

                return ret;

            }else {
                LISEMS_ERROR("Could not load band from file: " + AS_BandFilePath + " band n: " + QString::number(i));
                throw 1;
            }
        }

    }else {

        LISEMS_ERROR("Could not load band from non-file Map");
        throw 1;
    }
}

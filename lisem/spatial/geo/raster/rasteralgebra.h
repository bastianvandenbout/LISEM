#ifndef RASTERALGEBRA_H
#define RASTERALGEBRA_H

#include "geo/raster/map.h"


static inline cTMap * ApplyOperatorToRasters(float map1, cTMap *map2, std::function<float(float,float)> f)
{
    if(map2 == nullptr)
    {
        LISEMS_ERROR("One of the input maps does not exist");
        throw 1;
    }

    MaskedRaster<float> raster_data(map2->data.nr_rows(), map2->data.nr_cols(), map2->data.north(), map2->data.west(), map2->data.cell_size(), map2->data.cell_sizeY());
    cTMap *nmap = new cTMap(std::move(raster_data),map2->projection(),"");

    std::cout << "create operator " << nmap << std::endl;
    #pragma omp parallel for collapse(2)
    for(int r = 0; r < map2->data.nr_rows();r++)
    {
        for(int c = 0; c < map2->data.nr_cols();c++)
        {
            if(pcr::isMV(map2->data[r][c]) )
            {
                pcr::setMV(nmap->data[r][c]);
            }else
            {
                nmap->data[r][c] = f(map1,map2->data[r][c]);
            }
        }
    }

    nmap->AS_IsSingleValue = map2->AS_IsSingleValue;
    return nmap;
}



static inline cTMap * ApplyOperatorToRasters(cTMap * map1, float map2, std::function<float(float,float)> f, bool assign_to_one = false)
{
    if(map1 == nullptr)
    {
        LISEMS_ERROR("One of the input maps does not exist");
        throw 1;
    }

    cTMap *nmap;

    if(!assign_to_one)
    {
        MaskedRaster<float> raster_data(map1->data.nr_rows(), map1->data.nr_cols(), map1->data.north(), map1->data.west(), map1->data.cell_size(), map1->data.cell_sizeY());
        nmap = new cTMap(std::move(raster_data),map1->projection(),"");

    }else {
        nmap = map1;
    }

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < map1->data.nr_rows();r++)
    {
        for(int c = 0; c < map1->data.nr_cols();c++)
        {
            if(pcr::isMV(map1->data[r][c]) )
            {
                pcr::setMV(nmap->data[r][c]);
            }else
            {
                nmap->data[r][c] = f(map1->data[r][c],map2);
            }
        }
    }

    nmap->AS_IsSingleValue = map1->AS_IsSingleValue;
    return nmap;
}


static inline cTMap * ApplyOperatorToRasters(cTMap * map1, cTMap * map2, std::function<float(float,float)> f, bool assign_to_one = false)
{
    if(map1 == nullptr || map2 == nullptr)
    {
        LISEMS_ERROR("One of the input maps does not exist");
        throw 1;
    }
    if(map2->AS_IsSingleValue)
    {
        return ApplyOperatorToRasters(map1, map2->data[0][0], f,assign_to_one);
    }
    if(map1->AS_IsSingleValue)
    {
        return ApplyOperatorToRasters(map1->data[0][0], map2, f);
    }
    if(map1->nrCols() != map2->nrCols() || map1->nrRows() != map2->nrRows())
    {
        LISEMS_ERROR("Could not do operation on rasters of different size");
        throw 1;
    }

    cTMap *nmap;

    if(!assign_to_one)
    {
        MaskedRaster<float> raster_data(map1->data.nr_rows(), map1->data.nr_cols(), map1->data.north(), map1->data.west(), map1->data.cell_size(), map1->data.cell_sizeY());
        nmap = new cTMap(std::move(raster_data),map1->projection(),"");

    }else {
        nmap = map1;
    }

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < map1->data.nr_rows();r++)
    {
        for(int c = 0; c < map1->data.nr_cols();c++)
        {
            if(pcr::isMV(map1->data[r][c]) )
            {
                pcr::setMV(nmap->data[r][c]);
            }else
            {
                nmap->data[r][c] = f(map1->data[r][c],map2->data[r][c]);
            }
        }
    }

    return nmap;
}



inline cTMap * cTMap::OpAdd(cTMap * other)
{

    return ApplyOperatorToRasters(this,other,std::plus<float>());

    /*if((this->AS_IsSingleValue && other->AS_IsSingleValue) ||(!(this->AS_IsSingleValue) && !(other->AS_IsSingleValue)))
    {
        if(!(this->data.nr_rows() == other->data.nr_rows() && this->data.nr_cols() == other->data.nr_cols()))
        {
            LISEMS_ERROR("Numbers of rows and column do not match");
            throw -1;
        }

        MaskedRaster<float> raster_data(this->data.nr_rows(), this->data.nr_cols(), this->data.north(), this->data.west(), this->data.cell_size(), this->data.cell_sizeY());
        cTMap *map = new cTMap(std::move(raster_data),this->projection(),"");

        for(int r = 0; r < this->data.nr_rows();r++)
        {
            for(int c = 0; c < this->data.nr_cols();c++)
            {
                if(pcr::isMV(this->data[r][c]) || pcr::isMV(other->data[r][c]))
                {
                    pcr::setMV(map->data[r][c]);
                }else
                {
                    map->data[r][c] = this->data[r][c] + other->data[r][c];
                }
            }
        }

        map->AS_IsSingleValue = (this->AS_IsSingleValue && other->AS_IsSingleValue);
        return map;


    }else
    {

        cTMap * map1 = this;
        cTMap * map2 = other;

        if(map1->AS_IsSingleValue)
        {
            cTMap * temp = map2;
            map2 = map1;
            map1 = temp;
        }
        MaskedRaster<float> raster_data(map1->data.nr_rows(), map1->data.nr_cols(), map1->data.north(), map1->data.west(), map1->data.cell_size(), map1->data.cell_sizeY());
        cTMap *nmap = new cTMap(std::move(raster_data),map1->projection(),"");

        float vother = map2->data[0][0];

        cTMap * target = nmap;

        for(int r = 0; r < map1->data.nr_rows();r++)
        {
            for(int c = 0; c < map1->data.nr_cols();c++)
            {
                if(pcr::isMV(map1->data[r][c]) )
                {
                    pcr::setMV(target->data[r][c]);
                }else
                {
                    target->data[r][c] = map1->data[r][c] + vother;
                }
            }
        }
        target->AS_IsSingleValue = false;
        return target;
    }*/


}

inline cTMap * cTMap::OpMul(cTMap * other)
{
    return ApplyOperatorToRasters(this,other,std::multiplies<float>());
}


inline cTMap *        cTMap::OpNeg             ()
{
    return ApplyOperatorToRasters(this,-1,std::multiplies<float>());
}

inline cTMap *        cTMap::OpSub             (cTMap *other)
{
    return ApplyOperatorToRasters(this,other,std::minus<float>());

}
inline cTMap *        cTMap::OpDiv             (cTMap *other)
{
    return ApplyOperatorToRasters(this,other,std::divides<float>());
}
inline cTMap *        cTMap::OpPow             (cTMap *other)
{
    std::function<float(float,float)> powfunc = [](float a, float b){return std::pow(a,b);};
    return ApplyOperatorToRasters(this,other,powfunc);
}

inline cTMap *        cTMap::OpAddAssign       (cTMap *other)
{
    cTMap * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    ApplyOperatorToRasters(target,other,std::plus<float>(),true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;
}
inline cTMap *        cTMap::OpSubAssign       (cTMap *other)
{
    cTMap * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    ApplyOperatorToRasters(target,other,std::minus<float>(),true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;
}
inline cTMap *        cTMap::OpMulAssign       (cTMap *other)
{
    cTMap * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    ApplyOperatorToRasters(target,other,std::multiplies<float>(),true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;
}
inline cTMap *        cTMap::OpDivAssign       (cTMap *other)
{
    cTMap * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    ApplyOperatorToRasters(target,other,std::divides<float>(),true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;
}
inline cTMap *        cTMap::OpPowAssign       (cTMap *other)
{
    cTMap * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<double(float,float)> powfunc = [](float a, float b){return std::pow(a,b);};
    ApplyOperatorToRasters(target,other,powfunc,true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;

}

inline cTMap *        cTMap::OpAdd             (float other)
{

    return ApplyOperatorToRasters(this,other,std::plus<float>());

}
inline cTMap *        cTMap::OpMul             (float other)
{

    return ApplyOperatorToRasters(this,other,std::multiplies<float>());
}

inline cTMap *        cTMap::OpSub             (float other)
{

    return ApplyOperatorToRasters(this,other,std::minus<float>());

}
inline cTMap *        cTMap::OpDiv             (float other)
{
    return ApplyOperatorToRasters(this,other,std::divides<float>());
}
inline cTMap *        cTMap::OpPow             (float other)
{
   std::function<float(float,float)> powfunc = [](float a, float b){return std::pow(a,b);};
   return ApplyOperatorToRasters(this,other,powfunc);
}

inline cTMap *        cTMap::OpAddAssign       (float other)
{
    cTMap * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    ApplyOperatorToRasters(target,other,std::plus<float>(),true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;

}
inline cTMap *        cTMap::OpSubAssign       (float other)
{
    cTMap * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    ApplyOperatorToRasters(target,other,std::minus<float>(),true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;

}
inline cTMap *        cTMap::OpMulAssign       (float other)
{
    cTMap * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    ApplyOperatorToRasters(target,other,std::multiplies<float>(),true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;

}
inline cTMap *        cTMap::OpDivAssign       (float other)
{
    cTMap * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    ApplyOperatorToRasters(target,other,std::divides<float>(),true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;

}
inline cTMap *        cTMap::OpPowAssign       (float other)
{
    cTMap * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<float(float,float)> powfunc = [](float a, float b){return std::pow(a,b);};
    ApplyOperatorToRasters(target,other,powfunc,true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;

}




#include <QtGlobal>


inline cTMap *        cTMap::EqualTo           (cTMap *other)
{
    std::function<float(float,float)> eqfunc = [](float a, float b){return (qFuzzyCompare(a,b)? 1.0f: 0.0f);};
    return ApplyOperatorToRasters(this,other,eqfunc);
}
inline cTMap *        cTMap::LargerThen        (cTMap *other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a > b)? 1.0f: 0.0f;};
    return ApplyOperatorToRasters(this,other,compfunc);
}
inline cTMap *        cTMap::SmallerThen       (cTMap *other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a < b)? 1.0f: 0.0f;};
    return ApplyOperatorToRasters(this,other,compfunc);
}
inline cTMap *        cTMap::NotEqualTo        (cTMap *other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a != b)? 1.0f: 0.0f;};
    return ApplyOperatorToRasters(this,other,compfunc);
}
inline cTMap *        cTMap::LargerEqualThen   (cTMap *other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a >= b)? 1.0f: 0.0f;};
    return ApplyOperatorToRasters(this,other,compfunc);
}
inline cTMap *        cTMap::SmallerEqualThen  (cTMap *other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a <= b)? 1.0f: 0.0f;};
    return ApplyOperatorToRasters(this,other,compfunc);
}
inline cTMap *        cTMap::EqualTo           (float other)
{

    std::function<float(float,float)> compfunc = [](float a, float b){return (qFuzzyCompare(a,b)? 1.0f: 0.0f);};
    return ApplyOperatorToRasters(this,other,compfunc);
}
inline cTMap *        cTMap::LargerThen        (float other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a > b)? 1.0f: 0.0f;};
    return ApplyOperatorToRasters(this,other,compfunc);

}
inline cTMap *        cTMap::SmallerThen       (float other)
{

    std::function<float(float,float)> compfunc = [](float a, float b){return (a < b)? 1.0f: 0.0f;};
    return ApplyOperatorToRasters(this,other,compfunc);
}
inline cTMap *        cTMap::NotEqualTo        (float other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a != b)? 1.0f: 0.0f;};
    return ApplyOperatorToRasters(this,other,compfunc);
}
inline cTMap *        cTMap::LargerEqualThen   (float other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a >= b)? 1.0f: 0.0f;};
    return ApplyOperatorToRasters(this,other,compfunc);
}
inline cTMap *        cTMap::SmallerEqualThen  (float other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a <= b)? 1.0f: 0.0f;};
    return ApplyOperatorToRasters(this,other,compfunc);
}



inline cTMap *        cTMap::OpModAssign       (cTMap *other)
{
    cTMap * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<float(float,float)> powfunc = [](float a, float b){return std::fmod(a,b);};
    ApplyOperatorToRasters(target,other,powfunc,true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;

}
inline cTMap *        cTMap::OpModAssign       (float other)
{
    cTMap * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<float(float,float)> powfunc = [](float a, float b){return std::fmod(a,b);};
    ApplyOperatorToRasters(target,other,powfunc,true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;


}
inline cTMap *        cTMap::OpMod             (cTMap *other)
{
    std::function<float(float,float)> func = [](float a, float b){return std::fmod(a,b);};
    return ApplyOperatorToRasters(this,other,func);

}
inline cTMap *        cTMap::OpMod             (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return std::fmod(a,b);};
    return ApplyOperatorToRasters(this,other,func);
}

inline cTMap *        cTMap::And               (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) && LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToRasters(this,other,func);
}
inline cTMap *        cTMap::Or                (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) || LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToRasters(this,other,func);
}
inline cTMap *        cTMap::Xor               (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) ^ LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToRasters(this,other,func);
}

inline cTMap *        cTMap::And               (cTMap * other)
{
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) && LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToRasters(this,other,func);
}
inline cTMap *        cTMap::Or                (cTMap * other)
{
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) || LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToRasters(this,other,func);

}
inline cTMap *        cTMap::Xor               (cTMap * other)
{
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) ^ LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToRasters(this,other,func);
}


inline cTMap *        cTMap::AndAssign              (float other)
{
    cTMap * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) && LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToRasters(target,other,func);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;
}
inline cTMap *        cTMap::OrAssign                (float other)
{
    cTMap * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) || LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToRasters(target,other,func);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;

}
inline cTMap *        cTMap::XorAssign               (float other)
{
    cTMap * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }


    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) ^ LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToRasters(target,other,func);


    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;
}

inline cTMap *        cTMap::AndAssign               (cTMap * other)
{
    cTMap * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) && LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToRasters(target,other,func);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;





}
inline cTMap *        cTMap::OrAssign                (cTMap * other)
{
    cTMap * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) || LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToRasters(target,other,func);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;

}
inline cTMap *        cTMap::XorAssign               (cTMap * other)
{
    cTMap * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) ^ LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToRasters(target,other,func);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;
}


inline cTMap *        cTMap::Negate            ()
{
    std::function<float(float,float)> func = [](float a, float b){return (!(LISEM_ASMAP_BOOLFROMFLOAT(a)) )? 1.0f:0.0f;};
    return ApplyOperatorToRasters(this,1,func);
}


inline cTMap *        cTMap::OpMod_r           (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return std::fmod(b,a);};
    return ApplyOperatorToRasters(this,other,func);
}

inline cTMap *        cTMap::OpAdd_r           (float other)
{
    return ApplyOperatorToRasters(this,other,std::plus<float>());
}
inline cTMap *        cTMap::OpMul_r           (float other)
{
    return ApplyOperatorToRasters(this,other,std::multiplies<float>());
}
inline cTMap *        cTMap::OpSub_r           (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return b-a;};
    return ApplyOperatorToRasters(this,other,func);
}
inline cTMap *        cTMap::OpDiv_r           (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return b/a;};
    return ApplyOperatorToRasters(this,other,func);
}
inline cTMap *        cTMap::OpPow_r           (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return std::pow(b,a);};
    return ApplyOperatorToRasters(this,other,func);
}
inline cTMap *        cTMap::LargerThen_r      (float other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (b > a)? 1.0f: 0.0f;};
    return ApplyOperatorToRasters(this,other,compfunc);
}
inline cTMap *        cTMap::SmallerThen_r     (float other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (b < a)? 1.0f: 0.0f;};
    return ApplyOperatorToRasters(this,other,compfunc);
}
inline cTMap *        cTMap::NotEqualTo_r      (float other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a != b)? 1.0f: 0.0f;};
    return ApplyOperatorToRasters(this,other,compfunc);
}
inline cTMap *        cTMap::LargerEqualThen_r (float other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a >= b)? 1.0f: 0.0f;};
    return ApplyOperatorToRasters(this,other,compfunc);
}
inline cTMap *        cTMap::SmallerEqualThen_r(float other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a <= b)? 1.0f: 0.0f;};
    return ApplyOperatorToRasters(this,other,compfunc);
}
inline cTMap *        cTMap::EqualTo_r         (float other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a == b)? 1.0f: 0.0f;};
    return ApplyOperatorToRasters(this,other,compfunc);
}
inline cTMap *        cTMap::And_r             (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(b) && LISEM_ASMAP_BOOLFROMFLOAT(a))? 1.0f:0.0f;};
    return ApplyOperatorToRasters(this,other,func);
}
inline cTMap *        cTMap::Or_r              (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(b) || LISEM_ASMAP_BOOLFROMFLOAT(a))? 1.0f:0.0f;};
    return ApplyOperatorToRasters(this,other,func);
}
inline cTMap *        cTMap::Xor_r             (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(b) ^ LISEM_ASMAP_BOOLFROMFLOAT(a))? 1.0f:0.0f;};
    return ApplyOperatorToRasters(this,other,func);
}



#endif // RASTERALGEBRA_H

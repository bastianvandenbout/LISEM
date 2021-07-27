#ifndef FIELDALGEBRA_H
#define FIELDALGEBRA_H

#include "field.h"
#include "map.h"
#include "rasteralgebra.h"

static inline Field * ApplySingleFunctionToField(Field* map1, std::function<float(float)> f)
{
    if(map1 == nullptr)
    {
        LISEMS_ERROR("One of the input fields does not exist");
        throw 1;
    }


    //check dataset dimensions thoroughly

    //create output field


    Field *nmap;
    nmap = map1->GetCopy();


    //do actual calculation loop
    for(int i = 0; i < map1->nrLevels(); i++)
    {

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < map1->at(i)->data.nr_rows();r++)
        {
            for(int c = 0; c < map1->at(i)->data.nr_cols();c++)
            {
                if(pcr::isMV(map1->at(i)->data[r][c]) )
                {
                    pcr::setMV(nmap->at(i)->data[r][c]);
                }else
                {
                    nmap->at(i)->data[r][c] = f(map1->at(i)->data[r][c]);
                }
            }
        }
    }

    //return
    return nmap;
}



static inline Field * ApplyOperatorToFields(float map2, Field *map1, std::function<float(float,float)> f)
{
    if(map1 == nullptr)
    {
        LISEMS_ERROR("One of the input fields does not exist");
        throw 1;
    }


    //check dataset dimensions thoroughly

    //create output field


    Field *nmap;

    nmap = map1->GetCopy();

    //do actual calculation loop
    for(int i = 0; i < map1->nrLevels(); i++)
    {

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < map1->at(i)->data.nr_rows();r++)
        {
            for(int c = 0; c < map1->at(i)->data.nr_cols();c++)
            {
                if(pcr::isMV(map1->at(i)->data[r][c]) )
                {
                    pcr::setMV(nmap->at(i)->data[r][c]);
                }else
                {
                    nmap->at(i)->data[r][c] = f(map2,map1->at(i)->data[r][c]);
                }
            }
        }
    }


    //return
    return nmap;
}



static inline Field * ApplyOperatorToFields(Field * map1, float map2, std::function<float(float,float)> f, bool assign_to_one = false)
{
    if(map1 == nullptr)
    {
        LISEMS_ERROR("One of the input fields does not exist");
        throw 1;
    }


    //check dataset dimensions thoroughly

    //create output field


    Field *nmap;
    if(!assign_to_one)
    {
        nmap = map1->GetCopy();

    }else {
        nmap = map1;
    }

    //do actual calculation loop
    for(int i = 0; i < map1->nrLevels(); i++)
    {

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < map1->at(i)->data.nr_rows();r++)
        {
            for(int c = 0; c < map1->at(i)->data.nr_cols();c++)
            {
                if(pcr::isMV(map1->at(i)->data[r][c]) )
                {
                    pcr::setMV(nmap->at(i)->data[r][c]);
                }else
                {
                    nmap->at(i)->data[r][c] = f(map1->at(i)->data[r][c],map2);
                }
            }
        }
    }


    //return
    return nmap;
}


static inline Field * ApplyOperatorToFields(Field * map1, Field * map2, std::function<float(float,float)> f, bool assign_to_one = false)
{
    if(map1 == nullptr || map2 == nullptr)
    {
        LISEMS_ERROR("One of the input fields does not exist");
        throw 1;
    }


    //check dataset dimensions thoroughly

    if(map1->nrLevels() != map2->nrLevels())
    {
        LISEMS_ERROR("Input fields of different dimensions");
        throw 1;
    }


    for(int i = 0; i < map1->nrLevels(); i++)
    {
        if(map1->at(i)->nrCols() != map2->at(i)->nrCols())
        {
            LISEMS_ERROR("Input fields of different dimensions");
            throw 1;
        }
        if(map1->at(i)->nrRows() != map2->at(i)->nrRows())
        {
            LISEMS_ERROR("Input fields of different dimensions");
            throw 1;
        }

    }

    //create output field


    Field *nmap;
    if(!assign_to_one)
    {
        nmap = map1->GetCopy();

    }else {
        nmap = map1;
    }

    //do actual calculation loop
    for(int i = 0; i < map1->nrLevels(); i++)
    {

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < map1->at(i)->data.nr_rows();r++)
        {
            for(int c = 0; c < map1->at(i)->data.nr_cols();c++)
            {
                if(pcr::isMV(map1->at(i)->data[r][c]) )
                {
                    pcr::setMV(nmap->at(i)->data[r][c]);
                }else
                {
                    nmap->at(i)->data[r][c] = f(map1->at(i)->data[r][c],map2->at(i)->data[r][c]);
                }
            }
        }
    }


    //return
    return nmap;
}



inline Field * Field::OpAdd(Field * other)
{

    return ApplyOperatorToFields(this,other,std::plus<float>());

}

inline Field * Field::OpMul(Field * other)
{
    return ApplyOperatorToFields(this,other,std::multiplies<float>());
}


inline Field *        Field::OpNeg             ()
{
    return ApplyOperatorToFields(this,-1,std::multiplies<float>());
}

inline Field *        Field::OpSub             (Field *other)
{
    return ApplyOperatorToFields(this,other,std::minus<float>());

}
inline Field *        Field::OpDiv             (Field *other)
{
    return ApplyOperatorToFields(this,other,std::divides<float>());
}
inline Field *        Field::OpPow             (Field *other)
{
    std::function<float(float,float)> powfunc = [](float a, float b){return std::pow(a,b);};
    return ApplyOperatorToFields(this,other,powfunc);
}

inline Field *        Field::OpAddAssign       (Field *other)
{
    Field * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    ApplyOperatorToFields(target,other,std::plus<float>(),true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;
}
inline Field *        Field::OpSubAssign       (Field *other)
{
    Field * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    ApplyOperatorToFields(target,other,std::minus<float>(),true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;
}
inline Field *        Field::OpMulAssign       (Field *other)
{
    Field * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    ApplyOperatorToFields(target,other,std::multiplies<float>(),true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;
}
inline Field *        Field::OpDivAssign       (Field *other)
{
    Field * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    ApplyOperatorToFields(target,other,std::divides<float>(),true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;
}
inline Field *        Field::OpPowAssign       (Field *other)
{
    Field * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<double(float,float)> powfunc = [](float a, float b){return std::pow(a,b);};
    ApplyOperatorToFields(target,other,powfunc,true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;

}

inline Field *        Field::OpAdd             (float other)
{

    return ApplyOperatorToFields(this,other,std::plus<float>());

}
inline Field *        Field::OpMul             (float other)
{

    return ApplyOperatorToFields(this,other,std::multiplies<float>());
}

inline Field *        Field::OpSub             (float other)
{

    return ApplyOperatorToFields(this,other,std::minus<float>());

}
inline Field *        Field::OpDiv             (float other)
{
    return ApplyOperatorToFields(this,other,std::divides<float>());
}
inline Field *        Field::OpPow             (float other)
{
   std::function<float(float,float)> powfunc = [](float a, float b){return std::pow(a,b);};
   return ApplyOperatorToFields(this,other,powfunc);
}

inline Field *        Field::OpAddAssign       (float other)
{
    Field * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    ApplyOperatorToFields(target,other,std::plus<float>(),true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;

}
inline Field *        Field::OpSubAssign       (float other)
{
    Field * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    ApplyOperatorToFields(target,other,std::minus<float>(),true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;

}
inline Field *        Field::OpMulAssign       (float other)
{
    Field * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    ApplyOperatorToFields(target,other,std::multiplies<float>(),true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;

}
inline Field *        Field::OpDivAssign       (float other)
{
    Field * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    ApplyOperatorToFields(target,other,std::divides<float>(),true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;

}
inline Field *        Field::OpPowAssign       (float other)
{
    Field * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<float(float,float)> powfunc = [](float a, float b){return std::pow(a,b);};
    ApplyOperatorToFields(target,other,powfunc,true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;

}




#include <QtGlobal>


inline Field *        Field::EqualTo           (Field *other)
{
    std::function<float(float,float)> eqfunc = [](float a, float b){return (qFuzzyCompare(a,b)? 1.0f: 0.0f);};
    return ApplyOperatorToFields(this,other,eqfunc);
}
inline Field *        Field::LargerThen        (Field *other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a > b)? 1.0f: 0.0f;};
    return ApplyOperatorToFields(this,other,compfunc);
}
inline Field *        Field::SmallerThen       (Field *other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a < b)? 1.0f: 0.0f;};
    return ApplyOperatorToFields(this,other,compfunc);
}
inline Field *        Field::NotEqualTo        (Field *other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a != b)? 1.0f: 0.0f;};
    return ApplyOperatorToFields(this,other,compfunc);
}
inline Field *        Field::LargerEqualThen   (Field *other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a >= b)? 1.0f: 0.0f;};
    return ApplyOperatorToFields(this,other,compfunc);
}
inline Field *        Field::SmallerEqualThen  (Field *other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a <= b)? 1.0f: 0.0f;};
    return ApplyOperatorToFields(this,other,compfunc);
}
inline Field *        Field::EqualTo           (float other)
{

    std::function<float(float,float)> compfunc = [](float a, float b){return (qFuzzyCompare(a,b)? 1.0f: 0.0f);};
    return ApplyOperatorToFields(this,other,compfunc);
}
inline Field *        Field::LargerThen        (float other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a > b)? 1.0f: 0.0f;};
    return ApplyOperatorToFields(this,other,compfunc);

}
inline Field *        Field::SmallerThen       (float other)
{

    std::function<float(float,float)> compfunc = [](float a, float b){return (a < b)? 1.0f: 0.0f;};
    return ApplyOperatorToFields(this,other,compfunc);
}
inline Field *        Field::NotEqualTo        (float other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a != b)? 1.0f: 0.0f;};
    return ApplyOperatorToFields(this,other,compfunc);
}
inline Field *        Field::LargerEqualThen   (float other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a >= b)? 1.0f: 0.0f;};
    return ApplyOperatorToFields(this,other,compfunc);
}
inline Field *        Field::SmallerEqualThen  (float other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a <= b)? 1.0f: 0.0f;};
    return ApplyOperatorToFields(this,other,compfunc);
}



inline Field *        Field::OpModAssign       (Field *other)
{
    Field * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<float(float,float)> powfunc = [](float a, float b){return std::fmod(a,b);};
    ApplyOperatorToFields(target,other,powfunc,true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;

}
inline Field *        Field::OpModAssign       (float other)
{
    Field * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<float(float,float)> powfunc = [](float a, float b){return std::fmod(a,b);};
    ApplyOperatorToFields(target,other,powfunc,true);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;


}
inline Field *        Field::OpMod             (Field *other)
{
    std::function<float(float,float)> func = [](float a, float b){return std::fmod(a,b);};
    return ApplyOperatorToFields(this,other,func);

}
inline Field *        Field::OpMod             (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return std::fmod(a,b);};
    return ApplyOperatorToFields(this,other,func);
}

inline Field *        Field::And               (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) && LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToFields(this,other,func);
}
inline Field *        Field::Or                (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) || LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToFields(this,other,func);
}
inline Field *        Field::Xor               (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) ^ LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToFields(this,other,func);
}

inline Field *        Field::And               (Field * other)
{
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) && LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToFields(this,other,func);
}
inline Field *        Field::Or                (Field * other)
{
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) || LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToFields(this,other,func);

}
inline Field *        Field::Xor               (Field * other)
{
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) ^ LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToFields(this,other,func);
}


inline Field *        Field::AndAssign              (float other)
{
    Field * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) && LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToFields(target,other,func);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;
}
inline Field *        Field::OrAssign                (float other)
{
    Field * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) || LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToFields(target,other,func);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;

}
inline Field *        Field::XorAssign               (float other)
{
    Field * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }


    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) ^ LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToFields(target,other,func);


    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;
}

inline Field *        Field::AndAssign               (Field * other)
{
    Field * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }

    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) && LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToFields(target,other,func);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;





}
inline Field *        Field::OrAssign                (Field * other)
{
    Field * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) || LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToFields(target,other,func);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;

}
inline Field *        Field::XorAssign               (Field * other)
{
    Field * target = this;
    if(AS_writeonassign)
    {
        target = AS_readfunc(AS_FileName);
    }
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(a) ^ LISEM_ASMAP_BOOLFROMFLOAT(b))? 1.0f:0.0f;};
    return ApplyOperatorToFields(target,other,func);

    if(AS_writeonassign)
    {
        AS_writefunc(target,AS_FileName);
    }
    this->AS_writeonassign = false;

    return target;
}


inline Field *        Field::Negate            ()
{
    std::function<float(float,float)> func = [](float a, float b){return (!(LISEM_ASMAP_BOOLFROMFLOAT(a)) )? 1.0f:0.0f;};
    return ApplyOperatorToFields(this,1,func);
}


inline Field *        Field::OpMod_r           (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return std::fmod(b,a);};
    return ApplyOperatorToFields(this,other,func);
}

inline Field *        Field::OpAdd_r           (float other)
{
    return ApplyOperatorToFields(this,other,std::plus<float>());
}
inline Field *        Field::OpMul_r           (float other)
{
    return ApplyOperatorToFields(this,other,std::multiplies<float>());
}
inline Field *        Field::OpSub_r           (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return b-a;};
    return ApplyOperatorToFields(this,other,func);
}
inline Field *        Field::OpDiv_r           (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return b/a;};
    return ApplyOperatorToFields(this,other,func);
}
inline Field *        Field::OpPow_r           (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return std::pow(b,a);};
    return ApplyOperatorToFields(this,other,func);
}
inline Field *        Field::LargerThen_r      (float other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (b > a)? 1.0f: 0.0f;};
    return ApplyOperatorToFields(this,other,compfunc);
}
inline Field *        Field::SmallerThen_r     (float other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (b < a)? 1.0f: 0.0f;};
    return ApplyOperatorToFields(this,other,compfunc);
}
inline Field *        Field::NotEqualTo_r      (float other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a != b)? 1.0f: 0.0f;};
    return ApplyOperatorToFields(this,other,compfunc);
}
inline Field *        Field::LargerEqualThen_r (float other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a >= b)? 1.0f: 0.0f;};
    return ApplyOperatorToFields(this,other,compfunc);
}
inline Field *        Field::SmallerEqualThen_r(float other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a <= b)? 1.0f: 0.0f;};
    return ApplyOperatorToFields(this,other,compfunc);
}
inline Field *        Field::EqualTo_r         (float other)
{
    std::function<float(float,float)> compfunc = [](float a, float b){return (a == b)? 1.0f: 0.0f;};
    return ApplyOperatorToFields(this,other,compfunc);
}
inline Field *        Field::And_r             (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(b) && LISEM_ASMAP_BOOLFROMFLOAT(a))? 1.0f:0.0f;};
    return ApplyOperatorToFields(this,other,func);
}
inline Field *        Field::Or_r              (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(b) || LISEM_ASMAP_BOOLFROMFLOAT(a))? 1.0f:0.0f;};
    return ApplyOperatorToFields(this,other,func);
}
inline Field *        Field::Xor_r             (float other)
{
    std::function<float(float,float)> func = [](float a, float b){return (LISEM_ASMAP_BOOLFROMFLOAT(b) ^ LISEM_ASMAP_BOOLFROMFLOAT(a))? 1.0f:0.0f;};
    return ApplyOperatorToFields(this,other,func);
}

#endif

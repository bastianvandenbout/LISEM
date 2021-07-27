#ifndef SCRIPTARRAYHELPERS_H
#define SCRIPTARRAYHELPERS_H


////////////////////////////////////////
/*
We want to be able to re-use the c++ API in angelscript and python.
However, the c++ functions often take or return std::vector objects.
In the case of python, pybind provides funtionality for this, and std::vector is automatically converted to python list

However, in angelscript, the Array class (provided by the CScriptArray c++ class) does not map directly to std::vector.
We need some mechanism to do automated conversion of input and output.

The following variadic template wizardry fixes this problem.

normally calling the registration goes like:

std::vector<cTMap*> AS_Func(std::vector<cTMap *>, std::vector<QString);
engine->RegisterGlobalFunction("array<Map> @funcname(const array<Map> &in A, const array<String> &in B)", asFUNCTIONPR( func,(CScriptArray*, CScriptArray*),CScriptArray*),  asCALL_CDECL);

Angelscript ensures the right type of array object is being passed.
However, this gives a compilation error, as the arguments of AS_Func are not of type CScriptArray.

Thus, we make a generic template function that calls the function AS_Func, and type-selectively applies a conversion to the arguments.
Now, registration becomes


engine->RegisterGlobalFunction("array<Map> @funcname(const array<Map> &in A, const array<String> &in B)", asFUNCTIONPR( func,(CScriptArray*, CScriptArray*),CScriptArray*),  asCALL_CDECL);
*/
////////////////////////////////////////

#include <iostream>
#include <type_traits>
#include <typeinfo>
#include "scriptarray.h"
#include "functional"
#include "scriptmanager.h"
#include "lsmscriptengine.h"
#include "error.h"
#include "geo/raster/map.h"
#include "linear/lsm_vector2.h"


//type conversion function
template <typename From, typename tot>
tot asbind_convert(From x)
{
    tot y= x;
    std::cout <<" convert"<<std::endl;
    return y;
}

//type conversion function for lists of float
template <>
std::vector<float> asbind_convert<CScriptArray*, std::vector<float>>(CScriptArray* x)
{
    std::vector<float> ret = x->ToSTDList<float>();
    return ret;
}

//type conversion function for lists of float
template <>
CScriptArray * asbind_convert< std::vector<float>,CScriptArray*>(std::vector<float> x)
{
    // Obtain a pointer to the engine
    asIScriptContext *ctx = asGetActiveContext();
    if(ctx != nullptr)
    {
        asIScriptEngine *engine = ctx->GetEngine();
        asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<float>");

        // Create the array object
        CScriptArray *array = CScriptArray::Create(arrayType);

        array->Resize(x.size());

        for(int i = 0; i < x.size(); ++i)
        {
            array->SetValue(i,(void*)(new double(x.at(i))),false);
        }

        return array;

    }
    LISEMS_ERROR("Could not convert list to std::vector");
    throw 1;
}

//type conversion function for lists of pointers without any special needs
template <>
std::vector<cTMap*> asbind_convert<CScriptArray*, std::vector<cTMap*>>(CScriptArray* x)
{
    std::vector<cTMap*> ret = x->ToSTDList<cTMap*>();
    return ret;

}

template <>
CScriptArray* asbind_convert< std::vector<cTMap*>,CScriptArray*>(std::vector<cTMap*> x)
{
    // Obtain a pointer to the engine
    asIScriptContext *ctx = asGetActiveContext();
    asIScriptEngine *engine = ctx->GetEngine();

    // TODO: This should only be done once
    // TODO: This assumes that CScriptArray was already registered
    asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<Map>");

    // Create the array object
    CScriptArray *array = CScriptArray::Create(arrayType);
    array->Resize(x.size());
    for(int i = 0; i < x.size(); i++)
    {
        array->SetValue(i,x.at(i),false);
    }

    return array;
}




//type conversion function for lists of pointers without any special needs
//not allowed as c++ doesnt yet allow partial function template specialization
/*template <typename A>
std::vector<A*> asbind_convert<CScriptArray*, std::vector<A*>>(CScriptArray* x)
{
    std::vector<cTMap*> ret = x->ToSTDList<A*>();
    return ret;

}*/

//type conversion function for lists of pointers without any special needs
//we can not do this withou a type name as defined in the scripting language
/*template <typename A>
CScriptArray* asbind_convert< std::vector<A*>,CScriptArray*>(std::vector<A> x)
{
    // Obtain a pointer to the engine
    asIScriptContext *ctx = asGetActiveContext();
    if(ctx != nullptr)
    {
        asIScriptEngine *engine = ctx->GetEngine();
        asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<float>");

        // Create the array object
        CScriptArray *array = CScriptArray::Create(arrayType);

        array->Resize(x.size());

        for(int i = 0; i < x.size(); ++i)
        {
            array->SetValue(i,(void*)(new double(x.at(i))),false);
        }

        return array;

    }
    LISEMS_ERROR("Could not convert list to std::vector");
    throw 1;
}*/


//type conversion function for lists of pointers without any special needs
//not allowed, as we can not yet make partial function template specialization
/*template <typename A>
std::vector<std::vector<A>> asbind_convert<CScriptArray*, std::vector<std::vector<A>>>(CScriptArray* x)
{
    std::vector<CScriptArray*> retmp = x->ToSTDList<CScriptArray*>();
    std::vector<std::vector<A> ret;
    for(int i = 0; i < retmp.size(); i++)
    {
        ret.push_back(asbind_convert<CScriptArray*,std::vector<A>>(retmp.at(i)));
    }
    return ret;
}*/

//we can not do this one (not even the outer recursive step) as we need to know the full angelscript type of the object
/*CScriptArray* asbind_convert<std::vector<std::vector<A>>,CScriptArray*>(std::vector<std::vector<A>> x)
{





}*/



template<typename From, typename tot>
class asbindc_convert
{

public:

    inline static tot asbind_convert(From x)
    {
        tot y= x;
        std::cout <<" convert"<<std::endl;
        return y;
    }


};

template<>
class asbindc_convert<CScriptArray*, std::vector<float>>
{

public:

    inline static std::vector<float> asbind_convert(CScriptArray* x)
    {
        std::vector<float> ret = x->ToSTDList<float>();
        return ret;
    }


};


template<>
class asbindc_convert<CScriptArray*, std::vector<double>>
{

public:

    inline static std::vector<double> asbind_convert(CScriptArray* x)
    {
        std::vector<double> ret = x->ToSTDList<double>();
        return ret;
    }


};


template<>
class asbindc_convert<CScriptArray*, std::vector<int>>
{

public:

    inline static std::vector<int> asbind_convert(CScriptArray* x)
    {
        std::vector<int> ret = x->ToSTDList<int>();
        return ret;
    }


};



template<>
class asbindc_convert<CScriptArray*, std::vector<LSMVector2>>
{

public:

    inline static std::vector<LSMVector2> asbind_convert(CScriptArray* x)
    {
        std::vector<LSMVector2> ret = x->ToSTDList<LSMVector2>();
        return ret;
    }


};

template<>
class asbindc_convert<CScriptArray*, std::vector<LSMVector3>>
{

public:

    inline static std::vector<LSMVector3> asbind_convert(CScriptArray* x)
    {
        std::vector<LSMVector3> ret = x->ToSTDList<LSMVector3>();
        return ret;
    }


};





template<>
class asbindc_convert<CScriptArray*, std::vector<QString>>
{

public:

    inline static std::vector<QString> asbind_convert(CScriptArray* x)
    {
        std::vector<QString> ret = x->ToSTDList<QString>();
        return ret;
    }


};

template<>
class asbindc_convert<std::vector<QString>,CScriptArray*>
{

public:

    inline static CScriptArray * asbind_convert(std::vector<QString> x)
    {
        std::cout << "convert output qstringlist " << x.size() << std::endl;
        // Obtain a pointer to the engine
        asIScriptContext *ctx = asGetActiveContext();
        if(ctx != nullptr)
        {
            asIScriptEngine *engine = ctx->GetEngine();
            asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<string>");

            // Create the array object
            CScriptArray *array = CScriptArray::Create(arrayType);

            array->Resize(x.size());

            for(int i = 0; i < x.size(); ++i)
            {
                array->SetValue(i,(void*)(new QString(x.at(i))),false);
            }

            std::cout << "return list "<< array->GetSize() <<  std::endl;
            return array;

        }
        LISEMS_ERROR("Could not convert list to std::vector");
        throw 1;
    }

};

template<>
class asbindc_convert<std::vector<int>,CScriptArray*>
{

public:

    inline static CScriptArray * asbind_convert(std::vector<int> x)
    {
        // Obtain a pointer to the engine
        asIScriptContext *ctx = asGetActiveContext();
        if(ctx != nullptr)
        {
            asIScriptEngine *engine = ctx->GetEngine();
            asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<int>");

            // Create the array object
            CScriptArray *array = CScriptArray::Create(arrayType);

            array->Resize(x.size());

            for(int i = 0; i < x.size(); ++i)
            {
                array->SetValue(i,(void*)(new int(x.at(i))),false);
            }

            return array;

        }
        LISEMS_ERROR("Could not convert list to std::vector");
        throw 1;
    }

};

template<>
class asbindc_convert<std::vector<float>,CScriptArray*>
{

public:

    inline static CScriptArray * asbind_convert(std::vector<float> x)
    {
        // Obtain a pointer to the engine
        asIScriptContext *ctx = asGetActiveContext();
        if(ctx != nullptr)
        {
            asIScriptEngine *engine = ctx->GetEngine();
            asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<float>");

            // Create the array object
            CScriptArray *array = CScriptArray::Create(arrayType);

            array->Resize(x.size());

            for(int i = 0; i < x.size(); ++i)
            {
                array->SetValue(i,(void*)(new float(x.at(i))),false);
            }

            return array;

        }
        LISEMS_ERROR("Could not convert list to std::vector");
        throw 1;
    }

};

template<>
class asbindc_convert<CScriptArray*,std::vector<cTMap*>>
{

public:

    inline static std::vector<cTMap*> asbind_convert(CScriptArray* x)
    {
        std::vector<cTMap*> ret = x->ToSTDList<cTMap*>();
        return ret;

    }
};

template<>
class asbindc_convert<std::vector<cTMap*>,CScriptArray*>
{

public:

    inline static CScriptArray* asbind_convert(std::vector<cTMap*> x)
    {
        // Obtain a pointer to the engine
        asIScriptContext *ctx = asGetActiveContext();
        asIScriptEngine *engine = ctx->GetEngine();

        // TODO: This should only be done once
        // TODO: This assumes that CScriptArray was already registered
        asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<Map>");

        // Create the array object
        CScriptArray *array = CScriptArray::Create(arrayType);
        array->Resize(x.size());
        for(int i = 0; i < x.size(); i++)
        {
            array->SetValue(i,x.at(i),false);
        }

        return array;
    }
};

template<>
class asbindc_convert<std::vector<std::vector<float>>,CScriptArray*>
{

public:

    inline static CScriptArray* asbind_convert(std::vector<std::vector<float>> x)
    {
        // Obtain a pointer to the engine
        asIScriptContext *ctx = asGetActiveContext();
        asIScriptEngine *engine = ctx->GetEngine();

        // TODO: This should only be done once
        // TODO: This assumes that CScriptArray was already registered
        asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<array<float>>");

        // Create the array object
        CScriptArray *array = CScriptArray::Create(arrayType);
        array->Resize(x.size());
        for(int i = 0; i < x.size(); i++)
        {
            asITypeInfo *arrayType = engine->GetTypeInfoByDecl("array<array<float>>");

            // Create the array object
            CScriptArray *array2 = CScriptArray::Create(arrayType);
            array2->Resize(x.at(i).size());

            for(int j = 0; j < x.at(i).size(); j++)
            {
                array2->SetValueNoRefAdd(j, new float(x.at(i).at(j)));
            }

            array->SetValue(i,array2,false);
        }

        return array;
    }
};


template<>
class asbindc_convert<CScriptArray*,std::vector<std::vector<float>>>
{

public:

    inline static std::vector<std::vector<float>> asbind_convert(CScriptArray* x)
    {

        std::vector<std::vector<float>> ret;

        std::vector<CScriptArray *> temp = x->ToSTDList<CScriptArray*>();
        for(int i = 0; i < temp.size(); i++)
        {
            ret.push_back((temp.at(i)->ToSTDList<float>()));
        }
        return ret;
    }
};






//partial template specialization of the asbind_convert function
//this only works because we put it as a static member function

template<typename A>
class asbindc_convert<CScriptArray*,std::vector<A*>>
{

public:

    inline static std::vector<A*> asbind_convert(CScriptArray* x)
    {
        std::vector<A*> ret = x->ToSTDList<A*>();
        return ret;



    }
};


template<typename A>
class asbindc_convert<CScriptArray*,std::vector<std::vector<A>>>
{

public:

    inline static std::vector<std::vector<A>> asbind_convert(CScriptArray* x)
    {

        std::vector<std::vector<A>> ret;

        std::vector<CScriptArray *> temp = x->ToSTDList<CScriptArray*>();
        for(int i = 0; i < temp.size(); i++)
        {
            ret.push_back((temp.at(i)->ToSTDList<A>()));
        }
        return ret;
    }
};























//genera case
template<typename R, typename... Params, size_t... Is>
auto call_bindconvert(R(*fl)(Params...), index_list<Is...>)
{
    typedef function_traits<decltype(fl)> traits;


    //convert input params and output parameter
    //many overload may be required
    auto in_changed = std::bind(asbindc_convert<R,std::conditional_t<is_std_vector<R>::value,CScriptArray*,R>>::asbind_convert ,std::bind(fl,std::bind(asbindc_convert<std::conditional_t<is_std_vector<Params>::value,CScriptArray*,Params>,Params>::asbind_convert,std::_Placeholder<Is+1>())...));

    return in_changed;
}

//specialization for functions with void return type
template< typename... Params, size_t... Is>
auto call_bindconvert(void(*fl)(Params...), index_list<Is...>)
{
    typedef function_traits<decltype(fl)> traits;

    //convert input params
    //many overload may be required
    auto in_changed = std::bind(fl,std::bind(asbindc_convert<std::conditional_t<is_std_vector<Params>::value,CScriptArray*,Params>,Params>::asbind_convert,std::_Placeholder<Is+1>())...);

    return in_changed;
}

//the calling interface to get a lambda function
//example:
//
//
//void f(float x, std::vector<float> y)
//auto lambda = GetFuncConvert(f2);
//
//
//type of lambda is void(*)(float,CScriptArray*)
//
//this works as long as a conversion function can be found
//these should be defined by the user for relevant types

template<typename R, typename... Params>
std::function<std::conditional_t<is_std_vector<R>::value,CScriptArray*,R>(std::conditional_t<is_std_vector<Params>::value,CScriptArray*,Params>...)> GetFuncConvert(R(*fl)(Params...))
{
    return call_bindconvert(fl,index_range<0, sizeof...(Params)>());
}

#define asFUNCTIONPRCONV(f,p,r) (GetFuncConvert((static_cast<r (*)p>(f))))

#endif // SCRIPTARRAYHELPERS_H

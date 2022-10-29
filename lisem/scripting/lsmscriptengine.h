#ifndef SPHSCRIPTENGINE_H
#define SPHSCRIPTENGINE_H

#include "angelscript.h"
#include "QList"
#include "QString"
#include "error.h"
#include "QMap"
#include "iostream"
#include "defines.h"

// Base class for all functions so that we can store all functions
// in a single container.
struct ASSTDFunction {
  virtual ~ASSTDFunction() { }
};

// Derived class template for functions with a particular signature.
template<typename R, typename... Args>
struct ASSTDBasicFunction : ASSTDFunction {
  std::function<R(Args...)> function;
  ASSTDBasicFunction(std::function<R(Args...)> function) : function(function) { }
};


//we pass through all the relevant functions
//except we do our own error handling internally
//furthermore, we keep track of all the functions
//so we can later use them in the toolbox

typedef struct ScriptFunctionInfo
{
    bool is_basetypeoperator = false;
    QString basetypeoperatorname;
    QString basetypeoperatorparametertype;
    QString basetypeoperatorreturntype;
    bool is_member = false;
    bool is_constructor = false;
    bool is_valueobjectconstructor = false;
    asITypeInfo * ObjectType = nullptr;
    QString ObjectName = "";
    asIScriptFunction * Function;
    QString Description;
    QList<QString> ParameterTypes;
    QList<QString> Parameters;
    QList<QString> ParameterDescription;
    QString ReturnName;
    QString ReturnDescription;

} ScriptFunctionInfo;

typedef struct ScriptClassInfo
{
    QString classname;
    int templates;

} ScriptClassInfo;

typedef struct ScriptClassPropertyInfo
{

    QString classname;
    int templates;

    QString Name;
    QString Type;

} ScriptClassPropertyInfo;

class LISEM_API LSMScriptEngine : public asIScriptEngine
{

private:
    asIScriptEngine * m_ASEngine = nullptr;

    std::vector<ASSTDFunction*> m_ASSTDFuncs;

    QMap<QString, int> m_DescriptionMap;

    //function descriptions
    QList<QString> m_FunctionNames;
    QList<QString> m_FunctionDescriptions;
    QList<QString> m_ReturnNames;
    QList<QString> m_ReturnDescriptions;
    QList<QList<QString>> m_ParameterTypes;
    QList<QList<QString>> m_ParameterNames;
    QList<QList<QString>> m_ParameterDescriptions;

    QMap<QString,QString> m_Operators;
    QMap<QString,QString> m_OperatorNames;
public:

    LSMScriptEngine(asIScriptEngine * ASEngine);
    virtual ~LSMScriptEngine();
public:

    //////////////////
    ///this is specific functionality for our scripting needs
    //////////////////


    void RegisterFunctionDescriptions(QStringList file);

    void RegisterBaseTypeDebugFunctions();

    inline QString GetUITypeName(int in_typeid)
    {
        if(in_typeid == asTYPEID_VOID)
        {
            return "";
        }
        //The type id for void.

        if(in_typeid == asTYPEID_BOOL)
        {
            return "bool";
        }
        //The type id for bool.

        if(in_typeid == asTYPEID_INT8)
        {
            return "int";
        }
        //The type id for int8.

        if(in_typeid == asTYPEID_INT16)
        {
            return "int";
        }
        //The type id for int16.

        if(in_typeid == asTYPEID_INT32)
        {
            return "int";
        }
        //The type id for int.

        if(in_typeid == asTYPEID_INT64)
        {
            return "int";
        }
        //The type id for int64.

        if(in_typeid == asTYPEID_UINT8)
        {
            return "int";
        }
        //The type id for uint8.

        if(in_typeid == asTYPEID_UINT16)
        {
            return "int";
        }
        //The type id for uint16.

        if(in_typeid == asTYPEID_UINT32)
        {
            return "int";
        }
        //The type id for uint.

        if(in_typeid == asTYPEID_UINT64)
        {
            return "int";
        }
        //The type id for uint64.

        if(in_typeid == asTYPEID_FLOAT)
        {
            return "float";
        }
        //The type id for float.

        if(in_typeid == asTYPEID_DOUBLE)
        {
            return "double";
        }

        asITypeInfo * ti = GetTypeInfoById(in_typeid);
        QString type;
        if(ti != nullptr)
        {
            type += ti->GetName();
        }

        if(type == "Map")
        {

            return "Map";
        }

        if(type == "GeoProjection")
        {

            return "GeoProjection";
        }


        if(type == "Shapes")
        {

            return "Shapes";
        }

        if(type == "Table")
        {

            return "Table";
        }

        if(type == "PointCloud")
        {

            return "PointCloud";
        }

        if(type == "array<Map>")
        {

            return "MapArray";
        }

        if(type == "BoundingBox")
        {

            return "BoundingBox";
        }

        if(type == "Point")
        {

            return "BoundingBox";
        }


        if(type == "string")
        {
            return "string";
        }
        return type;

    }

    inline QString GetTypeNameFromUIName(QString name)
    {
        QString type;

        if(name == "Map")
        {
            return "Map";
        }else if(name == "GeoProjection")
        {
            return "GeoProjection";
        }else if(name == "Point")
        {
            return "Point";
        }else if(name == "Shapes")
        {
            return "Shapes";
        }else if(name == "Table")
        {
            return "Table";
        }else if(name == "PointCloud")
        {
            return "PointCloud";
        }else if(name == "MapArray")
        {
            return "array<Map>";
        }else if(name == "BoundingBox")
        {
            return "BoundingBox";
        }else if(name == "bool")
        {
            return "bool";
        }else if(name == "double")
        {
            return "double";
        }else if(name == "float")
        {
            return "float";
        }else if(name == "int")
        {
            return "int";
        }else if(name == "string")
        {
            return "string";
        }
        return type;
    }


    inline QString GetTypeName(int in_typeid)
    {

        asITypeInfo * ti = GetTypeInfoById(in_typeid);
        QString type;
        if(ti != nullptr)
        {
            type += ti->GetName();
        }else
        {
            type += GetBaseCodeTypeName(in_typeid);
        }

        return type;
    }

    inline QString GetBaseTypeName(int in_typeid)
    {
        if(in_typeid == asTYPEID_VOID)
        {
            return "void";
        }
        //The type id for void.

        if(in_typeid == asTYPEID_BOOL)
        {
            return "bool";
        }
        //The type id for bool.

        if(in_typeid == asTYPEID_INT8)
        {
            return "integer (8)";
        }
        //The type id for int8.

        if(in_typeid == asTYPEID_INT16)
        {
            return "integer (16)";
        }
        //The type id for int16.

        if(in_typeid == asTYPEID_INT32)
        {
            return "integer (32)";
        }
        //The type id for int.

        if(in_typeid == asTYPEID_INT64)
        {
            return "integer (64)";
        }
        //The type id for int64.

        if(in_typeid == asTYPEID_UINT8)
        {
            return "unsigned integer (8)";
        }
        //The type id for uint8.

        if(in_typeid == asTYPEID_UINT16)
        {
            return "unsigned integer (16)";
        }
        //The type id for uint16.

        if(in_typeid == asTYPEID_UINT32)
        {
            return "unsigned integer (32)";
        }
        //The type id for uint.

        if(in_typeid == asTYPEID_UINT64)
        {
            return "unsigned integer (64)";
        }
        //The type id for uint64.

        if(in_typeid == asTYPEID_FLOAT)
        {
            return "float (32)";
        }
        //The type id for float.

        if(in_typeid == asTYPEID_DOUBLE)
        {
            return "double (64)";
        }
        //The type id for double.

        return "void";

    }

    inline QString GetBaseCodeTypeName(int in_typeid)
    {
        if(in_typeid == asTYPEID_VOID)
        {
            return "void";
        }
        //The type id for void.

        if(in_typeid == asTYPEID_BOOL)
        {
            return "bool";
        }
        //The type id for bool.

        if(in_typeid == asTYPEID_INT8)
        {
            return "int";
        }
        //The type id for int8.

        if(in_typeid == asTYPEID_INT16)
        {
            return "int";
        }
        //The type id for int16.

        if(in_typeid == asTYPEID_INT32)
        {
            return "int";
        }
        //The type id for int.

        if(in_typeid == asTYPEID_INT64)
        {
            return "int";
        }
        //The type id for int64.

        if(in_typeid == asTYPEID_UINT8)
        {
            return "uint";
        }
        //The type id for uint8.

        if(in_typeid == asTYPEID_UINT16)
        {
            return "uint";
        }
        //The type id for uint16.

        if(in_typeid == asTYPEID_UINT32)
        {
            return "uint";
        }
        //The type id for uint.

        if(in_typeid == asTYPEID_UINT64)
        {
            return "uint";
        }
        //The type id for uint64.

        if(in_typeid == asTYPEID_FLOAT)
        {
            return "float";
        }
        //The type id for float.

        if(in_typeid == asTYPEID_DOUBLE)
        {
            return "double";
        }
        //The type id for double.

        return "void";

    }

    inline QStringList GetNumberUnaryOperators()
    {
        QStringList s;
        s.append("opNeg");
        return s;
    }

    inline QStringList GetBoolUnaryOperators()
    {
        QStringList s;

        s.append("opCom");

        return s;
    }

    inline QStringList GetNumberBinaryOperators()
    {
        QStringList s;

        s.append("opAdd");
        s.append("opSub");
        s.append("opMul");
        s.append("opDiv");
        s.append("opMod");
        s.append("opPow");

        return s;
    }

    inline QStringList GetNumberCompareOperators()
    {
        QStringList s;

        s.append("opCmp");
        s.append("opCmp2");
        s.append("opCmp3");
        s.append("opCmp4");
        s.append("opEquals");
        s.append("opEqualsnot");
        return s;
    }

    inline QStringList GetBoolBinaryOperators()
    {
        QStringList s;
        s.append("opAnd");
        s.append("opXor");
        s.append("opOr");
        s.append("opEquals");
        s.append("opEqualsnot");
        return s;
    }
    inline bool GetOperatorIsReverseFromMemberFunctionName(QString name)
    {
        if(name.endsWith("_r"))
        {
            return true;
        }else {
            return false;
        }
    }
    inline QString GetOperatorFromMemberFunctionName(QString name)
    {

        if(m_Operators.contains(name))
        {
            return m_Operators[name];
        }else {

            return QString();
        }

    }

    inline QString GetOperatorNameFromMemberFunctionName(QString name)
    {

        if(m_OperatorNames.contains(name))
        {
            return m_OperatorNames[name];
        }else {

            return QString();
        }

    }

    ////Get the information on the currently registered stuff

    inline QList<QString> GetGlobalFunctionStringList()
    {
        QList<QString> ret;
        for( int n = 0; n < (asUINT)m_ASEngine->GetGlobalFunctionCount(); n++ )
        {
            asIScriptFunction *func = m_ASEngine->GetGlobalFunctionByIndex(n);

            // Skip the functions that start with _ as these are not meant to be called explicitly by the user
            if( func->GetName()[0] != '_' )
            {
                ret.append(QString(" ")+func->GetDeclaration());
            }

        }

        return ret;
    }

    inline QList<ScriptFunctionInfo> GetConstructorFunctionList()
    {
        QList<ScriptFunctionInfo> ret;

        uint count = GetObjectTypeCount();
        for(uint i = 0; i < count; i++)
        {
            asITypeInfo * ti = GetObjectTypeByIndex(i);
            QString name = QString(ti->GetName());
            for(uint j = 0; j < ti->GetFactoryCount(); j++)
            {
                ScriptFunctionInfo fi;
                asIScriptFunction *func = ti->GetFactoryByIndex(j);
                fi.Function = func;
                fi.is_member = false;
                fi.is_constructor = true;
                fi.ObjectName = name;
                fi.ObjectType = ti;


                ret.append(fi);
            }
            for(uint j = 0; j < ti->GetBehaviourCount(); j++)
            {

                ScriptFunctionInfo fi;
                asEBehaviours b;
                asIScriptFunction *func = ti->GetBehaviourByIndex(j,&b);
                if(b == asBEHAVE_CONSTRUCT)
                {
                    fi.Function = func;
                    fi.is_member = false;
                    fi.is_constructor = true;
                    fi.is_valueobjectconstructor = true;
                    fi.ObjectName = name;
                    fi.ObjectType = ti;

                    ret.append(fi);
                }

            }
        }

        return ret;
    }

    inline QList<ScriptClassInfo> GetObjectList()
    {
        QList<ScriptClassInfo> ret;

        uint count = GetObjectTypeCount();
        for(uint i = 0; i < count; i++)
        {
            ScriptClassInfo reti;
            asITypeInfo * ti = GetObjectTypeByIndex(i);
            QString name = QString(ti->GetName());
            reti.classname = name;
            reti.templates = ti->GetSubTypeCount();
            ret.push_back(reti);
        }
        return ret;
    }

    inline QStringList GetObjectNameList()
    {
        QStringList ret;

        uint count = GetObjectTypeCount();
        for(uint i = 0; i < count; i++)
        {
            asITypeInfo * ti = GetObjectTypeByIndex(i);
            QString name = QString(ti->GetName());
            ret.append(name);
        }
        return ret;
    }

    inline QList<ScriptFunctionInfo> GetMemberFunctionList()
    {
        QList<ScriptFunctionInfo> ret;

        uint count = GetObjectTypeCount();
        for(uint i = 0; i < count; i++)
        {
            asITypeInfo * ti = GetObjectTypeByIndex(i);
            QString name = QString(ti->GetName());
            for(uint j = 0; j < ti->GetMethodCount(); j++)
            {
                ScriptFunctionInfo fi;
                asIScriptFunction *func = ti->GetMethodByIndex(j);
                fi.Function = func;
                fi.is_member = true;
                fi.ObjectName = name;
                fi.ObjectType = ti;

                ret.append(fi);
            }
        }

        return ret;
    }

    QList<ScriptFunctionInfo> GetGlobalFunctionList();


    inline QList<QString> GetTypeList()
    {

        QList<QString> ret;

        return ret;
    }

    inline QList<QString> GetTypeMethodList(QString type)
    {
        QList<QString> ret;

        return ret;
    }


    std::vector<std::pair<int,std::function<QString(void*)>>> m_Functions_DebugText;
    std::vector<std::pair<int,std::pair<QString,std::function<void(void*)>>>> m_Functions_DebugAction;
    std::vector<std::pair<int,std::function<void(void*)>>> m_Functions_DebugReplace;
    std::vector<std::pair<int,std::function<std::vector<int>(void*)>>> m_Functions_DebugListDims;
    std::vector<std::pair<int,std::function<int(void*)>>> m_Functions_DebugListType;
    std::vector<std::pair<int,std::function<void*(void*,std::vector<int>)>>> m_Functions_DebugListGet;

    template<typename A>
    inline bool int_vec_pair_containskey(A list, int key)
    {
        for(int i = 0; i < list.size(); i++)
        {
            if(list.at(i).first == key)
            {
                return true;
            }
        }

        return false;

    }
    inline void RegisterTypeDebugTextConversion(int typeId, std::function<QString(void*)> convertf)
    {
        m_Functions_DebugText.push_back(std::pair<int,std::function<QString(void*)>>(typeId,convertf));
    }
    inline void RegisterTypeDebugAction(int typeId, QString action, std::function<void(void*)> actionf)
    {
        m_Functions_DebugAction.push_back(std::pair<int, std::pair<QString,std::function<void(void*)>>>(typeId,std::pair<QString,std::function<void(void*)>>(QString(action),std::move(actionf))));
    }
    inline void RegisterTypeDebugReplace(int typeId, std::function<void* (void*)> replacef)
    {
        m_Functions_DebugReplace.push_back(std::pair<int,std::function<void*(void*)>>(typeId,replacef));
    }
    inline void RegisterTypeDebugList(int typeId, std::function<std::vector<int>(void*)> dimsf, std::function<int(void*)> typef,std::function<void*(void*,std::vector<int>)> getf)
    {
        m_Functions_DebugListDims.push_back(std::pair<int,std::function<std::vector<int>(void*)>>(typeId,dimsf));
        m_Functions_DebugListType.push_back(std::pair<int,std::function<int(void*)>>(typeId,typef));
        m_Functions_DebugListGet.push_back(std::pair<int,std::function<void*(void*,std::vector<int>)>>(typeId,getf));
    }

    inline std::vector<std::pair<QString,std::function<void(void*)>>> GetTypeDebugActions(int typeId)
    {
        std::vector<std::pair<QString,std::function<void(void*)>>> ret;

        for(int i = 0; i < m_Functions_DebugAction.size(); i++)
        {
            if(m_Functions_DebugAction.at(i).first == typeId)
            {
                ret.push_back(m_Functions_DebugAction.at(i).second);
            }
        }
        return ret;
    }

    inline bool GetTypeIsDebugList(int typeId)
    {
        return int_vec_pair_containskey(m_Functions_DebugListDims,typeId);
    }

    inline std::vector<int> GetTypeDebugListDims(int typeId, void * var)
    {
        for(int i = 0; i < m_Functions_DebugListDims.size(); i++)
        {
            if(m_Functions_DebugListDims.at(i).first == typeId)
            {
                return m_Functions_DebugListDims.at(i).second(var);
            }
        }

        return {0};

    }

    inline void* GetTypeDebugListGet(int typeId, void * var, std::vector<int> n)
    {

        for(int i = 0; i < m_Functions_DebugListGet.size(); i++)
        {
            if(m_Functions_DebugListGet.at(i).first == typeId)
            {
                return m_Functions_DebugListGet.at(i).second(var,n);
            }
        }

        return nullptr;

    }

    inline int GetTypeDebugListType(int typeId, void * var)
    {
        for(int i = 0; i < m_Functions_DebugListType.size(); i++)
        {
            if(m_Functions_DebugListType.at(i).first == typeId)
            {
                return m_Functions_DebugListType.at(i).second(var);
            }
        }

        return 0;
    }


    inline QList<QString> GetTypeDebugActionNames(int typeId)
    {
        QList<QString> ret;

        for(int i = 0; i < m_Functions_DebugAction.size(); i++)
        {
            if(m_Functions_DebugAction.at(i).first == typeId)
            {
                ret.append(m_Functions_DebugAction.at(i).second.first);
            }
        }

        return ret;
    }

    inline QString GetTypeDebugValue(int typeId, void * var)
    {
        for(int i = 0; i < m_Functions_DebugText.size(); i++)
        {
            if(m_Functions_DebugText.at(i).first == typeId)
            {
                return m_Functions_DebugText.at(i).second(var);
            }
        }

        return "{...}";
    }

    inline bool GetTypeDebugAction(int typeId, void * var, QString action)
    {
        for(int i = 0; i < m_Functions_DebugAction.size(); i++)
        {
            if(m_Functions_DebugAction.at(i).first == typeId)
            {
                if(m_Functions_DebugAction.at(i).second.first == action)
                {
                    m_Functions_DebugAction.at(i).second.second(var);
                    return true;
                }
            }
        }

        return false;

    }

    //////////////////
    ///this is the normal engine functionality
    //////////////////
    ///

    // Memory management
    inline int AddRef() const { return m_ASEngine->AddRef();}
    inline int Release() const { return m_ASEngine->Release();}
    inline int ShutDownAndRelease(){ return m_ASEngine->ShutDownAndRelease();}

    // Engine properties
    inline  int     SetEngineProperty(asEEngineProp property, asPWORD value){ return m_ASEngine->SetEngineProperty(property, value);}
    inline  asPWORD GetEngineProperty(asEEngineProp property)const { return m_ASEngine->GetEngineProperty(property);}

    // Compiler messages
    inline  int SetMessageCallback(const asSFuncPtr &callback, void *obj, asDWORD callConv){ return m_ASEngine->SetMessageCallback(callback, obj, callConv);}
    inline  int ClearMessageCallback(){ return m_ASEngine->ClearMessageCallback();}
    inline  int WriteMessage(const char *section, int row, int col, asEMsgType type, const char *message){ return m_ASEngine->WriteMessage(section, row, col, type, message);}

    // JIT Compiler
    inline  int SetJITCompiler(asIJITCompiler *compiler){ return m_ASEngine->SetJITCompiler(compiler);}
    inline  asIJITCompiler *GetJITCompiler() const { return m_ASEngine->GetJITCompiler();}




    // Global functions
    inline  int RegisterGlobalFunction(const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv, void *auxiliary = 0)
    {
        int r =m_ASEngine->RegisterGlobalFunction(declaration, funcPointer, callConv, auxiliary);
        if(r < 0)
        {
            LISEM_DEBUG("Error in registering global function: " + QString(declaration));
        }
        return r;
    }


    template<typename R, typename... Args>
    inline int RegisterGlobalSTDFunction(const char *declaration, std::function<R(Args...)> funcPointer)
    {
         ASSTDBasicFunction<R,Args...> *f = new ASSTDBasicFunction<R,Args...>(funcPointer);
         m_ASSTDFuncs.push_back(f);
         int r =m_ASEngine->RegisterGlobalFunction(declaration, asMETHOD(decltype(funcPointer), operator()), asCALL_THISCALL_ASGLOBAL, &f->function);
         if(r < 0)
         {
             LISEM_DEBUG("Error in registering global function: " + QString(declaration));
         }
         return r;


    }


    template<typename R, typename... Args>
    inline int RegisterGlobalSTDFunction(const char *declaration, std::function<R(Args...)> funcPointer, asDWORD callConv)
    {
         ASSTDBasicFunction<R,Args...> *f = new ASSTDBasicFunction<R,Args...>(funcPointer);
         m_ASSTDFuncs.push_back(f);
         int r =m_ASEngine->RegisterGlobalFunction(declaration, asMETHOD(decltype(funcPointer), operator()), asCALL_THISCALL_ASGLOBAL, &f->function);
         if(r < 0)
         {
             LISEM_DEBUG("Error in registering global function: " + QString(declaration));
         }
         return r;


    }

    template<typename R, typename... Args>
    inline  int            RegisterObjectSTDMethod(const char *obj, const char *declaration, std::function<R(Args...)> funcPointer)
    {
        ASSTDBasicFunction<R,Args...> *f = new ASSTDBasicFunction<R,Args...>(funcPointer);
        m_ASSTDFuncs.push_back(f);
        int r = m_ASEngine->RegisterObjectMethod(obj,declaration, asMETHOD(decltype(funcPointer), operator()),  asCALL_CDECL_OBJFIRST);
        if( r < 0)
        {
            LISEM_DEBUG("Error in registering object method: " + QString(obj) + " " + QString(declaration));
        }
        return r;
    }


    inline  asUINT             GetGlobalFunctionCount() const { return m_ASEngine->GetGlobalFunctionCount();}
    inline  asIScriptFunction *GetGlobalFunctionByIndex(asUINT index) const { return m_ASEngine->GetGlobalFunctionByIndex(index);}
    inline  asIScriptFunction *GetGlobalFunctionByDecl(const char *declaration) const { return m_ASEngine->GetGlobalFunctionByDecl(declaration) ;}

    // Global properties
    inline  int    RegisterGlobalProperty(const char *declaration, void *pointer)
    {

        return m_ASEngine->RegisterGlobalProperty( declaration, pointer);
    }
    inline  asUINT GetGlobalPropertyCount() const { return m_ASEngine->GetGlobalPropertyCount();}
    inline  int    GetGlobalPropertyByIndex(asUINT index, const char **name, const char **nameSpace = 0, int *typeId = 0, bool *isConst = 0, const char **configGroup = 0, void **pointer = 0, asDWORD *accessMask = 0) const { return m_ASEngine->GetGlobalPropertyByIndex(index, name, nameSpace , typeId , isConst, configGroup, pointer, accessMask) ;}
    inline  int    GetGlobalPropertyIndexByName(const char *name) const { return m_ASEngine->GetGlobalPropertyIndexByName(name);}
    inline  int    GetGlobalPropertyIndexByDecl(const char *decl) const { return m_ASEngine->GetGlobalPropertyIndexByDecl(decl);}

    // Object types
    inline  int            RegisterObjectType(const char *obj, int byteSize, asDWORD flags)
    {
        int r =  m_ASEngine->RegisterObjectType(obj,byteSize,flags);
        if(r < 0)
        {
            LISEM_DEBUG("Error in registering object type: " + QString(obj));
        }
        return r;
    }
    inline  int            RegisterObjectProperty(const char *obj, const char *declaration, int byteOffset, int compositeOffset = 0, bool isCompositeIndirect = false) { return m_ASEngine->RegisterObjectProperty(obj,declaration, byteOffset,compositeOffset,isCompositeIndirect);}
    inline  int            RegisterObjectMethod(const char *obj, const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv, void *auxiliary = 0, int compositeOffset = 0, bool isCompositeIndirect = false)
    {
        int r = m_ASEngine->RegisterObjectMethod(obj,declaration, funcPointer,  callConv, auxiliary, compositeOffset , isCompositeIndirect);
        if( r < 0)
        {
            LISEM_DEBUG("Error in registering object method: " + QString(obj) + " " + QString(declaration));
        }
        return r;
    }
    inline  int            RegisterObjectBehaviour(const char *obj, asEBehaviours behaviour, const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv, void *auxiliary = 0, int compositeOffset = 0, bool isCompositeIndirect = false)
    {
        int r = m_ASEngine->RegisterObjectBehaviour(obj, behaviour, declaration,funcPointer, callConv,auxiliary , compositeOffset , isCompositeIndirect);
        if(r < 0)
        {
            LISEM_DEBUG("Error in registering object behavior: " + QString(obj) + " declared as: " + QString(declaration));
        }
        return r;
    }
    inline  int            RegisterInterface(const char *name) { return m_ASEngine->RegisterInterface(name);}
    inline  int            RegisterInterfaceMethod(const char *intf, const char *declaration) { return m_ASEngine->RegisterInterfaceMethod(intf, declaration);}
    inline  asUINT         GetObjectTypeCount() const { return m_ASEngine->GetObjectTypeCount();}
    inline  asITypeInfo   *GetObjectTypeByIndex(asUINT index) const { return m_ASEngine->GetObjectTypeByIndex(index);}

    // String factory
    inline  int RegisterStringFactory(const char *datatype, asIStringFactory *factory) { return m_ASEngine->RegisterStringFactory(datatype, factory);}
    inline  int GetStringFactoryReturnTypeId(asDWORD *flags = 0) const { return m_ASEngine->GetStringFactoryReturnTypeId(flags = 0);}

    // Default array type
    inline  int RegisterDefaultArrayType(const char *type) { return m_ASEngine->RegisterDefaultArrayType(type);}
    inline  int GetDefaultArrayTypeId() const { return m_ASEngine->GetDefaultArrayTypeId();}

    // Enums
    inline  int          RegisterEnum(const char *type) { return m_ASEngine->RegisterEnum(type);}
    inline  int          RegisterEnumValue(const char *type, const char *name, int value) { return m_ASEngine->RegisterEnumValue(type,name, value);}
    inline  asUINT       GetEnumCount() const { return m_ASEngine->GetEnumCount();}
    inline  asITypeInfo *GetEnumByIndex(asUINT index) const { return m_ASEngine->GetEnumByIndex(index);}

    // Funcdefs
    inline  int          RegisterFuncdef(const char *decl)
    {
        int r = m_ASEngine->RegisterFuncdef(decl);
        if(r < 0)
        {
            LISEM_DEBUG("Error in registering function type definition declared as: " + QString(decl));
        }
        return r;
    }
    inline  asUINT       GetFuncdefCount() const { return m_ASEngine->GetFuncdefCount();}
    inline  asITypeInfo *GetFuncdefByIndex(asUINT index) const { return m_ASEngine->GetFuncdefByIndex(index);}

    // Typedefs
    inline  int          RegisterTypedef(const char *type, const char *decl) {
        int r = m_ASEngine->RegisterTypedef(type, decl) ;
       if(r < 0)
       {
           LISEM_DEBUG("Error in registering typedef: " + QString(type) + " declared as: " + QString(decl));
       }
     return r;}
    inline  asUINT       GetTypedefCount() const { return m_ASEngine->GetTypedefCount();}
    inline  asITypeInfo *GetTypedefByIndex(asUINT index) const { return m_ASEngine->GetTypedefByIndex(index);}

    // Configuration groups
    inline  int         BeginConfigGroup(const char *groupName) { return m_ASEngine->BeginConfigGroup(groupName);}
    inline  int         EndConfigGroup() { return m_ASEngine->EndConfigGroup();}
    inline  int         RemoveConfigGroup(const char *groupName) { return m_ASEngine->RemoveConfigGroup(groupName);}
    inline  asDWORD     SetDefaultAccessMask(asDWORD defaultMask) { return m_ASEngine->SetDefaultAccessMask(defaultMask);}
    inline  int         SetDefaultNamespace(const char *nameSpace) { return m_ASEngine->SetDefaultNamespace(nameSpace);}
    inline  const char *GetDefaultNamespace() const { return m_ASEngine->GetDefaultNamespace();}

    // Script modules
    inline  asIScriptModule *GetModule(const char *module, asEGMFlags flag = asGM_ONLY_IF_EXISTS) { return m_ASEngine->GetModule(module, flag);}
    inline  int              DiscardModule(const char *module) { return m_ASEngine->DiscardModule(module);}
    inline  asUINT           GetModuleCount() const { return m_ASEngine->GetModuleCount();}
    inline  asIScriptModule *GetModuleByIndex(asUINT index) const { return m_ASEngine->GetModuleByIndex(index);}

    // Script functions
    inline  asIScriptFunction *GetFunctionById(int funcId) const { return m_ASEngine->GetFunctionById(funcId);}

    // Type identification
    inline  int            GetTypeIdByDecl(const char *decl) const { return m_ASEngine->GetTypeIdByDecl(decl);}
    inline  const char    *GetTypeDeclaration(int typeId, bool includeNamespace = false) const { return m_ASEngine->GetTypeDeclaration(typeId, includeNamespace);}
    inline  int            GetSizeOfPrimitiveType(int typeId) const { return m_ASEngine->GetSizeOfPrimitiveType(typeId);}
    inline  asITypeInfo   *GetTypeInfoById(int typeId) const { return m_ASEngine->GetTypeInfoById(typeId);}
    inline  asITypeInfo   *GetTypeInfoByName(const char *name) const { return m_ASEngine->GetTypeInfoByName(name) ;}
    inline  asITypeInfo   *GetTypeInfoByDecl(const char *decl) const { return m_ASEngine->GetTypeInfoByDecl(decl);}

    // Script execution
    inline  asIScriptContext      *CreateContext() { return m_ASEngine->CreateContext();}
    inline  void                  *CreateScriptObject(const asITypeInfo *type) { return m_ASEngine->CreateScriptObject(type);}
    inline  void                  *CreateScriptObjectCopy(void *obj, const asITypeInfo *type) { return m_ASEngine->CreateScriptObjectCopy(obj, type);}
    inline  void                  *CreateUninitializedScriptObject(const asITypeInfo *type) { return m_ASEngine->CreateUninitializedScriptObject(type);}
    inline  asIScriptFunction     *CreateDelegate(asIScriptFunction *func, void *obj) { return m_ASEngine->CreateDelegate(func, obj);}
    inline  int                    AssignScriptObject(void *dstObj, void *srcObj, const asITypeInfo *type) { return m_ASEngine->AssignScriptObject(dstObj, srcObj, type);}
    inline  void                   ReleaseScriptObject(void *obj, const asITypeInfo *type) { return m_ASEngine->ReleaseScriptObject(obj, type);}
    inline  void                   AddRefScriptObject(void *obj, const asITypeInfo *type) { return m_ASEngine->AddRefScriptObject(obj, type);}
    inline  int                    RefCastObject(void *obj, asITypeInfo *fromType, asITypeInfo *toType, void **newPtr, bool useOnlyImplicitCast = false) { return m_ASEngine->RefCastObject(obj, fromType, toType,newPtr,useOnlyImplicitCast);}
    inline  asILockableSharedBool *GetWeakRefFlagOfScriptObject(void *obj, const asITypeInfo *type) const { return m_ASEngine->GetWeakRefFlagOfScriptObject(obj, type);}

    // Context pooling
    inline  asIScriptContext      *RequestContext() { return m_ASEngine->RequestContext();}
    inline  void                   ReturnContext(asIScriptContext *ctx) { return m_ASEngine->ReturnContext(ctx);}
    inline  int                    SetContextCallbacks(asREQUESTCONTEXTFUNC_t requestCtx, asRETURNCONTEXTFUNC_t returnCtx, void *param = 0) { return m_ASEngine->SetContextCallbacks(requestCtx, returnCtx, param);}

    // String interpretation
    inline  asETokenClass ParseToken(const char *string, size_t stringLength = 0, asUINT *tokenLength = 0) const { return m_ASEngine->ParseToken(string, stringLength, tokenLength);}

    // Garbage collection
    inline  int  GarbageCollect(asDWORD flags = asGC_FULL_CYCLE, asUINT numIterations = 1) { return m_ASEngine->GarbageCollect(flags, numIterations);}
    inline  void GetGCStatistics(asUINT *currentSize, asUINT *totalDestroyed = 0, asUINT *totalDetected = 0, asUINT *newObjects = 0, asUINT *totalNewDestroyed = 0) const { return m_ASEngine->GetGCStatistics(currentSize,totalDestroyed , totalDetected ,newObjects, totalNewDestroyed);}
    inline  int  NotifyGarbageCollectorOfNewObject(void *obj, asITypeInfo *type) { return m_ASEngine->NotifyGarbageCollectorOfNewObject(obj, type);}
    inline  int  GetObjectInGC(asUINT idx, asUINT *seqNbr = 0, void **obj = 0, asITypeInfo **type = 0) { return m_ASEngine->GetObjectInGC(idx,seqNbr, obj,type);}
    inline  void GCEnumCallback(void *reference) { return m_ASEngine->GCEnumCallback(reference);}
    inline  void ForwardGCEnumReferences(void *ref, asITypeInfo *type) { return m_ASEngine->ForwardGCEnumReferences(ref, type);}
    inline  void ForwardGCReleaseReferences(void *ref, asITypeInfo *type) { return m_ASEngine->ForwardGCReleaseReferences(ref, type);}
    inline  void SetCircularRefDetectedCallback(asCIRCULARREFFUNC_t callback, void *param = 0) { return m_ASEngine->SetCircularRefDetectedCallback(callback,param );}

    // User data
    inline  void *SetUserData(void *data, asPWORD type = 0) { return m_ASEngine->SetUserData(data, type);}
    inline  void *GetUserData(asPWORD type = 0) const { return m_ASEngine->GetUserData(type);}
    inline  void  SetEngineUserDataCleanupCallback(asCLEANENGINEFUNC_t callback, asPWORD type = 0) { return m_ASEngine->SetEngineUserDataCleanupCallback(callback,type);}
    inline  void  SetModuleUserDataCleanupCallback(asCLEANMODULEFUNC_t callback, asPWORD type = 0) { return m_ASEngine->SetModuleUserDataCleanupCallback(callback,type );}
    inline  void  SetContextUserDataCleanupCallback(asCLEANCONTEXTFUNC_t callback, asPWORD type = 0) { return m_ASEngine->SetContextUserDataCleanupCallback(callback,type) ;}
    inline  void  SetFunctionUserDataCleanupCallback(asCLEANFUNCTIONFUNC_t callback, asPWORD type = 0) { return m_ASEngine->SetFunctionUserDataCleanupCallback(callback, type);}
    inline  void  SetTypeInfoUserDataCleanupCallback(asCLEANTYPEINFOFUNC_t callback, asPWORD type = 0) { return m_ASEngine->SetTypeInfoUserDataCleanupCallback(callback, type);}
    inline  void  SetScriptObjectUserDataCleanupCallback(asCLEANSCRIPTOBJECTFUNC_t callback, asPWORD type = 0) { return m_ASEngine->SetScriptObjectUserDataCleanupCallback(callback, type);}

    // Exception handling
    inline  int SetTranslateAppExceptionCallback(asSFuncPtr callback, void *param, int callConv) { return m_ASEngine->SetTranslateAppExceptionCallback(callback, param,  callConv);}

};


#endif // SPHSCRIPTENGINE_H

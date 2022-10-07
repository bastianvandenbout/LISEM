#include "scriptmanager.h"


#include "extensions/modelscripting.h"

#include "angelscript.h"
#include "geo/raster/map.h"
#include "extensions/mapscripting.h"
#include "geo/raster/rasteralgebra.h"
#include "raster/rasterconstructors.h"
#include "extensions/vectorscripting.h"
#include "extensions/matrixtablescripting.h"
#include "QtConcurrent/QtConcurrent"
#include <iostream>
#include "extensions/scriptarray.h"
#include "extensions/geoalgorithmscripting.h"
#include "extensions/scriptoptimization.h"
#include "extensions/geoprojectionscripting.h"
#include "lsmscriptengine.h"
#include "extensions/scriptqstdstring.h"
#include "QString"
#include "extensions/scriptanalysis.h"
#include "extensions/scriptmath.h"
#include "extensions/pointcloudscripting.h"
#include "extensions/multimapscripting.h"
#include "extensions/precipitationanalysis.h"
#include "extensions/boundingboxscripting.h"
#include "extensions/scriptanalysis.h"
#include "extensions/scriptfunctionals.h"
#include "extensions/scriptshapes.h"
#include "extensions/linearalgebrascripting.h"
#include "opencv2/core.hpp"
#include "extensions/datetime.h"
#include "extensions/gpuscripting.h"
#include "extensions/meshscripting.h"
#include "lsmio.h"
#include "extensions/scriptio.h"
#include "site.h"
#include "extensions/fieldscripting.h"

ScriptManager * CScriptManager;
int CScriptCounter = 0;
QMutex CScriptCounterMutex;

ScriptManager::ScriptManager()
{

}


void ScriptManager::Initialize()
{
    int r;

    //register the angelscript engine
    m_Engine = new LSMScriptEngine(asCreateScriptEngine());


    // Register the callback with the engine
    m_Engine->SetTranslateAppExceptionCallback(asFUNCTION(TranslateException), 0, asCALL_CDECL);

    RegisterLinearAlgebraScripting(m_Engine);
    RegisterScriptArray(m_Engine,true);
    RegisterQStdString(m_Engine);
    RegisterScriptDateTime(m_Engine);

    m_Engine->RegisterFuncdef("double CALLBACKDFDL(array<double>)");
    m_Engine->RegisterFuncdef("double CALLBACKDFD(double)");
    m_Engine->RegisterFuncdef("double CALLBACKDFDD(double, double)");
    m_Engine->RegisterFuncdef("void CALLBACKDFDDBB(double, double, bool, bool)");
    m_Engine->RegisterFuncdef("void CALLBACKDFSBB(string, bool, bool)");
    m_Engine->RegisterFuncdef("void CALLBACKDFSDDBB(string, double,double,bool, bool)");
    m_Engine->RegisterFuncdef("string CALLBACKSFSL(array<string>)");

    RegisterGeoElementScripting(m_Engine);
    RegisterGeometryScripting(m_Engine);
    RegisterGeoProjectionToScriptEngine(m_Engine);
    RegisterTableToScriptEngine(m_Engine);
    RegisterMapToScriptEngine(m_Engine);
    RegisterMultiMapToScriptEngine(m_Engine);
    RegisterFieldToScriptEngine(m_Engine);
    RegisterVectorToScriptEngine(m_Engine);
    RegisterShapesToScriptEngine(m_Engine);
    RegisterPointCloudToScriptEngine(m_Engine);
    RegisterFunctionalsToScriptEngine(m_Engine);
    RegisterMapMathToScriptEngine(m_Engine);
    RegisterMapAlgorithmsToScriptEngine(m_Engine);
    RegisterLeastSquaresToScriptEngine(m_Engine);
    RegisterScriptMath(m_Engine);
    RegisterStatisticsToScriptEngine(m_Engine);
    RegisterGPUToScriptEngine(m_Engine);
    RegisterMeshToScriptEngine(m_Engine);
    RegisterScriptFunctionsIO(m_Engine);
    RegisterModelScriptFunctions(m_Engine);

    RegisterPrecipitationAnalysisToScriptEngine(m_Engine);
    RegisterGeoAlgorithmsToScriptEngine(m_Engine);

    // Tell the engine to output any error messages to printf
    m_Engine->SetMessageCallback(asFUNCTION(MessageCallback), this, asCALL_CDECL);

    // Register special function with overloads to catch any type.
    // This is used by the exec command to output the resulting value from the statement.
    r = m_Engine->RegisterGlobalFunction("void _grab(bool)", asFUNCTIONPR(grab, (bool), void), asCALL_CDECL); assert( r >= 0 );
    r = m_Engine->RegisterGlobalFunction("void _grab(int)", asFUNCTIONPR(grab, (int), void), asCALL_CDECL); assert( r >= 0 );
    r = m_Engine->RegisterGlobalFunction("void _grab(uint)", asFUNCTIONPR(grab, (asUINT), void), asCALL_CDECL); assert( r >= 0 );
    r = m_Engine->RegisterGlobalFunction("void _grab(float)", asFUNCTIONPR(grab, (float), void), asCALL_CDECL); assert( r >= 0 );
    r = m_Engine->RegisterGlobalFunction("void _grab(double)", asFUNCTIONPR(grab, (double), void), asCALL_CDECL); assert( r >= 0 );
    r = m_Engine->RegisterGlobalFunction("void _grab()", asFUNCTIONPR(grab, (void), void), asCALL_CDECL); assert( r >= 0 );
    r = m_Engine->RegisterGlobalFunction("void _grab(const string &in)", asFUNCTIONPR(grab, (const QString&), void), asCALL_CDECL); assert( r >= 0 );
    r = m_Engine->RegisterGlobalFunction("void Print(const string &in)", asFUNCTIONPR(print, (const QString&), void), asCALL_CDECL); assert( r >= 0 );
    r = m_Engine->RegisterGlobalFunction("void Print(const double &in)", asFUNCTIONPR(print, (const double&), void), asCALL_CDECL); assert( r >= 0 );
    r = m_Engine->RegisterGlobalFunction("void Print(const int &in)", asFUNCTIONPR(print, (const int&), void), asCALL_CDECL); assert( r >= 0 );
    r = m_Engine->RegisterGlobalFunction("void Print(const vec3 &in)", asFUNCTIONPR(print, (const LSMVector3&), void), asCALL_CDECL); assert( r >= 0 );
    r = m_Engine->RegisterGlobalFunction("string ToString(float x)", asFUNCTIONPR(ToString, (float), QString), asCALL_CDECL); assert( r >= 0 );
    r = m_Engine->RegisterGlobalFunction("string ToString(int x)", asFUNCTIONPR(ToString, (int), QString), asCALL_CDECL); assert( r >= 0 );
    r = m_Engine->RegisterGlobalFunction("string ToString(bool x)", asFUNCTIONPR(ToString, (bool), QString), asCALL_CDECL); assert( r >= 0 );
    r = m_Engine->RegisterGlobalFunction("string ToString(double x)", asFUNCTIONPR(ToString, (double), QString), asCALL_CDECL); assert( r >= 0 );
    r = m_Engine->RegisterGlobalFunction("void SetPlatformPreference(const string &in, int id)", asFUNCTIONPR(SetPlatformPreference, (const QString&, int id),  void), asCALL_CDECL);  assert( r >= 0 );

    r = m_Engine->RegisterGlobalProperty("string NVIDIA_PLATFORM", &NVIDIA_PLATFORM); assert( r >= 0 );
    r = m_Engine->RegisterGlobalProperty("string AMD_PLATFORM", &AMD_PLATFORM); assert( r >= 0 );
    r = m_Engine->RegisterGlobalProperty("string MESA_PLATFORM", &MESA_PLATFORM); assert( r >= 0 );
    r = m_Engine->RegisterGlobalProperty("string INTEL_PLATFORM", &INTEL_PLATFORM); assert( r >= 0 );
    r = m_Engine->RegisterGlobalProperty("string APPLE_PLATFORM", &APPLE_PLATFORM); assert( r >= 0 );


    //Get function descriptions
    QStringList function_descriptive_file = GetFileStringList(GetSite() + LISEM_FOLDER_ASSETS + "functions.txt");

    m_Engine->RegisterFunctionDescriptions(function_descriptive_file);
    m_Engine->RegisterBaseTypeDebugFunctions();

    // Do not output anything else to printf
    //m_Engine->ClearMessageCallback();

    asIScriptModule *mod = m_Engine->GetModule("console", asGM_CREATE_IF_NOT_EXISTS);

    this->start();

}

void ScriptManager::run()
{
    //we keep checking if there is a new assignment for the script manager
    //if so, run it, otherwise, wait untill woken up by someone adding extra work
    //if there is a request to quit, we do so

    m_StartWorkMutex.lock();


     bool stop = false;
     while(!stop)
     {


         //check if a stop is requested
         {
             m_StopMutex.lock();
             if(m_StopRequested)
             {
                 stop = true;
                 break;
             }
             m_StopMutex.unlock();
         }

         //wait untill notified to continue (either from work being added or through)
        m_StartWorkWaitCondition.wait(&m_StartWorkMutex);


        bool work = true;
        while(work)
        {
            //check if a stop is requested
            {
                m_StopMutex.lock();
                if(m_StopRequested)
                {
                    stop = true;
                    break;
                }
                m_StopMutex.unlock();
            }

            //get the first element from the work list

            SPHCodeJob worknow;

            //if there is no job we exit now
            if(m_Work.length() == 0)
            {
                work = false;
                continue;
            }else
            {
                worknow = m_Work.first();
                m_Work.removeAt(0);
            }

            //do the actual work
            QtConcurrent::run([this](asIScriptEngine *Engine,SPHCodeJob cj){

                m_ScriptWaitDoneMutex.lock();

                asIScriptContext *ctx = Engine->CreateContext();
                asIScriptFunction *func = cj.program->scriptbuilder->GetModule()->GetFunctionByDecl("void main()");
                ctx->Prepare(func);
                SetASDefaultPath(cj.program->HomeDir);

                ctx->SetLineCallback( asMETHODPR(SPHCodeJob, OnCallBackLine, (asIScriptContext*), void),&cj,asCALL_THISCALL);

                ctx->SetExceptionCallback( asMETHODPR(SPHCodeJob, OnCallBackException, (asIScriptContext*), void),&cj,asCALL_THISCALL);

                //set indicator that we are running
                this->m_IsScriptRuningMutex.lock();
                this->m_IsScriptRunning = true;
                this->m_IsScriptRuningMutex.unlock();

                try
                {

                    IncrementScriptCounter();

                    int r = ctx->Execute();

                    if( r == asEXECUTION_FINISHED )
                    {
                        cj.program->m_CallBackDone(true);
                    }else
                    {
                        cj.program->m_CallBackDone(false);
                    }
                }catch(std::exception e)
                {
                     LISEMS_ERROR("Fatal Exception");
                     cj.program->m_CallBackDone(false);


                }catch(cv::Exception e)
                {
                    LISEMS_ERROR("opencv exception" + QString(e.msg.c_str()));
                     cj.program->m_CallBackDone(false);

                }catch(...)
                {
                    LISEM_ERROR("Exception thrown during script run");
                     cj.program->m_CallBackDone(false);

                }

                //set indicator as done
                this->m_IsScriptRuningMutex.lock();
                this->m_IsScriptRunning = false;
                this->m_IsScriptRuningMutex.unlock();


                ctx->Release();

                m_ScriptWaitDoneMutex.unlock();
                m_ScriptWaitDoneCondition.notify_all();



            },m_Engine,worknow);

            //check if there is a job so we need to restart
            if(m_Work.length() == 0)
            {
                work = false;
                continue;
            }
        }


     }

     m_StopMutex.unlock();
     m_StartWorkMutex.unlock();

}


void MessageCallback(const asSMessageInfo *msg, void *param)
{
    ScriptManager * sm = static_cast<ScriptManager *>(param);

    if(sm->m_CurrentCompilation != nullptr)
    {
        if(sm->m_CurrentCompilation->m_CallBackCompileErrorSet)
        {
            sm->m_CurrentCompilation->m_CallBackCompileError(msg);
        }
    }

}


void ExecString(asIScriptEngine *engine, std::string &arg)
{
    std::string script;

    // Wrap the expression in with a call to _grab, which allow us to print the resulting value
    script = "_grab(" + arg + ")";

    // TODO: Add a time out to the script, so that never ending scripts doesn't freeze the application


    int r = ExecuteString(engine, script.c_str(), engine->GetModule("console"));
    if( r < 0 )
    {
        LISEM_ERROR("Invalid script statement. ");
    }
    else if( r == asEXECUTION_EXCEPTION )
    {
        LISEM_ERROR("A script exception was raised.");
    }
}



void AddVariable(asIScriptEngine *engine, std::string &arg)
{
    asIScriptModule *mod = engine->GetModule("console", asGM_CREATE_IF_NOT_EXISTS);

    // Add a semi-colon to end the statement (if not already there)
    if( arg.length() > 0 && arg[arg.length()-1] != ';' )
        arg += ";";

    int r = mod->CompileGlobalVar("addvar", arg.c_str(), 0);
    if( r < 0 )
    {
        // TODO: Add better description of error (invalid declaration, name conflict, etc)
        LISEM_ERROR("Failed to add variable. ")
    }
    else
        LISEM_ERROR("Variable added. ")
}

void DeleteVariable(asIScriptEngine *engine, std::string &arg)
{
    asIScriptModule *mod = engine->GetModule("console");
    if( mod == 0 || mod->GetGlobalVarCount() == 0 )
    {
        LISEM_ERROR("No variables have been added. ")
        return;
    }

    // trim the std::string to find the variable name
    size_t p1 = arg.find_first_not_of(" \n\r\t");
    if( p1 != std::string::npos )
        arg = arg.substr(p1, -1);
    size_t p2 = arg.find_last_not_of(" \n\r\t");
    if( p2 != std::string::npos )
        arg = arg.substr(0, p2+1);

    int index = mod->GetGlobalVarIndexByName(arg.c_str());
    if( index >= 0 )
    {
        mod->RemoveGlobalVar(index);
        LISEM_ERROR("Variable removed. ")
    }
    else
        LISEM_ERROR("No such variable. ")
}

void AddFunction(asIScriptEngine *engine, std::string &arg)
{
    asIScriptModule *mod = engine->GetModule("console", asGM_CREATE_IF_NOT_EXISTS);

    asIScriptFunction *func = 0;
    int r = mod->CompileFunction("addfunc", arg.c_str(), 0, asCOMP_ADD_TO_MODULE, &func);
    if( r < 0 )
    {
        // TODO: Add better description of error (invalid declaration, name conflict, etc)
        LISEM_ERROR("Failed to add function. ")
    }
    else
    {
        // The script engine supports function overloads, but to simplify the
        // console we'll disallow multiple functions with the same name.
        // We know the function was added, so if GetFunctionByName() fails it is
        // because there already was another function with the same name.
        if( mod->GetFunctionByName(func->GetName()) == 0 )
        {
            mod->RemoveFunction(func);
            LISEM_ERROR("Another function with that name already exists.")
        }
        else
            LISEM_ERROR("Function added. ")
    }

    // We must release the function object
    if( func )
        func->Release();
}

void DeleteFunction(asIScriptEngine *engine, std::string &arg)
{
    asIScriptModule *mod = engine->GetModule("console");
    if( mod == 0 || mod->GetFunctionCount() == 0 )
    {
        LISEM_ERROR("No functions have been added. ")
        return;
    }

    // trim the std::string to find the variable name
    size_t p1 = arg.find_first_not_of(" \n\r\t");
    if( p1 != std::string::npos )
        arg = arg.substr(p1, -1);
    size_t p2 = arg.find_last_not_of(" \n\r\t");
    if( p2 != std::string::npos )
        arg = arg.substr(0, p2+1);

    asIScriptFunction *func = mod->GetFunctionByName(arg.c_str());
    if( func )
    {
        mod->RemoveFunction(func);
        LISEM_ERROR("Function removed. ")
    }
    else
        LISEM_ERROR("No such function. ")

    // Since functions can be recursive, we'll call the garbage
    // collector to make sure the object is really freed
    engine->GarbageCollect();
}

void ListVariables(asIScriptEngine *engine)
{
    asUINT n;

    // List the application registered variables
    LISEM_DEBUG("Application variables:")
    for( n = 0; n < (asUINT)engine->GetGlobalPropertyCount(); n++ )
    {
        const char *name;
        int typeId;
        bool isConst;
        engine->GetGlobalPropertyByIndex(n, &name, 0, &typeId, &isConst);
        std::string decl = isConst ? " const " : " ";
        decl += engine->GetTypeDeclaration(typeId);
        decl += " ";
        decl += name;
        //decl << std::endl;
    }

    // List the user variables in the module
    asIScriptModule *mod = engine->GetModule("console");
    if( mod )
    {
        LISEM_DEBUG("User variables:")
        for( n = 0; n < (asUINT)mod->GetGlobalVarCount(); n++ )
        {
            LISEM_DEBUG(QString(" ") +mod->GetGlobalVarDeclaration(n));
        }
    }
}

void ListFunctions(asIScriptEngine *engine)
{
    asUINT n;

    // List the application registered functions
    LISEM_DEBUG("Application functions:")
    for( n = 0; n < (asUINT)engine->GetGlobalFunctionCount(); n++ )
    {
        asIScriptFunction *func = engine->GetGlobalFunctionByIndex(n);

        // Skip the functions that start with _ as these are not meant to be called explicitly by the user
        if( func->GetName()[0] != '_' )
            LISEM_DEBUG(QString(" ")+func->GetDeclaration());
    }

    // List the user functions in the module
    asIScriptModule *mod = engine->GetModule("console");
    if( mod )
    {

        LISEM_DEBUG("User functions:")
        for( n = 0; n < (asUINT)mod->GetFunctionCount(); n++ )
        {
            asIScriptFunction *func = mod->GetFunctionByIndex(n);
            LISEM_DEBUG(func->GetDeclaration());
        }
    }
}

void PrintHelp()
{
    LISEM_DEBUG("Commands:");
    LISEM_DEBUG(" addfunc [decl] - adds a user function");
    LISEM_DEBUG(" addvar [decl]  - adds a user variable");
    LISEM_DEBUG(" delfunc [name] - removes a user function");
    LISEM_DEBUG(" delvar [name]  - removes a user variable");
    LISEM_DEBUG(" exec [script]  - executes script statement and prints the result");
    LISEM_DEBUG(" help           - this command");
    LISEM_DEBUG(" listfuncs      - list functions");
    LISEM_DEBUG(" listvars       - list variables");
}

void grab(int v)
{
    //cout << v << endl;
}

void grab(asUINT v)
{
    //cout << v << endl;
}

void grab(bool v)
{
    //cout << boolalpha << v << endl;
}

void grab(float v)
{
    //cout << v << endl;
}

void grab(double v)
{
    //cout << v << endl;
}

void grab(const QString &v)
{
    //cout << v << endl;
}

void grab()
{
    // There is no value
}

void print(const QString &s)
{
   LISEM_DEBUG(QString(s));
   LISEMS_SCRIPTOUTPUT(QString(s));
}

void print(const LSMVector3 &s)
{
   LISEM_DEBUG("("+QString::number(s.x)+"," + QString::number(s.y)+ "," + QString::number(s.z)+")");
   LISEMS_SCRIPTOUTPUT("("+QString::number(s.x)+"," + QString::number(s.y)+ "," + QString::number(s.z)+")");
}


void print(const double &s)
{
   LISEM_DEBUG(QString::number(s));
   LISEMS_SCRIPTOUTPUT(QString::number(s));
}


void print(const int &s)
{
   LISEM_DEBUG(QString::number(s));
   LISEMS_SCRIPTOUTPUT(QString::number(s));
}


QString ToString(float x)
{
    return QString::number(x,'g',8);
}

QString ToString(double x)
{
    return QString::number(x,'g',17);
}

QString ToString(int x)
{
    return QString::number(x);
}

QString ToString(bool x)
{
    QString s;

    if(x)
    {
        s = "true";
    }else {
        s = "false";
    }

    return s;
}


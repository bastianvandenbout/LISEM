#pragma once

#include "defines.h"
#include <QString>

#include "error.h"

#include "angelscript.h"
#include "extensions/scriptqstdstring.h"
#include "scripthelper.h"
#include "OpenCLUtil.h"
#include "QThread"
#include <QWaitCondition>
#include "scriptbuilder.h"
#include <QRegularExpression>
#include "scriptpreprocessor.h"
#include "lsmscriptengine.h"
#include "linear/lsm_vector3.h"
#include "opencv2/core.hpp"
#include "qfileinfo.h"

LISEM_API void MessageCallback(const asSMessageInfo *msg, void *param);
LISEM_API void ExecString(asIScriptEngine *engine, std::string &arg);
LISEM_API void AddVariable(asIScriptEngine *engine, std::string &arg);
LISEM_API void DeleteVariable(asIScriptEngine *engine, std::string &arg);
LISEM_API void AddFunction(asIScriptEngine *engine, std::string &arg);
LISEM_API void DeleteFunction(asIScriptEngine *engine, std::string &arg);
LISEM_API void ListVariables(asIScriptEngine *engine);
LISEM_API void ListFunctions(asIScriptEngine *engine);
LISEM_API void PrintHelp();
LISEM_API void grab(int v);
LISEM_API void grab(asUINT v);
LISEM_API void grab(bool v);
LISEM_API void grab(float v);
LISEM_API void grab(double v);
LISEM_API void grab(const QString &v);
LISEM_API void grab();
LISEM_API void print(const QString &s);
LISEM_API void print(const double &s);
LISEM_API void print(const int &s);
LISEM_API void print(const LSMVector3 &s);
LISEM_API QString ToString(float x);
LISEM_API QString ToString(double x);
LISEM_API QString ToString(int x);
LISEM_API QString ToString(bool x);

static void TranslateException(asIScriptContext *ctx, void* /*userParam*/)
{
    std::cout << "exception from script " << std::endl;
  try
  {
    // Retrow the original exception so we can catch it again
    throw;
  }
  catch( std::exception &e )
  {
    // Tell the VM the type of exception that occurred
    LISEMS_ERROR("Exception from std library: " +QString(e.what()));
    LISEM_ERROR("Exception from std library: " +QString(e.what()));
  }catch(cv::Exception &e)
  {
        LISEMS_ERROR("Exception from opencv library: " +QString(e.msg.c_str()));
        LISEM_ERROR("Exception from opencv library: " +QString(e.msg.c_str()));
  }
  catch(...)
  {
    // The callback must not allow any exception to be thrown, but it is not necessary
    // to explicitly set an exception string if the default exception string is sufficient
  }
}
typedef struct SPHScript
{
    QString file;
    QString name;
    QString code;
    QString HomeDir;
    bool is_singleline = false;
    bool is_preprocess = false;
    bool is_allowinclude = false;
    bool compiled = false;

    CScriptBuilder * scriptbuilder;

    std::function<void(bool)> m_CallBackDone;
    std::function<void(asIScriptContext *)> m_CallBackLine;
    std::function<void(const asSMessageInfo *)> m_CallBackCompileError;
    std::function<void(asIScriptContext *)> m_CallBackException;
    std::function<void(QString)> m_CallBackPrint;

    bool m_CallBackDoneSet = false;
    bool m_CallBackLineSet = false;
    bool m_CallBackCompileErrorSet = false;
    bool m_CallBackExceptionSet = false;
    bool m_CallBackPrintSet = false;

    inline void SetCode(QString code_in)
    {
        code= code_in;
    }

    inline void SetFileOrigin(QString path)
    {
        file = path;
    }

    inline void SetSingleLine(bool x)
    {
        is_singleline = x;
    }

    inline void SetHomeDir(QString dir)
    {
        HomeDir = dir;
    }

    inline void SetAllowInclude(bool x)
    {
        is_allowinclude = x;
    }

    inline void SetPreProcess(bool x)
    {
        is_preprocess = x;
    }
    inline bool IsCompiled()
    {
        return compiled;

    }


    //example usage, either class member function, bound to this and placeholder, or lambda function

    //s->SetCallBackPrint(&DatabaseTool::OnScriptCallBackPrint,this,std::placeholders::_1);
    //s->SetCallBackPrint(std::function<void(SPHScript*,QString)>([=](SPHScript*,QString) -> void{  ;}),s,std::placeholders::_1);

    template<typename _Callable, typename... _Args>
    inline void SetCallBackPrint(_Callable&& __f, _Args&&... __args)
    {
        m_CallBackPrint = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...);
        m_CallBackPrintSet = true;
    }

    template<typename _Callable, typename... _Args>
    inline void SetCallBackDone(_Callable&& __f, _Args&&... __args)
    {
        m_CallBackDone = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...);
        m_CallBackDoneSet = true;

    }

    template<typename _Callable, typename... _Args>
    inline void SetCallBackLine(_Callable&& __f, _Args&&... __args)
    {
        m_CallBackLine = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...);
        m_CallBackLineSet = true;
    }

    template<typename _Callable, typename... _Args>
    inline void SetCallBackCompilerError(_Callable&& __f, _Args&&... __args)
    {
        m_CallBackCompileError = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...);
        m_CallBackCompileErrorSet = true;
    }

    template<typename _Callable, typename... _Args>
    inline void SetCallBackException(_Callable&& __f, _Args&&... __args)
    {
        m_CallBackException = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...);
        m_CallBackExceptionSet = true;
    }


} SPHScript;


struct SPHCodeJob
{
    SPHScript * program;


    inline void OnCallBackLine(asIScriptContext *ctx)
    {
        if(program->m_CallBackLineSet)
        {
            program->m_CallBackLine(ctx);

        }
    }

    inline void OnCallBackException(asIScriptContext *ctx)
    {
        if(program->m_CallBackExceptionSet)
        {
            program->m_CallBackException(ctx);
        }
    }


    inline void OnCallBackCompileError(const asSMessageInfo *msg)
    {
        if(program->m_CallBackCompileErrorSet)
        {
            program->m_CallBackCompileError(msg);
        }

    }

};




class LISEM_API ScriptManager : public QThread
{
    Q_OBJECT

public:
    ScriptManager();

    QMutex m_StartWorkMutex;
    QWaitCondition m_StartWorkWaitCondition;

    QMutex m_WorkMutex;
    QList<SPHCodeJob> m_Work;

    QMutex m_CompileMutex;
    SPHScript * m_CurrentCompilation = nullptr;

    QMutex m_StopMutex;
    bool m_StopRequested = false;

    LSMScriptEngine *m_Engine;

    void Initialize();
    void run() override;

    inline static int PragmaCallback(const std::string &pragmaText, CScriptBuilder &builder, void *userParam)
    {
        return 0;
    }

    inline static int IncludeCallback(const char *include, const char *from, CScriptBuilder *builder, void *userParam)
    {

        std::cout << "include additional script1 " <<  include << std::endl;


        std::string path = from;
        size_t posOfSlash = path.find_last_of("/\\");
        if( posOfSlash != std::string::npos )
        {
            path.resize(posOfSlash+1);
        }
        else
        {

            path = "";
        }

        std::string includesn = std::string(include);

        // If the include is a relative path, then prepend the path of the originating script
        if( includesn.find_first_of("/\\") != 0 &&
            includesn.find_first_of(":") == std::string::npos )
        {
            includesn = path + std::string(include);

            QFileInfo f(QString(includesn.c_str()));
            if(f.exists())
            {
                includesn = path + std::string(include);
            }else
            {
                includesn = path + std::string(include);

            }
        }

        QFileInfo f(QString(includesn.c_str()));
        if(f.exists())
        {

            std::cout << "include additional script " <<  includesn << std::endl;
            builder->AddSectionFromFile(includesn.c_str());
        }else
        {

        }

        return 0;
    }

    inline static int IncludeIgnoreCallback(const char *include, const char *from, CScriptBuilder *builder, void *userParam)
    {

        return 0;
    }

    inline QStringList GetGlobalFunctionNames()
    {
        QStringList ret;

        QList<ScriptFunctionInfo> list = m_Engine->GetGlobalFunctionList();

        for(int i =0; i < list.length(); i++)
        {
            ret.append(list.at(i).Function->GetName());
        }
        return ret;
    }

    inline SPHScript * CompileScriptTable(QString script)
    {
        SPHScript * s = new SPHScript();
        s->file= "";

            QString funcCode = "Table TableEdit(Table &t) {\n";
            funcCode += script;
            funcCode += ";\nreturn t;\n}";
            script = funcCode;

        s->code = script;
        s->is_singleline = true;


        s->scriptbuilder = new CScriptBuilder();
        int r = s->scriptbuilder->StartNewModule(this->m_Engine,"SPHTableScript");
        if(r < 0)
        {
            LISEMS_DEBUG("Could not create new module for script!");
            LISEM_DEBUG("Could not create new module for script!");
            s->compiled = false;
            return s;
        }
        s->scriptbuilder->SetPragmaCallback(ScriptManager::PragmaCallback,(void*)s);
        s->scriptbuilder->SetIncludeCallback(ScriptManager::IncludeCallback,(void*)s);

        r = s->scriptbuilder->AddSectionFromMemory("main",script.toStdString().c_str());

        LISEM_DEBUG(script);

        if(r < 0)
        {
            LISEM_DEBUG("Script compilation error!");
            LISEMS_DEBUG("Script compilation error!");
            s->compiled = false;
            return s;
        }

        r = s->scriptbuilder->BuildModule();
        if(r < 0)
        {
            LISEM_DEBUG("Script compilation error!");
            LISEMS_DEBUG("Script compilation error!");
            s->compiled = false;
            return s;
        }

        asIScriptFunction *func = s->scriptbuilder->GetModule()->GetFunctionByDecl("Table TableEdit(Table &t) ");
        if(func == 0)
        {
            LISEM_DEBUG("Could not find entry point: Table TableEdit(Table &t)");
            LISEMS_DEBUG("Could not find entry point: Table TableEdit(Table &t)");
            s->compiled = false;
            return s;
        }

        s->compiled = true;

        return s;

    }

    inline SPHScript * CompileScriptNode(QString script)
    {
        SPHScript * s = new SPHScript();
        s->file= "";


        s->code = script;
        s->is_singleline = true;


        s->scriptbuilder = new CScriptBuilder();
        int r = s->scriptbuilder->StartNewModule(this->m_Engine,"SPHNode");
        if(r < 0)
        {
            LISEMS_DEBUG("Could not create new module for script!");
            LISEM_DEBUG("Could not create new module for script!");
            s->compiled = false;
            return s;
        }
        s->scriptbuilder->SetPragmaCallback(ScriptManager::PragmaCallback,(void*)s);
        s->scriptbuilder->SetIncludeCallback(ScriptManager::IncludeCallback,(void*)s);

        r = s->scriptbuilder->AddSectionFromMemory("main",script.toStdString().c_str());

        LISEM_DEBUG(script);

        if(r < 0)
        {
            LISEM_DEBUG("Script compilation error!");
            LISEMS_DEBUG("Script compilation error!");
            s->compiled = false;
            return s;
        }

        r = s->scriptbuilder->BuildModule();
        if(r < 0)
        {
            LISEM_DEBUG("Script compilation error!");
            LISEMS_DEBUG("Script compilation error!");
            s->compiled = false;
            return s;
        }

        s->compiled = true;

        return s;

    }

    inline SPHScript * CompileScript(QString script, bool is_singleline = false, bool preprocess = false, QString HomeDir = "")
    {
        SPHScript * s = new SPHScript();
        s->SetFileOrigin("");
        s->SetHomeDir(HomeDir);
        s->SetSingleLine(is_singleline);
        s->SetPreProcess(preprocess);
        s->SetCode(script);


        return CompileScript_Generic(s);

    }

    inline SPHScript * CompileScript_Generic(SPHScript * s)
    {
        if(s->code.isEmpty())
        {
            s->compiled = false;
            return s;
        }

        if(s->is_singleline)
        {
            QString funcCode = " void main() {\n";
            funcCode += s->code;
            funcCode += "\n;}";
            s->code = funcCode;

        }

        if(s->is_preprocess)
        {
            s->code = PreProcessScript(s->code);

        }


        s->scriptbuilder = new CScriptBuilder();
        int r = s->scriptbuilder->StartNewModule(this->m_Engine,"SPHScript");
        if(r < 0)
        {
            LISEMS_DEBUG("Could not create new module for script!");
            LISEM_DEBUG("Could not create new module for script!");
            s->compiled = false;
            return s;
        }


        s->scriptbuilder->SetPragmaCallback(ScriptManager::PragmaCallback,(void*)s);
        if(s->is_allowinclude)
        {
            s->scriptbuilder->SetIncludeCallback(ScriptManager::IncludeCallback,(void*)s);
        }else {
            s->scriptbuilder->SetIncludeCallback(ScriptManager::IncludeIgnoreCallback,(void*)s);
        }

        m_CompileMutex.lock();
        m_CurrentCompilation = s;

        r = s->scriptbuilder->AddSectionFromMemory("main",s->code.toStdString().c_str());

        LISEM_DEBUG(s->code);

        if(r < 0)
        {
            m_CurrentCompilation = nullptr;
            m_CompileMutex.unlock();
            s->compiled = false;
            return s;
        }

        r = s->scriptbuilder->BuildModule();

        m_CurrentCompilation = nullptr;
        m_CompileMutex.unlock();


        if(r < 0)
        {
            s->compiled = false;
            return s;
        }

        asIScriptFunction *func = s->scriptbuilder->GetModule()->GetFunctionByDecl("void main()");
        if(func == 0)
        {
            LISEM_DEBUG("Could not find entry point: void main()");
            LISEMS_DEBUG("Could not find entry point: void main()");
            s->compiled = false;
            return s;
        }

        s->compiled = true;

        return s;

    }

    template<typename _Callable, typename... _Args>
    inline void RunScript(SPHScript * s,_Callable&& __f, _Args&&... __args)
    {
        SPHCodeJob cj;

        cj.program = s;

        cj.program->m_CallBackDone = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...);
        cj.program->m_CallBackDoneSet = true;



        m_StartWorkMutex.lock();

        m_Work.append(cj);
        m_StartWorkMutex.unlock();
        m_StartWorkWaitCondition.notify_all();
    }

    inline void RunScript(SPHScript * s)
    {

        SPHCodeJob cj;
        cj.program = s;

        m_StartWorkMutex.lock();

        m_Work.append(cj);
        m_StartWorkMutex.unlock();
        m_StartWorkWaitCondition.notify_all();
    }

    inline void RequestStop()
    {
        m_StopMutex.lock();
        m_StopRequested = true;
        m_StopMutex.unlock();
    }


    inline void RunCommand(QString name)
    {
        std::string cmd, arg;
        std::string input = name.toStdString();

        size_t pos;
        if( (pos = input.find(" ")) != std::string::npos )
        {
            cmd = input.substr(0, pos);
            arg = input.substr(pos+1);
        }
        else
        {
            cmd = input;
            arg = "";
        }

        // Interpret the command
        if( cmd == "exec" )
        {
            ExecString(m_Engine, arg);
        }
        else if( cmd == "addfunc" )
        {
            AddFunction(m_Engine, arg);
        }
        else if( cmd == "delfunc" )
        {
            DeleteFunction(m_Engine, arg);
        }
        else if( cmd == "addvar" )
        {
            AddVariable(m_Engine, arg);
        }
        else if( cmd == "delvar" )
        {
            DeleteVariable(m_Engine, arg);
        }
        else if( cmd == "help" )
        {
            PrintHelp();
        }
        else if( cmd == "listfuncs" )
        {
            ListFunctions(m_Engine);
        }
        else if( cmd == "listvars" )
        {
            ListVariables(m_Engine);
        }
        else
        {
            LISEM_ERROR("Unknown command.");
        }

    }

    inline void Destroy()
    {
        if(this->isRunning())
        {
            this->RequestStop();
            this->m_StartWorkWaitCondition.notify_all();

            if(!this->wait(100))
            {
                this->terminate();
            }
        }

        m_Engine->ShutDownAndRelease();
        m_Engine = NULL;
    }


    QMutex m_IsScriptRuningMutex;
    bool m_IsScriptRunning;
    inline bool IsScriptRunning()
    {
        this->m_IsScriptRuningMutex.lock();
        bool ret = m_IsScriptRunning = false;
        this->m_IsScriptRuningMutex.unlock();

        return ret;
    }

};


LISEM_API extern ScriptManager * CScriptManager;
LISEM_API extern int CScriptCounter;
LISEM_API extern QMutex CScriptCounterMutex;

inline static void InitScriptManager(ScriptManager * m)
{
   CScriptManager = m;
}

inline static ScriptManager * GetScriptManager()
{
    return CScriptManager;

}

inline void IncrementScriptCounter()
{
    CScriptCounterMutex.lock();
    CScriptCounter ++;
    CScriptCounterMutex.unlock();
}

inline int GetScriptCounter()
{
    int ret = 0;
    CScriptCounterMutex.lock();
    ret = CScriptCounter;
    CScriptCounterMutex.unlock();
    return ret;
}

inline static void DestroyScriptManager()
{
    if(CScriptManager != nullptr)
    {
        CScriptManager->Destroy();
        delete CScriptManager;
        CScriptManager = nullptr;

    };
}

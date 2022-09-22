#ifndef SCRIPTANALYZER_H
#define SCRIPTANALYZER_H

#include "scriptmanager.h"
#include "extensionprovider.h"




typedef struct ScriptToken
{
    QString text;
    asETokenClass tokenclass;
    int pos = 0;
    int length = 0;
    int nesting = 0;
    QString nspace;
    QString name;
    QString memberclass;
    QString Type;
    QList<QString> ParamTypes;
    QList<QString> ParamNames;
    bool is_function = false;
    bool is_variable = false;
    bool is_class = false;

} ScriptToken;

typedef struct ScriptScope
{
    int uid = 0;
    int pos_begin = 0;
    int pos_end = 0;
    int level = 0;
    QString type = "";
    QString bracket = "{";

} ScriptScope;

typedef struct ScriptLocation
{
    int line_n = 0;
    int col_n = 0;
    int pos = 0;

} ScriptLocation;

class ScriptAnalyzer
{
    ScriptManager * m_ScriptManager;
    LSMScriptEngine * m_ScriptEngine;


    QString m_CurrentDir;

    QStringList m_classes;
    QStringList m_classesScript;
    QList<ScriptFunctionInfo> funclist;
    QList<ScriptFunctionInfo> funcmemberlist;




    QList<ScriptScope> Scopes;
    QList<ScriptToken> tokens;
    QList<ScriptToken> ClassTokens;
    QList<ScriptToken> VariableTokens;
    QList<ScriptToken> FunctionTokens;

    QString m_Script;



public:
    inline ScriptAnalyzer(ScriptManager * engine)
    {
        m_ScriptManager = engine;
        m_ScriptEngine = m_ScriptManager->m_Engine;

        QStringList functionnames = m_ScriptManager->GetGlobalFunctionNames();

        funclist = m_ScriptManager->m_Engine->GetGlobalFunctionList();
        funcmemberlist = m_ScriptManager->m_Engine->GetMemberFunctionList();

        m_classes.push_back("int8");
        m_classes.push_back("int16");
        m_classes.push_back("int");
        m_classes.push_back("int64");
        m_classes.push_back("uint8");
        m_classes.push_back("uint16");
        m_classes.push_back("uint");
        m_classes.push_back("uint64");
        m_classes.push_back("float");
        m_classes.push_back("double");
    }

    void AnalyzeScript(QString qscript);
    inline void FindErrors();
    inline ScriptLocation GetLocationFromPosition(int pos);

};




#endif // SCRIPTANALYZER_H

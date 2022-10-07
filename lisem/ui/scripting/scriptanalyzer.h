#ifndef SCRIPTANALYZER_H
#define SCRIPTANALYZER_H

#include "scriptmanager.h"
#include "extensionprovider.h"





typedef struct ScriptScope
{
    int uid = 0;
    int pos_begin = 0;
    int pos_end = 0;
    int level = 0;
    QString type = "";
    QString bracket = "{";
    ScriptScope * parent = nullptr;

} ScriptScope;

typedef struct ScriptToken
{
    QString text;
    asETokenClass tokenclass;
    int pos = 0;
    int length = 0;
    int nesting = 0;
    ScriptScope * scope = nullptr;
    QString nspace;
    QString name;
    QString memberclass;
    QString Type;
    QList<QString> ParamTypes;
    QList<QString> ParamNames;
    bool is_function = false;
    bool is_variable = false;
    bool is_class = false;
    bool is_template =false;
    int n_templateargs = 0;
    asETokenClass TokenType = asTC_UNKNOWN;


} ScriptToken;


typedef struct ScriptExpressionType
{

    ScriptToken definition;
    QString type_full;
    QString type;
    QString name;
    QString classname;
    std::vector<QString> template_types;
    bool is_namespace = false;
    bool is_class = false;
    bool is_interface = false;
    bool is_enum = false;
    bool is_templated = false;
    bool is_function = false;
    bool is_member = false;
    bool is_object = false;
    bool is_cpp = false;
    bool is_unknown = true;
    bool is_empty = false;

    ScriptToken def;

    inline static ScriptExpressionType FromNameSpace(QString name)
    {
        ScriptExpressionType t;
        t.is_empty = false;
        t.is_unknown = false;
        t.is_namespace = true;
        t.name = name;

        return t;

    }

    inline static ScriptExpressionType FromLocalFunction(QString name, QString type)
    {
        ScriptExpressionType t;
        t.is_empty = false;
        t.is_unknown = false;
        t.is_cpp = false;
        t.is_function = true;
        t.is_class = false;
        t.name = name;
        t.type = type;
        t.type_full = type;
        return t;
    }

    inline static ScriptExpressionType FromCppType(QString name, QString type)
    {
        ScriptExpressionType t;
        t.is_empty = false;
        t.is_unknown = false;
        t.is_cpp = true;
        t.is_class = true;
        t.name = name;
        t.type = type;
        t.type_full = type;
        return t;
    }
    inline static ScriptExpressionType FromCppFunctionReturn(QString funcname, QString returntype, bool templated = false, std::vector<QString> temptypes = {}, bool ismember= false, QString parent_type = "")
    {
        ScriptExpressionType t;
        t.is_empty = false;
        t.is_unknown = false;
        t.is_cpp = true;
        t.is_class = false;
        t.is_function = true;
        t.is_member = ismember;
        t.is_templated = templated;
        t.template_types = temptypes;
        t.name = funcname;
        t.type = returntype;
        t.type_full = returntype;
        return t;
    }

    inline static ScriptExpressionType FromFunctionReturn(QString funcname, QString returntype, bool templated = false, std::vector<QString> temptypes = {}, bool ismember= false, QString parent_type = "")
    {
        ScriptExpressionType t;
        t.is_empty = false;
        t.is_unknown = false;
        t.is_cpp = false;
        t.is_class = false;
        t.is_function = true;
        t.is_member = ismember;
        t.is_templated = templated;
        t.template_types = temptypes;
        t.name = funcname;
        t.type = returntype;
        t.type_full = returntype;
        return t;
    }

    inline static ScriptExpressionType FromType(QString funcname, QString returntype, bool templated = false, std::vector<QString> temptypes = {}, bool ismember= false, QString parent_type = "", ScriptToken ti = ScriptToken())
    {
        ScriptExpressionType t;
        t.is_empty = false;
        t.is_unknown = false;
        t.is_cpp = false;
        t.is_class = false;
        t.is_object = true;
        t.is_member = ismember;
        t.definition = ti;
        t.is_templated = templated;
        t.template_types = temptypes;
        t.name = funcname;
        t.type = returntype;
        t.type_full = returntype;
        return t;
    }


    inline bool operator==(const ScriptExpressionType &t)
    {
        if(is_unknown || t.is_unknown)
        {
            return false;
        }
        if(is_empty && t.is_empty)
        {
            return true;
        }
        if(is_class && t.is_class)
        {
            if(t.name == name)
            {
                return true;
            }
        }
        if(is_function && t.is_function)
        {
            if(is_cpp && t.is_cpp)
            {
                if(is_member && t.is_member)
                {
                    if(t.name == name)
                    {
                        if(t.classname == classname)
                        {
                            return true;
                        }
                    }
                }else if(!is_member && !t.is_member)
                {
                    if(t.name == name)
                    {
                        return true;
                    }
                }
            }else if(!is_cpp && !t.is_cpp)
            {
                if(is_member && t.is_member)
                {
                    if(t.name == name)
                    {
                        if(t.classname == classname)
                        {
                            if(t.def.pos == def.pos)
                            {
                                return true;

                            }
                        }
                    }
                }else if(!is_member && !t.is_member)
                {
                    if(t.name == name)
                    {
                        return true;
                    }
                }
            }
        }
        if(is_object && t.is_object)
        {
            if(t.name == name)
            {
                if(t.def.pos == def.pos)
                {
                    return true;
                }
            }
        }

        if(is_enum && t.is_enum)
        {
            if(t.name == name)
            {
                return true;
            }

        }

        if(is_interface && t.is_interface)
        {
            if(t.name == name)
            {
                return true;
            }
        }

        if(is_namespace && t.is_namespace)
        {
            if(t.name == name)
            {
                return true;
            }
        }

        return false;

    }

} ScriptExpressionType;

typedef struct ScriptExpressionTypeSequence
{
    std::vector<ScriptExpressionType> m_ExpressionTypes;

    inline static ScriptExpressionTypeSequence FromType(ScriptExpressionType t)
    {
        ScriptExpressionTypeSequence ret;
        ret.m_ExpressionTypes.push_back(t);
        return ret;

    }

    inline static ScriptExpressionTypeSequence Unknown()
    {
        ScriptExpressionTypeSequence ret;
        ScriptExpressionType t;
        t.is_unknown = true;
        ret.m_ExpressionTypes.push_back(t);
        return ret;

    }


    ScriptExpressionTypeSequence Appended(ScriptExpressionType t)
    {

        ScriptExpressionTypeSequence ret;
        ret.m_ExpressionTypes = m_ExpressionTypes;
        ret.m_ExpressionTypes.push_back(t);
        return ret;

    }

    inline ScriptExpressionType GetLast() const
    {
        if(m_ExpressionTypes.size() > 0)
        {
            return m_ExpressionTypes.at(m_ExpressionTypes.size()-1);

        }else
        {
            ScriptExpressionType t;
            t.is_empty = true;
            t.is_unknown = false;
            return t;
        }
    }

    inline QString GetNameSpaceFull() const
    {
        int i = m_ExpressionTypes.size()-1;
        for(; i > -1; i --)
        {
            if(m_ExpressionTypes.at(i).is_namespace == true)
            {

            }else
            {
                break;
            }
        }

        QString namespace_full = "";

        for(; i < m_ExpressionTypes.size(); i++)
        {
            namespace_full += "::" + m_ExpressionTypes.at(i).name;
        }

        return namespace_full;
    }


} ScriptExpressionTypeSequence;

typedef struct ScriptReferenceResult
{
    QString name;
    QString expression;
    QString line;
    int pos = 0;
    int col_n = 0;
    int row_n = 0;
} ScriptReferenceResult;

typedef struct ScriptLocation
{
    int line_n = 0;
    int col_n = 0;
    int pos = 0;

} ScriptLocation;

typedef struct AutoCompleteOption
{
    QString complete;
    QString name;

} AutoCompleteOption;


class ScriptAnalyzer
{
    ScriptManager * m_ScriptManager;
    LSMScriptEngine * m_ScriptEngine;


    QString m_CurrentDir;

    QStringList m_classes;
    QStringList m_classesScript;
    QList<ScriptFunctionInfo> funclist;
    QList<ScriptFunctionInfo> funcmemberlist;
    QList<ScriptClassInfo> classlist;
    QList<ScriptClassPropertyInfo> classpropertylist;

    QList<QList<QString>> m_FileExtensionsList;
    QList<QString> m_FileTypesList;


    QList<ScriptScope*> Scopes;
    QList<ScriptToken> Tokens;
    QList<ScriptToken> ClassTokens;
    QList<ScriptToken> VariableTokens;
    QList<ScriptToken> FunctionTokens;

    QString m_Script;
    QList<QString> m_Lines;


public:
    inline ScriptAnalyzer(ScriptManager * engine)
    {
        m_ScriptManager = engine;
        m_ScriptEngine = m_ScriptManager->m_Engine;

        QStringList functionnames = m_ScriptManager->GetGlobalFunctionNames();

        funclist = m_ScriptManager->m_Engine->GetGlobalFunctionList();
        funcmemberlist = m_ScriptManager->m_Engine->GetMemberFunctionList();
        //classlist = m_ScriptManager->m_Engine->();
        //classproplist = m_ScriptManager->m_Engine->();

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

        m_FileExtensionsList.append(GetMapExtensions());
        m_FileTypesList.append("Map");
        m_FileExtensionsList.append(GetShapeExtensions());
        m_FileTypesList.append("Shapes");
        m_FileExtensionsList.append(GetModelExtensions());
        m_FileTypesList.append("Object");
        m_FileExtensionsList.append(GetFieldExtensions());
        m_FileTypesList.append("Field");
        m_FileExtensionsList.append(GetPointCloudExtensions());
        m_FileTypesList.append("PointCloud");
        m_FileExtensionsList.append(GetTableExtensions());
        m_FileTypesList.append("Table");

    }

    inline bool InRange(int pos)
    {
        return pos > -1 && pos < m_Script.size();
    }
    bool ScopeContains(ScriptScope * s1, int pos);
    ScriptScope * GetScopeAtPos(int posin);
    void AnalyzeScript(QString qscript);
    std::vector<ScriptToken> GetTokenDefinitions(QString name);
    QString GetExpressionLeftFull(int pos);
    QString GetExpressionRightFull(int pos);
    ScriptExpressionTypeSequence GetExpressionTypeString(QString expr, int pos, int scriptpos, ScriptScope * p = nullptr, bool scope_set = false);
    ScriptExpressionType GetExpressionTypeFunction(ScriptExpressionTypeSequence parents,QString name, int scriptpos);
    ScriptExpressionType GetExpressionTypeVariable(ScriptExpressionTypeSequence parents,QString name, int scriptpos);

    ScriptExpressionType GetArithmeticStatementType(QString statement, int scriptpos);
    QString GetArithmeticOperatorType(QString type1, QString type2, QString oper, int scriptpos);

    void FindSomeErrors();
    std::vector<ScriptReferenceResult> GetReferencesTo(QString name, int pos);
    std::vector<AutoCompleteOption> GetAutoCompletes(QString name, int pos);

    ScriptLocation GetLocationFromPosition(int pos);

    QString GetCodeLine(int l);


};




#endif // SCRIPTANALYZER_H

#include "lsmscriptengine.h"

LSMScriptEngine::LSMScriptEngine(asIScriptEngine * ASEngine)
{
    m_ASEngine = ASEngine;
    m_ASEngine->SetUserData(this);

    m_FunctionDescriptions.clear();
    m_ParameterDescriptions.clear();
    m_ReturnDescriptions.clear();
    m_FunctionNames.clear();
    m_ParameterNames.clear();
    m_ReturnNames.clear();

    m_Operators["opAdd"] = "+";
    m_Operators["opSub"] = "-";
    m_Operators["opMul"] = "*";
    m_Operators["opDiv"] = "/";
    m_Operators["opMod"] = "%";
    m_Operators["opPow"] = "**";
    m_Operators["opAnd"] = "&";
    m_Operators["opOr"] = "|";
    m_Operators["opXor"] = "^";
    m_Operators["opShl"] = "<<";
    m_Operators["opShr"] = ">>";
    m_Operators["opUShr"] = ">>>";

    m_Operators["opAdd_r"] = "+";
    m_Operators["opSub_r"] = "-";
    m_Operators["opMul_r"] = "*";
    m_Operators["opDiv_r"] = "/";
    m_Operators["opMod_r"] = "%";
    m_Operators["opPow_r"] = "**";
    m_Operators["opAnd_r"] = "&";
    m_Operators["opOr_r"] = "|";
    m_Operators["opXor_r"] = "^";
    m_Operators["opShl_r"] = "<<";
    m_Operators["opShr_r"] = ">>";
    m_Operators["opUShr_r"] = ">>>";

    m_Operators["opCmp"] = "<";
    m_Operators["opCmp2"] = ">";
    m_Operators["opCmp3"] = "<=";
    m_Operators["opCmp4"] = ">=";

    m_Operators["opNeg"] = "-";
    m_Operators["opCom"] = "~";
    m_Operators["opEquals"] = "==";
    m_Operators["opEqualsnot"] = "!=";

    m_OperatorNames["opAdd"] = "Add";
    m_OperatorNames["opSub"] = "Subtract";
    m_OperatorNames["opMul"] = "Multiply";
    m_OperatorNames["opDiv"] = "Divide";
    m_OperatorNames["opMod"] = "Modulus";
    m_OperatorNames["opPow"] = "Power";
    m_OperatorNames["opAnd"] = "AND";
    m_OperatorNames["opOr"] = "OR";
    m_OperatorNames["opXor"] = "XOR";
    m_OperatorNames["opShl"] = "Smaller then";
    m_OperatorNames["opShr"] = "Larger then";
    m_OperatorNames["opUShr"] = "";

    m_OperatorNames["opAdd_r"] = "Add";
    m_OperatorNames["opSub_r"] = "Subtract";
    m_OperatorNames["opMul_r"] = "Multiply";
    m_OperatorNames["opDiv_r"] = "Divide";
    m_OperatorNames["opMod_r"] = "Modulus";
    m_OperatorNames["opPow_r"] = "Power";
    m_OperatorNames["opAnd_r"] = "AND";
    m_OperatorNames["opOr_r"] = "OR";
    m_OperatorNames["opXor_r"] = "XOR";
    m_OperatorNames["opShl_r"] = "Smaller then";
    m_OperatorNames["opShr_r"] = "Bigger then";
    m_OperatorNames["opUShr_r"] = "";

    m_OperatorNames["opCmp"] = "Smaller then";
    m_OperatorNames["opCmp2"] = "Greater then";
    m_OperatorNames["opCmp3"] = "Smaller then or equal to";
    m_OperatorNames["opCmp4"] = "Greater then or equal to";

    m_OperatorNames["opNeg"] = "Negate";
    m_OperatorNames["opCom"] = "Complement";
    m_OperatorNames["opEquals"] = "Equal to";
    m_OperatorNames["opEqualsnot"] = "Not equal to";
}

LSMScriptEngine::~LSMScriptEngine()
{



}


void LSMScriptEngine::RegisterFunctionDescriptions(QStringList file)
{
    QStringList empty;
    QList<QString> parameternames;
    QList<QString> parameterdescriptions;
    QString s_multi;
    bool first_found = false;
    bool element_is_function = false;
    bool element_is_return = false;
    bool element_old_is_function = false;
    bool element_old_is_return = false;
    bool first_function = true;
    bool had_return = false;
    for(int i = 0;i < file.length(); i++)
    {
        QString s = file.at(i).trimmed();
        bool new_element= false;



        if(s.startsWith("#")) //new function description
        {
            QString name = s;
            int indexofpar = s.indexOf('(');
            if(indexofpar != -1)
            {
                name = name.mid(0,indexofpar);
                int indexofparend = s.indexOf(')',indexofpar+1);
                if(indexofparend > indexofpar)
                {
                    QString parameters = s.mid(indexofpar+1,indexofparend-indexofpar-1);
                    QStringList partypes = parameters.split(",");
                    m_ParameterTypes.append(partypes);
                }else {
                    m_ParameterTypes.append(empty);
                }
            }else {
                m_ParameterTypes.append(empty);
            }

            m_FunctionNames.append(name.right(name.length()-1));
            m_DescriptionMap.insertMulti(name.right(name.length()-1),m_FunctionNames.length()-1);

            new_element = true;
            element_old_is_return = element_is_return;
            element_old_is_function = element_is_function;
            element_is_function = true;
            element_is_return = false;
            had_return = false;

        }else if(s.startsWith("@")) //new parameter description
        {
            if(s.startsWith("@return")) //new parameter description
            {
                m_ReturnNames.append(s.right(s.length()-7));

                new_element = true;

                element_old_is_return = element_is_return;
                element_old_is_function = element_is_function;
                element_is_function = false;
                element_is_return = true;

                had_return = true;

                first_found = true;
            }else if(s.startsWith("@param"))
            {
                parameternames.append(s.right(s.length()-7));
                new_element = true;

                element_old_is_return = element_is_return;
                element_old_is_function = element_is_function;
                element_is_function = false;
                element_is_return = false;

                first_found = true;
            }

        }else //add to current object
        {
            s_multi.append(s + " ");
        }

        if(new_element || i == file.length()-1)
        {
            //if( first_found )
            {
                if(element_old_is_return)
                {
                    m_ReturnDescriptions.append(s_multi);
                }else if(element_old_is_function)
                {
                    m_FunctionDescriptions.append(s_multi);
                }else {
                    parameterdescriptions.append(s_multi);
                }
            }

            s_multi.clear();
        }


        if((s.startsWith("#") && !first_function)|| i == file.length()-1)
        {
            //store parameter stuff
            m_ParameterNames.append(parameternames);
            m_ParameterDescriptions.append(parameterdescriptions);

            if(!had_return)
            {
                m_ReturnNames.append("Void");
                m_ReturnDescriptions.append("Void");
            }
            parameternames.clear();
            parameterdescriptions.clear();
        }

        if((s.startsWith("#") && first_function))
        {
            first_function = false;
        }
    }
}


void LSMScriptEngine::RegisterBaseTypeDebugFunctions()
{

    RegisterTypeDebugTextConversion(asTYPEID_BOOL,[](void*par){return (*((bool*)par) == false) ? QString("false"):QString("true");});
    RegisterTypeDebugTextConversion(asTYPEID_INT8,[](void*par){return QString::number(*((int8_t*)par));});
    RegisterTypeDebugTextConversion(asTYPEID_INT16,[](void*par){return QString::number(*((int16_t*)par));});
    RegisterTypeDebugTextConversion(asTYPEID_INT32,[](void*par){return QString::number(*((int32_t*)par));});
    RegisterTypeDebugTextConversion(asTYPEID_INT64,[](void*par){return QString::number(*((int64_t*)par));});
    RegisterTypeDebugTextConversion(asTYPEID_UINT8,[](void*par){return QString::number(*((uint8_t*)par));});
    RegisterTypeDebugTextConversion(asTYPEID_UINT16,[](void*par){return QString::number(*((uint16_t*)par));});
    RegisterTypeDebugTextConversion(asTYPEID_UINT32,[](void*par){return QString::number(*((uint32_t*)par));});
    RegisterTypeDebugTextConversion(asTYPEID_UINT64,[](void*par){return QString::number(*((uint64_t*)par));});
    RegisterTypeDebugTextConversion(asTYPEID_FLOAT,[](void*par){return QString::number(*((float*)par));});
    RegisterTypeDebugTextConversion(asTYPEID_DOUBLE,[](void*par){return QString::number(*((double*)par));});



}



QList<ScriptFunctionInfo> LSMScriptEngine::GetGlobalFunctionList()
{

    QList<ScriptFunctionInfo> ret;
    for( int n = 0; n < (asUINT)m_ASEngine->GetGlobalFunctionCount(); n++ )
    {
        ScriptFunctionInfo fi;
        asIScriptFunction *func = m_ASEngine->GetGlobalFunctionByIndex(n);
        fi.Function = func;

        // Skip the functions that start with _ as these are not meant to be called explicitly by the user
        QString name = func->GetName();

        if( name[0] != '_' )

        {   int index_final = -1;
            //func function description
            QList<int> indices = m_DescriptionMap.values(name);

            for(int i = 0; i < indices.length(); i++)
            {
                bool found_exact = false;
                int index = indices.at(i);
                if(m_FunctionNames.at(index) == name)
                {
                    if(m_ParameterTypes.at(index).length() > 0)
                    {
                        bool parameters_same = true;
                        if(m_ParameterTypes.at(index).length() == func->GetParamCount())
                        {
                            for(int j = 0; j < m_ParameterTypes.at(index).length(); j++)
                            {
                                int Typeid = 0;
                                func->GetParam(j,&Typeid);
                                QString Typename;
                                asITypeInfo * ti = GetTypeInfoById(Typeid);
                                if(ti != nullptr)
                                {
                                    Typename = QString(ti->GetName());
                                }else
                                {
                                    Typename = GetBaseTypeName(Typeid);
                                }

                                if(!(m_ParameterTypes.at(index).at(j).compare(Typename,Qt::CaseInsensitive) == 0))
                                {
                                    parameters_same = false;
                                    break;
                                }

                            }
                        }else {
                            parameters_same = false;
                        }


                        if(parameters_same)
                        {
                            found_exact = true;
                            index_final = index;
                        }
                    }else {
                        index_final = index;
                    }
                }
                if(found_exact == true)
                {
                    break;
                }
            }

            //if we found a function description
            if(index_final >= 0)
            {
                if(index_final < m_ParameterNames.length())
                {
                    fi.Parameters = m_ParameterNames.at(index_final);
                }
                if(index_final < m_FunctionDescriptions.length())
                {
                    fi.Description = m_FunctionDescriptions.at(index_final);
                }
                if(index_final < m_ReturnNames.length())
                {
                    fi.ReturnName = m_ReturnNames.at(index_final);
                }
                if(index_final < m_ParameterDescriptions.length())
                {
                    fi.ParameterDescription = m_ParameterDescriptions.at(index_final);
                }
                if(index_final < m_ReturnDescriptions.length())
                {
                    fi.ReturnDescription = m_ReturnDescriptions.at(index_final);
                }
                if(index_final < m_ParameterTypes.length())
                {
                    fi.ParameterTypes = m_ParameterTypes.at(index_final);
                }
            }
            //append function
            ret.append(fi);
        }
    }

    return ret;
}


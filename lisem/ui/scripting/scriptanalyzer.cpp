#include "scriptanalyzer.h"


void ScriptAnalyzer::AnalyzeScript(QString qscript)
{

    m_Script = qscript;

    m_classesScript.clear();
    std::string modifiedScript = qscript.toStdString();
    std::string currentClass = "";
    std::string currentNamespace = "";
    unsigned int pos = 0;
    int nested = 0;
    int nested_total = 0;

    bool canbestatement = true;
    bool is_enum = false;

    int scope_uid_count = 0;

    QList<ScriptScope * > scopes;
    QList<ScriptScope * > scopestack;

    tokens.clear();
    ClassTokens.clear();
    VariableTokens.clear();
    FunctionTokens.clear();

    while( pos < modifiedScript.size() )
    {
        int pos_org = pos;
        asUINT len = 0;
        asETokenClass t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);

        std::string token;
        token.assign(&modifiedScript[pos], len);

        std::cout << "token:  " << token << std::endl;
        ScriptToken token_this;
        token_this.tokenclass = t;
        token_this.pos = pos;
        token_this.length = len;
        token_this.text = QString(token.c_str());

        tokens.append(token_this);

        //if( pos >  cpos)
        //{
        //    cursorlevel = nested_total;
        //}

        if(nested == 0)
        {
            // Check if class
            if( currentClass == "" && (modifiedScript.substr(pos,len) == "class" || modifiedScript.substr(pos,len) == "interface" ) )
            {
                // Get the identifier after "class"
                do
                {
                    pos += len;
                    if( pos >= modifiedScript.size() )
                    {
                        t = asTC_UNKNOWN;
                        break;
                    }
                    t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);
                } while(t == asTC_COMMENT || t == asTC_WHITESPACE);

                if( t == asTC_IDENTIFIER )
                {
                    currentClass = modifiedScript.substr(pos,len);

                    ScriptToken token_class;
                    token_class.is_class = true;
                    token_class.name = QString(modifiedScript.substr(pos,len).c_str());
                    token_class.nspace = QString(currentNamespace.c_str());
                    token_class.nesting = nested_total;
                    token_class.pos = token_this.pos;
                    token_class.length = pos + len - token_this.pos;

                    std::cout << "class " << token_class.name.toStdString() << std::endl;

                    ClassTokens.append(token_class);

                    m_classesScript.append(QString(currentClass.c_str()));

                    // Search until first { or ; is encountered
                    while( pos < modifiedScript.length() )
                    {
                        m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);

                        // If start of class section encountered stop
                        if( modifiedScript[pos] == '{' )
                        {
                            nested_total += 1;
                            pos += len;
                            break;
                        }
                        else if (modifiedScript[pos] == ';')
                        {
                            // The class declaration has ended and there are no children
                            currentClass = "";
                            pos += len;
                            break;
                        }

                        // Check next symbol
                        pos += len;
                    }
                }

                continue;
            }

            // Check if end of class
            if( currentClass != "" && modifiedScript[pos] == '}' )
            {
                currentClass = "";
                nested_total -= 1;
                pos += len;
                continue;
            }


            // Check if namespace
            if( modifiedScript.substr(pos,len) == "namespace" )
            {
                // Get the identifier after "namespace"
                do
                {
                    pos += len;
                    t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);
                } while(t == asTC_COMMENT || t == asTC_WHITESPACE);

                if( currentNamespace != "" )
                    currentNamespace += "::";
                currentNamespace += modifiedScript.substr(pos,len);

                // Search until first { is encountered
                while( pos < modifiedScript.length() )
                {
                    m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);

                    // If start of namespace section encountered stop
                    if( modifiedScript[pos] == '{' )
                    {
                        nested_total += 1;
                        pos += len;
                        break;
                    }

                    // Check next symbol
                    pos += len;
                }

                continue;
            }

            // Check if end of namespace
            if( currentNamespace != "" && modifiedScript[pos] == '}' )
            {
                size_t found = currentNamespace.rfind( "::" );
                if( found != std::string::npos )
                {
                    currentNamespace.erase( found );
                }
                else
                {
                    currentNamespace = "";
                }
                nested_total -= 1;
                pos += len;
                continue;
            }
        }

        //detect the potential begin of a new statement, so we can indicate we are looking for a new function or variable
        if(modifiedScript[pos] == '{')
        {
            //we are now in another nested scope, so the nested level increases
            nested += 1;
            nested_total += 1;

            //add scope to list
            //provide uid
            //add to scope stack
            ScriptScope * scope = new ScriptScope();
            scope->pos_begin = pos;
            scope->type = "";
            scope->bracket = "{";
            scope->level= nested;
            scope->uid = scope_uid_count;
            scope_uid_count += 1;

            scopes.push_back(scope);
            scopestack.push_back(scope);



            pos += len;
            continue;
        }

        //detect the potential begin of a new statement, so we can indicate we are looking for a new function or variable
        if(modifiedScript[pos] == '}')
        {
            //we leave a nested scope, so nesting level decreases
            nested -= 1;
            nested_total -= 1;
            nested = std::max(nested,0);
            nested_total = std::max(nested_total,0);

            //edit scope ending
            ScriptScope * scope = nullptr;
            if(scopestack.size() > 0)
            {
                scope = scopestack.at(scopestack.size()-1);

                scope->pos_end = pos;

                //remove from stack
                scopestack.removeLast();
            }


            pos += len;
            continue;
        }

        // Skip white spaces, comments, and leading decorators
        if( t == asTC_KEYWORD && !(token == "void" || token == "int" || token == "double" ||token == "float" || token == "bool"||token == "uint" || token == "int8" ||token == "uint8" ||token == "int16" ||token == "uint16" ||token == "int64" ||token == "uint64"))
        {
            //any reserved words, or [,],,,/,.,<,>,@,+,-,*,^,<<,>>,=,==

            if( token == "interface" || token == "class" ||
                    token == "enum" || token == "private" ||
                    token == "protected" ||
                    token == "shared" || token == "external" ||
                    token == "final" || token == "abstract" )
            {



            }

            //we detected classes and interfaces already, the other ones do not matter now
            if( token == "enum" )
            {
                bool did_identifier = false;
                // Search until first { or ; is encountered
                while( pos < modifiedScript.length() )
                {
                    int t_inner = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);

                    if(t_inner == asTC_IDENTIFIER)
                    {
                        if(!did_identifier)
                        {
                            std::string token_enum;
                            token_enum.assign(&modifiedScript[pos], len);
                            ScriptToken token_var;
                            token_var.is_variable = true;
                            token_var.name = QString(token_enum.c_str());
                            token_var.text = QString(token_enum.c_str());
                            token_var.nspace = QString(currentNamespace.c_str());
                            token_var.nesting = nested_total;
                            token_var.pos = token_this.pos;
                            token_var.length = pos + len - token_this.pos;

                            std::cout << "enum variable "<< std::endl;

                            //if non-global nesting is 0, it means it is an object member
                            if(nested == 0)
                            {
                                token_var.memberclass = QString(currentClass.c_str());
                            }

                            VariableTokens.append(token_var);

                            //enums are recognizeable variables
                            did_identifier = true;
                        }
                    }else
                    // If start of class section encountered stop
                    if( modifiedScript[pos] == ',' )
                    {
                        nested_total += 1;
                        pos += len;
                        did_identifier = false;
                        continue;
                    }
                    else
                    //end of enum block
                    if (modifiedScript[pos] == '}')
                    {
                        // The class declaration has ended and there are no children
                        currentClass = "";
                        pos += len;
                        break;
                    }

                    // Check next symbol
                    pos += len;
                }


            }


            //for and while
            if( token == "for" || token == "while" || token == "if" || token == "else")
            {
                std::string token_scopetype = token;
                int nested_inner = 0;

                for(; pos < (int)modifiedScript.size();)
                {

                    t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);
                    token.assign(&modifiedScript[pos], len);
                    if(token == "(")
                    {
                        nested_inner ++;
                    }
                    if(token == ")")
                    {
                        nested_inner --;

                    }else if(token == "{")
                    {
                        if(nested_inner == 0)
                        {
                            nested += 1;
                            nested_total += 1;

                            ScriptScope * scope = new ScriptScope();
                            scope->pos_begin = pos;
                            scope->type = QString(token_scopetype.c_str());
                            scope->bracket = "{";
                            scope->level= nested;
                            scope->uid = scope_uid_count;
                            scope_uid_count += 1;

                            scopes.push_back(scope);
                            scopestack.push_back(scope);


                            pos += len;
                            break;
                        }
                    }else if(token == ";")
                    {
                        if(nested_inner == 0)
                        {
                            //means either its do{}while(); or for() ...;
                            //in both cases, we already dealt with the nesting scope
                            pos += len;
                            break;
                        }
                    }

                    pos += len;
                }
            }


        }

        // get some kind of statement (can be a variable or a function)
        if( t == asTC_IDENTIFIER || (token == "void" || token == "int" || token == "double" ||token == "float" || token == "bool"||token == "uint" || token == "int8" ||token == "uint8" ||token == "int16" ||token == "uint16" ||token == "int64" ||token == "uint64"))
        {

            std::cout << "identifier found " << std::endl;
            pos = pos + len;

            //first check if there is a dot after this identifier. if so, we have a write to an object member or a script-recognized file
            if(pos < (int)modifiedScript.size())
            {
                if(modifiedScript[pos] == '.')
                {
                    pos=pos + 1;
                    if(pos < (int)modifiedScript.size())
                    {
                        t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);
                        std::string token_temp;
                        token_temp.assign(&modifiedScript[pos], len);


                        token = token + "."+ token_temp;
                        pos += len;
                    }
                }
            }

            bool hasParenthesis = false;
            int nestedParenthesis = 0;
            bool hasOtherParenthesis = false;
            int nestedOtherParenthesis = 0;
            std::string name;
            bool name_found = false;
            std::string decleration = token;
            std::string decleration_noargs = token;

            //detect if function or otherwise variable?
            for(; pos < (int)modifiedScript.size();)
            {

                t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);
                token.assign(&modifiedScript[pos], len);

                if (t == asTC_KEYWORD)
                {
                    if(!(hasParenthesis || (token == "(") || (token == ")")))
                    {
                        decleration_noargs += token;
                    }
                    decleration += token;
                    if (token == "{" && nestedParenthesis == 0)
                    {
                        if (hasParenthesis)
                        {
                            if(name_found)
                            {
                                // We've found the end of a function signature

                                ScriptToken token_func;
                                token_func.is_function = true;
                                token_func.name = QString(name.c_str());
                                token_func.text = QString(decleration.c_str());
                                token_func.nspace = QString(currentNamespace.c_str());
                                token_func.nesting = nested_total;
                                token_func.pos = token_this.pos;
                                token_func.length = pos + len - token_this.pos;

                                QString type = QString(decleration_noargs.c_str()).trimmed();
                                token_func.Type = type.left(std::max(0,(int) (type.length() - name.length()))).trimmed();

                                std::cout << "function " << name <<  "   " <<  decleration_noargs << " " << token_func.Type.toStdString() <<  std::endl;


                                //if non-global nesting is 0, it means it is an object member
                                if(nested == 0)
                                {
                                    token_func.memberclass = QString(currentClass.c_str());
                                }

                                FunctionTokens.append(token_func);


                                nested += 1;
                                nested_total += 1;

                                ScriptScope * scope = new ScriptScope();
                                scope->pos_begin = pos;
                                scope->type = "function" ;
                                scope->bracket = "{";
                                scope->level= nested;
                                scope->uid = scope_uid_count;
                                scope_uid_count += 1;

                                scopes.push_back(scope);
                                scopestack.push_back(scope);

                            }else if(token == "= ")
                            {
                                int nested_inner = 0;
                                for(; pos < (int)modifiedScript.size();)
                                {
                                    t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);
                                    token.assign(&modifiedScript[pos], len);
                                    if(token == "(")
                                    {
                                        nested_inner ++;
                                    }
                                    if(token == ")")
                                    {
                                        nested_inner --;

                                    }
                                    if(token == "[")
                                    {
                                        nested_inner ++;
                                    }
                                    if(token == "]")
                                    {
                                        nested_inner --;

                                    }
                                    if(token == "{")
                                    {
                                        nested_inner ++;
                                    }
                                    if(token == "}")
                                    {
                                        nested_inner --;

                                    }
                                    else if(token == ";")
                                    {
                                        if(nested_inner == 0)
                                        {
                                            pos += len;
                                            break;
                                        }
                                    }
                                    pos += len;
                                }
                                break;
                            }else
                            {
                                pos += len;
                                break;
                            }

                        }
                        else
                        {
                            // We've found a virtual property. Just keep the name

                        }
                        pos += len;
                        break;
                    }
                    if ((token == "=" && !hasParenthesis) || token == ";")
                    {
                        if (hasParenthesis)
                        {
                            if(name_found)
                            {
                                // The declaration is ambigous. It can be a variable with initialization, or a function prototype
                                // assume it is a variable

                                ScriptToken token_var;
                                token_var.is_variable = true;
                                token_var.name = QString(name.c_str());
                                token_var.text = QString(decleration.c_str());
                                token_var.nspace = QString(currentNamespace.c_str());
                                token_var.nesting = nested_total;
                                token_var.pos = token_this.pos;
                                token_var.length = pos + len - token_this.pos;

                                std::cout << "variable " << name << " d:" << decleration << std::endl;

                                QString type = QString(decleration_noargs.c_str()).trimmed();
                                type = type.left(type.length()-1).trimmed();
                                token_var.Type = type.left(std::max(0,(int) (type.length() - name.length())));


                            }

                            pos += len;
                            break;
                        }
                        else
                        {
                            //variable
                            // Substitute the declaration with just the name
                            if(name_found)
                            {
                                int nested_inner = 0;

                                ScriptToken token_var;
                                token_var.is_variable = true;
                                token_var.name = QString(name.c_str());
                                token_var.text = QString(decleration.c_str());
                                token_var.nspace = QString(currentNamespace.c_str());
                                token_var.nesting = nested_total;
                                token_var.pos = token_this.pos;
                                token_var.length = pos + len - token_this.pos;
                                QString type = QString(decleration.c_str()).trimmed();
                                type = type.left(type.length()-1).trimmed();
                                token_var.Type = type.left(std::max(0,(int) (type.length() - name.length())));

                                //if non-global nesting is 0, it means it is an object member
                                if(nested == 0)
                                {
                                    token_var.memberclass = QString(currentClass.c_str());
                                }

                                VariableTokens.append(token_var);

                                for(; pos < (int)modifiedScript.size();)
                                {

                                    t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);
                                    token.assign(&modifiedScript[pos], len);
                                    if(token == "(")
                                    {
                                        nested_inner ++;
                                    }
                                    if(token == ")")
                                    {
                                        nested_inner --;

                                    }
                                    if(token == "[")
                                    {
                                        nested_inner ++;
                                    }
                                    if(token == "]")
                                    {
                                        nested_inner --;

                                    }
                                    if(token == "{")
                                    {
                                        nested_inner ++;
                                    }
                                    if(token == "}")
                                    {
                                        nested_inner --;

                                    }
                                    else if(token == ";")
                                    {
                                        if(nested_inner == 0)
                                        {
                                            pos += len;
                                            break;
                                        }
                                    }
                                    pos += len;
                                }

                                break;
                            }else if(token == "=")
                            {
                                int nested_inner = 0;
                                for(; pos < (int)modifiedScript.size();)
                                {
                                    t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);
                                    token.assign(&modifiedScript[pos], len);
                                    if(token == "(")
                                    {
                                        nested_inner ++;
                                    }
                                    if(token == ")")
                                    {
                                        nested_inner --;

                                    }
                                    if(token == "[")
                                    {
                                        nested_inner ++;
                                    }
                                    if(token == "]")
                                    {
                                        nested_inner --;

                                    }
                                    if(token == "{")
                                    {
                                        nested_inner ++;
                                    }
                                    if(token == "}")
                                    {
                                        nested_inner --;

                                    }
                                    else if(token == ";")
                                    {
                                        if(nested_inner == 0)
                                        {
                                            pos += len;
                                            break;
                                        }
                                    }
                                    pos += len;
                                }
                                break;
                            }
                        }
                    }else if(token == "=")
                    {
                        int nested_inner = 0;
                        for(; pos < (int)modifiedScript.size();)
                        {

                            t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);
                            token.assign(&modifiedScript[pos], len);
                            if(token == "(")
                            {
                                nested_inner ++;
                            }
                            if(token == ")")
                            {
                                nested_inner --;

                            }
                            if(token == "[")
                            {
                                nested_inner ++;
                            }
                            if(token == "]")
                            {
                                nested_inner --;

                            }
                            if(token == "{")
                            {
                                nested_inner ++;
                            }
                            if(token == "}")
                            {
                                nested_inner --;

                            }
                            else if(token == ";")
                            {
                                if(nested_inner == 0)
                                {
                                    pos += len;
                                    break;
                                }
                            }
                            pos += len;
                        }

                        break;
                    }
                    else if (token == "(")
                    {
                        nestedParenthesis++;

                        // This is the first parenthesis we encounter. If the parenthesis isn't followed
                        // by a statement block, then this is a variable declaration, in which case we
                        // should only store the type and name of the variable, not the initialization parameters.
                        hasParenthesis = true;
                    }else if(token == "[")
                    {
                        nestedOtherParenthesis++;
                        hasOtherParenthesis = true;

                    }else if(token == "]")
                    {
                        nestedOtherParenthesis--;
                    }else if(token == "<")
                    {
                        nestedOtherParenthesis++;
                        hasOtherParenthesis = true;

                    }else if(token == ">")
                    {
                        nestedOtherParenthesis--;
                    }
                    else if (token == ")")
                    {
                        nestedParenthesis--;
                    }

                }
                else if( t == asTC_IDENTIFIER )
                {
                    if(!(hasParenthesis || (nestedOtherParenthesis>0)))
                    {
                        name_found = true;
                        name = token;

                    }
                    if(!hasParenthesis)
                    {
                        decleration_noargs += token;

                    }
                    decleration += token;
                }else if(!(t == asTC_COMMENT) && token != "\n")
                {
                    decleration += token;
                    if(!hasParenthesis)
                    {
                        decleration_noargs += token;
                    }
                }

                // Skip trailing decorators
                //if( !hasParenthesis || nestedParenthesis > 0 || t != asTC_IDENTIFIER || (token != "final" && token != "override") )
                //    declaration += token;

                //pos += len;
            }
            //finally unknown
        }

        pos += len;


    }


    std::cout << "number of scopes: " << scopes.size() << " " << scopestack.size() << std::endl;

    Scopes.clear();

    //onw copy the scopes
    for(int i = 0; i < scopes.size(); i++)
    {
        Scopes.append(*(scopes.at(i)));
        ScriptLocation loc = GetLocationFromPosition(Scopes.at(i).pos_begin);
        ScriptLocation loc2 = GetLocationFromPosition(Scopes.at(i).pos_end);
        std::cout << "i locs " << i << " " << loc.line_n << " " << loc.col_n << std::endl;
        std::cout << "i locs end " << i << " " << loc2.line_n << " " << loc2.col_n << std::endl;
        delete scopes.at(i);
    }


}

void ScriptAnalyzer::FindErrors()
{



}

ScriptLocation ScriptAnalyzer::GetLocationFromPosition(int pos)
{
    ScriptLocation sl;
    int linen = 0;
    int coln = 0;
    if(pos >= m_Script.size())
    {
        pos = m_Script.size() -1;
    }
    for(int i = 0;i < m_Script.size(); i++)
    {
        if(m_Script.at(i) == "\n")
        {
            linen ++;
            coln = 0;
        }

        coln ++;

        if(i +1 == pos)
        {
            break;
        }
    }

    sl.line_n = linen;
    sl.col_n = coln;
    return sl;

}


#include "scriptanalyzer.h"


void ScriptAnalyzer::AnalyzeScript(QString qscript)
{

    m_Script = qscript;

    m_Lines = m_Script.split("\n");

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

    Tokens.clear();
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

        std::cout << "token:  " << token << "   type: " <<  t << std::endl;

        ScriptToken token_this;
        token_this.tokenclass = t;
        token_this.pos = pos;
        token_this.length = len;
        token_this.text = QString(token.c_str());

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
                    {
                        ScriptScope * scope = nullptr;
                        if(scopestack.size() > 0)
                        {
                            scope = scopestack.at(scopestack.size()-1);
                        }
                        token_class.scope = scope;
                    }

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
                            nested += 1;
                            nested_total += 1;
                            ScriptScope * scope = new ScriptScope();
                            scope->pos_begin = pos;
                            scope->type = "class";
                            scope->bracket = "{";
                            scope->level= nested;
                            scope->uid = scope_uid_count;
                            scope_uid_count += 1;
                            {
                                ScriptScope * scopep = nullptr;
                                if(scopestack.size() > 0)
                                {
                                    scopep = scopestack.at(scopestack.size()-1);
                                }
                                scope->parent = scopep;
                            }

                            scopes.push_back(scope);
                            scopestack.push_back(scope);

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
                        nested += 1;
                        nested_total += 1;

                        ScriptScope * scope = new ScriptScope();
                        scope->pos_begin = pos;
                        scope->type = "namespace";
                        scope->bracket = "{";
                        scope->level= nested;
                        scope->uid = scope_uid_count;
                        scope_uid_count += 1;
                        {
                            ScriptScope * scopep = nullptr;
                            if(scopestack.size() > 0)
                            {
                                scopep = scopestack.at(scopestack.size()-1);
                            }
                            scope->parent = scopep;
                        }
                        scopes.push_back(scope);
                        scopestack.push_back(scope);


                        pos += len;
                        break;
                    }

                    // Check next symbol
                    pos += len;
                }

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
            {
                ScriptScope * scopep = nullptr;
                if(scopestack.size() > 0)
                {
                    scopep = scopestack.at(scopestack.size()-1);
                }
                scope->parent = scopep;
            }
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
                pos += len;
                continue;

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
                            ScriptScope * scope = nullptr;
                            if(scopestack.size() > 0)
                            {
                                scope = scopestack.at(scopestack.size()-1);
                                token_var.scope = scope;
                            }

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
                            {
                                ScriptScope * scopep = nullptr;
                                if(scopestack.size() > 0)
                                {
                                    scopep = scopestack.at(scopestack.size()-1);
                                }
                                scope->parent = scopep;
                            }
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

            bool is_property_assignment = false;
            //first check if there is a dot after this identifier. if so, we have a write to an object member or a script-recognized file
            if(pos < (int)modifiedScript.size())
            {
                if(modifiedScript[pos] == '.')
                {
                    is_property_assignment = true;
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

            //cover any white-spaces
            //now check for closing bracket
            //this shouldnt normally exist, but it can because in autocomplete the user has not added the semicolon yet

            bool done_closingb = false;
            for(; pos < (int)modifiedScript.size();)
            {
                t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);

                std::string token_temp;
                token_temp.assign(&modifiedScript[pos], len);
                if(t == asTC_WHITESPACE)
                {

                }else if(token_temp == "}")
                {
                    //we leave a nested scope, so nesting level decreases
                    /*nested -= 1;
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
                    }*/
                    done_closingb = true;
                    break;




                }else
                {
                    break;
                }

                pos += len;

            }

            if(done_closingb)
            {
                continue;
            }



            //check for return statement
            if(pos < (int)modifiedScript.size())
            {
                if(modifiedScript[pos] == ';')
                {
                    pos=pos + 1;
                    continue;

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
                                {
                                    ScriptScope * scope = nullptr;
                                    if(scopestack.size() > 0)
                                    {
                                        scope = scopestack.at(scopestack.size()-1);
                                    }
                                    token_func.scope = scope;
                                }

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
                                {
                                    ScriptScope * scopep = nullptr;
                                    if(scopestack.size() > 0)
                                    {
                                        scopep = scopestack.at(scopestack.size()-1);
                                    }
                                    scope->parent = scopep;
                                }
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
                                {
                                    ScriptScope * scope = nullptr;
                                    if(scopestack.size() > 0)
                                    {
                                        scope = scopestack.at(scopestack.size()-1);
                                    }
                                    token_var.scope = scope;
                                }
                                std::cout << "variable " << name << " d:" << decleration << std::endl;

                                QString type = QString(decleration_noargs.c_str()).trimmed();
                                type = type.left(type.length()-1).trimmed();
                                token_var.Type = type.left(std::max(0,(int) (type.length() - name.length()))).trimmed();

                                VariableTokens.append(token_var);
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
                                token_var.Type = type.left(std::max(0,(int) (type.length() - name.length()))).trimmed();
                                {
                                    ScriptScope * scope = nullptr;
                                    if(scopestack.size() > 0)
                                    {
                                        scope = scopestack.at(scopestack.size()-1);
                                    }
                                    token_var.scope = scope;
                                }

                                std::cout << "variable " << name << " d:" << decleration << " t: " << token_var.Type.toStdString() <<  std::endl;

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

                std::cout << "token not dealt " << token << std::endl;
                pos += len;
                continue;
            }
            //finally unknown
        }


        std::cout << "token not dealt at all" << token << std::endl;
        pos += len;


    }


    std::cout << "number of scopes: " << scopes.size() << " " << scopestack.size() << std::endl;

    Scopes.clear();

    //onw copy the scopes
    for(int i = 0; i < scopes.size(); i++)
    {
        Scopes.append((scopes.at(i)));
        ScriptLocation loc = GetLocationFromPosition(Scopes.at(i)->pos_begin);
        ScriptLocation loc2 = GetLocationFromPosition(Scopes.at(i)->pos_end);
        std::cout << "i locs " << i << " " << loc.line_n << " " << loc.col_n << std::endl;
        std::cout << "i locs end " << i << " " << loc2.line_n << " " << loc2.col_n << std::endl;

    }


    //with the scopes done, and all classes/interfaces/enums/variables/functions known
    //we can do a full tokenization and for each token get the correct scope

    ScriptScope * scope = nullptr;
    std::vector<ScriptScope *> scope_prev;
    int next_change_pos = 999999999;
    for(int i = 0; i < Scopes.size(); i++)
    {
        if(Scopes.at(i)->pos_begin < next_change_pos)
        {
            next_change_pos = Scopes.at(i)->pos_begin;

        }

    }

    std::cout << "do tokenizatoin " << std::endl;
    pos = 0;
    while( pos < modifiedScript.size() )
    {
        int pos_org = pos;
        asUINT len = 0;
        asETokenClass t = m_ScriptEngine->ParseToken(&modifiedScript[pos], modifiedScript.size() - pos, &len);

        std::cout << "get token " << std::endl;

        std::string token;
        token.assign(&modifiedScript[pos], len);
        if(pos == next_change_pos)
        {

            for(int i = 0; i < Scopes.size(); i++)
            {
                if(Scopes.at(i)->pos_begin >= pos && Scopes.at(i)->pos_begin < pos + len)
                {
                    std::cout << "enter scope " << std::endl;
                    Scopes.at(i)->parent = scope;
                    scope_prev.push_back(scope);
                    scope = Scopes.at(i);
                    break;
                }

                if(Scopes.at(i)->pos_end >= pos && Scopes.at(i)->pos_end < pos + len)
                {
                    std::cout << "exit scope " << std::endl;
                    ScriptScope * prev = nullptr;
                    if(scope_prev.size() > 0)
                    {
                        prev = scope_prev.at(scope_prev.size()-1);
                        scope_prev.erase(scope_prev.end()-1);
                        scope = prev;
                    }else
                    {
                        scope = nullptr;
                    }
                    break;
                }
            }
            std::cout << "get next change " << std::endl;
            next_change_pos = 999999999;
            for(int i = 0; i < Scopes.size(); i++)
            {
                if((Scopes.at(i)->pos_begin >= (pos + len)) &&  (Scopes.at(i)->pos_begin < next_change_pos))
                {
                    next_change_pos = Scopes.at(i)->pos_begin;
                }
                if((Scopes.at(i)->pos_end >= (pos + len)) &&  (Scopes.at(i)->pos_end < next_change_pos))
                {
                    next_change_pos = Scopes.at(i)->pos_end;
                }

            }
        }



        //get current scope

        std::cout << "store token " << std::endl;

        ScriptToken token_this;
        token_this.is_variable = true;
        token_this.name = "";
        token_this.TokenType = t;
        token_this.text = QString(token.c_str());
        token_this.nspace = QString(currentNamespace.c_str());
        token_this.nesting = nested_total;
        token_this.pos = pos;
        token_this.length = len;
        token_this.scope = scope;

        Tokens.append(token_this);

        int levelthis = 0;
        if(scope != nullptr)
        {
            levelthis =  scope->level;
        }
        std::cout << "token:  " << token << "   type: " <<  t << " scope nesting " << levelthis << std::endl;
        pos += len;
    }

}

ScriptScope * ScriptAnalyzer::GetScopeAtPos(int posin)
{
    ScriptScope * scope = nullptr;
    std::vector<ScriptScope *> scope_prev;
    int next_change_pos = 999999999;
    for(int i = 0; i < Scopes.size(); i++)
    {
        if(Scopes.at(i)->pos_begin < next_change_pos)
        {
            next_change_pos = Scopes.at(i)->pos_begin;

        }

    }

    int pos = 0;
    while( pos < m_Script.size() )
    {
        int pos_org = pos;
        asUINT len = 1;

        if(pos == next_change_pos)
        {

            for(int i = 0; i < Scopes.size(); i++)
            {
                if(Scopes.at(i)->pos_begin >= pos && Scopes.at(i)->pos_begin < pos + len)
                {
                    Scopes.at(i)->parent = scope;
                    scope_prev.push_back(scope);
                    scope = Scopes.at(i);
                    break;
                }

                if(Scopes.at(i)->pos_end >= pos && Scopes.at(i)->pos_end < pos + len)
                {
                    ScriptScope * prev = nullptr;
                    if(scope_prev.size() > 0)
                    {
                        prev = scope_prev.at(scope_prev.size()-1);
                    }
                    scope_prev.erase(scope_prev.end());
                    scope = prev;
                    break;
                }
            }
            next_change_pos = 999999999;
            for(int i = 0; i < Scopes.size(); i++)
            {
                if((Scopes.at(i)->pos_begin >= (pos + len)) &&  (Scopes.at(i)->pos_begin < next_change_pos))
                {
                    next_change_pos = Scopes.at(i)->pos_begin;
                }
                if((Scopes.at(i)->pos_end >= (pos + len)) &&  (Scopes.at(i)->pos_end < next_change_pos))
                {
                    next_change_pos = Scopes.at(i)->pos_end;
                }

            }
        }
        if(pos == posin)
        {
            break;
        }


    }
    return scope;

}

std::vector<ScriptToken> ScriptAnalyzer::GetTokenDefinitions(QString name)
{


    return {};
}

ScriptExpressionType ScriptAnalyzer::GetExpressionTypeFunction(ScriptExpressionTypeSequence parents,QString name, int scriptpos)
{


    ScriptExpressionType t;
    return t;
}
ScriptExpressionType ScriptAnalyzer::GetExpressionTypeVariable(ScriptExpressionTypeSequence parents,QString name, int scriptpos)
{

    ScriptExpressionType t;
    return t;
}



bool ScriptAnalyzer::ScopeContains(ScriptScope * s1, int pos)
{
     if(s1 != nullptr)
     {
         if(s1->pos_begin <= pos && s1->pos_end >= pos)
         {
             return true;
         }else
         {
             return false;
         }

     }else
     {
         return true;
     }

}

ScriptExpressionTypeSequence ScriptAnalyzer::GetExpressionTypeString(QString expr, int pos, int scriptpos, ScriptScope * scope, bool scope_set)
{
    //default namespace/scope is empty
    ScriptExpressionType type_ret;
    type_ret.is_empty = true;

    if(!scope_set)
    {
        scope = GetScopeAtPos(scriptpos);
        scope_set = true;
    }

    //go through the script on the left of position to find out the scope

    //check for object (temporary or variable)
    if(pos-1 > 0)
    {
        std::cout << "check expression " << QString(expr.at(pos-1)).toStdString() << std::endl;

        //check for function return object or arithmetic statement
        if(expr.at(pos-1) == "." && expr.at(pos-2) == ")")
        {
            std::cout << "func call detected " << std::endl;

            //get function name
            ScriptExpressionTypeSequence parent_type;
            int posn = pos - 3;
            bool quote = false;
            bool brackets = 0;
            QString args = "";
            QString temp = "";
            bool is_temp = false;
            while(posn > -1)
            {
                if(!quote)
                {
                    if(expr.at(posn) == "(")
                    {
                        if(brackets == 0)
                        {
                            break;
                        }else
                        {
                            brackets -= 1;
                        }

                    }
                    if(expr.at(posn) == ")")
                    {
                        brackets += 1;
                    }
                    if(expr.at(posn) == '"')
                    {
                        quote = !quote;
                    }
                }else
                {
                    if(expr.at(posn) == '"')
                    {
                        quote = !quote;
                    }
                }

                args = expr.at(posn) + args;


                posn = posn -1;
            }

            posn = posn -1;

            QList<QString> temptypes;
            if(posn > -1)
            {
                //we might have a constructor of a templated type, and directly a function member on that instantiation
                if(expr.at(posn) == ">")
                {

                    is_temp = true;
                    int brackets_temp = 0;
                    bool quote_temp = false;

                    while(posn > -1)
                    {
                        if(!quote_temp)
                        {
                            if(expr.at(posn) == "<")
                            {
                                if(brackets_temp == 0)
                                {
                                    break;
                                }else
                                {
                                    brackets_temp -= 1;
                                }

                            }
                            if(expr.at(posn) == ">")
                            {
                                brackets_temp += 1;
                            }
                            if(expr.at(posn) == '"')
                            {
                                quote_temp = !quote_temp;
                            }
                        }else
                        {
                            if(expr.at(posn) == '"')
                            {
                                quote_temp = !quote_temp;
                            }
                        }

                        if(expr.at(posn) != " "  && expr.at(posn) != "\t")
                        {
                            temp = expr.at(posn) + temp;
                        }

                        posn = posn -1;

                    }
                }
            }

            QString name = "";
            while(posn > -1)
            {
                if(expr.at(posn).isLetterOrNumber() || expr.at(posn) == "_")
                {
                    name = expr.at(posn) + name;
                }else
                {
                    break;
                }

                posn = posn -1;

            }

            //see if it is an object of someting else,

            if(posn -1 > 0)
            {
                if(expr.at(posn) == "." )
                {
                    std::cout << "get parent " << std::endl;
                    //otherwise it is a member of another object
                    parent_type =  GetExpressionTypeString(expr, posn+1,scriptpos,scope,true);

                }

                if(expr.at(posn-1) == ":" && expr.at(posn) == ":")
                {

                    std::cout << "get parent " << std::endl;
                    //we have a namespace to resolve
                    parent_type = GetExpressionTypeString(expr, posn+1,scriptpos,scope,true);

                }
            }

            if(is_temp)
            {
                temptypes = temp.split(",");

            }

            //then try to resolve this
            //is last object in parent type a class
            //search class methods

            std::cout << "function name: " << name.toStdString() << " " << parent_type.GetLast().is_empty <<  " " << parent_type.GetLast().is_object << " " <<  parent_type.GetLast().is_function <<  std::endl;

            if(name.isEmpty() && !is_temp)
            {
                //without function name we are talking about an arithmetic expression

                ScriptExpressionType arith_type = GetArithmeticStatementType(args,scriptpos);


            }else if(parent_type.GetLast().is_empty)
            {
                std::cout << "no parent " << is_temp << std::endl;
                if(is_temp)
                {
                    //we need to search all class constructors to find a templated match

                }else
                {
                    //check global cpp functions
                    for(int i = 0; i < funclist.size(); i++)
                    {
                        ScriptFunctionInfo fi = funclist.at(i);
                        int tid = fi.Function->GetReturnTypeId();
                        QString type = m_ScriptManager->m_Engine->GetTypeName(tid);
                        std::cout << "check " << fi.Function->GetName() << " " << type.toStdString() << std::endl;
                        //can we find a match
                        if(QString(fi.Function->GetName()) == name)
                        {
                            std::cout << "found match for global cpp func: " << fi.ObjectName.toStdString() << std::endl;

                            std::cout << "returns " << type.toStdString() << std::endl;

                            return ScriptExpressionTypeSequence::FromType(ScriptExpressionType::FromCppFunctionReturn(name,type));

                        }
                    }

                    //check local functions
                    for(int i = 0; i < FunctionTokens.size(); i++)
                    {
                        ScriptToken fi = FunctionTokens.at(i);
                        std::cout << "check local " << fi.name.toStdString() << " " <<fi.Type.toStdString() << std::endl;

                        //can we find a match non-member function
                        if(fi.name == name && fi.memberclass.isEmpty())
                        {
                            //check namespace?
                            {
                                std::cout << "found match for local func: " << fi.name.toStdString() << std::endl;

                                std::cout << "returns " << fi.Type.toStdString() << std::endl;


                                if(ScopeContains(fi.scope,scriptpos))
                                {
                                    std::cout << "scope okey" << std::endl;
                                    return ScriptExpressionTypeSequence::FromType(ScriptExpressionType::FromFunctionReturn(name,fi.Type,false,{},false,"",fi));

                                }


                            }
                        }

                    }
                }




            }else if(parent_type.GetLast().is_object || parent_type.GetLast().is_function)
            {
                //get all class methods, and check those

                std::cout << "call is object member func " << std::endl;

                QString ptype =parent_type.GetLast().type;
                QString ptypefull=  ptype;
                QString ttype = "";
                int brack_temp_here= 0;
                bool stop_temp_one = false;
                if(ptypefull.contains("<"))
                {
                    ptype = "";
                    for(int i = 0; i < ptypefull.length(); i++)
                    {
                        if(ptypefull.at(i) == "," && brack_temp_here == 1)
                        {
                            stop_temp_one = true;
                        }

                        if(ptypefull.at(i) != "<" && ptypefull.at(i) != " ")
                        {
                            if(brack_temp_here == 0)
                            {
                                ptype = ptype + ptypefull.at(i);
                            }else if(!stop_temp_one)
                            {
                                ttype = ttype + ptypefull.at(i);
                            }
                        }

                        if(ptypefull.at(i) == "<")
                        {
                            brack_temp_here += 1;
                        }
                        if(ptypefull.at(i) == "<")
                        {
                            brack_temp_here -= 1;
                            if(brack_temp_here == 0)
                            {
                                break;
                            }
                        }
                    }

                }

                //check global class methods
                for(int i = 0; i < funcmemberlist.size(); i++)
                {
                    ScriptFunctionInfo fi = funcmemberlist.at(i);
                    int tid = fi.Function->GetReturnTypeId();
                    QString type = m_ScriptManager->m_Engine->GetTypeName(tid);


                    std::cout << "check member func " <<  fi.ObjectName.toStdString() << " " << fi.Function->GetName() << " " << type.toStdString() << " " << ptype.toStdString() << " " << fi.ObjectName.toStdString() << std::endl;
                    if(QString(fi.Function->GetName()) == name && ptype == fi.ObjectName)
                    {
                        std::cout << "found member func  " << std::endl;
                        std::cout << "return " << type.toStdString() << std::endl;

                        if(type == "T")
                        {
                            type = ttype;
                        }
                        //correct the return type if object is a templated class

                        return ScriptExpressionTypeSequence::FromType(ScriptExpressionType::FromCppFunctionReturn(name,type,false,{},true,ptypefull));

                    }
                }

                //check local class methods

                for(int i = 0; i < FunctionTokens.size(); i++)
                {
                    ScriptToken t = FunctionTokens.at(i);
                    if(t.is_function && !t.memberclass.isEmpty())
                    {
                        if(name == t.name && ptype == t.memberclass)
                        {
                            if(ScopeContains(t.scope,scriptpos))
                            {
                                //correct the return type if object is a templated class




                                std::cout << "scope okey" << std::endl;
                                return ScriptExpressionTypeSequence::FromType(ScriptExpressionType::FromFunctionReturn(name,t.Type,false,{},true,ptype,t));
                            }
                        }
                    }
                }




            }else if(parent_type.GetLast().is_namespace)
            {
                //check global functions in namespace
                //check local functions
                for(int i = 0; i < FunctionTokens.size(); i++)
                {
                    ScriptToken fi = FunctionTokens.at(i);
                    //can we find a match non-member function
                    if(fi.name == name && fi.memberclass.isEmpty())
                    {

                        std::cout << "namespace check " <<fi.nspace.toStdString() << " " << parent_type.GetNameSpaceFull().toStdString()<< std::endl;

                        if(fi.nspace == parent_type.GetNameSpaceFull())
                        {
                            std::cout << "namespace checked " << std::endl;

                            //check namespace?
                            //if(ScopeContains(fi.scope,scriptpos))
                            {

                                return ScriptExpressionTypeSequence::FromType(ScriptExpressionType::FromFunctionReturn(name,fi.Type,false,{},false,"",fi));

                            }
                        }

                    }

                }

            }else
            {

                ScriptExpressionTypeSequence::Unknown();
            }

        //array indexed list item
        }else if(expr.at(pos-1) == "." &&  expr.at(pos-2) == "]")
        {
            std::cout <<"index op detected " << std::endl;

            //ignore list index here
            int posn = pos -2;

            bool quote = false;
            bool brackets = 0;
            QString index_args = "";
            bool is_temp = false;
            while(posn > -1)
            {
                if(!quote)
                {
                    if(expr.at(posn) == "[")
                    {
                        if(brackets == 0)
                        {
                            break;
                        }else
                        {
                            brackets -= 1;
                        }

                    }
                    if(expr.at(posn) == "]")
                    {
                        brackets += 1;
                    }
                    if(expr.at(posn) == '"')
                    {
                        quote = !quote;
                    }
                }else
                {
                    if(expr.at(posn) == '"')
                    {
                        quote = !quote;
                    }
                }

                index_args = expr.at(posn) + index_args;


                posn = posn -1;
            }

            //try again

            //ScriptExpressionTypeSequence index_type = GetExpressionTypeString(index_args, index_args.size(),scriptpos,scope,true);

            ScriptExpressionTypeSequence type_prev = GetExpressionTypeString(expr, posn,scriptpos,scope,true);


            if(type_prev.GetLast().is_object || type_prev.GetLast().is_function)
            {

                //is there an index operator available for this type?

                //get all the OpIndex overloads
                //get all class methods, and check those

                QString ptype =type_prev.GetLast().type;
                QString ptypefull=  ptype;
                QString ttype = "";
                int brack_temp_here= 0;
                bool stop_temp_one = false;
                if(ptypefull.contains("<"))
                {
                    ptype = "";
                    for(int i = 0; i < ptypefull.length(); i++)
                    {
                        if(ptypefull.at(i) == "," && brack_temp_here == 1)
                        {
                            stop_temp_one = true;
                        }

                        if(ptypefull.at(i) != "<" && ptypefull.at(i) != " ")
                        {
                            if(brack_temp_here == 0)
                            {
                                ptype = ptype + ptypefull.at(i);
                            }else if(!stop_temp_one)
                            {
                                ttype = ttype + ptypefull.at(i);
                            }
                        }

                        if(ptypefull.at(i) == "<")
                        {
                            brack_temp_here += 1;
                        }
                        if(ptypefull.at(i) == "<")
                        {
                            brack_temp_here -= 1;
                            if(brack_temp_here == 0)
                            {
                                break;
                            }
                        }
                    }

                }

                //check global class methods
                for(int i = 0; i < funcmemberlist.size(); i++)
                {
                    ScriptFunctionInfo fi = funcmemberlist.at(i);
                    int tid = fi.Function->GetReturnTypeId();
                    QString type = m_ScriptManager->m_Engine->GetTypeName(tid);

                    std::cout << "check member func opIndex " <<  fi.ObjectName.toStdString() << " " << fi.Function->GetName() << " " << type.toStdString() << " " << ptype.toStdString() << std::endl;
                    if(QString(fi.Function->GetName()) == "opIndex" && ptype == fi.ObjectName)
                    {
                        std::cout << "found member func  " << std::endl;
                        std::cout << "return " << type.toStdString() << std::endl;

                        //correct the return type if object is a templated class

                        if(type == "T")
                        {
                            type = ttype;
                        }

                        return ScriptExpressionTypeSequence::FromType(ScriptExpressionType::FromCppFunctionReturn("opIndex",type,false,{},true,ptype));

                    }
                }

                //check local class methods

                for(int i = 0; i < FunctionTokens.size(); i++)
                {
                    ScriptToken t = FunctionTokens.at(i);
                    if(t.is_function && !t.memberclass.isEmpty())
                    {
                        if("opIndex" == t.name && ptype == t.memberclass)
                        {
                            if(ScopeContains(t.scope,scriptpos))
                            {
                                //correct the return type if object is a templated class




                                std::cout << "scope okey" << std::endl;
                                return ScriptExpressionTypeSequence::FromType(ScriptExpressionType::FromFunctionReturn("opIndex",t.Type,false,{},true,ptype,t));
                            }
                        }
                    }
                }

                return ScriptExpressionTypeSequence::Unknown();

            }else
            {
                return ScriptExpressionTypeSequence::Unknown();
            }



        //object method (or static class method dont exist)
        }else if(expr.at(pos-1) == ".")
        {
            std::cout << "object prop detected " << std::endl;
            int posn = pos -2;
            //get object name
            QString name = "";
            while(posn > -1)
            {
                if(expr.at(posn).isLetterOrNumber() || expr.at(posn) == "_")
                {
                    name = expr.at(posn) + name;
                }else
                {
                    break;
                }

                posn = posn -1;

            }

            ScriptExpressionTypeSequence parent_type;

            //see if it is an object of someting else,
            if(posn -1 > 0)
            {
                if(expr.at(posn-1) == ".")
                {
                    std::cout << "parent detected " << QString(expr.at(pos-1)).toStdString() <<std::endl;

                    bool is_file = false;

                    QString filetype = "";
                    //see if this object is a specific supported file type
                    for(int i = 0; i < m_FileExtensionsList.size(); i++)
                    {
                        for(int j = 0; j < m_FileExtensionsList.at(i).size(); j++)
                        {
                            QString ext = m_FileExtensionsList.at(i).at(j);
                            if( "." + name == ext)
                            {
                                filetype = m_FileTypesList.at(i);
                                break;
                            }

                        }
                    }

                    if(is_file)
                    {
                        posn = pos -2;
                        //get object name
                        QString name = "";
                        while(posn > -1)
                        {
                            if(expr.at(posn).isLetterOrNumber() || expr.at(posn) == "_")
                            {
                                name = expr.at(posn) + name;
                            }else
                            {
                                break;
                            }

                            posn = posn -1;

                        }

                        return ScriptExpressionTypeSequence::FromType(ScriptExpressionType::FromCppType(name,filetype));


                    }else
                    {

                        //see if it is an object or function result
                        //get that other thing first

                       parent_type =  GetExpressionTypeString(expr, posn,scriptpos,scope,true);



                    }


                }
            }


            //then try to resolve this
            //we have parent_type and name

            if(parent_type.GetLast().is_empty)
            {
                for(int i = 0; i < VariableTokens.size(); i++)
                {
                    ScriptToken ti = VariableTokens.at(i);

                    std::cout << "check variable " << ti.name.toStdString() << " " << ti.Type.toStdString() <<  std::endl;
                    if(ti.name == name)
                    {
                        //check scope
                        std::cout << "found variable "<< ti.name.toStdString() << " " << ti.Type.toStdString() <<  std::endl;

                        if(ti.scope != nullptr)
                        {
                            std::cout << "scope " << ti.scope->pos_begin << " " << ti.scope->pos_end << " " << scriptpos << std::endl;
                        }else
                        {
                            std::cout << "scope nullptr " << std::endl;
                        }

                        if(ScopeContains(ti.scope,scriptpos))
                        {
                            std::cout << "scope checked, found type, empty parent"<<  std::endl;

                            return ScriptExpressionTypeSequence::FromType(ScriptExpressionType::FromType(name,ti.Type,false,{},false,{},ti));
                        }
                    }

                }

            }else if(parent_type.GetLast().is_object || parent_type.GetLast().is_function)
            {

                for(int i = 0; i < VariableTokens.size(); i++)
                {
                    ScriptToken ti = VariableTokens.at(i);

                    std::cout << "check variable " << ti.name.toStdString() << " " << ti.Type.toStdString() <<  std::endl;
                    if(ti.name == name)
                    {
                        //check scope
                        std::cout << "found variable "<< ti.name.toStdString() << " " << ti.Type.toStdString() <<  std::endl;

                        //if(ScopeContains(ti.scope,scriptpos))
                        {
                            std::cout << "scope checked"<<  std::endl;


                            std::cout << "class check " << ti.memberclass.toStdString() << " " << parent_type.GetLast().type.toStdString()<< std::endl;

                            if(ti.memberclass == parent_type.GetLast().type)
                            {
                                std::cout << "class checked " << std::endl;
                                return ScriptExpressionTypeSequence::FromType(ScriptExpressionType::FromType(name,ti.Type,false,{},true,ti.memberclass,ti));
                            }
                        }
                    }

                }

            }else if(parent_type.GetLast().is_namespace)
            {

                for(int i = 0; i < VariableTokens.size(); i++)
                {
                    ScriptToken ti = VariableTokens.at(i);

                    std::cout << "check variable " << ti.name.toStdString() << " " << ti.Type.toStdString() <<  std::endl;
                    if(ti.name == name)
                    {
                        //check scope
                        std::cout << "found variable "<< ti.name.toStdString() << " " << ti.Type.toStdString() <<  std::endl;

                        //if(ScopeContains(ti.scope,scriptpos))
                        {
                            std::cout << "scope checked"<<  std::endl;


                            std::cout << "namespace check " <<ti.nspace.toStdString() << " " << parent_type.GetNameSpaceFull().toStdString()<< std::endl;

                            if(ti.nspace == parent_type.GetNameSpaceFull())
                            {
                                std::cout << "namespace checked " << std::endl;
                                return ScriptExpressionTypeSequence::FromType(ScriptExpressionType::FromCppType(name,ti.Type));
                            }
                        }
                    }

                }



            }else
            {

                return ScriptExpressionTypeSequence::Unknown();
            }



        }

        //check for namespace resolver
        if(expr.at(pos-1) == ":" && expr.at(pos-2) == ":")
        {
            std::cout << "namespace detected " << std::endl;
            //get full namespace name
            int posn = pos-3;

            QString name = "";
            while(posn > -1)
            {
                if(expr.at(posn).isLetterOrNumber() || expr.at(posn) == "_")
                {
                    name = expr.at(posn) + name;
                }else
                {
                    break;
                }
                posn = posn -1;
            }

            //is there another namespace
            if(expr.at(posn-1) == ":" && expr.at(posn) == ":")
            {

                //we have a namespace to resolve
                //do it
                return GetExpressionTypeString(expr, posn-2,scriptpos,scope,true).Appended(ScriptExpressionType::FromNameSpace(name));



            }
            //then resolve this
            return ScriptExpressionTypeSequence::FromType(ScriptExpressionType::FromNameSpace(name));

        }

        std::cout << "undealt so empty return " << std::endl;
    }

    return ScriptExpressionTypeSequence::FromType(type_ret);


}

QString ScriptAnalyzer::GetArithmeticOperatorType(QString type1, QString type2, QString oper, int scriptpos)
{

    return "";


}

ScriptExpressionType ScriptAnalyzer::GetArithmeticStatementType(QString statement, int scriptpos)
{

    /*In expressions, the operator with the highest precedence is always computed first.

Unary operators
Unary operators have the higher precedence than other operators, and between unary operators the operator closest to the actual value has the highest precedence. Post-operators have higher precedence than pre-operators.

This list shows the available unary operators.

::	scope resolution operator
[]	indexing operator
++ --	post increment and decrement
.	member access
++ --	pre increment and decrement
not !	logical not
+ -	unary positive and negative
~	bitwise complement
@	handle of
Binary and ternary operators
This list shows the dual and ternary operator precedence in decreasing order.

**	exponent
* / %	multiply, divide, and modulo
+ -	add and subtract
<< >> >>>	left shift, right shift, and arithmetic right shift
&	bitwise and
^	bitwise xor
|	bitwise or
<= < >= >	comparison
== != is !is xor ^^	equality, identity, and logical exclusive or
and &&	logical and
or ||	logical or
?:	condition
= += -= *= /= %= **= &=
|= ^= <<= >>= >>>=	assignment and compound assignments*/


    //split the string into items (type to be determined) and operators, taking into account paranthesis

    //binary expression tree must be generated, each item must then be analyzed for type


    //operators and objects grouped based on operator presedence


    //then each operator must be checked for its resulting type

    //finally the resulting type at the top of the tree matters



    ScriptExpressionType t;
    t.is_unknown = true;
    t.is_empty = false;
    return t;

}


QString ScriptAnalyzer::GetExpressionLeftFull(int pos)
{
    std::string front;
    for(int i = 0; i < 10; i++)
    {
        if(pos + i < m_Script.size())
        {
            front = front + m_Script.at(pos + i).toLatin1();
        }
    }

    std::cout << "where are we? " << front << std::endl;

    //get the entire dependency string
    QString fullterm = "";
    int brack=  0;
    bool quo = false;
    if(InRange(pos -2))
    {
        if(m_Script.at(pos-1) == "." || m_Script.at(pos-1) == ":")
        {
            int posn = pos -1;
            while(posn > -1)
            {
                if(m_Script.at(posn) == ")" || m_Script.at(posn) == "}" || m_Script.at(posn) == "]")
                {
                    brack += 1;

                }
                if(m_Script.at(posn) == "(" || m_Script.at(posn) == "{" || m_Script.at(posn) == "[")
                {
                    brack -= 1;

                }
                if(m_Script.at(posn) == '"')
                {
                    quo = !quo;
                }
                //any character that breaks a scope dependency when not inside brackets
                if(m_Script.at(posn) == "+" ||
                        m_Script.at(posn) == "-" ||
                        m_Script.at(posn) == "/" ||
                        m_Script.at(posn) == " " ||
                        m_Script.at(posn) == "!" ||
                        m_Script.at(posn) == "@" ||
                        m_Script.at(posn) == "*" ||
                        m_Script.at(posn) == "=" ||
                        m_Script.at(posn) == '`' ||
                        m_Script.at(posn) == '~' ||
                        m_Script.at(posn) == '|' ||
                        m_Script.at(posn) == '"' ||
                        m_Script.at(posn) == "\t")
                {
                    if(brack == 0 && !quo)
                    {
                        break;
                    }
                }
                fullterm = m_Script.at(posn) + fullterm;
                posn = posn -1;
            }


        }
    }

    return fullterm;
}

QString ScriptAnalyzer::GetExpressionRightFull(int pos)
{
    std::string front;
    for(int i = 0; i < 10; i++)
    {
        if(pos + i < m_Script.size())
        {
            front = front + m_Script.at(pos + i).toLatin1();
        }
    }

    std::cout << "where are we front? " << front << std::endl;

    //get the entire dependency string
    QString fullterm = "";
    int brack=  0;
    bool quo = false;
    int posn = pos;
    if(InRange(pos))
    {
        int n_brack = 0;
        bool is_quo = false;
        //check if templated call (constructor of templated class type)
        if(m_Script.at(pos) == "<")
        {
            while(pos < m_Script.size())
            {
                fullterm = fullterm + m_Script.at(pos);

                if(m_Script.at(pos) == "<" && !is_quo)
                {
                    n_brack = n_brack + 1;
                }

                if(m_Script.at(pos) == '"')
                {
                    is_quo = !is_quo;
                }

                if(m_Script.at(pos) == ">" && !is_quo)
                {
                    n_brack = n_brack - 1;
                    if(n_brack == 0)
                    {
                        break;
                    }
                }



               pos = pos + 1;
            }


            posn = pos + 1;

        }
    }

    if(InRange(posn))
    {
        int n_brack = 0;
        bool is_quo = false;

        if(m_Script.at(posn) == "(")
        {
            while(posn < m_Script.size())
            {
                fullterm = fullterm + m_Script.at(posn);

                if(m_Script.at(posn) == "(" && !is_quo)
                {
                    n_brack = n_brack + 1;
                }

                if(m_Script.at(posn) == '"')
                {
                    is_quo = !is_quo;
                }

                if(m_Script.at(posn) == ")" && !is_quo)
                {
                    n_brack = n_brack - 1;
                    if(n_brack == 0)
                    {
                        break;
                    }
                }

               posn = posn + 1;
            }


        }
    }

    return fullterm;
}



std::vector<ScriptReferenceResult> ScriptAnalyzer::GetReferencesTo(QString name, int pos)
{

    //get all tokens with identical name, and then check for compatible scopes
    //this function can only be called when the script was analyzed first


    //first get the requested scope
    ScriptScope * scope = GetScopeAtPos(pos);

    //check if we are looking at a member of a class or namespace
    QString namespaceleft = GetExpressionLeftFull(pos);
    QString termright = GetExpressionRightFull(pos + name.size());

    namespaceleft = namespaceleft + name + termright +  ".";

    std::cout << "get type of " << namespaceleft.toStdString() << std::endl;

    ScriptExpressionTypeSequence ets = GetExpressionTypeString(namespaceleft,namespaceleft.size(),pos);

    ScriptExpressionType type;

    if(ets.m_ExpressionTypes.size() > 0)
    {
        type = ets.m_ExpressionTypes.at(ets.m_ExpressionTypes.size()-1);
    }else
    {
        type = ScriptExpressionType();
    }

    std::cout << "type is " << type.name.toStdString() << " " << type.type.toStdString() << std::endl;

    //get all the other potential tokens that have the same name and follow the same definition

    std::vector<ScriptReferenceResult> res;

    for(int i = 0; i < Tokens.size(); i++)
    {
        //check if token matches the name of our search
        if(Tokens.at(i).text == name)
        {

            //next up, create a string that represents the expression where it is used
            QString namespacelefth = GetExpressionLeftFull(Tokens.at(i).pos);
            QString termrighth = GetExpressionRightFull(Tokens.at(i).pos + Tokens.at(i).length);

            //next, alter the expression to make it usable with our GetExpressionType function (so with a . after the item whe wish to get the type from)
            namespacelefth = namespacelefth + Tokens.at(i).text + termrighth + ".";

            //do the analysis
            ScriptExpressionTypeSequence etsh = GetExpressionTypeString(namespacelefth,namespacelefth.size(),Tokens.at(i).pos);

            ScriptExpressionType typeh;

            if(etsh.m_ExpressionTypes.size() > 0)
            {
                typeh = etsh.m_ExpressionTypes.at(etsh.m_ExpressionTypes.size()-1);
            }else
            {
                typeh = ScriptExpressionType();
            }

            //now check if the last type in the returned sequence is similarly a function, member-function, variable, member-variable, class, namespace, interface or enum
            //and follows from the same definition
            if(typeh == type)
            {
                ScriptReferenceResult resi;
                resi.name = name;
                resi.expression = namespacelefth;
                resi.pos = Tokens.at(i).pos;

                ScriptLocation loc = GetLocationFromPosition(Tokens.at(i).pos);

                resi.line = GetCodeLine(loc.line_n);
                resi.row_n = loc.line_n;
                resi.col_n = loc.col_n;

                res.push_back(resi);




            }
        }
    }

    return res;
}

QString ScriptAnalyzer::GetCodeLine(int line )
{
    if(line <0)
    {
        return "";
    }
    if(line < m_Lines.size())
    {
        return m_Lines.at(line);
    }else
    {
        return "";
    }
}

std::vector<AutoCompleteOption> ScriptAnalyzer::GetAutoCompletes(QString name, int pos)
{


    return {};
}

ScriptLocation ScriptAnalyzer::GetExpressionDefinition(QString name, int pos)
{

    //first get the requested scope
    ScriptScope * scope = GetScopeAtPos(pos);

    //check if we are looking at a member of a class or namespace
    QString namespaceleft = GetExpressionLeftFull(pos);
    QString termright = GetExpressionRightFull(pos + name.size());

    namespaceleft = namespaceleft + name + termright +  ".";

    std::cout << "get type of " << namespaceleft.toStdString() << std::endl;

    ScriptExpressionTypeSequence ets = GetExpressionTypeString(namespaceleft,namespaceleft.size(),pos);

    ScriptExpressionType type;

    if(ets.m_ExpressionTypes.size() > 0)
    {
        type = ets.m_ExpressionTypes.at(ets.m_ExpressionTypes.size()-1);
    }else
    {
        type = ScriptExpressionType();
    }

    std::cout << "type is " << type.name.toStdString() << " " << type.type.toStdString() << std::endl;


    if(type.is_cpp)
    {
        ScriptLocation ret = ScriptLocation();
        ret.is_cpp =true;
        return ret;
    }else
    {
        ScriptLocation ret = ScriptLocation();
        ret = GetLocationFromPosition(type.def.pos);
        return ret;
    }
}


void ScriptAnalyzer::FindSomeErrors()
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


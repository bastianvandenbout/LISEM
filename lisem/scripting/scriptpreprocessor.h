#ifndef SCRIPTPREPROCESSOR_H
#define SCRIPTPREPROCESSOR_H

#include "QString"
#include "QList"
#include "QRegExp"
#include "QRegularExpression"

#include "extensionprovider.h"


inline static QString BeautifyScript(QString script)
{

    //style example:
    //
    //
    //
    //void main(type arg1, type<subtype> arg2 = {"a","b","c"}, ...)
    //{
    //  float a = arg1;
    //
    //  if(a < 3)
    //  {
    //      for(int i = 0; i < 10; i++)
    //      {
    //          a = a + 1;
    //      }
    //  }
    //
    //  return a;
    //}

    //removal of unneccesary empty lines
    //addition of empty lines before if, for, while and other control statements that are not directly nested
    //addition of newline after ";", "for(...)", after a function header, before and afer "{","}",class, namespace and enum, with exception of list initialization which can be detected by the absence of any ";" (other scopes without a ; will similarly be minimized but thats okey
    //removal of unneccesary spaces
    //addition of spaced in some places such as after comma in function arguments,between comparison operator and arguments,between different elements of for loop
    //right indentation, one tab per nested scope

    //how we achieve this:
    //simplistic, and probably with some faults, but good enough for all my use-cases so far
    //go through the code character by character, and check
    //1: when multiple spaces, reduce to 1
    //2: keep track of indentation requirements through the net amount of non-quated opening brackets
    //3: check for each opening bracket where the associated closing bracket is, and if there is a statement in-between
    //4: when multiple new-lines, reduce to 1
    //5: detect for loop as  for(..;..;..), get items, trim, and style

    //it also prevents some unicode confusion
    //word uses U+201C and U+201D left and right quotation marks.
    //we want to instead use U+0022, which is the normal neutral quotation mark
    //and U+2001, U+2002 and U+2003 should all be normal space (U+0020), instead of em space, em quad space etc..

    script.replace(QString::fromUtf8("\u201D"),"\"");
    script.replace(QString::fromUtf8("\u202D"),"\"");
    script.replace(QString::fromUtf8("\u2003")," ");
    script.replace(QString::fromUtf8("\u2002")," ");
    script.replace(QString::fromUtf8("\u2001")," ");

    //remove all newline (except for comment lines) and spaces that are more than one
    //within our scripting language, this allows for easier processing
    //there is an exception later we must account for, which is the switch statements, which are dependent on formatting with tabs
    QRegExp rews("\\s+");
    QRegExp rewnl("\\r?\\n");

    bool is_quote_0 = false;
    bool is_commentline = false;
    bool is_commentlong0 = false;
    for(int i = 0; i < script.length();i++)
    {
        QString si;
        si.append(script.at(i));
        QChar next = '-';
        bool has_next = true;
        if( i == script.length()-1)
        {
            has_next = false;
        }else
        {
            next = script.at(i+1);
        }
        if(si == "\"")
        {
            is_quote_0 = !is_quote_0;
        }
        if((rewnl.exactMatch(si) || si == "\n" || si == "\r\n")&& !is_quote_0)
        {
            if(!is_commentline && !is_commentlong0)
            {
                script.remove(i,1);
                script.insert(i," ");
            }
            is_commentline = false;

        }
        if((si == "/" && next == "/") && !is_quote_0)
        {
            is_commentline = true;
        }
        if((si == "/"  && next == "*") && !is_quote_0)
        {
            is_commentlong0 = true;
        }
        if((si == "*" && next == "/") && is_commentlong0)
        {
            is_commentlong0 = false;
        }
    }

    //remove newlines, replace with spaces
    //script.replace(rewnl," ");

    //remove any space that is more than one in sequence, and not within quotes
    bool is_quote_1 = false;
    bool last_space = false;
    bool this_space = false;
    for(int i = 0; i < script.length();i++)
    {
        QString si;
        si.append(script.at(i));
        if(si == "\"")
        {
            is_quote_1 = !is_quote_1;
        }
        if(rews.exactMatch(si) && !(rewnl.exactMatch(si) || si == "\n" || si == "\r\n"))
        {
            this_space = true;
        }else
        {
            this_space = false;
        }
        if((!is_quote_1) && (this_space && last_space))
        {
            script.remove(i,1);
            i -=1;
        }
        last_space = this_space;
    }
    bool is_quote = false;
    bool is_for = false;
    bool is_switch = false;
    bool assign_bracket = false;
    int brackets_since_assign = 0;
    bool is_comment = false;
    bool is_commentlong = true;
    int bracket_count = 0;
    int scope_count = 0;
    QList<int> scope_end_index;
    QList<bool> scope_end_index_contains_expression;
    int length_change = 0;

    std::cout << script.toStdString() << std::endl;
    std::cout << std::endl;
    //first replace all non-standard versions of white-space and newline with standard ones

    int i = 0;
    while(i < script.length())
    {
        QChar chari = script.at(i);
        int length_remain = script.length() - i;

        int length_prev_change = 0;
        int length_next_change = 0;
        int jumplength = 0;
        bool jumpword = false;
        QString token;
        int token_length = 0;

        QChar next;
        QChar prev;
        bool has_prev = true;
        bool has_next = true;
        if(i == 0)
        {
            has_prev = false;
        }else
        {
            prev = script.at(i-1);
        }
        if( i == script.length()-1)
        {
            has_next = false;

        }else
        {

            next = script.at(i+1);
        }

        for(int j = i; j < script.length(); j++)
        {
            QChar charj = script.at(j);
            QString t; t.append(charj);
            if(rews.exactMatch(t) || charj == '=' || charj == ' ' || charj == '!' ||
                    charj == '@' || charj == '#' || charj == '$' ||
                    charj == '%' || charj == '^' || charj == '&' ||
                charj == '*' || charj == '(' || charj == ')' ||
                        charj == '{' || charj == '}' || charj == '[' ||
                        charj == ']' || charj == '"' || charj == '\'' ||
                        charj == '<' || charj == '>' || charj == '!' ||
                        charj == '\\' || charj == '|' || charj == '/' ||
                        charj == ',' || charj == '.' || charj == '+' ||
                        charj == '~' || charj == '`' || charj == ';'
                    || j == script.length() -1)
            {
                if(j == i)
                {
                    token_length =1;
                    token = chari;

                }else
                {
                }

                break;
            }else
            {
                token_length += 1;
                token += charj;
            }
        }

        if(token_length > 1)
        {
            if( i + token_length == script.length()-1)
            {
                has_next = false;

            }else
            {

                next = script.at(i+token_length);
            }
        }
        jumplength = token_length;
        jumpword = true;
        if(chari == "\"")
        {
            is_quote = !is_quote;
        }
        if(!is_quote)
        {
            if(chari == '/' && has_next && next == '/')
            {
                script.insert(i-1,"\n");
                length_prev_change += 1;
                i = i +1;
            }
            if(chari == '/' && has_next && next == '/' && !is_comment)
            {
                is_comment = true;
                is_commentlong = false;
            }
            if(chari == '/' && has_next && next == '*' && !is_comment)
            {
                is_comment = true;
                is_commentlong = true;

            }
            if(is_comment && is_commentlong)
            {
                if(chari == '*' && has_next && next == '/')
                {
                    is_comment = false;
                    is_commentlong = false;

                }
            }


            if(token =="\n")
            {
                //do nothing for now
                if(is_comment && !is_commentlong)
                {
                    is_comment = false;
                }

                if(has_next && (next == " " || next == "\t"))
                {
                    script.remove(i+1,1);
                    length_change -= 1;
                    if(i == script.length()-1)
                    {
                        has_next = false;
                    }else
                    {
                        next = script.at(i+1);
                    }

                }


                //add proper number of tabs
                for(int k = 0; k < scope_count; k++)
                {
                    script.insert(i+token_length + length_next_change,'\t');
                    length_next_change += 1;
                }
                length_change += scope_count;
                jumplength += scope_count;


            }

            if(!is_comment)
            {
                if(chari == '(')
                {
                    if(is_for && !is_comment)
                    {
                        bracket_count += 1;
                    }

                }
                if(chari == ')')
                {
                    if(is_for && !is_comment)
                    {
                        bracket_count -= 1;
                        if(bracket_count == 0)
                        {
                            is_for = false;
                        }
                        bracket_count = std::max(0,bracket_count);
                    }
                }
            }


            if(!is_comment)
            {
                if(chari == '{')
                {
                    std::cout << "bracket " << has_prev << " " << prev.toLatin1() << std::endl;
                    //if the previous is a closing bracket ")", we have to add a newline and some tabs

                    if(has_prev && prev == " ")
                    {
                        script.remove(i-1,1);
                        i = i-1;
                        length_change -= 1;
                        if(i == 0)
                        {
                            has_prev = false;
                        }else
                        {
                            prev = script.at(i-1);
                        }

                    }
                    //check if previous characters are newlines, otherwise add them
                    if(has_prev && prev == ")")
                    {


                        script.insert(i,"\n");
                        length_prev_change += 1;
                        i = i +1;

                        for(int k = 0; k < scope_count; k++)
                        {
                            script.insert(i,'\t');
                            length_prev_change += 1;
                            i = i+1;
                        }
                        length_change += 1 + scope_count;

                    }
                    //add a scope level, and indent level

                    scope_count += 1;
                    //find the closing bracket for this one, and keep track if there is a statement in-between

                    //add a newline with the proper number of tabs

                    if(!(has_prev && prev == "="))
                    {
                        if(assign_bracket)
                        {
                            brackets_since_assign += 1;
                        }
                        //add newline with propert number of tabs
                        script.insert(i+token_length,"\n");
                        length_next_change +=1;
                        for(int k = 0; k < scope_count; k++)
                        {
                            script.insert(i+token_length + length_next_change,'\t');
                            length_next_change += 1;
                        }
                        length_change += 1 + scope_count;
                        jumplength += 1 + scope_count;
                    }else
                    {
                        assign_bracket = true;
                        brackets_since_assign = 0;
                    }

                }
                if(token == "}")
                {
                    //add a newline with the propert number of tabs
                    //and another newline with the proper number of tabs

                    bool dont_newline = false;
                    if(assign_bracket)
                    {
                        brackets_since_assign -= 1;
                        if(brackets_since_assign == 0)
                        {
                             dont_newline = true;
                             assign_bracket = false;
                        }
                        brackets_since_assign = std::max(0,brackets_since_assign);
                    }
                    if(has_prev && prev == " ")
                    {
                        script.remove(i-1,1);
                        i = i-1;
                        length_change -= 1;
                        if(i == 0)
                        {
                            has_prev = false;
                        }else
                        {
                            prev = script.at(i-1);
                        }

                    }

                    if(has_next && next == " ")
                    {
                        script.remove(i+1,1);
                        length_change -= 1;
                        if(i == script.length()-1)
                        {
                            has_next = false;
                        }else
                        {
                            next = script.at(i+1);
                        }

                    }



                    if(!dont_newline)
                     {
                        script.insert(i,"\n");
                        length_prev_change += 1;
                        i = i +1;
                        for(int k = 0; k < scope_count-1; k++)
                        {
                            script.insert(i,'\t');
                            length_prev_change += 1;
                            i = i+1;
                        }

                        scope_count -= 1;
                        scope_count = std::max(0,scope_count);

                        //add newline with propert number of tabs
                        script.insert(i+token_length,"\n");
                        length_next_change +=1;
                        for(int k = 0; k < scope_count-1; k++)
                        {
                            script.insert(i+token_length + length_next_change,'\t');
                            length_next_change += 1;
                        }
                        length_change += 1 + scope_count-1;
                        jumplength += 1 + scope_count-1;
                    }
                }
                if(token== "class" || token== "enum" || token== "namespace " || token== "else" || token== "try" || token== "catch"|| token== "do")
                {
                    if(has_prev && prev == " ")
                    {
                        script.remove(i-1,1);
                        i = i-1;
                        length_change -= 1;
                        if(i == 0)
                        {
                            has_prev = false;
                        }else
                        {
                            prev = script.at(i-1);
                        }

                    }

                    //remove any spaces and newlines
                    //add one newline
                    //check if next character is opening brace
                    //add one newline
                    script.insert(i+token_length,"\n");
                    length_next_change +=1;
                    for(int k = 0; k < scope_count; k++)
                    {
                        script.insert(i+token_length + length_next_change,'\t');
                        length_next_change += 1;
                    }
                    length_change += 1 + scope_count;
                    jumplength += 1 + scope_count;

                }
                if(token == ";")
                {
                    if(has_prev && prev == " ")
                    {
                        script.remove(i-1,1);
                        i = i-1;
                        length_change -= 1;
                        if(i == 0)
                        {
                            has_prev = false;
                        }else
                        {
                            prev = script.at(i-1);
                        }

                    }

                    if(has_next && next == " ")
                    {
                        script.remove(i+1,1);
                        length_change -= 1;
                        if(i == script.length()-1)
                        {
                            has_next = false;
                        }else
                        {
                            next = script.at(i+1);
                        }

                    }

                    //add newline with propert number of tabs
                    if(!is_for)
                    {
                        script.insert(i+token_length,"\n");
                        length_next_change +=1;
                        for(int k = 0; k < scope_count; k++)
                        {
                            script.insert(i+token_length + length_next_change,'\t');
                            length_next_change += 1;
                        }
                        length_change += 1 + scope_count;
                        jumplength += 1 + scope_count;
                    }else
                    {
                        script.insert(i+token_length," ");
                        length_next_change +=1;
                    }
                }

                if(token == "break")
                {
                    if(has_prev && prev == " ")
                    {
                        script.remove(i-1,1);
                        i = i-1;
                        length_change -= 1;
                        if(i == 0)
                        {
                            has_prev = false;
                        }else
                        {
                            prev = script.at(i-1);
                        }

                    }

                    //if next one is space, remove
                    if(has_next && next == ' ')
                    {
                        script.remove(i+token_length,1);
                        if(script.length() > i+token_length)
                        {
                            next = script.at(i+token_length);
                        }else
                        {
                            has_next = false;
                        }
                         length_change -= 1;
                    }
                    //if next one is ";", keep
                    if(has_next && next == ';')
                    {

                    }else
                    {
                        script.insert(i + token_length,";");
                        length_next_change + 1;
                        jumplength += 1;
                    }
                    //add newline with propert number of tabs
                    script.insert(i +token_length + length_next_change,"\n");
                    length_next_change += 1;
                    for(int i = 0; i < scope_count; i++)
                    {
                        script.insert(i +token_length+length_next_change,'\t');
                        length_next_change += 1;
                    }
                    length_change += 1 + scope_count;
                    jumplength += 1 + scope_count;
                }
                if(token== "defeault")
                {
                    if(has_prev && prev == " ")
                    {
                        script.remove(i-1,1);
                        i = i-1;
                        length_change -= 1;
                        if(i == 0)
                        {
                            has_prev = false;
                        }else
                        {
                            prev = script.at(i-1);
                        }

                    }

                    //if next one is space, remove
                    if(has_next && next == ' ')
                    {
                        script.remove(i+token_length,1);
                        if(script.length() > i+token_length)
                        {
                            next = script.at(i+token_length);
                        }else
                        {
                            has_next = false;
                        }
                         length_change -= 1;
                    }
                    //if next one is ";", keep
                    if(has_next && next == ':')
                    {

                    }else
                    {
                        script.insert(i + token_length,":");
                        length_next_change + 1;
                        jumplength += 1;
                    }
                    //add newline with propert number of tabs
                    script.insert(i +token_length + length_next_change,"\n");
                    length_next_change += 1;
                    for(int i = 0; i < scope_count; i++)
                    {
                        script.insert(i +token_length+length_next_change,'\t');
                        length_next_change += 1;
                    }
                    length_change += 1 + scope_count;
                    jumplength += 1 + scope_count;
                }
                if(token == "for")
                {
                    if(has_prev && prev == " ")
                    {
                        script.remove(i-1,1);
                        i = i-1;
                        length_change -= 1;
                        if(i == 0)
                        {
                            has_prev = false;
                        }else
                        {
                            prev = script.at(i-1);
                        }

                    }

                    //get starting parenthesis, and remove any spaces
                    is_for = true;
                }
                if(token== "while" || token== "if " || token == "else if" || token== "switch")
                {
                    if(has_prev && prev == " ")
                    {
                        script.remove(i-1,1);
                        i = i-1;
                        length_change -= 1;
                        if(i == 0)
                        {
                            has_prev = false;
                        }else
                        {
                            prev = script.at(i-1);
                        }

                    }

                    //we go directly to the closing parenthesis, check if the next character is opening braces
                    //if so, remove
                    //add newline, proper tabs, opening brace, newline, proper tabs
                }
                if(token== "return")
                {
                    if(has_prev && prev == " ")
                    {
                        script.remove(i-1,1);
                        i = i-1;
                        length_change -= 1;
                        if(i == 0)
                        {
                            has_prev = false;
                        }else
                        {
                            prev = script.at(i-1);
                        }

                    }

                }
                if(token== "switch")
                {
                    if(has_prev && prev == " ")
                    {
                        script.remove(i-1,1);
                        i = i-1;
                        length_change -= 1;
                        if(i == 0)
                        {
                            has_prev = false;
                        }else
                        {
                            prev = script.at(i-1);
                        }

                    }

                    //this one is more complex, get the full switch statement, then get the cases/defeaults within that, add spaces after
                    //then add a space before the ":", and finally a newline with proper tabs after that.

                }
                if(token== "case")
                {
                    if(has_prev && prev == " ")
                    {
                        script.remove(i-1,1);
                        i = i-1;
                        length_change -= 1;
                        if(i == 0)
                        {
                            has_prev = false;
                        }else
                        {
                            prev = script.at(i-1);
                        }

                    }

                }
            }


        }

        std::cout << "jump token: " << is_quote << " " << is_comment << " " << is_for << " " <<  jumplength << " " << token_length << " " << token.toStdString() << std::endl;
        if(jumpword)
        {
           i = i + jumplength;
        }else
        {
           i++;
        }

    }

    return script;

}

inline static QString PreProcessScript(QString script)
{

    //the preprocessor simplifies simple calculations in the scripting environment
    //it changes simple map names to the appropriate loading and saving functions
    //example:
    // save a.map = b.map + c.map;
    // is converted to
    // SaveThisMap("a.map") = LoadMap("b.map") + LoadMap("c.map");
    //
    // The SaveThisMap function returns a temporary map object that will store when the assignment operator is called
    // afterwards, its references are automatically deleted
    // however, a.map is not stored in memory

    //QStringList sl = script.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);

    //it also prevents some unicode confusion
    //word uses U+201C and U+201D left and right quotation marks.
    //we want to instead use U+0022, which is the normal neutral quotation mark
    //and U+2001, U+2002 and U+2003 should all be normal space (U+0020), instead of em space, em quad space etc..

    script.replace(QString::fromUtf8("\u201D"),"\"");
    script.replace(QString::fromUtf8("\u202D"),"\"");
    script.replace(QString::fromUtf8("\u2003")," ");
    script.replace(QString::fromUtf8("\u2002")," ");
    script.replace(QString::fromUtf8("\u2001")," ");

    QList<QString> list_namesload;
    QList<QString> list_namessave;

    QList<int> reparg1;
    QList<int> reparg2;
    QList<QString> reparg3;
    QList<int> reparg4;

    QString output;

    QList<QString> mapexts = GetMapExtensions();
    QList<QString> shapeexts = GetShapeExtensions();
    QList<QString> tableexts = GetTableExtensions();
    QList<QString> pointexts = GetPointCloudExtensions();
    QList<QString> modelexts = GetModelExtensions();
    QList<QString> fieldexts = GetFieldExtensions();
    QList<QString> rigidworldexts = GetRigidWorldExtensions();

    QStringList exts;
    std::vector<bool> shape;
    std::vector<bool> table;
    std::vector<bool> pointcloud;
    std::vector<bool> map;
    std::vector<bool> model;
    std::vector<bool> field;
    std::vector<bool> rigidworld;

    for(int i = 0; i < mapexts.length(); i++)
    {
        exts.append(mapexts.at(i));
        shape.push_back(false);
        table.push_back(false);
        pointcloud.push_back(false);
        map.push_back(true);
        model.push_back(false);
        field.push_back(false);
        rigidworld.push_back(false);
    }

    for(int i = 0; i < shapeexts.length(); i++)
    {
        exts.append(shapeexts.at(i));
        shape.push_back(true);
        table.push_back(false);
        pointcloud.push_back(false);
        map.push_back(false);
        model.push_back(false);
         field.push_back(false);
         rigidworld.push_back(false);
    }

    for(int i = 0; i < pointexts.length(); i++)
    {
        exts.append(pointexts.at(i));
        shape.push_back(false);
        table.push_back(false);
        pointcloud.push_back(true);
        map.push_back(false);
        model.push_back(false);
         field.push_back(false);
         rigidworld.push_back(false);
    }
    for(int i = 0; i < tableexts.length(); i++)
    {
        exts.append(tableexts.at(i));
        shape.push_back(false);
        table.push_back(true);
        pointcloud.push_back(false);
        map.push_back(false);
        model.push_back(false);
         field.push_back(false);
         rigidworld.push_back(false);
    }


    for(int i = 0; i < modelexts.length(); i++)
    {
        exts.append(modelexts.at(i));
        shape.push_back(false);
        table.push_back(false);
        pointcloud.push_back(false);
        map.push_back(false);
        model.push_back(true);
         field.push_back(false);
         rigidworld.push_back(false);
    }

    for(int i = 0; i < fieldexts.length(); i++)
    {
        exts.append(fieldexts.at(i));
        shape.push_back(false);
        table.push_back(false);
        pointcloud.push_back(false);
        map.push_back(false);
        model.push_back(false);
         field.push_back(true);
         rigidworld.push_back(false);
    }
    for(int i = 0; i < rigidworldexts.length(); i++)
    {
        exts.append(rigidworldexts.at(i));
        shape.push_back(false);
        table.push_back(false);
        pointcloud.push_back(false);
        map.push_back(false);
        model.push_back(false);
         field.push_back(false);
         rigidworld.push_back(true);
    }

    /*QStringList exts = {".map",".tif",".asc",".shp",".gpkg",".osm",".csv",".tbl",".las"};
    bool shape[9] = {false,false,false,true,true,true,false,false,false};
    bool table[9] = {false,false,false,false,false,false,true,true,false};
    bool pointcloud[9] = {false,false,false,false,false,false,false,false,true};
    bool map[9] = {true,true,true,false,false,false,false,false,false};*/




    //allowed character for name of maps
    QRegExp re("[A-Za-z0-9]");
    QRegExp rews("\\s+");

    //for(int j = 0; j < sl.length();j++)
    {
        QString scriptline = script;//sl.at(j);

        QString outline = scriptline;
        for(int i = 0; i < exts.length();i++)
        {

            reparg1.clear();
            reparg2.clear();
            reparg3.clear();
            reparg4.clear();

            //find areas that are part of strings, those we do not reinterpret
            QRegularExpression reSQ("(\"([^\"]|\"\")*\")");

            QRegularExpressionMatchIterator matchi = reSQ.globalMatch(scriptline);

            int *is_q = new int[scriptline.length()];

            for(int i = 0; i < scriptline.length();i++)
            {
                is_q[i] = 0;
            }

            QList<int> qbegin;
            QList<int> qend;
            while(matchi.hasNext())
            {
                QRegularExpressionMatch match = matchi.next();
                qbegin.append(match.capturedStart());
                qend.append(match.capturedEnd());
                for(int i = match.capturedStart(); i < match.capturedEnd(); i++)
                {
                    is_q[i] = 1;
                }
            }


            QString sstring = exts.at(i);
            bool is_shape = shape[i];
            bool is_table = table[i];
            bool is_pointcloud = pointcloud[i];
            bool is_map = map[i];
            bool is_model = model[i];
            bool is_field = field[i];
            bool is_rigidworld = rigidworld[i];

            int loc = scriptline.indexOf(sstring,0);

            while(loc > -1)
            {
                //oke, we found a map extension
                //find beginning and ending of the map name
                int end = loc +exts.at(i).length();
                int begin = loc;
                for(int k = loc-1; k > -1; k--)
                {
                    const QChar ch =scriptline.at(k);

                    //we detect special characters, but exclude some like '_'
                    //since these are allowed in file names and have
                    //no meaning in the scripting language

                    if(!(ch == '_'))
                    {
                        QString t; t.append(ch);

                        if(re.exactMatch(t)==false)
                        {
                            begin = k+1;
                            break;
                        }
                    }
                }

                bool is_assign = false;
                QChar nch = 'a';
                bool table_has_index = false;
                //now find the next non-whitespace character
                for(int k = end; k < scriptline.length(); k++)
                {
                    const QChar ch =scriptline.at(k);
                    QString t; t.append(ch);


                    if(table_has_index)
                    {
                        if(!(rews.exactMatch(t)))
                        {
                            nch = ch;
                            if(nch == ']' && is_q[k] == 0)
                            {
                                table_has_index = false;
                                continue;
                            }
                        }
                    }else {
                        if(!(rews.exactMatch(t)))
                        {

                            nch = ch;
                            if(nch == '=' )
                            {

                                if(k +1<scriptline.length())
                                {
                                    QChar nnch = scriptline.at(k+1);

                                    if(nnch == '=')
                                    {
                                        is_assign = false;
                                    }else {
                                        is_assign = true;
                                    }
                                }else
                                {
                                    is_assign = true;
                                }
                            }else if(nch == '+' || nch == '-' || nch == '/' || nch == '*' || nch == '&' || nch == '|' || nch == '^'|| nch == '%' )
                            {

                                if(k +1<scriptline.length())
                                {
                                    QChar nnch = scriptline.at(k+1);

                                    if(nnch == '=')
                                    {
                                        is_assign = true;
                                    }else {
                                        is_assign = false;
                                    }
                                }else
                                {
                                    is_assign = true;
                                }
                            }else if(nch == '*')
                            {

                                if(k +2<scriptline.length())
                                {
                                    QChar nnch = scriptline.at(k+1);
                                    QChar nnnch = scriptline.at(k+2);
                                    if(nnch == '*' && nnnch == '=')
                                    {
                                        is_assign = false;
                                    }else {
                                        is_assign = true;
                                    }
                                }else
                                {
                                    is_assign = true;
                                }

                            }else if((is_table || is_shape || is_map) && nch == '[' && is_q[k] == 0)
                            {
                                table_has_index = true;
                                continue;
                            }else{
                                is_assign = false;
                            }

                            break;
                        }

                    }

                }


                //the full name of the map is
                QString fullname = scriptline.mid(begin,end-begin);

                int locp =0;

                int add_search = 0;

                if(!is_q[end-2])
                {
                    //depending on wether there is assignment,replace
                    if(!is_shape && !is_table && !is_field && !is_pointcloud && !is_model)
                    {
                        if(is_assign)
                        {
                             QString replace = "SaveThisMap(\"" + fullname + "\")";
                             //scriptline.replace((int)begin,(int)(end-begin+1),replace);
                             list_namessave.append(fullname);

                             reparg1.append((int)begin);
                             reparg2.append((int)(end-begin));
                             reparg3.append(replace);
                             reparg4.append(15);
                             add_search += 15 + fullname.length();
                        }else
                        {
                            QString replace = "LoadMap(\"" + fullname + "\")";
                            //scriptline.replace((int)begin,(int)(end-begin+1),replace);
                            list_namesload.append(fullname);

                            reparg1.append((int)begin);
                            reparg2.append((int)(end-begin));
                            reparg3.append(replace);
                            reparg4.append(11);
                            add_search += 11 + fullname.length();
                        }
                    }
                    if(is_field)
                    {
                        if(is_assign)
                        {
                             QString replace = "SaveThisField(\"" + fullname + "\")";
                             //scriptline.replace((int)begin,(int)(end-begin+1),replace);
                             list_namessave.append(fullname);

                             reparg1.append((int)begin);
                             reparg2.append((int)(end-begin));
                             reparg3.append(replace);
                             reparg4.append(17);
                             add_search += 17 + fullname.length();
                        }else
                        {
                            QString replace = "LoadField(\"" + fullname + "\")";
                            //scriptline.replace((int)begin,(int)(end-begin+1),replace);
                            list_namesload.append(fullname);

                            reparg1.append((int)begin);
                            reparg2.append((int)(end-begin));
                            reparg3.append(replace);
                            reparg4.append(13);
                            add_search += 13 + fullname.length();
                        }


                    }if(is_shape)
                    {
                        if(is_assign)
                        {
                             QString replace = "SaveThisVector(\"" + fullname + "\")";
                             //scriptline.replace((int)begin,(int)(end-begin+1),replace);
                             list_namessave.append(fullname);

                             reparg1.append((int)begin);
                             reparg2.append((int)(end-begin));
                             reparg3.append(replace);
                             reparg4.append(18);
                             add_search += 18 + fullname.length();
                        }else
                        {
                            QString replace = "LoadVector(\"" + fullname + "\")";
                            //scriptline.replace((int)begin,(int)(end-begin+1),replace);
                            list_namesload.append(fullname);

                            reparg1.append((int)begin);
                            reparg2.append((int)(end-begin));
                            reparg3.append(replace);
                            reparg4.append(14);
                            add_search += 14 + fullname.length();
                        }

                    }else if(is_table){
                        if(is_assign)
                        {
                             QString replace = "SaveThisTable(\"" + fullname + "\")";
                             //scriptline.replace((int)begin,(int)(end-begin+1),replace);
                             list_namessave.append(fullname);

                             reparg1.append((int)begin);
                             reparg2.append((int)(end-begin));
                             reparg3.append(replace);
                             reparg4.append(17);
                             add_search += 17 + fullname.length();
                        }else
                        {
                            QString replace = "LoadTable(\"" + fullname + "\")";
                            //scriptline.replace((int)begin,(int)(end-begin+1),replace);
                            list_namesload.append(fullname);

                            reparg1.append((int)begin);
                            reparg2.append((int)(end-begin));
                            reparg3.append(replace);
                            reparg4.append(13);
                            add_search += 13 + fullname.length();
                        }

                    }else if(is_pointcloud)
                    {
                        if(is_assign)
                        {
                             QString replace = "SaveThisPointCloud(\"" + fullname + "\")";
                             //scriptline.replace((int)begin,(int)(end-begin+1),replace);
                             list_namessave.append(fullname);

                             reparg1.append((int)begin);
                             reparg2.append((int)(end-begin));
                             reparg3.append(replace);
                             reparg4.append(22);
                             add_search += 22 + fullname.length();
                        }else
                        {
                            QString replace = "LoadPointCloud(\"" + fullname + "\")";
                            //scriptline.replace((int)begin,(int)(end-begin+1),replace);
                            list_namesload.append(fullname);

                            reparg1.append((int)begin);
                            reparg2.append((int)(end-begin));
                            reparg3.append(replace);
                            reparg4.append(18);
                            add_search += 18 + fullname.length();
                        }
                    }else if(is_rigidworld)
                    {
                        if(is_assign)
                        {
                             QString replace = "SaveThisRigidWorld(\"" + fullname + "\")";
                             //scriptline.replace((int)begin,(int)(end-begin+1),replace);
                             list_namessave.append(fullname);

                             reparg1.append((int)begin);
                             reparg2.append((int)(end-begin));
                             reparg3.append(replace);
                             reparg4.append(22);
                             add_search += 22 + fullname.length();
                        }else
                        {
                            QString replace = "LoadRigidWorld(\"" + fullname + "\")";
                            //scriptline.replace((int)begin,(int)(end-begin+1),replace);
                            list_namesload.append(fullname);

                            reparg1.append((int)begin);
                            reparg2.append((int)(end-begin));
                            reparg3.append(replace);
                            reparg4.append(18);
                            add_search += 18 + fullname.length();
                        }
                    }else if(is_model)
                    {
                        if(is_assign)
                        {
                             QString replace = "SaveThisObject(\"" + fullname + "\")";
                             //scriptline.replace((int)begin,(int)(end-begin+1),replace);
                             list_namessave.append(fullname);

                             reparg1.append((int)begin);
                             reparg2.append((int)(end-begin));
                             reparg3.append(replace);
                             reparg4.append(18);
                             add_search += 18 + fullname.length();
                        }else
                        {
                            QString replace = "LoadObject(\"" + fullname + "\")";
                            //scriptline.replace((int)begin,(int)(end-begin+1),replace);
                            list_namesload.append(fullname);

                            reparg1.append((int)begin);
                            reparg2.append((int)(end-begin));
                            reparg3.append(replace);
                            reparg4.append(14);
                            add_search += 14 + fullname.length();
                        }


                    }
                }

                //find new location
                loc = scriptline.indexOf(sstring,loc + sstring.length() );
            }

            delete[] is_q;


            int add = 0;
            for(int i = reparg1.length()-1; i >-1 ;i--)
            {

                scriptline.replace(reparg1.at(i)+add,reparg2.at(i)+add,reparg3.at(i));
                //add = add + reparg4.at(i);
            }

        }

        output.append(scriptline + "\n");


    }

    std::cout << "preprocessed:  " << output.toStdString() << std::endl;
    return output;
}




#endif // SCRIPTPREPROCESSOR_H

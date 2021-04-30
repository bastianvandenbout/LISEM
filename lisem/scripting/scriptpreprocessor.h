#ifndef SCRIPTPREPROCESSOR_H
#define SCRIPTPREPROCESSOR_H

#include "QString"
#include "QList"
#include "QRegExp"
#include "QRegularExpression"

#include "extensionprovider.h"

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

    QStringList exts;
    std::vector<bool> shape;
    std::vector<bool> table;
    std::vector<bool> pointcloud;
    std::vector<bool> map;
    std::vector<bool> model;

    for(int i = 0; i < mapexts.length(); i++)
    {
        exts.append(mapexts.at(i));
        shape.push_back(false);
        table.push_back(false);
        pointcloud.push_back(false);
        map.push_back(true);
        model.push_back(false);
    }

    for(int i = 0; i < shapeexts.length(); i++)
    {
        exts.append(shapeexts.at(i));
        shape.push_back(true);
        table.push_back(false);
        pointcloud.push_back(false);
        map.push_back(false);
        model.push_back(false);
    }

    for(int i = 0; i < pointexts.length(); i++)
    {
        exts.append(pointexts.at(i));
        shape.push_back(false);
        table.push_back(false);
        pointcloud.push_back(true);
        map.push_back(false);
        model.push_back(false);
    }
    for(int i = 0; i < tableexts.length(); i++)
    {
        exts.append(tableexts.at(i));
        shape.push_back(false);
        table.push_back(true);
        pointcloud.push_back(false);
        map.push_back(false);
        model.push_back(false);
    }


    for(int i = 0; i < modelexts.length(); i++)
    {
        exts.append(modelexts.at(i));
        shape.push_back(false);
        table.push_back(false);
        pointcloud.push_back(false);
        map.push_back(false);
        model.push_back(true);
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
                    if(!is_shape && !is_table && !is_pointcloud && !is_model)
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

    return output;
}




#endif // SCRIPTPREPROCESSOR_H

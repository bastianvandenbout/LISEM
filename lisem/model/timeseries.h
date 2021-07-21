#ifndef TIMESERIES_H
#define TIMESERIES_H

#include <QString>
#include <QList>
#include "geo/raster/map.h"
#include "QFile"
#include "QFileInfo"
#include "QDir"
#include "error.h"
#include "iogdal.h"
#include <iostream>

struct TimeData
{
    float t;
    QString value;
    QList<float> valuesf;
    bool is_map = false;

};

class TimeSeries
{
    bool is_single_value;
    QString folder;
    QString file;
    QList<TimeData> m_Data;
    float m_Mult = 1.0;
public:
    TimeSeries();

    inline void Clear()
    {
        m_Data.clear();
        m_Mult = 1.0;
    }

    inline void LoadFromFile(QString file, bool allowmaps = true, bool check = true, double mult = 1.0)
    {
        m_Mult = mult;
        QFile fff(file);
        QFileInfo fi(file);
        QString S;
        QStringList Lines;

        LISEM_DEBUG("Loading timeseries from "+file);

        if (!fi.exists())
        {

            throw 1;
        }

        folder = fi.dir().absolutePath() + "/";


        fff.open(QIODevice::ReadOnly | QIODevice::Text);

        while (!fff.atEnd())
        {
            S = fff.readLine();
            if (S.contains("\n"))
                S.remove(S.count()-1,1);
            // readLine also reads \n as a character on an empty line!
            if (!S.trimmed().isEmpty())
                Lines << S.trimmed();
            //qDebug() << S;
        }
        fff.close();

        //first line is description

        //second line is number of columns
        bool ok = true;
        int count = Lines[1].toInt(&ok, 10);

        bool any_map_found =false;

        //skip two start lines, one description line per column
        int skiplines = count + 2;

        for(int i = skiplines; i < Lines.length(); i++)
        {
            //std::cout << Lines[i].toStdString().c_str() << std::endl;
            QStringList SL = Lines[i].split(QRegExp("\\s+"), QString::SkipEmptyParts);

            if (!(SL.length() >= count))
            {

                throw 1;
            }


            // split rainfall record row with whitespace
            TimeData data;
            bool okt = false;
            data.t = 60.0 * SL[0].toDouble(&ok);
            if (!ok)
            {
                throw 1;
            }


            if (SL[1].contains(QRegExp("[A-Za-z]")))
            {
                any_map_found = true;
                data.is_map = true;
                data.value = SL[1];

            }else
            {


                for (int i = 1; i < count; i++)
                {

                    bool ok = false;
                    double intensity = SL[i].toDouble(&ok);
                    if (!ok)
                    {
                        throw 1;
                    }

                    data.valuesf.append(intensity);
                }
            }

            m_Data.append(data);

        }


        if(count == 2 && any_map_found == false)
        {
            this->is_single_value = true;
        }

    }

    inline float fillValues(float t, float dt)
    {
        //get all relevent moments in time that influence the value to obtain
        QList<float> timings;
        QList<QString> values;
        QList<QList<float>> valuesf;
        float timetotal = 0;
        float fill = 0;

        for(int i = 0; i < m_Data.length();i++)
        {
            //std::cout << m_Data.at(i).t << " " << m_Data.at(i).valuesf.at(0) << std::endl;


            float t_next = 0;
            if(i < m_Data.length()-1)
            {
                t_next = m_Data.at(i+1).t;
            }else
            {
                t_next = 1e31;
            }

            TimeData d = m_Data.at(i);

            float t_this = ((i == 0)? -1e31 : d.t);

            float overlap = std::max(0.f,std::min(t_next, t+dt) - std::max(t_this, t));
            if(overlap > 0)
            {
                timetotal += overlap;
                float val = m_Mult*d.valuesf.at(0);
                fill += val*overlap;
            }else if(t_this > t+dt)
            {
                break;
            }

        }


        if(timetotal > 0)
        {

                fill = fill/timetotal;
        }
        return fill;
    }


    inline void fillValues(float t, float dt, cTMap * fill)
    {


        //get all relevent moments in time that influence the value to obtain
        QList<float> timings;
        QList<QString> values;
        QList<QList<float>> valuesf;
        float timetotal = 0;

        for(int i = 0; i < m_Data.length();i++)
        {


            float t_next = 0;
            if(i < m_Data.length()-1)
            {
                t_next = m_Data.at(i+1).t;
            }else
            {
                t_next = 1e31;
            }

            TimeData d = m_Data.at(i);

            float t_this = i == 0? -1e31 : d.t;

            float overlap = std::max(0.f,std::min(t_next, t+dt) - std::max(t, t));
            if(overlap > 0)
            {

                timetotal += overlap;
                if(d.is_map)
                {
                    std::cout << "load map for rain "<< std::endl;
                    //read raster
                    cTMap * mapr = new cTMap(readRaster(folder + d.value,0));

                    FOR_ROW_COL_MV(fill)
                    {

                        if(pcr::isMV(mapr->data[r][c]))
                        {
                           fill->data[r][c] = 0.0;
                        }else
                        {
                            fill->data[r][c] = m_Mult*mapr->data[r][c];
                        }

                    }

                    delete mapr;

                }else {

                    float val = d.valuesf.at(0);
                    FOR_ROW_COL_MV(fill)
                    {
                        fill->data[r][c] += m_Mult*val;
                    }

                }
            }
        }


        if(timetotal > 0)
        {
            FOR_ROW_COL_MV(fill)
            {

                fill->data[r][c] = fill->data[r][c]/timetotal;
            }
        }



    }





};

#endif // TIMESERIES_H

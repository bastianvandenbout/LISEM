#ifndef RASTERDATAPROVIDER_H
#define RASTERDATAPROVIDER_H


#include "geometry/triangleintersect.h"
#include "QList"
#include "geo/raster/map.h"
#include "iogdal.h"
#include <iostream>
#include <thread>
#include "openglcltexture.h"
#include "math.h"
#include "QJsonObject"
#include "serialization.h"

////
/// \brief The RasterDataProvider class
///
/// This class acts as an interface for a variety of raster data providers.
/// Implementations are included in this file
/// Memory-raster data provider
/// Disk-raster data provider
/// Timeseries-Raster data provider
/// Web-raster data provider
///
/// @see UILayer
///

#define MAX_BAND_HISTORY 4

#define LISEM_RASTER_SAMPLE_NEAREST 0
#define LISEM_RASTER_SAMPLE_LINEAR 1

class RasterDataProvider
{
private:

    QList<QString> m_FilesOrg;
    QList<QString> m_Files;
    QList<QString> m_Files2;
    cTMapProps m_Props;
    RasterBandStats m_Stats;
    bool m_HasTime = false;
    int m_MinTime = 0;
    int m_MaxTime = 0;
    QList<int> m_Times;
    bool m_IsNative = false;
    bool m_IsOwned = false;
    bool m_IsFile = false;
    bool m_HasFile = false;
    bool m_HasData = false;
    bool m_IsDuo = false;
    bool m_IsMemoryMap = false;
    bool m_IsSaveAble = false;


    QList<QList<cTMap *>> m_MMaps;
    QList<QList<RasterBandStats>> m_MBandStats;
    QList<QList<cTMapProps>> m_MBandProps;

    QList<RasterBandStats> m_BandStats;
    QList<cTMapProps> m_BandProps;

    QList<RasterBandStats> m_TBandStats;

    QList<QList<OpenGLCLTexture*>> m_Textures;

private:

    inline bool SetAsDuo()
    {
        if(m_Props.bands == 1 && m_Files.length() > 1 && (m_Files.length() % 2 == 0))
        {
            //split files into two lists, so we have n/2 duos
            int lhalf =m_Files.length()/2;

            for(int i = lhalf; i < lhalf*2; i++)
            {
                m_Files2.append(m_Files.at(i));
            }

            for(int i = m_Files.length() - 1; i > lhalf - 1; i--)
            {
                m_Files.removeAt(i);
            }

            m_IsDuo = true;
            return true;
        }

        return false;
    }

    inline void RasterDataProviderInitFromMemList(QList<QList<cTMap *>> Maps, bool duo = false, bool own = false)
    {
        m_IsFile = false;
        m_IsOwned = own;
        m_IsNative = false;
        m_IsMemoryMap = true;


        //initiate the raster data source


        //check if we have any bands, and if it is a timeseries if they are consistent
        m_HasData = true;
        int n_bands = 0;

        for(int i = 0; i < Maps.length(); i++)
        {
            if(i == 0)
            {
                n_bands = Maps.at(i).length();
                if(n_bands == 0)
                {
                    LISEM_ERROR("error in adding maps: Zero bands in map add request!")
                    m_HasData = false;
                    break;
                }
                if(duo)
                {
                    if(n_bands != 2)
                    {
                        LISEM_ERROR("error in adding maps: Duo map does not have 2 bands!")
                        m_HasData = false;
                        break;
                    }
                }

                for(int j = 0; j < Maps.at(i).length(); j++)
                {
                    if(i == 0 && j == 0)
                    {
                        m_Props = Maps.at(i).at(j)->GetRasterProps();
                        m_Props.band = 0;
                        m_Props.bands = n_bands;

                    }else if(!(m_Props.equals(Maps.at(i).at(j)->GetRasterProps())))
                    {
                        LISEM_ERROR("error in adding maps: Timeseries are not all of equal raster size!")
                        m_HasData = false;
                        break;
                    }
                }

            }else {
                if(!(n_bands == Maps.at(i).length()))
                {
                    LISEM_ERROR("error in adding maps: Timeseries are not all of equal band count!")
                    m_HasData = false;
                    break;
                }
            }
        }

        if(Maps.length() > 1)
        {
            m_HasTime = true;
            for(int i = 0; i < Maps.length(); i++)
            {
                m_Times.append(i);
            }
            m_MinTime = 0;
            m_MaxTime = Maps.length()-1;
        }else
        {
            m_HasTime = false;
        }

        if(m_HasData == false)
        {
            return;
        }

        if(duo)
        {
            m_IsDuo = true;
        }
        if(Maps.length() > 1)
        {
            m_HasTime = true;
        }

        m_MMaps = Maps;

        if(m_IsDuo)
        {
            double b1_tmin = 1e31;
            double b1_tmax = -1e31;
            double b1_mean = 0.0;
            double b1_stdev = 0.0;
            double n = Maps.length();


            double b2_tmin = 1e31;
            double b2_tmax = -1e31;
            double b2_mean = 0.0;
            double b2_stdev = 0.0;
            for(int i = 0; i < Maps.length(); i++)
            {
                RasterBandStats stats = Maps.at(i).at(0)->GetRasterBandStats();
                RasterBandStats stats2 = Maps.at(i).at(1)->GetRasterBandStats();
                QList<RasterBandStats> statsl;
                stats.band = 1;
                stats.band = 2;
                statsl.append(stats);
                statsl.append(stats2);
                m_MBandStats.append(statsl);

                QList<cTMapProps> propsl;
                cTMapProps props1 = Maps.at(i).at(0)->GetRasterProps();
                props1.band = 0;
                props1.bands = 2;
                cTMapProps props2 = Maps.at(i).at(0)->GetRasterProps();
                props2.band = 1;
                props2.bands = 2;
                propsl.append(props1);
                propsl.append(props2);
                m_MBandProps.append(propsl);

                b1_tmax = std::max(b1_tmax,stats.max);
                b1_tmin = std::min(b1_tmin,stats.min);
                b2_tmax = std::max(b2_tmax,stats2.max);
                b2_tmin = std::min(b2_tmin,stats2.min);

                b1_mean += stats.mean;
                b1_stdev += stats.stdev;
                b2_mean += stats2.mean;
                b2_stdev += stats2.stdev;
            }

            b1_mean = b1_mean/n;
            b1_stdev = b1_stdev/n;

            b2_mean = b2_mean/n;
            b2_stdev = b2_stdev/n;

            RasterBandStats Tstats1;
            RasterBandStats Tstats2;

            Tstats1.max = b1_tmax;
            Tstats1.min = b1_tmin;
            Tstats1.mean = b1_mean;
            Tstats1.stdev = b1_stdev;

            Tstats2.max = b2_tmax;
            Tstats2.min = b2_tmin;
            Tstats2.mean = b2_mean;
            Tstats2.stdev = b2_stdev;

            m_TBandStats.append(Tstats1);
            m_TBandStats.append(Tstats2);

        }else
        {

            for(int i = 0; i < Maps.length(); i++)
            {
                QList<cTMapProps> propsl;
                QList<RasterBandStats> statsl;
                for(int j = 0; j < m_Props.bands; j++)
                {
                    RasterBandStats stats = Maps.at(i).at(j)->GetRasterBandStats();
                    stats.band = j;
                    statsl.append(stats);

                    cTMapProps props = Maps.at(i).at(j)->GetRasterProps();
                    props.band = j;
                    props.bands = m_Props.bands;
                    propsl.append(props);
                }
                m_MBandStats.append(statsl);
                m_MBandProps.append(propsl);
            }

            for(int j = 0; j < m_Props.bands; j++)
            {
                double b1_tmin = 1e31;
                double b1_tmax = -1e31;
                double b1_mean = 0.0;
                double b1_stdev = 0.0;
                double n = Maps.length();

                for(int i = 0; i < Maps.length(); i++)
                {
                    RasterBandStats stats = m_MBandStats.at(i).at(j);

                    b1_tmax = std::max(b1_tmax,stats.max);
                    b1_tmin = std::min(b1_tmin,stats.min);
                    b1_mean += stats.mean;
                    b1_stdev += stats.stdev;
                }

                b1_mean = b1_mean/n;
                b1_stdev = b1_stdev/n;
                RasterBandStats Tstats1;

                Tstats1.max = b1_tmax;
                Tstats1.min = b1_tmin;
                Tstats1.mean = b1_mean;
                Tstats1.stdev = b1_stdev;

                m_TBandStats.append(Tstats1);
            }
        }

        m_Stats = m_MBandStats.at(0).at(0);

    }

    inline void RasterDataProviderInitFromFileList(QList<QString> file_paths, bool duo = false)
    {

        m_IsFile = true;
        m_IsOwned = true;
        m_IsNative = false;

        //initiate the raster data source

        if(file_paths.length() > 0)
        {
            cTMapProps p = readRasterProps(file_paths.at(0));
            if(p.can_be_read)
            {
                m_HasData = true;

                m_Props = p;
                m_Stats = readRasterStats(file_paths.at(0));
                m_BandProps.append(p);
                m_BandStats.append(m_Stats);

                for(int i = 0; i < file_paths.length(); i++)
                {
                    cTMapProps p_i = readRasterProps(file_paths.at(i));

                    //if this map can not be read, or the dimensions are not identical, then remove from list
                    if(!(p_i.can_be_read) || !(p_i.equals(p)))
                    {
                        file_paths.removeAt(i);
                        i--;
                    }

                }

                m_IsSaveAble = true;
                m_FilesOrg = file_paths;
                m_Files = file_paths;

                if(duo)
                {
                    SetAsDuo();
                }

                {//construct the time values
                    if(file_paths.length() > 1)
                    {
                        m_HasTime = true;
                        int t_max = -2147483647;
                        int t_min = 2147483647;

                        for(int i = 0; i < file_paths.length(); i++)
                        {
                                QFileInfo fi(file_paths.at(i));
                                QString suffix = fi.suffix();
                                QString name = fi.completeBaseName();

                                //detect time as integer at end of file name
                                bool ok = false;
                                int t = suffix.toInt(&ok);
                                if(ok)
                                {
                                    for(int j= name.length()-1; j > -1; j--)
                                    {
                                        if(name.at(j).isDigit())
                                        {
                                            suffix.prepend(name.at(j));
                                        }
                                    }
                                    bool ok2 = false;
                                    t = suffix.toInt(&ok2);
                                    if(!ok2)
                                    {
                                        t = i;
                                    }

                                //else simply set is as the index in the list
                                }else {
                                    t = i;
                                }
                                t_max = std::max(t,t_max);
                                t_min = std::min(t,t_min);

                                m_Times.append(t);
                        }
                        m_MaxTime = t_max;
                        m_MinTime = t_min;


                        //construct time-series stats
                        if(m_IsDuo)
                        {
                            double b1_tmin = 1e31;
                            double b1_tmax = -1e31;
                            double b1_mean = 0.0;
                            double b1_stdev = 0.0;
                            double n = m_Files.length();


                            double b2_tmin = 1e31;
                            double b2_tmax = -1e31;
                            double b2_mean = 0.0;
                            double b2_stdev = 0.0;
                            for(int i = 0; i < m_Files.length(); i++)
                            {
                                RasterBandStats stats = readRasterStats(m_Files.at(i));
                                RasterBandStats stats2 = readRasterStats(m_Files2.at(i));
                                b1_tmax = std::max(b1_tmax,stats.max);
                                b1_tmin = std::min(b1_tmin,stats.min);
                                b2_tmax = std::max(b2_tmax,stats2.max);
                                b2_tmin = std::min(b2_tmin,stats2.min);

                                b1_mean += stats.mean;
                                b1_stdev += stats.stdev;
                                b2_mean += stats2.mean;
                                b2_stdev += stats2.stdev;
                            }

                            b1_mean = b1_mean/n;
                            b1_stdev = b1_stdev/n;

                            b2_mean = b2_mean/n;
                            b2_stdev = b2_stdev/n;

                            RasterBandStats Tstats1;
                            RasterBandStats Tstats2;

                            Tstats1.max = b1_tmax;
                            Tstats1.min = b1_tmin;
                            Tstats1.mean = b1_mean;
                            Tstats1.stdev = b1_stdev;

                            Tstats2.max = b2_tmax;
                            Tstats2.min = b2_tmin;
                            Tstats2.mean = b2_mean;
                            Tstats2.stdev = b2_stdev;

                            m_TBandStats.append(Tstats1);
                            m_TBandStats.append(Tstats2);

                        }else
                        {

                            for(int j = 0; j < m_Props.bands; j++)
                            {
                                double b1_tmin = 1e31;
                                double b1_tmax = -1e31;
                                double b1_mean = 0.0;
                                double b1_stdev = 0.0;
                                double n = file_paths.length();

                                for(int i = 0; i < file_paths.length(); i++)
                                {
                                    RasterBandStats stats = readRasterStats(m_Files.at(i),j);
                                    b1_tmax = std::max(b1_tmax,stats.max);
                                    b1_tmin = std::min(b1_tmin,stats.min);

                                    b1_mean += stats.mean;
                                    b1_stdev += stats.stdev;
                                }

                                b1_mean = b1_mean/n;
                                b1_stdev = b1_stdev/n;
                                RasterBandStats Tstats1;

                                Tstats1.max = b1_tmax;
                                Tstats1.min = b1_tmin;
                                Tstats1.mean = b1_mean;
                                Tstats1.stdev = b1_stdev;

                                m_TBandStats.append(Tstats1);
                            }
                        }
                    }
                }
            }
        }

        //m_Stats = m_MBandStats.at(0).at(0);
    }

public:
    inline RasterDataProvider()
    {


    }

    inline RasterDataProvider(QList<QString> file_paths, bool duo = false)
    {
       RasterDataProviderInitFromFileList(file_paths,duo);

    }

    inline RasterDataProvider(QList<QList<cTMap *>> Maps, bool duo = false, bool own = false)
    {
        RasterDataProviderInitFromMemList(Maps,duo,own);

    }

    inline RasterDataProvider(QList<QList<cTMap *>> Maps,QList<QList<OpenGLCLTexture*>> Textures, bool duo, bool own = false)
    {

        RasterDataProviderInitFromMemList(Maps,duo,own);

        //initiate the raster opengl sources
        m_IsNative = true;

        m_Textures = Textures;

    }


    inline bool SetInMemory()
    {
        if(!m_IsMemoryMap)
        {
            if(m_Files.length() > 0)
            {
                if(m_IsDuo)
                {
                    for(int i = 0; i < m_Files.length(); i++)
                    {
                        QList<cTMap *> Maps = readRasterList(m_Files.at(i));
                        Maps.append(readRasterList(m_Files2.at(i)));
                        if(Maps.length() > 0)
                        {
                            if(Maps.at(0) != nullptr)
                            {
                                m_MMaps.append(Maps);
                                QList<cTMapProps> props = readRasterListProps(m_Files.at(i));
                                props.append(readRasterListProps(m_Files2.at(i)));
                                m_MBandProps.append(props);
                                QList<RasterBandStats> stats = readRasterListStats(m_Files.at(i));
                                stats.append(readRasterListStats(m_Files2.at(i)));
                                m_MBandStats.append(stats);

                                m_IsMemoryMap = true;

                            }else {
                                break;
                            }
                        }else {
                            break;
                        }
                    }

                }else {

                    std::cout << "set file in memory" << std::endl;
                    for(int i = 0; i < m_Files.length(); i++)
                    {
                        QList<cTMap *> Maps = readRasterList(m_Files.at(i));
                        m_IsMemoryMap = false;

                        std::cout << "file: "<< i << std::endl;
                        std::cout << "bands: " << Maps.length() << std::endl;
                        if(Maps.length() > 0)
                        {
                            if(Maps.at(0) != nullptr)
                            {
                                m_MMaps.append(Maps);

                                std::cout << "band props length: " << readRasterListProps(m_Files.at(i)).length() << std::endl;
                                std::cout << "band stats length: " << readRasterListStats(m_Files.at(i)).length() << std::endl;
                                m_MBandProps.append(readRasterListProps(m_Files.at(i)));
                                m_MBandStats.append(readRasterListStats(m_Files.at(i)));

                                m_IsMemoryMap = true;

                            }else {
                                break;
                            }
                        }else {
                            break;
                        }
                    }

                }

            }

            if(!m_IsMemoryMap)
            {
                for(int i = 0; i < m_MMaps.length(); i++)
                {

                    for(int j = 0; j < m_MMaps.at(i).length(); j++)
                    {
                        if(m_MMaps.at(i).at(j) != nullptr)
                        {
                            delete m_MMaps.at(i).at(j);
                        }
                    }
                }
                m_MMaps.clear();

                m_MBandProps.clear();
                m_MBandStats.clear();

            }
        }
        return m_IsMemoryMap;
    }

    inline bool Exists()
    {
        return m_HasData;
    }

    inline bool IsFile()
    {
        return m_HasFile;
    }

    inline bool IsNative()
    {
        return m_IsNative;
    }
    inline bool IsSaveAble()
    {

        return m_IsSaveAble;
    }

    inline OpenGLCLTexture * GetTexture(int band, int timeindex = 0)
    {
        return m_Textures.at(timeindex).at(band);
    }


    inline bool IsOwned()
    {

        return m_IsOwned;
    }

    inline bool IsMemoryMap()
    {
        return m_IsMemoryMap;
    }
    inline bool IsDuoMap()
    {
        return m_IsDuo;
    }

    inline bool IsEditable()
    {
        std::cout << m_IsDuo << " " << m_HasTime << " " << m_IsMemoryMap << std::endl;
        if(!m_IsDuo && !m_HasTime && m_IsMemoryMap)
        {
            return true;
        }

        return false;
    }

    inline void Destroy()
    {
        if(m_IsMemoryMap && m_IsOwned)
        {
            for(int i = 0; i < m_MMaps.length(); i++)
            {

                for(int j = 0; j < m_MMaps.at(i).length(); j++)
                {
                    if(m_MMaps.at(i).at(j) != nullptr)
                    {
                        delete m_MMaps.at(i).at(j);
                    }
                }
            }
            m_MMaps.clear();
        }
    }

    inline int GetBandCount()
    {
        if(m_IsDuo)
        {
            return 2;
        }
        return m_Props.bands;
    }

    inline bool HasTime()
    {
        return m_HasTime;
    }
    inline QList<int> GetTimes()
    {
        return m_Times;
    }

    inline int GetMaxTime()
    {
        return m_MaxTime;
    }

    inline int GetMinTime()
    {
        return m_MinTime;
    }

    inline int GetClosestTimeIndex(float time)
    {
        if(!m_HasTime)
        {
            return 0;
        }

        int index = 0;
        float min_distance = 1e31;
        for(int i = 0; i < m_Times.length(); i++)
        {
            if(std::fabs(time - m_Times.at(i)) < min_distance)
            {
                min_distance = std::fabs(time - m_Times.at(i));
                index = i;
            }
        }

        return index;

    }

    inline RasterBandStats GetTimeTotalBandStats(int band = 0)
    {
        return m_TBandStats.at(band);
    }

    inline void UpdateBandStats()
    {
        if(m_IsNative)
        {
            m_MBandStats.clear();
            m_TBandStats.clear();

            if(m_IsDuo)
            {
                double b1_tmin = 1e31;
                double b1_tmax = -1e31;
                double b1_mean = 0.0;
                double b1_stdev = 0.0;
                double n = m_MMaps.length();


                double b2_tmin = 1e31;
                double b2_tmax = -1e31;
                double b2_mean = 0.0;
                double b2_stdev = 0.0;
                for(int i = 0; i < m_MMaps.length(); i++)
                {
                    RasterBandStats stats = m_MMaps.at(i).at(0)->GetRasterBandStats();
                    RasterBandStats stats2 = m_MMaps.at(i).at(1)->GetRasterBandStats();
                    QList<RasterBandStats> statsl;
                    stats.band = 1;
                    stats.band = 2;
                    statsl.append(stats);
                    statsl.append(stats2);
                    m_MBandStats.append(statsl);
                    b1_tmax = std::max(b1_tmax,stats.max);
                    b1_tmin = std::min(b1_tmin,stats.min);
                    b2_tmax = std::max(b2_tmax,stats2.max);
                    b2_tmin = std::min(b2_tmin,stats2.min);

                    b1_mean += stats.mean;
                    b1_stdev += stats.stdev;
                    b2_mean += stats2.mean;
                    b2_stdev += stats2.stdev;
                }

                b1_mean = b1_mean/n;
                b1_stdev = b1_stdev/n;

                b2_mean = b2_mean/n;
                b2_stdev = b2_stdev/n;

                RasterBandStats Tstats1;
                RasterBandStats Tstats2;

                Tstats1.max = b1_tmax;
                Tstats1.min = b1_tmin;
                Tstats1.mean = b1_mean;
                Tstats1.stdev = b1_stdev;

                Tstats2.max = b2_tmax;
                Tstats2.min = b2_tmin;
                Tstats2.mean = b2_mean;
                Tstats2.stdev = b2_stdev;

                m_TBandStats.append(Tstats1);
                m_TBandStats.append(Tstats2);

            }else
            {

                for(int i = 0; i < m_MMaps.length(); i++)
                {
                    QList<RasterBandStats> statsl;
                    for(int j = 0; j < m_Props.bands; j++)
                    {
                        RasterBandStats stats = m_MMaps.at(i).at(j)->GetRasterBandStats();
                        stats.band = j;
                        statsl.append(stats);
                    }
                    m_MBandStats.append(statsl);
                }

                for(int j = 0; j < m_Props.bands; j++)
                {
                    double b1_tmin = 1e31;
                    double b1_tmax = -1e31;
                    double b1_mean = 0.0;
                    double b1_stdev = 0.0;
                    double n = m_MMaps.length();

                    for(int i = 0; i < m_MMaps.length(); i++)
                    {
                        RasterBandStats stats = m_MBandStats.at(i).at(j);

                        b1_tmax = std::max(b1_tmax,stats.max);
                        b1_tmin = std::min(b1_tmin,stats.min);
                        b1_mean += stats.mean;
                        b1_stdev += stats.stdev;
                    }

                    b1_mean = b1_mean/n;
                    b1_stdev = b1_stdev/n;
                    RasterBandStats Tstats1;

                    Tstats1.max = b1_tmax;
                    Tstats1.min = b1_tmin;
                    Tstats1.mean = b1_mean;
                    Tstats1.stdev = b1_stdev;

                    m_TBandStats.append(Tstats1);
                }
            }
        }

        m_Stats = m_MBandStats.at(0).at(0);
    }

    inline RasterBandStats GetBandStats(int band = 0, bool all_time = true)
    {
        if(m_HasTime && all_time)
        {
            return m_TBandStats.at(band);
        }
        if(band == 0)
        {
            return m_Stats;
        }else if(m_IsMemoryMap)
        {
            return m_MBandStats.at(0).at(band);

        }else {

            for(int i = 0; i < m_BandStats.length(); i++)
            {
                if(m_BandStats.at(i).band == band)
                {
                    return m_BandStats.at(i);
                }
            }
            RasterBandStats stats;

            if(m_IsDuo && band == 1)
            {
                stats = readRasterStats(m_Files2.at(0),0);
                stats.band = 1;
            }else {
                stats = readRasterStats(m_Files.at(0),band);
            }
            m_BandStats.prepend(stats);
            if(m_BandStats.length() > MAX_BAND_HISTORY)
            {
                m_BandStats.removeLast();
            }
            return stats;
        }
    }

    inline cTMapProps GetBandProps(int band = 0)
    {
        if(band == 0)
        {
            return m_Props;
        }else if(m_IsMemoryMap)
        {
            return m_MBandProps.at(0).at(band);

        }else{

            for(int i = 0; i < m_BandStats.length(); i++)
            {
                if(m_BandProps.at(i).band == band)
                {
                    return m_BandProps.at(i);
                }
            }

            cTMapProps props;

            if(m_IsDuo && band == 1)
            {
                props = readRasterProps(m_Files2.at(0),0);
                props.band = 1;
            }else {
                props = readRasterProps(m_Files.at(0),band);
            }

            m_BandProps.prepend(props);
            if(m_BandProps.length() > MAX_BAND_HISTORY)
            {
                m_BandProps.removeLast();
            }
            return props;
        }
    }

    inline QList<cTMap *> CopyMapListAtTime(int timeindex = 0)
    {
        QList<cTMap *> ret;
        if(m_IsMemoryMap)
        {
            for(int i = 0; i < m_MMaps.at(timeindex).length(); i++)
            {
                ret.append(m_MMaps.at(timeindex).at(i)->GetCopy());
            }
            return ret;
        }else if(m_IsDuo)
        {
            cTMap * m1;
            cTMap * m2;

            if(rasterCanBeOpenedForReading(m_Files2.at(timeindex)) && rasterCanBeOpenedForReading(m_Files.at(timeindex)))
            {
                m1 = new cTMap(readRaster(m_Files.at(timeindex)));
                m2 = new cTMap(readRaster(m_Files2.at(timeindex)));
            }

            return {m1,m2};
        }else{

            if(rasterCanBeOpenedForReading(m_Files.at(timeindex)))
            {

                return readRasterList(m_Files.at(timeindex));
            }else {
                return {};
            }
        }
        return {};
    }

    inline cTMap * GetMemoryMap(int band = 0, int timeindex = 0)
    {
        return m_MMaps.at(timeindex).at(band);
    }

    inline double GetCellSizeX(int band = 0)
    {
        return GetBandProps(band).cellsizex;
    }

    inline double GetCellSizeY(int band = 0)
    {
        return GetBandProps(band).cellsizey;

    }
    inline int GetTotalSizeX(int band = 0)
    {
        return GetBandProps(band).sizex;
    }
    inline int GetTotalSizeY(int band = 0)
    {
        return GetBandProps(band).sizey;
    }

    inline int GetLineWithinRasterExtent(LSMVector2 pos1, LSMVector2 pos2, LSMVector2 &pos1fix, LSMVector2 &pos2fix, int band = 0)
    {
        double tsx = GetTotalSizeX(band);
        double tsy = GetTotalSizeY(band);
        double csx = GetCellSizeX(band);
        double csy = GetCellSizeY(band);

        float rx = m_Props.ulx;
        float ry = m_Props.uly;
        float rw = (tsx * csx);
        float rh = (tsy * csy);

        LSMVector2 p1;
        LSMVector2 p2;
        lineRectUnion(pos1.x,pos1.y,pos2.x,pos2.y, rx, ry, rw, rh, p1, p2);

        pos1fix = p1;
        pos2fix = p2;

        std::cout << "csx: " << csx << " csy " << csy  << " p1: " << p1.x << ", " << p1.y << " p2: " << p2.x << ", "<< p2.y << std::endl;
        return std::max(1,(int)((std::fabs(((p1.x - p2.x)/csx)) +std::fabs((p1.y - p2.y)/csy))));

    }

    inline double GetValueAtLocation(LSMVector2 pos, int mode = LISEM_RASTER_SAMPLE_NEAREST, int band = 0, int timeindex = 0)
    {
        double tsx = GetTotalSizeX(band);
        double tsy = GetTotalSizeY(band);
        double csx = GetCellSizeX(band);
        double csy = GetCellSizeY(band);

        float rel_fracx = (pos.x - m_Props.ulx)/(tsx * csx);
        float rel_frac2y = (pos.y - m_Props.uly)/(tsy * csy);

        int px0 =  std::max(0,std::min(m_Props.sizex - 1,(int)(rel_fracx * float(m_Props.sizex))));
        int py0 =  std::max(0,std::min(m_Props.sizey - 1,(int)(rel_frac2y * float(m_Props.sizey))));

        float pixelx_relpos = std::max(0.0,std::min(1.0,std::fabs(((pos.x - m_Props.ulx) - (float)(px0) * m_Props.cellsizex)/m_Props.cellsizex)));
        float pixely_relpos = std::max(0.0,std::min(1.0,std::fabs(((pos.y - m_Props.uly) - (float)(py0) * m_Props.cellsizey)/m_Props.cellsizey)));

        double val = 0.0;

        //if outside of range, just return 0
        if(!(px0 > -1 && py0 > -1 && px0 < m_Props.sizex && py0 < m_Props.sizey))
        {
            return 0.0;
        }

        //if we are in the last row or column, we can not get a square of 4 cells for linear interpolation
        if((px0 == m_Props.sizex-1) || (py0 == m_Props.sizey-1))
        {
            mode =LISEM_RASTER_SAMPLE_NEAREST;
        }

        if(mode == LISEM_RASTER_SAMPLE_NEAREST)
        {
            if(m_IsMemoryMap)
            {
                val = m_MMaps.at(timeindex).at(band)->data[py0][px0];
            }else {

                QString path;
                if(m_IsDuo && band > 0)
                {
                    path = m_Files2.at(timeindex);
                    band = 0;
                }else {
                    path = m_Files.at(timeindex);
                }

                MaskedRaster<float> raster_data(1, 1,0.0,0.0,1.0);
                cTMap *temp = new cTMap(std::move(raster_data),"","");


                readRasterPixels(path,temp,px0,py0,1,1,band);
                val = temp->data[0][0];

                delete temp;
            }
        }else if(mode == LISEM_RASTER_SAMPLE_LINEAR)
        {
            QString path;
            if(m_IsDuo && band > 0)
            {
                path = m_Files2.at(timeindex);
                band = 0;
            }else {
                path = m_Files.at(timeindex);
            }
            MaskedRaster<float> raster_data(2,2,0.0,0.0,1.0);
            cTMap *temp = new cTMap(std::move(raster_data),"","");


            readRasterPixels(path,temp,px0,py0,2,2,band);

            double val_avg = 0.0;
            double nval = 0.0;
            val_avg += pcr::isMV(temp->data[0][0])?0.0:temp->data[0][0];
            nval += pcr::isMV(temp->data[0][0])?0.0:1.0;
            val_avg += pcr::isMV(temp->data[0][1])?0.0:temp->data[0][1];
            nval += pcr::isMV(temp->data[0][1])?0.0:1.0;
            val_avg += pcr::isMV(temp->data[1][0])?0.0:temp->data[1][0];
            nval += pcr::isMV(temp->data[1][0])?0.0:1.0;
            val_avg += pcr::isMV(temp->data[1][1])?0.0:temp->data[1][1];
            nval += pcr::isMV(temp->data[1][1])?0.0:1.0;

            if(nval > 0)
            {
                for(int r= 0; r < 2; r++)
                {
                    for(int c= 0; c < 2; c++)
                    {
                        if(pcr::isMV(temp->data[r][c]))
                        {
                            temp->data[r][c] = val_avg/nval;
                        }

                    }
                }

                val = (1.0- pixely_relpos) * ((pixelx_relpos * temp->data[0][1]) + (1.0 - pixelx_relpos)*temp->data[0][0]) + pixely_relpos * ((pixelx_relpos * temp->data[1][1]) + (1.0 - pixelx_relpos)*temp->data[1][0]);

            }else {

                val = temp->data[0][0];
            }


            delete temp;


        }

        return val;
    }

    inline void FillValuesToRaster(BoundingBox b, cTMap * raster, QMutex * m, bool * donesign, QMutex * m2, int band = 0, int timeindex = 0, std::function<void(void)> callback = [](){})
    {


        double tsx = GetTotalSizeX(band);
        double tsy = GetTotalSizeY(band);
        double csx = GetCellSizeX(band);
        double csy = GetCellSizeY(band);
        float rel_fracx = (b.GetMinX() - m_Props.ulx)/(tsx * csx);
        float rel_fracy = (b.GetMinY() - m_Props.uly)/(tsy * csy);
        float rel_frac2x = (b.GetMaxX() - m_Props.ulx)/(tsx * csx);
        float rel_frac2y = (b.GetMaxY() - m_Props.uly)/(tsy * csy);


        int px0 =  std::max(0,std::min(m_Props.sizex - 1,(int)(rel_fracx * float(m_Props.sizex))));
        int py0 =  std::max(0,std::min(m_Props.sizey - 1,(int)(rel_frac2y * float(m_Props.sizey))));

        int pxcount = std::max(0,std::min(m_Props.sizex - 1 - px0,(int)(rel_frac2x * float(m_Props.sizex) - px0)));
        int pycount = std::max(0,std::min(m_Props.sizey - 1 - py0,(int)(rel_fracy * float(m_Props.sizey) - py0)));

        if(m_Files.length() >  timeindex)
        {
            QString path;
            if(m_IsDuo && band > 0)
            {
                path = m_Files2.at(timeindex);
                band = 0;
            }else {
                path = m_Files.at(timeindex);
            }

            if(m2 == nullptr || m == nullptr || donesign == nullptr)
            {

                //do the rewriting on a thread
                std::thread t([callback,px0,py0,pxcount,band,pycount,path,raster,m,m2,donesign]()
                {

                       m->lock();

                       readRasterPixels(path,raster,px0,py0,pxcount,pycount,band);

                       m->unlock();

                       m2->lock();
                       *donesign = true;
                       m2->unlock();


                       callback();
                });

                t.join();
            }else
            {
                readRasterPixels(path,raster,px0,py0,pxcount,pycount,band);
                if(donesign != nullptr)
                {
                    *donesign = true;
                }

                callback();
            }

        }else {

        }

    }


    inline BoundingBox GetBoundingBox()
    {
        return BoundingBox(m_Props.ulx,m_Props.ulx + (double)(m_Props.sizex) * m_Props.cellsizex,m_Props.uly,m_Props.uly + (double)(m_Props.sizey) * m_Props.cellsizey);
    }

    inline GeoProjection GetProjection()
    {
        return GeoProjection::FromString(m_Props.projection);

    }

    inline bool Save(QJsonObject * obj)
    {
        if(m_IsSaveAble)
        {
            LSMSerialize::FromQListString(obj,"rasterfiles",m_FilesOrg);
            LSMSerialize::FromBool(obj,"is_duo",m_IsDuo);
            LSMSerialize::FromBool(obj,"is_memory",m_IsMemoryMap);

            return true;
        }
        return false;
    }

    inline bool load(QJsonObject * obj)
    {
        QList<QString> files;
        LSMSerialize::ToQListString(obj,"rasterfiles",files);

        std::cout <<"raster files load: " << files.length() << (( files.length() > 0)? files.at(0).toStdString():"") << std::endl;
        bool is_duo;
        LSMSerialize::ToBool(obj,"is_duo",is_duo);


        this->RasterDataProviderInitFromFileList(files,is_duo);

        bool setmem = false;
        LSMSerialize::ToBool(obj,"is_memory",setmem);

        if(setmem)
        {
            SetInMemory();
        }
        return true;
    }

    inline bool IsLDD()
    {
        return m_Props.is_ldd;
    }


    inline QString GetFilePathAtTime(int t)
    {
        if(m_IsFile)
        {
            if(m_Files.length() > 0)
            {
                m_Files.at(0);
            }
        }
        return "";
    }

    inline QList<double> GetTimeSeries(LSMVector2 loc, int band = 0)
    {
        QList<double> ret;

        if(m_HasTime)
        {
            for(int i = 0; i < m_Times.length(); i++)
            {
                ret.append(GetValueAtLocation(loc,LISEM_RASTER_SAMPLE_NEAREST, band, i));
            }

        }

        return ret;
    }


};




#endif // RASTERDATAPROVIDER_H

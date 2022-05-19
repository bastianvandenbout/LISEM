#ifndef RASTERCOMMON_H
#define RASTERCOMMON_H

#include "geo/raster/map.h"
#include "pcrtypes.h"
#include "defines.h"

#define LDD_VAL_LIST {0,1,2,3,4,5,6,7,8,9}
#define LDD_X_LIST {0, -1, 0, 1, -1, 0, 1, -1, 0, 1}
#define LDD_DIR_LENGTH 10
#define LDD_Y_LIST {0, 1, 1, 1, 0, 0, 0, -1, -1, -1}
#define LDD_DIST_DIAG 1.41421356237f
#define LDD_DIST 1.0f
#define LDD_DIST_LIST {1.0f,LDD_DIST_DIAG, LDD_DIST, LDD_DIST_DIAG,LDD_DIST, 1.0f, LDD_DIST,LDD_DIST_DIAG, LDD_DIST, LDD_DIST_DIAG }
#define LDD_PIT 5
#define LDD_FLAT 0
#define LDD_MAKE_TEMP(x) (x + LDD_DIR_LENGTH)
#define LDD_MAKE_ACTUAL(x) (x - LDD_DIR_LENGTH)
#define LDD_IS_ACTUAL(x) ((x> 0 && x < LDD_DIR_LENGTH) ? true : false)
#define LDD_IS_TEMP(x) (!(x < LDD_DIR_LENGTH) ? true : false)
#define INSIDE(map, r, c) (r>=0 && r<map->data.nr_rows() && c>=0 && c<map->data.nr_cols())
// check if cell From flows to To
#define FLOWS_TO(ldd, rFrom, cFrom, rTo, cTo) \
    ( ldd != 0 && rFrom >= 0 && cFrom >= 0 && rFrom+dy[ldd]==rTo && cFrom+dx[ldd]==cTo )


extern int n_warnhypergeom;



/// linked list structure for network in kin wave
typedef struct LDD_LINKEDLIST {
    int rowNr;
    int colNr;
    struct LDD_LINKEDLIST *prev;
}  LDD_LINKEDLIST;

/// pit structure
typedef struct LDD_PITPROPERTIES {
    int rowNr;
    int colNr;

    int catchment_nr;
    float catchment_area;
    float core_area;
    float core_volume;
    float core_ouflowdepth;
    float core_requiredprec;

    float dem_pit;
    float dem_lowestnb;
    int connectdir;
    int r_lowestnb;
    int c_lowestnb;
    int r_lowestnbconnect;
    int c_lowestnbconnect;

    int pi_flowinto = -1;

    bool mvnb;

}  LDD_PITPROPERTIES;

LISEM_API bool OUTORMV(cTMap * LDD, int r, int c);
LISEM_API int GetReverseLDD(int ldd);
LISEM_API int GetLDD(int dxin, int dyin);
LISEM_API int GetReverseLDD(int dxin, int dyin);
LISEM_API int ReversePath(cTMap * ldd, int r, int c);
LISEM_API bool ReversePath(cTMap * ldd, int rbegin, int cbegin, int rend, int cend);


inline int PitCompFunc(const  LDD_PITPROPERTIES a, const LDD_PITPROPERTIES b)
{
    return ( a.dem_pit >  b.dem_pit  );


}


inline LDD_LINKEDLIST * GetListRoot(cTMap * ldd, int r, int c)
{
    /// Linked list of cells in order of LDD flow network, ordered from pit upwards
    LDD_LINKEDLIST *list = nullptr, *temp = nullptr;
    list = (LDD_LINKEDLIST *)malloc(sizeof(LDD_LINKEDLIST));

    list->prev = nullptr;
    /// start gridcell: outflow point of area
    list->rowNr = r;
    list->colNr = c;

    return list;

}

inline LDD_LINKEDLIST * ListReplaceFirstByEQNB(cTMap * LDD,cTMap * Mask_Done, LDD_LINKEDLIST * list)
{
    if(list != nullptr)
    {
        LDD_LINKEDLIST *prev = list->prev;

        int rowNr = list->rowNr;
        int colNr = list->colNr;

        free(list);

        list = prev;

        LDD_LINKEDLIST *temp = NULL;
        int dx[LDD_DIR_LENGTH] = LDD_X_LIST;
        int dy[LDD_DIR_LENGTH] = LDD_Y_LIST;

        for(int i =1; i < LDD_DIR_LENGTH; i++)
        {
            int r, c;
            int val = 0;

            // this is the current cell
            if (i==LDD_PIT)
                continue;

            r = rowNr+dy[i];
            c = colNr+dx[i];

            if (INSIDE(LDD,r, c) && !pcr::isMV(LDD->data[r][c]))
            {
                val = (int) LDD->data[r][c];
            }
            else
            {
                continue;
            }

            // check if there are more cells upstream, if not subCatchDone remains true
            if (  val == rint((LDD->data[rowNr][colNr])) && pcr::isMV(Mask_Done->data[r][c]))
            {
                temp = (LDD_LINKEDLIST *)malloc(sizeof(LDD_LINKEDLIST));
                temp->prev = list;
                list = temp;
                list->rowNr = r;
                list->colNr = c;
            }

        }

    }


    return list;
}
inline LDD_LINKEDLIST * ListReplaceFirstByEQNBF2(cTMap * LDD,cTMap * Mask_Done, LDD_LINKEDLIST * list, float valoriginal, float ts = 0.0)
{
    if(list != nullptr)
    {
        LDD_LINKEDLIST *prev = list->prev;

        int rowNr = list->rowNr;
        int colNr = list->colNr;

        free (list);

        list = prev;

        LDD_LINKEDLIST *temp = NULL;
        int dx[LDD_DIR_LENGTH] = LDD_X_LIST;
        int dy[LDD_DIR_LENGTH] = LDD_Y_LIST;

        for(int i =1; i < LDD_DIR_LENGTH; i++)
        {
            int r, c;
            float val = 0;

            // this is the current cell
            if (i==LDD_PIT)
                continue;

            r = rowNr+dy[i];
            c = colNr+dx[i];

            if (INSIDE(LDD,r, c) && !pcr::isMV(LDD->data[r][c]))
            {
                val = LDD->data[r][c];
            }
            else
            {
                continue;
            }

            // check if there are more cells upstream, if not subCatchDone remains true
            if (  (std::fabs((val -(LDD->data[rowNr][colNr])) )< ts) && pcr::isMV(Mask_Done->data[r][c]))
            {
                temp = (LDD_LINKEDLIST *)malloc(sizeof(LDD_LINKEDLIST));
                temp->prev = list;
                list = temp;
                list->rowNr = r;
                list->colNr = c;
            }

        }

    }


    return list;
}

inline LDD_LINKEDLIST * ListReplaceFirstByEQNBF(cTMap * LDD,cTMap * Mask_Done, LDD_LINKEDLIST * list, float valoriginal, float ts = 0.0)
{
    if(list != nullptr)
    {
        LDD_LINKEDLIST *prev = list->prev;

        int rowNr = list->rowNr;
        int colNr = list->colNr;

        free (list);

        list = prev;

        LDD_LINKEDLIST *temp = NULL;
        int dx[LDD_DIR_LENGTH] = LDD_X_LIST;
        int dy[LDD_DIR_LENGTH] = LDD_Y_LIST;

        for(int i =1; i < LDD_DIR_LENGTH; i++)
        {
            int r, c;
            float val = 0;

            // this is the current cell
            if (i==LDD_PIT)
                continue;

            r = rowNr+dy[i];
            c = colNr+dx[i];

            if (INSIDE(LDD,r, c) && !pcr::isMV(LDD->data[r][c]))
            {
                val = LDD->data[r][c];
            }
            else
            {
                continue;
            }

            // check if there are more cells upstream, if not subCatchDone remains true
            if (  (std::fabs((val -(LDD->data[rowNr][colNr])) )< ts) && pcr::isMV(Mask_Done->data[r][c]))
            {
                temp = (LDD_LINKEDLIST *)malloc(sizeof(LDD_LINKEDLIST));
                temp->prev = list;
                list = temp;
                list->rowNr = r;
                list->colNr = c;
            }

        }

    }


    return list;
}

inline LDD_LINKEDLIST * ListReplaceFirstByUSNB(cTMap * LDD, LDD_LINKEDLIST * list)
{
    if(list != nullptr)
    {
        LDD_LINKEDLIST *prev = list->prev;

        int rowNr = list->rowNr;
        int colNr = list->colNr;

        free (list);

        list = prev;

        LDD_LINKEDLIST *temp = NULL;
        int dx[LDD_DIR_LENGTH] = LDD_X_LIST;
        int dy[LDD_DIR_LENGTH] = LDD_Y_LIST;

        for(int i =1; i < LDD_DIR_LENGTH; i++)
        {
            int r, c;
            int ldd = 0;

            // this is the current cell
            if (i==LDD_PIT)
                continue;

            r = rowNr+dy[i];
            c = colNr+dx[i];

            if (INSIDE(LDD,r, c) && !pcr::isMV(LDD->data[r][c]))
            {
                ldd = (int) LDD->data[r][c];
            }
            else
            {
                continue;
            }

            // check if there are more cells upstream, if not subCatchDone remains true
            if (    FLOWS_TO(ldd, r, c, rowNr, colNr) &&
                    INSIDE(LDD,r, c))
            {
                temp = (LDD_LINKEDLIST *)malloc(sizeof(LDD_LINKEDLIST));
                temp->prev = list;
                list = temp;
                list->rowNr = r;
                list->colNr = c;
            }

        }

    }


    return list;
}

inline static double MapMaximum(cTMap * map)
{
    float max = -1e31;
    bool set = false;
    for(int r = 0; r < map->nrRows(); r++)
    {
        for(int c = 0; c < map->nrCols();c++)
        {
            if(!pcr::isMV(map->data[r][c]))
            {
                if(set == false)
                {
                    set= true;
                    max = map->data[r][c];
                }else {
                    max = std::max(max,map->data[r][c]);
                }
            }
        }
    }
    return max;
}

inline static double MapMinimum(cTMap * map)
{
    float max = 1e31;
    bool set = false;
    for(int r = 0; r < map->nrRows(); r++)
    {
        for(int c = 0; c < map->nrCols();c++)
        {
            if(!pcr::isMV(map->data[r][c]))
            {
                if(set == false)
                {
                    set= true;
                    max = map->data[r][c];
                }else {
                    max = std::min(max,map->data[r][c]);
                }
            }
        }
    }
    return max;
}

inline float GetMapValue_3x3Av(cTMap * map, int r, int c)
{

    int dx[LDD_DIR_LENGTH] = LDD_X_LIST;
    int dy[LDD_DIR_LENGTH] = LDD_Y_LIST;


    float val = 0.0f;
    float n = 0.0f;

    for(int i = 1; i < LDD_DIR_LENGTH; i++)
    {
        int rn = r + dy[i];
        int cn = c + dx[i];;

        if(!(rn < 0 || cn < 0 || rn > map->data.nr_rows()-1 || cn > map->data.nr_cols()-1))
        {
            if(!pcr::isMV(map->data[rn][cn]))
            {
                n = n + 1.0f;
                val = val + map->data[rn][cn];
            }

        }
    }

    if((n > 0.0f))
    {
        val = val/n;
    }else
    {
        pcr::setMV(val);
    }

    return val;
}

inline float GetMapValue_OUTORMV3x3Av(cTMap * map, int r, int c)
{

    int dx[LDD_DIR_LENGTH] = LDD_X_LIST;
    int dy[LDD_DIR_LENGTH] = LDD_Y_LIST;


    //check if outside
    if(r < 0 || c < 0 || r > map->data.nr_rows()-1 || c > map->data.nr_cols()-1)
    {
        float val = 0.0f;
        float n = 0.0f;

        for(int i = 1; i < LDD_DIR_LENGTH; i++)
        {
            int rn = r + dy[i];
            int cn = c + dx[i];;

            if(!(rn < 0 || cn < 0 || rn > map->data.nr_rows()-1 || cn > map->data.nr_cols()-1))
            {
                if(!pcr::isMV(map->data[rn][cn]))
                {
                    n = n + 1.0f;
                    val = val + map->data[rn][cn];
                }

            }
        }

        if((n > 0.0f))
        {
            val = val/n;
        }else
        {
            pcr::setMV(val);
        }

        return val;


    }else if(pcr::isMV(map->data[r][c]))
    {
        float val = 0.0f;
        float n = 0.0f;



        for(int i = 1; i < LDD_DIR_LENGTH; i++)
        {
            int rn = r + dy[i];
            int cn = c + dx[i];;

            if(!(rn < 0 || cn < 0 || rn > map->data.nr_rows()-1 || cn > map->data.nr_cols()-1))
            {
                if(!pcr::isMV(map->data[rn][cn]))
                {
                    n = n + 1.0f;
                    val = val + map->data[rn][cn];
                }

            }
        }

        if((n > 0.0f))
        {
            val = val/n;
        }else
        {
            pcr::setMV(val);
        }

        return val;


    }else {
        return map->data[r][c];
    }

}

inline float GetMapValue_AvRepl(cTMap * map, int r, int c)
{
    if(r > -1 && c > -1 && r < map->nrRows() && c < map->nrCols())
    {
        if(!pcr::isMV(map->data[r][c]))
        {
            return map->data[r][c];

        }
    }

    return GetMapValue_OUTORMV3x3Av(map,r,c);
}



inline bool CheckMapsSameSizeError(QString functionname,std::vector<QString> argnames, std::vector<cTMap *> maps)
{
    if(maps.size() == 0)
    {
        return true;
    }
    int sizex = maps.at(0)->nrCols();
    int sizey = maps.at(0)->nrRows();

    for(int i = 0; i < maps.size(); i++)
    {

        if(maps.at(i)->nrCols() != sizex || maps.at(i)->nrRows() != sizey)
        {
            LISEMS_ERROR("Could not do function " + functionname+ " with inputs maps of different size : " + argnames.at(i));
            throw 1;
            return false;
        }
    }

    return true;
}

inline bool CheckMapsSameSize(std::vector<cTMap *> maps)
{
    if(maps.size() == 0)
    {
        return true;
    }
    int sizex = maps.at(0)->nrCols();
    int sizey = maps.at(0)->nrRows();

    for(int i = 0; i < maps.size(); i++)
    {

        if(maps.at(i)->nrCols() != sizex || maps.at(i)->nrRows() != sizey)
        {
            return false;
        }
    }

    return true;
}

#endif // RASTERCOMMON_H

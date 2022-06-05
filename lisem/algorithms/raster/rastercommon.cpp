#include "raster/rastercommon.h"


int n_warnhypergeom = 0;

bool OUTORMV(cTMap * LDD, int r, int c)
{
    if(r>=0 && r<LDD->nrRows() && c>=0 && c<LDD->nrCols())
    {
        if(!pcr::isMV(LDD->data[r][c]))
        {
            return false;
        }
    }
    return true;

}

int GetReverseLDD(int ldd)
{

    int dx[LDD_DIR_LENGTH] = LDD_X_LIST;
    int dy[LDD_DIR_LENGTH] = LDD_Y_LIST;

    for(int j = 1; j < LDD_DIR_LENGTH; j++)
    {
        if((-dx[j] == dx[ldd]) && (-dy[j] == dy[ldd]))
        {
            return j;
        }
    }
    return LDD_PIT;
}

int GetLDD(int dxin, int dyin)
{

    int dx[LDD_DIR_LENGTH] = LDD_X_LIST;
    int dy[LDD_DIR_LENGTH] = LDD_Y_LIST;

    for(int j = 1; j < LDD_DIR_LENGTH; j++)
    {
        if(dx[j] == dxin && dy[j] == dyin)
        {
            return j;
        }
    }
    return LDD_PIT;
}

int GetReverseLDD(int dxin, int dyin)
{

    int dx[LDD_DIR_LENGTH] = LDD_X_LIST;
    int dy[LDD_DIR_LENGTH] = LDD_Y_LIST;

    for(int j = 1; j < LDD_DIR_LENGTH; j++)
    {
        if(-dx[j] == dxin && -dy[j] == dyin)
        {
            return j;
        }
    }
    return LDD_PIT;
}

int ReversePath(cTMap * ldd, int r, int c)
{
    return GetReverseLDD(ldd->data[r][c]);
}

bool ReversePath(cTMap * ldd, int rbegin, int cbegin, int rend, int cend)
{
    int dx[LDD_DIR_LENGTH] = LDD_X_LIST;
    int dy[LDD_DIR_LENGTH] = LDD_Y_LIST;

    int r = rend;
    int c = cend;

    int rprev = r;
    int cprev = c;


    int lddval = (int) ldd->data[r][c];
    int lddvalold = ldd->data[r][c];

    bool first = true;
    while((LDD_IS_ACTUAL(lddval)) )
    {

        if((lddval == LDD_PIT) && (r == rbegin && c == cbegin))
        {

            //find direction toward previous cell
            ldd->data[r][c] = GetLDD(cprev-c,rprev-r);
            break;
        }else if((lddval == LDD_PIT))
        {

            ldd->data[r][c] = GetLDD(cprev-c,rprev-r);
            break;
        }

        if((r == rbegin && c == cbegin))
        {
            break;
        }

        int lddvalnew;


        if(first)
        {
            lddvalnew = GetReverseLDD(lddvalold);
        }else {
            lddvalnew = GetReverseLDD(lddvalold);
        }

        ldd->data[r][c] = lddvalnew;
        first = false;

        rprev = r;
        cprev = c;
        r = r + dy[lddval];
        c = c + dx[lddval];

        lddvalold = lddval;
        lddval = (int) ldd->data[r][c];

    }

    return true;

}



int GetReverseLDD4(int ldd)
{

    int dx[LDD4_DIR_LENGTH] = LDD4_X_LIST;
    int dy[LDD4_DIR_LENGTH] = LDD4_Y_LIST;

    for(int j = 1; j < LDD4_DIR_LENGTH; j++)
    {
        if((-dx[j] == dx[ldd]) && (-dy[j] == dy[ldd]))
        {
            return j;
        }
    }
    return LDD4_PIT;
}

int GetLDD4(int dxin, int dyin)
{

    int dx[LDD4_DIR_LENGTH] = LDD4_X_LIST;
    int dy[LDD4_DIR_LENGTH] = LDD4_Y_LIST;

    for(int j = 1; j < LDD4_DIR_LENGTH; j++)
    {
        if(dx[j] == dxin && dy[j] == dyin)
        {
            return j;
        }
    }
    return LDD4_PIT;
}

int GetReverseLDD4(int dxin, int dyin)
{

    int dx[LDD4_DIR_LENGTH] = LDD4_X_LIST;
    int dy[LDD4_DIR_LENGTH] = LDD4_Y_LIST;

    for(int j = 1; j < LDD4_DIR_LENGTH; j++)
    {
        if(-dx[j] == dxin && -dy[j] == dyin)
        {
            return j;
        }
    }
    return LDD4_PIT;
}

int ReversePath4(cTMap * ldd, int r, int c)
{
    return GetReverseLDD4(ldd->data[r][c]);
}

bool ReversePath4(cTMap * ldd, int rbegin, int cbegin, int rend, int cend)
{
    int dx[LDD4_DIR_LENGTH] = LDD4_X_LIST;
    int dy[LDD4_DIR_LENGTH] = LDD4_Y_LIST;

    int r = rend;
    int c = cend;

    int rprev = r;
    int cprev = c;


    int lddval = (int) ldd->data[r][c];
    int lddvalold = ldd->data[r][c];

    bool first = true;
    while((LDD4_IS_ACTUAL(lddval)) )
    {

        if((lddval == LDD4_PIT) && (r == rbegin && c == cbegin))
        {

            //find direction toward previous cell
            ldd->data[r][c] = GetLDD4(cprev-c,rprev-r);
            break;
        }else if((lddval == LDD4_PIT))
        {

            ldd->data[r][c] = GetLDD4(cprev-c,rprev-r);
            break;
        }

        if((r == rbegin && c == cbegin))
        {
            break;
        }

        int lddvalnew;


        if(first)
        {
            lddvalnew = GetReverseLDD4(lddvalold);
        }else {
            lddvalnew = GetReverseLDD4(lddvalold);
        }

        ldd->data[r][c] = lddvalnew;
        first = false;

        rprev = r;
        cprev = c;
        r = r + dy[lddval];
        c = c + dx[lddval];

        lddvalold = lddval;
        lddval = (int) ldd->data[r][c];

    }

    return true;

}

#ifndef FLUID2P2D_H
#define FLUID2P2D_H

#include "defines.h"
#include "geo/raster/map.h"

#include <vector>
#include <queue>

#define	GRAVITY		9.8
#define	DT			0.005
#define DIST		(0.1)
#define REDIST		1

template <class T> T ** alloc2D( int nx, int ny ) {
    T **ptr = new T *[nx+1];
    for( int i=0; i<nx; i++ ) {
        ptr[i] = new T[ny+1];
    }
    ptr[nx] = 0;
    return ptr;
}



static inline float hypot2( float a, float b ) {
    return a*a+b*b;
}


static void intersect( float x1, float y1, float x2, float y2, float &x, float &y  ) {
    float d = hypot2(x2-x1,y2-y1);
    float u = ((x-x1)*(x2-x1) + (y-y1)*(y2-y1))/d;
    u = fmin(1.0,fmax(0.0,u));
    x = x1 + u*(x2-x1);
    y = y1 + u*(y2-y1);
}


template <class T> void free2D( T **ptr ) {
    for( int i=0; ptr[i]; i++ ) delete [] ptr[i];
    delete [] ptr;
}

class Fluid2P2D
{

public:

    char subcell = 1;
    char show_velocity = 0;
    char show_dist = 0;
    char show_grid = 0;
    char show_region = 1;
    char interpMethd = 1;
    char do_redistance = 1;
    char do_volumeCorrection = 1;
    char solver_mode = 2;
    float maxdist = DIST;
    float volume0 = 0.0;
    float y_volume0 = 0.0;
    float volume_error = 0.0;
    float vdiv = 0.0;
    int gx = 0;
    int gy = 0;
    int method = 1;

    cTMap * ux = NULL;		// Access Bracket u[DIM][X][Y] ( Staggered Grid )
    cTMap * uy = NULL;
    cTMap * p = NULL;		// Equivalent to p[N][N]
    cTMap * d = NULL;		// Equivalent to d[N][N]
    cTMap * A = NULL;		// Level Set Field
    cTMap * source_dists = NULL;
    cTMap * swap_dists = NULL;
    cTMap * tmp = NULL;

    char **computed;
    char **region;
    cTMap *q;
    cTMap *q2;
    typedef struct {
            char known;
            char estimated;
            float dist;
            float pos[2];
            int gp[2];
        } grid;

    grid ** grids = NULL;
    cTMap * gradx = NULL;
    cTMap * grady = NULL;
    cTMap * test_q = NULL;
    cTMap *r = NULL;
    cTMap *P = NULL;
    cTMap *r2 = NULL;
    cTMap *z = NULL;
    cTMap *s = NULL;
    cTMap *ux1 = NULL;
    cTMap *ux2 = NULL;
    cTMap *uy1 = NULL;
    cTMap *uy2 = NULL;
    cTMap *ux_swap = NULL;
    cTMap *uy_swap = NULL;
    cTMap *block = NULL;
    cTMap *blocku = NULL;
    cTMap *blockv = NULL;
    struct gridComparison{
        bool operator () ( grid * left, grid * right){
            return fabs(left->dist) > fabs(right->dist);
        }
    };
    typedef std::priority_queue<grid *,std::vector<grid*>,gridComparison> grid_queue;


    float dx = 1.0;
    float dy = 1.0;

    float g = 9.81;
    float visc = 0.0;
    float b0 = 1.7;
    float e = 0.0001;

private:

    inline cTMap * CreateMap(int rows, int cols, bool setmv = false)
    {
        cTMap * m;
        MaskedRaster<float> data = MaskedRaster<float>(cols,rows,0.0,0.0,1.0,1.0);
        m = new cTMap(std::move(data),"","",false);
        if(setmv)
        {
            m->setAllMV();
        }
        return m;
    }
public:
    inline Fluid2P2D(int rows, int cols, float _dx, float _dy, float _b0)
    {
        gx = cols;
        gy = rows;
        ux = CreateMap(rows+2,cols+2);		// Access Bracket u[DIM][X][Y] ( Staggered Grid )
        uy = CreateMap(rows+2,cols+2);
        p = CreateMap(rows+1,cols+1);		// Equivalent to p[N][N]
        d = CreateMap(rows+1,cols+1);		// Equivalent to d[N][N]
        A = CreateMap(rows+1,cols+1);
        source_dists = CreateMap(rows+1,cols+1);
        swap_dists = CreateMap(rows+1,cols+1);
        region = alloc2D<char>(gx,gy);
        q = CreateMap(rows+1,cols+1);
        computed = alloc2D<char>(gx,gy);
        r = CreateMap(rows+1,cols+1);
        p = CreateMap(rows+1,cols+1);
        r2 = CreateMap(rows+1,cols+1);
        z = CreateMap(rows+1,cols+1);
        s = CreateMap(rows+1,cols+1);
        q2 = CreateMap(rows+1,cols+1);
        tmp = CreateMap(rows+1,cols+1);
        P = CreateMap(rows+1,cols+1);
        block = CreateMap(rows+1,cols+1);
        blocku = CreateMap(rows+1,cols+1);
        blockv = CreateMap(rows+1,cols+1);
        ux1 = CreateMap(rows+1,cols+1);
        ux2 = CreateMap(rows+1,cols+1);
        uy1 = CreateMap(rows+1,cols+1);
        uy2 = CreateMap(rows+1,cols+1);
        ux_swap = CreateMap(rows+1,cols+1);
        uy_swap = CreateMap(rows+1,cols+1);

        InitLevelSet(rows,cols);
        if( do_redistance )
        {
             maxdist = DIST;
        }else
        {
            maxdist = 1.0;
        }

        buildLevelset(maxdist);
        setInterpMethod(interpMethd);

        volume0 = getVolume();
        y_volume0 = 0.0;
    }

    inline void InitializeFromData(cTMap * FH, cTMap * FUx, cTMap * FUy, cTMap * Fp, cTMap * LevelSet, cTMap * blk, cTMap * blkux, cTMap * blkuy)
    {

        bool has_levelset = false;

        float FUxt  = 0.0;
        int n = 0;
        // Initialize Distances
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                if(!pcr::isMV(LevelSet->data[j][i]))
                {
                    has_levelset = true;
                }
                FUxt += FUx->data[j][i];
                n++;
            }
        }
        if(n > 0)
        {
            FUxt = FUxt/((float)(n));
        }

        // Initialize Distances
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                grids[i][j].known = true;
                if(has_levelset)
                {
                    if(pcr::isMV(LevelSet->data[j][i]))
                    {
                        grids[i][j].known = false;
                        grids[i][j].dist = 0.0;

                    }else {
                        if(blk->data[j][i] <=  0.5f)
                        {
                            grids[i][j].dist = LevelSet->data[j][i];
                        }else
                        {
                            grids[i][j].dist = 1.0;
                        }
                    }
                }else {
                    grids[i][j].dist = FH->data[j][i] > 0.5f? -1.0:1.0;
                }
                grids[i][j].pos[0] = 0.0;
                grids[i][j].pos[1] = 0.0;
                p->data[i][j] = Fp->data[j][i];
                ux->data[i][j] = FUx->data[j][i];
                uy->data[i][j] = FUy->data[j][i];
                block->data[i][j] = blk->data[j][i];
                blocku->data[i][j] = blkux->data[j][i];
                blockv->data[i][j] = blkuy->data[j][i];
            }
        }

        // Redistance...
        redistance(maxdist);


        //volume0 = getVolume();
        //y_volume0 = 0.0;
    }

    inline void StoreToData(cTMap * FH, cTMap * FUx, cTMap * FUy, cTMap * Fp, cTMap * LevelSet)
    {
        LevelSet->setAllMV();
        // Initialize Distances
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                FUx->data[j][i] = ux->data[i][j];
                FUy->data[j][i] = uy->data[i][j];
                Fp->data[j][i] = p->data[i][j];
                if(grids[i][j].known)
                {
                    LevelSet->data[j][i] = grids[i][j].dist;
                    if(grids[i][j].dist < 0)
                    {
                        FH->data[j][i] = 1.0;
                    }else {
                        FH->data[j][i] = 0.0;
                    }

                }else {
                    FH->data[j][i] = 0.0;
                }
            }
        }
    }

    inline void calcMarchingPoints( int i, int j, float p[8][2], int &pnum ) {
        pnum = 0;
        float wx = 1.0/(gx-1);
        float wy = 1.0/(gy-1);
        int quads[][2] = { {i, j}, {i+1, j}, {i+1, j+1}, {i, j+1} };
        for( int n=0; n<4; n++ ) {
            if( grids[quads[n][0]][quads[n][1]].known ) {
                // Inside Liquid
                if( grids[quads[n][0]][quads[n][1]].dist < 0.0 ) {
                    p[pnum][0] = wx*quads[n][0];
                    p[pnum][1] = wy*quads[n][1];
                    pnum ++;
                }
                // If Line Crossed
                if( grids[quads[n][0]][quads[n][1]].dist * grids[quads[(n+1)%4][0]][quads[(n+1)%4][1]].dist < 0 ) {
                    // Calculate Cross Position
                    float y0 = grids[quads[n][0]][quads[n][1]].dist;
                    float y1 = grids[quads[(n+1)%4][0]][quads[(n+1)%4][1]].dist;
                    float a = y0/(y0-y1);
                    float p0[2] = { wx*quads[n][0], wy*quads[n][1] };
                    float p1[2] = { wx*quads[(n+1)%4][0], wy*quads[(n+1)%4][1] };
                    p[pnum][0] = (1.0-a)*p0[0]+a*p1[0];
                    p[pnum][1] = (1.0-a)*p0[1]+a*p1[1];
                    pnum ++;
                }
            }
        }
    }

    inline void flow( float x, float y, float &uu, float &vv, float &dt ) {
        x = (gx-1)*fmin(1.0,fmax(0.0,x))+0.5;
        y = (gy-1)*fmin(1.0,fmax(0.0,y))+0.5;
        int i = x;
        int j = y;
        uu = (1.0-(x-i))*ux->data[i][j] + (x-i)*ux->data[i+1][j];
        vv = (1.0-(y-j))*uy->data[i][j] + (y-j)*uy->data[i][j+1];
        dt = DT;
    }
    inline float getVolume() {
        float volume = 0.0;
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                float p[8][2];
                int pnum;
                calcMarchingPoints( i, j, p, pnum );
                for( int m=0; m<pnum; m++ )
                {
                    volume += p[m][0]*p[(m+1)%pnum][1]-p[m][1]*p[(m+1)%pnum][0];
                }
            }
        }

        return volume*0.5;
    }


    inline void addGravity() {

int n = 0; int n2 = 0;
        for(int i = 0; i < gx+1; i++)
        {
            for(int j = 0; j < gy; j++)
            {

                if( j>0 && j<gy-1 && (A->data[i][j]<0.0 || A->data[i][j-1]<0.0))
                {
                    n ++;
                    uy->data[i][j] += DT*GRAVITY;
                }
                else
                {
                    n2 ++;
                    uy->data[i][j] = 0.0;
                }
            }
        }
        std::cout << "n " << n << " " << n2 << std::endl;

        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy+1; j++)
            {

            if( i>0 && i<gx-1 && (A->data[i][j]<0.0 || A->data[i-1][j]<0.0))
            {
                ux->data[i][j] += 0.0;
            }
            else
            {
                ux->data[i][j] = 0.0;
            }
            }
        }
    }

    inline void InitLevelSet(int rows, int cols)
    {
        grids = alloc2D<grid>(cols+1,rows+1);
        gradx = CreateMap(rows+1,cols+1);
        grady = CreateMap(rows+1,cols+1);
        test_q = CreateMap(rows+1,cols+1);
       for(int i = 0; i < gx; i++)
       {
           for(int j = 0; j < gy; j++)
           {

                grids[i][j].dist = 1.0;
                grids[i][j].known = false;
                grids[i][j].gp[0] = i;
                grids[i][j].gp[1] = j;
                gradx->data[i][j] = 0.0f;
                grady->data[i][j] = 0.0;
            }
       }
    }

    inline void buildLevelset(float tolerance)
    {
        // Initialize Distances
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                float x = i/(float)(gx-1);
                float y = j/(float)(gy-1);
                float sx = x;
                float sy = y;
                bool hit;
                hit = hypot(x-0.5,y-0.8) < 0.15 || y < 0.2;
                grids[i][j].known = true;
                grids[i][j].dist = hit ? -1.0 : 1.0;
                grids[i][j].pos[0] = 0.0;
                grids[i][j].pos[1] = 0.0;
            }
        }

        // Redistance...
        redistance(tolerance);
    }


    // Helper Function Used In FastMarch()
    inline bool update_distance( int i, int j, float pos[2], float &dist ) {
        float x = i/(LSMFLOAT)(gx-1);
        float y = j/(LSMFLOAT)(gy-1);
        dist = 1.0;
        bool updated = false;

        // For Neigboring Grids
        int query[][2] = { {i+1,j},{i-1,j},{i,j+1},{i,j-1},{i-1,j-1},{i-1,j+1},{i+1,j-1},{i+1,j+1} };
        for( int q=0; q<8; q++ ) {
            int qi = query[q][0];
            int qj = query[q][1];
            if( qi>=0 && qi<gx && qj>=0 && qj<gy ) {
                float sgn = grids[qi][qj].dist > 0 ? 1.0 : -1.0;
                // If The Neighborhood Is A Known Grid
                if( grids[qi][qj].known ) {
                    // Compute The Distance From The Point Of Its Neighbors
                    float d = hypot(grids[qi][qj].pos[0]-x,grids[qi][qj].pos[1]-y);
                    // If The Distance Is Closer Than Holding One, Just Replace
                    if( d < fabs(dist) ) {
                        dist = sgn*d;
                        for( int n=0; n<2; n++ ) pos[n] = grids[qi][qj].pos[n];
                        updated = true;
                    }
                }
            }
        }
        return updated;
    }


    inline void fastMarch( float tolerance ) {
        // Unknowns
        grid_queue unknowns;

        // Compute First Estimate of Distances
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                float pos[2];
                float dist;
                if( ! grids[i][j].known ) {
                    grids[i][j].estimated = false;
                    if( update_distance(i,j,pos,dist)) {
                        grids[i][j].dist = dist;
                        if( fabs(dist) < tolerance ) {
                            grids[i][j].pos[0] = pos[0];
                            grids[i][j].pos[1] = pos[1];
                            grids[i][j].estimated = true;
                            unknowns.push(&grids[i][j]);
                        }
                    } else {
                        grids[i][j].dist = 1.0;
                    }
                }
            }
        }

        // While Unknown Grid Exists
        while( ! unknowns.empty() ) {
            // Pop Out Top
            grid *mingrid = unknowns.top();
            int i = mingrid->gp[0];
            int j = mingrid->gp[1];
            unknowns.pop();
            grids[i][j].estimated = false;
            mingrid->known = true;

            // Dilate...
            int query[][2] = { {i+1,j},{i-1,j},{i,j+1},{i,j-1},{i-1,j-1},{i-1,j+1},{i+1,j-1},{i+1,j+1} };
            for( int q=0; q<8; q++ ) {
                int qi = query[q][0];
                int qj = query[q][1];
                if( qi>=0 && qi<gx && qj>=0 && qj<gy ) {
                    if( ! grids[qi][qj].estimated && ! grids[qi][qj].known ) {
                        LSMFLOAT pos[2];
                        LSMFLOAT dist;
                        if( update_distance(qi,qj,pos,dist)) {
                            if( fabs(dist) < tolerance ) {
                                grids[qi][qj].dist = dist;
                                grids[qi][qj].pos[0] = pos[0];
                                grids[qi][qj].pos[1] = pos[1];
                                grids[qi][qj].estimated = true;
                                unknowns.push(&grids[qi][qj]);
                            }
                        }
                    }
                }
            }
        }

    #if 1 // TODO check this code for gx an gy limits
        // Fill Inner Region With Scanline Order

        int dir = 0;
            for( int j=0; j<gy; j++ ) {
                float sgn = 1.0;

                // Forward Search
                for( int i=0; i<gx; i++ ) {
                    int idx[][2] = { {i,j} };

                    if( grids[idx[dir][0]][idx[dir][1]].known ) {
                        sgn = grids[idx[dir][0]][idx[dir][1]].dist < 0.0 ? -1.0 : 1.0;
                    } else if( sgn < 0.0 ) {
                        grids[idx[dir][0]][idx[dir][1]].dist = -1.0;
                        grids[idx[dir][0]][idx[dir][1]].known = true;
                    }
                }

                // Backward Search
                for( int i=gx-1; i>=0; i-- ) {
                    int idx[][2] = { {i,j}};

                    if( grids[idx[dir][0]][idx[dir][1]].known ) {
                        sgn = grids[idx[dir][0]][idx[dir][1]].dist < 0.0 ? -1.0 : 1.0;
                    } else if( sgn < 0.0 ) {
                        grids[idx[dir][0]][idx[dir][1]].dist = -1.0;
                        grids[idx[dir][0]][idx[dir][1]].known = true;
                    }
                }
            }

                for( int j=0; j<gx; j++ ) {
                    float sgn = 1.0;

                    // Forward Search
                    for( int i=0; i<gy; i++ ) {
                        int idx[][2] = { {j,i} };

                        if( grids[idx[dir][0]][idx[dir][1]].known ) {
                            sgn = grids[idx[dir][0]][idx[dir][1]].dist < 0.0 ? -1.0 : 1.0;
                        } else if( sgn < 0.0 ) {
                            grids[idx[dir][0]][idx[dir][1]].dist = -1.0;
                            grids[idx[dir][0]][idx[dir][1]].known = true;
                        }
                    }

                    // Backward Search
                    for( int i=gy-1; i>=0; i-- ) {
                        int idx[][2] = { {j,i}};

                        if( grids[idx[dir][0]][idx[dir][1]].known ) {
                            sgn = grids[idx[dir][0]][idx[dir][1]].dist < 0.0 ? -1.0 : 1.0;
                        } else if( sgn < 0.0 ) {
                            grids[idx[dir][0]][idx[dir][1]].dist = -1.0;
                            grids[idx[dir][0]][idx[dir][1]].known = true;
                        }
                    }
                }

    #endif
    }


    inline void redistance( float tolerance ) {
        float w = 1.0/(gx-1);
        float v = 1.0/(gy-1);
        // Make Everything Known First
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                if( ! grids[i][j].known ) grids[i][j].dist = 1.0;
                grids[i][j].known = true;
            }
        }

        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {

                bool farCell = true;
                int query[][2] = { {i+1,j},{i+1,j+1},{i,j+1},{i-1,j+1},{i-1,j},{i-1,j-1},{i,j-1},{i+1,j-1} };
                int qnum = 8;
                float pos[qnum][2];
                bool fnd[qnum];
                for( int q=0; q<qnum; q++ ) {
                    int qi = query[q][0];
                    int qj = query[q][1];
                    fnd[q] = false;
                    if( qi>=0 && qi<gx && qj>=0 && qj<gy ) {
                        if( grids[qi][qj].known && grids[qi][qj].dist * grids[i][j].dist < 0.0 ) {
                            farCell = false;

                            // Calculate New Cross Position
                            float y0 = grids[i][j].dist;
                            float y1 = grids[qi][qj].dist;
                            float a = y0/(y0-y1);
                            float p0[2] = { w*i, v*j };
                            float p1[2] = { w*qi, v*qj };
                            pos[q][0] = (1.0-a)*p0[0]+a*p1[0];
                            pos[q][1] = (1.0-a)*p0[1]+a*p1[1];
                            fnd[q] = true;
                        }
                    }
                }
                if( farCell ) {
                    grids[i][j].known = false;
                } else {
                    grids[i][j].known = true;
                    float mind = 9999.0;
                    for ( int q=0; q<qnum; q++ ) {
                        for( int rp=1; rp<3; rp++ ) {
                            if( fnd[(q+rp)%qnum] && fnd[q] ) {
                                float x = i*w;
                                float y = j*v;
                                intersect( pos[(q+rp)%qnum][0], pos[(q+rp)%qnum][1], pos[q][0], pos[q][1], x, y );
                                float d = hypot(x-i*w,y-j*v);
                                if( d < mind ) {
                                    mind = d;
                                    grids[i][j].pos[0] = x;
                                    grids[i][j].pos[1] = y;
                                    grids[i][j].dist = (grids[i][j].dist > 0.0 ? 1.0 : -1.0)*fmax(d,1.0e-8);
                                }
                            }
                        }
                    }

                    for ( int q=0; q<qnum; q++ ) {
                        if( !fnd[q] && fnd[(q+1)%qnum] && !fnd[(q+2)%qnum] ) {
                            float x = pos[(q+1)%qnum][0];
                            float y = pos[(q+1)%qnum][1];
                            float d = hypot(x-i*w,y-j*v);
                            if( d < mind ) {
                                mind = d;
                                grids[i][j].pos[0] = x;
                                grids[i][j].pos[1] = y;
                                grids[i][j].dist = (grids[i][j].dist > 0.0 ? 1.0 : -1.0)*fmax(d,1.0e-8);
                            }
                        }
                    }
                }
            }
        }

        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                if( ! grids[i][j].known )
                {
                    grids[i][j].dist = 1.0;
                }
            }

        }

        fastMarch(tolerance);
    }



    inline float clamp( float v, float min, float max ) {
        if( v < min ) return min;
        if( v > max ) return max;
        return v;
    }

    inline int iclamp( int v, int min, int max ) {
        if( v < min ) return min;
        if( v > max ) return max;
        return v;
    }

    inline float spline_cubic(const float a[4], float x) {
        if( a[0] == a[1] && a[0] == a[2] && a[0] == a[3] ) return a[0];
        int i, j;
        float alpha[4], l[4], mu[4], z[4];
        float b[4], c[4], d[4];
        for(i = 1; i < 3; i++)
            alpha[i] = 3. * (a[i+1] - a[i]) - 3. * (a[i] - a[i-1]);
        l[0] = 1.;
        mu[0] = 0.;
        z[0] = 0.;
        for(i = 1; i < 3; i++){
            l[i] = 4. - mu[i-1];
            mu[i] = 1. / l[i];
            z[i] = (alpha[i] - z[i-1]) / l[i];
        }
        l[3] = 1.;
        z[3] = 0.;
        c[3] = 0.;
        for(j = 2; 0 <= j; j--){
            c[j] = z[j] - mu[j] * c[j+1];
            b[j] = a[j+1] - a[j] - (c[j+1] + 2. * c[j]) / 3.;
            d[j] = (c[j+1] - c[j]) / 3.;
        }
        return clamp( a[1] + b[1] * x + c[1] * x * x + d[1] * x * x * x, fmin(a[1],a[2]), fmax(a[1],a[2]) );
    }

    inline float spline( cTMap *d, float x, float y, int w, int h ) {
        float f[16];
        float xn[4];

        if (x<0.0) x=0.0; if (x>=w) x=w-0.01;
        if (y<0.0) y=0.0; if (y>=h) y=h-0.01;

        for( int j=0; j<4; j++ ) {
            for( int i=0; i<4; i++ ) {
                int hd = (int)x - 1 + i;
                int v = (int)y - 1 + j;
                f[4*j+i] = d->data[iclamp(hd,0,w-1)][iclamp(v,0,h-1)];
            }
        }

        for( int j=0; j<4; j++ ) xn[j] = spline_cubic( &f[4*j], x - (int)x );
        return spline_cubic( xn, y - (int)y );
    }

    inline float linear ( cTMap *d, float x, float y, int w, int h ) {
        x = fmax(0.0,fmin(w,x));
        y = fmax(0.0,fmin(h,y));
        int i = std::min(x,((float)(w-2)));
        int j = std::min(y,((float)(h-2)));

        return ((i+1-x)*d->data[i][j]+(x-i)*d->data[i+1][j])*(j+1-y) + ((i+1-x)*d->data[i][j+1]+(x-i)*d->data[i+1][j+1])*(y-j);
    }

    inline float interp ( cTMap *d, float x, float y, int w, int h ) {
        float r = 0.0;
        switch (method) {
            case 0:
                r = linear(d,x,y,w,h);
                break;
            case 1:
                r = spline(d,x,y,w,h);
                break;
        }
        return r;
    }

    inline void setInterpMethod( int num ) {
        method = num;
    }


    inline void Destroy()
    {
        delete ux; ux = nullptr;
        delete uy; uy = nullptr;
        delete p; p = nullptr;
        delete d; d = nullptr;
        delete A; A = nullptr;
        delete gradx; gradx = nullptr;
        delete grady; grady = nullptr;
        delete test_q; test_q = nullptr;
        delete source_dists; source_dists = nullptr;
        delete swap_dists; swap_dists = nullptr;
        delete r; r = nullptr;
        delete p; p = nullptr;
        delete q; q = nullptr;
        delete q2; q2 = nullptr;
        delete z; z = nullptr;
        delete s; s = nullptr;
        delete r2; r2 = nullptr;
        delete tmp; tmp = nullptr;
        delete block; block = nullptr;
        delete blocku; blocku = nullptr;
        delete blockv; blockv = nullptr;
        free2D(grids);
    }

    inline ~Fluid2P2D()
    {


    }

    inline void markLiquid() {
        getLevelSet(A);
    }
    inline float getLevelSet( int i, int j ) {
        if( i<0 || i>gx-1 || j<0 || j>gy-1 || block->data[i][j] > 0.5f) return 1.0;
        return grids[i][j].dist;
    }

    inline void getLevelSet( cTMap *dists ) {

        int n = 0;
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                if(block->data[i][j])
                {
                     dists->data[i][j] = 1.0;
                }
                dists->data[i][j] = grids[i][j].known ? grids[i][j].dist : 1.0;

            }
        }
    }


    inline void advect()
    {


        // Copy Dists
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                source_dists->data[i][j] = grids[i][j].dist;
            }

        }

        // Advect
        float wx = 1.0/(gx-1);
        float wy = 1.0/(gy-1);
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                float x, y;
                float u, v, dt;
                x = i*wx;
                y = j*wy;
                flow( x, y, u, v, dt );
                // Semi-Lagragian
                x -= dt*u;
                y -= dt*v;
                x = fmin(1.0,fmax(0.0,x));
                y = fmin(1.0,fmax(0.0,y));
                // Interpolate Dists
                if( grids[(int)((gx-1)*x)][(int)((gy-1)*y)].known ) {
                    swap_dists->data[i][j] = y<1.0 ? interp( source_dists, (gx-1)*x, (gy-1)*y, gx, gy ) : 1.0;
                } else {
                    swap_dists->data[i][j] = source_dists->data[i][j];
                }
            }

        }

        // Swap
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                if(block->data[i][j] <= 0.5f)
                {
                    grids[i][j].dist = swap_dists->data[i][j];
                }else {
                    grids[i][j].dist = 1.0;
                }
            }
        }
    }

    inline void computeVolumeError() {
        float curVolume = getVolume();
        if( ! volume0 || ! do_volumeCorrection || ! curVolume ) {
            vdiv = 0.0;
            return;
        }
        volume_error = volume0-curVolume;

        std::cout << "vol error " << volume0 << " "  <<curVolume << std::endl;
        float x = (curVolume - volume0)/volume0;
        y_volume0 += x*DT;

        float kp = 2.3 / (25.0 * DT);
        float ki = kp*kp/16.0;
        vdiv = -(kp * x + ki * y_volume0) / (x + 1.0);
    }


    inline void enforce_boundary()
    {
        for(int i = 0; i < gx+1; i++)
        {
            for(int j = 0; j < gy; j++)
            {

            if( i==0 || i==gx )
            {
                ux->data[i][j] = 0.0;
            }else {
                if(block->data[i+1][j] > 0.5f)
                {
                    ux->data[i][j] = std::min(blocku->data[i+1][j],ux->data[i][j]);

                }
                if(block->data[i-1][j] > 0.5f)
                {
                    ux->data[i][j] = std::max(blocku->data[i-1][j],ux->data[i][j]);

                }
                if(block->data[i][j] > 0.5f)
                {
                    ux->data[i][j] = 0.0;
                }
            }

            }

        }

        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy+1; j++)
            {
            if( j==0 || j==gy)
            {
                uy->data[i][j] = 0.0;
            }else {
                if(block->data[i][j+1] > 0.5f)
                {
                    uy->data[i][j] = std::min(blockv->data[i][j+1],uy->data[i][j]);

                }
                if(block->data[i][j-1] > 0.5f)
                {
                    uy->data[i][j] = std::max(blockv->data[i][j-1],uy->data[i][j]);

                }
                if(block->data[i][j] > 0.5f)
                {
                    uy->data[i][j] = 0.0;
                }
            }
            }
        }
    }

    inline  void comp_divergence() {
        float hx = 1.0/gx;
        float hy = 1.0/gy;

        std::cout << "div " << vdiv << std::endl;
        // Swap
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                float div = A->data[i][j]<0.0 ? (ux->data[i+1][j]-ux->data[i][j])/hx + (uy->data[i][j+1]-uy->data[i][j])/hy : 0.0;
                d->data[i][j] = div - vdiv;
            }
        }
    }


    inline void compute_pressure() {
        // Clear Pressure
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                p->data[i][j] = 0.0;
            }
        }

        // Solve Ap = d ( p = Pressure, d = Divergence )
        solve( A, p, d, gx,gy, subcell, solver_mode );
    }

    inline void subtract_pressure() {
        float hx = 1.0/gx;
        float hy = 1.0/gy;
        for(int i = 0; i < gx+1; i++)
        {
            for(int j = 0; j < gy; j++)
            {

            if( i>0 && i<gx ) {
                float pf = p->data[i][j];
                float pb = p->data[i-1][j];
                if( subcell && A->data[i][j] * A->data[i-1][j] < 0.0 ) {
                    pf = A->data[i][j] < 0.0 ? p->data[i][j] : A->data[i][j]/fmin(1.0e-3,A->data[i-1][j])*p->data[i-1][j];
                    pb = A->data[i-1][j] < 0.0 ? p->data[i-1][j] : A->data[i-1][j]/fmin(1.0e-6,A->data[i][j])*p->data[i][j];
                }
                ux->data[i][j] -= (pf-pb)/hx;


            }
            }
        }

        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy+1; j++)
            {

            if( j>0 && j<gy ) {
                float pf = p->data[i][j];
                float pb = p->data[i][j-1];
                if( subcell && A->data[i][j] * A->data[i][j-1] < 0.0 ) {
                    pf = A->data[i][j] < 0.0 ? p->data[i][j] : A->data[i][j]/fmin(1.0e-3,A->data[i][j-1])*p->data[i][j-1];
                    pb = A->data[i][j-1] < 0.0 ? p->data[i][j-1] : A->data[i][j-1]/fmin(1.0e-6,A->data[i][j])*p->data[i][j];
                }
                uy->data[i][j] -= (pf-pb)/hy;
            }
            }
        }
    }
    inline void extrapolate( cTMap *_q, char **region ) {
        // Unknowns
        grid_queue unknowns;

        // Computed Field


        // Push

        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
            if( ! region[i][j] && grids[i][j].known ) {
                unknowns.push(&grids[i][j]);
            }
            computed[i][j] = region[i][j];
        }}

        // While Unknowns Exists
        while( ! unknowns.empty() ) {
            // Pop Out Top
            grid *mingrid = unknowns.top();
            int i = mingrid->gp[0];
            int j = mingrid->gp[1];

            int sum = 0;
            float sumq = 0.0;
            int query[][2] = { {i+1,j},{i-1,j},{i,j+1},{i,j-1},{i-1,j-1},{i-1,j+1},{i+1,j-1},{i+1,j+1} };
            for( int nq=0; nq<8; nq++ ) {
                int qi = query[nq][0];
                int qj = query[nq][1];
                if( qi>=0 && qi<gx && qj>=0 && qj<gy ) {
                    if( computed[qi][qj] ) {
                        sumq += _q->data[qi][qj];
                        sum ++;
                    }
                }
            }
            if( sum ) {
                if(block->data[i][j])
                {
                    _q->data[i][j] = 0.0;
                }else {
                    _q->data[i][j] = sumq / sum;
                }
            }
            unknowns.pop();
            computed[i][j] = 1;
        }
    }

    inline void extrapolateVelocity() {

        // Map To LevelSet (X Direction)
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
            if( i<gx-1 && A->data[i][j]<0.0 ) {
                region[i][j] = 1;
                q->data[i][j] = (ux->data[i][j]+ux->data[i+1][j])*0.5;
            }
            else {
                region[i][j] = 0;
                q->data[i][j] = 0.0;
            }
            }
        }

        // Extrapolate
        extrapolate( q, region );

        // Map Back (X Direction)
        for(int i = 0; i < gx+1; i++)
        {
            for(int j = 0; j < gy; j++)
            {
            if( i>0 && i<gx && (A->data[i][j]>0.0 || A->data[i-1][j]>0.0) ) ux->data[i][j] = (q->data[i][j]+q->data[i-1][j])*0.5;
        } }

        // Map To LevelSet (Y Direction)
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
            if( j<gy-1 && A->data[i][j]<0.0 ) {
                region[i][j] = 1;
                q->data[i][j] = (uy->data[i][j]+uy->data[i][j+1])*0.5;
            } else {
                region[i][j] = 0;
                q->data[i][j] = 0.0;
            }
            }
        }

        // Extrapolate
        extrapolate( q, region );

        // Map Back (Y Direction)
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy+1; j++)
            {
            if( j>0 && j<gy && (A->data[i][j]>0.0 || A->data[i][j-1]>0.0) ) uy->data[i][j] = (q->data[i][j]+q->data[i][j-1])*0.5;
        }}
    }





    char subcell_solver = 0;
    char solver_mode_solver = 0;

    // Clamped Fetch
    inline float x_ref( cTMap *_A, cTMap *_x, int fi, int fj, int i, int j, int nx, int ny ) {
        i = std::min(std::max(0,i),nx-1);
        j = std::min(std::max(0,j),ny-1);
        if(block->data[i][j] > 0.5f)
        {
            i = fi;
            j = fj;
            if( _A->data[i][j] < 0.0 ) return _x->data[i][j];
            return 0.0;
        }
        if( _A->data[i][j] < 0.0 ) return _x->data[i][j];
        return subcell_solver ? _A->data[i][j]/fmin(1.0e-6,_A->data[fi][fj])*_x->data[fi][fj] : 0.0;
    }

    // Ans = Ax
    inline void compute_Ax( cTMap *_A, cTMap *_x, cTMap *_ans, int nx, int ny ) {
        float hx2 = 1.0/(nx*nx);
        float hy2 = 1.0/(ny*ny);
        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
                if( _A->data[i][j] < 0.0 ) {
                    _ans->data[i][j] = (2.0*_x->data[i][j]-x_ref(_A,_x,i,j,i+1,j,nx,ny)-x_ref(_A,_x,i,j,i-1,j,nx,ny))/hx2;
                    _ans->data[i][j] += (2.0*_x->data[i][j]-x_ref(_A,_x,i,j,i,j+1,nx,ny)-x_ref(_A,_x,i,j,i,j-1,nx,ny))/hy2;
                } else {
                    _ans->data[i][j] = 0.0;
                }
            }

        }
    }

    // ans = x^T * x
    inline float product( cTMap *_A, cTMap *_x, cTMap *_y, int nx, int ny ) {
        float ans = 0.0;
        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
                if( _A->data[i][j] < 0.0 ) ans += _x->data[i][j]*_y->data[i][j];
            }
        }
        return ans;
    }

    // x = 0
    inline void clear( cTMap *_x, int nx, int ny ) {
        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
                _x->data[i][j] = 0.0;
            }
        }
    }

    inline void flip( cTMap *_x, int nx, int ny ) {
        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
                _x->data[i][j] = -_x->data[i][j];
            }
        }
    }

    // x <= y
    inline void copy( cTMap *_x, cTMap *_y, int nx, int ny ) {
        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
                _x->data[i][j] = _y->data[i][j];
            }
        }
    }

    // Ans = x + a*y
    inline void op( cTMap *_A, cTMap *_x, cTMap *_y, cTMap *_ans,float a, int nx, int ny ) {

        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
                if( _A->data[i][j] < 0.0 ) tmp->data[i][j] = _x->data[i][j]+a*_y->data[i][j];
            }
        }
        copy(_ans,tmp,nx,ny);
    }

    // r = b - Ax
    inline void residual( cTMap *_A, cTMap *_x, cTMap *_b, cTMap *_r, int nx, int ny ) {
        compute_Ax(_A,_x,_r,nx,ny);
        op( _A, _b, _r, _r, -1.0, nx,ny );
    }

    inline float square( float a ) {
        return a*a;
    }

    inline float A_ref( cTMap *_A, int i, int j, int qi, int qj, int nx, int ny ) {
        if( i<0 || i>nx-1 || j<0 || j>ny-1 || _A->data[i][j]>0.0 || block->data[i][j] > 0.5f) return 0.0;
        if( qi<0 || qi>nx-1 || qj<0 || qj>ny-1 || _A->data[qi][qj]>0.0 || block->data[qi][qj] > 0.5f) return 0.0;
        return -1.0;
    }

    inline float A_diag( cTMap *_A, int i, int j, int nx, int ny ) {
        float diag = 4.0;
        if( _A->data[i][j] > 0.0 ) return diag;
        int q3[][2] = { {i-1,j}, {i+1,j}, {i,j-1}, {i,j+1} };
        for( int m=0; m<4; m++ ) {
            int qi = q3[m][0];
            int qj = q3[m][1];
            if( qi<0 || qi>nx-1 || qj<0 || qj>ny-1 || block->data[qi][qj] > 0.5f) diag -= 1.0;
            else if( _A->data[qi][qj] > 0.0 && subcell ) {
                diag -= _A->data[qi][qj]/fmin(1.0e-6,_A->data[i][j]);
            }
        }
        return diag;
    }

    inline float P_ref( cTMap *_P, int i, int j, int nx,int ny ) {
        if( i<0 || i>nx-1 || j<0 || j>ny-1 ) return 0.0;
        if(block->data[i][j] > 0.5f)
        {
            return 0.0;
        }
        return _P->data[i][j];
    }



    inline void buildPreconditioner( cTMap *_P, cTMap *_A, int nx, float ny ) {
        clear(_P,nx,ny);
        float t = solver_mode == 2 ? 0.97 : 0.0;
        float a = 0.25;
        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
                if(_A->data[i][j] < 0.0 ) {
                    float left = A_ref(_A,i-1,j,i,j,nx,ny)*P_ref(_P,i-1,j,nx,ny);
                    float bottom = A_ref(A,i,j-1,i,j,nx,ny)*P_ref(_P,i,j-1,nx,ny);
                    float mleft = A_ref(_A,i-1,j,i,j,nx,ny)*A_ref(_A,i,j-1,i,j,nx,ny)*square(P_ref(_P,i-1,j,nx,ny));
                    float mbottom = A_ref(_A,i,j-1,i,j,nx,ny)*A_ref(_A,i-1,j,i,j,nx,ny)*square(P_ref(_P,i,j-1,nx,ny));

                    float diag = A_diag( _A, i, j, nx,ny );
                    float e = diag - square(left) - square(bottom) - t*( mleft + mbottom );
                    if( e < a*diag ) e = diag;
                    _P->data[i][j] = 1.0/sqrtf(e);
                    //std::cout << _P->data[i][j] << " " << e  << "  " << diag << " " << t <<  " " << left << " " << std::endl;

                }
            }
        }
    }

    inline void applyPreconditioner(  cTMap *_z,  cTMap *_r,  cTMap *_P, cTMap *_A, int nx, int ny ) {
        if( solver_mode_solver == 0 ) {
            copy(_z,_r,nx,ny);
            return;
        }

        clear(q2,nx,ny);

        // Lq = r
        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
                if( _A->data[i][j] < 0.0 ) {
                    float left = A_ref(_A,i-1,j,i,j,nx,ny)*P_ref(_P,i-1,j,nx,ny)*P_ref(q2,i-1,j,nx,ny);
                    float bottom = A_ref(_A,i,j-1,i,j,nx,ny)*P_ref(_P,i,j-1,nx,ny)*P_ref(q2,i,j-1,nx,ny);

                    float t = _r->data[i][j] - left - bottom;
                    q2->data[i][j] = t*_P->data[i][j];

                }
            }
        }

        // L^T z = q
        for( int i=nx-1; i>=0; i-- ) {
            for( int j=ny-1; j>=0; j-- ) {
                if( _A->data[i][j] < 0.0 ) {
                    float right = A_ref(_A,i,j,i+1,j,nx,ny)*P_ref(_P,i,j,nx,ny)*P_ref(_z,i+1,j,nx,ny);
                    float top = A_ref(_A,i,j,i,j+1,nx,ny)*P_ref(_P,i,j,nx,ny)*P_ref(_z,i,j+1,nx,ny);

                    float t = q2->data[i][j] - right - top;
                    _z->data[i][j] = t*_P->data[i][j];
                }
            }
        }
    }

    inline void conjGrad( cTMap *_A, cTMap *_P, cTMap *_x, cTMap * _b, int nx,int ny ) {
        // Pre-allocate Memory

        std::cout << "start iteration 2d "<< std::endl;



        clear(_x,nx,ny);									// p = 0
        copy(r,_b,nx,ny);								// r = b
        applyPreconditioner(z,r,_P,_A,nx,ny);				// Apply Conditioner z = f(r)
        copy(s,z,nx,ny);								// s = z

        float a = product( _A, z, r, nx,ny );			// a = z . r
        for( int k=0; k<nx*ny; k++ ) {

            compute_Ax( _A, s, z, nx,ny );				// z = applyA(s)
            float alpha = a/product( _A, z, s, nx,ny );	// alpha = a/(z . s)
            op( _A, _x, s, _x, alpha, nx,ny );				// p = p + alpha*s
            op( _A, r, z, r, -alpha, nx,ny );			// r = r - alpha*z;
            float error2 = product( _A, r, r, nx,ny );	// error2 = r . r

            std::cout << "iter 2d " << error2/(nx*ny) << std::endl;

            if( error2/(nx*ny) < 1.0e-6 ) break;
            applyPreconditioner(z,r,_P,_A,nx,ny);			// Apply Conditioner z = f(r)
            float a2 = product( _A, z, r, nx,ny );		// a2 = z . r
            float beta = a2/a;
            op( _A, z, s, s, beta, nx,ny );				// s = z + beta*s
            a = a2;
        }
    }

    inline float solve( cTMap *_A, cTMap * _x, cTMap *_b, int nx,int ny, char subcell_aware, char solver_type ) {

        clear(r2,nx,ny);

        // Save Mode
        subcell_solver = subcell_aware;
        solver_mode_solver = solver_type;

        // Flip Divergence
        flip(_b,nx,ny);

        // Build Modified Incomplete Cholesky Precondioner Matrix
        if( solver_mode >= 1 ) buildPreconditioner(P,_A,nx,ny);

        // Conjugate Gradient Method
        conjGrad(_A,P,_x,_b,nx,ny);


        residual(_A,_x,_b,r2,nx,ny);
        float res = std::sqrt(product( _A, r2, r2, nx,ny ))/(nx*ny);
        // printf( "Residual = %e\n", res );
        return res;
    }


    cTMap *gux = NULL;
    cTMap *guy = NULL;

    // Clamped Fluid Flow Fetch
    inline float u_ref( int dir, int i, int j ) {
        if( dir == 0 )
            return gux->data[std::max(0,std::min(gx,i))][std::max(0,std::min(gy-1,j))];
        else
            return guy->data[std::max(0,std::min(gx-1,i))][std::max(0,std::min(gy,j))];
    }

    inline void semiLagrangian( cTMap *_d, cTMap*_d0, int width, int height, cTMap* u1, cTMap * u2 ) {
        for( int n=0; n<width*height; n++ ) {
            int i = n%width;
            int j = n/width;
            _d->data[i][j] = interp( _d0, i-gx*u1->data[i][j]*DT, j-gy*u2->data[i][j]*DT, width, height );
        }
    }

    // Semi-Lagrangian Advection Method
    inline  void advect_semiLagrangian( cTMap * _ux, cTMap * _uy, cTMap * _outx, cTMap * _outy ) {
        gux = _ux;
        guy = _uy;

        int nx = gx;
        int ny = gy;
        for( int i=0; i<nx+1; i++ ) {
            for( int j=0; j<ny; j++ ) {
            ux1->data[i][j] = _ux->data[i][j];
            ux2->data[i][j] = (u_ref(1,i-1,j)+u_ref(1,i,j)+u_ref(1,i-1,j+1)+u_ref(1,i,j+1))/4.0;
        }}

        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny+1; j++ ) {
            uy1->data[i][j] = (u_ref(0,i,j-1)+u_ref(0,i,j)+u_ref(0,i+1,j)+u_ref(0,i+1,j-1))/4.0;
            uy2->data[i][j] = _uy->data[i][j];
        }}

        // BackTrace X Flow
        semiLagrangian( _outx, _ux, gx+1, gy, ux1,ux2 );

        // BackTrace Y Flow
        semiLagrangian( _outy, _uy, gx, gy+1, uy1,uy2 );
    }

    inline void advect_fluid() {

        advect_semiLagrangian( ux,uy, ux_swap, uy_swap );

        int nx = gx;
        int ny = gy;

        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
            ux->data[i][j] = ux_swap->data[i][j];
        }}
        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
            uy->data[i][j] = uy_swap->data[i][j];
        }}
    }

    inline void setMaxDistOfLevelSet() {
    #if 0
        LSMFLOAT max_vel = 0.0;
        FOR_EVERY_CELL(gn) {
            LSMFLOAT xv = (u[0][i][j]+u[0][i+1][j])*0.5;
            LSMFLOAT xu = (u[1][i][j]+u[1][i+1][j])*0.5;
            LSMFLOAT vel = hypotf(xv,xu);
            if( vel > max_vel ) max_vel = vel;
        } END_FOR;
        maxdist = fmax(DIST, 1.5*DT*max_vel);
    #endif
    }



    inline void TimeStep(float dt)
    {
        // Mark Liquid Domain
            markLiquid();

            float av = 0.0;

            // Visualize Everything
            //render();

            // Add Gravity Force
            addGravity();

            // Compute Volume Error
            computeVolumeError();

            // Solve Fluid
            enforce_boundary();

            comp_divergence();

            compute_pressure();

            subtract_pressure();

            enforce_boundary();

            // Extrapolate Quantity
            extrapolateVelocity();

            // Advect Flow
            advect_fluid();

            // Advect
            advect();

            // Redistancing
            if(do_redistance) {
                static int wait=0;
                if(wait++%REDIST==0) {
                    setMaxDistOfLevelSet();
                    redistance(maxdist);
                }
            }


    }

};

#endif // FLUID2P2D_H

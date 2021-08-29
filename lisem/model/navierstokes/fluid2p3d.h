#ifndef FLUID2P3D_H
#define FLUID2P3D_H


#include "geo/raster/map.h"
#include "QList"
#include <vector>
#include <queue>
#include "linear/lsm_vector3.h"

#define	GRAVITY		9.8
#define	DT			0.015
#define DIST		(0.1)
#define REDIST		1




template <class T> T *** alloc3D( int nx, int ny , int nz) {
    T ***ptr = new T **[nx+1];
        for( int i=0; i<nx; i++ ) {
            ptr[i] = new T *[ny+1];
            for(int j=0; j<ny; j++){
                ptr[i][j] = new T[nz+1];
            }
            ptr[i][ny] = 0;
        }
        ptr[nx] = 0;
        return ptr;

}



template <class T> void free3D( T **ptr ) {
    for( int i=0; ptr[i]; i++ ) {
            for(int j=0; ptr[i][j]; j++) delete [] ptr[i][j];
            delete [] ptr[i];
        }
        delete [] ptr;
}



typedef struct cTMap3D
{

    int nx = 0;
    int ny = 0;
    int nz = 0;

    float***data;

    inline cTMap3D(int x, int y, int z)
    {
        nx = x;
        ny = y;
        nz = z;
        data = alloc3D<float>(x,y,z);

        for(int i = 0; i < nx; i++)
        {
            for(int j = 0; j < ny; j++)
            {
                for(int k = 0; k < nz;k++)
                {
                    data[i][j][k] = 0.0;
                }


            }
        }
    }

    inline double GetAverage()
    {

        double n = 0.0;
        double valt = 0.0;
        for(int i = 0; i < nx; i++)
        {
            for(int j = 0; j < ny; j++)
            {
                for(int k = 0; k < nz;k++)
                {

                    if(!pcr::isMV(data[i][j][k]))
                    {
                        n += 1.0;
                        valt += data[i][j][k];
                    }
                }


            }
        }

        if(n > 0.0)
        {
            valt = valt / n;
        }

        return valt;
    }

    inline void setAllMV()
    {
        for(int i = 0; i < nx; i++)
        {
            for(int j = 0; j < ny; j++)
            {
                for(int k = 0; k < nz;k++)
                {
                    pcr::setMV(data[i][j][k]);
                }


            }
        }

    }

    inline ~cTMap3D()
    {
        free3D(data);
    }
} cTMap3D;

static inline float hypot3( float a, float b, float c ) {
    return std::sqrt(a*a+b*b+c*c);
}

static void intersect( float x1, float y1, float z1, float x2, float y2,float z2, float x3, float y3, float z3, float &x, float &y, float &z  ) {

    //get plane normal
    LSMVector3 normal  = LSMVector3::CrossProduct(LSMVector3(x1,y1,z1) - LSMVector3(x2,y2,z2),LSMVector3(x3,y3,z3) - LSMVector3(x2,y2,z2)).Normalize();

    LSMVector3 intersect =LSMVector3(x,y,z) -  normal.dot(LSMVector3(x,y,z) - LSMVector3(x1,y1,y2)) * normal;
    //dot product of plane normal and vector from point on plane to point

    x = intersect.x;
    y = intersect.y;
    z = intersect.z;
}

static void intersect2( float x1, float y1, float z1,float x2, float y2,float z2, float &x, float &y , float &z ) {
    float d = hypot3(x2-x1,y2-y1,z2-z1);
    float u = ((x-x1)*(x2-x1) + (y-y1)*(y2-y1) + (z-z1)*(z2-z1))/d;
    u = fmin(1.0,fmax(0.0,u));
    x = x1 + u*(x2-x1);
    y = y1 + u*(y2-y1);
    z = z1 + u*(z2-z1);
}



class Fluid2P3D
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
    int gz = 0;
    int method = 1;

    double m_DT = DT;

    cTMap3D * ux = NULL;		// Access Bracket u[DIM][X][Y] ( Staggered Grid )
    cTMap3D * uy = NULL;
    cTMap3D * uz = NULL;
    cTMap3D * p = NULL;		// Equivalent to p[N][N]
    cTMap3D * d = NULL;		// Equivalent to d[N][N]
    cTMap3D * A = NULL;		// Level Set Field
    cTMap3D * source_dists = NULL;
    cTMap3D * swap_dists = NULL;
    cTMap3D * tmp = NULL;

    cTMap3D * block = NULL;
    cTMap3D * blocku = NULL;
    cTMap3D * blockv = NULL;
    cTMap3D * blockw = NULL;

    char ***computed;
    char ***region;
    cTMap3D *q;
    cTMap3D *q2;
    typedef struct {
            char known;
            char estimated;
            float dist;
            float pos[3];
            int gp[3];
        } grid;

    grid *** grids = NULL;
    cTMap3D * gradx = NULL;
    cTMap3D * grady = NULL;
    cTMap3D * gradz = NULL;
    cTMap3D * test_q = NULL;
    cTMap3D *r = NULL;
    cTMap3D *P = NULL;
    cTMap3D *r2 = NULL;
    cTMap3D *z = NULL;
    cTMap3D *s = NULL;
    cTMap3D *ux1 = NULL;
    cTMap3D *ux2 = NULL;
    cTMap3D *ux3 = NULL;
    cTMap3D *uy1 = NULL;
    cTMap3D *uy2 = NULL;
    cTMap3D *uy3 = NULL;
    cTMap3D *uz1 = NULL;
    cTMap3D *uz2 = NULL;
    cTMap3D *uz3 = NULL;
    cTMap3D *ux_swap = NULL;
    cTMap3D *uy_swap = NULL;
    cTMap3D *uz_swap = NULL;

    struct gridComparison{
        bool operator () ( grid * left, grid * right){
            return fabs(left->dist) > fabs(right->dist);
        }
    };
    typedef std::priority_queue<grid *,std::vector<grid*>,gridComparison> grid_queue;


    float m_sx = 1.0;
    float m_sy = 1.0;
    float m_sz = 1.0;

    float m_sxr = 1.0;
    float m_syr = 1.0;
    float m_szr = 1.0;

    float m_scx = 1.0;
    float m_scy = 1.0;
    float m_scz = 1.0;


    float dx = 1.0;
    float dy = 1.0;

    float g = 9.81;
    float visc = 0.0;
    float b0 = 1.7;
    float e = 0.0001;

private:

    inline cTMap3D * CreateMap(int rows, int cols, int levels, bool setmv = false)
    {
        cTMap3D * m;
        m = new cTMap3D(cols,rows,levels);
        if(setmv)
        {
            m->setAllMV();
        }
        return m;
    }

public:
    inline Fluid2P3D(int rows, int cols, int levels, float _dx, float _dy, float _dz)
    {
        gx = cols;
        gy = rows;
        gz = levels;
        ux = CreateMap(rows+2,cols+2,levels +2);		// Access Bracket u[DIM][X][Y] ( Staggered Grid )
        uy = CreateMap(rows+2,cols+2,levels +2);
        uz = CreateMap(rows+2,cols+2,levels +2);
        p = CreateMap(rows+1,cols+1,levels +1);		// Equivalent to p[N][N]
        d = CreateMap(rows+1,cols+1,levels +1);		// Equivalent to d[N][N]
        A = CreateMap(rows+1,cols+1,levels +1);
        source_dists = CreateMap(rows+1,cols+1,levels +1);
        swap_dists = CreateMap(rows+1,cols+1,levels +1);
        region = alloc3D<char>(gx,gy,gz);
        q = CreateMap(rows+1,cols+1,levels +1);
        computed = alloc3D<char>(gx,gy,gz);
        r = CreateMap(rows+1,cols+1,levels +1);
        p = CreateMap(rows+1,cols+1,levels +1);
        r2 = CreateMap(rows+1,cols+1,levels +1);
        z = CreateMap(rows+1,cols+1,levels +1);
        s = CreateMap(rows+1,cols+1,levels +1);
        q2 = CreateMap(rows+1,cols+1,levels +1);
        tmp = CreateMap(rows+1,cols+1,levels +1);
        P = CreateMap(rows+1,cols+1,levels +1);

        ux1 = CreateMap(rows+1,cols+1,levels +1);
        ux2 = CreateMap(rows+1,cols+1,levels +1);
        ux3 = CreateMap(rows+1,cols+1,levels +1);
        uy1 = CreateMap(rows+1,cols+1,levels +1);
        uy2 = CreateMap(rows+1,cols+1,levels +1);
        uy3 = CreateMap(rows+1,cols+1,levels +1);
        uz1 = CreateMap(rows+1,cols+1,levels +1);
        uz2 = CreateMap(rows+1,cols+1,levels +1);
        uz3 = CreateMap(rows+1,cols+1,levels +1);
        ux_swap = CreateMap(rows+1,cols+1,levels +1);
        uy_swap = CreateMap(rows+1,cols+1,levels +1);
        uz_swap = CreateMap(rows+1,cols+1,levels +1);


        block = CreateMap(rows+1,cols+1,levels +1);
        blocku = CreateMap(rows+1,cols+1,levels +1);
        blockv = CreateMap(rows+1,cols+1,levels +1);
        blockw = CreateMap(rows+1,cols+1,levels +1);

        InitLevelSet(rows,cols,levels);
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

    inline ~Fluid2P3D()
    {


    }

    inline void SetSizes(float sx, float sy, float sz)
    {
        m_sxr = std::fabs(sx);
        m_syr = std::fabs(sy);
        m_szr = std::fabs(sz);
    }

    inline void InitializeFromData(std::vector<cTMap *> FH, std::vector<cTMap *> FUx, std::vector<cTMap *> FUy, std::vector<cTMap *> FUz,std::vector<cTMap *> Fp, std::vector<cTMap *> LevelSet, std::vector<cTMap *> Block, std::vector<cTMap *> Blocku, std::vector<cTMap *> Blockv, std::vector<cTMap *> Blockw)
    {

        bool has_levelset = false;

        // Initialize Distances
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz; k++)
                {
                    if(!pcr::isMV(LevelSet.at(k)->data[j][i]))
                    {
                        has_levelset = true;
                    }
                }
            }
        }

        m_sx = 1.0/((float)(gx));
        m_sy = 1.0/((float)(gy));
        m_sz = 1.0/((float)(gz));

        int nf = 0;

        // Initialize Distances
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz; k++)
                {
                    grids[i][j][k].known = true;
                    if(has_levelset)
                    {
                        if(pcr::isMV(LevelSet.at(k)->data[j][i]))
                        {
                            grids[i][j][k].known = false;
                            grids[i][j][k].dist = 0.0;

                        }else {
                            grids[i][j][k].dist = LevelSet.at(k)->data[j][i];
                        }
                    }else {
                        grids[i][j][k].dist = FH.at(k)->data[j][i] > 0.5f? -1.0:1.0;

                        if(FH.at(k)->data[j][i])
                        {
                            nf ++;
                        }
                    }
                    grids[i][j][k].pos[0] = 0.0;
                    grids[i][j][k].pos[1] = 0.0;
                    grids[i][j][k].pos[2] = 0.0;
                    p->data[i][j][k] = Fp.at(k)->data[j][i];
                    ux->data[i][j][k] = FUx.at(k)->data[j][i];
                    uy->data[i][j][k] = FUy.at(k)->data[j][i];
                    uz->data[i][j][k] = FUz.at(k)->data[j][i];

                    block->data[i][j][k] = Block.at(k)->data[j][i];
                    blocku->data[i][j][k] = Blocku.at(k)->data[j][i];
                    blockv->data[i][j][k] = Blockv.at(k)->data[j][i];
                    blockw->data[i][j][k] = Blockw.at(k)->data[j][i];
                }
            }
        }

        std::cout << "has levelset ? " << has_levelset << " " << nf << std::endl;
        // Redistance...
        if(!has_levelset)
        {
            redistance(maxdist);
        }

        //volume0 = getVolume();
        //y_volume0 = 0.0;

    }

    inline void StoreToData(std::vector<cTMap *> FH, std::vector<cTMap *> FUx, std::vector<cTMap *> FUy,std::vector<cTMap *> FUz,  std::vector<cTMap *> Fp, std::vector<cTMap *> LevelSet)
    {
        for(int k = 0; k < gz; k++)
        {
            LevelSet.at(k)->setAllMV();
        }
        // Initialize Distances
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz; k++)
                {
                    FUx.at(k)->data[j][i] = ux->data[i][j][k];
                    FUy.at(k)->data[j][i] = uy->data[i][j][k];
                    FUz.at(k)->data[j][i] = uz->data[i][j][k];
                    Fp.at(k)->data[j][i] = p->data[i][j][k];
                    if(grids[i][j][k].known)
                    {
                        LevelSet.at(k)->data[j][i] = grids[i][j][k].dist;
                        if(grids[i][j][k].dist < 0)
                        {
                            FH.at(k)->data[j][i] = 1.0;
                        }else {
                            FH.at(k)->data[j][i] = 0.0;
                        }

                    }else {
                        FH.at(k)->data[j][i] = 0.0;
                    }
                }
            }
        }
    }

    inline void calcMarchingPoints( int i, int j, int k, float p[16][3], int &pnum ) {
        pnum = 0;
        float wx = 1.0/(gx-1);
        float wy = 1.0/(gy-1);
        float wz = 1.0/(gz-1);
        int quads[][3] = { {i, j,k}, {i+1, j,k}, {i+1, j+1,k}, {i, j+1,k},{i, j,k+1}, {i+1, j,k+1}, {i+1, j+1,k+1}, {i, j+1,k+1} };
        for( int n=0; n<8; n++ ) {
            if( grids[quads[n][0]][quads[n][1]][quads[n][2]].known ) {
                // Inside Liquid
                if( grids[quads[n][0]][quads[n][1]][quads[n][2]].dist < 0.0 ) {
                    p[pnum][0] = wx*quads[n][0];
                    p[pnum][1] = wy*quads[n][1];
                    p[pnum][2] = wz*quads[n][2];
                    pnum ++;
                }
                // If Line Crossed
                if( grids[quads[n][0]][quads[n][1]][quads[n][2]].dist * grids[quads[(n+1)%8][0]][quads[(n+1)%8][1]][quads[(n+1)%8][2]].dist < 0 ) {
                    // Calculate Cross Position
                    float y0 = grids[quads[n][0]][quads[n][1]][quads[n][2]].dist;
                    float y1 = grids[quads[(n+1)%8][0]][quads[(n+1)%8][1]][quads[(n+1)%8][2]].dist;
                    float a = y0/(y0-y1);
                    float p0[3] = { wx*quads[n][0], wy*quads[n][1],wz*quads[n][2] };
                    float p1[3] = { wx*quads[(n+1)%8][0], wy*quads[(n+1)%8][1],wz*quads[(n+1)%8][2] };
                    p[pnum][0] = (1.0-a)*p0[0]+a*p1[0];
                    p[pnum][1] = (1.0-a)*p0[1]+a*p1[1];
                    p[pnum][2] = (1.0-a)*p0[2]+a*p1[2];
                    pnum ++;
                }
            }
        }
    }

    inline void flow( float x, float y,  float z, float &uu, float &vv, float  &ww, float &dt ) {
        x = (gx-1)*fmin(1.0,fmax(0.0,x))+0.5;
        y = (gy-1)*fmin(1.0,fmax(0.0,y))+0.5;
        z = (gz-1)*fmin(1.0,fmax(0.0,z))+0.5;
        int i = x;
        int j = y;
        int k = z;
        uu = (1.0-(x-i))*ux->data[i][j][k] + (x-i)*ux->data[i+1][j][k];
        vv = (1.0-(y-j))*uy->data[i][j][k] + (y-j)*uy->data[i][j+1][k];
        ww = (1.0-(z-k))*uz->data[i][j][k] + (z-k)*uz->data[i][j][k+1];
        dt = m_DT;
    }
    inline float getVolume() {
        float volume = 0.0;
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz; k++)
                {
                    float p[16][3];
                    int pnum;
                    calcMarchingPoints( i, j, k, p, pnum );
                    for( int m=0; m<pnum; m++ )
                    {
                        volume += p[m][0]*p[(m+1)%pnum][1]-p[m][1]*p[(m+1)%pnum][0];
                    }
                }
            }
        }

        std::cout << "volume calc " << volume * 0.5 << std::endl;

        return volume*0.5;
    }


    inline void addGravity() {

        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy+1; j++)
            {
                for(int k = 0; k < gz; k++)
                {
                    if( j>0 && j<gy-1 && (A->data[i][j][k]<0.0 || A->data[i][j-1][k]<0.0))
                    {
                        uy->data[i][j][k] += 0.0;
                    }
                    else
                    {
                        uy->data[i][j][k] = 0.0;
                    }
                }
            }
        }
        for(int i = 0; i < gx+1; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz; k++)
                {
                    if( i>0 && i<gx-1 && (A->data[i][j][k]<0.0 || A->data[i-1][j][k]<0.0))
                    {
                        ux->data[i][j][k] += 0.0;
                    }
                    else
                    {
                        ux->data[i][j][k] = 0.0;
                    }
                }
            }
        }

        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz+1; k++)
                {
                    if( k>0 && k<gz-1 && (A->data[i][j][k]<0.0 || A->data[i][j][k-1]<0.0))
                    {
                        uz->data[i][j][k] -= m_DT * GRAVITY;
                    }
                    else
                    {
                        uz->data[i][j][k] = 0.0;
                    }
                }
            }
        }
    }

    inline void InitLevelSet(int rows, int cols,int levels)
    {
        grids = alloc3D<grid>(cols+1,rows+1,levels+1);
        gradx = CreateMap(rows+1,cols+1,levels+1);
        grady = CreateMap(rows+1,cols+1,levels+1);
        gradz = CreateMap(rows+1,cols+1,levels+1);
        test_q = CreateMap(rows+1,cols+1,levels+1);
       for(int i = 0; i < gx; i++)
       {
           for(int j = 0; j < gy; j++)
           {
               for(int k = 0; k < gz; k++)
               {

                    grids[i][j][k].dist = 1.0;
                    grids[i][j][k].known = false;
                    grids[i][j][k].gp[0] = i;
                    grids[i][j][k].gp[1] = j;
                    grids[i][j][k].gp[2] = k;
                    gradx->data[i][j][k] = 0.0f;
                    grady->data[i][j][k] = 0.0;
                    gradz->data[i][j][k] = 0.0;
               }
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
                for(int k = 0; k < gz; k++)
                {
                    float x = i/(float)(gx-1);
                    float y = j/(float)(gy-1);
                    float z = k/(float)(gz-1);
                    float sx = x;
                    float sy = y;
                    float sz = z;
                    bool hit;
                    hit = hypot3(x-0.5,y-0.8,z - 0.8) < 0.15 || y < 0.2;
                    grids[i][j][k].known = true;
                    grids[i][j][k].dist = hit ? -1.0 : 1.0;
                    grids[i][j][k].pos[0] = 0.0;
                    grids[i][j][k].pos[1] = 0.0;
                    grids[i][j][k].pos[2] = 0.0;
                }
            }
        }

        // Redistance...
        redistance(tolerance);
    }


    // Helper Function Used In FastMarch()
    inline bool update_distance( int i, int j, int k, float pos[3], float &dist ) {
        float x = i/(float)(gx-1);
        float y = j/(float)(gy-1);
        float z = k/(float)(gz-1);
        dist = 1.0;
        bool updated = false;

        // For Neigboring Grids
        int query[][3] = { {i+1,j,k-1},{i-1,j,k-1},{i,j+1,k-1},{i,j-1,k-1},
                           {i-1,j-1,k-1},{i-1,j+1,k-1},{i+1,j-1,k-1},{i+1,j+1,k-1},
                           {i+1,j,k},{i-1,j,k},{i,j+1,k},{i,j-1,k},
                           {i-1,j-1,k},{i-1,j+1,k},{i+1,j-1,k},{i+1,j+1,k},
                           {i+1,j,k+1},{i-1,j,k+1},{i,j+1,k+1},{i,j-1,k+1},
                           {i-1,j-1,k+1},{i-1,j+1,k+1},{i+1,j-1,k+1},{i+1,j+1,k+1},
                           {i,j,k+1},{i,j,k-1}};

        for( int q=0; q<26; q++ ) {
            int qi = query[q][0];
            int qj = query[q][1];
            int qk = query[q][2];
            if( qi>=0 && qi<gx && qj>=0 && qj<gy && qk>=0 && qk<gz && block->data[qi][qj][qk] < 0.5f ) {
                float sgn = grids[qi][qj][qk].dist > 0 ? 1.0 : -1.0;
                // If The Neighborhood Is A Known Grid
                if( grids[qi][qj][qk].known ) {
                    // Compute The Distance From The Point Of Its Neighbors
                    float d = hypot3(grids[qi][qj][qk].pos[0]-x,grids[qi][qj][qk].pos[1]-y,grids[qi][qj][qk].pos[2]-z);
                    // If The Distance Is Closer Than Holding One, Just Replace
                    if( d < fabs(dist) ) {
                        dist = sgn*d;
                        for( int n=0; n<3; n++ ) pos[n] = grids[qi][qj][qk].pos[n];
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

        {
            int n = 0;
            int n2 = 0;
            int n3 = 0;
            for(int i = 0; i < gx; i++)
            {
                for(int j = 0; j < gy; j++)
                {
                    for(int k = 0; k < gz; k++)
                    {
                        if(grids[i][j][k].known)
                        {
                            n++;
                        }

                        if(grids[i][j][k].known && block->data[i][j][k] > 0.5f)
                        {
                            n2 ++;
                            grids[i][j][k].dist = 1.0;

                        }
                        if(!grids[i][j][k].known && block->data[i][j][k] > 0.5f)
                        {
                            n3 ++;
                            grids[i][j][k].dist = 1.0;
                            grids[i][j][k].known = true;
                        }
                    }
                }
            }

            std::cout << "n flow5a " << n << " " << n2 << " " << n3 << std::endl;
        }


        // Compute First Estimate of Distances
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz; k++)
                {
                    float pos[3];
                    float dist;
                    if( ! grids[i][j][k].known ) {
                        grids[i][j][k].estimated = false;
                        if( update_distance(i,j,k,pos,dist)) {
                            grids[i][j][k].dist = dist;
                            if( fabs(dist) < tolerance ) {
                                grids[i][j][k].pos[0] = pos[0];
                                grids[i][j][k].pos[1] = pos[1];
                                grids[i][j][k].pos[2] = pos[2];
                                grids[i][j][k].estimated = true;
                                unknowns.push(&grids[i][j][k]);
                            }
                        } else {
                            grids[i][j][k].dist = 1.0;
                        }
                    }
                }
            }
        }


        int nupd = 0;
        int nins = 0;

        // While Unknown Grid Exists
        while( ! unknowns.empty() ) {
            // Pop Out Top
            grid *mingrid = unknowns.top();
            int i = mingrid->gp[0];
            int j = mingrid->gp[1];
            int k = mingrid->gp[2];
            unknowns.pop();
            grids[i][j][k].estimated = false;
            mingrid->known = true;

            // Dilate...
            int query[][3] = { {i+1,j,k-1},{i-1,j,k-1},{i,j+1,k-1},{i,j-1,k-1},
                               {i-1,j-1,k-1},{i-1,j+1,k-1},{i+1,j-1,k-1},{i+1,j+1,k-1},
                               {i+1,j,k},{i-1,j,k},{i,j+1,k},{i,j-1,k},
                               {i-1,j-1,k},{i-1,j+1,k},{i+1,j-1,k},{i+1,j+1,k},
                               {i+1,j,k+1},{i-1,j,k+1},{i,j+1,k+1},{i,j-1,k+1},
                               {i-1,j-1,k+1},{i-1,j+1,k+1},{i+1,j-1,k+1},{i+1,j+1,k+1},
                               {i,j,k+1},{i,j,k-1}};
            for( int q=0; q<26; q++ ) {
                int qi = query[q][0];
                int qj = query[q][1];
                int qk = query[q][2];
                if( qi>=0 && qi<gx && qj>=0 && qj<gy && qk>=0 && qk<gz && block->data[qi][qj][qk] < 0.5) {
                    if( ! grids[qi][qj][qk].estimated && ! grids[qi][qj][qk].known ) {
                        float pos[3];
                        float dist;
                        if( update_distance(qi,qj,qk,pos,dist)) {

                            if( fabs(dist) < tolerance ) {
                                nupd ++;
                                grids[qi][qj][qk].dist = dist;
                                grids[qi][qj][qk].pos[0] = pos[0];
                                grids[qi][qj][qk].pos[1] = pos[1];
                                grids[qi][qj][qk].pos[2] = pos[2];
                                grids[qi][qj][qk].estimated = true;
                                unknowns.push(&grids[qi][qj][qk]);
                            }else
                            {
                                nins ++;
                            }
                        }
                    }
                }
            }
        }

        {
            int n = 0;
            for(int i = 0; i < gx; i++)
            {
                for(int j = 0; j < gy; j++)
                {
                    for(int k = 0; k < gz; k++)
                    {
                        if(!std::isfinite(grids[i][j][k].dist))
                        {
                            n++;
                        }

                    }
                }
            }

            std::cout << "n flow5b " << n <<  " "  << nupd << " " << nins <<  std::endl;
        }


    #if 1 // TODO check this code for gx an gy limits
        // Fill Inner Region With Scanline Order

        int dir = 0;
            for( int j=0; j<gy; j++ ) {
                 for( int k=0; k<gz; k++ ) {
                     float sgn = 1.0;

                    // Forward Search
                    for( int i=0; i<gx; i++ ) {

                        int idx[][3] = { {i,j,k} };

                        if( grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].known ) {
                            sgn = grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].dist < 0.0 ? -1.0 : 1.0;
                        } else if( sgn < 0.0 ) {
                            grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].dist = -1.0;
                            grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].known = true;
                        }
                    }



                    // Backward Search
                    for( int i=gx-1; i>=0; i-- ) {
                        int idx[][3] = { {i,j,k}};

                        if( grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].known ) {
                            sgn = grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].dist < 0.0 ? -1.0 : 1.0;
                        } else if( sgn < 0.0 ) {
                            grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].dist = -1.0;
                            grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].known = true;
                        }
                    }
                }
            }

            for( int j=0; j<gx; j++ ) {
                for( int k=0; k<gz; k++ ) {

                float sgn = 1.0;

                // Forward Search
                for( int i=0; i<gy; i++ ) {
                    int idx[][3] = { {j,i,k} };

                    if( grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].known ) {
                        sgn = grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].dist < 0.0 ? -1.0 : 1.0;
                    } else if( sgn < 0.0 ) {
                        grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].dist = -1.0;
                        grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].known = true;
                    }
                }

                // Backward Search
                for( int i=gy-1; i>=0; i-- ) {
                    int idx[][3] = { {j,i,k}};

                    if( grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].known ) {
                        sgn = grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].dist < 0.0 ? -1.0 : 1.0;
                    } else if( sgn < 0.0 ) {
                        grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].dist = -1.0;
                        grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].known = true;
                    }
                }

            }
        }

            for( int i=0; i<gx; i++ ) {
                for( int j=0; j<gy; j++ ) {

                float sgn = 1.0;

                // Forward Search
                for( int k=0; k<gz; k++ ) {
                    int idx[][3] = { {i,j,k} };

                    if( grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].known ) {
                        sgn = grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].dist < 0.0 ? -1.0 : 1.0;
                    } else if( sgn < 0.0 ) {
                        grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].dist = -1.0;
                        grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].known = true;
                    }
                }

                // Backward Search
                for( int k =gz-1; k>=0; k-- ) {
                    int idx[][3] = { {i,j,k}};

                    if( grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].known ) {
                        sgn = grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].dist < 0.0 ? -1.0 : 1.0;
                    } else if( sgn < 0.0 ) {
                        grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].dist = -1.0;
                        grids[idx[dir][0]][idx[dir][1]][idx[dir][2]].known = true;
                    }
                }

            }
        }

    #endif
    }


    inline void redistance( float tolerance ) {
        float wx = 1.0/(gx-1);
        float wy = 1.0/(gy-1);
        float wz = 1.0/(gz-1);
        // Make Everything Known First
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz; k++)
                {
                    if( ! grids[i][j][k].known ) grids[i][j][k].dist = 1.0;
                    grids[i][j][k].known = true;
                }
            }
        }

        {
            int n = 0;
            for(int i = 0; i < gx; i++)
            {
                for(int j = 0; j < gy; j++)
                {
                    for(int k = 0; k < gz; k++)
                    {
                        if(!std::isfinite(grids[i][j][k].dist))
                        {
                            n++;
                        }
                    }
                }
            }

            std::cout << "n flow4 " << n << std::endl;
        }


        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz; k++)
                {
                    bool farCell = true;
                    int query[][3] = { {i+1,j,k-1},{i-1,j,k-1},{i,j+1,k-1},{i,j-1,k-1},
                                       {i-1,j-1,k-1},{i-1,j+1,k-1},{i+1,j-1,k-1},{i+1,j+1,k-1},

                                       {i+1,j,k},{i-1,j,k},{i,j+1,k},{i,j-1,k},
                                       {i-1,j-1,k},{i-1,j+1,k},{i+1,j-1,k},{i+1,j+1,k},

                                       {i+1,j,k+1},{i-1,j,k+1},{i,j+1,k+1},{i,j-1,k+1},
                                       {i-1,j-1,k+1},{i-1,j+1,k+1},{i+1,j-1,k+1},{i+1,j+1,k+1},
                                       {i,j,k+1},{i,j,k-1}};
                    int qnum = 26;
                    float pos[qnum][3];
                    bool fnd[qnum];
                    for( int q=0; q<qnum; q++ ) {
                        int qi = query[q][0];
                        int qj = query[q][1];
                        int qk = query[q][2];
                        fnd[q] = false;
                        if( qi>=0 && qi<gx && qj>=0 && qj<gy && qk>=0 && qk<gz && block->data[qi][qj][qk] < 0.5f ) {
                            if( grids[qi][qj][qk].known && grids[qi][qj][qk].dist * grids[i][j][k].dist < 0.0 ) {
                                farCell = false;

                                // Calculate New Cross Position
                                float y0 = grids[i][j][k].dist;
                                float y1 = grids[qi][qj][qk].dist;
                                float a = y0/(y0-y1);
                                float p0[3] = { wx*i, wy*j ,wz * k};
                                float p1[3] = { wx*qi, wy*qj , wz * qk};
                                pos[q][0] = (1.0-a)*p0[0]+a*p1[0];
                                pos[q][1] = (1.0-a)*p0[1]+a*p1[1];
                                pos[q][2] = (1.0-a)*p0[2]+a*p1[2];
                                fnd[q] = true;
                            }
                        }
                    }
                    if( farCell ) {
                        grids[i][j][k].known = false;
                    } else {
                        grids[i][j][k].known = true;
                        float mind = 9999.0;

                        //for now, we do not take into account any potential hyperplanes/other shapes that might provide a closer distance to the level set
                        //then the linear interpolation with direct neighbors.
                        //the 2d code does this rather nicely
                        //extending this to 3d would require checking a large number of potential planes

                        for ( int q=0; q<qnum; q++ ) {
                            for( int rp=1; rp<26; rp++ ) {

                                /*if(!((query[q][0]-i == -(query[(q+rp)%qnum][0]-i)) && (query[q][1]-j == -(query[(q+rp)%qnum][1]-j)) && (query[q][2]-k == -(query[(q+rp)%qnum][2]-k))))
                                {
                                    if( fnd[(q+rp)%qnum] && fnd[q] ) {
                                        float x = i*wx;
                                        float y = j*wy;
                                        float z = k*wz;
                                        intersect( pos[(q+rp)%qnum][0], pos[(q+rp)%qnum][1], pos[(q+rp)%qnum][2], pos[q][0], pos[q][1], pos[q][2], x, y ,z);
                                        float d = hypot3(x-i*wx,y-j*wy,z-k*wz);
                                        if( d < mind ) {
                                            mind = d;
                                            grids[i][j][k].pos[0] = x;
                                            grids[i][j][k].pos[1] = y;
                                            grids[i][j][k].pos[2] = z;
                                            grids[i][j][k].dist = (grids[i][j][k].dist > 0.0 ? 1.0 : -1.0)*fmax(d,1.0e-8);
                                        }
                                    }

                                }*/
                            }
                        }

                        for ( int q=0; q<qnum; q++ ) {
                            if( fnd[q])
                            {
                                float x = pos[(q)][0];
                                float y = pos[(q)][1];
                                float z = pos[(q)][2];
                                float d = hypot3(x-i*wx,y-j*wy,z-k*wz);
                                if( d < mind ) {
                                    mind = d;
                                    grids[i][j][k].pos[0] = x;
                                    grids[i][j][k].pos[1] = y;
                                    grids[i][j][k].pos[2] = z;
                                    grids[i][j][k].dist = (grids[i][j][k].dist > 0.0 ? 1.0 : -1.0)*fmax(d,1.0e-8);
                                }
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
                for(int k = 0; k < gz; k++)
                {
                    if( ! grids[i][j][k].known )
                    {
                        grids[i][j][k].dist = 1.0;
                    }
                }
            }
        }

        {
            int n = 0;
            int n2 = 0;
            for(int i = 0; i < gx; i++)
            {
                for(int j = 0; j < gy; j++)
                {
                    for(int k = 0; k < gz; k++)
                    {
                        if(grids[i][j][k].dist  < 0.0 && grids[i][j][k].known == true)
                        {
                            n++;
                        }
                        if(!std::isfinite(grids[i][j][k].dist))
                        {

                            n2 ++;
                        }
                    }
                }
            }

            std::cout << "n flow6 " << n << " " << n2 << std::endl;
        }

        fastMarch(tolerance);

        {
            int n = 0;
            int n2 = 0;
            for(int i = 0; i < gx; i++)
            {
                for(int j = 0; j < gy; j++)
                {
                    for(int k = 0; k < gz; k++)
                    {
                        if(grids[i][j][k].dist  == 0.0 && grids[i][j][k].known == true)
                        {
                            n++;
                        }
                        if(!std::isfinite(grids[i][j][k].dist))
                        {

                            n2 ++;
                        }
                    }
                }
            }

            std::cout << "n flow6 " << n << " " << n2 << std::endl;
        }

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

    inline float spline( cTMap3D *d, float x, float y, float z, int w, int h, int l ) {

        float yn[4];
        for(int k = 0; k < 4; k++)
        {
            float f[16];
            float xn[4];

            if (x<0.0) x=0.0; if (x>=w) x=w-0.01;
            if (y<0.0) y=0.0; if (y>=h) y=h-0.01;
            if (z<0.0) z=0.0; if (z>=l) z=l-0.01;

            for( int j=0; j<4; j++ ) {
                for( int i=0; i<4; i++ ) {
                    int hd = (int)x - 1 + i;
                    int v = (int)y - 1 + j;
                    int vz = (int)z - 1 + k;
                    f[4*j+i] = d->data[iclamp(hd,0,w-1)][iclamp(v,0,h-1)][iclamp(vz,0,l-1)];
                }
            }

            for( int j=0; j<4; j++ )
            {
                xn[j] = spline_cubic( &f[4*j], x - (int)x );
            }
            yn[k] = spline_cubic( xn, y - (int)y );
        }

        return spline_cubic(yn,z-(int)z);

    }

    inline float linear ( cTMap3D *d, float x, float y, float z, int w, int h , int l) {
        /*x = fmax(0.0,fmin(w,x));
        y = fmax(0.0,fmin(h,y));
        int i = std::min(x,((float)(w-2)));
        int j = std::min(y,((float)(h-2)));
        int k = std::min(z,((float)(l-2)));
        return ((i+1-x)*d->data[i][j][k]+(x-i)*d->data[i+1][j][k])*(j+1-y) + ((i+1-x)*d->data[i][j+1][k]+(x-i)*d->data[i+1][j+1][k])*(y-j);
        */

        x = fmax(0.0,fmin(w,x));
        y = fmax(0.0,fmin(h,y));
        z = fmax(0.0,fmin(l,z));
        int i = std::min(x,((float)(w-2)));
        int j = std::min(y,((float)(h-2)));
        int k = std::min(z,((float)(l-2)));
        return ( ((k+1-z)*d->data[i][j][k]+(z-k)*d->data[i][j][k+1])*(j+1-y) + ((k+1-z)*d->data[i][j+1][k]+(z-k)*d->data[i][j+1][k+1])*(y-j) )*(i+1-x) + ( ((k+1-z)*d->data[i+1][j][k]+(z-k)*d->data[i+1][j][k+1])*(j+1-y) + ((k+1-z)*d->data[i+1][j+1][k]+(z-k)*d->data[i+1][j+1][k+1])*(y-j) )*(x-i);

    }

    inline float interp ( cTMap3D *d, float x, float y, float z,int w, int h, int l ) {
        float r = 0.0;
        switch (method) {
            case 0:
                r = linear(d,x,y,z,w,h, l);
                break;
            case 1:
                r = spline(d,x,y,z,w,h,l);
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
        delete blockw; blockw = nullptr;

        free3D(grids);
    }
    inline void markLiquid() {
        getLevelSet(A);
    }
    inline float getLevelSet( int i, int j , int k) {
        if( i<0 || i>gx-1 || j<0 || j>gy-1 || k<0 || k>gz-1 || block->data[i][j][k] > 0.5f) return 1.0;
        return grids[i][j][k].dist;
    }

    inline void getLevelSet( cTMap3D *dists ) {

        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz; k++)
                {
                    if(block->data[i][j][k])
                    {
                        dists->data[i][j][k] = 1.0;
                    }

                     dists->data[i][j][k] = grids[i][j][k].known ? grids[i][j][k].dist : 1.0;

                }
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
                for(int k = 0; k < gz; k++)
                {
                    source_dists->data[i][j][k] = grids[i][j][k].dist;
                }
            }

        }

        {
            int n = 0;
            int n2 = 0;
            int n4 = 0;
            int n3 = 0;
            for(int i = 0; i < gx; i++)
            {
                for(int j = 0; j < gy; j++)
                {
                    for(int k = 0; k < gz; k++)
                    {

                        if(grids[i][j][k].dist  == 0.0 && grids[i][j][k].known == true)
                        {
                            n++;
                        }
                        if(grids[i][j][k].known == true)
                        {
                            n3++;
                        }
                        if(grids[i][j][k].dist < 0.0 )
                        {
                            n4 ++;
                        }
                        if(!std::isfinite(grids[i][j][k].dist))
                        {

                            n2 ++;
                        }
                    }
                }
            }
        }



        // Advect
        float wx = 1.0/(gx-1);
        float wy = 1.0/(gy-1);
        float wz = 1.0/(gz-1);
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz; k++)
                {
                    float x, y,z ;
                    float u, v, w, dt;
                    x = (i)*wx;//-(1.0/((float(gx))))
                    y = (j)*wy;
                    z = (k)*wz;
                    flow( x, y,z, u, v, w,dt );
                    // Semi-Lagragian
                    x -= dt*u * (1.0/((float(gx-1))*m_sxr)) ;
                    y -= dt*v * (1.0/((float(gy-1))*m_syr)) ;
                    z -= dt*w * (1.0/((float(gz-1))*m_szr)) ;
                    x = fmin(1.0,fmax(0.0,x));
                    y = fmin(1.0,fmax(0.0,y));
                    z = fmin(1.0,fmax(0.0,z));
                    // Interpolate Dists
                    if( grids[(int)((gx-1)*x)][(int)((gy-1)*y)][(int)((gz-1)*z)].known ) {
                        swap_dists->data[i][j][k] = y<1.0 ? interp( source_dists, (gx-1)*x, (gy-1)*y,(gz-1)*z, gx, gy,gz ) : 1.0;
                    } else {
                        swap_dists->data[i][j][k] = source_dists->data[i][j][k];
                    }
                }
            }

        }

        // Swap
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz; k++)
                {
                    if(block->data[i][j][k] <= 0.5f)
                    {
                        grids[i][j][k].dist = swap_dists->data[i][j][k];
                    }else
                    {
                        grids[i][j][k].dist = 1.0;
                    }
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

        std::cout << "volume error " << volume0<< "  " << curVolume << std::endl;

        float x = (curVolume - volume0)/volume0;
        y_volume0 += x*m_DT;

        float kp = 2.3 / (25.0 * m_DT);
        float ki = kp*kp/16.0;
        vdiv = -(kp * x + ki * y_volume0) / (x + 1.0);
    }


    inline void enforce_boundary()
    {
        for(int i = 0; i < gx+1; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz; k++)
                {

                    if( i==0 || i==gx )
                    {
                        ux->data[i][j][k] = 0.0;
                    }else {
                        if(block->data[i][j][k] > 0.5f)
                        {
                            ux->data[i][j][k] = std::min(blocku->data[i][j][k],ux->data[i][j][k]);

                        }
                        if(block->data[i-1][j][k] > 0.5f)
                        {
                            ux->data[i][j][k] = std::max(blocku->data[i-1][j][k],ux->data[i][j][k]);

                        }
                        if(block->data[i][j][k] > 0.5f && block->data[i-1][j][k] > 0.5f)
                        {
                            ux->data[i][j][k] = 0.0;
                        }
                    }
                }
            }

        }

        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy+1; j++)
            {
                for(int k = 0; k < gz; k++)
                {
                    if( j==0 || j==gy)
                    {
                        uy->data[i][j][k] = 0.0;
                    }else {
                        if(block->data[i][j][k] > 0.5f)
                        {
                            uy->data[i][j][k] = std::min(blockv->data[i][j][k],uy->data[i][j][k]);

                        }
                        if(block->data[i][j-1][k] > 0.5f)
                        {
                            uy->data[i][j][k] = std::max(blockv->data[i][j-1][k],uy->data[i][j][k]);

                        }
                        if(block->data[i][j][k] > 0.5f && block->data[i][j-1][k] > 0.5f)
                        {
                            uy->data[i][j][k] = 0.0;
                        }
                    }
                }
            }
        }

        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz+1; k++)
                {
                    if( k==0 || k==gz)
                    {
                        uz->data[i][j][k] = 0.0;
                    }else {
                        if(block->data[i][j][k] > 0.5f)
                        {
                            uz->data[i][j][k] = std::min(blockw->data[i][j][k],uz->data[i][j][k]);

                        }
                        if(block->data[i][j][k-1] > 0.5f)
                        {
                            uz->data[i][j][k] = std::max(blockw->data[i][j][k-1],uz->data[i][j][k]);

                        }
                        if(block->data[i][j][k] > 0.5f && block->data[i][j][k-1] > 0.5f)
                        {
                            uz->data[i][j][k] = 0.0;
                        }
                    }
                }
            }
        }
    }

    inline  void comp_divergence() {
        float hx = m_sx;
        float hy = m_sy;
        float hz = m_sz;
        //std::cout << "do div " << vdiv << std::endl;
        // Swap

        //#pragma omp parallel for collapse(3)
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz; k++)
                {

                    float div = A->data[i][j][k]<0.0 ? (ux->data[i+1][j][k]-ux->data[i][j][k])/(m_sxr) + (uy->data[i][j+1][k]-uy->data[i][j][k])/(m_syr) + (uz->data[i][j][k+1]-uz->data[i][j][k])/(m_szr) : 0.0;
                    d->data[i][j][k] = div;// - vdiv;
                    if(A->data[i][j][k]<0.0)
                    {
                       //std::cout << "vel " << i << "  " << j << " " << k << " " << ux->data[i][j][k] << " " << uy->data[i][j][k] << " " << uz->data[i][j][k] <<  std::endl;
                       //std::cout << "div " << i << "  " << j << " " << k << " " << div << std::endl;
                    }
                }
            }
        }
    }


    inline void compute_pressure() {
        // Clear Pressure
        #pragma omp parallel for collapse(3)
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz; k++)
                {
                    p->data[i][j][k] = 0.0;
                }
            }
        }

        // Solve Ap = d ( p = Pressure, d = Divergence )
        solve( A, p, d, gx,gy,gz, subcell, solver_mode );
    }

    inline void subtract_pressure() {
        float hx = m_sx;
        float hy = m_sy;
        float hz = m_sz;

        #pragma omp parallel for collapse(3)
        for(int i = 0; i < gx+1; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz; k++)
                {

                if( i>0 && i<gx ) {
                    float pf = p->data[i][j][k];
                    float pb = p->data[i-1][j][k];
                    if( subcell && A->data[i][j][k] * A->data[i-1][j][k] < 0.0 ) {
                        pf = A->data[i][j][k] < 0.0 ? p->data[i][j][k] : A->data[i][j][k]/fmin(1.0e-3,A->data[i-1][j][k])*p->data[i-1][j][k];
                        pb = A->data[i-1][j][k] < 0.0 ? p->data[i-1][j][k] : A->data[i-1][j][k]/fmin(1.0e-6,A->data[i][j][k])*p->data[i][j][k];
                    }

                    if(!(block->data[i][j][k] > 0.5 || block->data[i-1][j][k] > 0.5))
                    {
                    ux->data[i][j][k] -= (pf-pb)/(m_sxr);
                    }
                }


            }
            }
        }

        #pragma omp parallel for collapse(3)
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy+1; j++)
            {
                for(int k = 0; k < gz; k++)
                {


            if( j>0 && j<gy ) {
                float pf = p->data[i][j][k];
                float pb = p->data[i][j-1][k];
                if( subcell && A->data[i][j][k] * A->data[i][j-1][k] < 0.0 ) {
                    pf = A->data[i][j][k] < 0.0 ? p->data[i][j][k] : A->data[i][j][k]/fmin(1.0e-3,A->data[i][j-1][k])*p->data[i][j-1][k];
                    pb = A->data[i][j-1][k] < 0.0 ? p->data[i][j-1][k] : A->data[i][j-1][k]/fmin(1.0e-6,A->data[i][j][k])*p->data[i][j][k];
                }
                if(!(block->data[i][j][k] > 0.5 || block->data[i][j-1][k] > 0.5))
                {
                uy->data[i][j][k] -= (pf-pb)/(m_syr);
                }
            }
            }
            }
        }

        #pragma omp parallel for collapse(3)
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz+1; k++)
                {


            if( k>0 && k<gz ) {
                float pf = p->data[i][j][k];
                float pb = p->data[i][j][k-1];
                if( subcell && A->data[i][j][k] * A->data[i][j][k-1] < 0.0 ) {
                    pf = A->data[i][j][k] < 0.0 ? p->data[i][j][k] : A->data[i][j][k]/fmin(1.0e-3,A->data[i][j][k-1])*p->data[i][j][k-1];
                    pb = A->data[i][j][k-1] < 0.0 ? p->data[i][j][k-1] : A->data[i][j][k-1]/fmin(1.0e-6,A->data[i][j][k])*p->data[i][j][k];
                }
                if(!(block->data[i][j][k] > 0.5 || block->data[i][j][k-1] > 0.5))
                {uz->data[i][j][k] -= (pf-pb)/(m_szr);
                }
            }
            }
            }
        }
    }
    inline void extrapolate( cTMap3D *_q, char ***region ) {
        // Unknowns
        grid_queue unknowns;

        // Computed Field


        // Push

        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz; k++)
                {
                if( ! region[i][j][k] && grids[i][j][k].known ) {
                    unknowns.push(&grids[i][j][k]);
                }
                computed[i][j][k] = region[i][j][k];
                }
            }}

        // While Unknowns Exists
        while( ! unknowns.empty() ) {
            // Pop Out Top
            grid *mingrid = unknowns.top();
            int i = mingrid->gp[0];
            int j = mingrid->gp[1];
            int k = mingrid->gp[2];

            int sum = 0;
            float sumq = 0.0;
            int query[][3] = { {i+1,j,k-1},{i-1,j,k-1},{i,j+1,k-1},{i,j-1,k-1},{i-1,j-1,k-1},{i-1,j+1,k-1},{i+1,j-1,k-1},{i+1,j+1,k-1}, {i+1,j,k},{i-1,j,k},{i,j+1,k},{i,j-1,k},{i-1,j-1,k},{i-1,j+1,k},{i+1,j-1,k},{i+1,j+1,k}, {i+1,j,k+1},{i-1,j,k+1},{i,j+1,k+1},{i,j-1,k+1},{i-1,j-1,k+1},{i-1,j+1,k+1},{i+1,j-1,k+1},{i+1,j+1,k+1},{i,j,k+1},{i,j,k-1}};
            int qnum = 26;
            for( int nq=0; nq<qnum; nq++ ) {
                int qi = query[nq][0];
                int qj = query[nq][1];
                int qk = query[nq][2];
                if( qi>=0 && qi<gx && qj>=0 && qj<gy && qk>=0 && qk<gz && block->data[qi][qj][qk] < 0.5f) {
                    if( computed[qi][qj][qk] ) {
                        sumq += _q->data[qi][qj][qk];
                        sum ++;
                    }
                }
            }
            if( sum ) {
                if(block->data[i][j][k])
                {
                    _q->data[i][j][k] = 0.0;
                }else
                {
                    _q->data[i][j][k] = sumq / sum;
                }
            }
            unknowns.pop();
            computed[i][j][k] = 1;
        }
    }

    inline void extrapolateVelocity() {

        // Map To LevelSet (X Direction)
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz; k++)
                {
                    if( i<gx-1 && A->data[i][j][k]<0.0 ) {
                        region[i][j][k] = 1;
                        q->data[i][j][k] = (ux->data[i][j][k]+ux->data[i+1][j][k])*0.5;
                    }
                    else {
                        region[i][j][k] = 0;
                        q->data[i][j][k] = 0.0;
                    }
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
                for(int k = 0; k < gz; k++)
                {
                if( i>0 && i<gx && (A->data[i][j][k]>0.0 || A->data[i-1][j][k]>0.0) ) ux->data[i][j][k] = (q->data[i][j][k]+q->data[i-1][j][k])*0.5;
                }
        } }

        // Map To LevelSet (Y Direction)
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz; k++)
                {
                if( j<gy-1 && A->data[i][j][k]<0.0 ) {
                    region[i][j][k] = 1;
                    q->data[i][j][k] = (uy->data[i][j][k]+uy->data[i][j+1][k])*0.5;
                } else {
                    region[i][j][k] = 0;
                    q->data[i][j][k] = 0.0;
                }
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
                for(int k = 0; k < gz; k++)
                {
                if( j>0 && j<gy && (A->data[i][j][k]>0.0 || A->data[i][j-1][k]>0.0) ) uy->data[i][j][k] = (q->data[i][j][k]+q->data[i][j-1][k])*0.5;
                }
        }}

        // Map To LevelSet (Z Direction)
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz; k++)
                {
                if( k<gz-1 && A->data[i][j][k]<0.0 ) {
                    region[i][j][k] = 1;
                    q->data[i][j][k] = (uz->data[i][j][k]+uz->data[i][j][k+1])*0.5;
                } else {
                    region[i][j][k] = 0;
                    q->data[i][j][k] = 0.0;
                }
                }
            }
        }

        // Extrapolate
        extrapolate( q, region );

        // Map Back (Z Direction)
        for(int i = 0; i < gx; i++)
        {
            for(int j = 0; j < gy; j++)
            {
                for(int k = 0; k < gz+1; k++)
                {
                if( k>0 && k<gz && (A->data[i][j][k]>0.0 || A->data[i][j][k-1]>0.0) ) uz->data[i][j][k] = (q->data[i][j][k]+q->data[i][j][k-1])*0.5;
                }
        }}
    }





    char subcell_solver = 0;
    char solver_mode_solver = 0;

    // Clamped Fetch
    inline float x_ref( cTMap3D *_A, cTMap3D *_x, int fi, int fj,int fz, int i, int j, int k,int nx, int ny, int nz ) {
        i = std::min(std::max(0,i),nx-1);
        j = std::min(std::max(0,j),ny-1);
        k = std::min(std::max(0,k),nz-1);
        if(block->data[i][j][k] > 0.5f)
        {
            i = fi;
            j = fj;
            k = fz;
            if( _A->data[i][j][k] < 0.0 ) return _x->data[i][j][k];
            return 0.0;
        }

        if( _A->data[i][j][k] < 0.0 ) return _x->data[i][j][k];
        return subcell_solver ? _A->data[i][j][k]/fmin(1.0e-6,_A->data[fi][fj][fz])*_x->data[fi][fj][fz] : 0.0;
    }

    // Ans = Ax
    inline void compute_Ax( cTMap3D *_A, cTMap3D *_x, cTMap3D *_ans, int nx, int ny, int nz ) {
        float hx =(m_sxr);
        float hy =(m_syr);
        float hz =(m_szr);
        float h3 = 1.0/(nx*ny*nz);
        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
                for( int k=0; k<nz; k++ ) {
                    if( _A->data[i][j][k] < 0.0 ) {
                        //_ans->data[i][j][k] = (6.0*_x->data[i][j][k]-x_ref(_A,_x,i,j,k,i+1,j,k,nx,ny,nz)-x_ref(_A,_x,i,j,k,i-1,j,k,nx,ny,nz)-x_ref(_A,_x,i,j,k,i,j+1,k,nx,ny,nz)-x_ref(_A,_x,i,j,k,i,j-1,k,nx,ny,nz) -x_ref(_A,_x,i,j,k,i,j,k+1,nx,ny,nz)-x_ref(_A,_x,i,j,k,i,j,k-1,nx,ny,nz))/(h3);
                        _ans->data[i][j][k] = (2.0*_x->data[i][j][k]-x_ref(_A,_x,i,j,k,i+1,j,k,nx,ny,nz)-x_ref(_A,_x,i,j,k,i-1,j,k,nx,ny,nz))/(hx*hx);
                        _ans->data[i][j][k] += (2.0*_x->data[i][j][k]-x_ref(_A,_x,i,j,k,i,j+1,k,nx,ny,nz)-x_ref(_A,_x,i,j,k,i,j-1,k,nx,ny,nz))/(hy*hy);
                        _ans->data[i][j][k] += (2.0*_x->data[i][j][k]-x_ref(_A,_x,i,j,k,i,j,k+1,nx,ny,nz)-x_ref(_A,_x,i,j,k,i,j,k-1,nx,ny,nz))/(hz*hz);

                    } else {
                        _ans->data[i][j][k] = 0.0;
                    }
                }
            }

        }
    }

    // ans = x^T * x
    inline float product( cTMap3D *_A, cTMap3D *_x, cTMap3D *_y, int nx, int ny, int nz ) {
        float ans = 0.0;
        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
                for( int k=0; k<nz; k++ ) {
                    if( _A->data[i][j][k] < 0.0 ) ans += _x->data[i][j][k]*_y->data[i][j][k];
                }
            }
        }
        return ans;
    }

    // x = 0
    inline void clear( cTMap3D *_x, int nx, int ny, int nz) {
        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
                for( int k=0; k<nz; k++ ) {
                    _x->data[i][j][k] = 0.0;
                }
            }
        }
    }

    inline void flip( cTMap3D *_x, int nx, int ny, int nz ) {
        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
                for( int k=0; k<nz; k++ ) {
                    _x->data[i][j][k] = -_x->data[i][j][k];
                }
            }
        }
    }

    // x <= y
    inline void copy( cTMap3D *_x, cTMap3D *_y, int nx, int ny, int nz ) {
        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
                for( int k=0; k<nz; k++ ) {
                    _x->data[i][j][k] = _y->data[i][j][k];
                }
            }
        }
    }

    // Ans = x + a*y
    inline void op( cTMap3D *_A, cTMap3D *_x, cTMap3D *_y, cTMap3D *_ans,float a, int nx, int ny, int nz ) {

        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
                for( int k=0; k<nz; k++ ) {
                    if( _A->data[i][j][k] < 0.0 )
                    {
                        tmp->data[i][j][k] = _x->data[i][j][k]+a*_y->data[i][j][k];
                    }else {
                        tmp->data[i][j][k] = 0.0;
                    }
                }
            }
        }
        copy(_ans,tmp,nx,ny,nz);
    }

    // r = b - Ax
    inline void residual( cTMap3D *_A, cTMap3D *_x, cTMap3D *_b, cTMap3D *_r, int nx, int ny, int nz ) {
        compute_Ax(_A,_x,_r,nx,ny,nz);
        op( _A, _b, _r, _r, -1.0, nx,ny,nz );
    }

    inline float square( float a ) {
        return a*a;
    }

    inline float A_ref( cTMap3D *_A, int i, int j, int k,int qi, int qj, int qk,int nx, int ny, int nz) {
        if( i<0 || i>nx-1 || j<0 || j>ny-1 ||  k<0 || k>nz-1 || _A->data[i][j][k]>0.0 || block->data[i][j][k] > 0.5f) return 0.0;
        if( qi<0 || qi>nx-1 || qj<0 || qj>ny-1 ||  qk<0 || qk>nz-1 || _A->data[qi][qj][qk]>0.0|| block->data[qi][qj][qk] > 0.5f ) return 0.0;
        return -1.0;
    }

    inline float A_diag( cTMap3D *_A, int i, int j, int k, int nx, int ny, int nz ) {
        float diag = 6.0;
        if( _A->data[i][j][k] > 0.0 ) return diag;
        int q3[][3] = { {i-1,j,k}, {i+1,j,k}, {i,j-1,k}, {i,j+1,k} , {i,j,k-1}, {i,j,k+1} };
        for( int m=0; m<6; m++ ) {
            int qi = q3[m][0];
            int qj = q3[m][1];
            int qk = q3[m][2];
            if( qi<0 || qi>nx-1 || qj<0 || qj>ny-1 || qk<0 || qk>nz-1 || block->data[qi][qj][qk] > 0.5f) diag -= 1.0;
            else if( _A->data[qi][qj][qk] > 0.0 && subcell ) {
                diag -= _A->data[qi][qj][qk]/fmin(1.0e-6,_A->data[i][j][k]);
            }
        }
        return diag;
    }

    inline float P_ref( cTMap3D *_P, int i, int j,int k, int nx,int ny , int nz) {
        if( i<0 || i>nx-1 || j<0 || j>ny-1 ||  k<0 || k>nz-1 ) return 0.0;
        if(block->data[i][j][k] > 0.5f)
        {
            return 0.0;
        }
        return _P->data[i][j][k];
    }



    inline void buildPreconditioner( cTMap3D *_P, cTMap3D *_A, int nx, int ny, int nz ) {
        clear(_P,nx,ny,nz);
        float t = solver_mode == 2 ? 0.97 : 0.0;
        float a = 0.125;
        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
                for( int k=0; k<nz; k++ ) {
                    if(_A->data[i][j][k] < 0.0 ) {
                        float left = A_ref(_A,i-1,j,k,i,j,k,nx,ny,nz)*P_ref(_P,i-1,j,k,nx,ny,nz);
                        float bottom = A_ref(A,i,j-1,k,i,j,k,nx,ny,nz)*P_ref(_P,i,j-1,k,nx,ny,nz);
                        float top = A_ref(A,i,j,k-1,i,j,k,nx,ny,nz)*P_ref(_P,i,j,k-1,nx,ny,nz);
                        float mleft = A_ref(_A,i-1,j,k,i,j,k,nx,ny,nz)*A_ref(_A,i,j-1,k,i,j,k,nx,ny,nz)*A_ref(_A,i-1,j,k,i,j,k,nx,ny,nz)*square(P_ref(_P,i-1,j,k,nx,ny,nz));
                        float mbottom = A_ref(_A,i,j-1,k,i,j,k,nx,ny,nz)*A_ref(_A,i-1,j,k,i,j,k,nx,ny,nz)*A_ref(_A,i,j-1,k,i,j,k,nx,ny,nz)*square(P_ref(_P,i,j-1,k,nx,ny,nz));
                        float mtop1 = A_ref(_A,i-1,j,k,i,j,k,nx,ny,nz)*A_ref(_A,i,j,k-1,i,j,k,nx,ny,nz)*A_ref(_A,i-1,j,k,i,j,k,nx,ny,nz)*square(P_ref(_P,i-1,j,k,nx,ny,nz));
                        float mtop2 = A_ref(_A,i,j-1,k,i,j,k,nx,ny,nz)*A_ref(_A,i,j,k-1,i,j,k,nx,ny,nz)*A_ref(_A,i,j-1,k,i,j,k,nx,ny,nz)*square(P_ref(_P,i,j-1,k,nx,ny,nz));
                        float mtop3 = A_ref(_A,i,j,k-1,i,j,k,nx,ny,nz)*A_ref(_A,i-1,j,k,i,j,k,nx,ny,nz)*A_ref(_A,i,j,k-1,i,j,k,nx,ny,nz)*square(P_ref(_P,i,j,k-1,nx,ny,nz));
                        float mtop4 = A_ref(_A,i,j,k-1,i,j,k,nx,ny,nz)*A_ref(_A,i,j-1,k,i,j,k,nx,ny,nz)*A_ref(_A,i,j,k-1,i,j,k,nx,ny,nz)*square(P_ref(_P,i,j,k-1,nx,ny,nz));

                        float diag = A_diag( _A, i, j,k, nx,ny ,nz);
                        float e = std::max(1e-4f,diag - square(left) - square(bottom) - square(top)- t*( mleft + mbottom+ mtop1 + mtop2 + mtop3 + mtop4 ));
                        if( e < a*diag ) e = diag;
                        _P->data[i][j][k] = 1.0/sqrtf(e);
                        if(!std::isfinite(_P->data[i][j][k]) )
                        {
                            std::cout << _P->data[i][j][k] << " " << e  << "  " << diag << " " << t <<  " " << left << " " << std::endl;

                        }
                    }
                }
            }
        }
    }

    inline void applyPreconditioner(  cTMap3D *_z,  cTMap3D *_r,  cTMap3D *_P, cTMap3D *_A, int nx, int ny,int nz ) {
        if( solver_mode_solver == 0 ) {
            copy(_z,_r,nx,ny,nz);
            return;
        }

        clear(q2,nx,ny,nz);
        //clear(_z,nx,ny,nz);

        // Lq = r
        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
                for( int k=0; k<nz; k++ ) {
                    if( _A->data[i][j][k] < 0.0 ) {
                    float left = A_ref(_A,i-1,j,k,i,j,k,nx,ny,nz)*P_ref(_P,i-1,j,k,nx,ny,nz)*P_ref(q2,i-1,j,k,nx,ny,nz);
                    float bottom = A_ref(_A,i,j-1,k,i,j,k,nx,ny,nz)*P_ref(_P,i,j-1,k,nx,ny,nz)*P_ref(q2,i,j-1,k,nx,ny,nz);
                    float top = A_ref(_A,i,j,k-1,i,j,k,nx,ny,nz)*P_ref(_P,i,j,k-1,nx,ny,nz)*P_ref(q2,i,j,k-1,nx,ny,nz);

                    float t = _r->data[i][j][k] - left - bottom - top;
                    q2->data[i][j][k] = t*_P->data[i][j][k];
                    if(!std::isfinite(q2->data[i][j][k]))
                    {
                        std::cout << "nan1 " << i << " " << j << " " << k << " " << t << " "<< _r->data[i][j][k] << "   "  <<_P->data[i][j][k] << "    " << A_ref(_A,i-1,j,k,i,j,k,nx,ny,nz) << "  " << A_ref(_A,i,j-1,k,i,j,k,nx,ny,nz)<< " " << A_ref(_A,i,j,k-1,i,j,k,nx,ny,nz) <<  " "  <<  P_ref(_P,i-1,j,k,nx,ny,nz) << " "<< P_ref(_P,i,j,k-1,nx,ny,nz)  << "  " << P_ref(_P,i,j-1,k,nx,ny,nz) <<  "  "<< P_ref(q2,i-1,j,k,nx,ny,nz) << "  " << P_ref(q2,i,j-1,k,nx,ny,nz) <<  "  " << P_ref(q2,i,j,k-1,nx,ny,nz) << std::endl;
                    }
                }}
            }
        }

        // L^T z = q

        for( int i=nx-1; i>=0; i-- ) {
            for( int j=ny-1; j>=0; j-- ) {
                for( int k=nz -1; k >= 0 ; k-- ) {
                    if( _A->data[i][j][k] < 0.0 ) {
                    float right = A_ref(_A,i,j,k,i+1,j,k,nx,ny,nz)*P_ref(_P,i,j,k,nx,ny,nz)*P_ref(_z,i+1,j,k,nx,ny,nz);
                    float top = A_ref(_A,i,j,k,i,j+1,k,nx,ny,nz)*P_ref(_P,i,j,k,nx,ny,nz)*P_ref(_z,i,j+1,k,nx,ny,nz);
                    float side = A_ref(_A,i,j,k,i,j,k+1,nx,ny,nz)*P_ref(_P,i,j,k,nx,ny,nz)*P_ref(_z,i,j,k+1,nx,ny,nz);

                    float t = q2->data[i][j][k] - right - top - side;
                    _z->data[i][j][k] = t*_P->data[i][j][k];

                    if(!std::isfinite(_z->data[i][j][k]))
                    {
                        std::cout << "nan2 " << i << " " << j << " "  <<k << " " << A_ref(_A,i,j,k,i+1,j,k,nx,ny,nz) << " " << P_ref(_P,i,j,k,nx,ny,nz) << " " << P_ref(_z,i+1,j,k,nx,ny,nz) << " " << P_ref(_z,i,j,k+1,nx,ny,nz) << " " << q2->data[i][j][k]  << std::endl;
                    }
                }}
            }
        }
    }

    inline int GetMVCells(cTMap3D * _x,int nx, int ny, int nz)
    {
        int n = 0;
        for( int i=nx-1; i>=0; i-- ) {
            for( int j=ny-1; j>=0; j-- ) {
                for( int k=0; k<nz; k++ ) {
                    if(!std::isfinite(_x->data[i][j][k]))
                    {
                        n++;
                    }
                }
            }
        }

        return n;

    }

    inline int GetFlowCells(cTMap3D * _x,int nx, int ny, int nz)
    {
        int n = 0;
        for( int i=nx-1; i>=0; i-- ) {
            for( int j=ny-1; j>=0; j-- ) {
                for( int k=0; k<nz; k++ ) {
                    if(!std::isfinite(_x->data[i][j][k]))
                    {
                            n ++;

                    }
                }
            }
        }

        return n;

    }


    inline int  GetCellCount( cTMap3D *_A, int nx, int ny, int nz ) {

        int count = 0;
        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
                for( int k=0; k<nz; k++ ) {
                    if( _A->data[i][j][k] < 0.0 )
                    {
                        count ++;

                    }
                }
            }
        }

        return count;
    }
    inline void conjGrad( cTMap3D *_A, cTMap3D *_P, cTMap3D *_x, cTMap3D * _b, int nx,int ny, int nz ) {
        // Pre-allocate Memory

        std::cout << "flow3d_conjgrad "<< GetMVCells(_A,nx,ny,nz) << " "<< GetMVCells(_P,nx,ny,nz) << " "<< GetMVCells(_x,nx,ny,nz) << " "<< GetMVCells(_b,nx,ny,nz)<<  " "<< GetMVCells(r,nx,ny,nz)<<  " "<< GetMVCells(z,nx,ny,nz)<<  " "<< GetMVCells(s,nx,ny,nz)<< std::endl;

        clear(_x,nx,ny,nz );									// p = 0
        copy(r,_b,nx,ny,nz );								// r = b
        applyPreconditioner(z,r,_P,_A,nx,ny,nz );				// Apply Conditioner z = f(r)
        copy(s,z,nx,ny,nz );								// s = z

        std::cout << "flow3d_conjgrad2 "<< GetMVCells(_A,nx,ny,nz) << " "<< GetMVCells(_P,nx,ny,nz) << " "<< GetMVCells(_x,nx,ny,nz) << " "<< GetMVCells(_b,nx,ny,nz)<<  " "<< GetMVCells(r,nx,ny,nz)<<  " "<< GetMVCells(z,nx,ny,nz)<<  " "<< GetMVCells(s,nx,ny,nz)<< std::endl;


        float cellcount = GetCellCount(_A,nx,ny,nz);

        float a = product( _A, z, r, nx,ny,nz  );			// a = z . r
        for( int k=0; k<nx*ny*nz; k++ ) {

            std::cout << "iter "<< k << "   " << GetMVCells(_A,nx,ny,nz) << " "<< GetMVCells(_P,nx,ny,nz) << " "<< GetMVCells(_x,nx,ny,nz) << " "<< GetMVCells(_b,nx,ny,nz)<<  " "<< GetMVCells(r,nx,ny,nz)<<  " "<< GetMVCells(z,nx,ny,nz)<<  " "<< GetMVCells(s,nx,ny,nz)<< std::endl;

            std::cout << "flow cells " << GetFlowCells(_A,nx,ny,nz) << std::endl;

            compute_Ax( _A, s, z, nx,ny,nz );				// z = applyA(s)
            std::cout << product( _A, z, s, nx,ny,nz  ) << std::endl;

            float prod = product( _A, z, s, nx,ny,nz  );
            if(std::fabs(prod) < 1e-20)
            {
                break;
            }
            float alpha = a/product( _A, z, s, nx,ny,nz  );	// alpha = a/(z . s)
            op( _A, _x, s, _x, alpha, nx,ny ,nz );				// p = p + alpha*s
            op( _A, r, z, r, -alpha, nx,ny,nz  );			// r = r - alpha*z;
            float error2 = product( _A, r, r, nx,ny,nz  );	// error2 = r . r
            std::cout << "error "<< k << "   " << error2/(cellcount)  << " " << cellcount << std::endl;

            if( error2/(cellcount) < 1.0e-10 && k > 35 ) break;
            applyPreconditioner(z,r,_P,_A,nx,ny,nz );			// Apply Conditioner z = f(r)
            float a2 = product( _A, z, r, nx,ny,nz  );		// a2 = z . r
            float beta = a2/a;
            op( _A, z, s, s, beta, nx,ny,nz  );				// s = z + beta*s
            a = a2;
        }
    }

    inline float solve( cTMap3D *_A, cTMap3D * _x, cTMap3D *_b, int nx,int ny, int nz, char subcell_aware, char solver_type ) {

        clear(r2,nx,ny,nz );

        // Save Mode
        subcell_solver = subcell_aware;
        solver_mode_solver = solver_type;

        // Flip Divergence
        flip(_b,nx,ny,nz );

        // Build Modified Incomplete Cholesky Precondioner Matrix
        if( solver_mode >= 1 ) buildPreconditioner(P,_A,nx,ny,nz );

        // Conjugate Gradient Method
        conjGrad(_A,P,_x,_b,nx,ny,nz );


        residual(_A,_x,_b,r2,nx,ny,nz );
        float res = std::sqrt(product( _A, r2, r2, nx,ny,nz  ))/(nx*ny*nz);
        // printf( "Residual = %e\n", res );
        return res;
    }


    cTMap3D *gux = NULL;
    cTMap3D *guy = NULL;
    cTMap3D *guz = NULL;

    // Clamped Fluid Flow Fetch
    inline float u_ref( int dir, int i, int j, int k ) {
        if( dir == 0 )
        {

            return gux->data[std::max(0,std::min(gx,i))][std::max(0,std::min(gy-1,j))][std::max(0,std::min(gz-1,k))];
        }
        else if(dir == 1)
        {
            return guy->data[std::max(0,std::min(gx-1,i))][std::max(0,std::min(gy,j))][std::max(0,std::min(gz-1,k))];
        }else {
            return guz->data[std::max(0,std::min(gx-1,i))][std::max(0,std::min(gy-1,j))][std::max(0,std::min(gz,k))];
        }
    }

    inline void semiLagrangian( cTMap3D *_d, cTMap3D*_d0, int width, int height, int length, cTMap3D* u1, cTMap3D * u2,cTMap3D * u3 ) {
        for( int i=0; i<gx; i++ ) {
            for( int j=0; j<gy; j++ ) {
                for( int k=0; k<gz; k++ ) {
            _d->data[i][j][k] = interp( _d0, i-u1->data[i][j][k]*m_DT/m_sxr, j-u2->data[i][j][k]*m_DT/m_syr, k-u3->data[i][j][k]*m_DT/m_szr,width, height , length);
                }
            }
        }
    }

    // Semi-Lagrangian Advection Method
    inline  void advect_semiLagrangian( cTMap3D * _ux, cTMap3D * _uy, cTMap3D * _uz,cTMap3D * _outx, cTMap3D * _outy, cTMap3D *_outz ) {
        gux = _ux;
        guy = _uy;
        guz = _uz;

        int nx = gx;
        int ny = gy;
        int nz = gz;

        #pragma omp parallel for collapse(3)
        for( int i=0; i<nx+1; i++ ) {
            for( int j=0; j<ny; j++ ) {
                for( int k=0; k<nz; k++ ) {
                ux1->data[i][j][k] = ux->data[i][j][k];
                ux2->data[i][j][k] = (u_ref(1,i-1,j,k)+u_ref(1,i,j,k)+u_ref(1,i-1,j+1,k)+u_ref(1,i,j+1,k))/4.0;
                ux3->data[i][j][k] = (u_ref(2,i,j,k)+u_ref(2,i,j,k+1)+u_ref(2,i-1,j,k)+u_ref(2,i-1,j,j+1))/4.0;
        }}}

        #pragma omp parallel for collapse(3)
        for( int i=0; i<nx+1; i++ ) {
            for( int j=0; j<ny; j++ ) {
                for( int k=0; k<nz; k++ ) {
                uy1->data[i][j][k] = (u_ref(0,i,j-1,k)+u_ref(0,i,j,k)+u_ref(0,i+1,j,k)+u_ref(0,i+1,j-1,k))/4.0;
                uy2->data[i][j][k] = uy->data[i][j][k];
                uy3->data[i][j][k] = (u_ref(2,i,j-1,k+1)+u_ref(2,i,j,k)+u_ref(2,i,j,k+1)+u_ref(2,i,j-1,k))/4.0;
            }}}

        #pragma omp parallel for collapse(3)
        for( int i=0; i<nx+1; i++ ) {
            for( int j=0; j<ny; j++ ) {
                for( int k=0; k<nz; k++ ) {
                uz1->data[i][j][k] = (u_ref(0,i,j,k)+u_ref(0,i+1,j,k)+u_ref(0,i,j,k-1)+u_ref(0,i+1,j,k-1))/4.0;
                uz2->data[i][j][k] = (u_ref(1,i,j,k)+u_ref(1,i,j+1,k)+u_ref(1,i,j,k-1)+u_ref(1,i,j+1,k-1))/4.0;
                uz3->data[i][j][k] = uz->data[i][j][k];
            }}}

        // BackTrace X Flow
        semiLagrangian( _outx, _ux, gx+1, gy, gz, ux1,ux2,ux3 );

        // BackTrace Y Flow
        semiLagrangian( _outy, _uy, gx, gy+1,gz, uy1,uy2,uy3 );

        // BackTrace Z Flow
        semiLagrangian( _outz, _uz, gx, gy,gz+1, uz1,uz2,uz3 );
    }

    inline void advect_fluid() {

        advect_semiLagrangian( ux,uy,uz, ux_swap, uy_swap,uz_swap );

        int nx = gx;
        int ny = gy;
        int nz = gz;


        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
            for( int k=0; k<nz; k++ ) {
                ux->data[i][j][k] = ux_swap->data[i][j][k];
            }
        }}
        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
            for( int k=0; k<nz; k++ ) {
                uy->data[i][j][k] = uy_swap->data[i][j][k];
            }
        }}
        for( int i=0; i<nx; i++ ) {
            for( int j=0; j<ny; j++ ) {
            for( int k=0; k<nz; k++ ) {
                uz->data[i][j][k] = uz_swap->data[i][j][k];
            }
        }}
    }

    inline void setMaxDistOfLevelSet() {
    #if 0
        FLOAT max_vel = 0.0;
        FOR_EVERY_CELL(gn) {
            FLOAT xv = (u[0][i][j]+u[0][i+1][j])*0.5;
            FLOAT xu = (u[1][i][j]+u[1][i+1][j])*0.5;
            FLOAT vel = hypotf(xv,xu);
            if( vel > max_vel ) max_vel = vel;
        } END_FOR;
        maxdist = fmax(DIST, 1.5*DT*max_vel);
    #endif
    }



    inline void TimeStep(float dt)
    {
        m_scx = m_sxr/m_sx;
        m_scy = m_syr/m_sy;
        m_scz = m_szr/m_sz;

        m_DT= dt;
        // Mark Liquid Domain
            markLiquid();
            float av = 0.0;

            // Visualize Everything
            //render();
            // Add Gravity Force
            addGravity();
            // Compute Volume Error
            //computeVolumeError();

            // Solve Fluid
            enforce_boundary();
            comp_divergence();
            compute_pressure();
            subtract_pressure();
            comp_divergence();
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
                    //setMaxDistOfLevelSet();
                    redistance(maxdist);
                }
            }


    }

};

#endif // FLUID2P3D_H

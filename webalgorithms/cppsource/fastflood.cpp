#include <stdio.h>
#include "map.h"
#include "parallel.h"
#include "emscripten.h"

#define MAPMATH_PI 3.14159f
#define Drc data[r][c]
#define FOR_ROW_COL_MV(map) for(int r = 0; r < map->nrRows(); r++)\
    for (int c = 0; c < map->nrCols(); c++)\
    if(!pcr::isMV(map->data[r][c]))

#define FOR_ROW_COL(map) for(int r = 0; r < map->nrRows(); r++)\
    for (int c = 0; c < map->nrCols(); c++)\

#define SWAPMAP(x, y) do { cTMap * SWAP = x; x = y; y = SWAP; } while (0)

#define LDD4_VAL_LIST {0,1,2,3,4,5}
#define LDD_VAL_LIST {0,1,2,3,4,5,6,7,8,9}
#define LDD_X_LIST {0, -1, 0, 1, -1, 0, 1, -1, 0, 1}
#define LDD_DIR_LENGTH 10
#define LDD_Y_LIST {0, 1, 1, 1, 0, 0, 0, -1, -1, -1}
//these indices are not the final values, but we convert at the end to be consistent with the 8-dir ldd values
#define LDD4_X_LIST {0, -1, 0, 0, -1, 0}
#define LDD4_DIR_LENGTH 6
#define LDD4_Y_LIST {0, 0, 1, -1, 0, 0}
#define LDD_DIST_DIAG 1.41421356237f
#define LDD4_DIST 1.0f
#define LDD4_DIST_LIST {1.0f,LDD_DIST, LDD_DIST, LDD_DIST,LDD_DIST, 1.0f}
#define LDD_DIST 1.0f
#define LDD_DIST_LIST {1.0f,LDD_DIST_DIAG, LDD_DIST, LDD_DIST_DIAG,LDD_DIST, 1.0f, LDD_DIST,LDD_DIST_DIAG, LDD_DIST, LDD_DIST_DIAG }
#define LDD_PIT 5
#define LDD_FLAT 0
#define LDD4_PIT 5
#define LDD4_FLAT 0
#define LDD_MAKE_TEMP(x) (x + LDD_DIR_LENGTH)
#define LDD_MAKE_ACTUAL(x) (x - LDD_DIR_LENGTH)
#define LDD_IS_ACTUAL(x) ((x> 0 && x < LDD_DIR_LENGTH) ? true : false)
#define LDD_IS_TEMP(x) (!(x < LDD_DIR_LENGTH) ? true : false)
#define INSIDE(map, r, c) (r>=0 && r<map->data.nr_rows() && c>=0 && c<map->data.nr_cols())
// check if cell From flows to To
#define FLOWS_TO(ldd, rFrom, cFrom, rTo, cTo) \
    ( ldd != 0 && rFrom >= 0 && cFrom >= 0 && rFrom+dy[ldd]==rTo && cFrom+dx[ldd]==cTo )
#define LDD4_MAKE_TEMP(x) (x + LDD4_DIR_LENGTH)
#define LDD4_MAKE_ACTUAL(x) (x - LDD4_DIR_LENGTH)
#define LDD4_IS_ACTUAL(x) ((x> 0 && x < LDD4_DIR_LENGTH) ? true : false)
#define LDD4_IS_TEMP(x) (!(x < LDD4_DIR_LENGTH) ? true : false)
#define INSIDE4(map, r, c) (r>=0 && r<map->data.nr_rows() && c>=0 && c<map->data.nr_cols())
// check if cell From flows to To
#define FLOWS4_TO(ldd, rFrom, cFrom, rTo, cTo) \
    ( ldd != 0 && rFrom >= 0 && cFrom >= 0 && rFrom+dy[ldd]==rTo && cFrom+dx[ldd]==cTo )
/// linked list structure for network in kin wave

int main() {
  printf("Loaded wasm module for fastflood algorithm\n");
  return 0;
}

class cTMapFV
{
	float * m_datav;
	int m_width;
	int m_height;
	double m_dx;
	double m_north;
	double m_west;
	std::string m_CRS;
	
	public:
	
	inline cTMapFV(float * data, int w, int h, double dx, double north, double west, std::string CRS)
	{
		m_datav = data;
		m_width = w;
		m_height = h;
		m_dx = dx;
		m_north = north;
		m_west = west;
		m_CRS =CRS;
		
		
	}
	
	float* operator[](int index)
	{
		return &(m_datav[index * m_width]);
	}
	
	inline int nrCols()
	{
		return m_width;
	}
	inline int nrRows()
	{
		return m_height;
	}
	
	int nr_rows()
	{
		return m_height;
	}
	
	int nr_cols()
	{
		return m_width;
	}
	
};





#include "opencv2/opencv.hpp"
void EMSCRIPTEN_KEEPALIVE  BilateralFilter(float * res, float * in_map, int w, int h, float in_dx, int size,  float sigma_space, float sigma_val)
{

	cTMap * Map = new cTMap(in_map,w,h,0.0,0.0, in_dx,- in_dx);
    cv::Mat original_ = cv::Mat(Map->nrRows(),Map->nrCols(), CV_32F,0.0);

    for(int r = 0; r < Map->nrRows(); r++)
    {
        for(int c = 0; c < Map->nrCols(); c++)
        {
            if(!pcr::isMV(Map->data[r][c]))
            {
                original_.at<float>(r,c) = Map->data[r][c];
            }else
            {
                original_.at<float>(r,c) = 0.0;
            }
        }
    }


    cv::Mat result_;
    bilateralFilter(original_,result_,size,sigma_val,sigma_space);



    for(int r = 0; r < Map->nrRows(); r++)
    {
        for(int c = 0; c < Map->nrCols(); c++)
        {
            float val = result_.at<float>(r,c);
			
			int index = r* Map->Cols() + c;
			if(!pcr::isMV(val))
			{
				
				res[index] = val;
			}else{
					
				pcr::setMV(res[index]);
			}
		
        }
    }


}



extern "C"
{

#define RC_INDEX(r,c) ((r)*(w) + (c))
void EMSCRIPTEN_KEEPALIVE do_algo_hillshade(float*ret,float*data, int w, int h, float dx, float scale)
{
	cTMapFV RES(ret,w,h,dx,0.0,0.0,"");
	cTMapFV DEM(data,w,h,dx,0.0,0.0,"");
	
	
    float _dx =dx;
    float _dy =dx;

    float maxDem = -1e31;
    float minDem = 1e31;

    int n_cells = 0;

    for(int r = 0; r < h; r++)
	{
		for(int c = 0; c < w; c++)
		{
			if(!(DEM[r][c]<-1e30f))
			{
			
				n_cells++;

				float mat0 = DEM[r][c];
				float mat1 = mat0;
				float mat2 = mat0;
				float mat3 = mat0;
				float mat4 = mat0;
				float mat5 = mat0;
				float mat6 = mat0;
				float mat7 = mat0;
				float mat8 = mat0;
				float mat9 = mat0;
				float dx, dy;//, aspect;
				float factor = 1.0;

				minDem = std::min(DEM[r][c], minDem);
				maxDem = std::max(DEM[r][c], maxDem);

				if (r > 0 && r < DEM.nrRows()-1 && c > 0 && c < DEM.nrCols()-1)
				{
					if(!(DEM[r-1][c-1]<-1e30f))
					{
						mat0 = DEM[r-1][c-1];
					}
					if(!(DEM[r-1][c  ]<-1e30f))
					{
						mat1 = DEM[r-1][c  ];
					}
					if(!(DEM[r-1][c+1]<-1e30f))
					{
						mat2 = DEM[r-1][c+1];
					}
					if(!(DEM[r  ][c-1]<-1e30f))
					{
						mat3 = DEM[r  ][c-1];
					}

					if(!(DEM[r  ][c+1]<-1e30f))
					{
						mat5 = DEM[r  ][c+1];
					}
					if(!(DEM[r+1][c-1]<-1e30f))
					{
						mat6 = DEM[r+1][c-1];
					}
					if(!(DEM[r+1][c  ]<-1e30f))
					{
						mat7 = DEM[r+1][c  ];
					}
					if(!(DEM[r+1][c+1]<-1e30f))
					{
						mat8 = DEM[r+1][c+1];
					}
				}

				dx = (mat2 + 2*mat5 + mat8 - mat0 -2*mat3 - mat6)/(8*_dx);
				dy = (mat0 + 2*mat1 + mat2 - mat6 -2*mat7 - mat8)/(8*_dx);

				//http://edndoc.esri.com/arcobjects/9.2/net/shared/geoprocessing/spatial_analyst_tools/how_hillshade_works.htm
				//Burrough, P. A. and McDonell, R.A., 1998. Principles of Geographical Information Systems (Oxford University Press, New York), p. 190.
				float z_factor = 2.0;
				float Slope_rad = atan( z_factor * sqrt ( dx*dx+dy*dy) );
				float Aspect_rad = 0;
				if( dx != 0)
				{
					Aspect_rad = atan2(dy, -dx);
					if (Aspect_rad < 0)
						Aspect_rad = 2.0f*MAPMATH_PI + Aspect_rad;
				}
				else
				{
					if(dy > 0)
					{
						Aspect_rad = MAPMATH_PI/2.0f;
					}
					else
					{
						Aspect_rad = 2.0f*MAPMATH_PI - MAPMATH_PI/2.0f;
					}
				}
				float Zenith_rad = 70.0f * MAPMATH_PI / 180.0f;
				float Azimuth_rad = 130.0f * MAPMATH_PI / 180.0f;
				RES[r][c] = 255.0f * ( ( cos(Zenith_rad) * cos(Slope_rad) ) + ( sin(Zenith_rad) * sin(Slope_rad) * cos(Azimuth_rad - Aspect_rad) ) );
			}

		}
	}
	float MaxV = 0.0;
	float MinV = 0.0;
	bool valfound = false;
	for(int r = 0; r < DEM.nr_rows();r++)
	{
		for(int c = 0; c < DEM.nr_cols();c++)
		{
			if(!(DEM[r][c]<-1e30f))
			{
				if(!valfound)
				{
					MinV = RES[r][c];
					MaxV = RES[r][c];
					valfound = true;
				}else{
					MinV = std::min(MinV,RES[r][c]);
					MaxV = std::max(MaxV,RES[r][c]);
				}
				
			}
		}
	}
	
	printf("Hillshade cal min: %f  \n", MinV);
	printf("Hillshade cal max: %f  \n", MaxV);

	for(int r = 0; r < h; r++)
	{
		for(int c = 0; c < w; c++)
		{
			RES[r][c] = (RES[r][c]-MinV)/(MaxV-MinV);
			RES[r][c] = 0.8f*RES[r][c]+0.2f*(DEM[r][c] - minDem)/(maxDem-minDem);
		}
	}

	for(int r = 0; r < DEM.nr_rows();r++)
	{
		for(int c = 0; c < DEM.nr_cols();c++)
		{
			if((DEM[r][c]<-1e30f))
			{
				pcr::setMV(RES[r][c]);
			}
			
		}
	}

}
	
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
    int r_lowestnbconnect = -1;
    int c_lowestnbconnect = -1;

    int pi_flowinto = -1;

    bool mvnb;

}  LDD_PITPROPERTIES;

static inline bool OUTORMV(cTMap * LDD, int r, int c)
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

static const int UF_DIRECTION_X = 1;
static const int UF_DIRECTION_Y = 2;
static const int UF_DIRECTION_XY = 3;
static const int UF_DERIVATIVE_LR = 0;
static const int UF_DERIVATIVE_L = 1;
static const int UF_DERIVATIVE_R = 2;

static bool UF_OUTORMV(cTMap * map, int r, int c)
{
    if(r > -1 && c > -1 && r < map->nrRows() && c < map->nrCols())
    {
        return pcr::isMV(map->data[r][c]);
    }
    return true;
}
static double UF2D_Derivative(cTMap * _dem, int r, int c, int direction,double scale, int calculationside)
{
    cTMap * _in = _dem;
    float _dx = _dem->cellSize();
    if(UF_OUTORMV(_dem,r,c))
    {
        return 0;
    }
    if(direction == UF_DIRECTION_X)
    {
        if(calculationside == UF_DERIVATIVE_LR)
        {

            double dx1 = !(UF_OUTORMV(_dem,r,c+1))? _in->data[r][c+1] -_in->Drc :0.0;
            double dx2 = !(UF_OUTORMV(_dem,r,c-1))? _in->Drc - _in->data[r][c-1] :0.0;
            return (dx1 + dx2)/(2.0*_dx);

        }else if(calculationside == UF_DERIVATIVE_L)
        {
            return (!UF_OUTORMV(_dem,r,c-1))? (_in->Drc -_in->data[r][c-1])/_dx :0.0;

        }else if(calculationside == UF_DERIVATIVE_R)
        {
            return (!UF_OUTORMV(_dem,r,c+1))? (_in->data[r][c+1] -_in->Drc)/_dx :0.0;

        }
    }
    if(direction == UF_DIRECTION_Y)
    {
        if(calculationside == UF_DERIVATIVE_LR)
        {
            double dy1 = (!UF_OUTORMV(_dem,r+1,c))? _in->data[r+1][c] -_in->Drc :0.0;
            double dy2 = (!UF_OUTORMV(_dem,r-1,c))? _in->Drc - _in->data[r-1][c] :0.0;
            return (dy1 + dy2)/(2.0*_dx);

        }else if(calculationside == UF_DERIVATIVE_L)
        {
            return (!UF_OUTORMV(_dem,r-1,c))? (_in->Drc - _in->data[r-1][c])/_dx :0.0;

        }else if(calculationside == UF_DERIVATIVE_R)
        {
            return (!UF_OUTORMV(_dem,r+1,c))? (_in->data[r+1][c] -_in->Drc)/_dx :0.0;
        }
    }
    return 0;
}


static double UF2D_Derivative_scaled(cTMap * _dem, int r, int c, int direction,double scale, int calculationside)
{
    cTMap * _in = _dem;
    float _dx = _dem->cellSize();
    if(UF_OUTORMV(_dem,r,c))
    {
        return 0;
    }
    if(direction == UF_DIRECTION_X)
    {
        if(calculationside == UF_DERIVATIVE_LR)
        {

            double dx1 = !(UF_OUTORMV(_dem,r,c+1))? scale *_in->data[r][c+1] -scale *_in->Drc :0.0;
            double dx2 = !(UF_OUTORMV(_dem,r,c-1))? scale *_in->Drc - scale *_in->data[r][c-1] :0.0;
            return (dx1 + dx2)/(2.0*_dx);

        }else if(calculationside == UF_DERIVATIVE_L)
        {
            return (!UF_OUTORMV(_dem,r,c-1))? (scale *_in->Drc -scale *_in->data[r][c-1])/_dx :0.0;

        }else if(calculationside == UF_DERIVATIVE_R)
        {
            return (!UF_OUTORMV(_dem,r,c+1))? (scale *_in->data[r][c+1] -scale *_in->Drc)/_dx :0.0;

        }
    }
    if(direction == UF_DIRECTION_Y)
    {
        if(calculationside == UF_DERIVATIVE_LR)
        {
            double dy1 = (!UF_OUTORMV(_dem,r+1,c))? scale *_in->data[r+1][c] -scale *_in->Drc :0.0;
            double dy2 = (!UF_OUTORMV(_dem,r-1,c))? scale *_in->Drc - scale *_in->data[r-1][c] :0.0;
            return (dy1 + dy2)/(2.0*_dx);

        }else if(calculationside == UF_DERIVATIVE_L)
        {
            return (!UF_OUTORMV(_dem,r-1,c))? (scale *_in->Drc - scale *_in->data[r-1][c])/_dx :0.0;

        }else if(calculationside == UF_DERIVATIVE_R)
        {
            return (!UF_OUTORMV(_dem,r+1,c))? (scale *_in->data[r+1][c] -scale *_in->Drc)/_dx :0.0;
        }
    }
    return 0;

}

static double UF2D_Derivative2(cTMap * _dem, int r, int c, int direction, int calculationside, double dx)
{
    cTMap * _in = _dem;
    if(dx == 0)
    {
        dx = _dem->cellSize();
    }
    if(UF_OUTORMV(_dem,r,c))
    {
        return 0;
    }
    if(direction == UF_DIRECTION_X)
    {
        double x = _in->Drc;
        double x1 = !UF_OUTORMV(_dem,r,c+1)? _in->data[r][c+1] : x;
        double x2 = !UF_OUTORMV(_dem,r,c-1)? _in->data[r][c-1] : x;
        double x11 = !UF_OUTORMV(_dem,r,c+2)? _in->data[r][c+2] : x;
        double x22 = !UF_OUTORMV(_dem,r,c-2)? _in->data[r][c-2] : x;
        return (-x11 + 16.0*x1 -  30.0*x + 16.0*x2 - x22)/(12.0*dx*dx);
    }
    if(direction == UF_DIRECTION_Y)
    {
        double y = _in->Drc;
        double y1 = !UF_OUTORMV(_dem,r+1,c)? _in->data[r+1][c] : y;
        double y2 = !UF_OUTORMV(_dem,r-1,c)? _in->data[r-1][c] : y;
        double y11 = !UF_OUTORMV(_dem,r+2,c)? _in->data[r+2][c] : y;
        double y22 = !UF_OUTORMV(_dem,r-2,c)? _in->data[r-2][c] : y;
        return (-y11 + 16.0*y1 -  30.0*y + 16.0*y2 - y22)/(12.0*dx*dx);
    }
    if(direction == UF_DIRECTION_XY)
    {
        double xy = _in->Drc;
        double xy1 = !UF_OUTORMV(_dem,r+1,c+1)? _in->data[r+1][c+1] : xy;
        double xy2 = !UF_OUTORMV(_dem,r+1,c-1)? _in->data[r+1][c-1] : xy;
        double xy3 = !UF_OUTORMV(_dem,r-1,c+1)? _in->data[r-1][c+1] : xy;
        double xy4 = !UF_OUTORMV(_dem,r-1,c-1)? _in->data[r-1][c-1] : xy;

        return (xy1 - xy2 - xy3 + xy4)/(4.0*dx*dx);
    }
    return 0;

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



static cTMap * AS_Slope(cTMap * Other)
{

    //MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(nullptr, Other->data.nr_cols(), Other->data.nr_rows(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());//std::move(raster_data),"","");

    float dx = Other->cellSize();

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                //calculate slope based on neighbour cell values
                //use a 3x3 kernel to estimate average slope

                float x_11 = GetMapValue_OUTORMV3x3Av(Other,r-1,c-1);
                float x_12 = GetMapValue_OUTORMV3x3Av(Other,r-1,c);
                float x_13 = GetMapValue_OUTORMV3x3Av(Other,r-1,c+1);
                float x_21 = GetMapValue_OUTORMV3x3Av(Other,r,c-1);
                float x_22 = GetMapValue_OUTORMV3x3Av(Other,r,c);
                float x_23 = GetMapValue_OUTORMV3x3Av(Other,r,c+1);
                float x_31 = GetMapValue_OUTORMV3x3Av(Other,r+1,c-1);
                float x_32 = GetMapValue_OUTORMV3x3Av(Other,r+1,c);
                float x_33 = GetMapValue_OUTORMV3x3Av(Other,r+1,c+1);

                float dzdx = (x_13 + 2.0f * x_23 + x_33 - x_11 - 2.0f* x_21 - x_31)/(8.0f*dx);
                float dzdy = (x_31 + 2.0f * x_32 + x_33 - x_11 - 2.0f * x_12 - x_13)/(8.0f*dx);

                float slope = std::sqrt(dzdx*dzdx + dzdy*dzdy);

                map->data[r][c] = slope;

            }
        }
    }
    return map;

}


static cTMap * AS_Aspect(cTMap * dem)
{
    //MaskedRaster<float> raster_data(dem->data.nr_rows(), dem->data.nr_cols(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());
    cTMap *map = new cTMap(nullptr,  dem->data.nr_cols(),dem->data.nr_rows(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());//std::move(raster_data),dem->projection(),"");


    float demVal; /* value in the dem.map */
        int r, c, nrRows, nrCols;

        nrRows = dem->nrRows();
        nrCols = dem->nrCols();

        /* For every cell in the dem map calculate the orient. */
        for (r = 0; r < nrRows; r++) {
            for (c = 0; c < nrCols; c++) {
                if (!pcr::isMV(dem->data[r][c])) {
                    float Dx, Dy;

                    Dx = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_LR);
                    Dy = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_LR);

                    if (Dx == 0) /* exception value */
                    {
                        if (Dy == 0) /* Flat -> -1 */
                            map->data[r][c] = -1.0;
                        if (Dy > 0) /* north: 0 DEGREES */
                            map->data[r][c] = 0.0;
                        if (Dy < 0) /* south: 180 DEGREES */
                            map->data[r][c] = M_PI;
                    } else {
                        if (Dy == 0) {
                            float orientVal = M_PI / (float)2;
                            if (Dx < 0) /* west */
                            {           /* 270 DEG */
                                orientVal *= 3;
                                map->data[r][c]= orientVal;
                            } else /* east 90 DEG */
                                map->data[r][c] = orientVal;
                        } else {
                            float A = atan(Dx / Dy);
                            if (Dy < 0)
                                A += M_PI;
                            if (Dy > 0 && Dx < 0)
                                A += 2 * M_PI;
                            map->data[r][c] = A;
                        }
                    }
                } else
                    pcr::setMV(map->data[r][c]);
            }
        }

    return map;
}
static void ZevenbergenGrid(float *Z, cTMap *dem, int r, int c)
{
    Z[1] = GetMapValue_AvRepl(dem, r - 1, c - 1);
    Z[2] = GetMapValue_AvRepl(dem, r - 1, c);
    Z[3] = GetMapValue_AvRepl(dem, r - 1, c + 1);
    Z[4] = GetMapValue_AvRepl(dem, r, c - 1);
    Z[5] = GetMapValue_AvRepl(dem, r, c);
    Z[6] = GetMapValue_AvRepl(dem, r, c + 1);
    Z[7] = GetMapValue_AvRepl(dem, r + 1, c - 1);
    Z[8] = GetMapValue_AvRepl(dem, r + 1, c);
    Z[9] = GetMapValue_AvRepl(dem, r + 1, c + 1);
}


static void PCR_CurvStuff(float dx,float *D, float *E, float *F, float *G, float *H, const float *Z)
{
    float L = dx;

    *D = ((Z[4] + Z[6]) / 2 - Z[5]) / std::sqrt(L);
    *E = ((Z[2] + Z[8]) / 2 - Z[5]) / std::sqrt(L);
    *F = (-Z[1] + Z[3] + Z[7] - Z[9]) / (4 * std::sqrt(L));
    *G = (-Z[4] + Z[6]) / (2 * L);
    *H = (Z[2] - Z[8]) / (2 * L);
}



inline cTMap * AS_ProfileCurvature(cTMap * dem)
{
    //MaskedRaster<float> raster_data(dem->data.nr_rows(), dem->data.nr_cols(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());
    cTMap *map = new cTMap(nullptr, dem->data.nr_cols(), dem->data.nr_rows(),dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());//std::move(raster_data),dem->projection(),"");


    float val;
     int nrRows, nrCols, r, c;

     nrRows = dem->nrRows();
     nrCols = dem->nrCols();

    //#pragma omp parallel for collapse(2)
     for (r = 0; r < nrRows; r++)
     {
             for (c = 0; c < nrCols; c++)
             {

                 if (!pcr::isMV(dem->data[r][c]))
                 {

                     float Z[10], D, E, F, G, H;


                     ZevenbergenGrid(Z, dem, r, c);
                     PCR_CurvStuff(dem->cellSizeX(),&D, &E, &F, &G, &H, Z);
                     if (G == 0 && H == 0)
                     {
                         val = 0;
                     }
                     else
                     {
                         val = -2 * (D * std::sqrt(std::fabs(G)) + E * std::sqrt(std::fabs(H)) + F * G * H) / (std::sqrt(std::fabs(G)) + std::sqrt(std::fabs(H)));
                     }
                     map->data[r][c] = val;
                 } else
                 {
                     pcr::setMV(map->data[r][c]);
                 }
             }
     }
    return map;
}

inline cTMap * AS_PlanarCurvature(cTMap * dem)
{
    //MaskedRaster<float> raster_data(dem->data.nr_rows(), dem->data.nr_cols(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());
    cTMap *map = new cTMap(nullptr, dem->data.nr_cols(), dem->data.nr_rows(),dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());//std::move(raster_data),dem->projection(),"");


    float val; /* value read in dem.map */
        int nrRows, nrCols, r, c;

        nrRows = dem->nrRows();
        nrCols = dem->nrCols();

        /* For every cell in the dem map calculate the curv. */
        //#pragma omp parallel for collapse(2)
        for (r = 0; r < nrRows; r++)
        {
            for (c = 0; c < nrCols; c++)
            {
                if(!pcr::isMV(dem->data[r][c])) {

                    float Z[10], D, E, F, G, H;


                    ZevenbergenGrid(Z, dem, r, c);
                    PCR_CurvStuff(dem->cellSizeX(),&D, &E, &F, &G, &H, Z);
                    if (G == 0 && H == 0)
                    {
                        val = 0;
                    }
                    else
                    {
                        val = 2 * ((D) * std::sqrt(std::fabs(H)) + E * std::sqrt(std::fabs(G)) - F * G * H) / (std::sqrt(std::fabs(G)) + std::sqrt(std::fabs(H)));
                    }
                    map->data[r][c] = val;
                } else
                {
                    pcr::setMV(map->data[r][c]);
                }
            }
        }
        return map;
}
inline cTMap * AS_SlopeX(cTMap * dem)
{
    //MaskedRaster<float> raster_data(dem->data.nr_rows(), dem->data.nr_cols(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());
    cTMap *map = new cTMap(nullptr, dem->data.nr_cols(), dem->data.nr_rows(),dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());//std::move(raster_data),dem->projection(),"");


    float demVal; /* value in the dem.map */
        int r, c, nrRows, nrCols;

        nrRows = dem->nrRows();
        nrCols = dem->nrCols();

        /* For every cell in the dem map calculate the orient. */
        //#pragma omp parallel for collapse(2)
        for (r = 0; r < nrRows; r++) {
            for (c = 0; c < nrCols; c++) {
                if (!pcr::isMV(dem->data[r][c])) {
                    float Dx, Dy;

                    Dx = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_LR);
                    //Dy = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_LR);

                    map->data[r][c] = Dx;
                } else
                    pcr::setMV(map->data[r][c]);
            }
        }

    return map;
}

inline cTMap * AS_SlopeX1(cTMap * dem)
{
	
    //MaskedRaster<float> raster_data(dem->data.nr_rows(), dem->data.nr_cols(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());
    //std::cout << "raster_data " << &raster_data << " " << raster_data.nr_cols() << " " << raster_data.nr_rows() << std::endl;

	
	cTMap *map = new cTMap( nullptr, dem->data.nr_cols(),dem->data.nr_rows(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());//std::move(raster_data),dem->projection(),"");

    float demVal; /* value in the dem.map */
        int r, c, nrRows, nrCols;

        nrRows = dem->nrRows();
        nrCols = dem->nrCols();

        /* For every cell in the dem map calculate the orient. */
        //#pragma omp parallel for collapse(2)
        for (r = 0; r < nrRows; r++) {
            for (c = 0; c < nrCols; c++) {
				
				
                if (!pcr::isMV(dem->data[r][c])) {
                    float Dx, Dy;

                    Dx = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_L);
                    //Dy = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_LR);

					
					map->data[r][c] = Dx;
                } else
					pcr::setMV(map->data[r][c]);
            }
        }

    return map;
}

inline cTMap * AS_SlopeX2(cTMap * dem)
{
    //MaskedRaster<float> raster_data(dem->data.nr_rows(), dem->data.nr_cols(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());
    cTMap *map = new cTMap(nullptr, dem->data.nr_cols(), dem->data.nr_rows(),dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());//std::move(raster_data),dem->projection(),"");


    float demVal; /* value in the dem.map */
        int r, c, nrRows, nrCols;

        nrRows = dem->nrRows();
        nrCols = dem->nrCols();

        /* For every cell in the dem map calculate the orient. */
        //#pragma omp parallel for collapse(2)
        for (r = 0; r < nrRows; r++) {
            for (c = 0; c < nrCols; c++) {
                if (!pcr::isMV(dem->data[r][c])) {
                    float Dx, Dy;

                    Dx = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_R);
                    //Dy = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_LR);

                    map->data[r][c] = Dx;
                } else
                    pcr::setMV(map->data[r][c]);
            }
        }

    return map;
}




inline cTMap * AS_SlopeY(cTMap *dem)
{
    //MaskedRaster<float> raster_data(dem->data.nr_rows(), dem->data.nr_cols(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());
    cTMap *map = new cTMap(nullptr, dem->data.nr_cols(), dem->data.nr_rows(),dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());//std::move(raster_data),dem->projection(),"");

    float demVal; /* value in the dem.map */
        int r, c, nrRows, nrCols;

        nrRows = dem->nrRows();
        nrCols = dem->nrCols();

        /* For every cell in the dem map calculate the orient. */
        //#pragma omp parallel for collapse(2)
        for (r = 0; r < nrRows; r++) {
            for (c = 0; c < nrCols; c++) {
                if (!pcr::isMV(dem->data[r][c])) {
                    float Dx, Dy;

                    //Dx = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_LR);
                    Dy = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_LR);

                    map->data[r][c] = Dy;
                } else
                    pcr::setMV(map->data[r][c]);
            }
        }

    return map;
}




inline cTMap * AS_SlopeY1(cTMap *dem)
{
    cTMap *map = new cTMap( nullptr, dem->data.nr_cols(),dem->data.nr_rows(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());//std::move(raster_data),dem->projection(),"");

    float demVal; /* value in the dem.map */
        int r, c, nrRows, nrCols;

        nrRows = dem->nrRows();
        nrCols = dem->nrCols();

        /* For every cell in the dem map calculate the orient. */
        //#pragma omp parallel for collapse(2)
        for (r = 0; r < nrRows; r++) {
            for (c = 0; c < nrCols; c++) {
                if (!pcr::isMV(dem->data[r][c])) {
                    float Dx, Dy;

                    //Dx = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_LR);
                    Dy = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_L);

                    map->data[r][c] = Dy;
                } else
                    pcr::setMV(map->data[r][c]);
            }
        }

    return map;
}





inline cTMap * AS_SlopeY2(cTMap *dem)
{
    cTMap *map = new cTMap( nullptr, dem->data.nr_cols(),dem->data.nr_rows(), dem->data.north(), dem->data.west(), dem->data.cell_size(),dem->data.cell_sizeY());//std::move(raster_data),dem->projection(),"");

    float demVal; /* value in the dem.map */
        int r, c, nrRows, nrCols;

        nrRows = dem->nrRows();
        nrCols = dem->nrCols();

        /* For every cell in the dem map calculate the orient. */
        //#pragma omp parallel for collapse(2)
        for (r = 0; r < nrRows; r++) {
            for (c = 0; c < nrCols; c++) {
                if (!pcr::isMV(dem->data[r][c])) {
                    float Dx, Dy;

                    //Dx = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_LR);
                    Dy = UF2D_Derivative_scaled(dem,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_R);

                    map->data[r][c] = Dy;
                } else
                    pcr::setMV(map->data[r][c]);
            }
        }

    return map;
}


inline float AS_MapMinimumRed(cTMap * Other)
{
    float minv = 1e31;
    double n = 0.0;
    bool found = false;

    //#pragma omp parallel for shared(found) collapse(2) reduction(min:minv)
    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
            }else
            {
                minv = std::min(minv,Other->data[r][c]);
                found = true;

            }
        }
    }

    if(found)
    {
        return minv;
    }else {
        return minv;
    }


}

inline float AS_MapMaximumRed(cTMap * Other)
{
    float maxv = -1e31;
    double n = 0.0;
    bool found = false;

    //#pragma omp parallel for collapse(2) shared(found) reduction(max:maxv)
    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
            }else
            {
                maxv = std::max(maxv,Other->data[r][c]);
                found = true;

            }
        }
    }

    if(found)
    {
        return maxv;
    }else {
        return maxv;
    }

}


inline cTMap * AS_SpreadMonotonicReconstruct(cTMap * DEM, float delta,int iter_max = 0)
{

    //MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
    cTMap *map = new cTMap(nullptr,DEM->data.nr_cols(),DEM->data.nr_rows(),  DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());//std::move(raster_data),DEM->projection(),"");


    //initialize the map with friction values
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(pcr::isMV(DEM->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else {
                map->data[r][c] = 1e31;
            }
        }
    }


    float dx = map->cellSize();

    //we keep iterating through this algorithm untill there is no change left to make
    bool change = true;
    bool first = true;

    int iter = 0;

    while(change && ((iter_max <= 0) || (iter_max > 0 && iter < iter_max)))
    {
        iter ++;
        change = false;

        if(iter%2 == 0)
        {
            //first we move in right-lower direction
            for(int r = 0; r < map->data.nr_rows();r++)
            {
                for(int c = 0; c < map->data.nr_cols();c++)
                {
                    float v_points = DEM->data[r][c];

                    if(!pcr::isMV(v_points))
                    {
                        if((r-1 > -1))
                        {
                            float vn_points = DEM->data[r-1][c];
                            if(pcr::isMV(vn_points))
                            {
                                map->data[r][c] = DEM->data[r][c];
                            }else
                            {
                                float demh;

                                    demh = std::max(map->data[r-1][c] + delta,DEM->data[r][c]);

                                float mapn = std::min(demh,map->data[r][c]);
                                if(mapn < map->data[r][c])
                                {
                                    change = true;
                                    map->data[r][c] = mapn;
                                }
                            }
                        }else
                        {
                            map->data[r][c] = DEM->data[r][c];
                        }

                        if((c-1 > -1))
                        {
                            float vn_points = DEM->data[r][c-1];
                            if(pcr::isMV(vn_points))
                            {
                                map->data[r][c] = DEM->data[r][c];
                            }else
                            {
                                float demh;

                                    demh = std::max(map->data[r][c-1]+ delta,DEM->data[r][c]);

                                float mapn = std::min(demh,map->data[r][c]);
                                if(mapn < map->data[r][c])
                                {
                                    change = true;
                                    map->data[r][c] = mapn;
                                }
                            }
                        }else
                        {
                            map->data[r][c] = DEM->data[r][c];
                        }
                    }
                }
            }

            //then we move in left-upper direction
            for(int r = map->data.nr_rows()-1; r > -1 ;r--)
            {
                for(int c = map->data.nr_cols()-1; c > -1 ;c--)
                {
                    float v_points = DEM->data[r][c];

                    if(!pcr::isMV(v_points))
                    {
                        if((r+1 < map->data.nr_rows()))
                        {
                            float vn_points = DEM->data[r+1][c];
                            if(pcr::isMV(vn_points))
                            {
                                map->data[r][c] = DEM->data[r][c];
                            }else
                            {
                                float demh;

                                    demh = std::max(map->data[r+1][c]+ delta,DEM->data[r][c]);

                                float mapn = std::min(demh,map->data[r][c]);
                                if(mapn < map->data[r][c])
                                {
                                    change = true;
                                    map->data[r][c] = mapn;
                                }
                            }
                        }else
                        {
                            map->data[r][c] = DEM->data[r][c];
                        }

                        if((c+1 < map->data.nr_cols()))
                        {
                            float vn_points = DEM->data[r][c+1];
                            if(pcr::isMV(vn_points))
                            {
                                map->data[r][c] = DEM->data[r][c];
                            }else
                            {
                                float demh;

                                    demh = std::max(map->data[r][c+1]+ delta,DEM->data[r][c]);

                                float mapn = std::min(demh,map->data[r][c]);
                                if(mapn < map->data[r][c])
                                {
                                    change = true;
                                    map->data[r][c] = mapn;
                                }
                            }
                        }else
                        {
                            map->data[r][c] = DEM->data[r][c];
                        }
                    }

                }

            }
        }else
            {
                //first we move in right-lower direction
                for(int r = 0; r < map->data.nr_rows();r++)
                {
                    for(int c = map->data.nr_cols()-1; c > -1 ;c--)
                    {
                        float v_points = DEM->data[r][c];

                        if(!pcr::isMV(v_points))
                        {
                            if((r-1 > -1))
                            {
                                float vn_points = DEM->data[r-1][c];
                                if(pcr::isMV(vn_points))
                                {
                                    map->data[r][c] = DEM->data[r][c];
                                }else
                                {
                                    float demh;

                                        demh = std::max(map->data[r-1][c]+ delta,DEM->data[r][c]);

                                    float mapn = std::min(demh,map->data[r][c]);
                                    if(mapn < map->data[r][c])
                                    {
                                        change = true;
                                        map->data[r][c] = mapn;
                                    }
                                }
                            }else
                            {
                                map->data[r][c] = DEM->data[r][c];
                            }


                            if((c+1 < map->data.nr_cols()))
                            {
                                float vn_points = DEM->data[r][c+1];
                                if(pcr::isMV(vn_points))
                                {
                                    map->data[r][c] = DEM->data[r][c];
                                }else
                                {
                                    float demh;

                                        demh = std::max(map->data[r][c+1]+ delta,DEM->data[r][c]);

                                    float mapn = std::min(demh,map->data[r][c]);
                                    if(mapn < map->data[r][c])
                                    {
                                        change = true;
                                        map->data[r][c] = mapn;
                                    }
                                }
                            }else
                            {
                                map->data[r][c] = DEM->data[r][c];
                            }
                        }
                    }
                }


                //then we move in left-upper direction
                for(int r = map->data.nr_rows()-1; r > -1 ;r--)
                {
                    for(int c = 0; c < map->data.nr_cols();c++)
                    {
                        float v_points = DEM->data[r][c];

                        if(!pcr::isMV(v_points))
                        {
                            if((r+1 < map->data.nr_rows()))
                            {
                                float vn_points = DEM->data[r+1][c];
                                if(pcr::isMV(vn_points))
                                {
                                    map->data[r][c] = DEM->data[r][c];
                                }else
                                {
                                    float demh;

                                        demh = std::max(map->data[r+1][c]+ delta,DEM->data[r][c]);

                                    float mapn = std::min(demh,map->data[r][c]);
                                    if(mapn < map->data[r][c])
                                    {
                                        change = true;
                                        map->data[r][c] = mapn;
                                    }
                                }
                            }else
                            {
                                map->data[r][c] = DEM->data[r][c];
                            }

                            if((c-1 > -1))
                            {
                                float vn_points = DEM->data[r][c-1];
                                if(pcr::isMV(vn_points))
                                {
                                    map->data[r][c] = DEM->data[r][c];
                                }else
                                {
                                    float demh;


                                        demh = std::max(map->data[r][c-1]+ delta,DEM->data[r][c]);

                                    float mapn = std::min(demh,map->data[r][c]);
                                    if(mapn < map->data[r][c])
                                    {
                                        change = true;
                                        map->data[r][c] = mapn;
                                    }
                                }
                            }else
                            {
                                map->data[r][c] = DEM->data[r][c];
                            }
                        }

                    }
            }


        }


        first = false;

    }

    return map;

}


inline cTMap * AS_Clump(cTMap * Other, float threshold = 0.0, bool adjust_threshold = false)
{
  
    //MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(nullptr, Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());//std::move(raster_data),Other->projection(),"");

    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {
        for(int co = 0; co < map->data.nr_cols();co++)
        {
            pcr::setMV(map->data[ro][co]);
        }
    }

    int classn = 1;
    float val = 0.0;

    for(int ro = 0; ro < map->data.nr_rows();ro++)
    {
        for(int co = 0; co < map->data.nr_cols();co++)
        {


            if(!pcr::isMV(Other->data[ro][co]) && pcr::isMV(map->data[ro][co]))
            {
                {
                    LDD_LINKEDLIST * list = GetListRoot(Other,ro,co);
                    val =Other->data[ro][co];

                    while(list != nullptr)
                    {
                        int r = list->rowNr;
                        int c = list->colNr;

                        map->data[r][c] = classn;


                        if(adjust_threshold)
                        {
                            list = ListReplaceFirstByEQNBF2(Other,map,list, val, threshold);

                        }else
                        {
                            list = ListReplaceFirstByEQNBF(Other,map,list, val, threshold);

                        }
                    }

                    classn++;
                }

            }

        }
    }

    return map;

}


inline cTMap * AS_SpreadDepressionFind(cTMap * DEM, int iter_max = 0)
{

    //MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
     cTMap *map = new cTMap(nullptr,DEM->data.nr_cols(),DEM->data.nr_rows(),  DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());//std::move(raster_data),DEM->projection(),"");


    //initialize the map with friction values
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(pcr::isMV(DEM->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else {
                map->data[r][c] = 1e31;
            }
        }
    }


    float dx = map->cellSize();

    //we keep iterating through this algorithm untill there is no change left to make
    bool change = true;
    bool first = true;

    int iter = 0;

    while(change && ((iter_max <= 0) || (iter_max > 0 && iter < iter_max)))
    {
        iter ++;
        change = false;

        if(iter%2 == 0)
        {
            //first we move in right-lower direction
            for(int r = 0; r < map->data.nr_rows();r++)
            {
                for(int c = 0; c < map->data.nr_cols();c++)
                {
                    float v_points = DEM->data[r][c];

                    if(!pcr::isMV(v_points))
                    {
                        if((r-1 > -1))
                        {
                            float vn_points = DEM->data[r-1][c];
                            if(pcr::isMV(vn_points))
                            {
                                map->data[r][c] = DEM->data[r][c];
                            }else
                            {
                                float demh;

                                    demh = std::max(map->data[r-1][c],DEM->data[r][c]);

                                float mapn = std::min(demh,map->data[r][c]);
                                if(mapn < map->data[r][c])
                                {
                                    change = true;
                                    map->data[r][c] = mapn;
                                }
                            }
                        }else
                        {
                            map->data[r][c] = DEM->data[r][c];
                        }

                        if((c-1 > -1))
                        {
                            float vn_points = DEM->data[r][c-1];
                            if(pcr::isMV(vn_points))
                            {
                                map->data[r][c] = DEM->data[r][c];
                            }else
                            {
                                float demh;

                                    demh = std::max(map->data[r][c-1],DEM->data[r][c]);

                                float mapn = std::min(demh,map->data[r][c]);
                                if(mapn < map->data[r][c])
                                {
                                    change = true;
                                    map->data[r][c] = mapn;
                                }
                            }
                        }else
                        {
                            map->data[r][c] = DEM->data[r][c];
                        }
                    }
                }
            }

            //then we move in left-upper direction
            for(int r = map->data.nr_rows()-1; r > -1 ;r--)
            {
                for(int c = map->data.nr_cols()-1; c > -1 ;c--)
                {
                    float v_points = DEM->data[r][c];

                    if(!pcr::isMV(v_points))
                    {
                        if((r+1 < map->data.nr_rows()))
                        {
                            float vn_points = DEM->data[r+1][c];
                            if(pcr::isMV(vn_points))
                            {
                                map->data[r][c] = DEM->data[r][c];
                            }else
                            {
                                float demh;

                                    demh = std::max(map->data[r+1][c],DEM->data[r][c]);

                                float mapn = std::min(demh,map->data[r][c]);
                                if(mapn < map->data[r][c])
                                {
                                    change = true;
                                    map->data[r][c] = mapn;
                                }
                            }
                        }else
                        {
                            map->data[r][c] = DEM->data[r][c];
                        }

                        if((c+1 < map->data.nr_cols()))
                        {
                            float vn_points = DEM->data[r][c+1];
                            if(pcr::isMV(vn_points))
                            {
                                map->data[r][c] = DEM->data[r][c];
                            }else
                            {
                                float demh;

                                    demh = std::max(map->data[r][c+1],DEM->data[r][c]);

                                float mapn = std::min(demh,map->data[r][c]);
                                if(mapn < map->data[r][c])
                                {
                                    change = true;
                                    map->data[r][c] = mapn;
                                }
                            }
                        }else
                        {
                            map->data[r][c] = DEM->data[r][c];
                        }
                    }

                }

            }
        }else
            {
                //first we move in right-lower direction
                for(int r = 0; r < map->data.nr_rows();r++)
                {
                    for(int c = map->data.nr_cols()-1; c > -1 ;c--)
                    {
                        float v_points = DEM->data[r][c];

                        if(!pcr::isMV(v_points))
                        {
                            if((r-1 > -1))
                            {
                                float vn_points = DEM->data[r-1][c];
                                if(pcr::isMV(vn_points))
                                {
                                    map->data[r][c] = DEM->data[r][c];
                                }else
                                {
                                    float demh;

                                        demh = std::max(map->data[r-1][c],DEM->data[r][c]);

                                    float mapn = std::min(demh,map->data[r][c]);
                                    if(mapn < map->data[r][c])
                                    {
                                        change = true;
                                        map->data[r][c] = mapn;
                                    }
                                }
                            }else
                            {
                                map->data[r][c] = DEM->data[r][c];
                            }


                            if((c+1 < map->data.nr_cols()))
                            {
                                float vn_points = DEM->data[r][c+1];
                                if(pcr::isMV(vn_points))
                                {
                                    map->data[r][c] = DEM->data[r][c];
                                }else
                                {
                                    float demh;

                                        demh = std::max(map->data[r][c+1],DEM->data[r][c]);

                                    float mapn = std::min(demh,map->data[r][c]);
                                    if(mapn < map->data[r][c])
                                    {
                                        change = true;
                                        map->data[r][c] = mapn;
                                    }
                                }
                            }else
                            {
                                map->data[r][c] = DEM->data[r][c];
                            }
                        }
                    }
                }


                //then we move in left-upper direction
                for(int r = map->data.nr_rows()-1; r > -1 ;r--)
                {
                    for(int c = 0; c < map->data.nr_cols();c++)
                    {
                        float v_points = DEM->data[r][c];

                        if(!pcr::isMV(v_points))
                        {
                            if((r+1 < map->data.nr_rows()))
                            {
                                float vn_points = DEM->data[r+1][c];
                                if(pcr::isMV(vn_points))
                                {
                                    map->data[r][c] = DEM->data[r][c];
                                }else
                                {
                                    float demh;

                                        demh = std::max(map->data[r+1][c],DEM->data[r][c]);

                                    float mapn = std::min(demh,map->data[r][c]);
                                    if(mapn < map->data[r][c])
                                    {
                                        change = true;
                                        map->data[r][c] = mapn;
                                    }
                                }
                            }else
                            {
                                map->data[r][c] = DEM->data[r][c];
                            }

                            if((c-1 > -1))
                            {
                                float vn_points = DEM->data[r][c-1];
                                if(pcr::isMV(vn_points))
                                {
                                    map->data[r][c] = DEM->data[r][c];
                                }else
                                {
                                    float demh;


                                        demh = std::max(map->data[r][c-1],DEM->data[r][c]);

                                    float mapn = std::min(demh,map->data[r][c]);
                                    if(mapn < map->data[r][c])
                                    {
                                        change = true;
                                        map->data[r][c] = mapn;
                                    }
                                }
                            }else
                            {
                                map->data[r][c] = DEM->data[r][c];
                            }
                        }

                    }
            }


        }


        first = false;

    }

    return map;

}


inline cTMap * AS_DiffusiveMaxWave(cTMap * DEM,cTMap * H, int iter, float couranti)
{


	printf("start diffusive max wave\n");

    //MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
     cTMap *map = new cTMap(nullptr,DEM->data.nr_cols(),DEM->data.nr_rows(),  DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());//std::move(raster_data),DEM->projection(),"");


    //MaskedRaster<float> raster_data2(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
     cTMap *map2 = new cTMap(nullptr,DEM->data.nr_cols(),DEM->data.nr_rows(),  DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());//std::move(raster_data),DEM->projection(),"");


    float dx = 1.0;
    float sl;
    float dem,dem_xl,dem_xr,dem_yl,dem_yr;
    float h, h_xl, h_xr, h_yl, h_yr;
    float hn, qmax_xr,qmax_xl,qmax_yl,qmax_yr,signx,signy,vx,vy,qx,qy,sx,sy;
    bool mv_xl,mv_xr,mv_yl,mv_yr;
    float qmaxx, qmaxy;

    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                map->data[r][c] = (!pcr::isMV(H->data[r][c]))? H->Drc : 0.0;
                map2->data[r][c] = map->data[r][c];
            }
        }
    }

    for(int i = 0; i < iter; i++)
    {
        float courant = couranti;
        if(i > iter - 10)
        {
            float in = i-(iter-10);
            courant = couranti * ((1.0- in/10.0)*1.0) + (in/10.0) * 0.05 * couranti;
        std::cout << "iter " << i << " " << courant << std::endl;
        }

        if(i%2 == 0)
        {
            for(int r = DEM->data.nr_rows()-1; r >-1 ;r--)
            {
                for(int c = DEM->data.nr_cols() -1; c > -1 ;c--)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        //reset variables
                        hn = 0.0;
                        qmax_xr = 0.0;
                        qmax_xl = 0.0;
                        qmax_yl = 0.0;
                        qmax_yr = 0.0;
                        signx = 0.0;
                        signy = 0.0;
                        vx = 0.0;
                        vy = 0.0;
                        qx = 0.0;
                        qy = 0.0;
                        sx = 0.0;
                        sy = 0.0;
                        mv_xl = false;
                        mv_xr = false;
                        mv_yl = false;
                        mv_yr = false;

                        dem =DEM->data[r][c];

                        //slope
                        if(c > 0)
                        {
                            dem_xl = pcr::isMV(DEM->data[r][c-1])?dem:DEM->data[r][c-1];
                        }else {
                            dem_xl = dem;
                        }
                        if(c < DEM->nrCols()-1)
                        {
                            dem_xr = pcr::isMV(DEM->data[r][c+1])?dem:DEM->data[r][c+1];
                        }else {
                            dem_xr = dem;
                        }
                        if(r > 0)
                        {
                            dem_yl = pcr::isMV(DEM->data[r-1][c])?dem:DEM->data[r-1][c];
                        }else {
                            dem_yl = dem;
                        }
                        if(r < DEM->nrRows()-1)
                        {
                            dem_yr = pcr::isMV(DEM->data[r+1][c])?dem:DEM->data[r+1][c];
                        }else {
                            dem_yr = dem;

                        }

                        h = map->Drc;

                        if(c > 0)
                        {
                            h_xl = pcr::isMV(map->data[r][c-1])?h:map->data[r][c-1];
                        }else {
                            h_xl = h;
                        }
                        if(c < DEM->nrCols()-1)
                        {
                            h_xr = pcr::isMV(map->data[r][c+1])?h:map->data[r][c+1];
                        }else {
                            h_xr = h;
                        }
                        if(r > 0)
                        {
                            h_yl = pcr::isMV(map->data[r-1][c])?h:map->data[r-1][c];
                        }else {
                            h_yl = h;
                        }
                        if(r < DEM->nrRows()-1)
                        {
                            h_yr = pcr::isMV(map->data[r+1][c])?h:map->data[r+1][c];
                        }else {
                            h_yr = h;
                        }


                        sx = (h_xr +dem_xr)-(h_xl+dem_xl);
                        sy = (h_yr +dem_yr)-(h_yl+dem_yl);


                        //velocity
                        sl = std::sqrt(sx*sx + sy*sy);
                        vx = sx/sl;
                        vy = sy/sl;
                        signx = vx > 0.0? -1.0:1.0;
                        signy = vy > 0.0? -1.0:1.0;

                        //flux limiter

                        qmax_xl = std::max(0.0f,dem+h - dem_xl - h_xl);
                        qmax_xr = std::max(0.0f,dem+h - dem_xr - h_xr);
                        qmax_yl = std::max(0.0f,dem+h - dem_yl - h_yl);
                        qmax_yr = std::max(0.0f,dem+h - dem_yr - h_yr);

                        //discharge

                        qmaxx = signx > 0.0? qmax_xr : qmax_xl;
                        qmaxy = signy > 0.0? qmax_yr : qmax_yl;

                        qx = std::min(courant*h,std::min(qmaxx,std::abs(courant*h * vx)));
                        qy = std::min(courant*h,std::min(qmaxy,std::abs(courant*h * vy)));
                        //change height

                        hn = h - qx - qy;
                        map2->data[r][c] = std::max(0.0f,map2->data[r][c] - qx - qy);

                        if(c > 0)
                        {
                            if(signx< 0 && !pcr::isMV(DEM->data[r][c-1]))
                            {
                                map2->data[r][c-1] = map2->data[r][c-1]+qx;
                            }
                        }
                        if(c < DEM->nrCols() -1)
                        {
                            if(signx > 0 && !pcr::isMV(DEM->data[r][c+1]))
                            {
                                map2->data[r][c+1] = map2->data[r][c+1]+qx;
                            }
                        }

                        if(r > 0)
                        {
                            if(signy < 0 && !pcr::isMV(DEM->data[r-1][c]))
                            {
                                map2->data[r-1][c] = map2->data[r-1][c]+qy;
                            }
                        }
                        if(r < DEM->nrRows() -1)
                        {
                            if(signy > 0 && !pcr::isMV(DEM->data[r+1][c]))
                            {
                                map2->data[r+1][c] = map2->data[r+1][c]+qy;
                            }
                        }

                    }
                }
            }

        }else {
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {

                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        //reset variables
                        hn = 0.0;
                        qmax_xr = 0.0;
                        qmax_xl = 0.0;
                        qmax_yl = 0.0;
                        qmax_yr = 0.0;
                        signx = 0.0;
                        signy = 0.0;
                        vx = 0.0;
                        vy = 0.0;
                        qx = 0.0;
                        qy = 0.0;
                        sx = 0.0;
                        sy = 0.0;
                        mv_xl = false;
                        mv_xr = false;
                        mv_yl = false;
                        mv_yr = false;

                        dem =DEM->data[r][c];

                        //slope
                        if(c > 0)
                        {
                            dem_xl = pcr::isMV(DEM->data[r][c-1])?dem:DEM->data[r][c-1];
                        }else {
                            dem_xl = dem;
                        }
                        if(c < DEM->nrCols()-1)
                        {
                            dem_xr = pcr::isMV(DEM->data[r][c+1])?dem:DEM->data[r][c+1];
                        }else {
                            dem_xr = dem;
                        }
                        if(r > 0)
                        {
                            dem_yl = pcr::isMV(DEM->data[r-1][c])?dem:DEM->data[r-1][c];
                        }else {
                            dem_yl = dem;
                        }
                        if(r < DEM->nrRows()-1)
                        {
                            dem_yr = pcr::isMV(DEM->data[r+1][c])?dem:DEM->data[r+1][c];
                        }else {
                            dem_yr = dem;

                        }

                        h = map->Drc;

                        if(c > 0)
                        {
                            h_xl = pcr::isMV(map->data[r][c-1])?h:map->data[r][c-1];
                        }else {
                            h_xl = h;
                        }
                        if(c < DEM->nrCols()-1)
                        {
                            h_xr = pcr::isMV(map->data[r][c+1])?h:map->data[r][c+1];
                        }else {
                            h_xr = h;
                        }
                        if(r > 0)
                        {
                            h_yl = pcr::isMV(map->data[r-1][c])?h:map->data[r-1][c];
                        }else {
                            h_yl = h;
                        }
                        if(r < DEM->nrRows()-1)
                        {
                            h_yr = pcr::isMV(map->data[r+1][c])?h:map->data[r+1][c];
                        }else {
                            h_yr = h;
                        }


                        sx = (h_xr +dem_xr)-(h_xl+dem_xl);
                        sy = (h_yr +dem_yr)-(h_yl+dem_yl);


                        //velocity
                        sl = std::sqrt(sx*sx + sy*sy);
                        vx = sx/sl;
                        vy = sy/sl;
                        signx = vx > 0.0? -1.0:1.0;
                        signy = vy > 0.0? -1.0:1.0;

                        //flux limiter

                        qmax_xl = std::max(0.0f,dem+h - dem_xl - h_xl);
                        qmax_xr = std::max(0.0f,dem+h - dem_xr - h_xr);
                        qmax_yl = std::max(0.0f,dem+h - dem_yl - h_yl);
                        qmax_yr = std::max(0.0f,dem+h - dem_yr - h_yr);

                        //discharge

                        qmaxx = signx > 0.0? qmax_xr : qmax_xl;
                        qmaxy = signy > 0.0? qmax_yr : qmax_yl;

                        qx = std::min(courant*h,std::min(qmaxx,std::abs(courant*h * vx)));
                        qy = std::min(courant*h,std::min(qmaxy,std::abs(courant*h * vy)));
                        //change height

                        hn = h - qx - qy;
                        map2->data[r][c] = std::max(0.0f,map2->data[r][c] - qx - qy);

                        if(c > 0)
                        {
                            if(signx< 0 && !pcr::isMV(DEM->data[r][c-1]))
                            {
                                map2->data[r][c-1] = map2->data[r][c-1]+qx;
                            }
                        }
                        if(c < DEM->nrCols() -1)
                        {
                            if(signx > 0 && !pcr::isMV(DEM->data[r][c+1]))
                            {
                                map2->data[r][c+1] = map2->data[r][c+1]+qx;
                            }
                        }

                        if(r > 0)
                        {
                            if(signy < 0 && !pcr::isMV(DEM->data[r-1][c]))
                            {
                                map2->data[r-1][c] = map2->data[r-1][c]+qy;
                            }
                        }
                        if(r < DEM->nrRows() -1)
                        {
                            if(signy > 0 && !pcr::isMV(DEM->data[r+1][c]))
                            {
                                map2->data[r+1][c] = map2->data[r+1][c]+qy;
                            }
                        }

                    }
                }
            }


        }

        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                map->data[r][c] = map2->data[r][c];
            }
        }

    }

    delete map2;

    return map;
}



inline cTMap * AS_DiffusiveQMaxWave(cTMap * DEM,cTMap * H, int iter, float couranti, float power = 0.6)
{

	printf("start diffusive max q wave\n");


    float power2 = 1.0/std::max(1e-12f,power);

   

    //MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
    cTMap *map = new cTMap(nullptr,DEM->data.nr_cols(),DEM->data.nr_rows(),  DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());//std::move(raster_data),DEM->projection(),"");


    //MaskedRaster<float> raster_data2(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
     cTMap *map2 = new cTMap(nullptr,DEM->data.nr_cols(),DEM->data.nr_rows(),  DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());//std::move(raster_data),DEM->projection(),"");


    float dx = 1.0;
    float sl;
    float dem,dem_xl,dem_xr,dem_yl,dem_yr;
    float h, h_xl, h_xr, h_yl, h_yr;
    float hn, qmax_xr,qmax_xl,qmax_yl,qmax_yr,signx,signy,vx,vy,qx,qy,sx,sy;
    bool mv_xl,mv_xr,mv_yl,mv_yr;
    float qmaxx, qmaxy;

    float total = 0.0;
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                map->data[r][c] = (!pcr::isMV(H->data[r][c]))? std::pow(H->Drc,power2) : 0.0;
                map2->data[r][c] = map->data[r][c];
                total += map->data[r][c];
            }
        }
    }

    std::cout << "total1 " << total <<  std::endl;

    for(int i = 0; i < iter; i++)
    {
        float courant = couranti;
        if(i > iter - 10)
        {
            float in = i-(iter-10);
            courant = couranti * ((1.0- in/10.0)*1.0) + (in/10.0) * 0.05 * couranti;

        }
         std::cout << "iter " << i << " " << courant << std::endl;


        if(true)//i%2 == 0)
        {
            for(int r = DEM->data.nr_rows()-1; r >-1 ;r--)
            {
                for(int c = DEM->data.nr_cols() -1; c > -1 ;c--)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        //reset variables
                        hn = 0.0;
                        qmax_xr = 0.0;
                        qmax_xl = 0.0;
                        qmax_yl = 0.0;
                        qmax_yr = 0.0;
                        signx = 0.0;
                        signy = 0.0;
                        vx = 0.0;
                        vy = 0.0;
                        qx = 0.0;
                        qy = 0.0;
                        sx = 0.0;
                        sy = 0.0;
                        mv_xl = false;
                        mv_xr = false;
                        mv_yl = false;
                        mv_yr = false;

                        dem =DEM->data[r][c];

                        //slope
                        if(c > 0)
                        {
                            dem_xl = pcr::isMV(DEM->data[r][c-1])?dem:DEM->data[r][c-1];
                        }else {
                            dem_xl = dem;
                        }
                        if(c < DEM->nrCols()-1)
                        {
                            dem_xr = pcr::isMV(DEM->data[r][c+1])?dem:DEM->data[r][c+1];
                        }else {
                            dem_xr = dem;
                        }
                        if(r > 0)
                        {
                            dem_yl = pcr::isMV(DEM->data[r-1][c])?dem:DEM->data[r-1][c];
                        }else {
                            dem_yl = dem;
                        }
                        if(r < DEM->nrRows()-1)
                        {
                            dem_yr = pcr::isMV(DEM->data[r+1][c])?dem:DEM->data[r+1][c];
                        }else {
                            dem_yr = dem;

                        }

                        h = map->Drc;

                        if(c > 0)
                        {
                            h_xl = pcr::isMV(map->data[r][c-1])?h:map->data[r][c-1];
                        }else {
                            h_xl = h;
                        }
                        if(c < DEM->nrCols()-1)
                        {
                            h_xr = pcr::isMV(map->data[r][c+1])?h:map->data[r][c+1];
                        }else {
                            h_xr = h;
                        }
                        if(r > 0)
                        {
                            h_yl = pcr::isMV(map->data[r-1][c])?h:map->data[r-1][c];
                        }else {
                            h_yl = h;
                        }
                        if(r < DEM->nrRows()-1)
                        {
                            h_yr = pcr::isMV(map->data[r+1][c])?h:map->data[r+1][c];
                        }else {
                            h_yr = h;
                        }

                        float h_org = h;
                        float h_org_xl = h_xl;
                        float h_org_xr = h_xr;
                        float h_org_yl = h_yl;
                        float h_org_yr = h_yr;

                        h = std::pow(h,power);
                        h_xl = std::pow(h_xl,power);
                        h_xr = std::pow(h_xr,power);
                        h_yl = std::pow(h_yl,power);
                        h_yr = std::pow(h_yr,power);

                        sx = (h_xr +dem_xr)-(h_xl+dem_xl);
                        sy = (h_yr +dem_yr)-(h_yl+dem_yl);


                        //velocity
                        sl = std::sqrt(sx*sx + sy*sy);
                        vx = sx/sl;
                        vy = sy/sl;
                        signx = vx > 0.0? -1.0:1.0;
                        signy = vy > 0.0? -1.0:1.0;

                        //how much can we transfer so that the height in the neighbor is the same as ours?


                        //flux limiter

                        qmax_xl = std::max(0.0f,std::pow(std::max(0.0f,dem+h - dem_xl),power2) -h_org_xl);
                        qmax_xr = std::max(0.0f,std::pow(std::max(0.0f,dem+h - dem_xr),power2) -h_org_xr);
                        qmax_yl = std::max(0.0f,std::pow(std::max(0.0f,dem+h - dem_yl),power2) -h_org_yl);
                        qmax_yr = std::max(0.0f,std::pow(std::max(0.0f,dem+h - dem_yr),power2) -h_org_yr);

                        //discharge

                        qmaxx = signx > 0.0? qmax_xr : qmax_xl;
                        qmaxy = signy > 0.0? qmax_yr : qmax_yl;

                        qx = std::min(courant*h_org,std::min(0.5f*qmaxx,std::abs(courant*h_org * vx)));
                        qy = std::min(courant*h_org,std::min(0.5f*qmaxy,std::abs(courant*h_org * vy)));
                        //change height

                        hn = h - qx - qy;
                        map2->data[r][c] = std::max(0.0f,map2->data[r][c] - qx - qy);

                        if(c > 0)
                        {
                            if(signx< 0 && !pcr::isMV(DEM->data[r][c-1]))
                            {
                                map2->data[r][c-1] = map2->data[r][c-1]+qx;
                            }
                        }
                        if(c < DEM->nrCols() -1)
                        {
                            if(signx > 0 && !pcr::isMV(DEM->data[r][c+1]))
                            {
                                map2->data[r][c+1] = map2->data[r][c+1]+qx;
                            }
                        }

                        if(r > 0)
                        {
                            if(signy < 0 && !pcr::isMV(DEM->data[r-1][c]))
                            {
                                map2->data[r-1][c] = map2->data[r-1][c]+qy;
                            }
                        }
                        if(r < DEM->nrRows() -1)
                        {
                            if(signy > 0 && !pcr::isMV(DEM->data[r+1][c]))
                            {
                                map2->data[r+1][c] = map2->data[r+1][c]+qy;
                            }
                        }

                    }
                }
            }

        }else {
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {

                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        //reset variables
                        hn = 0.0;
                        qmax_xr = 0.0;
                        qmax_xl = 0.0;
                        qmax_yl = 0.0;
                        qmax_yr = 0.0;
                        signx = 0.0;
                        signy = 0.0;
                        vx = 0.0;
                        vy = 0.0;
                        qx = 0.0;
                        qy = 0.0;
                        sx = 0.0;
                        sy = 0.0;
                        mv_xl = false;
                        mv_xr = false;
                        mv_yl = false;
                        mv_yr = false;

                        dem =DEM->data[r][c];

                        //slope
                        if(c > 0)
                        {
                            dem_xl = pcr::isMV(DEM->data[r][c-1])?dem:DEM->data[r][c-1];
                        }else {
                            dem_xl = dem;
                        }
                        if(c < DEM->nrCols()-1)
                        {
                            dem_xr = pcr::isMV(DEM->data[r][c+1])?dem:DEM->data[r][c+1];
                        }else {
                            dem_xr = dem;
                        }
                        if(r > 0)
                        {
                            dem_yl = pcr::isMV(DEM->data[r-1][c])?dem:DEM->data[r-1][c];
                        }else {
                            dem_yl = dem;
                        }
                        if(r < DEM->nrRows()-1)
                        {
                            dem_yr = pcr::isMV(DEM->data[r+1][c])?dem:DEM->data[r+1][c];
                        }else {
                            dem_yr = dem;

                        }

                        h = map->Drc;

                        if(c > 0)
                        {
                            h_xl = pcr::isMV(map->data[r][c-1])?h:map->data[r][c-1];
                        }else {
                            h_xl = h;
                        }
                        if(c < DEM->nrCols()-1)
                        {
                            h_xr = pcr::isMV(map->data[r][c+1])?h:map->data[r][c+1];
                        }else {
                            h_xr = h;
                        }
                        if(r > 0)
                        {
                            h_yl = pcr::isMV(map->data[r-1][c])?h:map->data[r-1][c];
                        }else {
                            h_yl = h;
                        }
                        if(r < DEM->nrRows()-1)
                        {
                            h_yr = pcr::isMV(map->data[r+1][c])?h:map->data[r+1][c];
                        }else {
                            h_yr = h;
                        }

                        h = std::pow(h,power);
                        h_xl = std::pow(h_xl,power);
                        h_xr = std::pow(h_xr,power);
                        h_yl = std::pow(h_yl,power);
                        h_yr = std::pow(h_yr,power);




                        sx = (h_xr +dem_xr)-(h_xl+dem_xl);
                        sy = (h_yr +dem_yr)-(h_yl+dem_yl);


                        //velocity
                        sl = std::sqrt(sx*sx + sy*sy);
                        vx = sx/sl;
                        vy = sy/sl;
                        signx = vx > 0.0? -1.0:1.0;
                        signy = vy > 0.0? -1.0:1.0;

                        //flux limiter

                        qmax_xl = std::max(0.0f,dem+h - dem_xl - h_xl);
                        qmax_xr = std::max(0.0f,dem+h - dem_xr - h_xr);
                        qmax_yl = std::max(0.0f,dem+h - dem_yl - h_yl);
                        qmax_yr = std::max(0.0f,dem+h - dem_yr - h_yr);

                        //discharge

                        qmaxx = signx > 0.0? qmax_xr : qmax_xl;
                        qmaxy = signy > 0.0? qmax_yr : qmax_yl;

                        qx = std::min(courant*h,std::min(qmaxx,std::abs(courant*h * vx)));
                        qy = std::min(courant*h,std::min(qmaxy,std::abs(courant*h * vy)));
                        //change height

                        hn = h - qx - qy;
                        map2->data[r][c] = std::max(0.0f,map2->data[r][c] - qx - qy);

                        if(c > 0)
                        {
                            if(signx< 0 && !pcr::isMV(DEM->data[r][c-1]))
                            {
                                map2->data[r][c-1] = map2->data[r][c-1]+qx;
                            }
                        }
                        if(c < DEM->nrCols() -1)
                        {
                            if(signx > 0 && !pcr::isMV(DEM->data[r][c+1]))
                            {
                                map2->data[r][c+1] = map2->data[r][c+1]+qx;
                            }
                        }

                        if(r > 0)
                        {
                            if(signy < 0 && !pcr::isMV(DEM->data[r-1][c]))
                            {
                                map2->data[r-1][c] = map2->data[r-1][c]+qy;
                            }
                        }
                        if(r < DEM->nrRows() -1)
                        {
                            if(signy > 0 && !pcr::isMV(DEM->data[r+1][c]))
                            {
                                map2->data[r+1][c] = map2->data[r+1][c]+qy;
                            }
                        }

                    }
                }
            }


        }

        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                map->data[r][c] = map2->data[r][c];
            }
        }

    }

    delete map2;

    total = 0.0;
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(map->data[r][c]))
            {
                total += map->data[r][c];
                map->data[r][c] = std::pow(map->data[r][c],power);
            }

        }
    }

    std::cout << "total2 " << total <<  std::endl;


    return map;
}


inline cTMap * AS_SpreadFlowMD2(cTMap * source ,cTMap * fracx1, cTMap * fracx2, cTMap * fracy1,cTMap * fracy2, int iter_max = 0)
{
	printf("start spread flow md2 %f %f %f \n", source->cellSizeX(), source->cellSizeY(), source->cellSize());
    //MaskedRaster<float> raster_data(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
    printf("start spread flow md22 \n");
    
	cTMap *mx1 = new cTMap(nullptr, source->data.nr_cols(),source->data.nr_rows(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());//std::move(raster_data),source->projection(),"");
    //MaskedRaster<float> raster_data2();
    cTMap *mx2 = new cTMap(nullptr, source->data.nr_cols(),source->data.nr_rows(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());//std::move(raster_data),source->projection(),"");
    //MaskedRaster<float> raster_data3(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
    cTMap *my1 = new cTMap(nullptr, source->data.nr_cols(),source->data.nr_rows(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());//std::move(raster_data),source->projection(),"");
    //MaskedRaster<float> raster_data4(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
    cTMap *my2 = new cTMap(nullptr, source->data.nr_cols(),source->data.nr_rows(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());//std::move(raster_data),source->projection(),"");
    
    //MaskedRaster<float> raster_data5(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
    cTMap *map = new cTMap(nullptr, source->data.nr_cols(),source->data.nr_rows(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());//std::move(raster_data),source->projection(),"");
    
	printf("start spread flow md23\n");
    

    //initialize the map with friction values

    //#pragma omp parallel for collapse(2)
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(pcr::isMV(source->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else {
                map->data[r][c] = 0.0;
                mx1->data[r][c] = fracx1->data[r][c] * source->data[r][c];
                my1->data[r][c] = fracy1->data[r][c] * source->data[r][c];
                mx2->data[r][c] = fracx2->data[r][c] * source->data[r][c];
                my2->data[r][c] = fracy2->data[r][c] * source->data[r][c];

            }
        }
    }

    float dx = map->cellSize();

    //we keep iterating through this algorithm untill there is no change left to make
    bool change = true;
    bool first = true;

    int iter = 0;

    while(change && ((iter_max <= 0) || (iter_max > 0 && iter < iter_max)))
    {
        iter ++;
        change = false;

        if(iter%2 == 0)
        {

            //first we move in right-lower direction
            for(int r = 0; r < map->data.nr_rows();r++)
            {
                for(int c = 0; c < map->data.nr_cols();c++)
                {
                    float v_points = source->data[r][c];
                    if(!pcr::isMV(v_points))
                    {

                        float mxf1 = mx1->data[r][c];
                        float myf1 = my1->data[r][c];
                        float mxf2 = mx2->data[r][c];
                        float myf2 = my2->data[r][c];
                        if((r+1 < map->data.nr_rows()) && myf2 > 1e-10)
                        {
                            {
                                mx1->data[r+1][c] += myf2 * fracx1->data[r+1][c];
                                my1->data[r+1][c] += myf2 * fracy1->data[r+1][c];
                                mx2->data[r+1][c] += myf2 * fracx2->data[r+1][c];
                                my2->data[r+1][c] += myf2 * fracy2->data[r+1][c];

                                change  = true;
                                map->data[r][c] += myf2;

                            }

                            my2->data[r][c] = 0.0;
                        }

                        if((c+1 < map->data.nr_cols()) && mxf2 > 1e-10)
                        {
                            {
                                mx1->data[r][c+1] += mxf2 * fracx1->data[r][c+1];
                                my1->data[r][c+1] += mxf2 * fracy1->data[r][c+1];
                                mx2->data[r][c+1] += mxf2 * fracx2->data[r][c+1];
                                my2->data[r][c+1] += mxf2 * fracy2->data[r][c+1];
                                change  = true;
                                map->data[r][c] += mxf2;

                            }

                            mx2->data[r][c] = 0.0;
                        }

                    }
                }
            }

            //then we move in left-upper direction
            for(int r = map->data.nr_rows()-1; r > -1 ;r--)
            {
                for(int c = map->data.nr_cols()-1; c > -1 ;c--)
                {
                    float v_points = source->data[r][c];
                    if(!pcr::isMV(v_points))
                    {

                        float mxf1 = mx1->data[r][c];
                        float myf1 = my1->data[r][c];
                        float mxf2 = mx2->data[r][c];
                        float myf2 = my2->data[r][c];
                        if((r-1 > -1) && myf1 < -1e-10)
                        {
                            {
                                mx1->data[r-1][c] -= myf1 * fracx1->data[r-1][c];
                                my1->data[r-1][c] -= myf1 * fracy1->data[r-1][c];
                                mx2->data[r-1][c] -= myf1 * fracx2->data[r-1][c];
                                my2->data[r-1][c] -= myf1 * fracy2->data[r-1][c];
                                change  = true;
                                map->data[r][c] -= myf1;

                            }

                            my1->data[r][c] = 0.0;
                        }

                        if((c-1 > -1) && mxf1 < -1e-10)
                        {
                            {
                                mx1->data[r][c-1] -= mxf1 * fracx1->data[r][c-1];
                                my1->data[r][c-1] -= mxf1 * fracy1->data[r][c-1];
                                mx2->data[r][c-1] -= mxf1 * fracx2->data[r][c-1];
                                my2->data[r][c-1] -= mxf1 * fracy2->data[r][c-1];
                                change  = true;
                                map->data[r][c] -= mxf1;

                            }

                            mx1->data[r][c] = 0.0;
                        }
                    }


                }
            }
        }else
        {

            //first we move in right-lower direction
            for(int r = 0; r < map->data.nr_rows();r++)
            {
                for(int c = map->data.nr_cols()-1; c > -1 ;c--)
                {
                    float v_points = source->data[r][c];
                    if(!pcr::isMV(v_points))
                    {

                        float mxf1 = mx1->data[r][c];
                        float myf1 = my1->data[r][c];
                        float mxf2 = mx2->data[r][c];
                        float myf2 = my2->data[r][c];
                        if((r+1 < map->data.nr_rows()) && myf2 > 1e-10)
                        {
                            {
                                mx1->data[r+1][c] += myf2 * fracx1->data[r+1][c];
                                my1->data[r+1][c] += myf2 * fracy1->data[r+1][c];
                                mx2->data[r+1][c] += myf2 * fracx2->data[r+1][c];
                                my2->data[r+1][c] += myf2 * fracy2->data[r+1][c];

                                change  = true;
                                map->data[r][c] += myf2;

                            }

                            my2->data[r][c] = 0.0;
                        }
                        if((c-1 > -1) && mxf1 < -1e-10)
                        {
                            {
                                mx1->data[r][c-1] -= mxf1 * fracx1->data[r][c-1];
                                my1->data[r][c-1] -= mxf1 * fracy1->data[r][c-1];
                                mx2->data[r][c-1] -= mxf1 * fracx2->data[r][c-1];
                                my2->data[r][c-1] -= mxf1 * fracy2->data[r][c-1];
                                change  = true;
                                map->data[r][c] -= mxf1;

                            }

                            mx1->data[r][c] = 0.0;
                        }

                    }
                }
            }

            //then we move in left-upper direction
            for(int r = map->data.nr_rows()-1; r > -1 ;r--)
            {
                for(int c = 0; c < map->data.nr_cols();c++)
                {
                    float v_points = source->data[r][c];
                    if(!pcr::isMV(v_points))
                    {

                        float mxf1 = mx1->data[r][c];
                        float myf1 = my1->data[r][c];
                        float mxf2 = mx2->data[r][c];
                        float myf2 = my2->data[r][c];
                        if((r-1 > -1) && myf1 < -1e-10)
                        {
                            {
                                mx1->data[r-1][c] -= myf1 * fracx1->data[r-1][c];
                                my1->data[r-1][c] -= myf1 * fracy1->data[r-1][c];
                                mx2->data[r-1][c] -= myf1 * fracx2->data[r-1][c];
                                my2->data[r-1][c] -= myf1 * fracy2->data[r-1][c];
                                change  = true;
                                map->data[r][c] -= myf1;

                            }

                            my1->data[r][c] = 0.0;
                        }

                        if((c+1 < map->data.nr_cols()) && mxf2 > 1e-10)
                        {
                            {
                                mx1->data[r][c+1] += mxf2 * fracx1->data[r][c+1];
                                my1->data[r][c+1] += mxf2 * fracy1->data[r][c+1];
                                mx2->data[r][c+1] += mxf2 * fracx2->data[r][c+1];
                                my2->data[r][c+1] += mxf2 * fracy2->data[r][c+1];
                                change  = true;
                                map->data[r][c] += mxf2;

                            }

                            mx2->data[r][c] = 0.0;
                        }

                    }


                }
            }

        }
        first = false;

    }

    delete mx1;
    delete my1;
    delete mx2;
    delete my2;

	printf("start spread flow md24\n");
    
	
    return map;

}


inline cTMap * AS_SpreadFlowMD2Depression(cTMap * source ,cTMap * fracx1, cTMap * fracx2, cTMap * fracy1,cTMap * fracy2, int iter_max, cTMap * DEPRESSIONID, std::vector<float> & DEPRESSIONVOLS, std::vector<float> & DEPRESSIONCVOLS, bool store_depressionvol, float scale)
{

    std::cout << "spread with depression "<< DEPRESSIONVOLS.size() << " " << DEPRESSIONCVOLS.size() << " " << std::endl;

    std::vector<float> temp;
    std::vector<float> *DEPRESSIONCVOLSH;
    if(!store_depressionvol)
    {
        for(int i = 0; i < DEPRESSIONCVOLS.size(); i++)
        {
            temp.push_back(0.0);
        }
        DEPRESSIONCVOLSH = &temp;
    }else
    {
        DEPRESSIONCVOLSH = &DEPRESSIONCVOLS;
    }
    std::cout << "spread with depression "<< DEPRESSIONVOLS.size() << " " << DEPRESSIONCVOLS.size() << " " << DEPRESSIONCVOLSH->size() << std::endl;


    cTMap *mx1 = new cTMap(nullptr, source->data.nr_cols(),source->data.nr_rows(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());//std::move(raster_data),source->projection(),"");
    //MaskedRaster<float> raster_data2();
    cTMap *mx2 = new cTMap(nullptr, source->data.nr_cols(),source->data.nr_rows(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());//std::move(raster_data),source->projection(),"");
    //MaskedRaster<float> raster_data3(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
    cTMap *my1 = new cTMap(nullptr, source->data.nr_cols(),source->data.nr_rows(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());//std::move(raster_data),source->projection(),"");
    //MaskedRaster<float> raster_data4(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
    cTMap *my2 = new cTMap(nullptr, source->data.nr_cols(),source->data.nr_rows(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());//std::move(raster_data),source->projection(),"");
    
    //MaskedRaster<float> raster_data5(source->data.nr_rows(), source->data.nr_cols(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());
    cTMap *map = new cTMap(nullptr, source->data.nr_cols(),source->data.nr_rows(), source->data.north(), source->data.west(), source->data.cell_size(),source->data.cell_sizeY());//std::move(raster_data),source->projection(),"");
    

    //initialize the map with friction values

    //#pragma omp parallel for collapse(2)
    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(pcr::isMV(source->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else {
                map->data[r][c] = 0.0;
                mx1->data[r][c] = fracx1->data[r][c] * source->data[r][c];
                my1->data[r][c] = fracy1->data[r][c] * source->data[r][c];
                mx2->data[r][c] = fracx2->data[r][c] * source->data[r][c];
                my2->data[r][c] = fracy2->data[r][c] * source->data[r][c];

            }
        }
    }

    float dx = map->cellSize();

    //we keep iterating through this algorithm untill there is no change left to make
    bool change = true;
    bool first = true;

    int iter = 0;

    while(change && ((iter_max <= 0) || (iter_max > 0 && iter < iter_max)))
    {
        iter ++;
        change = false;

        if(iter%2 == 0)
        {

            //first we move in right-lower direction
            for(int r = 0; r < map->data.nr_rows();r++)
            {
                for(int c = 0; c < map->data.nr_cols();c++)
                {
                    float v_points = source->data[r][c];
                    if(!pcr::isMV(v_points))
                    {

                        float mxf1 = mx1->data[r][c];
                        float myf1 = my1->data[r][c];
                        float mxf2 = mx2->data[r][c];
                        float myf2 = my2->data[r][c];


                        if((r+1 < map->data.nr_rows()) && myf2 > 1e-10)
                        {
                            if(!pcr::isMV(DEPRESSIONID->data[r][c]))
                            {
                                float frac_depr = 0.0;
                                int id = DEPRESSIONID->data[r][c];
                                float vol = DEPRESSIONVOLS.at(id);
                                float cvol = DEPRESSIONCVOLSH->at(id);
                                if(cvol < vol - 0.0001)
                                {
                                    if(std::max(0.0f,myf2)*scale < vol - cvol)
                                    {
                                        frac_depr = 1.0;
                                    }else
                                    {
                                        frac_depr = std::max(0.0001f,vol-cvol)/(std::max(0.001f,myf2*scale));
                                    }

                                    DEPRESSIONCVOLSH->at(id) += std::max(0.0f,myf2*scale) * (frac_depr);

                                    myf2 = myf2 * (1.0-frac_depr);
                                }
                            }
                            {
                                mx1->data[r+1][c] += myf2 * fracx1->data[r+1][c];
                                my1->data[r+1][c] += myf2 * fracy1->data[r+1][c];
                                mx2->data[r+1][c] += myf2 * fracx2->data[r+1][c];
                                my2->data[r+1][c] += myf2 * fracy2->data[r+1][c];

                                change  = true;
                                map->data[r][c] += myf2;

                            }

                            my2->data[r][c] = 0.0;
                        }

                        if((c+1 < map->data.nr_cols()) && mxf2 > 1e-10)
                        {
                            if(!pcr::isMV(DEPRESSIONID->data[r][c]))
                            {
                                float frac_depr = 0.0;
                                int id = DEPRESSIONID->data[r][c];
                                float vol = DEPRESSIONVOLS.at(id);
                                float cvol = DEPRESSIONCVOLSH->at(id);
                                if(cvol < vol - 0.0001)
                                {
                                    if(std::max(0.0f,mxf2)*scale < vol - cvol)
                                    {
                                        frac_depr = 1.0;
                                    }else
                                    {
                                        frac_depr = std::max(0.0001f,vol-cvol)/(std::max(0.001f,mxf2*scale));
                                    }

                                    DEPRESSIONCVOLSH->at(id) += std::max(0.0f,mxf2*scale) *  (frac_depr);

                                    mxf2 = mxf2 * (1.0-frac_depr);
                                }
                            }
                            {
                                mx1->data[r][c+1] += mxf2 * fracx1->data[r][c+1];
                                my1->data[r][c+1] += mxf2 * fracy1->data[r][c+1];
                                mx2->data[r][c+1] += mxf2 * fracx2->data[r][c+1];
                                my2->data[r][c+1] += mxf2 * fracy2->data[r][c+1];
                                change  = true;
                                map->data[r][c] += mxf2;

                            }

                            mx2->data[r][c] = 0.0;
                        }

                    }
                }
            }

            //then we move in left-upper direction
            for(int r = map->data.nr_rows()-1; r > -1 ;r--)
            {
                for(int c = map->data.nr_cols()-1; c > -1 ;c--)
                {
                    float v_points = source->data[r][c];
                    if(!pcr::isMV(v_points))
                    {

                        float mxf1 = mx1->data[r][c];
                        float myf1 = my1->data[r][c];
                        float mxf2 = mx2->data[r][c];
                        float myf2 = my2->data[r][c];



                        if((r-1 > -1) && myf1 < -1e-10)
                        {

                            if(!pcr::isMV(DEPRESSIONID->data[r][c]))
                            {
                                float frac_depr = 0.0;
                                int id = DEPRESSIONID->data[r][c];
                                float vol = DEPRESSIONVOLS.at(id);
                                float cvol = DEPRESSIONCVOLSH->at(id);
                                if(cvol < vol - 0.0001)
                                {
                                    if(std::max(0.0f,-myf1)*scale < vol - cvol)
                                    {
                                        frac_depr = 1.0;
                                    }else
                                    {
                                        frac_depr = std::max(0.0001f,vol-cvol)/(std::max(0.001f,-myf1*scale));
                                    }

                                    DEPRESSIONCVOLSH->at(id) += std::max(0.0f,-myf1*scale) *  (frac_depr);

                                    myf1 = myf1 * (1.0-frac_depr);
                                }
                            }
                            {
                                mx1->data[r-1][c] -= myf1 * fracx1->data[r-1][c];
                                my1->data[r-1][c] -= myf1 * fracy1->data[r-1][c];
                                mx2->data[r-1][c] -= myf1 * fracx2->data[r-1][c];
                                my2->data[r-1][c] -= myf1 * fracy2->data[r-1][c];
                                change  = true;
                                map->data[r][c] -= myf1;

                            }

                            my1->data[r][c] = 0.0;
                        }

                        if((c-1 > -1) && mxf1 < -1e-10)
                        {
                            if(!pcr::isMV(DEPRESSIONID->data[r][c]))
                            {
                                float frac_depr = 0.0;
                                int id = DEPRESSIONID->data[r][c];
                                float vol = DEPRESSIONVOLS.at(id);
                                float cvol = DEPRESSIONCVOLSH->at(id);
                                if(cvol < vol - 0.0001)
                                {
                                    if(std::max(0.0f,-mxf1)*scale < vol - cvol)
                                    {
                                        frac_depr = 1.0;
                                    }else
                                    {
                                        frac_depr = std::max(0.0001f,vol-cvol)/(std::max(0.001f,-mxf1*scale));
                                    }

                                    DEPRESSIONCVOLSH->at(id) += std::max(0.0f,-mxf1*scale) *  (frac_depr);

                                    mxf1 = mxf1 * (1.0-frac_depr);
                                }
                            }
                            {
                                mx1->data[r][c-1] -= mxf1 * fracx1->data[r][c-1];
                                my1->data[r][c-1] -= mxf1 * fracy1->data[r][c-1];
                                mx2->data[r][c-1] -= mxf1 * fracx2->data[r][c-1];
                                my2->data[r][c-1] -= mxf1 * fracy2->data[r][c-1];
                                change  = true;
                                map->data[r][c] -= mxf1;

                            }

                            mx1->data[r][c] = 0.0;
                        }
                    }


                }
            }
        }else
        {

            //first we move in right-lower direction
            for(int r = 0; r < map->data.nr_rows();r++)
            {
                for(int c = map->data.nr_cols()-1; c > -1 ;c--)
                {
                    float v_points = source->data[r][c];
                    if(!pcr::isMV(v_points))
                    {

                        float mxf1 = mx1->data[r][c];
                        float myf1 = my1->data[r][c];
                        float mxf2 = mx2->data[r][c];
                        float myf2 = my2->data[r][c];


                        if((r+1 < map->data.nr_rows()) && myf2 > 1e-10)
                        {
                            if(!pcr::isMV(DEPRESSIONID->data[r][c]))
                            {
                                float frac_depr = 0.0;
                                int id = DEPRESSIONID->data[r][c];
                                float vol = DEPRESSIONVOLS.at(id);
                                float cvol = DEPRESSIONCVOLSH->at(id);
                                if(cvol < vol - 0.0001)
                                {
                                    if(std::max(0.0f,myf2*scale) < vol - cvol)
                                    {
                                        frac_depr = 1.0;
                                    }else
                                    {
                                        frac_depr = std::max(0.0001f,vol-cvol)/(std::max(0.001f,myf2*scale));
                                    }

                                    DEPRESSIONCVOLSH->at(id) += std::max(0.0f,myf2*scale) *  (frac_depr);

                                    myf2 = myf2 * (1.0-frac_depr);
                                }
                            }
                            {
                                mx1->data[r+1][c] += myf2 * fracx1->data[r+1][c];
                                my1->data[r+1][c] += myf2 * fracy1->data[r+1][c];
                                mx2->data[r+1][c] += myf2 * fracx2->data[r+1][c];
                                my2->data[r+1][c] += myf2 * fracy2->data[r+1][c];

                                change  = true;
                                map->data[r][c] += myf2;

                            }

                            my2->data[r][c] = 0.0;
                        }
                        if((c-1 > -1) && mxf1 < -1e-10)
                        {
                            if(!pcr::isMV(DEPRESSIONID->data[r][c]))
                            {
                                float frac_depr = 0.0;
                                int id = DEPRESSIONID->data[r][c];
                                float vol = DEPRESSIONVOLS.at(id);
                                float cvol = DEPRESSIONCVOLSH->at(id);
                                if(cvol < vol - 0.0001)
                                {
                                    if(std::max(0.0f,-mxf1*scale) < vol - cvol)
                                    {
                                        frac_depr = 1.0;
                                    }else
                                    {
                                        frac_depr = std::max(0.0001f,vol-cvol)/(std::max(0.001f,-mxf1*scale));
                                    }

                                    DEPRESSIONCVOLSH->at(id) += std::max(0.0f,-mxf1*scale) *  (frac_depr);

                                    mxf1 = mxf1 * (1.0-frac_depr);
                                }
                            }
                            {
                                mx1->data[r][c-1] -= mxf1 * fracx1->data[r][c-1];
                                my1->data[r][c-1] -= mxf1 * fracy1->data[r][c-1];
                                mx2->data[r][c-1] -= mxf1 * fracx2->data[r][c-1];
                                my2->data[r][c-1] -= mxf1 * fracy2->data[r][c-1];
                                change  = true;
                                map->data[r][c] -= mxf1;

                            }

                            mx1->data[r][c] = 0.0;
                        }

                    }
                }
            }

            //then we move in left-upper direction
            for(int r = map->data.nr_rows()-1; r > -1 ;r--)
            {
                for(int c = 0; c < map->data.nr_cols();c++)
                {
                    float v_points = source->data[r][c];
                    if(!pcr::isMV(v_points))
                    {

                        float mxf1 = mx1->data[r][c];
                        float myf1 = my1->data[r][c];
                        float mxf2 = mx2->data[r][c];
                        float myf2 = my2->data[r][c];



                        if((r-1 > -1) && myf1 < -1e-10)
                        {
                            if(!pcr::isMV(DEPRESSIONID->data[r][c]))
                            {
                                float frac_depr = 0.0;
                                int id = DEPRESSIONID->data[r][c];
                                float vol = DEPRESSIONVOLS.at(id);
                                float cvol = DEPRESSIONCVOLSH->at(id);
                                if(cvol < vol - 0.0001)
                                {
                                    if(std::max(0.0f,-myf1*scale) < vol - cvol)
                                    {
                                        frac_depr = 1.0;
                                    }else
                                    {
                                        frac_depr = std::max(0.0001f,vol-cvol)/(std::max(0.001f,-myf1*scale));
                                    }

                                    DEPRESSIONCVOLSH->at(id) += std::max(0.0f,-myf1*scale) *  (frac_depr);

                                    myf1 = myf1 * (1.0-frac_depr);
                                }
                            }
                            {
                                mx1->data[r-1][c] -= myf1 * fracx1->data[r-1][c];
                                my1->data[r-1][c] -= myf1 * fracy1->data[r-1][c];
                                mx2->data[r-1][c] -= myf1 * fracx2->data[r-1][c];
                                my2->data[r-1][c] -= myf1 * fracy2->data[r-1][c];
                                change  = true;
                                map->data[r][c] -= myf1;

                            }

                            my1->data[r][c] = 0.0;
                        }

                        if((c+1 < map->data.nr_cols()) && mxf2 > 1e-10)
                        {
                            if(!pcr::isMV(DEPRESSIONID->data[r][c]))
                            {
                                float frac_depr = 0.0;
                                int id = DEPRESSIONID->data[r][c];
                                float vol = DEPRESSIONVOLS.at(id);
                                float cvol = DEPRESSIONCVOLSH->at(id);
                                if(cvol < vol - 0.0001)
                                {
                                    if(std::max(0.0f,mxf2)*scale < vol - cvol)
                                    {
                                        frac_depr = 1.0;
                                    }else
                                    {
                                        frac_depr = std::max(0.0001f,vol-cvol)/(std::max(0.001f,mxf2*scale));
                                    }

                                    DEPRESSIONCVOLSH->at(id) += std::max(0.0f,mxf2*scale) *  (frac_depr);

                                    mxf2 = mxf2 * (1.0-frac_depr);
                                }
                            }

                            {
                                mx1->data[r][c+1] += mxf2 * fracx1->data[r][c+1];
                                my1->data[r][c+1] += mxf2 * fracy1->data[r][c+1];
                                mx2->data[r][c+1] += mxf2 * fracx2->data[r][c+1];
                                my2->data[r][c+1] += mxf2 * fracy2->data[r][c+1];
                                change  = true;
                                map->data[r][c] += mxf2;

                            }

                            mx2->data[r][c] = 0.0;
                        }

                    }


                }
            }

        }
        first = false;

    }

    delete mx1;
    delete my1;
    delete mx2;
    delete my2;

    return map;

}

#include "lsm_vector2.h"

inline void Bresenham_Drawline(int x0, int y0, int x1, int y1, std::function<void(int,int)> f)
{
  int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2; /* error value e_xy */

  for (;;){  /* loop */
    f(x0,y0);
    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
    if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
  }
}


inline float direction(LSMVector2 pi, LSMVector2 pj, LSMVector2 pk) {
	return (pk.x - pi.x) * (pj.y - pi.y) - (pj.x - pi.x) * (pk.y - pi.y);
}

inline bool onSegment(LSMVector2 pi, LSMVector2 pj, LSMVector2 pk) {
	if (std::min(pi.x, pj.x) <= pk.x && pk.x <= std::max(pi.x, pj.x)
		&& std::min(pi.y, pj.y) <= pk.y && pk.y <= std::max(pi.y, pj.y)) {
		return true;
	} else {
		return false;
	}
}

inline bool segmentIntersect(LSMVector2 p1, LSMVector2 p2, LSMVector2 p3, LSMVector2 p4) {
	float d1 = direction(p3, p4, p1);
	float d2 = direction(p3, p4, p2);
	float d3 = direction(p1, p2, p3);
	float d4 = direction(p1, p2, p4);

	if (((d1 > 0.0 && d2 <  0.0) || (d1 <  0.0 && d2 >  0.0)) && ((d3 >  0.0 && d4 <  0.0) || (d3 <  0.0 && d4 >  0.0))) {
		return true;
	} else if (d1 ==  0.0 && onSegment(p3, p4, p1)) {
		return true;
	} else if (d2 ==  0.0 && onSegment(p3, p4, p2)) {
		return true;
	} else if (d3 ==  0.0 && onSegment(p1, p2, p3)) {
		return true;
	} else if (d4 ==  0.0 && onSegment(p1, p2, p4)) {
		return true;
	} else {
		return false;
	}
}
	
	
void EMSCRIPTEN_KEEPALIVE do_rasterize_shapes(float*ret,float*data_dem,int w, int h, float * point,
float * values, float * lengths, float * types, float in_dx, int shapes)
{
	
	printf("rasterize shapes %i \n", shapes);
	
	//go over all shapes 
	cTMap * DEM = new cTMap(data_dem,w,h,0.0,0.0, 1.0,1.0);
	cTMap * Mask = DEM;
	int index_values = 0;
	int index_point = 0;
	for(int k=0; k< shapes; k++)
	{
		
		int point_count = lengths[k];
		float type = types[k];
		
		printf("shape %i number of points: %i type %f \n",k,point_count, type);
		
		std::vector<float> px;
		std::vector<float> py;
		std::vector<float> val;
		
		for(int j = 0;j < point_count; j++)
		{
			float pxj = point[index_point*2];
			float pyj = point[index_point*2 + 1];
			float valj = values[index_values];
			
			px.push_back(pxj);
			py.push_back(pyj);
			val.push_back(valj);
			
			index_values ++;
			index_point ++;
			
		}
		
		//polyline 
		if(type > -0.5 && type < 0.5)
		{
			//for each line
			
			  //make a series of induvidual line segments
			for(int i = 0; i < point_count-1; i++)
			{
				LSMVector2 p1 = LSMVector2(px.at(i),py.at(i));
				LSMVector2 p2 = LSMVector2(px.at(i+1),py.at(i+1));
				float val0 = val.at(i);
				float val1 = val.at(i+1);

				float ulx = Mask->West();
				float uly = Mask->North();
				float brx=  ulx + ((float)(Mask->nrCols()) * Mask->cellSizeX());
				float bry=  uly + ((float)(Mask->nrRows()) * Mask->cellSizeY());

				float xmin = ulx;
				float xmax = brx;
				float ymin = uly;
				float ymax = bry;
				if(xmax < xmin)
				{
					float temp = xmax;
					xmax = xmin; xmin = temp;
				}
				if(ymax < ymin)
				{
					float temp = ymax;
					ymax = ymin; ymin = temp;
				}
				
				printf("point %f %f %f %f ",p1.x,p1.y,p2.x,p2.y);
				printf("map %f %f %f %f ",ulx,uly,brx,bry);
				//see if the points are outside of the bounds of the map
				if((p1.x < xmin && p2.x < xmin) || (p1.x > xmax && p2.x > xmax))
				{
					continue;
				}
				if((p1.y < ymin && p2.y < ymin) || (p1.y > ymax && p2.y > ymax))
				{
					continue;
				}

				//if so, cut the line by the edge of the map
				if(p1.x < xmin)
				{
					//move p1 to minx;
					LSMVector2 dir = (p2-p1).Normalize();
					float units = std::fabs(xmin - p1.x)/std::max(1e-20f,std::fabs(dir.x));
					p1.x = xmin;
					p1.y = p1.y + units * dir.y;
				}
				if(p2.x > xmax)
				{
					//move p1 to minx;
					LSMVector2 dir = (p2-p1).Normalize();
					float units = std::fabs(xmax - p2.x)/std::max(1e-20f,std::fabs(dir.x));
					p2.x = xmax;
					p2.y = p2.y - units * dir.y;
				}

				if(p2.x < xmin)
				{
					//move p1 to minx;
					LSMVector2 dir = (p1-p2).Normalize();
					float units = std::fabs(xmin - p2.x)/std::max(1e-20f,std::fabs(dir.x));
					p2.x = xmin;
					p2.y = p2.y + units * dir.y;
				}
				if(p1.x > xmax)
				{
					//move p1 to minx;
					LSMVector2 dir = (p1-p2).Normalize();
					float units = std::fabs(xmax - p1.x)/std::max(1e-20f,std::fabs(dir.x));
					p1.x = xmax;
					p1.y = p1.y - units * dir.y;
				}



				if(p1.y < ymin)
				{
					//move p1 to miny;
					LSMVector2 dir = (p2-p1).Normalize();
					float units = std::fabs(ymin - p1.y)/std::max(1e-20f,std::fabs(dir.y));
					p1.y = ymin;
					p1.x = p1.x + units * dir.x;
				}
				if(p2.y > ymax)
				{
					//move p1 to miny;
					LSMVector2 dir = (p2-p1).Normalize();
					float units = std::fabs(ymax - p2.y)/std::max(1e-20f,std::fabs(dir.y));
					p2.y = ymax;
					p2.x = p2.x - units * dir.x;
				}

				if(p2.y < ymin)
				{
					//move p1 to minx;
					LSMVector2 dir = (p1-p2).Normalize();
					float units = std::fabs(ymin - p2.y)/std::max(1e-20f,std::fabs(dir.y));
					p2.y = ymin;
					p2.x = p2.x + units * dir.x;
				}
				if(p1.y > ymax)
				{
					//move p1 to minx;
					LSMVector2 dir = (p1-p2).Normalize();
					float units = std::fabs(ymax - p1.y)/std::max(1e-20f,std::fabs(dir.y));
					p1.y = ymax;
					p1.x = p1.x - units * dir.x;
				}
				//use bresenhams line algorithm, as it produces single-width lines that work better for flux counting

				{
					int r0 = (p1.y - DEM->north())/DEM->cellSizeY();
					int c0 = (p1.x - DEM->west())/DEM->cellSizeX();
					int r1 = (p2.y - DEM->north())/DEM->cellSizeY();
					int c1 = (p2.x - DEM->west())/DEM->cellSizeX();
					
					
					printf("line segment: %i %i %i %i  ", r0,c0,r1,c1);
	

					float celldist = std::sqrt(((float)(r1)-(float)(r0)) *((float)(r1)-(float)(r0)) + ((float)(c1)-(float)(c0))*((float)(c1)-(float)(c0)));
					r0 = std::max(0, std::min(DEM->nrRows()-1,r0));
					r1 = std::max(0, std::min(DEM->nrRows()-1,r1));
					c0 = std::max(0, std::min(DEM->nrCols()-1,c0));
					c1 = std::max(0, std::min(DEM->nrCols()-1,c1));

					Bresenham_Drawline(c0,r0,c1,r1,[DEM, celldist,val1,val0,r0,c0,c1,r1](int c, int r){

						r = std::max(0, std::min(DEM->nrRows()-1,r));
						c = std::max(0, std::min(DEM->nrCols()-1,c));
						
						float dist0 = std::sqrt(((float)(r)-(float)(r0)) *((float)(r)-(float)(r0)) + ((float)(c)-(float)(c0))*((float)(c)-(float)(c0)));
						
						double valfinal = val1 * (dist0/std::max(0.00001f,celldist)) + val0 * (1.0- (dist0/std::max(0.00001f,celldist)));
						DEM->data[r][c] += valfinal;
					});
				}
			}
	
	
			
		}
		//polygon
		if(type > 0.5 && type < 1.5&& px.size() > 2 && py.size() > 2 && val.size() > 0)
		{
			printf("rasterize polygon \n");
			float rmin = 0;
			float rmax = 0;
			float cmin = 0;
			float cmax = 0;
			
			bool is_first = false;
			for(int i = 0; i < point_count; i++)
			{
				if(is_first)
				{
					is_first=false;
					rmin = py.at(i);
					rmax = py.at(i);
					cmin = px.at(i);
					cmax = px.at(i);
					
				}else{
					rmin = std::min(rmin,py.at(i));
					rmax = std::max(rmax,py.at(i));
					cmin = std::min(cmin,px.at(i));
					cmax = std::max(cmax,px.at(i));
				}
				
			}
			
			
			rmin = std::max(0.0f, std::min((float)(DEM->nrRows()-1.0f),rmin));
			rmax = std::max(0.0f, std::min((float)(DEM->nrRows()-1.0f),rmax));
			cmin = std::max(0.0f, std::min((float)(DEM->nrCols()-1.0f),cmin));
			cmax = std::max(0.0f, std::min((float)(DEM->nrCols()-1.0f),cmax));
			
			float valfinal = val.at(0);
			
			float ptargetx = rmax + 1.0;
			float ptargety = cmax + 1.0;
			
			for(int r = rmin; r < rmax; r++)
			{
				for(int c = cmin; c < cmax; c++)
				{
					int intersections = 0;
					float pxh = c;
					float pyh = r;
					
					for(int i = 0; i < point_count; i++)
					{
						int ip = i+1;
						if(ip > point_count -1)
						{
							ip = 0;
						}
						
						float lsx1 = px.at(i);
						float lsx2 = px.at(ip);
						float lsy1 = py.at(i);
						float lsy2 = py.at(ip);
						
						//is there an intersection
						bool intersect = segmentIntersect(LSMVector2(pxh,pyh),LSMVector2(ptargetx,ptargety),LSMVector2(lsx1,lsy1),LSMVector2(lsx2,lsy2));
						if(intersect)
						{
							intersections ++;
						}
					}
				
					bool pixel_is_in = (intersections % 2 != 0);
					
					if(pixel_is_in)
					{
						DEM->data[r][c] += valfinal;
					}
					
				}
				
			}

		
			
		}
		
		//circle
		if(type > 1.5 && type < 2.5 && px.size() > 1 && py.size() > 1 && val.size() > 0)
		{
			LSMVector2 p1 = LSMVector2(px.at(0),py.at(0));
			LSMVector2 p2 = LSMVector2(px.at(1),py.at(1));
			
			float ptargetx = p1.x;
			float ptargety = p1.y;
			
			float valfinal = val.at(0);
			float radius = std::sqrt((p1.x - p2.x)*(p1.x-p2.x) +(p1.y - p2.y)*(p1.y-p2.y)) ;
			float radiussq = radius * radius;
			
			float rmin = p1.y - radius;
			float rmax = p1.y + radius;
			float cmin = p1.x - radius;
			float cmax = p1.x + radius;
			
			rmin = std::max(0.0f, std::min((float)(DEM->nrRows()-1.0f),rmin));
			rmax = std::max(0.0f, std::min((float)(DEM->nrRows()-1.0f),rmax));
			cmin = std::max(0.0f, std::min((float)(DEM->nrCols()-1.0f),cmin));
			cmax = std::max(0.0f, std::min((float)(DEM->nrCols()-1.0f),cmax));
					
			for(int r = rmin; r < rmax; r++)
			{
				for(int c = cmin; c < cmax; c++)
				{
					float px = c ;
					float py = r;
					
					float dist_orgsq = (px - ptargetx)*(px - ptargetx)  + (py - ptargety) *(py - ptargety) ;
					
					if(radiussq >= dist_orgsq)
					{
						DEM->data[r][c] += valfinal;
					}
					
				}
			}
		}
		
		
	}	
	
	for(int r = 0; r < DEM->Rows(); r++) {
            for(int c = 0; c < DEM->Cols(); c++)
            {
					int index = r* DEM->Cols() + c;
                if(!pcr::isMV(DEM->data[r][c]))
                {
					
					ret[index] = DEM->data[r][c];
				}
			}
	}
	
}

void EMSCRIPTEN_KEEPALIVE SaxtonKSat(float * ret, float * in_SILT, float * in_CLAY, float * in_ORGANIC, int w, int h, float in_dx, float densityfactor )
{
	cTMap * SILT = new cTMap(in_SILT,w,h,0.0,0.0, in_dx,- in_dx);
	cTMap * CLAY = new cTMap(in_CLAY,w,h,0.0,0.0, in_dx,- in_dx);
	cTMap * ORGANIC = new cTMap(in_ORGANIC,w,h,0.0,0.0, in_dx,- in_dx);
	
    for(int r = 0; r < SILT->Rows(); r++) {
            for(int c = 0; c < SILT->Cols(); c++)
            
    {

			int index = r* SILT->Cols() + c;
        float C = std::min(0.9f,std::max(0.1f,CLAY->data[r][c]/1000.0f));
		float Si = std::min(0.9f,std::max(0.1f,SILT->data[r][c]/1000.0f));
        float S = std::min(0.9f,std::max(0.1f,1.0f-C-Si));
        float OM = std::min(1000.0f,std::max(0.1f,2.7f*ORGANIC->data[r][c]/1000.0f));
        if(CLAY->data[r][c] < 0.01 && SILT->data[r][c]  < 0.01)
		{
			ret[index] = 0.0;
		}else{
			
			float Gravel = 0.1;

			float M1500 =-0.024*S+0.487*C+0.006*OM+0.005*S*OM-0.013*C*OM+0.068*S*C+0.031;
			float M1500adj =M1500+0.14*M1500-0.02;
			float M33  =-0.251*S+0.195*C+0.011*OM+0.006*S*OM-0.027*C*OM+0.452*S*C+0.299;
			float M33adj = M33+(1.283*M33*M33-0.374*M33-0.015);
			float PM33    = 0.278*S+0.034*C+0.022*OM-0.018*S*OM-0.027*C*OM-0.584*S*C+0.078;
			float PM33adj = PM33+(0.636*PM33-0.107);
			float SatPM33 = M33adj + PM33adj;
			float SatSadj = -0.097*S+0.043;
			float SadjSat = SatPM33  + SatSadj;
			float Dens_om = (1.0-SadjSat)*2.65;
			float Dens_comp = Dens_om * densityfactor;
			float PORE_comp =(1.0-Dens_om/2.65)-(1.0-Dens_comp/2.65);
			float M33comp = M33adj - 0.2*PORE_comp;
			float thetast = 1.0-(Dens_comp/2.65);
			float PoreMcomp = thetast-M33comp;
			float LAMBDA = (std::log(M33comp)-std::log(M1500adj))/(std::log(1500.0)-std::log(33.0));
			float GravelRedKsat =(1.0-Gravel)/(1.0-Gravel*(1.0-1.5*(Dens_comp/2.65)));

			float Ksat1 =(1930.0*std::pow((PoreMcomp),(3.0-LAMBDA))*GravelRedKsat);

			float BD1 = Gravel*2.65+(1.0f-Gravel)*Dens_comp;
			float WP1 = M1500adj;
			float FC1 = M33adj;
			float PAW1 = (M33adj - M1500adj)*(1.0f-Gravel);

			float bB = (std::log(1500.0f)-std::log(33.0f))/((std::log(FC1)-std::log(WP1)));
			float aA = exp(std::log(33.0f) + bB*std::log(FC1));

			float B = bB;
			float A = aA;

			float psi1 = std::max(10.0f,(float) std::pow(aA*(FC1 + 0.7 * (thetast - FC1)),-bB));
			float thetai1 = (FC1 + 0.7 * (thetast - FC1));
		
			if(!pcr::isMV(Ksat1))
			{
				
				ret[index] = Ksat1;
			}else{
					
				pcr::setMV(ret[index]);
			}
		}
	}
	}
	
		
}

void EMSCRIPTEN_KEEPALIVE do_algo_demfill(float * ret, float * dem, int w, int h, float in_dx, float delta, int iters)
{
	
	cTMap * DEM = new cTMap(dem,w,h,0.0,0.0, in_dx,- in_dx);
	cTMap * DEM2 = AS_SpreadMonotonicReconstruct(DEM,/*0.00001*/delta,iters);

	for(int r = 0; r < DEM->Rows(); r++) {
            for(int c = 0; c < DEM->Cols(); c++)
            {
					int index = r* DEM->Cols() + c;
                if(!pcr::isMV(DEM->data[r][c]))
                {
					
					ret[index] = DEM2->data[r][c];
				}else{
					pcr::setMV(ret[index]);
				}
			}
	}
	
	delete DEM2;
	
}

void EMSCRIPTEN_KEEPALIVE do_algo_accu(float * ret, float * dem, int w, int h, float in_dx, float delta, int iters)
{
	
		cTMap * DEM = new cTMap(dem,w,h,0.0,0.0, in_dx,- in_dx);
	cTMap * DEM2 = AS_SpreadMonotonicReconstruct(DEM,/*0.00001*/delta,iters);
	
	cTMap * GradX1 = DEM->GetCopy();
	cTMap * GradX2 = DEM->GetCopy();
	cTMap * GradY1 = DEM->GetCopy();
	cTMap * GradY2 = DEM->GetCopy();
	
	
	
	//get new normalized flux network
        //#pragma omp parallel for collapse(2)
        for (int r = 0; r < DEM->Rows(); r++) {
            for (int c = 0; c < DEM->Cols(); c++) {
                if (!pcr::isMV(DEM->data[r][c])) {

                    float gx1 =std::min(0.0,-UF2D_Derivative_scaled(DEM2,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_L));
                    float gx2 =std::max(0.0,-UF2D_Derivative_scaled(DEM2,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_R));
                    float gy1 =std::min(0.0,-UF2D_Derivative_scaled(DEM2,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_L));
                    float gy2 =std::max(0.0,-UF2D_Derivative_scaled(DEM2,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_R));

                    float gtot = std::max(1e-12f,std::fabs(gx1) + std::fabs(gx2) + std::fabs(gy1) + std::fabs(gy2));

                    GradX1->data[r][c] = gx1/gtot;
                    GradX2->data[r][c] = gx2/gtot;
                    GradY1->data[r][c] = gy1/gtot;
                    GradY2->data[r][c] = gy2/gtot;

                } else
                {
                    pcr::setMV(GradX1->data[r][c]);
                    pcr::setMV(GradX2->data[r][c]);
                    pcr::setMV(GradY1->data[r][c]);
                    pcr::setMV(GradY2->data[r][c]);
                }
            }
        }
		
		
        //do flow accumulation
        cTMap * q;


        //MaskedRaster<float> raster_data4();
        cTMap *Rain = DEM->GetCopy1();

				
		q = AS_SpreadFlowMD2(Rain,GradX1,GradX2,GradY1,GradY2,iters);
			
			for(int r = 0; r < DEM->Rows(); r++) {
            for(int c = 0; c < DEM->Cols(); c++)
            {
					int index = r* DEM->Cols() + c;
                if(!pcr::isMV(DEM->data[r][c]))
                {
					
					ret[index] = q->data[r][c];
				}else{
					pcr::setMV(ret[index]);
				}
			}
	}
	
	delete q;
	delete Rain;
	delete GradX1;
	delete GradX2;
	delete GradY1;
	delete GradY2;
	delete DEM2;
	
}

void EMSCRIPTEN_KEEPALIVE do_algo_ldd(float * ret, float * dem, int w, int h, float in_dx, float delta, int iters)
{
	
	cTMap * DEM = new cTMap(dem,w,h,0.0,0.0, in_dx,- in_dx);
	cTMap * DEM2 = AS_SpreadMonotonicReconstruct(DEM,/*0.00001*/delta,iters);

	
	//get new normalized flux network
        //#pragma omp parallel for collapse(2)
        for (int r = 0; r < DEM->Rows(); r++) {
            for (int c = 0; c < DEM->Cols(); c++) {
                int index = r* DEM->Cols() + c;
				
				if (!pcr::isMV(DEM->data[r][c])) {

                    float gx1 =std::min(0.0,-UF2D_Derivative_scaled(DEM2,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_L));
                    float gx2 =std::max(0.0,-UF2D_Derivative_scaled(DEM2,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_R));
                    float gy1 =std::min(0.0,-UF2D_Derivative_scaled(DEM2,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_L));
                    float gy2 =std::max(0.0,-UF2D_Derivative_scaled(DEM2,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_R));

                  
				   if(gx1 < 0.0)
				   {
					   if(gy1 < 0.0)
					   {
						   if(gy1 < gx1)
						   {
							   ret[index] = 8;
						   }else{
							   ret[index] = 4;
						   }
					   }else if(gy2 > 0.0)
					   {
						   if(-gy2 < gx1)
						   {
							   ret[index] = 2;
						   }else{
							   ret[index] = 4;
						   }
					   }else{
						   ret[index] = 4;
						   
					   }
				   }else if(gx2 > 0.0)
				   {
					   if(gy1 < 0.0)
					   {
						   if(gy1 < -gx2)
						   {
							   ret[index] = 8;
						   }else{
							   ret[index] = 6;
						   }
					   }else if(gy2 > 0.0)
					   {
						   if(gy2 > gx2)
						   {
							   ret[index] = 2;
						   }else{
							   ret[index] = 6;
						   }
					   }else{
						   ret[index] = 6;
						   
					   }
				   }else if(gy1 <0.0)
				   {
					   ret[index] = 8;
				   }else if(gy2 >0.0)
				   {
					   ret[index] = 2; 
				   }
                } else
                {
                    pcr::setMV(ret[index]);
                }
            }
        }
		
	
	delete DEM2;
	
	
}


 void EMSCRIPTEN_KEEPALIVE do_algo_channeldim(float * width, float * height, float * dem, int w, int h, float in_dx, float delta, int iters, float channel_a, float channel_b, float channel_c, float channel_d)
{
		cTMap * DEM = new cTMap(dem,w,h,0.0,0.0, in_dx,- in_dx);
	cTMap * DEM2 = AS_SpreadMonotonicReconstruct(DEM,/*0.00001*/delta,iters);
	cTMap * GradX1 = DEM->GetCopy();
	cTMap * GradX2 = DEM->GetCopy();
	cTMap * GradY1 = DEM->GetCopy();
	cTMap * GradY2 = DEM->GetCopy();
	
	
	
	//get new normalized flux network
        //#pragma omp parallel for collapse(2)
        for (int r = 0; r < DEM->Rows(); r++) {
            for (int c = 0; c < DEM->Cols(); c++) {
                if (!pcr::isMV(DEM->data[r][c])) {

                    float gx1 =std::min(0.0,-UF2D_Derivative_scaled(DEM2,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_L));
                    float gx2 =std::max(0.0,-UF2D_Derivative_scaled(DEM2,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_R));
                    float gy1 =std::min(0.0,-UF2D_Derivative_scaled(DEM2,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_L));
                    float gy2 =std::max(0.0,-UF2D_Derivative_scaled(DEM2,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_R));

                    float gtot = std::max(1e-12f,std::fabs(gx1) + std::fabs(gx2) + std::fabs(gy1) + std::fabs(gy2));

                    GradX1->data[r][c] = gx1/gtot;
                    GradX2->data[r][c] = gx2/gtot;
                    GradY1->data[r][c] = gy1/gtot;
                    GradY2->data[r][c] = gy2/gtot;

                } else
                {
                    pcr::setMV(GradX1->data[r][c]);
                    pcr::setMV(GradX2->data[r][c]);
                    pcr::setMV(GradY1->data[r][c]);
                    pcr::setMV(GradY2->data[r][c]);
                }
            }
        }
		
		
        //do flow accumulation
        cTMap * q;


        //MaskedRaster<float> raster_data4();
        cTMap *Rain = DEM->GetCopy1();

				
		q = AS_SpreadFlowMD2(Rain,GradX1,GradX2,GradY1,GradY2,iters);
			
			for(int r = 0; r < DEM->Rows(); r++) {
            for(int c = 0; c < DEM->Cols(); c++)
            {
					int index = r* DEM->Cols() + c;
                if(!pcr::isMV(DEM->data[r][c]))
                {
					
					width[index] = channel_a *std::pow((q->data[r][c]*in_dx*in_dx/1000000.0),channel_b);
					height[index] = channel_c *std::pow((q->data[r][c]*in_dx*in_dx/1000000.0),channel_d);
				}else{
					width[index] = 0.0;
					height[index] = 0.0;
				}
			}
	}
	
	delete q;	
	delete Rain;
	delete GradX1;
	delete GradX2;
	delete GradY1;
	delete GradY2;
	delete DEM2;
	
	
}


void EMSCRIPTEN_KEEPALIVE do_algo_fastflood(float*ret,float*data_dem,float*data_man, float*data_rain, int w, int h, float in_dx, float duration, int do_depr, int correct_hq, int pressure_iters, int iterations, int in_fluxiters, float delta, float diffusivity_coefficient)
{
	
	
	cTMap * DEM_ORG = new cTMap(data_dem,w,h,0.0,0.0, in_dx,- in_dx);
	cTMap * N = new cTMap(data_man,w,h,0.0,0.0, in_dx,- in_dx);
	cTMap * Rain_ORG = new cTMap(data_rain,w,h,0.0,0.0, in_dx,- in_dx);
	bool corr = correct_hq > 0;
	bool do_depressions = do_depr > 0;
	int piters = pressure_iters;
	int itersp = iterations;
	
	
	printf("do flood algorithm : %i %i %i %i %f",  do_depressions, corr, piters, iterations,duration);
	
	//keep track of previous total (compensated) discharge
    //and of previous water height
    cTMap * DEM = DEM_ORG->GetCopy();
    cTMap * WH_TOT = DEM->GetCopy0();
    cTMap * WH_PREV = DEM->GetCopy0();
    cTMap * QSS_PREV = DEM->GetCopy0();
    cTMap * Rain = Rain_ORG->GetCopy();

	if(piters <= 1)
    {
        piters = 1;
    }
	
	emscripten_run_script("increment_progress_fastflood()");
	
	
	for(int ip = 0; ip < piters; ip++)
    {
        if(itersp < 0)
        {
            itersp = 40;
        }
        //first we get the right gradient maps

        //#pragma omp parallel for collapse(2)
        for (int r = 0; r < DEM->Rows(); r++) {
            for (int c = 0; c < DEM->Cols(); c++) {
                if (!pcr::isMV(DEM->data[r][c])) {

                    DEM->data[r][c] = DEM_ORG->data[r][c] + WH_TOT->data[r][c];

                }
            }
        }

        float dx = DEM->cellSizeX();
        int iters= in_fluxiters * std::max(1,std::max(DEM->nrCols(),DEM->nrRows())/1000);
        cTMap * GradX1 = AS_SlopeX1(DEM); GradX1->setcellSizeX(in_dx); GradX1->setcellSizeY(-in_dx);
        cTMap * GradY1 = AS_SlopeY1(DEM); GradY1->setcellSizeX(in_dx); GradY1->setcellSizeY(-in_dx);
        cTMap * GradX2 = AS_SlopeX2(DEM); GradX2->setcellSizeX(in_dx); GradX2->setcellSizeY(-in_dx);
        cTMap * GradY2 = AS_SlopeY2(DEM); GradY2->setcellSizeX(in_dx); GradY2->setcellSizeY(-in_dx);
        cTMap * one = DEM->GetCopy1();

        //get a map with seed points for corrected dem
        //MaskedRaster<float> raster_data3(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
        cTMap *Seeds = new cTMap(nullptr, DEM->data.nr_cols(), DEM->data.nr_rows(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());//std::move(raster_data3),DEM->projection(),"");

        int r = 0,c = 0;
        int seeds = 0;
        float mindem = AS_MapMinimumRed(DEM);

        //#pragma omp parallel for collapse(2)
        for (r = 0; r < DEM->Rows(); r++) {
            for (c = 0; c < DEM->Cols(); c++) {
                if (!pcr::isMV(DEM->data[r][c])) {

                    if(ip == 0)
                    {
                        Rain->data[r][c] = (1.0/((float)(piters)))*(dx*dx*Rain_ORG->data[r][c])/3600000.0;
                    }
                    //if dem < smallest dem, or if it is an edge cell, make it a seed for the new dem
                    if(OUTORMV(DEM,r,c) || OUTORMV(DEM,r,c+1) || OUTORMV(DEM,r,c-1) || OUTORMV(DEM,r+1,c) || OUTORMV(DEM,r-1,c) || (DEM->data[r][c] < mindem + 1e-6))
                    {
                        seeds ++;
                        Seeds->data[r][c] =1.0;
                    }else
                    {
                        Seeds->data[r][c] =0.0;
                    }
                }else
                {
                    Seeds->data[r][c] =0.0;
                }
            }
        }

		emscripten_run_script("increment_progress_fastflood()");
		
        //then do the elevation model fix
        cTMap * DEM2;

        cTMap * DEMD;
        cTMap * DEMDM;
        cTMap * DEMDID;
        //get depressions

        std::vector<int> depressionids;
        std::vector<float> depressionvols;
        std::vector<float> depressioncvols;

        std::vector<std::thread> workers1;
        workers1.push_back(std::thread([&DEM2,DEM,Seeds,Rain,N,GradX1,GradX2,GradY1,GradY2,iters,delta]()
        {
			
            DEM2 = AS_SpreadMonotonicReconstruct(DEM,/*0.00001*/delta,iters);//AS_SpreadDirectionalAbsMaxMD(Seeds,DEM,GradX1,GradX2,GradY1,GradY2);


        }));
        if(do_depressions)
        {
			//printf("do clump etc.. \n");
            workers1.push_back(std::thread([dx,&depressioncvols,&depressionvols,&depressionids,&DEMDID,DEM,&DEMD,&DEMDM,Rain,N,GradX1,GradX2,GradY1,GradY2,iters]()
            {
				
				//printf("a thread2 started \n");
                //DEMD will contain the depression depth
                //MaskedRaster<float> raster_data5();
                DEMDM = new cTMap(nullptr,  DEM->data.nr_cols(),DEM->data.nr_rows(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());//std::move(raster_data5),DEM->projection(),"");

                DEMD = AS_SpreadDepressionFind(DEM);
				
                int depressioncells = 0;
                 for (int r = 0; r < DEM->Rows(); r++) {
                     for (int c = 0; c < DEM->Cols(); c++) {
                         if (!pcr::isMV(DEM->data[r][c])) {
                             float valn = DEMD->data[r][c]-DEM->data[r][c];

                             if(valn > 0.0001)
                             {
                                 DEMDM->data[r][c] = DEMD->data[r][c];
                                 depressioncells++;

                             }else
                             {
                                 pcr::setMV(DEMDM->data[r][c]);
                             }
                             DEMD->data[r][c] = valn;
                         }else
                         {
                             pcr::setMV(DEMDM->data[r][c]);
                         }
                     }
                 }
				//printf("a thread2 clump \n");
                 DEMDID = AS_Clump(DEMDM,0.1,true);

				//printf("a thread2 clumpd \n");

                 //get depression volumes
                 float cellarea = dx * dx;

                 depressioncells = 0;
                 for(int r = 0; r < DEMDID->data.nr_rows();r++)
                 {
                     for(int c = 0; c < DEMDID->data.nr_cols();c++)
                     {
                         if(!pcr::isMV(DEMDID->data[r][c]))
                         {
                             depressioncells ++;
                             bool found = false;
                             int class_current = (int)DEMDID->data[r][c];
                             //std::cout << "add class " << class_current << " " << depressionids.size() <<  std::endl;

                             if(class_current > (((int)depressionids.size())-5))
                             {
                                 //std::cout << "add class " << class_current << " " << depressionids.size() <<  std::endl;
                                 int oldsize = depressionids.size();
                                 for(int i = 0; i < (5 + class_current - depressionids.size()); i++)
                                 {
                                     //std::cout << "add" << std::endl;
                                     depressionids.push_back(oldsize + i);
                                     depressionvols.push_back(0.0);
                                 }

                             }
                             {
                                 depressionvols.at(class_current) += cellarea * DEMD->data[r][c];
                             }
                         }

                     }
                 }
                 for(int i =0; i < depressionvols.size(); i++)
                 {
                     depressioncvols.push_back(0.0);
                 }

				//printf("a thread2 done \n");

            }));
        }
		
		emscripten_run_script("increment_progress_fastflood()");

       // Looping every thread via for_each
       // Then, joins one by one, and this works like barrier
       std::for_each(workers1.begin(), workers1.end(), [](std::thread &t)
       {
           t.join();
       });
	   //printf("do clump etc..  done \n");

        //get new normalized flux network
        //#pragma omp parallel for collapse(2)
        for (r = 0; r < DEM->Rows(); r++) {
            for (c = 0; c < DEM->Cols(); c++) {
                if (!pcr::isMV(DEM->data[r][c])) {

                    float gx1 =std::min(0.0,-UF2D_Derivative_scaled(DEM2,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_L));
                    float gx2 =std::max(0.0,-UF2D_Derivative_scaled(DEM2,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_R));
                    float gy1 =std::min(0.0,-UF2D_Derivative_scaled(DEM2,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_L));
                    float gy2 =std::max(0.0,-UF2D_Derivative_scaled(DEM2,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_R));

                    float gtot = std::max(1e-12f,std::fabs(gx1) + std::fabs(gx2) + std::fabs(gy1) + std::fabs(gy2));

                    GradX1->data[r][c] = gx1/gtot;
                    GradX2->data[r][c] = gx2/gtot;
                    GradY1->data[r][c] = gy1/gtot;
                    GradY2->data[r][c] = gy2/gtot;

                } else
                {
                    pcr::setMV(GradX1->data[r][c]);
                    pcr::setMV(GradX2->data[r][c]);
                    pcr::setMV(GradY1->data[r][c]);
                    pcr::setMV(GradY2->data[r][c]);
                }
            }
        }
		
		
        //do flow accumulation
        cTMap * q;


        //MaskedRaster<float> raster_data4();
        cTMap *RainN = new cTMap(nullptr, DEM->data.nr_cols(), DEM->data.nr_rows(),DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());//std::move(raster_data4),DEM->projection(),"");
        //MaskedRaster<float> raster_data5(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());
        cTMap *Vel = new cTMap(nullptr, DEM->data.nr_cols(),  DEM->data.nr_rows(),DEM->data.north(), DEM->data.west(), DEM->data.cell_size(),DEM->data.cell_sizeY());//std::move(raster_data5),DEM->projection(),"");

        cTMap * accN;
        cTMap * accqreal;
        cTMap * acc1;
        cTMap * accacc1;
        cTMap * accvel;

        // vector container stores threads
        std::vector<std::thread> workers;
		

		emscripten_run_script("increment_progress_fastflood()");

        if(!do_depressions)
        {
			
            workers.push_back(std::thread([&q,&accN,DEM,RainN,Rain,N,GradX1,GradX2,GradY1,GradY2,iters]()
            {
				
				q = AS_SpreadFlowMD2(Rain,GradX1,GradX2,GradY1,GradY2,iters);

            }));

            workers.push_back(std::thread([&accN,DEM,RainN,Rain,N,GradX1,GradX2,GradY1,GradY2,iters]()
            {
				
				for (int r = 0; r < DEM->Rows(); r++) {
                    for (int c = 0; c < DEM->Cols(); c++) {
                        if (!pcr::isMV(DEM->data[r][c])) {

                            RainN->data[r][c] = Rain->data[r][c] * N->data[r][c];
                        }
                    }
                }

                accN = AS_SpreadFlowMD2(RainN,GradX1,GradX2,GradY1,GradY2,iters);


            }));
            workers.push_back(std::thread([one,DEM,&acc1,&accacc1,N,GradX1,GradX2,GradY1,GradY2,iters]()
            {
				
				
                acc1 = AS_SpreadFlowMD2(one,GradX1,GradX2,GradY1,GradY2,iters);
				
                accacc1 = AS_SpreadFlowMD2(acc1,GradX1,GradX2,GradY1,GradY2,iters);
				
				

            }));
            workers.push_back(std::thread([&Vel,DEM,&accvel,RainN,Rain,N,GradX1,GradX2,GradY1,GradY2,iters]()
            {
				
				for (int r = 0; r < DEM->Rows(); r++) {
                    for (int c = 0; c < DEM->Cols(); c++) {
                        if (!pcr::isMV(DEM->data[r][c])) {

                            //9.68*(accuflux.tif/1e6)**0.32
                            float slope = 0.5*std::max(1e-12f,std::fabs(GradX1->data[r][c]) + std::fabs(GradX2->data[r][c]) + std::fabs(GradY1->data[r][c]) + std::fabs(GradY2->data[r][c]));

                            Vel->data[r][c] = Rain->data[r][c] *0.1* sqrt(slope)/std::max(0.001f,N->data[r][c]);
                        }
                    }
                }
				
				
                accvel = AS_SpreadFlowMD2(Vel,GradX1,GradX2,GradY1,GradY2,iters);


            }));
        }else
        {
			
				//printf("start depressed workers \n");
			
            workers.push_back(std::thread([&DEMDID,&depressioncvols,&depressionvols,&q,&accN,DEM,RainN,Rain,N,GradX1,GradX2,GradY1,GradY2,iters]()
            {
				
                q = AS_SpreadFlowMD2(Rain,GradX1,GradX2,GradY1,GradY2,iters);


            }));
            workers.push_back(std::thread([duration,&accqreal,&DEMDID,&depressioncvols,&depressionvols,&q,&accN,DEM,RainN,Rain,N,GradX1,GradX2,GradY1,GradY2,iters]()
            {
				
                accqreal = AS_SpreadFlowMD2Depression(Rain,GradX1,GradX2,GradY1,GradY2,iters,DEMDID,depressionvols,depressioncvols,true,duration * 60.0 * 60.0);


            }));

            workers.push_back(std::thread([&DEMDID,&depressioncvols,&depressionvols,&accN,DEM,RainN,Rain,N,GradX1,GradX2,GradY1,GradY2,iters]()
            {
				
                for (int r = 0; r < DEM->Rows(); r++) {
                    for (int c = 0; c < DEM->Cols(); c++) {
                        if (!pcr::isMV(DEM->data[r][c])) {

                            RainN->data[r][c] = Rain->data[r][c] * N->data[r][c];
                        }
                    }
                }
				
                accN = AS_SpreadFlowMD2(RainN,GradX1,GradX2,GradY1,GradY2,iters);


            }));
            workers.push_back(std::thread([&DEMDID,&depressioncvols,&depressionvols,one,DEM,&acc1,&accacc1,N,GradX1,GradX2,GradY1,GradY2,iters]()
            {
				
                acc1 = AS_SpreadFlowMD2(one,GradX1,GradX2,GradY1,GradY2,iters);
                accacc1 = AS_SpreadFlowMD2(acc1,GradX1,GradX2,GradY1,GradY2,iters);
				

            }));
            workers.push_back(std::thread([&DEMDID,&depressioncvols,&depressionvols,&Vel,DEM,&accvel,RainN,Rain,N,GradX1,GradX2,GradY1,GradY2,iters]()
            {
                for (int r = 0; r < DEM->Rows(); r++) {
                    for (int c = 0; c < DEM->Cols(); c++) {
                        if (!pcr::isMV(DEM->data[r][c])) {

                            //9.68*(accuflux.tif/1e6)**0.32
                            float slope = 0.5*std::max(1e-12f,std::fabs(GradX1->data[r][c]) + std::fabs(GradX2->data[r][c]) + std::fabs(GradY1->data[r][c]) + std::fabs(GradY2->data[r][c]));

                            Vel->data[r][c] = Rain->data[r][c] *0.1 * sqrt(slope)/std::max(0.001f,N->data[r][c]);
                        }
                    }
                }
                accvel = AS_SpreadFlowMD2(Vel,GradX1,GradX2,GradY1,GradY2,iters);
				

            }));
        }


        // Looping every thread via for_each
        // Then, joins one by one, and this works like barrier
        std::for_each(workers.begin(), workers.end(), [](std::thread &t)
        {
            t.join();
        });
		
		
		//printf("depressed workers done \n");
		
		emscripten_run_script("increment_progress_fastflood()");

        //get average distance, N and velocity
        //#pragma omp parallel for collapse(2)
        for (r = 0; r < DEM->Rows(); r++) {
            for (c = 0; c < DEM->Cols(); c++) {
                if (!pcr::isMV(DEM->data[r][c])) {

                    RainN->data[r][c] = accN->data[r][c]/std::max(0.00001f,q->data[r][c]);
                    Vel->data[r][c] = accvel->data[r][c]/std::max(0.00001f,q->data[r][c]);
                    accacc1->data[r][c] = dx * accacc1->data[r][c]/std::max(1.0f,acc1->data[r][c]);

                    //get steady-state compensation
                    //Map h =  pow( q* N /max(0.001,sqrt(max(0.001,Slope(dem)))),q * 0.0 + (5.0/6.0));


                   

                    float i_dx = dx;
                    float i_L = accacc1->data[r][c] * 3.0/2.0;//initial guess for iteration based on average distance of circular segment to center of circle
                    float i_n = acc1->data[r][c];
                    float i_b = 1.0; //start iteration with linear
                    float i_Lav = accacc1->data[r][c];//average travel distance to outlet

                    bool do_break = false;
                    //this should typically only take a small amount of iterations (<5)
                    for(int i = 0; i < 10; i++)
                    {

                        float Lnew = ((2.0 + i_b) *i_Lav)/(1.0 + i_b); // N
                        float bnew = (-std::log(i_dx/i_L) + std::log(1.0/i_n))/std::log(i_dx/i_L); // 

                        if(((Lnew - i_L) < 0.001) && ((bnew - i_b) < 0.001))
                        {
                            do_break = true;
                        }
                        i_L = Lnew;
                        i_b = bnew;
                        if(do_break)
                        {
                            break;
                        }
                    }
                    if(!std::isnormal(i_b) || !std::isnormal(i_L))
                    {
                        i_b = 1.0;
                        i_L = accacc1->data[r][c] * 3.0/2.0;
                    }

                    i_b = std::max(0.00001f,i_b);
                    i_L = std::max(dx,i_L);

                    float ss_comp_x = duration * 60.0 * 60.0 * Vel->data[r][c]/i_L; //relative event duration
                    float ss_comp;
                    if(ss_comp_x < 1.0)
                    {
						
						float term_diffusivity = std::exp(-diffusivity_coefficient*std::log((1.0/std::min(1.0f,std::max(0.0001f,ss_comp_x)))));
					
                        ss_comp = (1.0 - std::pow(1.0 - ss_comp_x,1.0 + i_b)) * term_diffusivity; //1 - (1 - x)^(1 + b)
                    }else
                    {
                        ss_comp = 1.0;
                    }

                    float SlopeX = UF2D_Derivative(DEM,r,c,UF_DIRECTION_X,1.0,UF_DERIVATIVE_LR);
                    float SlopeY = UF2D_Derivative(DEM,r,c,UF_DIRECTION_Y,1.0,UF_DERIVATIVE_LR);
                    float Slope = std::fabs(SlopeX) + std::fabs(SlopeY);

                    //fill with h, but compensate for any previous flow (due to velocity increase, q increases non-linearly with h)
                    if(!do_depressions)
                    {
                        float q_old = QSS_PREV->data[r][c];
                        float h_old = std::pow((1.0/(dx)) * ss_comp * q_old * N->data[r][c]/std::max(0.001f,Slope),3.0/5.0);
                        one->data[r][c] = std::max(0.0,-h_old + std::pow((1.0/(dx)) * ss_comp * (q->data[r][c] + q_old) * N->data[r][c]/std::max(0.001f,Slope),3.0/5.0));

                        WH_PREV->data[r][c] += one->data[r][c];
                        QSS_PREV->data[r][c] += ss_comp * q->data[r][c];
                    }else
                    {
                        float q_old = QSS_PREV->data[r][c];
                        float h_old = std::pow((1.0/(dx)) * ss_comp * q_old * N->data[r][c]/std::max(0.001f,Slope),3.0/5.0);
                        one->data[r][c] = std::max(0.0,-h_old + std::pow((1.0/(dx)) *ss_comp * (accqreal->data[r][c] + q_old) * N->data[r][c]/std::max(0.001f,Slope),3.0/5.0));
                        WH_PREV->data[r][c] += one->data[r][c];
                        QSS_PREV->data[r][c] += ss_comp *accqreal->data[r][c];
                    }

                    if(!std::isnormal(one->data[r][c]))
                    {
                        //one->data[r][c] = 0.0;
                        //WH_PREV->data[r][c] = 0.0;
                    }
                    if(do_depressions)
                    {

                        //is this cell in a depression?
                        if(!pcr::isMV(DEMDID->data[r][c]))
                        {
                            int id = DEMDID->data[r][c];
                            //to what extent is this depression filled?
                            float frac = depressioncvols.at(id)/ std::max(0.01f,depressionvols.at(id));
                            one->data[r][c] += DEMD->data[r][c] * frac;
                            WH_PREV->data[r][c] += DEMD->data[r][c] * frac;
                        }
                    }

                }
            }
        }

		printf("set flow height \n");
		
		emscripten_run_script("increment_progress_fastflood()");

        //water redistribution for pressure advection
        cTMap * hfinal;
        if(corr)
        {
            if(!do_depressions)
            {
                //if we dont do depressions, we better use the corrected dem as it improves throughflow in low resolution rough terrain
               hfinal = AS_DiffusiveQMaxWave(DEM2,one,itersp,0.15);
            }else
            {
                hfinal = AS_DiffusiveQMaxWave(DEM,one ,itersp,0.15);
            }
        }else
        {
            if(!do_depressions)
            {
                //if we dont do depressions, we better use the corrected dem as it improves throughflow in low resolution rough terrain
               hfinal = AS_DiffusiveMaxWave(DEM2,one ,itersp,0.15);
            }else
            {
                hfinal = AS_DiffusiveMaxWave(DEM,one,itersp,0.15);
            }
        }
		
		emscripten_run_script("increment_progress_fastflood()");
				
        //return flow height
        //get average distance, N and velocity
        //#pragma omp parallel for collapse(2)
        for(r = 0; r < DEM->Rows(); r++) {
            for(c = 0; c < DEM->Cols(); c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {

                    WH_TOT->data[r][c] += hfinal->data[r][c];
                }
            }
        }


        delete RainN;
        delete Vel;
        delete accN;
        delete acc1;
        delete accacc1;
        delete accvel;
        delete DEM2;
        delete Seeds;
        delete q;
        delete one;
        delete GradX1;
        delete GradY1;
        delete GradX2;
        delete GradY2;

        if(do_depressions)
        {
            delete accqreal;
            delete DEMD;
            delete DEMDM;
            delete DEMDID;
        }

        delete hfinal;

    }

    delete QSS_PREV;
    delete WH_PREV;
	
	
	
	for(int r = 0; r < DEM->Rows(); r++) {
            for(int c = 0; c < DEM->Cols(); c++)
            {
					int index = r* DEM->Cols() + c;
                if(!pcr::isMV(DEM->data[r][c]))
                {
					
					ret[index] = WH_TOT->data[r][c];
				}else{
						
					pcr::setMV(ret[index]);
				}
			}
	}
	
    delete DEM;
	
	delete WH_TOT;
	
}
}
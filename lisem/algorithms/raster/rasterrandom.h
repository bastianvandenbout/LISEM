#ifndef RASTERRANDOM_H
#define RASTERRANDOM_H

#include "geo/raster/map.h"
#include "random"
#include "random/perlinnoise.h"

static float RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

static float RandomNormalFloat(float a, float b) {
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(a,b);

    double number = distribution(generator);
    return number;
}

static cTMap * AS_MapRandom(cTMap * Other, float min, float max)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                map->data[r][c] = RandomFloat(min,max);
            }
        }
    }
    return map;
}

static cTMap * AS_MapRandomNormal(cTMap * Other, float min, float max)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    for(int r = 0; r < Other->data.nr_rows();r++)
    {
        for(int c = 0; c < Other->data.nr_cols();c++)
        {
            if(pcr::isMV(Other->data[r][c]))
            {
                pcr::setMV(map->data[r][c]);
            }else
            {
                map->data[r][c] = RandomNormalFloat(min,max);
            }
        }
    }
    return map;

}

static cTMap * AS_MapAreaRandom(cTMap * Other, float min, float max)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    std::vector<int> classes;
    std::vector<float> vals;


    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                bool found = false;
                int class_current = (int)Other->data[r][c];
                int i;
                for(i= 0; i < classes.size(); i++)
                {
                    if(class_current == classes.at(i))
                    {
                        found = true;
                        break;
                    }

                }

                if(found)
                {
                    map->data[r][c] = vals.at(i);
                }else
                {
                    i = classes.size();
                    classes.push_back(class_current);
                    vals.push_back(RandomFloat(min,max));
                    map->data[r][c] = vals.at(i);
                }
            }

        }
    }


    return map;

}

static cTMap * AS_MapAreaRandomNormal(cTMap * Other, float min, float max)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    std::vector<int> classes;
    std::vector<float> vals;


    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                bool found = false;
                int class_current = (int)Other->data[r][c];
                int i;
                for(i= 0; i < classes.size(); i++)
                {
                    if(class_current == classes.at(i))
                    {
                        found = true;
                        break;
                    }

                }

                if(found)
                {
                    map->data[r][c] = vals.at(i);
                }else
                {
                    i = classes.size();
                    classes.push_back(class_current);
                    vals.push_back(RandomNormalFloat(min,max));
                    map->data[r][c] = vals.at(i);
                }
            }

        }
    }


    return map;

}


static cTMap * AS_PerlinNoise(cTMap * Other, float wavelength)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");


    PerlinNoise p(rand());


    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {
            if(!pcr::isMV(Other->data[r][c]))
            {
                map->data[r][c] =p.noise(((float)(c))*map->cellSizeX()/wavelength,((float)(r))*map->cellSizeY()/wavelength,0.0);
            }
        }
    }

    return map;
}



//https://www.shadertoy.com/view/3tdGzn
static cTMap * AS_GaborNoise(cTMap * Other, float dirx, float diry, float scalex, float scaley)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");


    float t = 10.0 * RandomFloat(0.0,10000.0);

    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {


            if(!pcr::isMV(Other->data[r][c]))
            {

                float y = (((float)(r) / ((float)(map->data.nr_rows()))))/scaley;
                float x = (((float)(c) / ((float)(map->data.nr_cols()))))/scalex;


                float _n = 0.0;
                float noise = 1.0;
                float S = 0.0;
                float df = dirx;
                float da = 3.1415927 * diry;
                for(int m= 0; m < 20; m++)
                {
                    _n = noise;
                    float temp = 1.0e3*sin(1.0e3 * m);
                    noise = 2.0 *(temp - std::floor(temp)) -1.0;
                    float angle = da * noise;
                    temp = x * cos(angle) -y * sin(angle);
                    S += sin( (1.0 + df*_n)*temp - t - ((float)(m)));
                }
                S = S/ (2.0*sqrt(20.0));
                map->data[r][c] = S;
            }
        }
    }

    return map;
}



// distance functions
#define LISEM_DISTANCE 0
#define LISEM_SQR_DISTANCE 1
#define LISEM_MINKOWSKI 2
#define LISEM_MANHATTAN 3
#define LISEM_CHEBYCHEV 4
#define LISEM_QUADRATIC 5

// voronoi types
#define LISEM_CLOSEST_1 0
#define LISEM_CLOSEST_2 1
#define LISEM_DIFFERENCE_21 2
#define LISEM_CRACKLE 3

inline static float distance(LSMVector3 a, LSMVector3 b)
{
    return (a - b).length();
}

inline static float sqr_distance(LSMVector3 a, LSMVector3 b)
{
    LSMVector3 d = a - b;
    return d.dot(d);
}

static inline float manhattan(LSMVector3 a, LSMVector3 b)
{
    LSMVector3 d = a - b;
    d.x = fabs(d.x);
    d.y = fabs(d.y);
    d.z = fabs(d.z);
    return d.x + d.y + d.z;
}

static inline float chebychev(LSMVector3 a, LSMVector3 b)
{
    return std::max(std::max(fabs(a.x - b.x), fabs(a.y - b.y)), fabs(a.z - b.z));
}

static inline float quadratic(LSMVector3 a, LSMVector3 b)
{
    LSMVector3 d = a - b;
    return (d.x*d.x+d.y*d.y+d.z*d.z+d.x*d.y+d.x*d.z+d.y*d.z);
}

static inline float minkowski(LSMVector3 a, LSMVector3 b)
{
    LSMVector3 d = a - b;
    float p = 0.5;
    LSMVector3 temp = d;
    temp.x = std::pow(fabs(temp.x),p);
    temp.y = std::pow(fabs(temp.y),p);
    temp.z = std::pow(fabs(temp.z),p);
    float x = temp.dot(LSMVector3(1.0,1.0,1.0));
    return std::pow(x, 1.0 / p);
}

// iq
static inline LSMVector3 random3f( LSMVector3 p )
{
    LSMVector3 temp = LSMVector3(p.dot(LSMVector3(1.0,57.0,113.0)),
                                 p.dot(LSMVector3(57.0,113.0,1.0)),
                                 p.dot(LSMVector3(113.0,1.0,57.0)));

    temp.x = (sin(temp.x) *43758.5453) -std::floor((sin(temp.x) *43758.5453));
    temp.y = (sin(temp.y) *43758.5453) -std::floor((sin(temp.y) *43758.5453));
    temp.z = (sin(temp.z) *43758.5453) -std::floor((sin(temp.z) *43758.5453));

    return temp;
}


static inline float voronoi3(float x, float y, float z,int n_points, int dist_func, int type)
{
    LSMVector3 p = LSMVector3(x,y,z);
    LSMVector3 fp = LSMVector3(x,y,z);
    fp.x = std::floor(fp.x);
    fp.y = std::floor(fp.y);
    fp.z = std::floor(fp.z);


    float d1 = 1./0.;
    float d2 = 1./0.;

    for(int i = -1; i < 2; i++)
    {
        for(int j = -1; j < 2; j++)
        {
            for(int k = -1; k < 2; k++)
            {
                LSMVector3 cur_p = fp + LSMVector3(i, j, k);

                LSMVector3 r = random3f(cur_p);

                float cd = 0.0;

                if(dist_func == LISEM_DISTANCE)
                    cd = distance(p, cur_p + r);
                else if(dist_func == LISEM_SQR_DISTANCE)
                    cd = sqr_distance(p, cur_p + r);
                else if(dist_func == LISEM_MANHATTAN)
                    cd = manhattan(p, cur_p + r);
                else if(dist_func == LISEM_CHEBYCHEV)
                    cd = chebychev(p, cur_p + r);
                else if(dist_func == LISEM_QUADRATIC)
                    cd = quadratic(p, cur_p + r);
                else if(dist_func == LISEM_MINKOWSKI)
                    cd = minkowski(p, cur_p + r);

                d2 = std::min(d2, std::max(cd, d1));
                d1 = std::min(d1, cd);
            }
        }
    }

    if(type == LISEM_CLOSEST_1)
    {
        return d1;
    }
    else if(type == LISEM_CLOSEST_2)
    {
        return d2;
    }
    else if(type == LISEM_DIFFERENCE_21)
    {
        return d2 - d1;
    }
    else if(type == LISEM_CRACKLE)
    {
        return std::max(0.0,std::min(1.0,std::max(0.5, 16.0 * (d2-d1))));
    }
    else
    {
        return 0.0;
    }
}

//https://www.shadertoy.com/view/3tdGzn
static inline  cTMap * AS_Voronoise(cTMap * Other, float scalex, float scaley, int n_points)
{
    MaskedRaster<float> raster_data(Other->data.nr_rows(), Other->data.nr_cols(), Other->data.north(), Other->data.west(), Other->data.cell_size(),Other->data.cell_sizeY());
    cTMap *map = new cTMap(std::move(raster_data),Other->projection(),"");

    float t = RandomFloat(0.0,1000000.0);


    for(int r = 0; r < map->data.nr_rows();r++)
    {
        for(int c = 0; c < map->data.nr_cols();c++)
        {


            if(!pcr::isMV(Other->data[r][c]))
            {

                float y = (((float)(r) / ((float)(map->data.nr_rows()))))/scaley;
                float x = (((float)(c) / ((float)(map->data.nr_cols()))))/scalex;

                float v = voronoi3(x,y,t,n_points,LISEM_DISTANCE,LISEM_CLOSEST_1);

                map->data[r][c] = v;

            }
        }
    }

    return map;
}

#endif // RASTERRANDOM_H

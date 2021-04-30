#ifndef RASTERTERRAIN_H
#define RASTERTERRAIN_H

#include "geo/raster/map.h"
#include "raster/rasterrandom.h"
#include "interval.h"


inline static cTMap * AS_LandscapeErode(cTMap * DEM,float p_erode, float p_deposit, float p_gravity, float p_tc, float p_evapo, float p_inertia, float p_minslope,float radius)
{

    //at each pixel we make a particle
    //let every particle run down terrain (simple emperical energy balance based on p_gravity and p+inertia
    //let the particles erode and deposit based on emperical transport capacity
    //finally, erosion and deposition occur over gaussian kernel of size [radius]

    cTMap * PX = DEM->GetCopy0();
    cTMap * PY = DEM->GetCopy0();
    cTMap * PUX = DEM->GetCopy0();
    cTMap * PUY = DEM->GetCopy0();
    cTMap * PW = DEM->GetCopy0();
    cTMap * PS = DEM->GetCopy0();
    cTMap * SX = DEM->GetCopy0();
    cTMap * SY = DEM->GetCopy0();

    cTMap * DEMN = DEM->GetCopy();

    LSMInterval ix = LSMInterval(DEM->west(),DEM->west() + ((float)(DEM->nrCols()))*DEM->cellSizeX());
    LSMInterval iy = LSMInterval(DEM->north(),DEM->north() + ((float)(DEM->nrRows()))*DEM->cellSizeY());

    float dx = DEM->cellSizeX();
    float dy = DEM->cellSizeY();

    for(int r = 0; r < DEM->nrRows(); r++)
    {
        for(int c = 0; c < DEM->nrCols(); c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                double dem = DEM->Drc;

                        double demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                        double demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                        double demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                        double demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                        double Sx1 = -(demx1 - (dem))/(dx);
                        double Sx2 = -((dem)- (demx2))/(dx);
                        double Sy1 = -(demy1 - (dem))/(dy);
                        double Sy2 = -((dem)- (demy2))/(dy);

                  SX->data[r][c] = 0.5*(Sx1 + Sx2);
                  SY->data[r][c] = 0.5*(Sy1 + Sy2);

                  PX->data[r][c] = DEM->west() + DEM->cellSizeX() * ((float)(c)) + RandomFloat(0.0,1.0) * DEM->cellSizeX();
                  PY->data[r][c] = DEM->north() + DEM->cellSizeY() * ((float)(r))  + RandomFloat(0.0,1.0) * DEM->cellSizeY();
                  PW->data[r][c] = 1.0;
            }
        }
    }


    int radius_pixels_x = radius/std::fabs(DEM->cellSizeX());
    int radius_pixels_y = radius/std::fabs(DEM->cellSizeY());

    int n_iters = std::max(1, ((int)(1.0/p_evapo)));

    for(int i = 0; i < n_iters; i++)
    {
        std::cout << "iter " << i << n_iters << std::endl;

        for(int r = 0; r < DEM->nrRows(); r++)
        {
            for(int c = 0; c < DEM->nrCols(); c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                    double dem = DEMN->Drc;

                            double demx1 = !OUTORMV(DEMN,r,c-1)? DEMN->data[r][c-1] : dem;
                            double demx2 = !OUTORMV(DEMN,r,c+1)? DEMN->data[r][c+1] : dem;
                            double demy1 = !OUTORMV(DEMN,r-1,c)? DEMN->data[r-1][c] : dem;
                            double demy2 = !OUTORMV(DEMN,r+1,c)? DEMN->data[r+1][c] : dem;

                            double Sx1 = (demx1 - (dem))/(dx);
                            double Sx2 = ((dem)- (demx2))/(dx);
                            double Sy1 = (demy1 - (dem))/(dy);
                            double Sy2 = ((dem)- (demy2))/(dy);

                      SX->data[r][c] = 0.5*(Sx1 + Sx2);
                      SY->data[r][c] = 0.5*(Sy1 + Sy2);
                }
            }
        }



        for(int r = 0; r < DEM->nrRows(); r++)
        {
            for(int c = 0; c < DEM->nrCols(); c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                    if(!pcr::isMV(PX->data[r][c]) && !pcr::isMV(PY->data[r][c]))
                    {

                        float x = PX->data[r][c];
                        float y = PY->data[r][c];

                        if(!(ix.Contains(x) && iy.Contains(y)))
                        {

                        }else {

                            float slopeX = SX->SampleLinear(x,y);
                            float slopeY = SY->SampleLinear(x,y);

                            //local pit

                            int ct = std::max(0,std::min(DEM->nrCols()-1,(int)((x-DEM->west())/DEM->cellSizeX())));
                            int rt = std::max(0,std::min(DEM->nrRows()-1,(int)((y-DEM->north())/DEM->cellSizeY())));

                            if(pcr::isMV(DEM->data[rt][ct]))
                            {
                                pcr::setMV(PX->data[r][c]);
                                pcr::setMV(PY->data[r][c]);
                                continue;
                            }


                            PUX->data[r][c] += slopeX *p_gravity * DEM->cellSizeX();
                            PUY->data[r][c] += slopeY *p_gravity * DEM->cellSizeY();

                            PUX->data[r][c] = PUX->data[r][c] * p_inertia;
                            PUY->data[r][c] = PUY->data[r][c] * p_inertia;

                            PX->data[r][c] += PUX->data[r][c];
                            PY->data[r][c] += PUY->data[r][c];

                            float z = DEMN->SampleLinear(x,y);

                            float xn = PX->data[r][c];
                            float yn = PY->data[r][c];

                            if(!(ix.Contains(xn) && iy.Contains(yn)))
                            {
                                pcr::setMV(PX->data[r][c]);
                                pcr::setMV(PY->data[r][c]);
                                continue;
                            }

                            int cn = std::max(0,std::min(DEM->nrCols(),(int)((xn-DEM->west())/DEM->cellSizeX())));
                            int rn = std::max(0,std::min(DEM->nrRows(),(int)((yn-DEM->north())/DEM->cellSizeY())));

                            if(pcr::isMV(DEM->data[rn][cn]))
                            {
                                pcr::setMV(PX->data[r][c]);
                                pcr::setMV(PY->data[r][c]);
                                continue;
                            }

                            float zn = DEMN->SampleLinear(xn,yn);

                            float vel = std::sqrt(PUX->data[r][c]*PUX->data[r][c] +PUY->data[r][c]*PUY->data[r][c]);
                            float capacity =  vel * PW->data[r][c] * p_tc;

                            float erosion = std::max(0.0f,std::min((capacity - PS->data[r][c]) * p_erode,z-zn));
                            float deposition = std::min(PS->data[r][c],std::max(0.0f,PS->data[r][c] - capacity)* p_deposit);

                            PS->data[r][c] += erosion - deposition;

                            int rmin = std::max(0,rn - radius_pixels_y);
                            int rmax = std::min(DEM->nrRows()-1,rn + radius_pixels_y);
                            int cmin = std::max(0,cn - radius_pixels_x);
                            int cmax = std::min(DEM->nrCols()-1,cn + radius_pixels_x);

                            float weight_total = 0.0;
                            for(int r2 = rmin; r2 < rmax; r2++)
                            {
                                for(int c2 = cmin; c2 < cmax; c2++)
                                {
                                    if(!(r2 >-1 && r2 < DEM->nrRows() && c2 > -1 && c2 < DEM->nrCols()))
                                    {
                                        continue;
                                    }
                                    if(pcr::isMV(DEM->data[r2][c2]))
                                    {
                                        continue;
                                    }

                                    float distx = (c2-cn)/radius_pixels_x;
                                    float disty = (r2-rn)/radius_pixels_y;
                                    float weight = 1.0 - std::sqrt(distx*distx + disty*disty);

                                    if(weight > 0.0)
                                    {
                                        weight_total += weight;
                                    }
                                }
                            }

                            if(weight_total > 0.0)
                            {
                                for(int r2 = rmin; r2 < rmax; r2++)
                                {
                                    for(int c2 = cmin; c2 < cmax; c2++)
                                    {
                                        if(!(r2 >-1 && r2 < DEM->nrRows() && c2 > -1 && c2 < DEM->nrCols()))
                                        {
                                            continue;
                                        }
                                        if(pcr::isMV(DEM->data[r2][c2]))
                                        {
                                            continue;
                                        }

                                        float distx = ((float)(c2-cn))/radius_pixels_x;
                                        float disty = ((float)(r2-rn))/radius_pixels_y;
                                        float weight = 1.0 - std::sqrt(distx*distx + disty*disty);

                                        if(weight > 0.0)
                                        {
                                            //std::cout << "doing erosion " << erosion << std::endl;
                                            DEMN->data[r2][c2] += (weight/weight_total)*1.0;//deposition;
                                            DEMN->data[r2][c2] -= (weight/weight_total)*0.0;//erosion;
                                        }
                                    }
                                }
                            }

                            PW->data[r][c] = PW->data[r][c] * (1.0- p_evapo);
                        }
                    }
                }
            }
        }
    }

    //update position
    //get cluster size for calculations
    //get erosion/deposition
    //do evaporation
    //update velocity

    delete PX;
    delete PY;
    delete PUX;
    delete PUY;
    delete PW;
    delete PS;
    delete SX;
    delete SY;

    return DEMN;
}

#endif // RASTERTERRAIN_H

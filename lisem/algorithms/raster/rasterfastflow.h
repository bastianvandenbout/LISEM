#ifndef RASTERFASTFLOW_H
#define RASTERFASTFLOW_H

#include "error.h"
#include "QString"
#include "geo/raster/map.h"
#include "rasterconstructors.h"
#include "linear/lsm_vector3.h"
#include "raster/rastercommon.h"
#include "rasterderivative.h"
#include <vector>

inline std::vector<cTMap *> AS_AccuFluxDiffusive(cTMap * DEMIn, cTMap * source, cTMap * FlowSource, cTMap * HInitial, cTMap * HForced, int iter_max, float courant, float hscale = 1.0, int iter_accu = -1, bool precondition = false, float pre_coef_slopedem = 1.0, bool do_forced = false)
{

    if(iter_accu < 0)
    {
        iter_accu = iter_max;
    }

    if(iter_max < 1)
    {
        LISEMS_ERROR("Can not run steady state flow without iterations");
        throw 1;
    }
    if(courant < 1e-12 || courant  > 1.0)
    {
        LISEM_ERROR("Courant value must be between 1e-12 and 1");
        throw 1;
    }
    if(hscale < 1e-10)
    {
        LISEM_ERROR("Height scale value must be larger then 1e-12");
        throw 1;
    }

    cTMap * DEM= DEMIn->GetCopy();

    MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *H = new cTMap(std::move(raster_data),DEM->projection(),"");

    //create temporary maps



    MaskedRaster<float> raster_data2(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *HN = new cTMap(std::move(raster_data2),DEM->projection(),"");

    MaskedRaster<float> raster_data3(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX1 = new cTMap(std::move(raster_data3),DEM->projection(),"");

    MaskedRaster<float> raster_data4(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX2 = new cTMap(std::move(raster_data4),DEM->projection(),"");

    MaskedRaster<float> raster_data5(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY1 = new cTMap(std::move(raster_data5),DEM->projection(),"");

    MaskedRaster<float> raster_data6(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY2 = new cTMap(std::move(raster_data6),DEM->projection(),"");

    MaskedRaster<float> raster_data7(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SX = new cTMap(std::move(raster_data7),DEM->projection(),"");

    MaskedRaster<float> raster_data8(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SY = new cTMap(std::move(raster_data8),DEM->projection(),"");

    MaskedRaster<float> raster_data9(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *S = new cTMap(std::move(raster_data9),DEM->projection(),"");

    cTMap * QAX = QX1->GetCopy();
    cTMap * QAY = QX1->GetCopy();
    cTMap * QC = QX1->GetCopy0();
    cTMap * QTX1 = QX1->GetCopy0();
    cTMap * QTX2 = QX1->GetCopy0();
    cTMap * QTY1 = QX1->GetCopy0();
    cTMap * QTY2 = QX1->GetCopy0();
    cTMap * HA = QX1->GetCopy();
    cTMap * QT = QX1->GetCopy();
    cTMap * QM = QX1->GetCopy0();
    cTMap * QR = QX1->GetCopy0();
    cTMap * HS = QX1->GetCopy0();

    bool stop = false;
    bool stop2 = false;

    double ifac = 0.0;

    //preconditioning

    float dt_req_min = 0.1;

    float totalpix= DEM->data.nr_rows() * DEM->data.nr_cols();
    float maxv = -1e31;
    float minv = 1e31;

    {
        bool found = false;

        #pragma omp parallel for collapse(2) shared(found) reduction(max:maxv)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(pcr::isMV(DEM->data[r][c]))
                {
                }else
                {
                    maxv = std::max(maxv,DEM->data[r][c]);
                    found = true;

                }
            }
        }

        if(!found)
        {
            maxv = 1.0;
        }
    }

    {
        bool found = false;

        #pragma omp parallel for collapse(2) shared(found) reduction(min:minv)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(pcr::isMV(DEM->data[r][c]))
                {
                }else
                {
                    minv = std::min(minv,DEM->data[r][c]);
                    found = true;

                }
            }
        }

        if(!found)
        {
            minv = 0.0;
        }
    }
    if(maxv - minv < 1e-6 && precondition)
    {
        LISEMS_ERROR("Can not run the preconditioning without elevation differences, turning it off");
        precondition = false;
    }

    std::cout << "precondition " << precondition << " " << maxv << " " << minv << std::endl;

    double hchangetotal = 0.0;
    double hweighttotal = 0.0;

    int i = 0;
    int i2 = 0;
    #pragma omp parallel private(stop,stop2) shared(i,ifac)
    {


        float N = 0.05;
        double _dx = std::fabs(DEM->cellSizeX());
        float dt = dt_req_min; //is actually dt/dx

        #pragma omp for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                    float dem = DEM->Drc;
                    float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                    float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                    float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                    float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                    float Sx1 = (demx1 - (dem))/(_dx);
                    float Sx2 = ((dem)- (demx2))/(_dx);
                    float Sy1 = (demy1 - (dem))/(_dx);
                    float Sy2 = ((dem)- (demy2))/(_dx);

                    float sx = 0.0;
                    float sy = 0.0;
                    if(demx1 < demx2)
                    {
                        sx = Sx1;
                    }else
                    {
                        sx = Sx2;
                    }

                    if(demy1 < demy2)
                    {
                        sy = Sy1;
                    }else
                    {
                        sy = Sy2;
                    }

                    S->data[r][c] = (std::fabs(sx) + std::fabs(sy) + 1e-8f);

                    float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                    DEM->data[r][c] = DEM->data[r][c] + pit;
                }

            }
        }



        #pragma omp barrier



        //if(precondition)
        {
            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        if(precondition)
                        {

                            float dem = DEM->Drc;
                            //already checked the maxv and minv values earlier
                            float inv_normz = 1.0 - (dem - minv)/(maxv-minv);
                            float inv_norms = exp(-S->data[r][c]);

                            H->data[r][c] = pre_coef_slopedem * inv_normz * inv_norms;
                        }else
                        {
                            H->data[r][c] = HInitial->data[r][c];
                        }

                        if(do_forced)
                        {
                            if(!(HForced->data[r][c] < 0.0f))
                            {
                                H->data[r][c] =HForced->data[r][c];
                            }
                        }
                    }
                }
            }

        }


        #pragma omp critical
        {
            i = 0;

        }


        while( true)
        {
            #pragma omp single
            {
                hchangetotal = 0.0;
                hweighttotal = 0.0;

                stop = false;
                i = i+1;
                std::cout << "i = " << i << std::endl;
            }

            #pragma omp barrier

            bool do_stop = false;

            #pragma omp critical
            {
                i2 = i;
                if(i2>= iter_max)
                {
                    std::cout << "should break" <<std::endl;
                    stop = true;
                    do_stop = true;
                }
                dt_req_min = 1e6;

            }

            #pragma omp barrier

            if(do_stop)
            {
                std::cout << "do break " << std::endl;
                break;
            }


            float progress = ((float)(i))/std::max(1.0f,((float)(iter_max)));


            #pragma omp for collapse(2) reduction(min:dt_req_min)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        float dem = DEM->Drc;
                        float demh =DEM->data[r][c]+hscale *H->data[r][c];
                        bool outlet = false;
                        if(OUTORMV(DEM,r,c-1) || OUTORMV(DEM,r,c+1) || OUTORMV(DEM,r-1,c) || OUTORMV(DEM,r+1,c))
                        {
                            outlet = 1.0;
                        }

                        float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                        float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                        float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                        float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                        float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                        dem = dem + pit;
                        demh = demh + pit;

                        float demhx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1]+hscale*H->data[r][c-1] : dem;
                        float demhx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1]+hscale*H->data[r][c+1] : dem;
                        float demhy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c]+hscale*H->data[r-1][c] : dem;
                        float demhy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c]+hscale*H->data[r+1][c] : dem;

                        float demhmin = std::min(std::min(std::min(demhx1,demhx2),demhy1),demhy2);
                        float h = std::max(0.0f,H->data[r][c]);

                        float Shx1 = (demhx1 - (demh))/(_dx);
                        float Shx2 = ((demh)- (demhx2))/(_dx);
                        float Shy1 = (demhy1 - (demh))/(_dx);
                        float Shy2 = ((demh)- (demhy2))/(_dx);

                        float Sx1 = (demx1 - (dem))/(_dx);
                        float Sx2 = ((dem)- (demx2))/(_dx);
                        float Sy1 = (demy1 - (dem))/(_dx);
                        float Sy2 = ((dem)- (demy2))/(_dx);

                        if(demhx1 < demhx2)
                        {
                            SX->Drc = Shx1;
                        }else
                        {
                            SX->Drc = Shx2;
                        }

                        if(demhy1 < demhy2)
                        {
                            SY->Drc = Shy1;
                        }else
                        {
                            SY->Drc = Shy2;
                        }

                        float flowwidth = _dx;


                        S->Drc = (std::fabs(SX->Drc) + std::fabs(SY->Drc) + 1e-8);

                        float HL = h;//std::min(h,std::max(0.0,hscale*H->data[r][c] - std::max(0.0,(demhmin -(DEM->data[r][c])))));
                        //float Hm = std::min(hscale*h,std::max(0.0f,hscale*H->data[r][c] - std::max(0.0f,(demhmin -(DEM->data[r][c])))))/hscale;
                        HA->data[r][c] = HL - h;

                        float q = _dx * flowwidth * HL * std::sqrt(std::fabs(S->Drc + 0.001))*(1.0f/N) *std::pow((HL),2.0f/3.0f)/(_dx*_dx);
                        //float qm = 1.0 *flowwidth * Hm * std::sqrt(std::fabs(S->Drc + 0.001))*(1.0f/N) *std::pow((Hm),2.0f/3.0f);

                        float dt_req = courant * h/std::max(1e-6f,q);

                        q = q ;

                        QR->data[r][c] = q/std::max(0.0001f,h);
                        QC->data[r][c] = q;
                        q = courant *h;//std::min(progress * q + (1.0f-progress) * (courant * h),courant * h);
                        dt_req_min = std::min(std::max(1.0f/*progress * 1e-5f + (1.0f-progress) * 0.1f*/,dt_req),dt_req_min);

                        //float V = h > 0? std::sqrt(HL) * std::sqrt(std::fabs(S->data[r][c] + 0.001f))*(1.0f/N) *std::pow((HL*_dx)/(2.0f * HL + _dx),2.0f/3.0f) : 0.0f;

                        float qx = (demhx1 < demhx2? -1.0f:1.0f) * (std::fabs(SX->Drc)/S->Drc) * q;
                        float qy = (demhy1 < demhy2? -1.0f:1.0f) * (std::fabs(SY->Drc)/S->Drc) * q;

                        QT->data[r][c] += std::fabs(qx) + std::fabs(qy);
                        QX1->Drc = qx < 0.0f? qx :0.0f;
                        QX2->Drc = qx > 0.0f? qx :0.0f;
                        QY1->Drc = qy < 0.0f? qy :0.0f;
                        QY2->Drc = qy > 0.0f? qy :0.0f;

                        QTX1->Drc += QX1->Drc;
                        QTX2->Drc += QX2->Drc;
                        QTY1->Drc += QY1->Drc;
                        QTY2->Drc += QY2->Drc;
                    }
                }
            }


            #pragma omp critical
            {

            }

            #pragma omp barrier

            float in_total = 0.0;
            float out_total = 0.0;

            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        float dem = DEM->Drc;

                                float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                                float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                                float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                                float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                                float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                                dem = dem + pit;

                                float demhx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1]+hscale*H->data[r][c-1] : dem;
                                float demhx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1]+hscale*H->data[r][c+1] : dem;
                                float demhy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c]+hscale*H->data[r-1][c] : dem;
                                float demhy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c]+hscale*H->data[r+1][c] : dem;

                                float demhx11 = !OUTORMV(DEM,r-1,c-1)? DEM->data[r-1][c-1]+hscale*H->data[r-1][c-1] : dem;
                                float demhx22 = !OUTORMV(DEM,r+1,c+1)? DEM->data[r+1][c+1]+hscale*H->data[r+1][c+1] : dem;
                                float demhy11 = !OUTORMV(DEM,r-1,c-1)? DEM->data[r-1][c+1]+hscale*H->data[r-1][c+1] : dem;
                                float demhy22 = !OUTORMV(DEM,r+1,c+1)? DEM->data[r+1][c-1]+hscale*H->data[r+1][c-1] : dem;

                                float demhmin = std::min(std::min(std::min(demhx1,demhx2),demhy1),demhy2);
                                demhmin =std::min(demhmin, std::min(std::min(std::min(demhx11,demhx22),demhy11),demhy22));

                                float qx1 = std::max(0.0f,((!OUTORMV(DEM,r,c-1))? (std::fabs(QX2->data[r][c-1])) : 0.0f));
                                float qx2 = std::max(0.0f,((!OUTORMV(DEM,r,c+1))? (std::fabs(QX1->data[r][c+1])) : 0.0f));
                                float qy1 = std::max(0.0f,((!OUTORMV(DEM,r-1,c))? (std::fabs(QY2->data[r-1][c])) : 0.0f));
                                float qy2 = std::max(0.0f,((!OUTORMV(DEM,r+1,c))? (std::fabs(QY1->data[r+1][c])) : 0.0f));

                                float qoutx1 = std::max(0.0f,(std::fabs(QX2->data[r][c])));
                                float qoutx2 = std::max(0.0f,(std::fabs(QX1->data[r][c])));
                                float qouty1 = std::max(0.0f,(std::fabs(QY2->data[r][c])));
                                float qouty2 = std::max(0.0f,(std::fabs(QY1->data[r][c])));

                                //total net incoming discharge
                                float Q = std::max(0.0f, (qoutx1 + qoutx2 + qouty1 + qouty2));// - qoutx1 - qoutx2 - qouty1 - qouty2

                                float QIN = std::max(0.0f,(qx1 + qx2 + qy1 + qy2) + FlowSource->data[r][c]);// - qoutx1 - qoutx2 - qouty1 - qouty2

                                in_total += std::max(0.0f, (qoutx1 + qoutx2 + qouty1 + qouty2));
                                out_total += std::max(0.0f,(qx1 + qx2 + qy1 + qy2));

                                //solve for height, so that new discharge is incoming discharge
                                //float sol = std::max(0.0f,(demhmin -DEM->data[r][c] ))+ std::pow( _dx * QIN*  N /(sqrt(S->Drc + 0.001f)),6.0f/5.0f);

                                float sol = QIN / courant;
                                HS->data[r][c] = sol;

                                float hnew = std::max(std::max(0.0f,demhmin- dem)/hscale,sol);
                                HN->data[r][c] = H->Drc  + QIN* dt - Q* dt ;//(H->Drc > 1 || hnew > 1)? std::min(H->Drc * 2.0,std::max(H->Drc*1.0,hnew)) : hnew;
                     }
                 }
               }

            #pragma omp barrier


            #pragma omp for collapse(2) reduction(+:hchangetotal)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        hweighttotal += HN->data[r][c];
                        hchangetotal += HN->data[r][c] * std::fabs(HN->data[r][c] - H->data[r][c])/std::max(0.00000001f,H->data[r][c]);
                        H->data[r][c] = HN->data[r][c];
                        if(do_forced)
                        {
                            if(!(HForced->data[r][c] < 0.0f))
                            {
                                H->data[r][c] =HForced->data[r][c];
                            }
                        }
                    }
                }
            }


            #pragma omp single
            {
                std::cout << "dt is " << dt_req_min << "      with delta = " << (hchangetotal/std::max(0.000001,hweighttotal))/totalpix <<  std::endl;

            }

            #pragma omp barrier

        }

        #pragma omp barrier

        #pragma omp for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {

                    HA->data[r][c] = source->data[r][c];

                    //if we are at the edge of a forced section of the domain, we must compensate the initial flow accumulation source to include the flux required to maintain the forced steady-state height at that position
                    if(do_forced)
                    {
                        float forced = HForced->data[r][c];
                        float forcedx1 = !OUTORMV(DEM,r,c-1)? HForced->data[r][c-1] : forced;
                        float forcedx2 = !OUTORMV(DEM,r,c+1)? HForced->data[r][c+1] : forced;
                        float forcedy1 = !OUTORMV(DEM,r-1,c)? HForced->data[r-1][c] : forced;
                        float forcedy2 = !OUTORMV(DEM,r+1,c)? HForced->data[r+1][c] : forced;

                        if(!(forced  < 0.0))
                        {
                            if((forcedx1 < 0.0)||(forcedx2 < 0.0)||(forcedy1 < 0.0)||(forcedy2 < 0.0))
                            {

                                float flowwidth = _dx;
                                float Nhere = 0.1;
                                float q = std::pow(HForced->data[r][c],5.0/3.0)* std::sqrt(std::fabs(S->Drc + 0.001))  /(0.1 * _dx*_dx);

                                HA->data[r][c] = q;
                            }
                        }

                    }


                    S->data[r][c] = 0.0;
                    QTX1->data[r][c] = std::fabs(QTX1->data[r][c]);
                    QTX2->data[r][c] = std::fabs(QTX2->data[r][c]);
                    QTY1->data[r][c] = std::fabs(QTY1->data[r][c]);
                    QTY2->data[r][c] = std::fabs(QTY2->data[r][c]);

                    QT->data[r][c] = 0.0;

                }
            }
        }



        #pragma omp critical
        {
             i = 0;
        }
        while( true)
        {
            #pragma omp single
            {
                i = i+1;
                ifac = ifac + (float)i;
                stop2 = false;
                std::cout << "i = " << i << std::endl;
            }

            #pragma omp barrier

            bool do_stop = false;
            #pragma omp critical
            {
                i2 = i;

                if(i2>= iter_accu)
                {
                    std::cout << "should break " << std::endl;
                    stop2 = true;
                    do_stop = true;
                }
            }

            #pragma omp barrier

            if(do_stop)
            {
                std::cout << "do break " << std::endl;
                break;
            }

            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    //Get Q Total

                    if(!pcr::isMV(DEM->data[r][c]))
                    {


                            float qxa = (QTX2->data[r][c]-QTX1->data[r][c]);
                            float qya = (QTY2->data[r][c]-QTY1->data[r][c]);

                            float QT2 = std::fabs(qxa) + std::fabs(qya);

                            if(QT2 > 0.0)
                            {
                                //Distribute outflow
                                QAX->data[r][c] =HA->data[r][c] * (qxa/QT2);
                                QAY->data[r][c] =HA->data[r][c] * (qya/QT2);
                            }else {
                                QAX->data[r][c] = 0.0;
                                QAY->data[r][c] = 0.0;
                            }


                    }
                }
            }


            #pragma omp barrier

            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        float QTt = 0.0;
                        if(!OUTORMV(DEM,r,c+1))
                        {
                            if(QAX->data[r][c+1] < 0)
                            {
                                QTt += std::fabs(QAX->data[r][c+1]);
                            }
                        }
                        if(!OUTORMV(DEM,r,c-1))
                        {
                            if(QAX->data[r][c-1] > 0)
                            {
                                QTt += std::fabs(QAX->data[r][c-1]);
                            }
                        }
                        if(!OUTORMV(DEM,r+1,c))
                        {
                            if(QAY->data[r+1][c] < 0)
                            {
                                QTt += std::fabs(QAY->data[r+1][c]);
                            }
                        }
                        if(!OUTORMV(DEM,r-1,c))
                        {
                            if(QAY->data[r-1][c] > 0)
                            {
                                QTt += std::fabs(QAY->data[r-1][c]);
                            }
                        }

                        S->data[r][c] += QTt;
                        QT->data[r][c] += QTt * (float(i));
                        QM->data[r][c] = std::max(QTt,QM->data[r][c]);
                        HA->data[r][c] = QTt;
                    }
                }
            }
        }

        //final normalized velocity directional field

        #pragma omp for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                //Get Q Total

                if(!pcr::isMV(DEM->data[r][c]))
                {

                        float qxa = (QTX2->data[r][c]-QTX1->data[r][c]);
                        float qya = (QTY2->data[r][c]-QTY1->data[r][c]);

                        float QT2 = std::fabs(qxa) + std::fabs(qya);

                        if(QT2 > 0.0)
                        {
                            //Distribute outflow
                            QAX->data[r][c] =(qxa/QT2);
                            QAY->data[r][c] =(qya/QT2);
                        }else {
                            QAX->data[r][c] = 0.0;
                            QAY->data[r][c] = 0.0;
                        }


                }
            }
        }

    }

    //delete temporary map

    delete HN;
    delete QX1;
    delete QX2;
    delete QY1;
    delete QY2;

    delete SX;
    delete SY;
    delete QTX1;
    delete QTX2;
    delete QTY1;
    delete QTY2;

    return {H,S,QT,QM,HA,QAX,QAY,QR, QC,HS};
}



inline std::vector<cTMap *> AS_AccuFluxDiffusive2(cTMap * DEMIn, cTMap * source, cTMap * FlowSource, cTMap * HInitial, cTMap * HForced, int iter_max, float courant, float hscale = 1.0, int iter_accu = -1, bool precondition = false, float pre_coef_slopedem = 1.0, bool do_forced = false)
{

    if(iter_accu < 0)
    {
        iter_accu = iter_max;
    }

    if(iter_max < 1)
    {
        LISEMS_ERROR("Can not run steady state flow without iterations");
        throw 1;
    }
    if(courant < 1e-12 || courant  > 1.0)
    {
        LISEM_ERROR("Courant value must be between 1e-12 and 1");
        throw 1;
    }
    if(hscale < 1e-10)
    {
        LISEM_ERROR("Height scale value must be larger then 1e-12");
        throw 1;
    }

    cTMap * DEM= DEMIn->GetCopy();

    MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *H = new cTMap(std::move(raster_data),DEM->projection(),"");

    //create temporary maps



    MaskedRaster<float> raster_data2(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *HN = new cTMap(std::move(raster_data2),DEM->projection(),"");

    MaskedRaster<float> raster_data3(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX1 = new cTMap(std::move(raster_data3),DEM->projection(),"");

    MaskedRaster<float> raster_data4(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX2 = new cTMap(std::move(raster_data4),DEM->projection(),"");

    MaskedRaster<float> raster_data5(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY1 = new cTMap(std::move(raster_data5),DEM->projection(),"");

    MaskedRaster<float> raster_data6(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY2 = new cTMap(std::move(raster_data6),DEM->projection(),"");

    MaskedRaster<float> raster_data7(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SX = new cTMap(std::move(raster_data7),DEM->projection(),"");

    MaskedRaster<float> raster_data8(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SY = new cTMap(std::move(raster_data8),DEM->projection(),"");

    MaskedRaster<float> raster_data9(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *S = new cTMap(std::move(raster_data9),DEM->projection(),"");

    cTMap * QAX = QX1->GetCopy();
    cTMap * QAY = QX1->GetCopy();
    cTMap * SXO = QX1->GetCopy0();
    cTMap * SYO = QX1->GetCopy0();

    cTMap * QAX1 = QX1->GetCopy0();
    cTMap * QAY1 = QX1->GetCopy0();
    cTMap * QAX2 = QX1->GetCopy0();
    cTMap * QAY2 = QX1->GetCopy0();

    cTMap * QC = QX1->GetCopy0();
    cTMap * QTX1 = QX1->GetCopy0();
    cTMap * QTX2 = QX1->GetCopy0();
    cTMap * QTY1 = QX1->GetCopy0();
    cTMap * QTY2 = QX1->GetCopy0();
    cTMap * HA = QX1->GetCopy();
    cTMap * QT = QX1->GetCopy();
    cTMap * QM = QX1->GetCopy0();
    cTMap * QR = QX1->GetCopy0();
    cTMap * HS = QX1->GetCopy0();

    bool stop = false;
    bool stop2 = false;

    double ifac = 0.0;

    //preconditioning

    float dt_req_min = 0.1;

    float totalpix= DEM->data.nr_rows() * DEM->data.nr_cols();
    float maxv = -1e31;
    float minv = 1e31;

    {
        bool found = false;

        #pragma omp parallel for collapse(2) shared(found) reduction(max:maxv)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(pcr::isMV(DEM->data[r][c]))
                {
                }else
                {
                    maxv = std::max(maxv,DEM->data[r][c]);
                    found = true;

                }
            }
        }

        if(!found)
        {
            maxv = 1.0;
        }
    }

    {
        bool found = false;

        #pragma omp parallel for collapse(2) shared(found) reduction(min:minv)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(pcr::isMV(DEM->data[r][c]))
                {
                }else
                {
                    minv = std::min(minv,DEM->data[r][c]);
                    found = true;

                }
            }
        }

        if(!found)
        {
            minv = 0.0;
        }
    }
    if(maxv - minv < 1e-6 && precondition)
    {
        LISEMS_ERROR("Can not run the preconditioning without elevation differences, turning it off");
        precondition = false;
    }

    std::cout << "precondition " << precondition << " " << maxv << " " << minv << std::endl;

    double hchangetotal = 0.0;
    double schangetotal = 0.0;
    double hweighttotal = 0.0;

    int i = 0;
    int i2 = 0;
    #pragma omp parallel private(stop,stop2) shared(i,ifac)
    {


        float N = 0.05;
        double _dx = std::fabs(DEM->cellSizeX());
        float dt = dt_req_min; //is actually dt/dx

        #pragma omp for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                    float dem = DEM->Drc;
                    float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                    float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                    float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                    float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                    float Sx1 = (demx1 - (dem))/(_dx);
                    float Sx2 = ((dem)- (demx2))/(_dx);
                    float Sy1 = (demy1 - (dem))/(_dx);
                    float Sy2 = ((dem)- (demy2))/(_dx);

                    float sx = 0.0;
                    float sy = 0.0;
                    if(demx1 < demx2)
                    {
                        sx = Sx1;
                    }else
                    {
                        sx = Sx2;
                    }

                    if(demy1 < demy2)
                    {
                        sy = Sy1;
                    }else
                    {
                        sy = Sy2;
                    }

                    S->data[r][c] = (std::fabs(sx) + std::fabs(sy) + 1e-8f);

                    float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                    DEM->data[r][c] = DEM->data[r][c] + pit;
                }

            }
        }



        #pragma omp barrier



        //if(precondition)
        {
            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        if(precondition)
                        {

                            float dem = DEM->Drc;
                            //already checked the maxv and minv values earlier
                            float inv_normz = 1.0 - (dem - minv)/(maxv-minv);
                            float inv_norms = exp(-S->data[r][c]);

                            H->data[r][c] = pre_coef_slopedem * inv_normz * inv_norms;
                        }else
                        {
                            H->data[r][c] = HInitial->data[r][c];
                        }

                        if(do_forced)
                        {
                            if(!(HForced->data[r][c] < 0.0f))
                            {
                                H->data[r][c] =HForced->data[r][c];
                            }
                        }
                    }
                }
            }

        }


        #pragma omp critical
        {
            i = 0;

        }


        while( true)
        {
            #pragma omp single
            {

                schangetotal = 0.0;
                hchangetotal = 0.0;
                hweighttotal = 0.0;

                stop = false;
                i = i+1;
                std::cout << "i = " << i << std::endl;
            }

            #pragma omp barrier

            bool do_stop = false;

            #pragma omp critical
            {
                i2 = i;
                if(i2>= iter_max)
                {
                    std::cout << "should break" <<std::endl;
                    stop = true;
                    do_stop = true;
                }
                dt_req_min = 1e6;

            }

            #pragma omp barrier

            if(do_stop)
            {
                std::cout << "do break " << std::endl;
                break;
            }


            float progress = ((float)(i))/std::max(1.0f,((float)(iter_max)));


            #pragma omp for collapse(2) reduction(min:dt_req_min)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        float dem = DEM->Drc;
                        float demh =DEM->data[r][c]+hscale *H->data[r][c];
                        bool outlet = false;
                        if(OUTORMV(DEM,r,c-1) || OUTORMV(DEM,r,c+1) || OUTORMV(DEM,r-1,c) || OUTORMV(DEM,r+1,c))
                        {
                            outlet = 1.0;
                        }

                        float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                        float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                        float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                        float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                        float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                        dem = dem + pit;
                        demh = demh + pit;

                        float hx1 = !OUTORMV(DEM,r,c-1)? hscale*H->data[r][c-1] : 0.0f;
                        float hx2 = !OUTORMV(DEM,r,c+1)? hscale*H->data[r][c+1] : 0.0f;
                        float hy1 = !OUTORMV(DEM,r-1,c)? hscale*H->data[r-1][c] : 0.0f;
                        float hy2 = !OUTORMV(DEM,r+1,c)? hscale*H->data[r+1][c] : 0.0f;


                        float demhx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1]+hscale*H->data[r][c-1] : dem;
                        float demhx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1]+hscale*H->data[r][c+1] : dem;
                        float demhy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c]+hscale*H->data[r-1][c] : dem;
                        float demhy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c]+hscale*H->data[r+1][c] : dem;

                        float demhmin = std::min(std::min(std::min(demhx1,demhx2),demhy1),demhy2);
                        float h = std::max(0.0f,H->data[r][c]);



                        float Shx1 = (demhx1 - (demh))/(_dx);
                        float Shx2 = ((demh)- (demhx2))/(_dx);
                        float Shy1 = (demhy1 - (demh))/(_dx);
                        float Shy2 = ((demh)- (demhy2))/(_dx);

                        float Sx1 = (demx1 - (dem))/(_dx);
                        float Sx2 = ((dem)- (demx2))/(_dx);
                        float Sy1 = (demy1 - (dem))/(_dx);
                        float Sy2 = ((dem)- (demy2))/(_dx);

                        if(demhx1 < demhx2)
                        {
                            SX->Drc = Shx1;
                        }else
                        {
                            SX->Drc = Shx2;
                        }

                        if(demhy1 < demhy2)
                        {
                            SY->Drc = Shy1;
                        }else
                        {
                            SY->Drc = Shy2;
                        }

                        float flowwidth = _dx;

                        S->Drc = (std::fabs(SX->Drc) + std::fabs(SY->Drc) + 1e-8);

                        float HL = h;//std::min(h,std::max(0.0,hscale*H->data[r][c] - std::max(0.0,(demhmin -(DEM->data[r][c])))));
                        //float Hm = std::min(hscale*h,std::max(0.0f,hscale*H->data[r][c] - std::max(0.0f,(demhmin -(DEM->data[r][c])))))/hscale;
                        HA->data[r][c] = HL - h;

                        float q = _dx * flowwidth * HL * std::sqrt(std::fabs(S->Drc + 0.001))*(1.0f/N) *std::pow((HL),2.0f/3.0f)/(_dx*_dx);
                        //float qm = 1.0 *flowwidth * Hm * std::sqrt(std::fabs(S->Drc + 0.001))*(1.0f/N) *std::pow((Hm),2.0f/3.0f);

                        float dt_req = courant * h/std::max(1e-6f,q);

                        q = q ;

                        QR->data[r][c] = q/std::max(0.0001f,h);
                        QC->data[r][c] = q;
                        q = courant *h;//std::min(progress * q + (1.0f-progress) * (courant * h),courant * h);

                        float qx1 = hx1 * courant;
                        float qx2 = hx2 * courant;
                        float qy1 = hy1 * courant;
                        float qy2 = hy2 * courant;

                        dt_req_min = std::min(std::max(1.0f/*progress * 1e-5f + (1.0f-progress) * 0.1f*/,dt_req),dt_req_min);

                        //float V = h > 0? std::sqrt(HL) * std::sqrt(std::fabs(S->data[r][c] + 0.001f))*(1.0f/N) *std::pow((HL*_dx)/(2.0f * HL + _dx),2.0f/3.0f) : 0.0f;

                        float ws_total =std::max(1e-6f,((demhx1 < demh)? std::fabs(Shx1): 0.0f) +
                                                 ((demhy1 < demh)? std::fabs(Shy1): 0.0f) +
                                                 ((demhx2 < demh)? std::fabs(Shx2): 0.0f) +
                                                 ((demhy2 < demh)? std::fabs(Shy2): 0.0f));
                        float ws_ototal =std::max(1e-6f,-(std::min(0.0f,Shx1) +std::min(0.0f,Shy1) + std::min(0.0f,-Shx2) + std::min(0.0f,-Shy2)));

                        qx1 = (demhx1 < demh)? std::fabs((Shx1)/ws_total) * q : 0.0;
                        if(demhx1 == demh)
                        {
                            qx1 = 0.0;
                        }
                        qy1 = (demhy1 < demh)? std::fabs((Shy1)/ws_total) * q : 0.0;
                        if(demhy1 == demh)
                        {
                            qy1 = 0.0;
                        }
                        qx2 = (demh < demhx2)? 0.0 : std::fabs((Shx2)/ws_total) * q;
                        if(demhx2 == demh)
                        {
                            qx2 = 0.0;
                        }
                        qy2 = (demh < demhy2)? 0.0 : std::fabs((Shy2)/ws_total) * q;
                        if(demhy2 == demh)
                        {
                            qy2 = 0.0;
                        }
                        qx1 = std::isnormal(qx1)?qx1:0.0;
                        qx2 = std::isnormal(qx2)?qx2:0.0;
                        qy1 = std::isnormal(qy1)?qy1:0.0;
                        qy2 = std::isnormal(qy2)?qy2:0.0;

                        QT->data[r][c] += std::fabs(q);


                        QX1->Drc = qx1;// < 0.0f? qx :0.0f;
                        QX2->Drc = qx2;// > 0.0f? qx :0.0f;
                        QY1->Drc = qy1;// < 0.0f? qy :0.0f;
                        QY2->Drc = qy2;// > 0.0f? qy :0.0f;


                    }
                }
            }


            #pragma omp critical
            {

            }

            #pragma omp barrier

            float in_total = 0.0;
            float out_total = 0.0;

            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        float dem = DEM->Drc;

                                float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                                float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                                float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                                float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                                float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                                dem = dem + pit;

                                float demh = dem + hscale*H->data[r][c];
                                float demhx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1]+hscale*H->data[r][c-1] : dem;
                                float demhx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1]+hscale*H->data[r][c+1] : dem;
                                float demhy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c]+hscale*H->data[r-1][c] : dem;
                                float demhy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c]+hscale*H->data[r+1][c] : dem;

                                float demhx11 = !OUTORMV(DEM,r-1,c-1)? DEM->data[r-1][c-1]+hscale*H->data[r-1][c-1] : dem;
                                float demhx22 = !OUTORMV(DEM,r+1,c+1)? DEM->data[r+1][c+1]+hscale*H->data[r+1][c+1] : dem;
                                float demhy11 = !OUTORMV(DEM,r-1,c-1)? DEM->data[r-1][c+1]+hscale*H->data[r-1][c+1] : dem;
                                float demhy22 = !OUTORMV(DEM,r+1,c+1)? DEM->data[r+1][c-1]+hscale*H->data[r+1][c-1] : dem;

                                float demhmin = std::min(std::min(std::min(demhx1,demhx2),demhy1),demhy2);
                                demhmin =std::min(demhmin, std::min(std::min(std::min(demhx11,demhx22),demhy11),demhy22));

                                float qx1 = std::max(0.0f,((!OUTORMV(DEM,r,c-1))? ((QX2->data[r][c-1])) : 0.0f));
                                float qx2 = std::max(0.0f,((!OUTORMV(DEM,r,c+1))? ((QX1->data[r][c+1])) : 0.0f));
                                float qy1 = std::max(0.0f,((!OUTORMV(DEM,r-1,c))? ((QY2->data[r-1][c])) : 0.0f));
                                float qy2 = std::max(0.0f,((!OUTORMV(DEM,r+1,c))? ((QY1->data[r+1][c])) : 0.0f));

                                float qoutx1 = std::max(0.0f,((QX1->data[r][c])));
                                float qoutx2 = std::max(0.0f,((QX2->data[r][c])));
                                float qouty1 = std::max(0.0f,((QY1->data[r][c])));
                                float qouty2 = std::max(0.0f,((QY2->data[r][c])));

                                //total net incoming discharge
                                float Q = std::max(0.0f, (qoutx1 + qoutx2 + qouty1 + qouty2));// - qoutx1 - qoutx2 - qouty1 - qouty2

                                float QIN = std::max(0.0f,(qx1 + qx2 + qy1 + qy2) + FlowSource->data[r][c]);// - qoutx1 - qoutx2 - qouty1 - qouty2

                                in_total += std::max(0.0f, (qoutx1 + qoutx2 + qouty1 + qouty2));
                                out_total += std::max(0.0f,(qx1 + qx2 + qy1 + qy2));

                                //solve for height, so that new discharge is incoming discharge
                                //float sol = std::max(0.0f,(demhmin -DEM->data[r][c] ))+ std::pow( _dx * QIN*  N /(sqrt(S->Drc + 0.001f)),6.0f/5.0f);

                                float sol = QIN / courant;
                                HS->data[r][c] = sol;

                                float hnew = std::max(std::max(0.0f,demhmin- dem)/hscale,sol);


                                HN->data[r][c] = std::max(0.0f,H->Drc  + QIN* dt - Q* dt );//(H->Drc > 1 || hnew > 1)? std::min(H->Drc * 2.0,std::max(H->Drc*1.0,hnew)) : hnew;

                                QTX1->Drc += qoutx1-qx1;
                                QTX2->Drc += qoutx2-qx2;
                                QTY1->Drc += qouty1-qy1;
                                QTY2->Drc += qouty2-qy2;
                     }
                 }
               }

            #pragma omp barrier


            #pragma omp for collapse(2) reduction(+:hchangetotal,schangetotal)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        hweighttotal += HN->data[r][c];
                        hchangetotal += HN->data[r][c] * std::fabs(HN->data[r][c] - H->data[r][c])/std::max(0.00000001f,H->data[r][c]);
                        H->data[r][c] = HN->data[r][c];
                        if(do_forced)
                        {
                            if(!(HForced->data[r][c] < 0.0f))
                            {
                                H->data[r][c] =HForced->data[r][c];
                            }
                        }

                        //not actually normalized as it should be with sqrt(x2+y2)
                        float sxor = SXO->data[r][c]/std::max(0.001f,std::sqrt(SXO->data[r][c]* SXO->data[r][c])+ std::fabs(SYO->data[r][c]*SYO->data[r][c]));
                        float syor = SYO->data[r][c]/std::max(0.001f,std::sqrt(SXO->data[r][c]* SXO->data[r][c])+ std::fabs(SYO->data[r][c]*SYO->data[r][c]));

                        float sxr = SX->data[r][c]/std::max(0.001f,std::sqrt(SX->data[r][c]* SX->data[r][c])+ std::fabs(SY->data[r][c]*SY->data[r][c]));
                        float syr = SY->data[r][c]/std::max(0.001f,std::sqrt(SY->data[r][c]* SX->data[r][c])+ std::fabs(SY->data[r][c]*SY->data[r][c]));

                        schangetotal +=HN->data[r][c] *(-0.5 * (-1.0 + (sxor*sxr + syor * syr)));

                        SXO->data[r][c] = SX->data[r][c];
                        SYO->data[r][c] = SY->data[r][c];

                    }
                }
            }


            #pragma omp single
            {
                std::cout << "dt is " << dt_req_min << "      with delta = " << (hchangetotal/std::max(0.000001,hweighttotal))/totalpix <<   "  and deltav = " << (schangetotal/std::max(0.000001,hweighttotal))/totalpix << std::endl;

            }

            #pragma omp barrier

        }

        #pragma omp barrier

        #pragma omp for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {

                    HA->data[r][c] = source->data[r][c];

                    //if we are at the edge of a forced section of the domain, we must compensate the initial flow accumulation source to include the flux required to maintain the forced steady-state height at that position
                    if(do_forced)
                    {
                        float forced = HForced->data[r][c];
                        float forcedx1 = !OUTORMV(DEM,r,c-1)? HForced->data[r][c-1] : forced;
                        float forcedx2 = !OUTORMV(DEM,r,c+1)? HForced->data[r][c+1] : forced;
                        float forcedy1 = !OUTORMV(DEM,r-1,c)? HForced->data[r-1][c] : forced;
                        float forcedy2 = !OUTORMV(DEM,r+1,c)? HForced->data[r+1][c] : forced;

                        if(!(forced  < 0.0))
                        {
                            if((forcedx1 < 0.0)||(forcedx2 < 0.0)||(forcedy1 < 0.0)||(forcedy2 < 0.0))
                            {

                                float flowwidth = _dx;
                                float Nhere = 0.1;
                                float q = std::pow(HForced->data[r][c],5.0/3.0)* std::sqrt(std::fabs(S->Drc + 0.001))  /(0.1 * _dx*_dx);

                                HA->data[r][c] = q;
                            }
                        }

                    }


                    S->data[r][c] = 0.0;
                    QTX1->data[r][c] = (QTX1->data[r][c]);
                    QTX2->data[r][c] = (QTX2->data[r][c]);
                    QTY1->data[r][c] = (QTY1->data[r][c]);
                    QTY2->data[r][c] = (QTY2->data[r][c]);

                    QT->data[r][c] = 0.0;

                }
            }
        }



        #pragma omp critical
        {
             i = 0;
        }
        while( true)
        {
            #pragma omp single
            {
                i = i+1;
                ifac = ifac + (float)i;
                stop2 = false;
                std::cout << "i = " << i << std::endl;
            }

            #pragma omp barrier

            bool do_stop = false;
            #pragma omp critical
            {
                i2 = i;

                if(i2>= iter_accu)
                {
                    std::cout << "should break " << std::endl;
                    stop2 = true;
                    do_stop = true;
                }
            }

            #pragma omp barrier

            if(do_stop)
            {
                std::cout << "do break " << std::endl;
                break;
            }

            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    //Get Q Total

                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        float qxa1 = (QTX1->data[r][c]);
                        float qya1 = (QTY1->data[r][c]);

                        float qxa2 = (QTX2->data[r][c]);
                        float qya2 = (QTY2->data[r][c]);

                        float q_total = std::max(1e-12f,std::max(0.0f,qxa1) +std::max(0.0f,qxa2)+std::max(0.0f,qya1)+std::max(0.0f,qya2));

                        QAX1->data[r][c] = HA->data[r][c] * std::max(0.0f,qxa1)/q_total;
                        QAX2->data[r][c] = HA->data[r][c] * std::max(0.0f,qxa2)/q_total;
                        QAY1->data[r][c] = HA->data[r][c] * std::max(0.0f,qya1)/q_total;
                        QAY2->data[r][c] = HA->data[r][c] * std::max(0.0f,qya2)/q_total;


                    }
                }
            }


            #pragma omp barrier

            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        float QTt = 0.0;
                        if(!OUTORMV(DEM,r,c+1))
                        {
                            if(QAX1->data[r][c+1] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAX1->data[r][c+1]);
                            }
                        }
                        if(!OUTORMV(DEM,r,c-1))
                        {
                            if(QAX2->data[r][c-1] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAX2->data[r][c-1]);
                            }
                        }
                        if(!OUTORMV(DEM,r+1,c))
                        {
                            if(QAY1->data[r+1][c] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAY1->data[r+1][c]);
                            }
                        }
                        if(!OUTORMV(DEM,r-1,c))
                        {
                            if(QAY2->data[r-1][c] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAY2->data[r-1][c]);
                            }
                        }

                        S->data[r][c] += QTt;
                        QT->data[r][c] += QTt * (float(i));
                        QM->data[r][c] = std::max(QTt,QM->data[r][c]);
                        HA->data[r][c] = QTt;
                    }
                }
            }
        }

        //final normalized velocity directional field

        #pragma omp for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                //Get Q Total

                if(!pcr::isMV(DEM->data[r][c]))
                {

                        float qxa = (QTX2->data[r][c]-QTX1->data[r][c]);
                        float qya = (QTY2->data[r][c]-QTY1->data[r][c]);

                        float QT2 = std::fabs(qxa) + std::fabs(qya);

                        if(QT2 > 0.0)
                        {
                            //Distribute outflow
                            QAX->data[r][c] =(qxa/QT2);
                            QAY->data[r][c] =(qya/QT2);
                        }else {
                            QAX->data[r][c] = 0.0;
                            QAY->data[r][c] = 0.0;
                        }


                }
            }
        }

    }

    //delete temporary map

    delete HN;
    delete QX1;
    delete QX2;
    delete QY1;
    delete QY2;
    delete SXO;
    delete SYO;

    delete SX;
    delete SY;
    delete QTX1;
    delete QTX2;
    delete QTY1;
    delete QTY2;

    return {H,S,QT,QM,HA,QAX,QAY,QR, QC,HS};
}


inline float GetVelocity(float g, float h1, float h2, float zi, float zi2, float dx, float n)
{

    float den = -(-h1*h1*h1 - h1*h1*h2 + h1 *h2*h2 + h2*h2*h2 - h1*h1* zi - 2.0* h1* h2* zi - h2 * h2* zi + h1*h1*zi2 + 2.0*h1* h2* zi2 + h2*h2*zi2)/(dx*n*n);
    if(den >= 0.0)
    {
        return 0.5 * std::sqrt(den);
    }else
    {
        return -0.5 * std::sqrt(den);
    }

}


//more efficient with real pressure term
inline std::vector<cTMap *> AS_AccuFluxDiffusive3(cTMap * DEMIn, cTMap * source, cTMap * FlowSource, cTMap * HInitial, cTMap * HForced, cTMap * coeff_initial, int iter_max, float courant, float hscale = 1.0, int iter_accu = -1, bool precondition = false,  bool do_forced = false, bool do_stabilize = false, float add_next = 0.0)
{

    if(iter_accu < 0)
    {
        iter_accu = iter_max;
    }

    if(iter_max < 1)
    {
        LISEMS_ERROR("Can not run steady state flow without iterations");
        throw 1;
    }
    if(courant < 1e-12 || courant  > 1.0)
    {
        LISEM_ERROR("Courant value must be between 1e-12 and 1");
        throw 1;
    }
    if(hscale < 1e-10)
    {
        LISEM_ERROR("Height scale value must be larger then 1e-12");
        throw 1;
    }

    cTMap * DEM= DEMIn->GetCopy();

    MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *H = new cTMap(std::move(raster_data),DEM->projection(),"");

    //create temporary maps



    MaskedRaster<float> raster_data2(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *HN = new cTMap(std::move(raster_data2),DEM->projection(),"");

    MaskedRaster<float> raster_data3(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX1 = new cTMap(std::move(raster_data3),DEM->projection(),"");

    MaskedRaster<float> raster_data4(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX2 = new cTMap(std::move(raster_data4),DEM->projection(),"");

    MaskedRaster<float> raster_data5(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY1 = new cTMap(std::move(raster_data5),DEM->projection(),"");

    MaskedRaster<float> raster_data6(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY2 = new cTMap(std::move(raster_data6),DEM->projection(),"");

    MaskedRaster<float> raster_data7(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SX = new cTMap(std::move(raster_data7),DEM->projection(),"");

    MaskedRaster<float> raster_data8(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SY = new cTMap(std::move(raster_data8),DEM->projection(),"");

    MaskedRaster<float> raster_data9(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *S = new cTMap(std::move(raster_data9),DEM->projection(),"");

    cTMap * QAX = QX1->GetCopy();
    cTMap * QAY = QX1->GetCopy();
    cTMap * SXO = QX1->GetCopy0();
    cTMap * SYO = QX1->GetCopy0();

    cTMap * QAX1 = QX1->GetCopy0();
    cTMap * QAY1 = QX1->GetCopy0();
    cTMap * QAX2 = QX1->GetCopy0();
    cTMap * QAY2 = QX1->GetCopy0();

    cTMap * QC = QX1->GetCopy0();
    cTMap * QTX1 = QX1->GetCopy0();
    cTMap * QTX2 = QX1->GetCopy0();
    cTMap * QTY1 = QX1->GetCopy0();
    cTMap * QTY2 = QX1->GetCopy0();
    cTMap * HA = QX1->GetCopy();
    cTMap * QTR = QX1->GetCopy0();
    cTMap * QT = QX1->GetCopy();
    cTMap * QM = QX1->GetCopy0();
    cTMap * QR = QX1->GetCopy0();
    cTMap * HS = QX1->GetCopy0();

    bool stop = false;
    bool stop2 = false;

    double ifac = 0.0;

    //preconditioning

    float dt_req_min = 0.1;

    float totalpix= DEM->data.nr_rows() * DEM->data.nr_cols();
    float maxv = -1e31;
    float minv = 1e31;

    {
        bool found = false;

        #pragma omp parallel for collapse(2) shared(found) reduction(max:maxv)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(pcr::isMV(DEM->data[r][c]))
                {
                }else
                {
                    maxv = std::max(maxv,DEM->data[r][c]);
                    found = true;

                }
            }
        }

        if(!found)
        {
            maxv = 1.0;
        }
    }

    {
        bool found = false;

        #pragma omp parallel for collapse(2) shared(found) reduction(min:minv)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(pcr::isMV(DEM->data[r][c]))
                {
                }else
                {
                    minv = std::min(minv,DEM->data[r][c]);
                    found = true;

                }
            }
        }

        if(!found)
        {
            minv = 0.0;
        }
    }
    if(maxv - minv < 1e-6 && precondition)
    {
        LISEMS_ERROR("Can not run the preconditioning without elevation differences, turning it off");
        precondition = false;
    }

    std::cout << "precondition " << precondition << " " << maxv << " " << minv << std::endl;

    double hchangetotal = 0.0;
    double schangetotal = 0.0;
    double hweighttotal = 0.0;

    int i = 0;
    int i2 = 0;
    #pragma omp parallel private(stop,stop2) shared(i,ifac)
    {


        float N = 0.05;
        double _dx = std::fabs(DEM->cellSizeX());
        float dt = 1.0;

        #pragma omp for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                    float dem = DEM->Drc;
                    float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                    float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                    float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                    float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                    float Sx1 = (demx1 - (dem))/(_dx);
                    float Sx2 = ((dem)- (demx2))/(_dx);
                    float Sy1 = (demy1 - (dem))/(_dx);
                    float Sy2 = ((dem)- (demy2))/(_dx);

                    float sx = 0.0;
                    float sy = 0.0;
                    if(demx1 < demx2)
                    {
                        sx = Sx1;
                    }else
                    {
                        sx = Sx2;
                    }

                    if(demy1 < demy2)
                    {
                        sy = Sy1;
                    }else
                    {
                        sy = Sy2;
                    }

                    S->data[r][c] = (std::fabs(sx) + std::fabs(sy) + 1e-8f);

                    float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                    DEM->data[r][c] = DEM->data[r][c] + pit;
                }

            }
        }



        #pragma omp barrier



        //if(precondition)
        {
            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        if(precondition)
                        {

                            float dem = DEM->Drc;
                            //already checked the maxv and minv values earlier
                            float inv_normz = 1.0 - (dem - minv)/(maxv-minv);
                            float inv_norms = exp(-S->data[r][c]);

                            H->data[r][c] = coeff_initial->data[r][c] * inv_normz * inv_norms;
                        }else
                        {
                            H->data[r][c] = HInitial->data[r][c];
                        }

                        if(do_forced)
                        {
                            if(!(HForced->data[r][c] < 0.0f))
                            {
                                H->data[r][c] =HForced->data[r][c];
                            }
                        }
                    }
                }
            }

        }


        #pragma omp critical
        {
            i = 0;

        }


        while( true)
        {
            #pragma omp single
            {

                schangetotal = 0.0;
                hchangetotal = 0.0;
                hweighttotal = 0.0;

                stop = false;
                i = i+1;
            }

            #pragma omp barrier

            bool do_stop = false;

            #pragma omp critical
            {
                i2 = i;
                if(i2>= iter_max)
                {
                    stop = true;
                    do_stop = true;
                }
                //dt_req_min = 1e6;

            }

            #pragma omp barrier

            if(do_stop)
            {
                break;
            }


            float progress = ((float)(i))/std::max(1.0f,((float)(iter_max)));


            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        float dem = DEM->Drc;
                        float demh =DEM->data[r][c]+hscale *H->data[r][c];
                        bool outlet = false;
                        if(OUTORMV(DEM,r,c-1) || OUTORMV(DEM,r,c+1) || OUTORMV(DEM,r-1,c) || OUTORMV(DEM,r+1,c))
                        {
                            outlet = 1.0;
                        }

                        float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                        float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                        float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                        float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                        float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                        dem = dem + pit;
                        demh = demh + pit;

                        float hx1 = !OUTORMV(DEM,r,c-1)? hscale*H->data[r][c-1] : 0.0f;
                        float hx2 = !OUTORMV(DEM,r,c+1)? hscale*H->data[r][c+1] : 0.0f;
                        float hy1 = !OUTORMV(DEM,r-1,c)? hscale*H->data[r-1][c] : 0.0f;
                        float hy2 = !OUTORMV(DEM,r+1,c)? hscale*H->data[r+1][c] : 0.0f;


                        float demhx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1]+hscale*H->data[r][c-1] : dem;
                        float demhx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1]+hscale*H->data[r][c+1] : dem;
                        float demhy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c]+hscale*H->data[r-1][c] : dem;
                        float demhy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c]+hscale*H->data[r+1][c] : dem;

                        float demhmin = std::min(std::min(std::min(demhx1,demhx2),demhy1),demhy2);
                        float h = std::max(0.0f,H->data[r][c]);


                        float Shx1 = (demhx1 - (demh))/(_dx);
                        float Shx2 = ((demh)- (demhx2))/(_dx);
                        float Shy1 = (demhy1 - (demh))/(_dx);
                        float Shy2 = ((demh)- (demhy2))/(_dx);

                        float Sx1 = (demx1 - (dem))/(_dx);
                        float Sx2 = ((dem)- (demx2))/(_dx);
                        float Sy1 = (demy1 - (dem))/(_dx);
                        float Sy2 = ((dem)- (demy2))/(_dx);

                        if(demhx1 < demhx2)
                        {
                            SX->Drc = Shx1;
                        }else
                        {
                            SX->Drc = Shx2;
                        }

                        if(demhy1 < demhy2)
                        {
                            SY->Drc = Shy1;
                        }else
                        {
                            SY->Drc = Shy2;
                        }

                        float flowwidth = _dx;

                        S->Drc = (std::fabs(SX->Drc) + std::fabs(SY->Drc) + 1e-8);

                        float HL = h;//std::min(h,std::max(0.0,hscale*H->data[r][c] - std::max(0.0,(demhmin -(DEM->data[r][c])))));
                        //float Hm = std::min(hscale*h,std::max(0.0f,hscale*H->data[r][c] - std::max(0.0f,(demhmin -(DEM->data[r][c])))))/hscale;
                        HA->data[r][c] = HL - h;

                        float q = _dx * flowwidth * HL * std::sqrt(std::fabs(S->Drc + 0.001))*(1.0f/N) *std::pow((HL),2.0f/3.0f)/(_dx*_dx);
                        //float qm = 1.0 *flowwidth * Hm * std::sqrt(std::fabs(S->Drc + 0.001))*(1.0f/N) *std::pow((Hm),2.0f/3.0f);

                        //float dt_req = courant * h/std::max(1e-6f,q);

                        q = q ;

                        QR->data[r][c] = q/std::max(0.0001f,h);
                        QC->data[r][c] = q;
                        q = courant *h;//std::min(progress * q + (1.0f-progress) * (courant * h),courant * h);

                        float qx1 =0.0;
                        float qx2 =0.0;
                        float qy1 =0.0;
                        float qy2 =0.0;

                        //ratio of artificial velocity over actual velocity
                        //float pressureforce_fac = courant * _dx /();

                        //the actual pressure force factor is there to compensate for what the heights should be when not using artificial velocities
                        float pressureforce_fac = std::max(0.0001,std::min(1.0,pow(_dx*courant,0.6) * pow(h*N/std::sqrt(std::max(0.0001f,S->data[r][c])),3.0f/5.0f)/std::max(0.000001f,h)));

                        float wx1 = GetVelocity(9.81,h,hx1,dem,demx1,_dx,N);
                        float wx2 = GetVelocity(9.81,h,hx2,dem,demx2,_dx,N);
                        float wy1 = GetVelocity(9.81,h,hy1,dem,demy1,_dx,N);
                        float wy2 = GetVelocity(9.81,h,hy2,dem,demy2,_dx,N);

                        float pfac_x1 = std::min(1.0f,std::max(0.0f,std::fabs(h*h -hx1*hx1)/(std::fabs(h*h -hx1*hx1) + std::fabs(dem - demx1))));
                        float pfac_x2 = std::min(1.0f,std::max(0.0f,std::fabs(h*h -hx2*hx2)/(std::fabs(h*h -hx2*hx2) + std::fabs(dem - demx2))));
                        float pfac_y1 = std::min(1.0f,std::max(0.0f,std::fabs(h*h -hy1*hy1)/(std::fabs(h*h -hy1*hy1) + std::fabs(dem - demy1))));
                        float pfac_y2 = std::min(1.0f,std::max(0.0f,std::fabs(h*h -hy2*hy2)/(std::fabs(h*h -hy2*hy2) + std::fabs(dem - demy2))));

                        wx1 = wx1 * (pressureforce_fac * pfac_x1 + (1.0-pfac_x1)* 1.0);
                        wx2 = wx2 * (pressureforce_fac * pfac_x2 + (1.0-pfac_x2)* 1.0);
                        wy1 = wy1 * (pressureforce_fac * pfac_y1 + (1.0-pfac_y1)* 1.0);
                        wy2 = wy2 * (pressureforce_fac * pfac_y2 + (1.0-pfac_y2)* 1.0);

                        //dt_req_min = std::min(std::max(1.0f/*progress * 1e-5f + (1.0f-progress) * 0.1f*/,dt_req),dt_req_min);

                        //float V = h > 0? std::sqrt(HL) * std::sqrt(std::fabs(S->data[r][c] + 0.001f))*(1.0f/N) *std::pow((HL*_dx)/(2.0f * HL + _dx),2.0f/3.0f) : 0.0f;

                        float ws_total =std::max(1e-6f,((wx1 > 0.0)? std::fabs(wx1): 0.0f) +
                                                 ((wx2 > 0.0)? std::fabs(wx2): 0.0f) +
                                                 ((wy1 > 0.0)? std::fabs(wy1): 0.0f) +
                                                 ((wy2 > 0.0)? std::fabs(wy2): 0.0f));

                        qx1 = (wx1 > 0.0)? std::fabs((wx1)/ws_total) * q : 0.0;
                        if(demhx1 == demh)
                        {
                            qx1 = 0.0;
                        }
                        qx2 = (wx2 > 0.0)? std::fabs((wx2)/ws_total) * q : 0.0;
                        if(demhx2 == demh)
                        {
                            qx2 = 0.0;
                        }
                        qy1 = (wy1 > 0.0)?  std::fabs((wy1)/ws_total) * q : 0.0;
                        if(demhy1 == demh)
                        {
                            qy1 = 0.0;
                        }
                        qy2 = (wy2 > 0.0)? std::fabs((wy2)/ws_total) * q : 0.0;
                        if(demhy2 == demh)
                        {
                            qy2 = 0.0;
                        }
                        qx1 = std::isnormal(qx1)?qx1:0.0;
                        qx2 = std::isnormal(qx2)?qx2:0.0;
                        qy1 = std::isnormal(qy1)?qy1:0.0;
                        qy2 = std::isnormal(qy2)?qy2:0.0;



                        QX1->Drc = qx1;// < 0.0f? qx :0.0f;
                        QX2->Drc = qx2;// > 0.0f? qx :0.0f;
                        QY1->Drc = qy1;// < 0.0f? qy :0.0f;
                        QY2->Drc = qy2;// > 0.0f? qy :0.0f;

                        /*if(do_stabilize)
                        {
                            QX1->Drc = std::max(0.0f,std::min(0.25f *(demh - demhx1),QX1->Drc));// < 0.0f? qx :0.0f;
                            QX2->Drc = std::max(0.0f,std::min(0.25f *(demh - demhx2),QX2->Drc));// > 0.0f? qx :0.0f;
                            QY1->Drc = std::max(0.0f,std::min(0.25f *(demh - demhy1),QY1->Drc));// < 0.0f? qy :0.0f;
                            QY2->Drc = std::max(0.0f,std::min(0.25f *(demh - demhy2),QY2->Drc));// > 0.0f? qy :0.0f;
                        }*/
                    }
                }
            }

            #pragma omp barrier

            //
            {

                //limit flow to height equilibra, to oppose oscillating behavior
                #pragma omp for collapse(2)
                for(int r = 0; r < DEM->data.nr_rows();r++)
                {
                    for(int c = 0; c < DEM->data.nr_cols();c++)
                    {
                        if(!pcr::isMV(DEM->data[r][c]))
                        {
                                float dem = DEM->Drc;

                                float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                                float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                                float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                                float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                                float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                                dem = dem + pit;

                                if(do_stabilize)
                                {
                                    float h = hscale * H->data[r][c];

                                    float demh = dem + h;
                                    float demhx1 = (!OUTORMV(DEM,r,c-1))? DEM->data[r][c-1]+hscale*H->data[r][c-1] : dem;
                                    float demhx2 = (!OUTORMV(DEM,r,c+1))? DEM->data[r][c+1]+hscale*H->data[r][c+1] : dem;
                                    float demhy1 = (!OUTORMV(DEM,r-1,c))? DEM->data[r-1][c]+hscale*H->data[r-1][c] : dem;
                                    float demhy2 = (!OUTORMV(DEM,r+1,c))? DEM->data[r+1][c]+hscale*H->data[r+1][c] : dem;

                                    float qx2_x2 = std::max(0.0f,((!OUTORMV(DEM,r,c+1))? ((QX2->data[r][c+1])) : 0.0f));
                                    float qx1_x1 = std::max(0.0f,((!OUTORMV(DEM,r,c-1))? ((QX1->data[r][c-1])) : 0.0f));
                                    float qy2_y2 = std::max(0.0f,((!OUTORMV(DEM,r+1,c))? ((QY2->data[r+1][c])) : 0.0f));
                                    float qy1_y1 = std::max(0.0f,((!OUTORMV(DEM,r-1,c))? ((QY1->data[r-1][c])) : 0.0f));

                                    QX1->Drc = std::max(0.0f,std::min(0.25f *(demh - demhx1) + add_next*std::fabs(qx1_x1),QX1->Drc));// < 0.0f? qx :0.0f;
                                    QX2->Drc = std::max(0.0f,std::min(0.25f *(demh - demhx2) + add_next*std::fabs(qx2_x2),QX2->Drc));// > 0.0f? qx :0.0f;
                                    QY1->Drc = std::max(0.0f,std::min(0.25f *(demh - demhy1) + add_next*std::fabs(qy1_y1),QY1->Drc));// < 0.0f? qy :0.0f;
                                    QY2->Drc = std::max(0.0f,std::min(0.25f *(demh - demhy2) + add_next*std::fabs(qy2_y2),QY2->Drc));// > 0.0f? qy :0.0f;
                            }


                                QTR->data[r][c] += std::fabs(QX1->data[r][c]) + std::fabs(QX2->data[r][c]) + std::fabs(QY1->data[r][c]) + std::fabs(QY2->data[r][c]);
                        }

                    }
                }



            }

            float in_total = 0.0;
            float out_total = 0.0;

            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        float dem = DEM->Drc;

                                float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                                float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                                float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                                float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                                float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                                dem = dem + pit;

                                float demh = dem + hscale*H->data[r][c];
                                float demhx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1]+hscale*H->data[r][c-1] : dem;
                                float demhx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1]+hscale*H->data[r][c+1] : dem;
                                float demhy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c]+hscale*H->data[r-1][c] : dem;
                                float demhy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c]+hscale*H->data[r+1][c] : dem;

                                float demhx11 = !OUTORMV(DEM,r-1,c-1)? DEM->data[r-1][c-1]+hscale*H->data[r-1][c-1] : dem;
                                float demhx22 = !OUTORMV(DEM,r+1,c+1)? DEM->data[r+1][c+1]+hscale*H->data[r+1][c+1] : dem;
                                float demhy11 = !OUTORMV(DEM,r-1,c-1)? DEM->data[r-1][c+1]+hscale*H->data[r-1][c+1] : dem;
                                float demhy22 = !OUTORMV(DEM,r+1,c+1)? DEM->data[r+1][c-1]+hscale*H->data[r+1][c-1] : dem;

                                float demhmin = std::min(std::min(std::min(demhx1,demhx2),demhy1),demhy2);
                                demhmin =std::min(demhmin, std::min(std::min(std::min(demhx11,demhx22),demhy11),demhy22));

                                float qx1 = std::max(0.0f,((!OUTORMV(DEM,r,c-1))? ((QX2->data[r][c-1])) : 0.0f));
                                float qx2 = std::max(0.0f,((!OUTORMV(DEM,r,c+1))? ((QX1->data[r][c+1])) : 0.0f));
                                float qy1 = std::max(0.0f,((!OUTORMV(DEM,r-1,c))? ((QY2->data[r-1][c])) : 0.0f));
                                float qy2 = std::max(0.0f,((!OUTORMV(DEM,r+1,c))? ((QY1->data[r+1][c])) : 0.0f));

                                float qoutx1 = std::max(0.0f,((QX1->data[r][c])));
                                float qoutx2 = std::max(0.0f,((QX2->data[r][c])));
                                float qouty1 = std::max(0.0f,((QY1->data[r][c])));
                                float qouty2 = std::max(0.0f,((QY2->data[r][c])));

                                //total net incoming discharge
                                float Q = std::max(0.0f, (qoutx1 + qoutx2 + qouty1 + qouty2));// - qoutx1 - qoutx2 - qouty1 - qouty2

                                float QIN = std::max(0.0f,(qx1 + qx2 + qy1 + qy2) + FlowSource->data[r][c]);// - qoutx1 - qoutx2 - qouty1 - qouty2

                                in_total += std::max(0.0f, (qoutx1 + qoutx2 + qouty1 + qouty2));
                                out_total += std::max(0.0f,(qx1 + qx2 + qy1 + qy2));

                                //solve for height, so that new discharge is incoming discharge
                                //float sol = std::max(0.0f,(demhmin -DEM->data[r][c] ))+ std::pow( _dx * QIN*  N /(sqrt(S->Drc + 0.001f)),6.0f/5.0f);

                                float sol = QIN / courant;
                                HS->data[r][c] = sol;

                                float hnew = std::max(std::max(0.0f,demhmin- dem)/hscale,sol);


                                HN->data[r][c] = std::max(0.0f,H->Drc  + QIN* dt - Q* dt );//(H->Drc > 1 || hnew > 1)? std::min(H->Drc * 2.0,std::max(H->Drc*1.0,hnew)) : hnew;

                                QTX1->Drc += qoutx1-qx1;
                                QTX2->Drc += qoutx2-qx2;
                                QTY1->Drc += qouty1-qy1;
                                QTY2->Drc += qouty2-qy2;
                     }
                 }
               }

            #pragma omp barrier

        #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        H->data[r][c] = HN->data[r][c];

                        if(do_forced)
                        {
                            if(!(HForced->data[r][c] < 0.0f))
                            {
                                H->data[r][c] =HForced->data[r][c];
                            }
                        }
                    }
                }
            }


           /* #pragma omp for collapse(2) reduction(+:hchangetotal,schangetotal)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        hweighttotal += HN->data[r][c];
                        hchangetotal += HN->data[r][c] * std::fabs(HN->data[r][c] - H->data[r][c])/std::max(0.00000001f,H->data[r][c]);
                        H->data[r][c] = HN->data[r][c];
                        if(do_forced)
                        {
                            if(!(HForced->data[r][c] < 0.0f))
                            {
                                H->data[r][c] =HForced->data[r][c];
                            }
                        }

                        //not actually normalized as it should be with sqrt(x2+y2)
                        float sxor = SXO->data[r][c]/std::max(0.001f,std::sqrt(SXO->data[r][c]* SXO->data[r][c])+ std::fabs(SYO->data[r][c]*SYO->data[r][c]));
                        float syor = SYO->data[r][c]/std::max(0.001f,std::sqrt(SXO->data[r][c]* SXO->data[r][c])+ std::fabs(SYO->data[r][c]*SYO->data[r][c]));

                        float sxr = SX->data[r][c]/std::max(0.001f,std::sqrt(SX->data[r][c]* SX->data[r][c])+ std::fabs(SY->data[r][c]*SY->data[r][c]));
                        float syr = SY->data[r][c]/std::max(0.001f,std::sqrt(SY->data[r][c]* SX->data[r][c])+ std::fabs(SY->data[r][c]*SY->data[r][c]));

                        schangetotal +=HN->data[r][c] *(-0.5 * (-1.0 + (sxor*sxr + syor * syr)));

                        SXO->data[r][c] = SX->data[r][c];
                        SYO->data[r][c] = SY->data[r][c];

                    }
                }
            }*/


            #pragma omp barrier

        }

        #pragma omp barrier

        #pragma omp for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {

                    HA->data[r][c] = source->data[r][c];

                    //if we are at the edge of a forced section of the domain, we must compensate the initial flow accumulation source to include the flux required to maintain the forced steady-state height at that position
                    if(do_forced)
                    {
                        float forced = HForced->data[r][c];
                        float forcedx1 = !OUTORMV(DEM,r,c-1)? HForced->data[r][c-1] : forced;
                        float forcedx2 = !OUTORMV(DEM,r,c+1)? HForced->data[r][c+1] : forced;
                        float forcedy1 = !OUTORMV(DEM,r-1,c)? HForced->data[r-1][c] : forced;
                        float forcedy2 = !OUTORMV(DEM,r+1,c)? HForced->data[r+1][c] : forced;

                        if(!(forced  < 0.0))
                        {
                            if((forcedx1 < 0.0)||(forcedx2 < 0.0)||(forcedy1 < 0.0)||(forcedy2 < 0.0))
                            {

                                float flowwidth = _dx;
                                float Nhere = 0.1;
                                float q = std::pow(HForced->data[r][c],5.0/3.0)* std::sqrt(std::fabs(S->Drc + 0.001))  /(0.1 * _dx*_dx);

                                HA->data[r][c] = q;
                            }
                        }

                    }


                    S->data[r][c] = 0.0;
                    QTX1->data[r][c] = (QTX1->data[r][c]);
                    QTX2->data[r][c] = (QTX2->data[r][c]);
                    QTY1->data[r][c] = (QTY1->data[r][c]);
                    QTY2->data[r][c] = (QTY2->data[r][c]);

                    QT->data[r][c] = 0.0;

                }
            }
        }



        #pragma omp critical
        {
             i = 0;
        }
        while( true)
        {
            #pragma omp single
            {
                i = i+1;
                ifac = ifac + (float)i;
                stop2 = false;
                std::cout << "i = " << i << std::endl;
            }

            #pragma omp barrier

            bool do_stop = false;
            #pragma omp critical
            {
                i2 = i;

                if(i2>= iter_accu)
                {
                    std::cout << "should break " << std::endl;
                    stop2 = true;
                    do_stop = true;
                }
            }

            #pragma omp barrier

            if(do_stop)
            {
                std::cout << "do break " << std::endl;
                break;
            }

            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    //Get Q Total

                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        float qxa1 = (QTX1->data[r][c]);
                        float qya1 = (QTY1->data[r][c]);

                        float qxa2 = (QTX2->data[r][c]);
                        float qya2 = (QTY2->data[r][c]);

                        float q_total = std::max(1e-12f,std::max(0.0f,qxa1) +std::max(0.0f,qxa2)+std::max(0.0f,qya1)+std::max(0.0f,qya2));

                        QAX1->data[r][c] = HA->data[r][c] * std::max(0.0f,qxa1)/q_total;
                        QAX2->data[r][c] = HA->data[r][c] * std::max(0.0f,qxa2)/q_total;
                        QAY1->data[r][c] = HA->data[r][c] * std::max(0.0f,qya1)/q_total;
                        QAY2->data[r][c] = HA->data[r][c] * std::max(0.0f,qya2)/q_total;


                    }
                }
            }


            #pragma omp barrier

            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        float QTt = 0.0;
                        if(!OUTORMV(DEM,r,c+1))
                        {
                            if(QAX1->data[r][c+1] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAX1->data[r][c+1]);
                            }
                        }
                        if(!OUTORMV(DEM,r,c-1))
                        {
                            if(QAX2->data[r][c-1] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAX2->data[r][c-1]);
                            }
                        }
                        if(!OUTORMV(DEM,r+1,c))
                        {
                            if(QAY1->data[r+1][c] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAY1->data[r+1][c]);
                            }
                        }
                        if(!OUTORMV(DEM,r-1,c))
                        {
                            if(QAY2->data[r-1][c] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAY2->data[r-1][c]);
                            }
                        }

                        S->data[r][c] += QTt;
                        QT->data[r][c] += QTt * (float(i));
                        QM->data[r][c] = std::max(QTt,QM->data[r][c]);
                        HA->data[r][c] = QTt;
                    }
                }
            }
        }

        //final normalized velocity directional field

        #pragma omp for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                //Get Q Total

                if(!pcr::isMV(DEM->data[r][c]))
                {

                        float qxa = (QTX2->data[r][c]-QTX1->data[r][c]);
                        float qya = (QTY2->data[r][c]-QTY1->data[r][c]);

                        float QT2 = std::fabs(qxa) + std::fabs(qya);

                        if(QT2 > 0.0)
                        {
                            //Distribute outflow
                            QAX->data[r][c] =(qxa/QT2);
                            QAY->data[r][c] =(qya/QT2);
                        }else {
                            QAX->data[r][c] = 0.0;
                            QAY->data[r][c] = 0.0;
                        }


                }
            }
        }

    }

    //delete temporary map

    delete HN;
    //delete QX1;
    //delete QX2;
    //delete QY1;
    //delete QY2;
    delete SXO;
    delete SYO;

    delete SX;
    delete SY;
    //delete QTX1;
    //delete QTX2;
    //delete QTY1;
    //delete QTY2;

    return {H,S,QT,QM,HA,QAX,QAY,QTX1,QTX2,QTY1,QTY2,QX1,QX2,QY1,QY2, QTR};
}

//more efficient with real pressure term
//more efficient with real pressure term
inline std::vector<cTMap *> AS_AccuFluxDiffusive32(cTMap * DEMIn, cTMap * source, cTMap * FlowSource, cTMap * HInitial, cTMap * HForced, cTMap * coeff_initial, cTMap * scale_pressure, int iter_max, float courant, float hscale = 1.0, int iter_accu = -1, bool precondition = false,  bool do_forced = false, bool do_stabilize = false, float add_next = 0.0)
{

    if(iter_accu < 0)
    {
        iter_accu = iter_max;
    }

    if(iter_max < 1)
    {
        LISEMS_ERROR("Can not run steady state flow without iterations");
        throw 1;
    }
    if(courant < 1e-12 || courant  > 1.0)
    {
        LISEM_ERROR("Courant value must be between 1e-12 and 1");
        throw 1;
    }
    if(hscale < 1e-10)
    {
        LISEM_ERROR("Height scale value must be larger then 1e-12");
        throw 1;
    }

    cTMap * DEM= DEMIn->GetCopy();

    MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *H = new cTMap(std::move(raster_data),DEM->projection(),"");

    //create temporary maps



    MaskedRaster<float> raster_data2(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *HN = new cTMap(std::move(raster_data2),DEM->projection(),"");

    MaskedRaster<float> raster_data3(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX1 = new cTMap(std::move(raster_data3),DEM->projection(),"");

    MaskedRaster<float> raster_data4(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX2 = new cTMap(std::move(raster_data4),DEM->projection(),"");

    MaskedRaster<float> raster_data5(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY1 = new cTMap(std::move(raster_data5),DEM->projection(),"");

    MaskedRaster<float> raster_data6(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY2 = new cTMap(std::move(raster_data6),DEM->projection(),"");

    MaskedRaster<float> raster_data7(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SX = new cTMap(std::move(raster_data7),DEM->projection(),"");

    MaskedRaster<float> raster_data8(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SY = new cTMap(std::move(raster_data8),DEM->projection(),"");

    MaskedRaster<float> raster_data9(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *S = new cTMap(std::move(raster_data9),DEM->projection(),"");

    cTMap * QAX = QX1->GetCopy();
    cTMap * QAY = QX1->GetCopy();
    cTMap * SXO = QX1->GetCopy0();
    cTMap * SYO = QX1->GetCopy0();

    cTMap * QAX1 = QX1->GetCopy0();
    cTMap * QAY1 = QX1->GetCopy0();
    cTMap * QAX2 = QX1->GetCopy0();
    cTMap * QAY2 = QX1->GetCopy0();

    cTMap * QC = QX1->GetCopy0();
    cTMap * QTX1 = QX1->GetCopy0();
    cTMap * QTX2 = QX1->GetCopy0();
    cTMap * QTY1 = QX1->GetCopy0();
    cTMap * QTY2 = QX1->GetCopy0();
    cTMap * HA = QX1->GetCopy();
    cTMap * QTR = QX1->GetCopy0();
    cTMap * QT = QX1->GetCopy();
    cTMap * QM = QX1->GetCopy0();
    cTMap * QR = QX1->GetCopy0();
    cTMap * HS = QX1->GetCopy0();

    bool stop = false;
    bool stop2 = false;

    double ifac = 0.0;

    //preconditioning

    float dt_req_min = 0.1;

    float totalpix= DEM->data.nr_rows() * DEM->data.nr_cols();
    float maxv = -1e31;
    float minv = 1e31;

    {
        bool found = false;

        #pragma omp parallel for collapse(2) shared(found) reduction(max:maxv)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(pcr::isMV(DEM->data[r][c]))
                {
                }else
                {
                    maxv = std::max(maxv,DEM->data[r][c]);
                    found = true;

                }
            }
        }

        if(!found)
        {
            maxv = 1.0;
        }
    }

    {
        bool found = false;

        #pragma omp parallel for collapse(2) shared(found) reduction(min:minv)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(pcr::isMV(DEM->data[r][c]))
                {
                }else
                {
                    minv = std::min(minv,DEM->data[r][c]);
                    found = true;

                }
            }
        }

        if(!found)
        {
            minv = 0.0;
        }
    }
    if(maxv - minv < 1e-6 && precondition)
    {
        LISEMS_ERROR("Can not run the preconditioning without elevation differences, turning it off");
        precondition = false;
    }

    std::cout << "precondition " << precondition << " " << maxv << " " << minv << std::endl;

    double hchangetotal = 0.0;
    double schangetotal = 0.0;
    double hweighttotal = 0.0;

    int i = 0;
    int i2 = 0;
    #pragma omp parallel private(stop,stop2) shared(i,ifac)
    {


        float N = 0.05;
        double _dx = std::fabs(DEM->cellSizeX());
        float dt = 1.0;

        #pragma omp for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                    float dem = DEM->Drc;
                    float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                    float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                    float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                    float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                    float Sx1 = (demx1 - (dem))/(_dx);
                    float Sx2 = ((dem)- (demx2))/(_dx);
                    float Sy1 = (demy1 - (dem))/(_dx);
                    float Sy2 = ((dem)- (demy2))/(_dx);

                    float sx = 0.0;
                    float sy = 0.0;
                    if(demx1 < demx2)
                    {
                        sx = Sx1;
                    }else
                    {
                        sx = Sx2;
                    }

                    if(demy1 < demy2)
                    {
                        sy = Sy1;
                    }else
                    {
                        sy = Sy2;
                    }

                    S->data[r][c] = (std::fabs(sx) + std::fabs(sy) + 1e-8f);

                    float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                    DEM->data[r][c] = DEM->data[r][c] + pit;
                }

            }
        }



        #pragma omp barrier



        //if(precondition)
        {
            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        if(precondition)
                        {

                            float dem = DEM->Drc;
                            //already checked the maxv and minv values earlier
                            float inv_normz = 1.0 - (dem - minv)/(maxv-minv);
                            float inv_norms = exp(-S->data[r][c]);

                            H->data[r][c] = coeff_initial->data[r][c] * inv_normz * inv_norms;
                        }else
                        {
                            H->data[r][c] = HInitial->data[r][c];
                        }

                        if(do_forced)
                        {
                            if(!(HForced->data[r][c] < 0.0f))
                            {
                                H->data[r][c] =HForced->data[r][c];
                            }
                        }
                    }
                }
            }

        }


        #pragma omp critical
        {
            i = 0;

        }


        while( true)
        {
            #pragma omp single
            {

                schangetotal = 0.0;
                hchangetotal = 0.0;
                hweighttotal = 0.0;

                stop = false;
                i = i+1;
            }

            #pragma omp barrier

            bool do_stop = false;

            #pragma omp critical
            {
                i2 = i;
                if(i2>= iter_max)
                {
                    stop = true;
                    do_stop = true;
                }
                //dt_req_min = 1e6;

            }

            #pragma omp barrier

            if(do_stop)
            {
                break;
            }


            float progress = ((float)(i))/std::max(1.0f,((float)(iter_max)));


            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        float dem = DEM->Drc;
                        float demh =DEM->data[r][c]+hscale *H->data[r][c];
                        bool outlet = false;
                        if(OUTORMV(DEM,r,c-1) || OUTORMV(DEM,r,c+1) || OUTORMV(DEM,r-1,c) || OUTORMV(DEM,r+1,c))
                        {
                            outlet = 1.0;
                        }

                        float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                        float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                        float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                        float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                        float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                        dem = dem + pit;
                        demh = demh + pit;

                        float hx1 = !OUTORMV(DEM,r,c-1)? hscale*H->data[r][c-1] : 0.0f;
                        float hx2 = !OUTORMV(DEM,r,c+1)? hscale*H->data[r][c+1] : 0.0f;
                        float hy1 = !OUTORMV(DEM,r-1,c)? hscale*H->data[r-1][c] : 0.0f;
                        float hy2 = !OUTORMV(DEM,r+1,c)? hscale*H->data[r+1][c] : 0.0f;


                        float demhx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1]+hscale*H->data[r][c-1] : dem;
                        float demhx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1]+hscale*H->data[r][c+1] : dem;
                        float demhy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c]+hscale*H->data[r-1][c] : dem;
                        float demhy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c]+hscale*H->data[r+1][c] : dem;

                        float demhmin = std::min(std::min(std::min(demhx1,demhx2),demhy1),demhy2);
                        float h = std::max(0.0f,H->data[r][c]);


                        float Shx1 = (demhx1 - (demh))/(_dx);
                        float Shx2 = ((demh)- (demhx2))/(_dx);
                        float Shy1 = (demhy1 - (demh))/(_dx);
                        float Shy2 = ((demh)- (demhy2))/(_dx);

                        float Sx1 = (demx1 - (dem))/(_dx);
                        float Sx2 = ((dem)- (demx2))/(_dx);
                        float Sy1 = (demy1 - (dem))/(_dx);
                        float Sy2 = ((dem)- (demy2))/(_dx);

                        if(demhx1 < demhx2)
                        {
                            SX->Drc = Shx1;
                        }else
                        {
                            SX->Drc = Shx2;
                        }

                        if(demhy1 < demhy2)
                        {
                            SY->Drc = Shy1;
                        }else
                        {
                            SY->Drc = Shy2;
                        }

                        float flowwidth = _dx;

                        S->Drc = (std::fabs(SX->Drc) + std::fabs(SY->Drc) + 1e-8);

                        float HL = h;//std::min(h,std::max(0.0,hscale*H->data[r][c] - std::max(0.0,(demhmin -(DEM->data[r][c])))));
                        //float Hm = std::min(hscale*h,std::max(0.0f,hscale*H->data[r][c] - std::max(0.0f,(demhmin -(DEM->data[r][c])))))/hscale;
                        HA->data[r][c] = HL - h;

                        float q = _dx * flowwidth * HL * std::sqrt(std::fabs(S->Drc + 0.001))*(1.0f/N) *std::pow((HL),2.0f/3.0f)/(_dx*_dx);
                        //float qm = 1.0 *flowwidth * Hm * std::sqrt(std::fabs(S->Drc + 0.001))*(1.0f/N) *std::pow((Hm),2.0f/3.0f);

                        //float dt_req = courant * h/std::max(1e-6f,q);

                        q = q ;

                        QR->data[r][c] = q/std::max(0.0001f,h);
                        QC->data[r][c] = q;
                        q = courant *h;//std::min(progress * q + (1.0f-progress) * (courant * h),courant * h);

                        float qx1 =0.0;
                        float qx2 =0.0;
                        float qy1 =0.0;
                        float qy2 =0.0;

                        //ratio of artificial velocity over actual velocity
                        //float pressureforce_fac = courant * _dx /();

                        //the actual pressure force factor is there to compensate for what the heights should be when not using artificial velocities
                        float pressureforce_fac =  std::max(0.0001,std::min(1.0,pow(_dx*courant,0.6) * pow(h*N/std::sqrt(std::max(0.0001f,S->data[r][c])),3.0f/5.0f)/std::max(0.000001f,h)));

                        float hratio_x1 = !OUTORMV(DEM,r,c-1)? scale_pressure->data[r][c-1]  : 1.0f;
                        float hratio_x2 = !OUTORMV(DEM,r,c+1)? scale_pressure->data[r][c+1]  : 1.0f;
                        float hratio_y1 = !OUTORMV(DEM,r-1,c)? scale_pressure->data[r-1][c]  : 1.0f;
                        float hratio_y2 = !OUTORMV(DEM,r+1,c)? scale_pressure->data[r+1][c]  : 1.0f;


                        float scale_pressure_x1= 0.5 * (scale_pressure->data[r][c] + hratio_x1);
                        float scale_pressure_x2= 0.5 * (scale_pressure->data[r][c] + hratio_x2);
                        float scale_pressure_y1= 0.5 * (scale_pressure->data[r][c] + hratio_y1);
                        float scale_pressure_y2= 0.5 * (scale_pressure->data[r][c] + hratio_y2);

                        hx1 = hx1 *scale_pressure->data[r][c]/std::max(0.001f,hratio_x1);
                        hx2 = hx2 *scale_pressure->data[r][c]/std::max(0.001f,hratio_x2);
                        hy1 = hy1 *scale_pressure->data[r][c]/std::max(0.001f,hratio_y1);
                        hy2 = hy2 *scale_pressure->data[r][c]/std::max(0.001f,hratio_y2);

                        float wx1 = GetVelocity(9.81,h,hx1,scale_pressure_x1 *dem,scale_pressure_x1 *demx1,_dx,N);
                        float wx2 = GetVelocity(9.81,h,hx2,scale_pressure_x2 *dem,scale_pressure_x2 *demx2,_dx,N);
                        float wy1 = GetVelocity(9.81,h,hy1,scale_pressure_y1 *dem,scale_pressure_y1 *demy1,_dx,N);
                        float wy2 = GetVelocity(9.81,h,hy2,scale_pressure_y2 *dem,scale_pressure_y2 *demy2,_dx,N);

                        float pfac_x1 = std::min(1.0f,std::max(0.0f,std::fabs(h*h -hx1*hx1)/(std::fabs(h*h -hx1*hx1) + scale_pressure_x1 *std::fabs(dem - demx1))));
                        float pfac_x2 = std::min(1.0f,std::max(0.0f,std::fabs(h*h -hx2*hx2)/(std::fabs(h*h -hx2*hx2) + scale_pressure_x2 *std::fabs(dem - demx2))));
                        float pfac_y1 = std::min(1.0f,std::max(0.0f,std::fabs(h*h -hy1*hy1)/(std::fabs(h*h -hy1*hy1) + scale_pressure_y1 * std::fabs(dem - demy1))));
                        float pfac_y2 = std::min(1.0f,std::max(0.0f,std::fabs(h*h -hy2*hy2)/(std::fabs(h*h -hy2*hy2) + scale_pressure_y2 * std::fabs(dem - demy2))));

                        wx1 = wx1 * (pressureforce_fac * pfac_x1 + (1.0-pfac_x1)* 1.0);
                        wx2 = wx2 * (pressureforce_fac * pfac_x2 + (1.0-pfac_x2)* 1.0);
                        wy1 = wy1 * (pressureforce_fac * pfac_y1 + (1.0-pfac_y1)* 1.0);
                        wy2 = wy2 * (pressureforce_fac * pfac_y2 + (1.0-pfac_y2)* 1.0);

                        //dt_req_min = std::min(std::max(1.0f/*progress * 1e-5f + (1.0f-progress) * 0.1f*/,dt_req),dt_req_min);

                        //float V = h > 0? std::sqrt(HL) * std::sqrt(std::fabs(S->data[r][c] + 0.001f))*(1.0f/N) *std::pow((HL*_dx)/(2.0f * HL + _dx),2.0f/3.0f) : 0.0f;

                        float ws_total =std::max(1e-6f,((wx1 > 0.0)? std::fabs(wx1): 0.0f) +
                                                 ((wx2 > 0.0)? std::fabs(wx2): 0.0f) +
                                                 ((wy1 > 0.0)? std::fabs(wy1): 0.0f) +
                                                 ((wy2 > 0.0)? std::fabs(wy2): 0.0f));

                        qx1 = (wx1 > 0.0)? std::fabs((wx1)/ws_total) * q : 0.0;
                        if(demhx1 == demh)
                        {
                            qx1 = 0.0;
                        }
                        qx2 = (wx2 > 0.0)? std::fabs((wx2)/ws_total) * q : 0.0;
                        if(demhx2 == demh)
                        {
                            qx2 = 0.0;
                        }
                        qy1 = (wy1 > 0.0)?  std::fabs((wy1)/ws_total) * q : 0.0;
                        if(demhy1 == demh)
                        {
                            qy1 = 0.0;
                        }
                        qy2 = (wy2 > 0.0)? std::fabs((wy2)/ws_total) * q : 0.0;
                        if(demhy2 == demh)
                        {
                            qy2 = 0.0;
                        }
                        qx1 = std::isnormal(qx1)?qx1:0.0;
                        qx2 = std::isnormal(qx2)?qx2:0.0;
                        qy1 = std::isnormal(qy1)?qy1:0.0;
                        qy2 = std::isnormal(qy2)?qy2:0.0;



                        QX1->Drc = qx1;// < 0.0f? qx :0.0f;
                        QX2->Drc = qx2;// > 0.0f? qx :0.0f;
                        QY1->Drc = qy1;// < 0.0f? qy :0.0f;
                        QY2->Drc = qy2;// > 0.0f? qy :0.0f;

                        /*if(do_stabilize)
                        {
                            QX1->Drc = std::max(0.0f,std::min(0.25f *(demh - demhx1),QX1->Drc));// < 0.0f? qx :0.0f;
                            QX2->Drc = std::max(0.0f,std::min(0.25f *(demh - demhx2),QX2->Drc));// > 0.0f? qx :0.0f;
                            QY1->Drc = std::max(0.0f,std::min(0.25f *(demh - demhy1),QY1->Drc));// < 0.0f? qy :0.0f;
                            QY2->Drc = std::max(0.0f,std::min(0.25f *(demh - demhy2),QY2->Drc));// > 0.0f? qy :0.0f;
                        }*/
                    }
                }
            }

            #pragma omp barrier

            //
            {

                //limit flow to height equilibra, to oppose oscillating behavior
                #pragma omp for collapse(2)
                for(int r = 0; r < DEM->data.nr_rows();r++)
                {
                    for(int c = 0; c < DEM->data.nr_cols();c++)
                    {
                        if(!pcr::isMV(DEM->data[r][c]))
                        {
                                float dem = DEM->Drc;

                                float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                                float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                                float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                                float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                                float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                                dem = dem + pit;

                                if(do_stabilize)
                                {
                                    float h = hscale * H->data[r][c];

                                    float hratio_x1 = !OUTORMV(DEM,r,c-1)? scale_pressure->data[r][c-1]  : 1.0f;
                                    float hratio_x2 = !OUTORMV(DEM,r,c+1)? scale_pressure->data[r][c+1]  : 1.0f;
                                    float hratio_y1 = !OUTORMV(DEM,r-1,c)? scale_pressure->data[r-1][c]  : 1.0f;
                                    float hratio_y2 = !OUTORMV(DEM,r+1,c)? scale_pressure->data[r+1][c]  : 1.0f;

                                    float hx1 = !OUTORMV(DEM,r,c-1)? hscale*H->data[r][c-1] : 0.0f;
                                    float hx2 = !OUTORMV(DEM,r,c+1)? hscale*H->data[r][c+1] : 0.0f;
                                    float hy1 = !OUTORMV(DEM,r-1,c)? hscale*H->data[r-1][c] : 0.0f;
                                    float hy2 = !OUTORMV(DEM,r+1,c)? hscale*H->data[r+1][c] : 0.0f;


                                    float scale_pressure_x1= 0.5 * (scale_pressure->data[r][c] + hratio_x1);
                                    float scale_pressure_x2= 0.5 * (scale_pressure->data[r][c] + hratio_x2);
                                    float scale_pressure_y1= 0.5 * (scale_pressure->data[r][c] + hratio_y1);
                                    float scale_pressure_y2= 0.5 * (scale_pressure->data[r][c] + hratio_y2);

                                    hx1 = hx1 *scale_pressure->data[r][c]/std::max(0.001f,hratio_x1);
                                    hx2 = hx2 *scale_pressure->data[r][c]/std::max(0.001f,hratio_x2);
                                    hy1 = hy1 *scale_pressure->data[r][c]/std::max(0.001f,hratio_y1);
                                    hy2 = hy2 *scale_pressure->data[r][c]/std::max(0.001f,hratio_y2);


                                    float demh = dem + h;
                                    float demx1 = (!OUTORMV(DEM,r,c-1))? DEM->data[r][c-1]: dem;
                                    float demx2 = (!OUTORMV(DEM,r,c+1))? DEM->data[r][c+1] : dem;
                                    float demy1 = (!OUTORMV(DEM,r-1,c))? DEM->data[r-1][c] : dem;
                                    float demy2 = (!OUTORMV(DEM,r+1,c))? DEM->data[r+1][c] : dem;

                                    float qx2_x2 = std::max(0.0f,((!OUTORMV(DEM,r,c+1))? ((QX2->data[r][c+1])) : 0.0f));
                                    float qx1_x1 = std::max(0.0f,((!OUTORMV(DEM,r,c-1))? ((QX1->data[r][c-1])) : 0.0f));
                                    float qy2_y2 = std::max(0.0f,((!OUTORMV(DEM,r+1,c))? ((QY2->data[r+1][c])) : 0.0f));
                                    float qy1_y1 = std::max(0.0f,((!OUTORMV(DEM,r-1,c))? ((QY1->data[r-1][c])) : 0.0f));

                                    QX1->Drc = std::max(0.0f,std::min(0.25f *(scale_pressure_x1 * dem + h - scale_pressure_x1 *demx1 -hx1) + add_next*std::fabs(qx1_x1),QX1->Drc));// < 0.0f? qx :0.0f;
                                    QX2->Drc = std::max(0.0f,std::min(0.25f *(scale_pressure_x2 * dem + h - scale_pressure_x2 *demx2 -hx2) + add_next*std::fabs(qx2_x2),QX2->Drc));// > 0.0f? qx :0.0f;
                                    QY1->Drc = std::max(0.0f,std::min(0.25f *(scale_pressure_y1 * dem + h - scale_pressure_y1 *demy1 -hy1) + add_next*std::fabs(qy1_y1),QY1->Drc));// < 0.0f? qy :0.0f;
                                    QY2->Drc = std::max(0.0f,std::min(0.25f *(scale_pressure_y2 * dem + h - scale_pressure_y2 *demy2 -hy2) + add_next*std::fabs(qy2_y2),QY2->Drc));// > 0.0f? qy :0.0f;
                            }


                                QTR->data[r][c] += std::fabs(QX1->data[r][c]) + std::fabs(QX2->data[r][c]) + std::fabs(QY1->data[r][c]) + std::fabs(QY2->data[r][c]);
                        }

                    }
                }



            }

            float in_total = 0.0;
            float out_total = 0.0;

            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        float dem = DEM->Drc;

                                float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                                float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                                float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                                float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                                float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                                dem = dem + pit;

                                float demh = dem + hscale*H->data[r][c];
                                float demhx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1]+hscale*H->data[r][c-1] : dem;
                                float demhx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1]+hscale*H->data[r][c+1] : dem;
                                float demhy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c]+hscale*H->data[r-1][c] : dem;
                                float demhy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c]+hscale*H->data[r+1][c] : dem;

                                float demhx11 = !OUTORMV(DEM,r-1,c-1)? DEM->data[r-1][c-1]+hscale*H->data[r-1][c-1] : dem;
                                float demhx22 = !OUTORMV(DEM,r+1,c+1)? DEM->data[r+1][c+1]+hscale*H->data[r+1][c+1] : dem;
                                float demhy11 = !OUTORMV(DEM,r-1,c-1)? DEM->data[r-1][c+1]+hscale*H->data[r-1][c+1] : dem;
                                float demhy22 = !OUTORMV(DEM,r+1,c+1)? DEM->data[r+1][c-1]+hscale*H->data[r+1][c-1] : dem;

                                float demhmin = std::min(std::min(std::min(demhx1,demhx2),demhy1),demhy2);
                                demhmin =std::min(demhmin, std::min(std::min(std::min(demhx11,demhx22),demhy11),demhy22));

                                float qx1 = std::max(0.0f,((!OUTORMV(DEM,r,c-1))? ((QX2->data[r][c-1])) : 0.0f));
                                float qx2 = std::max(0.0f,((!OUTORMV(DEM,r,c+1))? ((QX1->data[r][c+1])) : 0.0f));
                                float qy1 = std::max(0.0f,((!OUTORMV(DEM,r-1,c))? ((QY2->data[r-1][c])) : 0.0f));
                                float qy2 = std::max(0.0f,((!OUTORMV(DEM,r+1,c))? ((QY1->data[r+1][c])) : 0.0f));

                                float qoutx1 = std::max(0.0f,((QX1->data[r][c])));
                                float qoutx2 = std::max(0.0f,((QX2->data[r][c])));
                                float qouty1 = std::max(0.0f,((QY1->data[r][c])));
                                float qouty2 = std::max(0.0f,((QY2->data[r][c])));

                                //total net incoming discharge
                                float Q = std::max(0.0f, (qoutx1 + qoutx2 + qouty1 + qouty2));// - qoutx1 - qoutx2 - qouty1 - qouty2

                                float QIN = std::max(0.0f,(qx1 + qx2 + qy1 + qy2) + FlowSource->data[r][c]);// - qoutx1 - qoutx2 - qouty1 - qouty2

                                in_total += std::max(0.0f, (qoutx1 + qoutx2 + qouty1 + qouty2));
                                out_total += std::max(0.0f,(qx1 + qx2 + qy1 + qy2));

                                //solve for height, so that new discharge is incoming discharge
                                //float sol = std::max(0.0f,(demhmin -DEM->data[r][c] ))+ std::pow( _dx * QIN*  N /(sqrt(S->Drc + 0.001f)),6.0f/5.0f);

                                float sol = QIN / courant;
                                HS->data[r][c] = sol;

                                float hnew = std::max(std::max(0.0f,demhmin- dem)/hscale,sol);


                                HN->data[r][c] = std::max(0.0f,H->Drc  + QIN* dt - Q* dt );//(H->Drc > 1 || hnew > 1)? std::min(H->Drc * 2.0,std::max(H->Drc*1.0,hnew)) : hnew;

                                QTX1->Drc += qoutx1-qx1;
                                QTX2->Drc += qoutx2-qx2;
                                QTY1->Drc += qouty1-qy1;
                                QTY2->Drc += qouty2-qy2;
                     }
                 }
               }

            #pragma omp barrier

        #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        H->data[r][c] = HN->data[r][c];

                        if(do_forced)
                        {
                            if(!(HForced->data[r][c] < 0.0f))
                            {
                                H->data[r][c] =HForced->data[r][c];
                            }
                        }
                    }
                }
            }


           /* #pragma omp for collapse(2) reduction(+:hchangetotal,schangetotal)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        hweighttotal += HN->data[r][c];
                        hchangetotal += HN->data[r][c] * std::fabs(HN->data[r][c] - H->data[r][c])/std::max(0.00000001f,H->data[r][c]);
                        H->data[r][c] = HN->data[r][c];
                        if(do_forced)
                        {
                            if(!(HForced->data[r][c] < 0.0f))
                            {
                                H->data[r][c] =HForced->data[r][c];
                            }
                        }

                        //not actually normalized as it should be with sqrt(x2+y2)
                        float sxor = SXO->data[r][c]/std::max(0.001f,std::sqrt(SXO->data[r][c]* SXO->data[r][c])+ std::fabs(SYO->data[r][c]*SYO->data[r][c]));
                        float syor = SYO->data[r][c]/std::max(0.001f,std::sqrt(SXO->data[r][c]* SXO->data[r][c])+ std::fabs(SYO->data[r][c]*SYO->data[r][c]));

                        float sxr = SX->data[r][c]/std::max(0.001f,std::sqrt(SX->data[r][c]* SX->data[r][c])+ std::fabs(SY->data[r][c]*SY->data[r][c]));
                        float syr = SY->data[r][c]/std::max(0.001f,std::sqrt(SY->data[r][c]* SX->data[r][c])+ std::fabs(SY->data[r][c]*SY->data[r][c]));

                        schangetotal +=HN->data[r][c] *(-0.5 * (-1.0 + (sxor*sxr + syor * syr)));

                        SXO->data[r][c] = SX->data[r][c];
                        SYO->data[r][c] = SY->data[r][c];

                    }
                }
            }*/


            #pragma omp barrier

        }

        #pragma omp barrier

        #pragma omp for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {

                    HA->data[r][c] = source->data[r][c];

                    //if we are at the edge of a forced section of the domain, we must compensate the initial flow accumulation source to include the flux required to maintain the forced steady-state height at that position
                    if(do_forced)
                    {
                        float forced = HForced->data[r][c];
                        float forcedx1 = !OUTORMV(DEM,r,c-1)? HForced->data[r][c-1] : forced;
                        float forcedx2 = !OUTORMV(DEM,r,c+1)? HForced->data[r][c+1] : forced;
                        float forcedy1 = !OUTORMV(DEM,r-1,c)? HForced->data[r-1][c] : forced;
                        float forcedy2 = !OUTORMV(DEM,r+1,c)? HForced->data[r+1][c] : forced;

                        if(!(forced  < 0.0))
                        {
                            if((forcedx1 < 0.0)||(forcedx2 < 0.0)||(forcedy1 < 0.0)||(forcedy2 < 0.0))
                            {

                                float flowwidth = _dx;
                                float Nhere = 0.1;
                                float q = std::pow(HForced->data[r][c],5.0/3.0)* std::sqrt(std::fabs(S->Drc + 0.001))  /(0.1 * _dx*_dx);

                                HA->data[r][c] = q;
                            }
                        }

                    }


                    S->data[r][c] = 0.0;
                    QTX1->data[r][c] = (QTX1->data[r][c]);
                    QTX2->data[r][c] = (QTX2->data[r][c]);
                    QTY1->data[r][c] = (QTY1->data[r][c]);
                    QTY2->data[r][c] = (QTY2->data[r][c]);

                    QT->data[r][c] = 0.0;

                }
            }
        }



        #pragma omp critical
        {
             i = 0;
        }
        while( true)
        {
            #pragma omp single
            {
                i = i+1;
                ifac = ifac + (float)i;
                stop2 = false;
                std::cout << "i = " << i << std::endl;
            }

            #pragma omp barrier

            bool do_stop = false;
            #pragma omp critical
            {
                i2 = i;

                if(i2>= iter_accu)
                {
                    std::cout << "should break " << std::endl;
                    stop2 = true;
                    do_stop = true;
                }
            }

            #pragma omp barrier

            if(do_stop)
            {
                std::cout << "do break " << std::endl;
                break;
            }

            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    //Get Q Total

                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        float qxa1 = (QTX1->data[r][c]);
                        float qya1 = (QTY1->data[r][c]);

                        float qxa2 = (QTX2->data[r][c]);
                        float qya2 = (QTY2->data[r][c]);

                        float q_total = std::max(1e-12f,std::max(0.0f,qxa1) +std::max(0.0f,qxa2)+std::max(0.0f,qya1)+std::max(0.0f,qya2));

                        QAX1->data[r][c] = HA->data[r][c] * std::max(0.0f,qxa1)/q_total;
                        QAX2->data[r][c] = HA->data[r][c] * std::max(0.0f,qxa2)/q_total;
                        QAY1->data[r][c] = HA->data[r][c] * std::max(0.0f,qya1)/q_total;
                        QAY2->data[r][c] = HA->data[r][c] * std::max(0.0f,qya2)/q_total;


                    }
                }
            }


            #pragma omp barrier

            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        float QTt = 0.0;
                        if(!OUTORMV(DEM,r,c+1))
                        {
                            if(QAX1->data[r][c+1] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAX1->data[r][c+1]);
                            }
                        }
                        if(!OUTORMV(DEM,r,c-1))
                        {
                            if(QAX2->data[r][c-1] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAX2->data[r][c-1]);
                            }
                        }
                        if(!OUTORMV(DEM,r+1,c))
                        {
                            if(QAY1->data[r+1][c] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAY1->data[r+1][c]);
                            }
                        }
                        if(!OUTORMV(DEM,r-1,c))
                        {
                            if(QAY2->data[r-1][c] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAY2->data[r-1][c]);
                            }
                        }

                        S->data[r][c] += QTt;
                        QT->data[r][c] += QTt * (float(i));
                        QM->data[r][c] = std::max(QTt,QM->data[r][c]);
                        HA->data[r][c] = QTt;
                    }
                }
            }
        }

        //final normalized velocity directional field

        #pragma omp for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                //Get Q Total

                if(!pcr::isMV(DEM->data[r][c]))
                {

                        float qxa = (QTX2->data[r][c]-QTX1->data[r][c]);
                        float qya = (QTY2->data[r][c]-QTY1->data[r][c]);

                        float QT2 = std::fabs(qxa) + std::fabs(qya);

                        if(QT2 > 0.0)
                        {
                            //Distribute outflow
                            QAX->data[r][c] =(qxa/QT2);
                            QAY->data[r][c] =(qya/QT2);
                        }else {
                            QAX->data[r][c] = 0.0;
                            QAY->data[r][c] = 0.0;
                        }


                }
            }
        }

    }

    //delete temporary map

    delete HN;
    //delete QX1;
    //delete QX2;
    //delete QY1;
    //delete QY2;
    delete SXO;
    delete SYO;

    delete SX;
    delete SY;
    //delete QTX1;
    //delete QTX2;
    //delete QTY1;
    //delete QTY2;

    return {H,S,QT,QM,HA,QAX,QAY,QTX1,QTX2,QTY1,QTY2,QX1,QX2,QY1,QY2, QTR};
}

inline std::vector<cTMap *> AccuFluxDiffusive(cTMap * DEMIn, cTMap * FlowSource, cTMap * HInitial, cTMap * HForced, cTMap * Terrain_Scale, int iter_max, float courant, float hscale = 1.0,  bool precondition = false,  bool do_forced = false, bool do_stabilize = false, float add_next = 0.0)
{

    if(iter_max < 1)
    {
        LISEMS_ERROR("Can not run steady state flow without iterations");
        throw 1;
    }
    if(courant < 1e-12 || courant  > 1.0)
    {
        LISEM_ERROR("Courant value must be between 1e-12 and 1");
        throw 1;
    }
    if(hscale < 1e-10)
    {
        LISEM_ERROR("Height scale value must be larger then 1e-12");
        throw 1;
    }

    cTMap * DEM= DEMIn->GetCopy();

    MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *H = new cTMap(std::move(raster_data),DEM->projection(),"");

    //create temporary maps



    MaskedRaster<float> raster_data2(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *HN = new cTMap(std::move(raster_data2),DEM->projection(),"");

    MaskedRaster<float> raster_data3(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX1 = new cTMap(std::move(raster_data3),DEM->projection(),"");

    MaskedRaster<float> raster_data4(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX2 = new cTMap(std::move(raster_data4),DEM->projection(),"");

    MaskedRaster<float> raster_data5(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY1 = new cTMap(std::move(raster_data5),DEM->projection(),"");

    MaskedRaster<float> raster_data6(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY2 = new cTMap(std::move(raster_data6),DEM->projection(),"");

    MaskedRaster<float> raster_data7(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SX = new cTMap(std::move(raster_data7),DEM->projection(),"");

    MaskedRaster<float> raster_data8(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SY = new cTMap(std::move(raster_data8),DEM->projection(),"");

    MaskedRaster<float> raster_data9(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *S = new cTMap(std::move(raster_data9),DEM->projection(),"");



    bool stop = false;
    bool stop2 = false;

    double ifac = 0.0;

    //preconditioning

    float dt_req_min = 0.1;

    float totalpix= DEM->data.nr_rows() * DEM->data.nr_cols();

    double hchangetotal = 0.0;
    double schangetotal = 0.0;
    double hweighttotal = 0.0;

    int i = 0;
    int i2 = 0;
    #pragma omp parallel private(stop,stop2) shared(i,ifac)
    {


        float N = 0.05;
        double _dx = std::fabs(DEM->cellSizeX());
        float dt = 1.0;

        #pragma omp for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                    float dem = DEM->Drc;
                    float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                    float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                    float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                    float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                    float Sx1 = (demx1 - (dem))/(_dx);
                    float Sx2 = ((dem)- (demx2))/(_dx);
                    float Sy1 = (demy1 - (dem))/(_dx);
                    float Sy2 = ((dem)- (demy2))/(_dx);

                    float sx = 0.0;
                    float sy = 0.0;
                    if(demx1 < demx2)
                    {
                        sx = Sx1;
                    }else
                    {
                        sx = Sx2;
                    }

                    if(demy1 < demy2)
                    {
                        sy = Sy1;
                    }else
                    {
                        sy = Sy2;
                    }

                    S->data[r][c] = (std::fabs(sx) + std::fabs(sy) + 1e-8f);

                    float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                    DEM->data[r][c] = DEM->data[r][c] + pit;
                }

            }
        }



        #pragma omp barrier



        //if(precondition)
        {
            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        {
                            H->data[r][c] = HInitial->data[r][c];
                        }

                        if(do_forced)
                        {
                            if(!(HForced->data[r][c] < 0.0f))
                            {
                                H->data[r][c] =HForced->data[r][c];
                            }
                        }
                    }
                }
            }

        }


        #pragma omp critical
        {
            i = 0;

        }


        while( true)
        {
            #pragma omp single
            {

                schangetotal = 0.0;
                hchangetotal = 0.0;
                hweighttotal = 0.0;

                stop = false;
                i = i+1;
            }

            #pragma omp barrier

            bool do_stop = false;

            #pragma omp critical
            {
                i2 = i;
                if(i2>= iter_max)
                {
                    stop = true;
                    do_stop = true;
                }
                //dt_req_min = 1e6;

            }

            #pragma omp barrier

            if(do_stop)
            {
                break;
            }


            float progress = ((float)(i))/std::max(1.0f,((float)(iter_max)));

            float courant_here = courant * (1.0-progress) + progress * (1.0/8.0)*courant;

            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        float dem = DEM->Drc;
                        float demh =DEM->data[r][c]+hscale *H->data[r][c];
                        bool outlet = false;
                        if(OUTORMV(DEM,r,c-1) || OUTORMV(DEM,r,c+1) || OUTORMV(DEM,r-1,c) || OUTORMV(DEM,r+1,c))
                        {
                            outlet = 1.0;
                        }

                        float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                        float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                        float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                        float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                        float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                        dem = dem + pit;
                        demh = demh + pit;

                        float hx1 = !OUTORMV(DEM,r,c-1)? hscale*H->data[r][c-1] : 0.0f;
                        float hx2 = !OUTORMV(DEM,r,c+1)? hscale*H->data[r][c+1] : 0.0f;
                        float hy1 = !OUTORMV(DEM,r-1,c)? hscale*H->data[r-1][c] : 0.0f;
                        float hy2 = !OUTORMV(DEM,r+1,c)? hscale*H->data[r+1][c] : 0.0f;


                        float demhx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1]+hscale*H->data[r][c-1] : dem;
                        float demhx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1]+hscale*H->data[r][c+1] : dem;
                        float demhy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c]+hscale*H->data[r-1][c] : dem;
                        float demhy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c]+hscale*H->data[r+1][c] : dem;

                        float demhmin = std::min(std::min(std::min(demhx1,demhx2),demhy1),demhy2);
                        float h = std::max(0.0f,H->data[r][c]);


                        float Shx1 = (demhx1 - (demh))/(_dx);
                        float Shx2 = ((demh)- (demhx2))/(_dx);
                        float Shy1 = (demhy1 - (demh))/(_dx);
                        float Shy2 = ((demh)- (demhy2))/(_dx);

                        float Sx1 = (demx1 - (dem))/(_dx);
                        float Sx2 = ((dem)- (demx2))/(_dx);
                        float Sy1 = (demy1 - (dem))/(_dx);
                        float Sy2 = ((dem)- (demy2))/(_dx);

                        if(demhx1 < demhx2)
                        {
                            SX->Drc = Shx1;
                        }else
                        {
                            SX->Drc = Shx2;
                        }

                        if(demhy1 < demhy2)
                        {
                            SY->Drc = Shy1;
                        }else
                        {
                            SY->Drc = Shy2;
                        }

                        float flowwidth = _dx;

                        S->Drc = (std::fabs(SX->Drc) + std::fabs(SY->Drc) + 1e-8);

                        float HL = h;//std::min(h,std::max(0.0,hscale*H->data[r][c] - std::max(0.0,(demhmin -(DEM->data[r][c])))));
                        //float Hm = std::min(hscale*h,std::max(0.0f,hscale*H->data[r][c] - std::max(0.0f,(demhmin -(DEM->data[r][c])))))/hscale;

                        float q = _dx * flowwidth * HL * std::sqrt(std::fabs(S->Drc + 0.001))*(1.0f/N) *std::pow((HL),2.0f/3.0f)/(_dx*_dx);
                        //float qm = 1.0 *flowwidth * Hm * std::sqrt(std::fabs(S->Drc + 0.001))*(1.0f/N) *std::pow((Hm),2.0f/3.0f);

                        //float dt_req = courant * h/std::max(1e-6f,q);

                        q = q ;

                        q = courant_here *h;//std::min(progress * q + (1.0f-progress) * (courant * h),courant * h);

                        float qx1 =0.0;
                        float qx2 =0.0;
                        float qy1 =0.0;
                        float qy2 =0.0;

                        //ratio of artificial velocity over actual velocity
                        //float pressureforce_fac = courant * _dx /();

                        //the actual pressure force factor is there to compensate for what the heights should be when not using artificial velocities
                        float pressureforce_fac = std::max(0.0001,std::min(1.0,pow(_dx*courant_here,0.6) * pow(h*N/std::sqrt(std::max(0.0001f,S->data[r][c])),3.0f/5.0f)/std::max(0.000001f,h)));

                        float wx1 = GetVelocity(9.81,h,hx1,dem,demx1,_dx,N);
                        float wx2 = GetVelocity(9.81,h,hx2,dem,demx2,_dx,N);
                        float wy1 = GetVelocity(9.81,h,hy1,dem,demy1,_dx,N);
                        float wy2 = GetVelocity(9.81,h,hy2,dem,demy2,_dx,N);

                        float pfac_x1 = std::min(1.0f,std::max(0.0f,std::fabs(h*h -hx1*hx1)/(std::fabs(h*h -hx1*hx1) + std::fabs(dem - demx1))));
                        float pfac_x2 = std::min(1.0f,std::max(0.0f,std::fabs(h*h -hx2*hx2)/(std::fabs(h*h -hx2*hx2) + std::fabs(dem - demx2))));
                        float pfac_y1 = std::min(1.0f,std::max(0.0f,std::fabs(h*h -hy1*hy1)/(std::fabs(h*h -hy1*hy1) + std::fabs(dem - demy1))));
                        float pfac_y2 = std::min(1.0f,std::max(0.0f,std::fabs(h*h -hy2*hy2)/(std::fabs(h*h -hy2*hy2) + std::fabs(dem - demy2))));

                        wx1 = wx1 * (pressureforce_fac * pfac_x1 + (1.0-pfac_x1)* 1.0);
                        wx2 = wx2 * (pressureforce_fac * pfac_x2 + (1.0-pfac_x2)* 1.0);
                        wy1 = wy1 * (pressureforce_fac * pfac_y1 + (1.0-pfac_y1)* 1.0);
                        wy2 = wy2 * (pressureforce_fac * pfac_y2 + (1.0-pfac_y2)* 1.0);

                        //dt_req_min = std::min(std::max(1.0f/*progress * 1e-5f + (1.0f-progress) * 0.1f*/,dt_req),dt_req_min);

                        //float V = h > 0? std::sqrt(HL) * std::sqrt(std::fabs(S->data[r][c] + 0.001f))*(1.0f/N) *std::pow((HL*_dx)/(2.0f * HL + _dx),2.0f/3.0f) : 0.0f;

                        float ws_total =std::max(1e-6f,((wx1 > 0.0)? std::fabs(wx1): 0.0f) +
                                                 ((wx2 > 0.0)? std::fabs(wx2): 0.0f) +
                                                 ((wy1 > 0.0)? std::fabs(wy1): 0.0f) +
                                                 ((wy2 > 0.0)? std::fabs(wy2): 0.0f));

                        qx1 = (wx1 > 0.0)? std::fabs((wx1)/ws_total) * q : 0.0;
                        if(demhx1 == demh)
                        {
                            qx1 = 0.0;
                        }
                        qx2 = (wx2 > 0.0)? std::fabs((wx2)/ws_total) * q : 0.0;
                        if(demhx2 == demh)
                        {
                            qx2 = 0.0;
                        }
                        qy1 = (wy1 > 0.0)?  std::fabs((wy1)/ws_total) * q : 0.0;
                        if(demhy1 == demh)
                        {
                            qy1 = 0.0;
                        }
                        qy2 = (wy2 > 0.0)? std::fabs((wy2)/ws_total) * q : 0.0;
                        if(demhy2 == demh)
                        {
                            qy2 = 0.0;
                        }
                        qx1 = std::isnormal(qx1)?qx1:0.0;
                        qx2 = std::isnormal(qx2)?qx2:0.0;
                        qy1 = std::isnormal(qy1)?qy1:0.0;
                        qy2 = std::isnormal(qy2)?qy2:0.0;



                        QX1->Drc = qx1;// < 0.0f? qx :0.0f;
                        QX2->Drc = qx2;// > 0.0f? qx :0.0f;
                        QY1->Drc = qy1;// < 0.0f? qy :0.0f;
                        QY2->Drc = qy2;// > 0.0f? qy :0.0f;

                        /*if(do_stabilize)
                        {
                            QX1->Drc = std::max(0.0f,std::min(0.25f *(demh - demhx1),QX1->Drc));// < 0.0f? qx :0.0f;
                            QX2->Drc = std::max(0.0f,std::min(0.25f *(demh - demhx2),QX2->Drc));// > 0.0f? qx :0.0f;
                            QY1->Drc = std::max(0.0f,std::min(0.25f *(demh - demhy1),QY1->Drc));// < 0.0f? qy :0.0f;
                            QY2->Drc = std::max(0.0f,std::min(0.25f *(demh - demhy2),QY2->Drc));// > 0.0f? qy :0.0f;
                        }*/
                    }
                }
            }

            #pragma omp barrier

            //
            {

                //limit flow to height equilibra, to oppose oscillating behavior
                #pragma omp for collapse(2)
                for(int r = 0; r < DEM->data.nr_rows();r++)
                {
                    for(int c = 0; c < DEM->data.nr_cols();c++)
                    {
                        if(!pcr::isMV(DEM->data[r][c]))
                        {
                                float dem = DEM->Drc;

                                float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                                float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                                float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                                float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                                float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                                dem = dem + pit;

                                if(i + 15 >= iter_max)
                                {
                                    float h = hscale * H->data[r][c];

                                    float demh = dem + h;
                                    float demhx1 = (!OUTORMV(DEM,r,c-1))? DEM->data[r][c-1]+hscale*H->data[r][c-1] : dem;
                                    float demhx2 = (!OUTORMV(DEM,r,c+1))? DEM->data[r][c+1]+hscale*H->data[r][c+1] : dem;
                                    float demhy1 = (!OUTORMV(DEM,r-1,c))? DEM->data[r-1][c]+hscale*H->data[r-1][c] : dem;
                                    float demhy2 = (!OUTORMV(DEM,r+1,c))? DEM->data[r+1][c]+hscale*H->data[r+1][c] : dem;

                                    float qx2_x2 = std::max(0.0f,((!OUTORMV(DEM,r,c+1))? ((QX2->data[r][c+1])) : 0.0f));
                                    float qx1_x1 = std::max(0.0f,((!OUTORMV(DEM,r,c-1))? ((QX1->data[r][c-1])) : 0.0f));
                                    float qy2_y2 = std::max(0.0f,((!OUTORMV(DEM,r+1,c))? ((QY2->data[r+1][c])) : 0.0f));
                                    float qy1_y1 = std::max(0.0f,((!OUTORMV(DEM,r-1,c))? ((QY1->data[r-1][c])) : 0.0f));

                                    QX1->Drc = std::max(0.0f,std::min(0.25f *(demh - demhx1) + add_next*std::fabs(qx1_x1),QX1->Drc));// < 0.0f? qx :0.0f;
                                    QX2->Drc = std::max(0.0f,std::min(0.25f *(demh - demhx2) + add_next*std::fabs(qx2_x2),QX2->Drc));// > 0.0f? qx :0.0f;
                                    QY1->Drc = std::max(0.0f,std::min(0.25f *(demh - demhy1) + add_next*std::fabs(qy1_y1),QY1->Drc));// < 0.0f? qy :0.0f;
                                    QY2->Drc = std::max(0.0f,std::min(0.25f *(demh - demhy2) + add_next*std::fabs(qy2_y2),QY2->Drc));// > 0.0f? qy :0.0f;
                            }

     }

                    }
                }



            }

            float in_total = 0.0;
            float out_total = 0.0;

            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        float dem = DEM->Drc;

                                float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                                float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                                float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                                float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                                float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                                dem = dem + pit;

                                float demh = dem + hscale*H->data[r][c];
                                float demhx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1]+hscale*H->data[r][c-1] : dem;
                                float demhx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1]+hscale*H->data[r][c+1] : dem;
                                float demhy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c]+hscale*H->data[r-1][c] : dem;
                                float demhy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c]+hscale*H->data[r+1][c] : dem;

                                float demhx11 = !OUTORMV(DEM,r-1,c-1)? DEM->data[r-1][c-1]+hscale*H->data[r-1][c-1] : dem;
                                float demhx22 = !OUTORMV(DEM,r+1,c+1)? DEM->data[r+1][c+1]+hscale*H->data[r+1][c+1] : dem;
                                float demhy11 = !OUTORMV(DEM,r-1,c-1)? DEM->data[r-1][c+1]+hscale*H->data[r-1][c+1] : dem;
                                float demhy22 = !OUTORMV(DEM,r+1,c+1)? DEM->data[r+1][c-1]+hscale*H->data[r+1][c-1] : dem;

                                float demhmin = std::min(std::min(std::min(demhx1,demhx2),demhy1),demhy2);
                                demhmin =std::min(demhmin, std::min(std::min(std::min(demhx11,demhx22),demhy11),demhy22));

                                float qx1 = std::max(0.0f,((!OUTORMV(DEM,r,c-1))? ((QX2->data[r][c-1])) : 0.0f));
                                float qx2 = std::max(0.0f,((!OUTORMV(DEM,r,c+1))? ((QX1->data[r][c+1])) : 0.0f));
                                float qy1 = std::max(0.0f,((!OUTORMV(DEM,r-1,c))? ((QY2->data[r-1][c])) : 0.0f));
                                float qy2 = std::max(0.0f,((!OUTORMV(DEM,r+1,c))? ((QY1->data[r+1][c])) : 0.0f));

                                float qoutx1 = std::max(0.0f,((QX1->data[r][c])));
                                float qoutx2 = std::max(0.0f,((QX2->data[r][c])));
                                float qouty1 = std::max(0.0f,((QY1->data[r][c])));
                                float qouty2 = std::max(0.0f,((QY2->data[r][c])));

                                //total net incoming discharge
                                float Q = std::max(0.0f, (qoutx1 + qoutx2 + qouty1 + qouty2));// - qoutx1 - qoutx2 - qouty1 - qouty2

                                float QIN = std::max(0.0f,(qx1 + qx2 + qy1 + qy2) + FlowSource->data[r][c]);// - qoutx1 - qoutx2 - qouty1 - qouty2

                                in_total += std::max(0.0f, (qoutx1 + qoutx2 + qouty1 + qouty2));
                                out_total += std::max(0.0f,(qx1 + qx2 + qy1 + qy2));

                                //solve for height, so that new discharge is incoming discharge
                                //float sol = std::max(0.0f,(demhmin -DEM->data[r][c] ))+ std::pow( _dx * QIN*  N /(sqrt(S->Drc + 0.001f)),6.0f/5.0f);

                                float sol = QIN / courant_here;

                                float hnew = std::max(std::max(0.0f,demhmin- dem)/hscale,sol);


                                HN->data[r][c] = std::max(0.0f,H->Drc  + QIN* dt - Q* dt );//(H->Drc > 1 || hnew > 1)? std::min(H->Drc * 2.0,std::max(H->Drc*1.0,hnew)) : hnew;

                     }
                 }
               }

            #pragma omp barrier

        #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        H->data[r][c] = HN->data[r][c];

                        if(do_forced)
                        {
                            if(!(HForced->data[r][c] < 0.0f))
                            {
                                H->data[r][c] =HForced->data[r][c];
                            }
                        }
                    }
                }
            }
        }
    }


    delete SX;
    delete SY;

    return {H,QX1,QX2,QY1,QY2};

}

inline std::vector<cTMap *> AccuFluxDiffusiveCP(cTMap * DEMIn, cTMap * FlowSource, cTMap * HInitial, cTMap * HForced, cTMap * scale_pressure, int iter_max, float courant, float hscale = 1.0,  bool precondition = false,  bool do_forced = false, bool do_stabilize = false, float add_next = 0.0)
{

    if(iter_max < 1)
    {
        LISEMS_ERROR("Can not run steady state flow without iterations");
        throw 1;
    }
    if(courant < 1e-12 || courant  > 1.0)
    {
        LISEM_ERROR("Courant value must be between 1e-12 and 1");
        throw 1;
    }
    if(hscale < 1e-10)
    {
        LISEM_ERROR("Height scale value must be larger then 1e-12");
        throw 1;
    }

    cTMap * DEM= DEMIn->GetCopy();

    MaskedRaster<float> raster_data(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *H = new cTMap(std::move(raster_data),DEM->projection(),"");

    //create temporary maps



    MaskedRaster<float> raster_data2(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *HN = new cTMap(std::move(raster_data2),DEM->projection(),"");

    MaskedRaster<float> raster_data3(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX1 = new cTMap(std::move(raster_data3),DEM->projection(),"");

    MaskedRaster<float> raster_data4(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QX2 = new cTMap(std::move(raster_data4),DEM->projection(),"");

    MaskedRaster<float> raster_data5(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY1 = new cTMap(std::move(raster_data5),DEM->projection(),"");

    MaskedRaster<float> raster_data6(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *QY2 = new cTMap(std::move(raster_data6),DEM->projection(),"");

    MaskedRaster<float> raster_data7(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SX = new cTMap(std::move(raster_data7),DEM->projection(),"");

    MaskedRaster<float> raster_data8(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *SY = new cTMap(std::move(raster_data8),DEM->projection(),"");

    MaskedRaster<float> raster_data9(DEM->data.nr_rows(), DEM->data.nr_cols(), DEM->data.north(), DEM->data.west(), DEM->data.cell_size(), DEM->data.cell_sizeY());
    cTMap *S = new cTMap(std::move(raster_data9),DEM->projection(),"");



    bool stop = false;
    bool stop2 = false;

    double ifac = 0.0;

    //preconditioning

    float dt_req_min = 0.1;

    float totalpix= DEM->data.nr_rows() * DEM->data.nr_cols();

    double hchangetotal = 0.0;
    double schangetotal = 0.0;
    double hweighttotal = 0.0;

    int i = 0;
    int i2 = 0;
    #pragma omp parallel private(stop,stop2) shared(i,ifac)
    {


        float N = 0.05;
        double _dx = std::fabs(DEM->cellSizeX());
        float dt = 1.0;

        #pragma omp for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                    float dem = DEM->Drc;
                    float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                    float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                    float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                    float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                    float Sx1 = (demx1 - (dem))/(_dx);
                    float Sx2 = ((dem)- (demx2))/(_dx);
                    float Sy1 = (demy1 - (dem))/(_dx);
                    float Sy2 = ((dem)- (demy2))/(_dx);

                    float sx = 0.0;
                    float sy = 0.0;
                    if(demx1 < demx2)
                    {
                        sx = Sx1;
                    }else
                    {
                        sx = Sx2;
                    }

                    if(demy1 < demy2)
                    {
                        sy = Sy1;
                    }else
                    {
                        sy = Sy2;
                    }

                    S->data[r][c] = (std::fabs(sx) + std::fabs(sy) + 1e-8f);

                    float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                    DEM->data[r][c] = DEM->data[r][c] + pit;
                }

            }
        }



        #pragma omp barrier



        //if(precondition)
        {
            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        {
                            H->data[r][c] = HInitial->data[r][c];
                        }

                        if(do_forced)
                        {
                            if(!(HForced->data[r][c] < 0.0f))
                            {
                                H->data[r][c] =HForced->data[r][c];
                            }
                        }
                    }
                }
            }

        }


        #pragma omp critical
        {
            i = 0;

        }


        while( true)
        {
            #pragma omp single
            {

                schangetotal = 0.0;
                hchangetotal = 0.0;
                hweighttotal = 0.0;

                stop = false;
                i = i+1;
            }

            #pragma omp barrier

            bool do_stop = false;

            #pragma omp critical
            {
                i2 = i;
                if(i2>= iter_max)
                {
                    stop = true;
                    do_stop = true;
                }
                //dt_req_min = 1e6;

            }

            #pragma omp barrier

            if(do_stop)
            {
                break;
            }


            float progress = ((float)(i))/std::max(1.0f,((float)(iter_max)));

            float courant_here = courant * (1.0-progress) + progress * (1.0/8.0)*courant;

            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        float dem = DEM->Drc;
                        float demh =DEM->data[r][c]+hscale *H->data[r][c];
                        bool outlet = false;
                        if(OUTORMV(DEM,r,c-1) || OUTORMV(DEM,r,c+1) || OUTORMV(DEM,r-1,c) || OUTORMV(DEM,r+1,c))
                        {
                            outlet = 1.0;
                        }

                        float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                        float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                        float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                        float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                        float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                        dem = dem + pit;
                        demh = demh + pit;

                        float hx1 = !OUTORMV(DEM,r,c-1)? hscale*H->data[r][c-1] : 0.0f;
                        float hx2 = !OUTORMV(DEM,r,c+1)? hscale*H->data[r][c+1] : 0.0f;
                        float hy1 = !OUTORMV(DEM,r-1,c)? hscale*H->data[r-1][c] : 0.0f;
                        float hy2 = !OUTORMV(DEM,r+1,c)? hscale*H->data[r+1][c] : 0.0f;


                        float demhx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1]+hscale*H->data[r][c-1] : dem;
                        float demhx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1]+hscale*H->data[r][c+1] : dem;
                        float demhy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c]+hscale*H->data[r-1][c] : dem;
                        float demhy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c]+hscale*H->data[r+1][c] : dem;

                        float demhmin = std::min(std::min(std::min(demhx1,demhx2),demhy1),demhy2);
                        float h = std::max(0.0f,H->data[r][c]);


                        float Shx1 = (demhx1 - (demh))/(_dx);
                        float Shx2 = ((demh)- (demhx2))/(_dx);
                        float Shy1 = (demhy1 - (demh))/(_dx);
                        float Shy2 = ((demh)- (demhy2))/(_dx);

                        float Sx1 = (demx1 - (dem))/(_dx);
                        float Sx2 = ((dem)- (demx2))/(_dx);
                        float Sy1 = (demy1 - (dem))/(_dx);
                        float Sy2 = ((dem)- (demy2))/(_dx);

                        if(demhx1 < demhx2)
                        {
                            SX->Drc = Shx1;
                        }else
                        {
                            SX->Drc = Shx2;
                        }

                        if(demhy1 < demhy2)
                        {
                            SY->Drc = Shy1;
                        }else
                        {
                            SY->Drc = Shy2;
                        }

                        float flowwidth = _dx;

                        S->Drc = (std::fabs(SX->Drc) + std::fabs(SY->Drc) + 1e-8);

                        float HL = h;//std::min(h,std::max(0.0,hscale*H->data[r][c] - std::max(0.0,(demhmin -(DEM->data[r][c])))));
                        //float Hm = std::min(hscale*h,std::max(0.0f,hscale*H->data[r][c] - std::max(0.0f,(demhmin -(DEM->data[r][c])))))/hscale;

                        float q = _dx * flowwidth * HL * std::sqrt(std::fabs(S->Drc + 0.001))*(1.0f/N) *std::pow((HL),2.0f/3.0f)/(_dx*_dx);
                        //float qm = 1.0 *flowwidth * Hm * std::sqrt(std::fabs(S->Drc + 0.001))*(1.0f/N) *std::pow((Hm),2.0f/3.0f);

                        //float dt_req = courant * h/std::max(1e-6f,q);

                        q = q ;

                        q = courant_here *h;//std::min(progress * q + (1.0f-progress) * (courant * h),courant * h);

                        float qx1 =0.0;
                        float qx2 =0.0;
                        float qy1 =0.0;
                        float qy2 =0.0;

                        //ratio of artificial velocity over actual velocity
                        //float pressureforce_fac = courant * _dx /();

                        //the actual pressure force factor is there to compensate for what the heights should be when not using artificial velocities
                        float pressureforce_fac = std::max(0.0001,std::min(1.0,pow(_dx*courant_here,0.6) * pow(h*N/std::sqrt(std::max(0.0001f,S->data[r][c])),3.0f/5.0f)/std::max(0.000001f,h)));


                        float hratio_x1 = !OUTORMV(DEM,r,c-1)? scale_pressure->data[r][c-1]  : 1.0f;
                        float hratio_x2 = !OUTORMV(DEM,r,c+1)? scale_pressure->data[r][c+1]  : 1.0f;
                        float hratio_y1 = !OUTORMV(DEM,r-1,c)? scale_pressure->data[r-1][c]  : 1.0f;
                        float hratio_y2 = !OUTORMV(DEM,r+1,c)? scale_pressure->data[r+1][c]  : 1.0f;


                        float scale_pressure_x1= 0.5 * (scale_pressure->data[r][c] + hratio_x1);
                        float scale_pressure_x2= 0.5 * (scale_pressure->data[r][c] + hratio_x2);
                        float scale_pressure_y1= 0.5 * (scale_pressure->data[r][c] + hratio_y1);
                        float scale_pressure_y2= 0.5 * (scale_pressure->data[r][c] + hratio_y2);

                        hx1 = hx1 *scale_pressure->data[r][c]/std::max(0.001f,hratio_x1);
                        hx2 = hx2 *scale_pressure->data[r][c]/std::max(0.001f,hratio_x2);
                        hy1 = hy1 *scale_pressure->data[r][c]/std::max(0.001f,hratio_y1);
                        hy2 = hy2 *scale_pressure->data[r][c]/std::max(0.001f,hratio_y2);

                        float wx1 = GetVelocity(9.81,h,hx1,scale_pressure_x1 *dem,scale_pressure_x1 *demx1,_dx,N);
                        float wx2 = GetVelocity(9.81,h,hx2,scale_pressure_x2 *dem,scale_pressure_x2 *demx2,_dx,N);
                        float wy1 = GetVelocity(9.81,h,hy1,scale_pressure_y1 *dem,scale_pressure_y1 *demy1,_dx,N);
                        float wy2 = GetVelocity(9.81,h,hy2,scale_pressure_y2 *dem,scale_pressure_y2 *demy2,_dx,N);

                        float pfac_x1 = std::min(1.0f,std::max(0.0f,std::fabs(h*h -hx1*hx1)/(std::fabs(h*h -hx1*hx1) + scale_pressure_x1 *std::fabs(dem - demx1))));
                        float pfac_x2 = std::min(1.0f,std::max(0.0f,std::fabs(h*h -hx2*hx2)/(std::fabs(h*h -hx2*hx2) + scale_pressure_x2 *std::fabs(dem - demx2))));
                        float pfac_y1 = std::min(1.0f,std::max(0.0f,std::fabs(h*h -hy1*hy1)/(std::fabs(h*h -hy1*hy1) + scale_pressure_y1 * std::fabs(dem - demy1))));
                        float pfac_y2 = std::min(1.0f,std::max(0.0f,std::fabs(h*h -hy2*hy2)/(std::fabs(h*h -hy2*hy2) + scale_pressure_y2 * std::fabs(dem - demy2))));

                        wx1 = wx1 * (pressureforce_fac * pfac_x1 + (1.0-pfac_x1)* 1.0);
                        wx2 = wx2 * (pressureforce_fac * pfac_x2 + (1.0-pfac_x2)* 1.0);
                        wy1 = wy1 * (pressureforce_fac * pfac_y1 + (1.0-pfac_y1)* 1.0);
                        wy2 = wy2 * (pressureforce_fac * pfac_y2 + (1.0-pfac_y2)* 1.0);

                        //dt_req_min = std::min(std::max(1.0f/*progress * 1e-5f + (1.0f-progress) * 0.1f*/,dt_req),dt_req_min);

                        //float V = h > 0? std::sqrt(HL) * std::sqrt(std::fabs(S->data[r][c] + 0.001f))*(1.0f/N) *std::pow((HL*_dx)/(2.0f * HL + _dx),2.0f/3.0f) : 0.0f;

                        float ws_total =std::max(1e-6f,((wx1 > 0.0)? std::fabs(wx1): 0.0f) +
                                                 ((wx2 > 0.0)? std::fabs(wx2): 0.0f) +
                                                 ((wy1 > 0.0)? std::fabs(wy1): 0.0f) +
                                                 ((wy2 > 0.0)? std::fabs(wy2): 0.0f));

                        qx1 = (wx1 > 0.0)? std::fabs((wx1)/ws_total) * q : 0.0;
                        if(demhx1 == demh)
                        {
                            qx1 = 0.0;
                        }
                        qx2 = (wx2 > 0.0)? std::fabs((wx2)/ws_total) * q : 0.0;
                        if(demhx2 == demh)
                        {
                            qx2 = 0.0;
                        }
                        qy1 = (wy1 > 0.0)?  std::fabs((wy1)/ws_total) * q : 0.0;
                        if(demhy1 == demh)
                        {
                            qy1 = 0.0;
                        }
                        qy2 = (wy2 > 0.0)? std::fabs((wy2)/ws_total) * q : 0.0;
                        if(demhy2 == demh)
                        {
                            qy2 = 0.0;
                        }
                        qx1 = std::isnormal(qx1)?qx1:0.0;
                        qx2 = std::isnormal(qx2)?qx2:0.0;
                        qy1 = std::isnormal(qy1)?qy1:0.0;
                        qy2 = std::isnormal(qy2)?qy2:0.0;


                        QX1->Drc = qx1;// < 0.0f? qx :0.0f;
                        QX2->Drc = qx2;// > 0.0f? qx :0.0f;
                        QY1->Drc = qy1;// < 0.0f? qy :0.0f;
                        QY2->Drc = qy2;// > 0.0f? qy :0.0f;

                        /*if(do_stabilize)
                        {
                            QX1->Drc = std::max(0.0f,std::min(0.25f *(demh - demhx1),QX1->Drc));// < 0.0f? qx :0.0f;
                            QX2->Drc = std::max(0.0f,std::min(0.25f *(demh - demhx2),QX2->Drc));// > 0.0f? qx :0.0f;
                            QY1->Drc = std::max(0.0f,std::min(0.25f *(demh - demhy1),QY1->Drc));// < 0.0f? qy :0.0f;
                            QY2->Drc = std::max(0.0f,std::min(0.25f *(demh - demhy2),QY2->Drc));// > 0.0f? qy :0.0f;
                        }*/
                    }
                }
            }

            #pragma omp barrier

            //
            {

                //limit flow to height equilibra, to oppose oscillating behavior
                #pragma omp for collapse(2)
                for(int r = 0; r < DEM->data.nr_rows();r++)
                {
                    for(int c = 0; c < DEM->data.nr_cols();c++)
                    {
                        if(!pcr::isMV(DEM->data[r][c]))
                        {
                                float dem = DEM->Drc;

                                float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                                float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                                float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                                float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                                float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                                dem = dem + pit;

                                if(i + 15 >= iter_max && (!(i == iter_max-1)))
                                {
                                    float h = hscale * H->data[r][c];

                                    float hratio_x1 = !OUTORMV(DEM,r,c-1)? scale_pressure->data[r][c-1]  : 1.0f;
                                    float hratio_x2 = !OUTORMV(DEM,r,c+1)? scale_pressure->data[r][c+1]  : 1.0f;
                                    float hratio_y1 = !OUTORMV(DEM,r-1,c)? scale_pressure->data[r-1][c]  : 1.0f;
                                    float hratio_y2 = !OUTORMV(DEM,r+1,c)? scale_pressure->data[r+1][c]  : 1.0f;

                                    float hx1 = !OUTORMV(DEM,r,c-1)? hscale*H->data[r][c-1] : 0.0f;
                                    float hx2 = !OUTORMV(DEM,r,c+1)? hscale*H->data[r][c+1] : 0.0f;
                                    float hy1 = !OUTORMV(DEM,r-1,c)? hscale*H->data[r-1][c] : 0.0f;
                                    float hy2 = !OUTORMV(DEM,r+1,c)? hscale*H->data[r+1][c] : 0.0f;


                                    float scale_pressure_x1= 0.5 * (scale_pressure->data[r][c] + hratio_x1);
                                    float scale_pressure_x2= 0.5 * (scale_pressure->data[r][c] + hratio_x2);
                                    float scale_pressure_y1= 0.5 * (scale_pressure->data[r][c] + hratio_y1);
                                    float scale_pressure_y2= 0.5 * (scale_pressure->data[r][c] + hratio_y2);

                                    hx1 = hx1 *scale_pressure->data[r][c]/std::max(0.001f,hratio_x1);
                                    hx2 = hx2 *scale_pressure->data[r][c]/std::max(0.001f,hratio_x2);
                                    hy1 = hy1 *scale_pressure->data[r][c]/std::max(0.001f,hratio_y1);
                                    hy2 = hy2 *scale_pressure->data[r][c]/std::max(0.001f,hratio_y2);


                                    float demh = dem + h;
                                    float demx1 = (!OUTORMV(DEM,r,c-1))? DEM->data[r][c-1]: dem;
                                    float demx2 = (!OUTORMV(DEM,r,c+1))? DEM->data[r][c+1] : dem;
                                    float demy1 = (!OUTORMV(DEM,r-1,c))? DEM->data[r-1][c] : dem;
                                    float demy2 = (!OUTORMV(DEM,r+1,c))? DEM->data[r+1][c] : dem;

                                    float qx2_x2 = std::max(0.0f,((!OUTORMV(DEM,r,c+1))? ((QX2->data[r][c+1])) : 0.0f));
                                    float qx1_x1 = std::max(0.0f,((!OUTORMV(DEM,r,c-1))? ((QX1->data[r][c-1])) : 0.0f));
                                    float qy2_y2 = std::max(0.0f,((!OUTORMV(DEM,r+1,c))? ((QY2->data[r+1][c])) : 0.0f));
                                    float qy1_y1 = std::max(0.0f,((!OUTORMV(DEM,r-1,c))? ((QY1->data[r-1][c])) : 0.0f));

                                    QX1->Drc = std::max(0.0f,std::min(0.25f *(scale_pressure_x1 * dem + h - scale_pressure_x1 *demx1 -hx1) + add_next*std::fabs(qx1_x1),QX1->Drc));// < 0.0f? qx :0.0f;
                                    QX2->Drc = std::max(0.0f,std::min(0.25f *(scale_pressure_x2 * dem + h - scale_pressure_x2 *demx2 -hx2) + add_next*std::fabs(qx2_x2),QX2->Drc));// > 0.0f? qx :0.0f;
                                    QY1->Drc = std::max(0.0f,std::min(0.25f *(scale_pressure_y1 * dem + h - scale_pressure_y1 *demy1 -hy1) + add_next*std::fabs(qy1_y1),QY1->Drc));// < 0.0f? qy :0.0f;
                                    QY2->Drc = std::max(0.0f,std::min(0.25f *(scale_pressure_y2 * dem + h - scale_pressure_y2 *demy2 -hy2) + add_next*std::fabs(qy2_y2),QY2->Drc));// > 0.0f? qy :0.0f;

                            }

     }

                    }
                }



            }

            float in_total = 0.0;
            float out_total = 0.0;

            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        if((!(i == iter_max-1)))
                        {
                            float dem = DEM->Drc;

                                float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                                float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                                float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                                float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                                float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                                dem = dem + pit;

                                float demh = dem + hscale*H->data[r][c];
                                float demhx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1]+hscale*H->data[r][c-1] : dem;
                                float demhx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1]+hscale*H->data[r][c+1] : dem;
                                float demhy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c]+hscale*H->data[r-1][c] : dem;
                                float demhy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c]+hscale*H->data[r+1][c] : dem;

                                float demhx11 = !OUTORMV(DEM,r-1,c-1)? DEM->data[r-1][c-1]+hscale*H->data[r-1][c-1] : dem;
                                float demhx22 = !OUTORMV(DEM,r+1,c+1)? DEM->data[r+1][c+1]+hscale*H->data[r+1][c+1] : dem;
                                float demhy11 = !OUTORMV(DEM,r-1,c-1)? DEM->data[r-1][c+1]+hscale*H->data[r-1][c+1] : dem;
                                float demhy22 = !OUTORMV(DEM,r+1,c+1)? DEM->data[r+1][c-1]+hscale*H->data[r+1][c-1] : dem;

                                float demhmin = std::min(std::min(std::min(demhx1,demhx2),demhy1),demhy2);
                                demhmin =std::min(demhmin, std::min(std::min(std::min(demhx11,demhx22),demhy11),demhy22));

                                float qx1 = std::max(0.0f,((!OUTORMV(DEM,r,c-1))? ((QX2->data[r][c-1])) : 0.0f));
                                float qx2 = std::max(0.0f,((!OUTORMV(DEM,r,c+1))? ((QX1->data[r][c+1])) : 0.0f));
                                float qy1 = std::max(0.0f,((!OUTORMV(DEM,r-1,c))? ((QY2->data[r-1][c])) : 0.0f));
                                float qy2 = std::max(0.0f,((!OUTORMV(DEM,r+1,c))? ((QY1->data[r+1][c])) : 0.0f));

                                float qoutx1 = std::max(0.0f,((QX1->data[r][c])));
                                float qoutx2 = std::max(0.0f,((QX2->data[r][c])));
                                float qouty1 = std::max(0.0f,((QY1->data[r][c])));
                                float qouty2 = std::max(0.0f,((QY2->data[r][c])));

                                //total net incoming discharge
                                float Q = std::max(0.0f, (qoutx1 + qoutx2 + qouty1 + qouty2));// - qoutx1 - qoutx2 - qouty1 - qouty2

                                float QIN = std::max(0.0f,(qx1 + qx2 + qy1 + qy2) + FlowSource->data[r][c]);// - qoutx1 - qoutx2 - qouty1 - qouty2

                                in_total += std::max(0.0f, (qoutx1 + qoutx2 + qouty1 + qouty2));
                                out_total += std::max(0.0f,(qx1 + qx2 + qy1 + qy2));

                                //solve for height, so that new discharge is incoming discharge
                                //float sol = std::max(0.0f,(demhmin -DEM->data[r][c] ))+ std::pow( _dx * QIN*  N /(sqrt(S->Drc + 0.001f)),6.0f/5.0f);

                                float sol = QIN / courant_here;

                                float hnew = std::max(std::max(0.0f,demhmin- dem)/hscale,sol);


                                HN->data[r][c] = std::max(0.0f,H->Drc  + QIN* dt - Q* dt );//(H->Drc > 1 || hnew > 1)? std::min(H->Drc * 2.0,std::max(H->Drc*1.0,hnew)) : hnew;
                        }
                     }
                 }
               }

            #pragma omp barrier

        #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                        H->data[r][c] = HN->data[r][c];

                        if(do_forced)
                        {
                            if(!(HForced->data[r][c] < 0.0f))
                            {
                                H->data[r][c] =HForced->data[r][c];
                            }
                        }
                    }
                }
            }
        }
    }


    delete SX;
    delete SY;

    return {H,QX1,QX2,QY1,QY2};

}

//mass-preserving backtracing semi-langrangian advection flow accumulation over velocity-field
inline std::vector<cTMap *> AS_AccumulateFlowAcc(cTMap * DEM, cTMap * UX, cTMap * UY, cTMap * source, int iter_max = 2500)
{

    cTMap * QX1Y1 = DEM->GetCopy0();
    cTMap * QX2Y1 = DEM->GetCopy0();
    cTMap * QX1Y2 = DEM->GetCopy0();
    cTMap * QX2Y2 = DEM->GetCopy0();
    cTMap * QX1 = DEM->GetCopy0();
    cTMap * QY1 = DEM->GetCopy0();
    cTMap * QX2 = DEM->GetCopy0();
    cTMap * QY2 = DEM->GetCopy0();

    cTMap * SN = source->GetCopy();
    cTMap * S = source->GetCopy();
    cTMap * ST = DEM->GetCopy0();
    cTMap * WT = DEM->GetCopy0();

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                //get normalized velocity
                float ux = UX->data[r][c];
                float uy = UY->data[r][c];
                float vel = std::sqrt(ux*ux + uy*uy);
                ux = ux/std::max(1e-9f,vel);
                uy = uy/std::max(1e-9f,vel);

                //get back-tracked location
                int r_other = r + uy > 0.0? 1: -1;
                int c_other = c + ux > 0.0? 1: -1;


                //get four bilinear interpolated weights
                float w_x2y1 = std::fabs(ux) * (1.0-std::fabs(uy));
                float w_x2y2 = std::fabs(ux) * (std::fabs(uy));
                float w_x1y2 = (1.0-std::fabs(ux)) * (std::fabs(uy));
                float w_x1y1 = (1.0-std::fabs(ux)) * (1.0-std::fabs(uy));

                float w_total = std::max(1e-9f,w_x2y1 + w_x2y2 + w_x1y2);

                if(ux > 0.0)
                {
                    if(uy > 0.0)
                    {
                        QX2->data[r][c] = w_x2y1/w_total;
                        QY2->data[r][c] = w_x1y2/w_total;
                        QX2Y2->data[r][c] = w_x2y2/w_total;

                    }else
                    {
                        QX2->data[r][c] = w_x2y1/w_total;
                        QY1->data[r][c] = w_x1y2/w_total;
                        QX2Y1->data[r][c] = w_x2y2/w_total;

                    }
                }else
                {
                    if(uy > 0.0)
                    {
                        QX1->data[r][c] = w_x2y1/w_total;
                        QY2->data[r][c] = w_x1y2/w_total;
                        QX1Y2->data[r][c] = w_x2y2/w_total;

                    }else
                    {
                        QX1->data[r][c] = w_x2y1/w_total;
                        QY1->data[r][c] = w_x1y2/w_total;
                        QX1Y1->data[r][c] = w_x2y2/w_total;

                    }
                }
            }

        }
    }

    int dx[8] = {-1,-1,-1,0,0,1,1,1};
    int dy[8] = {-1,0,1,-1,1,-1,0,1};
    cTMap * ws[8] = {QX1Y1,QX1,QX1Y2,QY1,QY2,QX2Y1, QX2, QX2Y2};
    cTMap * ws2[8] = {QX2Y2,QX2,QX2Y1,QY2,QY1,QX1Y2, QX1, QX1Y1};

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                //get weights that have been assigned to this pixel by all neighbors


                float w_total = 0.0;
                for(int i = 0; i < 8; i++)
                {
                    int r2 = r + dy[i];
                    int c2 = c + dx[i];

                    if(r2 > -1 && r2 < DEM->data.nr_rows())
                    {
                        if(c2 > -1 && c2 < DEM->data.nr_cols())
                        {
                            w_total += ws2[i]->data[r2][c2];
                        }
                    }
                }

                WT->data[r][c] = w_total;

                //The absolute value of weights doesnt matter really,
                //we just want to move all material (total weight = 1) to neighbors
                //as long as we scale all the weights assigned to this pixel equally
                //the finally solved accuflux is equal.

                //if sum of weigths is bigger than 1, we have diverging flow, and we normalize weights
                //if sum of weights is less than 1, we have converging flow, and we also normalize weights

                w_total = std::max(1e-9f,w_total);

                for(int i = 0; i < 8; i++)
                {
                    int r2 = r + dy[i];
                    int c2 = c + dx[i];

                    if(r2 > -1 && r2 < DEM->data.nr_rows())
                    {
                        if(c2 > -1 && c2 < DEM->data.nr_cols())
                        {
                            ws2[i]->data[r2][c2] = ws2[i]->data[r2][c2]/w_total;
                        }
                    }
                }

            }

        }
    }


    bool stop = false;
    bool stop2 = false;
    float hscale = 1.0;

    int i = 0;
    int i2 = 0;
    #pragma omp parallel private(stop,stop2) shared(i)
    {



        #pragma omp barrier



        #pragma omp critical
        {
            i = 0;

        }


        while( true)
        {
            #pragma omp single
            {



                stop = false;
                i = i+1;
            }

            #pragma omp barrier

            bool do_stop = false;

            #pragma omp critical
            {
                i2 = i;
                if(i2>= iter_max)
                {
                    stop = true;
                    do_stop = true;
                }
                //dt_req_min = 1e6;

            }

            #pragma omp barrier

            if(do_stop)
            {
                break;
            }


            float progress = ((float)(i))/std::max(1.0f,((float)(iter_max)));


            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    //Get Q Total

                    if(!pcr::isMV(DEM->data[r][c]))
                    {

                        float qin_tot = 0.0;

                        for(int i = 0; i < 8; i++)
                        {
                            int r2 = r + dy[i];
                            int c2 = c + dx[i];

                            if(r2 > -1 && r2 < DEM->data.nr_rows())
                            {
                                if(c2 > -1 && c2 < DEM->data.nr_cols())
                                {
                                    qin_tot += ws[i]->data[r][c] * S->data[r2][c2];
                                }
                            }
                        }

                        SN->data[r][c] = qin_tot;
                        ST->data[r][c] += qin_tot;

                    }
                }
            }


            #pragma omp barrier

            #pragma omp for collapse(2)
            for(int r = 0; r < DEM->data.nr_rows();r++)
            {
                for(int c = 0; c < DEM->data.nr_cols();c++)
                {
                    if(!pcr::isMV(DEM->data[r][c]))
                    {
                       S->data[r][c] = SN->data[r][c];
                    }
                }
            }
            #pragma omp barrier

        }
    }

    delete QX1Y1;
    delete QX2Y1;
    delete QX1Y2;
    delete QX2Y2;
    delete QX1;
    delete QY1;
    delete QX2;
    delete QY2;

    delete SN;

    return {ST,WT,S};

}

inline cTMap * AS_Accumulate2DTReach(cTMap * QTX1, cTMap * QTX2, cTMap * QTY1, cTMap * QTY2, cTMap * H, cTMap * Reach, int iter_max)
{
    if(!(H->data.nr_rows() ==  QTX1->data.nr_rows() && H->data.nr_cols() ==  QTX1->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match for Accumulate2D");
        throw -1;
    }
    if(!(H->data.nr_rows() ==  QTX2->data.nr_rows() && H->data.nr_cols() ==  QTX2->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match for Accumulate2D");
        throw -1;
    }
    if(!(H->data.nr_rows() ==  QTY1->data.nr_rows() && H->data.nr_cols() ==  QTY1->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match for Accumulate2D");
        throw -1;
    }
    if(!(H->data.nr_rows() ==  QTY2->data.nr_rows() && H->data.nr_cols() ==  QTY2->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match for Accumulate2D");
        throw -1;
    }
    if(iter_max < 1)
    {
        LISEMS_ERROR("Can not run property spread with fewer than 1 iterations");
        throw -1;
    }

    cTMap * HA = H->GetCopy();
    cTMap * S = H->GetCopy0();
    cTMap * SI = H->GetCopy0();
cTMap * QAX1 = H->GetCopy0();
cTMap * QAX2 = H->GetCopy0();
cTMap * QAY1 = H->GetCopy0();
cTMap * QAY2 = H->GetCopy0();

    //initialize the map with friction values

    bool stop = false;

    int i = 0;
    int i2 = 0;

    #pragma omp parallel private(stop) shared(i)
    {
        double _dx = std::fabs(H->cellSizeX());




        #pragma omp critical
        {
            i = 0;

        }


        while( true)
        {
            #pragma omp single
            {

                stop = false;
                i = i+1;
                std::cout << "I is " << i << std::endl;
            }

            #pragma omp barrier

            bool do_stop = false;

            #pragma omp critical
            {
                i2 = i;
                if(i2>= iter_max)
                {
                    stop = true;
                    do_stop = true;
                }
            }

            #pragma omp barrier

            if(do_stop)
            {
                break;
            }


            float progress = ((float)(i))/std::max(1.0f,((float)(iter_max)));


            #pragma omp for collapse(2)
            for(int r = 0; r < H->data.nr_rows();r++)
            {
                for(int c = 0; c < H->data.nr_cols();c++)
                {
                    //Get Q Total

                    if(!pcr::isMV(H->data[r][c]))
                    {

                        float qxa1 = (QTX1->data[r][c]);
                        float qya1 = (QTY1->data[r][c]);

                        float qxa2 = (QTX2->data[r][c]);
                        float qya2 = (QTY2->data[r][c]);

                        float q_total = std::max(1e-12f,std::max(0.0f,qxa1) +std::max(0.0f,qxa2)+std::max(0.0f,qya1)+std::max(0.0f,qya2));

                        QAX1->data[r][c] = HA->data[r][c] * std::max(0.0f,qxa1)/q_total;
                        QAX2->data[r][c] = HA->data[r][c] * std::max(0.0f,qxa2)/q_total;
                        QAY1->data[r][c] = HA->data[r][c] * std::max(0.0f,qya1)/q_total;
                        QAY2->data[r][c] = HA->data[r][c] * std::max(0.0f,qya2)/q_total;


                    }
                }
            }


            #pragma omp barrier

            #pragma omp for collapse(2)
            for(int r = 0; r < H->data.nr_rows();r++)
            {
                for(int c = 0; c < H->data.nr_cols();c++)
                {
                    if(!pcr::isMV(H->data[r][c]))
                    {
                        float QTt = 0.0;
                        if(!OUTORMV(H,r,c+1))
                        {
                            if(QAX1->data[r][c+1] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAX1->data[r][c+1]);
                            }
                        }
                        if(!OUTORMV(H,r,c-1))
                        {
                            if(QAX2->data[r][c-1] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAX2->data[r][c-1]);
                            }
                        }
                        if(!OUTORMV(H,r+1,c))
                        {
                            if(QAY1->data[r+1][c] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAY1->data[r+1][c]);
                            }
                        }
                        if(!OUTORMV(H,r-1,c))
                        {
                            if(QAY2->data[r-1][c] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAY2->data[r-1][c]);
                            }
                        }

                        if(S->data[r][c] < Reach->data[r][c] && ((S->data[r][c] + QTt) >= Reach->data[r][c]))
                        {
                            SI->data[r][c] += i;
                        }
                        S->data[r][c] += QTt;
                        HA->data[r][c] = QTt;
                    }
                }
            }


            #pragma omp barrier

        }

        #pragma omp barrier

    }



    delete QAX1;
    delete QAX2;
    delete QAY1;
    delete QAY2;
    delete HA;
    delete S;
    return SI;
}

inline std::vector<cTMap *> AS_Accumulate2DT(cTMap * QTX1, cTMap * QTX2, cTMap * QTY1, cTMap * QTY2, cTMap * H, int iter_max)
{
    if(!(H->data.nr_rows() ==  QTX1->data.nr_rows() && H->data.nr_cols() ==  QTX1->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match for Accumulate2D");
        throw -1;
    }
    if(!(H->data.nr_rows() ==  QTX2->data.nr_rows() && H->data.nr_cols() ==  QTX2->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match for Accumulate2D");
        throw -1;
    }
    if(!(H->data.nr_rows() ==  QTY1->data.nr_rows() && H->data.nr_cols() ==  QTY1->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match for Accumulate2D");
        throw -1;
    }
    if(!(H->data.nr_rows() ==  QTY2->data.nr_rows() && H->data.nr_cols() ==  QTY2->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match for Accumulate2D");
        throw -1;
    }
    if(iter_max < 1)
    {
        LISEMS_ERROR("Can not run property spread with fewer than 1 iterations");
        throw -1;
    }

    cTMap * HA = H->GetCopy();
    cTMap * S = H->GetCopy0();
    cTMap * SI = H->GetCopy0();
cTMap * QAX1 = H->GetCopy0();
cTMap * QAX2 = H->GetCopy0();
cTMap * QAY1 = H->GetCopy0();
cTMap * QAY2 = H->GetCopy0();

    //initialize the map with friction values

    bool stop = false;

    int i = 0;
    int i2 = 0;

    #pragma omp parallel private(stop) shared(i)
    {
        double _dx = std::fabs(H->cellSizeX());




        #pragma omp critical
        {
            i = 0;

        }


        while( true)
        {
            #pragma omp single
            {

                stop = false;
                i = i+1;
                std::cout << "I is " << i << std::endl;
            }

            #pragma omp barrier

            bool do_stop = false;

            #pragma omp critical
            {
                i2 = i;
                if(i2>= iter_max)
                {
                    stop = true;
                    do_stop = true;
                }
            }

            #pragma omp barrier

            if(do_stop)
            {
                break;
            }


            float progress = ((float)(i))/std::max(1.0f,((float)(iter_max)));


            #pragma omp for collapse(2)
            for(int r = 0; r < H->data.nr_rows();r++)
            {
                for(int c = 0; c < H->data.nr_cols();c++)
                {
                    //Get Q Total

                    if(!pcr::isMV(H->data[r][c]))
                    {

                        float qxa1 = (QTX1->data[r][c]);
                        float qya1 = (QTY1->data[r][c]);

                        float qxa2 = (QTX2->data[r][c]);
                        float qya2 = (QTY2->data[r][c]);

                        float q_total = std::max(1e-12f,std::max(0.0f,qxa1) +std::max(0.0f,qxa2)+std::max(0.0f,qya1)+std::max(0.0f,qya2));

                        QAX1->data[r][c] = HA->data[r][c] * std::max(0.0f,qxa1)/q_total;
                        QAX2->data[r][c] = HA->data[r][c] * std::max(0.0f,qxa2)/q_total;
                        QAY1->data[r][c] = HA->data[r][c] * std::max(0.0f,qya1)/q_total;
                        QAY2->data[r][c] = HA->data[r][c] * std::max(0.0f,qya2)/q_total;


                    }
                }
            }


            #pragma omp barrier

            #pragma omp for collapse(2)
            for(int r = 0; r < H->data.nr_rows();r++)
            {
                for(int c = 0; c < H->data.nr_cols();c++)
                {
                    if(!pcr::isMV(H->data[r][c]))
                    {
                        float QTt = 0.0;
                        if(!OUTORMV(H,r,c+1))
                        {
                            if(QAX1->data[r][c+1] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAX1->data[r][c+1]);
                            }
                        }
                        if(!OUTORMV(H,r,c-1))
                        {
                            if(QAX2->data[r][c-1] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAX2->data[r][c-1]);
                            }
                        }
                        if(!OUTORMV(H,r+1,c))
                        {
                            if(QAY1->data[r+1][c] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAY1->data[r+1][c]);
                            }
                        }
                        if(!OUTORMV(H,r-1,c))
                        {
                            if(QAY2->data[r-1][c] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAY2->data[r-1][c]);
                            }
                        }

                        SI->data[r][c] += QTt*progress;
                        S->data[r][c] += QTt;
                        HA->data[r][c] = QTt;
                    }
                }
            }



            #pragma omp barrier

        }

        #pragma omp barrier

    }

        #pragma omp for collapse(2)
        for(int r = 0; r < S->data.nr_rows();r++)
        {
            for(int c = 0; c < S->data.nr_cols();c++)
            {
                if(!pcr::isMV(S->data[r][c]))
                {
                    SI->data[r][c] = SI->data[r][c]/std::max(1e-10f,S->data[r][c]);
                }
            }
        }

    delete QAX1;
    delete QAX2;
    delete QAY1;
    delete QAY2;
    delete HA;
    return {S,SI};
}

inline cTMap * AS_Accumulate2DM(cTMap * QTX1, cTMap * QTX2, cTMap * QTY1, cTMap * QTY2, cTMap * H, int iter_max)
{
    if(!(H->data.nr_rows() ==  QTX1->data.nr_rows() && H->data.nr_cols() ==  QTX1->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match for Accumulate2D");
        throw -1;
    }
    if(!(H->data.nr_rows() ==  QTX2->data.nr_rows() && H->data.nr_cols() ==  QTX2->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match for Accumulate2D");
        throw -1;
    }
    if(!(H->data.nr_rows() ==  QTY1->data.nr_rows() && H->data.nr_cols() ==  QTY1->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match for Accumulate2D");
        throw -1;
    }
    if(!(H->data.nr_rows() ==  QTY2->data.nr_rows() && H->data.nr_cols() ==  QTY2->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match for Accumulate2D");
        throw -1;
    }
    if(iter_max < 1)
    {
        LISEMS_ERROR("Can not run property spread with fewer than 1 iterations");
        throw -1;
    }

    cTMap * HA = H->GetCopy();
    cTMap * S = H->GetCopy0();
    cTMap * SI = H->GetCopy0();
cTMap * QAX1 = H->GetCopy0();
cTMap * QAX2 = H->GetCopy0();
cTMap * QAY1 = H->GetCopy0();
cTMap * QAY2 = H->GetCopy0();

    //initialize the map with friction values

    bool stop = false;

    int i = 0;
    int i2 = 0;

    #pragma omp parallel private(stop) shared(i)
    {
        double _dx = std::fabs(H->cellSizeX());




        #pragma omp critical
        {
            i = 0;

        }


        while( true)
        {
            #pragma omp single
            {

                stop = false;
                i = i+1;
                std::cout << "I is " << i << std::endl;
            }

            #pragma omp barrier

            bool do_stop = false;

            #pragma omp critical
            {
                i2 = i;
                if(i2>= iter_max)
                {
                    stop = true;
                    do_stop = true;
                }
            }

            #pragma omp barrier

            if(do_stop)
            {
                break;
            }


            float progress = ((float)(i))/std::max(1.0f,((float)(iter_max)));


            #pragma omp for collapse(2)
            for(int r = 0; r < H->data.nr_rows();r++)
            {
                for(int c = 0; c < H->data.nr_cols();c++)
                {
                    //Get Q Total

                    if(!pcr::isMV(H->data[r][c]))
                    {

                        float qxa1 = (QTX1->data[r][c]);
                        float qya1 = (QTY1->data[r][c]);

                        float qxa2 = (QTX2->data[r][c]);
                        float qya2 = (QTY2->data[r][c]);

                        float q_total = std::max(1e-12f,std::max(0.0f,qxa1) +std::max(0.0f,qxa2)+std::max(0.0f,qya1)+std::max(0.0f,qya2));

                        QAX1->data[r][c] = HA->data[r][c] * std::max(0.0f,qxa1)/q_total;
                        QAX2->data[r][c] = HA->data[r][c] * std::max(0.0f,qxa2)/q_total;
                        QAY1->data[r][c] = HA->data[r][c] * std::max(0.0f,qya1)/q_total;
                        QAY2->data[r][c] = HA->data[r][c] * std::max(0.0f,qya2)/q_total;


                    }
                }
            }


            #pragma omp barrier

            #pragma omp for collapse(2)
            for(int r = 0; r < H->data.nr_rows();r++)
            {
                for(int c = 0; c < H->data.nr_cols();c++)
                {
                    if(!pcr::isMV(H->data[r][c]))
                    {
                        float QTt = 0.0;
                        if(!OUTORMV(H,r,c+1))
                        {
                            if(QAX1->data[r][c+1] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAX1->data[r][c+1]);
                            }
                        }
                        if(!OUTORMV(H,r,c-1))
                        {
                            if(QAX2->data[r][c-1] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAX2->data[r][c-1]);
                            }
                        }
                        if(!OUTORMV(H,r+1,c))
                        {
                            if(QAY1->data[r+1][c] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAY1->data[r+1][c]);
                            }
                        }
                        if(!OUTORMV(H,r-1,c))
                        {
                            if(QAY2->data[r-1][c] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAY2->data[r-1][c]);
                            }
                        }

                        SI->data[r][c] += std::max(SI->data[r][c],QTt);
                        S->data[r][c] += QTt;
                        HA->data[r][c] = QTt;
                    }
                }
            }



            #pragma omp barrier

        }

        #pragma omp barrier

    }



    delete QAX1;
    delete QAX2;
    delete QAY1;
    delete QAY2;
    delete HA;
    delete S;
    return SI;
}


inline cTMap * AS_Accumulate2D(cTMap * QTX1, cTMap * QTX2, cTMap * QTY1, cTMap * QTY2, cTMap * H, int iter_max)
{
    if(!(H->data.nr_rows() ==  QTX1->data.nr_rows() && H->data.nr_cols() ==  QTX1->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match for Accumulate2D");
        throw -1;
    }
    if(!(H->data.nr_rows() ==  QTX2->data.nr_rows() && H->data.nr_cols() ==  QTX2->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match for Accumulate2D");
        throw -1;
    }
    if(!(H->data.nr_rows() ==  QTY1->data.nr_rows() && H->data.nr_cols() ==  QTY1->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match for Accumulate2D");
        throw -1;
    }
    if(!(H->data.nr_rows() ==  QTY2->data.nr_rows() && H->data.nr_cols() ==  QTY2->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match for Accumulate2D");
        throw -1;
    }
    if(iter_max < 1)
    {
        LISEMS_ERROR("Can not run property spread with fewer than 1 iterations");
        throw -1;
    }

    cTMap * HA = H->GetCopy();
    cTMap * S = H->GetCopy0();
cTMap * QAX1 = H->GetCopy0();
cTMap * QAX2 = H->GetCopy0();
cTMap * QAY1 = H->GetCopy0();
cTMap * QAY2 = H->GetCopy0();

    //initialize the map with friction values

    bool stop = false;

    int i = 0;
    int i2 = 0;

    #pragma omp parallel private(stop) shared(i)
    {
        double _dx = std::fabs(H->cellSizeX());




        #pragma omp critical
        {
            i = 0;

        }


        while( true)
        {
            #pragma omp single
            {

                stop = false;
                i = i+1;
                std::cout << "I is " << i << std::endl;
            }

            #pragma omp barrier

            bool do_stop = false;

            #pragma omp critical
            {
                i2 = i;
                if(i2>= iter_max)
                {
                    stop = true;
                    do_stop = true;
                }
            }

            #pragma omp barrier

            if(do_stop)
            {
                break;
            }


            float progress = ((float)(i))/std::max(1.0f,((float)(iter_max)));


            #pragma omp for collapse(2)
            for(int r = 0; r < H->data.nr_rows();r++)
            {
                for(int c = 0; c < H->data.nr_cols();c++)
                {
                    //Get Q Total

                    if(!pcr::isMV(H->data[r][c]))
                    {

                        float qxa1 = (QTX1->data[r][c]);
                        float qya1 = (QTY1->data[r][c]);

                        float qxa2 = (QTX2->data[r][c]);
                        float qya2 = (QTY2->data[r][c]);

                        float q_total = std::max(1e-12f,std::max(0.0f,qxa1) +std::max(0.0f,qxa2)+std::max(0.0f,qya1)+std::max(0.0f,qya2));

                        QAX1->data[r][c] = HA->data[r][c] * std::max(0.0f,qxa1)/q_total;
                        QAX2->data[r][c] = HA->data[r][c] * std::max(0.0f,qxa2)/q_total;
                        QAY1->data[r][c] = HA->data[r][c] * std::max(0.0f,qya1)/q_total;
                        QAY2->data[r][c] = HA->data[r][c] * std::max(0.0f,qya2)/q_total;


                    }
                }
            }


            #pragma omp barrier

            #pragma omp for collapse(2)
            for(int r = 0; r < H->data.nr_rows();r++)
            {
                for(int c = 0; c < H->data.nr_cols();c++)
                {
                    if(!pcr::isMV(H->data[r][c]))
                    {
                        float QTt = 0.0;
                        if(!OUTORMV(H,r,c+1))
                        {
                            if(QAX1->data[r][c+1] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAX1->data[r][c+1]);
                            }
                        }
                        if(!OUTORMV(H,r,c-1))
                        {
                            if(QAX2->data[r][c-1] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAX2->data[r][c-1]);
                            }
                        }
                        if(!OUTORMV(H,r+1,c))
                        {
                            if(QAY1->data[r+1][c] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAY1->data[r+1][c]);
                            }
                        }
                        if(!OUTORMV(H,r-1,c))
                        {
                            if(QAY2->data[r-1][c] > 0.0f)
                            {
                                QTt += std::max(0.0f,QAY2->data[r-1][c]);
                            }
                        }

                        S->data[r][c] += QTt;
                        HA->data[r][c] = QTt;
                    }
                }
            }



            #pragma omp barrier

        }

        #pragma omp barrier

    }


    delete QAX1;
    delete QAX2;
    delete QAY1;
    delete QAY2;
    delete HA;
    return S;
}

inline std::vector<cTMap *> AS_FlowPropertySpread(cTMap * HI ,cTMap * Prop, cTMap * DEM, cTMap * SlopeAdd, float fac_adapt, int iter_max)
{

    if(!(HI->data.nr_rows() ==  Prop->data.nr_rows() && HI->data.nr_cols() ==  Prop->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }
    if(!(HI->data.nr_rows() ==  DEM->data.nr_rows() && HI->data.nr_cols() ==  DEM->data.nr_cols()))
    {
        LISEMS_ERROR("Numbers of rows and column do not match");
        throw -1;
    }
    if(iter_max < 1)
    {
        LISEMS_ERROR("Can not run property spread with fewer than 1 iterations");
        throw -1;
    }

    cTMap * H = HI->GetCopy();
    cTMap * ROrg = HI->GetCopy0();
    cTMap * COrg = HI->GetCopy0();
    cTMap * PropN = Prop->GetCopy();



    #pragma omp parallel for collapse(2)
    for(int r = 0; r < H->data.nr_rows();r++)
    {
        for(int c = 0; c < H->data.nr_cols();c++)
        {
            if(!pcr::isMV(H->data[r][c]))
            {
                ROrg->data[r][c] = r;
                COrg->data[r][c] = c;
            }
        }
    }


    //initialize the map with friction values

    bool stop = false;

    int i = 0;
    int i2 = 0;

    #pragma omp parallel private(stop) shared(i)
    {
        double _dx = std::fabs(H->cellSizeX());




        #pragma omp critical
        {
            i = 0;

        }


        while( true)
        {
            #pragma omp single
            {

                stop = false;
                i = i+1;
                std::cout << "I is " << i << std::endl;
            }

            #pragma omp barrier

            bool do_stop = false;

            #pragma omp critical
            {
                i2 = i;
                if(i2>= iter_max)
                {
                    stop = true;
                    do_stop = true;
                }
            }

            #pragma omp barrier

            if(do_stop)
            {
                break;
            }


            float progress = ((float)(i))/std::max(1.0f,((float)(iter_max)));


            #pragma omp for collapse(1)
            for(int r = 0; r < H->data.nr_rows();r++)
            {
                for(int c = H->data.nr_cols()-1; c > 0;c--)
                {
                    if(!pcr::isMV(H->data[r][c]))
                    {
                        if(!OUTORMV(H,r,c-1))
                        {
                            float Sthis = SlopeAdd->data[r][c];
                            float Snext = SlopeAdd->data[r][c-1];
                            float Zthis = DEM->data[r][c];
                            float Znext = DEM->data[r][c-1];
                            float Hthis = H->data[r][c] - std::fabs(Znext-Zthis) - 0.5f * (Sthis + Snext);
                            float Hnext = H->data[r][c-1];
                            if(Hthis > Hnext)
                            {
                                H->data[r][c-1] = Hthis*(1.0-fac_adapt) +Hnext * fac_adapt;
                                PropN->data[r][c-1] = PropN->data[r][c]*(1.0-fac_adapt) + PropN->data[r][c-1] * fac_adapt;
                                ROrg->data[r][c-1] = ROrg->data[r][c];
                                COrg->data[r][c-1] = COrg->data[r][c];
                            }
                        }

                    }
                }
            }

            #pragma omp for collapse(1)
            for(int r = 0; r < H->data.nr_rows();r++)
            {
                for(int c = 0; c < H->data.nr_cols()-1;c++)
                {
                    if(!pcr::isMV(H->data[r][c]))
                    {
                        if(!OUTORMV(H,r,c+1))
                        {
                            float Sthis = SlopeAdd->data[r][c];
                            float Snext = SlopeAdd->data[r][c+1];
                            float Zthis = DEM->data[r][c];
                            float Znext = DEM->data[r][c+1];
                            float Hthis = H->data[r][c] - std::fabs(Znext-Zthis)- 0.5f * (Sthis + Snext);
                            float Hnext = H->data[r][c+1];
                            if(Hthis > Hnext)
                            {
                                H->data[r][c+1] = Hthis*(1.0-fac_adapt) + Hnext * fac_adapt;
                                PropN->data[r][c+1] = PropN->data[r][c]*(1.0-fac_adapt) + PropN->data[r][c+1] * fac_adapt;
                                ROrg->data[r][c+1] = ROrg->data[r][c];
                                COrg->data[r][c+1] = COrg->data[r][c];
                            }
                        }
                    }
                }
            }

            #pragma omp for collapse(1)
            for(int c = 0; c < H->data.nr_cols();c++)
            {
                for(int r = H->data.nr_rows()-1; r > 0;r--)
                {
                    if(!pcr::isMV(H->data[r][c]))
                    {
                        if(!OUTORMV(H,r-1,c))
                        {
                            float Sthis = SlopeAdd->data[r][c];
                            float Snext = SlopeAdd->data[r-1][c];
                            float Zthis = DEM->data[r][c];
                            float Znext = DEM->data[r-1][c];
                            float Hthis = H->data[r][c] - std::fabs(Znext-Zthis)- 0.5f * (Sthis + Snext);
                            float Hnext = H->data[r-1][c];
                            if(Hthis > Hnext)
                            {
                                H->data[r-1][c] = Hthis*(1.0-fac_adapt) + Hnext * fac_adapt;
                                PropN->data[r-1][c] = PropN->data[r][c]*(1.0-fac_adapt) + PropN->data[r-1][c] * fac_adapt;
                                ROrg->data[r-1][c] = ROrg->data[r][c];
                                COrg->data[r-1][c] = COrg->data[r][c];
                            }
                        }
                    }
                }
            }


            #pragma omp for collapse(1)
            for(int c = 0; c < H->data.nr_cols();c++)
            {
                for(int r = 0; r < H->data.nr_rows()-1;r++)
                {
                    if(!pcr::isMV(H->data[r][c]))
                    {
                        if(!OUTORMV(H,r+1,c))
                        {
                            float Sthis = SlopeAdd->data[r][c];
                            float Snext = SlopeAdd->data[r+1][c];
                            float Zthis = DEM->data[r][c];
                            float Znext = DEM->data[r+1][c];
                            float Hthis = H->data[r][c] - std::fabs(Znext-Zthis)- 0.5f * (Sthis + Snext);
                            float Hnext = H->data[r+1][c];
                            if(Hthis > Hnext)
                            {
                                H->data[r+1][c] = Hthis*(1.0-fac_adapt) + Hnext * fac_adapt;
                                PropN->data[r+1][c] = PropN->data[r][c]*(1.0-fac_adapt) + PropN->data[r+1][c] * fac_adapt;
                                ROrg->data[r+1][c] = ROrg->data[r][c];
                                COrg->data[r+1][c] = COrg->data[r][c];
                            }
                        }
                    }
                }
            }


            #pragma omp barrier

            /*#pragma omp for collapse(2)
            for(int r = 0; r < data->data.nr_rows();r++)
            {
                for(int c = 0; c < data->data.nr_cols();c++)
                {
                    if(!pcr::isMV(data->data[r][c]))
                    {
                        data->data[r][c] =dataN->data[r][c];
                    }
                }
            }*/
        }

        #pragma omp barrier

    }

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < H->data.nr_rows();r++)
    {
        for(int c = 0; c < H->data.nr_cols();c++)
        {
            if(!pcr::isMV(H->data[r][c]))
            {
                ROrg->data[r][c] = std::sqrt((ROrg->data[r][c]  - r) *  (ROrg->data[r][c]  - r) +  (COrg->data[r][c]  - c) *  (COrg->data[r][c]  - c));
            }
        }
    }

    delete COrg;
    return {PropN,H,ROrg};
}

inline float AS_GetTotalRainfall(std::vector<float> time, std::vector<float> rain, float r)
{
    if(time.size() != rain.size())
    {

        LISEMS_ERROR("Time and rainfall records are not of same size.");
        throw 1;

    }
    float total = 0.0;
    for(int i =0; i < time.size(); i++)
    {
        total += time.at(i) * rain.at(i);
    }
    return r;
}

inline float GetSSTotalRainfall(std::vector<float> &time, std::vector<float> &rain, float r)
{
    if(time.size() != rain.size())
    {

        LISEMS_ERROR("Time and rainfall records are not of same size.");
        throw 1;

    }
    float total = 0.0;
    float totaltemp = 0.0;
    bool started=  false;
    for(int i =0; i < time.size(); i++)
    {
        if(rain.at(i) >= r)
        {
            started = true;
            total += totaltemp;
            total += time.at(i) * rain.at(i);
            totaltemp = 0.0;
        }else
        {
            if(started == true)
            {
                totaltemp += time.at(i) * rain.at(i);
            }
        }
    }
    return total;
}


inline float GetDurationFromRainfall(std::vector<float> &time, std::vector<float> &rain, float r)
{
    if(time.size() != rain.size())
    {

        LISEMS_ERROR("Time and rainfall records are not of same size.");
        throw 1;

    }
    float tmin = 0.0;
    float tmax = 0.0;
    float tmin_set = false;
    float tmax_set = false;
    for(int i =0; i < time.size(); i++)
    {
        if(tmin_set == false && i == 0)
        {
            tmin = time.at(i);
        }

        if(rain.at(i) >= r)
        {
            tmin = time.at(i);
            break;
        }
    }
    tmax = tmin;
    for(int i =time.size()-1; i > -1 ; i--)
    {
        if(tmax_set == false && i == 0)
        {
            tmax = time.at(i);
        }

        if(rain.at(i) >= r)
        {
            tmax = time.at(i);
            break;
        }
    }
    return tmax - tmin;
}

inline float AS_GetDurationFromRainfall(std::vector<float> time, std::vector<float> rain, float rainval)
{
    return GetDurationFromRainfall(time,rain,rainval);
}


inline float AS_GetSSDurationFromRainfall(std::vector<float> time, std::vector<float> rain, float rainval)
{


    return GetDurationFromRainfall(time,rain,rainval);
}
inline float AS_GetSSRainfallMaxVol(std::vector<float> time, std::vector<float> rain)
{

    if(time.size() != rain.size())
    {

        LISEMS_ERROR("Time and rainfall records are not of same size.");
        throw 1;

    }
    //get total rainfall

    float rain_total = 0.0;
    float rain_max = 0;

    for(int i = 0; i < rain.size(); i++)
    {
        rain_max = std::max(rain_max,rain.at(i));
    }

    float rain_best = rain_max * 0.5;

    //
    float rain_previous = 0.0;
    float rain_current = rain_max;
    float raint_previous = 0.0;
    float raint_current = GetSSTotalRainfall(time,rain,rain_max);

    for(int i = 0; i < 30; i++)
    {
        //try a value in-between rain_previous and rain_current
        //it replaces the worst of the two
        //assumption is that it is at least better than one of the two guesses
        //if it is equal, pick the higher half of the current range

        float rain_this = 0.5 * (rain_current + rain_previous);
        float raint_this = GetSSTotalRainfall(time,rain,rain_this);

        std::cout << "try rain value " << rain_this << " " << raint_this << std::endl;
        bool set = false;
        if(raint_previous < raint_current)
        {
            if(raint_this > raint_previous)
            {
                rain_previous = rain_this;
                raint_previous = raint_this;
                set = true;
            }

        }else
        {
            if(raint_this > raint_current)
            {
                rain_current = rain_this;
                raint_current = raint_this;
                set = true;

            }
        }

        if(set == false)
        {
            std::cout << "rain not set " << std::endl;

            //if we are stuck with convergence,
            //see if we are already pretty far along
            //otherwise we move towards the higher half

            if( i> 10 || (std::fabs(rain_current - rain_previous) < 0.01))
            {
                break;
            }else
            {
                rain_previous = rain_this;
                raint_previous = raint_this;
                set = true;

            }

        }

    }
    return 0.5 * (rain_current + rain_previous);
}

inline cTMap* AS_FloodFill(cTMap * DEM, cTMap * HBoundary, int iter_max, bool subtract_slope)
{
    cTMap * H = HBoundary->GetCopy();
    cTMap * HN = HBoundary->GetCopy();

    float _dx = std::fabs(H->cellSizeX());

    //initialize the map with friction values

    bool stop = false;

    int i = 0;
    int i2 = 0;

    #pragma omp parallel private(stop) shared(i)
    {

        #pragma omp critical
        {
            i = 0;

        }


        while( true)
        {
            #pragma omp single
            {

                stop = false;
                i = i+1;
            }

            #pragma omp barrier

            bool do_stop = false;

            #pragma omp critical
            {
                i2 = i;
                if(i2>= iter_max)
                {
                    stop = true;
                    do_stop = true;
                }
            }

            #pragma omp barrier

            if(do_stop)
            {
                break;
            }


            float progress = ((float)(i))/std::max(1.0f,((float)(iter_max)));

            #pragma omp for collapse(2)
            for(int c = 0; c < H->data.nr_cols();c++)
            {
                for(int r = 0; r < H->data.nr_rows();r++)
                {
                    if(!pcr::isMV(H->data[r][c]))
                    {

                        //get the new height we would get from a neighbor, check if its bigger than current height, replace if so

                        float hnew_x1 = H->data[r][c];
                        float hnew_x2 = H->data[r][c];
                        float hnew_y1 = H->data[r][c];
                        float hnew_y2 = H->data[r][c];

                        if(!subtract_slope)
                        {
                            if(!OUTORMV(H,r,c-1))
                            {
                                hnew_x1 = std::max(0.0f,(H->data[r][c-1] + DEM->data[r][c-1]) -  DEM->data[r][c]);
                            }
                            if(!OUTORMV(H,r,c-1))
                            {
                                hnew_x2 = std::max(0.0f,(H->data[r][c+1] + DEM->data[r][c+1]) -  DEM->data[r][c]);
                            }
                            if(!OUTORMV(H,r,c-1))
                            {
                                hnew_y1 = std::max(0.0f,(H->data[r-1][c] + DEM->data[r-1][c]) -  DEM->data[r][c]);
                            }
                            if(!OUTORMV(H,r,c-1))
                            {
                                hnew_y2 = std::max(0.0f,(H->data[r+1][c] + DEM->data[r+1][c]) -  DEM->data[r][c]);
                            }
                        }else
                        {
                            if(!OUTORMV(H,r,c-1))
                            {
                                hnew_x1 = std::max(0.0f,(H->data[r][c-1] + DEM->data[r][c-1]) -  DEM->data[r][c]) - std::max(0.0f, DEM->data[r][c-1] - DEM->data[r][c]);
                            }
                            if(!OUTORMV(H,r,c-1))
                            {
                                hnew_x2 = std::max(0.0f,(H->data[r][c+1] + DEM->data[r][c+1]) -  DEM->data[r][c]) - std::max(0.0f, DEM->data[r][c+1] - DEM->data[r][c]);
                            }
                            if(!OUTORMV(H,r,c-1))
                            {
                                hnew_y1 = std::max(0.0f,(H->data[r-1][c] + DEM->data[r-1][c]) -  DEM->data[r][c]) - std::max(0.0f, DEM->data[r-1][c] - DEM->data[r][c]);
                            }
                            if(!OUTORMV(H,r,c-1))
                            {
                                hnew_y2 = std::max(0.0f,(H->data[r+1][c] + DEM->data[r+1][c]) -  DEM->data[r][c]) - std::max(0.0f, DEM->data[r+1][c] - DEM->data[r][c]);
                            }
                        }

                        float hnew = std::max(hnew_x1,std::max(hnew_x2,std::max(hnew_y1,hnew_y2)));
                        HN->data[r][c] = std::max(H->data[r][c],hnew);
                    }
                }
            }


            #pragma omp barrier

            #pragma omp for collapse(2)
            for(int r = 0; r < H->data.nr_rows();r++)
            {
                for(int c = 0; c < H->data.nr_cols();c++)
                {
                    if(!pcr::isMV(H->data[r][c]))
                    {
                        H->data[r][c] = HN->data[r][c];
                    }
                }
            }
        }

        #pragma omp barrier

    }

    delete HN;

    return H;


}

//fill in pressure-based indundation areas based on a height estimate from inverted flow accumulation
inline cTMap * AS_FlowReconstruct(cTMap * DEM, cTMap * HSS, cTMap  *Hinv, cTMap * DX, cTMap * DY, int iter_max)
{

    //assumption: flow is in direction of water surface gradient (and proportional to)

    //we have a water surface slope map from the steady-state solution (Slope(DEM + HSS))
    //the slopes of the inverted accuflux solution are non-conform due to lack of pressure-driven flow directionality
    //here we can reconstruct a pressure-conform flow height field by using the surface slopes from the steady state map to expand the new height field

    //
    //
    //
    //

    cTMap * H = Hinv->GetCopy();
    cTMap * HN = Hinv->GetCopy();

    float _dx = std::fabs(H->cellSizeX());

    //initialize the map with friction values

    bool stop = false;

    int i = 0;
    int i2 = 0;

    #pragma omp parallel private(stop) shared(i)
    {

        #pragma omp critical
        {
            i = 0;

        }


        while( true)
        {
            #pragma omp single
            {

                stop = false;
                i = i+1;
            }

            #pragma omp barrier

            bool do_stop = false;

            #pragma omp critical
            {
                i2 = i;
                if(i2>= iter_max)
                {
                    stop = true;
                    do_stop = true;
                }
            }

            #pragma omp barrier

            if(do_stop)
            {
                break;
            }


            float progress = ((float)(i))/std::max(1.0f,((float)(iter_max)));

            #pragma omp for collapse(2)
            for(int c = 0; c < H->data.nr_cols();c++)
            {
                for(int r = 0; r < H->data.nr_rows();r++)
                {
                    if(!pcr::isMV(H->data[r][c]))
                    {

                        //get the new height we would get from a neighbor, check if its bigger than current height, replace if so

                        float hnew_x1 = H->data[r][c];
                        float hnew_x2 = H->data[r][c];
                        float hnew_y1 = H->data[r][c];
                        float hnew_y2 = H->data[r][c];

                        {
                            if(!OUTORMV(H,r,c-1))
                            {
                                hnew_x1 = std::max(0.0f,(H->data[r][c-1] + DEM->data[r][c-1]) -  DEM->data[r][c]);
                            }
                            if(!OUTORMV(H,r,c-1))
                            {
                                hnew_x2 = std::max(0.0f,(H->data[r][c+1] + DEM->data[r][c+1]) -  DEM->data[r][c]);
                            }
                            if(!OUTORMV(H,r,c-1))
                            {
                                hnew_y1 = std::max(0.0f,(H->data[r-1][c] + DEM->data[r-1][c]) -  DEM->data[r][c]);
                            }
                            if(!OUTORMV(H,r,c-1))
                            {
                                hnew_y2 = std::max(0.0f,(H->data[r+1][c] + DEM->data[r+1][c]) -  DEM->data[r][c]);
                            }
                        }

                        float hnew = std::max(hnew_x1,std::max(hnew_x2,std::max(hnew_y1,hnew_y2)));
                        HN->data[r][c] = std::max(H->data[r][c],hnew);
                    }
                }
            }


            #pragma omp barrier

            #pragma omp for collapse(2)
            for(int r = 0; r < H->data.nr_rows();r++)
            {
                for(int c = 0; c < H->data.nr_cols();c++)
                {
                    if(!pcr::isMV(H->data[r][c]))
                    {
                        H->data[r][c] = HN->data[r][c];
                    }
                }
            }
        }

        #pragma omp barrier

    }

    delete HN;

    return H;



}

/*inline std::vector<cTMap *> AS_FloodFill(cTMap * DEM, cTMap * HBoundary, cTMap * Manning, int iter_max, bool subtract_slope)
{
    cTMap * H = HBoundary->GetCopy();
    cTMap * HN = HBoundary->GetCopy();
    cTMap * Slope = DEM->GetCopy0();
    cTMap * Travel = DEM->GetCopy0();
    cTMap * TravelN = DEM->GetCopy0();
    cTMap * Dist = DEM->GetCopy0();
    cTMap * DistN = DEM->GetCopy0();
    cTMap * UX = DEM->GetCopy0();
    cTMap * UXN = DEM->GetCopy0();
    cTMap * UY = DEM->GetCopy0();
    cTMap * UYN = DEM->GetCopy0();

    float _dx = std::fabs(H->cellSizeX());

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                float dem = DEM->Drc;
                float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                float Sx1 = (demx1 - (dem))/(_dx);
                float Sx2 = ((dem)- (demx2))/(_dx);
                float Sy1 = (demy1 - (dem))/(_dx);
                float Sy2 = ((dem)- (demy2))/(_dx);

                float sx = 0.0;
                float sy = 0.0;
                if(demx1 < demx2)
                {
                    sx = Sx1;
                }else
                {
                    sx = Sx2;
                }

                if(demy1 < demy2)
                {
                    sy = Sy1;
                }else
                {
                    sy = Sy2;
                }

                Slope->data[r][c] = (std::fabs(sx) + std::fabs(sy) + 1e-8f);
            }
        }
    }


    //initialize the map with friction values

    bool stop = false;

    int i = 0;
    int i2 = 0;

    #pragma omp parallel private(stop) shared(i)
    {
        double _dx = std::fabs(H->cellSizeX());




        #pragma omp critical
        {
            i = 0;

        }


        while( true)
        {
            #pragma omp single
            {

                stop = false;
                i = i+1;
                std::cout << "I is " << i << std::endl;
            }

            #pragma omp barrier

            bool do_stop = false;

            #pragma omp critical
            {
                i2 = i;
                if(i2>= iter_max)
                {
                    stop = true;
                    do_stop = true;
                }
            }

            #pragma omp barrier

            if(do_stop)
            {
                break;
            }


            float progress = ((float)(i))/std::max(1.0f,((float)(iter_max)));

            #pragma omp for collapse(2)
            for(int c = 0; c < H->data.nr_cols();c++)
            {
                for(int r = 0; r < H->data.nr_rows();r++)
                {
                    if(!pcr::isMV(H->data[r][c]))
                    {
                        if(H->data[r][c] > 1e-8f)
                        {
                            float dem = 0.0;//;DEM->Drc;
                            float demx1 =0.0;//!OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                            float demx2 = 0.0;//!OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                            float demy1 = 0.0;//!OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                            float demy2 = 0.0;//!OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;


                                dem = DEM->Drc;
                                demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                                demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                                demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                                demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;


                            float newHx1 = H->data[r][c] - subtract_slope? std::max(0.0f,dem - demx1):0.0;
                            float newHx2 = H->data[r][c] - subtract_slope? std::max(0.0f,dem - demx2):0.0;
                            float newHy1 = H->data[r][c] - subtract_slope? std::max(0.0f,dem - demy1):0.0;
                            float newHy2 = H->data[r][c] - subtract_slope? std::max(0.0f,dem - demy2):0.0;



                            float vel = std::max(1e-4f,std::pow(H->data[r][c],2.0f/3.0f) * std::sqrt(Slope->data[r][c] + 0.5f * H->data[r][c])/Manning->data[r][c]);
                            //float velx = sx/Slope->data[r][c];
                            //float vely = sy/Slope->data[r][c];
                        }
                    }
                }
            }


            #pragma omp barrier

            #pragma omp for collapse(2)
            for(int r = 0; r < H->data.nr_rows();r++)
            {
                for(int c = 0; c < H->data.nr_cols();c++)
                {
                    if(!pcr::isMV(H->data[r][c]))
                    {
                        H->data[r][c] = HN->data[r][c];
                        Travel->data[r][c] = TravelN->data[r][c];
                    }
                }
            }
        }

        #pragma omp barrier

    }

    delete HN;
    delete TravelN;

    return {H,Travel};
}*/

#include <math.h>

#define STOP 1.0e-8
#define TINY 1.0e-30

inline double incbeta(double a, double b, double x) {
    if (x < 0.0 || x > 1.0)
    {
        return 1.0/0.0;
    }

    /*The continued fraction converges nicely for x < (a+1)/(a+b+2)*/
    if (x > (a+1.0)/(a+b+2.0)) {
        return (1.0-incbeta(b,a,1.0-x)); /*Use the fact that beta is symmetrical.*/
    }

    /*Find the first part before the continued fraction.*/
    const double lbeta_ab = lgamma(a)+lgamma(b)-lgamma(a+b);
    const double front = exp(log(x)*a+log(1.0-x)*b-lbeta_ab) / a;

    /*Use Lentz's algorithm to evaluate the continued fraction.*/
    double f = 1.0, c = 1.0, d = 0.0;

    int i, m;
    for (i = 0; i <= 200; ++i) {
        m = i/2;

        double numerator;
        if (i == 0) {
            numerator = 1.0; /*First numerator is 1.0.*/
        } else if (i % 2 == 0) {
            numerator = (m*(b-m)*x)/((a+2.0*m-1.0)*(a+2.0*m)); /*Even term.*/
        } else {
            numerator = -((a+m)*(a+b+m)*x)/((a+2.0*m)*(a+2.0*m+1)); /*Odd term.*/
        }

        /*Do an iteration of Lentz's algorithm.*/
        d = 1.0 + numerator * d;
        if (fabs(d) < TINY) d = TINY;
        d = 1.0 / d;

        c = 1.0 + numerator / c;
        if (fabs(c) < TINY) c = TINY;

        const double cd = c*d;
        f *= cd;

        /*Check for stop.*/
        if (fabs(1.0-cd) < STOP) {
            return front * (f-1.0);
        }
    }

    return 1.0/0.0; /*Needed more loops, did not converge.*/
}

inline double student_t_cdf(double t, double v) {
    /*The cumulative distribution function (CDF) for Student's t distribution*/
    double x = (t + sqrt(t * t + v)) / (2.0 * sqrt(t * t + v));
    double prob = incbeta(v/2.0, v/2.0, x);
    return prob;
}

inline static cTMap * SteadyStateCorrection2(cTMap * b, cTMap * Trel)
{

    if(!(b->nrRows() == Trel->nrRows() && b->nrCols() == Trel->nrCols()))
    {
        LISEMS_ERROR("Steay state correction called with maps of different numbers of rows and columns");
        throw 1;
    }

    cTMap * ret =b->GetCopy();

    for(int r = 0; r < ret->nrRows(); r++)
    {
        for(int c = 0; c < ret->nrCols();c++)
        {
            if(!pcr::isMV(b->data[r][c]) && !pcr::isMV(Trel->data[r][c]))
            {
                double tf = std::min(1.0f,Trel->data[r][c]);
                double bh = b->data[r][c];

                double g = tgamma(1.0+1.0*bh);
                double g2 = tgamma(2.0+2.0*bh);
                double final =((std::pow(1.0-tf,1.0+bh)*std::pow(tf,bh) + (1.0+bh)*incbeta(1.0+bh,1.0+bh,tf))*g2)/(g*g);



                if(std::isfinite(final))
                {
                    ret->data[r][c] = std::min(1.0,std::max(0.0,final));
                }else
                {
                    //okey, so if we dont get the convergence with the gamma functions and hypergeometric 2F1 functions, this is generally because we are on the very far right of the distribution.
                    //simply set the value to 1.0 seems to work well enough
                    ret->data[r][c] = 1.0;
                }


            }else
            {
                pcr::setMV(ret->data[r][c]);
            }
        }
    }

    return ret;

}


#include "rasterdiffuse.h"


//returns height, vel, discharge, accuflux, arrival time, flood effort, QX1, QX2, QY1, QY2
inline std::vector<cTMap * > AS_FastFlood(cTMap * DEMin, cTMap * N, cTMap * rain, float duration, float scale_initial = 1.0, float iter_scale = 1.0)
{


    int iter = std::max(1,(int)std::max(DEMin->data.nr_rows(),DEMin->data.nr_cols())) * iter_scale;

    std::cout << "iter " << iter << std::endl;

    //get plain old preconditioning

    float cellsize = std::max(1e-6,std::fabs(DEMin->cellSizeX()));

    cTMap * DEM = DEMin->GetCopy();
    cTMap * HInitial = DEM->GetCopy();
    cTMap * Rain = DEM->GetCopy();
    cTMap * Zero = DEM->GetCopy0();
    cTMap * One = DEM->GetCopy0();
    cTMap * HmN = DEM->GetCopy0();
    cTMap * Vel = DEM->GetCopy();
    //cTMap * TT = DEM->GetCopy();
    //cTMap * Q = DEM->GetCopy();
    cTMap * S = DEM->GetCopy();
    cTMap * H2 = DEM->GetCopy0();
    cTMap * S2 = DEM->GetCopy();
    cTMap * B = DEM->GetCopy();

    //get ratio
    cTMap * temp2 = H2->GetCopy(); // h3 * ratio
    cTMap * temp1 = H2->GetCopy(); // h3
    cTMap * temp3 = H2->GetCopy(); // ratio_sigma
    cTMap * temp4 = H2->GetCopy(); // ratio_feedback

    float totalpix= DEM->data.nr_rows() * DEM->data.nr_cols();
    float maxv = -1e31;
    float minv = 1e31;

    {
        bool found = false;

        #pragma omp parallel for collapse(2) shared(found) reduction(max:maxv)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(pcr::isMV(DEM->data[r][c]))
                {
                }else
                {
                    maxv = std::max(maxv,DEM->data[r][c]);
                    found = true;

                }
            }
        }

        if(!found)
        {
            maxv = 1.0;
        }
    }

    {
        bool found = false;

        #pragma omp parallel for collapse(2) shared(found) reduction(min:minv)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(pcr::isMV(DEM->data[r][c]))
                {
                }else
                {
                    minv = std::min(minv,DEM->data[r][c]);
                    found = true;

                }
            }
        }

        if(!found)
        {
            minv = 0.0;
        }
    }
    if(maxv - minv < 1e-6)
    {
        LISEMS_ERROR("Can not run the preconditioning without elevation differences, turning it off");
        maxv = minv + 1.0;
    }

    std::cout << "precondition " <<  " " << maxv << " " << minv << std::endl;

    double _dx = std::fabs(DEM->cellSizeX());
    float dt = 1.0;

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                float dem = DEM->Drc;
                float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                float Sx1 = (demx1 - (dem))/(_dx);
                float Sx2 = ((dem)- (demx2))/(_dx);
                float Sy1 = (demy1 - (dem))/(_dx);
                float Sy2 = ((dem)- (demy2))/(_dx);

                float sx = 0.0;
                float sy = 0.0;
                if(demx1 < demx2)
                {
                    sx = Sx1;
                }else
                {
                    sx = Sx2;
                }

                if(demy1 < demy2)
                {
                    sy = Sy1;
                }else
                {
                    sy = Sy2;
                }

                S->data[r][c] = (std::fabs(sx) + std::fabs(sy) + 1e-8f);

                float pit = std::min(std::max(0.0f,demx1 - dem),std::min(std::max(0.0f,demx2 - dem),std::min(std::max(0.0f,demy1 - dem),std::max(0.0f,demy2 - dem))));

                DEM->data[r][c] = DEM->data[r][c] + pit;
            }

        }
    }

    #pragma omp for collapse(2)
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {

                    float dem = DEM->Drc;
                    //already checked the maxv and minv values earlier
                    float inv_normz = 1.0 - (dem - minv)/(maxv-minv);
                    float inv_norms = exp(-S->data[r][c]);

                    float initial_guess_size = (cellsize*((float)(std::max(DEM->nrCols(),DEM->nrRows()))))/8000.0;
                    if(initial_guess_size > 1.0)
                    {
                        initial_guess_size = 1.0;
                    }
                    float initial_guess_rain = 0.35 *(rain->data[r][c] *(1.0/3600000.0)/0.00001);

                    HInitial->data[r][c] = initial_guess_size * initial_guess_rain*scale_initial * inv_normz * inv_norms;
                    Rain->data[r][c] = rain->data[r][c] * 1.0/3600000.0;
                    One->data[r][c] = 1.0;

            }
        }
    }

    std::cout << "do flow " << std::endl;
    //run non-limited accuflux-2d
    std::vector<cTMap * > flow1 = AccuFluxDiffusive(DEM,Rain,HInitial,Zero,One,iter,0.15,1.0,false,false,false,0.0);

    //std::cout << "do flow 2 "<< std::endl;

    {
        std::vector<cTMap *> att_aft =AS_Accumulate2DT(flow1[1],flow1[2],flow1[3],flow1[4],Rain,iter);
        cTMap * at_af = att_aft[1]; //will be arrival time later
        cTMap * q_af = att_aft[0]; //will be accuflux

        #pragma omp for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                    double at = at_af->data[r][c];
                    B->data[r][c] = - at /(at-1.0);

                    float dem = DEM->Drc;
                    float h = flow1[0]->Drc;
                    float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                    float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                    float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                    float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                    float hx1 = !OUTORMV(DEM,r,c-1)? flow1[0]->data[r][c-1] : h;
                    float hx2 = !OUTORMV(DEM,r,c+1)? flow1[0]->data[r][c+1] : h;
                    float hy1 = !OUTORMV(DEM,r-1,c)? flow1[0]->data[r-1][c] : h;
                    float hy2 = !OUTORMV(DEM,r+1,c)? flow1[0]->data[r+1][c] : h;

                    demx1 = demx1 + hx1;
                    demx2 = demx2 + hx2;
                    demy1 = demy1 + hy1;
                    demy2 = demy2 + hy2;


                    float Sx1 = (demx1 - (dem+h))/(_dx);
                    float Sx2 = ((dem+h)- (demx2))/(_dx);
                    float Sy1 = (demy1 - (dem+h))/(_dx);
                    float Sy2 = ((dem+h)- (demy2))/(_dx);


                    float sx = 0.0;
                    float sy = 0.0;
                    if(demx1 < demx2)
                    {
                        sx = Sx1;
                    }else
                    {
                        sx = Sx2;
                    }

                    if(demy1 < demy2)
                    {
                        sy = Sy1;
                    }else
                    {
                        sy = Sy2;
                    }


                    float slope = (std::fabs(sx) + std::fabs(sy) + 1e-8f);

                    Vel->data[r][c] = std::pow(h,3.0f/2.0f)*std::sqrt(slope)/N->data[r][c];

                }
            }
        }
        double tot1 = 0.0;
        double tot2 = 0.0;

        #pragma omp parallel for collapse(2) reduction(+:tot1,tot2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                    tot1 += cellsize/std::max(0.1f,Vel->data[r][c]);
                    tot2 += 1;
                }
            }
        }

        double corr_av = tot1/std::max(1e-6,tot2);
        double duration_norm = std::min(1.0,duration*3600.0/(iter * corr_av));

        std::cout << "normalized duration " << duration_norm << " " << corr_av << std::endl;

        cTMap * DN = DEM->GetCopy();
        #pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                    DN->data[r][c] = duration_norm;
                }
            }
        }

        std::cout << "get steady-state correction " << std::endl;
        cTMap * SS = SteadyStateCorrection2(B,DN);

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                    if(std::isnormal(SS->data[r][c]))
                    {
                        SS->data[r][c] = 1.0/std::max(0.001f,SS->data[r][c]);
                    }else
                    {
                        SS->data[r][c] = 1.0;
                    }
                }
            }
        }

        std::vector<cTMap * > flow2 = AccuFluxDiffusiveCP(DEM,Rain,flow1[0],Zero,SS,100,0.1,1.0,false,false,false,0.0);

        #pragma omp parallel for collapse(2)
        for(int r = 0; r < DEM->data.nr_rows();r++)
        {
            for(int c = 0; c < DEM->data.nr_cols();c++)
            {
                if(!pcr::isMV(DEM->data[r][c]))
                {
                   flow2[0]->data[r][c] = flow2[0]->data[r][c]/std::max(0.001f,SS->data[r][c]);
                }
            }
        }


        delete at_af; //will be arrival time later
        delete q_af; //will be accuflux

        delete SS;
        delete DN;
        delete flow1[0];
        delete flow1[1];
        delete flow1[2];
        delete flow1[3];
        delete flow1[4];
        flow1[0] = flow2[0];
        flow1[1] = flow2[1];
        flow1[2] = flow2[2];
        flow1[3] = flow2[3];
        flow1[4] = flow2[4];
    }
    //std::vector<cTMap * > flow2 = AccuFluxDiffusive(DEM,Rain,flow1[0],Zero,200,0.05,1.0,false,false,true,0.0);

    std::cout << "prepare accumulation " << std::endl;
    //run limited accuflux-2d
    #pragma omp for collapse(2)
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {

                float dem = DEM->Drc;
                float h = flow1[0]->Drc;
                float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                float hx1 = !OUTORMV(DEM,r,c-1)? flow1[0]->data[r][c-1] : h;
                float hx2 = !OUTORMV(DEM,r,c+1)? flow1[0]->data[r][c+1] : h;
                float hy1 = !OUTORMV(DEM,r-1,c)? flow1[0]->data[r-1][c] : h;
                float hy2 = !OUTORMV(DEM,r+1,c)? flow1[0]->data[r+1][c] : h;

                demx1 = demx1 + hx1;
                demx2 = demx2 + hx2;
                demy1 = demy1 + hy1;
                demy2 = demy2 + hy2;


                float Sx1 = (demx1 - (dem+h))/(_dx);
                float Sx2 = ((dem+h)- (demx2))/(_dx);
                float Sy1 = (demy1 - (dem+h))/(_dx);
                float Sy2 = ((dem+h)- (demy2))/(_dx);


                float sx = 0.0;
                float sy = 0.0;
                if(demx1 < demx2)
                {
                    sx = Sx1;
                }else
                {
                    sx = Sx2;
                }

                if(demy1 < demy2)
                {
                    sy = Sy1;
                }else
                {
                    sy = Sy2;
                }

                float slope = (std::fabs(sx) + std::fabs(sy) + 1e-8f);

                S2->data[r][c] = slope;
                HmN->data[r][c] = flow1[0]->data[r][c] * N->data[r][c];
                Vel->data[r][c] = std::pow(h,3.0f/2.0f)*std::sqrt(slope)/N->data[r][c];

            }
        }
    }

    std::cout << "do accumulate " <<std::endl;

    std::vector<cTMap *> att_aft =AS_Accumulate2DT(flow1[1],flow1[2],flow1[3],flow1[4],Rain,iter);
    cTMap * at_af = att_aft[1]; //will be arrival time later
    cTMap * q_af = att_aft[0]; //will be accuflux
    cTMap * hn_af = AS_Accumulate2D(flow1[1],flow1[2],flow1[3],flow1[4],HmN,iter); //will be n_af later
    cTMap * h_af = AS_Accumulate2D(flow1[1],flow1[2],flow1[3],flow1[4],flow1[0],iter);
    //cTMap * one_af = AS_Accumulate2D(flow1[1],flow1[2],flow1[3],flow1[4],One,iter);
    //cTMap * dt_af = AS_Accumulate2D(flow1[1],flow1[2],flow1[3],flow1[4],TT,iter);


    double tot1 = 0.0;
    double tot2 = 0.0;

    #pragma omp parallel for collapse(2) reduction(+:tot1,tot2)
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                tot1 += cellsize/std::max(0.1f,Vel->data[r][c]);
                tot2 += 1;
            }
        }
    }

    double corr_av = tot1/std::max(1e-6,tot2);
    double duration_norm = std::min(1.0,duration*3600.0/(iter * corr_av));

    std::cout << "invert accumulation " << std::endl;
    #pragma omp parallel for collapse(2)
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                HmN->data[r][c] = hn_af->data[r][c]/std::max(1e-6f,h_af->data[r][c]);
                double at = at_af->data[r][c];
                B->data[r][c] = - at /(at-1.0);

                float s = S2->data[r][c];
                float sx1 = !OUTORMV(S2,r,c-1)? S2->data[r][c-1] : s;
                float sx2 = !OUTORMV(S2,r,c+1)? S2->data[r][c+1] : s;
                float sy1 = !OUTORMV(S2,r-1,c)? S2->data[r-1][c] : s;
                float sy2 = !OUTORMV(S2,r+1,c)? S2->data[r+1][c] : s;
                float sx11 = !OUTORMV(S2,r-1,c-1)? S2->data[r-1][c-1] : s;
                float sx22 = !OUTORMV(S2,r+1,c+1)? S2->data[r+1][c+1] : s;
                float sy11 = !OUTORMV(S2,r-1,c+1)? S2->data[r-1][c+1] : s;
                float sy22 = !OUTORMV(S2,r+1,c-1)? S2->data[r+1][c-1] : s;

                float slopeav =std::max(0.001,(1.0/9.0)*(s + sx1 + sx2 + sy1 + sy2 + sx11 + sx22 + sy11 + sy22));

                //need to apply steady-state compensation later
                H2->data[r][c] = std::pow(cellsize*q_af->data[r][c] * N->data[r][c]/std::sqrt(slopeav),5.0f/6.0f);
            }
        }
    }

    std::cout << "normalized duration " << duration_norm << " " << corr_av << std::endl;

    cTMap * DN = DEM->GetCopy();
    #pragma omp parallel for collapse(2)
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                DN->data[r][c] = duration_norm;
            }
        }
    }

    std::cout << "get steady-state correction " << std::endl;
    cTMap * SS = SteadyStateCorrection2(B,DN);
    cTMap * H3 = DEM->GetCopy0();


    cTMap * ratio = DEM->GetCopy();
    #pragma omp parallel for collapse(2)
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                if(!std::isnormal(SS->data[r][c]))
                {
                    SS->data[r][c] = 1.0;
                }

                H3->data[r][c] = H2->data[r][c];

                H2->data[r][c] = H2->data[r][c] * SS->data[r][c];

                //float h_afv = h_af->data[r][c]/std::max(1e-6f,one_af->data[r][c]);

                ratio->data[r][c] =std::min(5.0f,H2->data[r][c] / std::max(1e-6f,flow1[0]->data[r][c]));

                float h3 = std::max(std::min(0.01f,std::max(std::pow(std::min(0.5f,std::max(0.01f,0.01f/std::max(0.00000000001f,H2->data[r][c]))),10.0f),0.000000000000001f)),H2->data[r][c] * H2->data[r][c] * H2->data[r][c]);


                float h = flow1[0]->data[r][c];
                if(!pcr::isMV(h))
                {
                    temp1->data[r][c] = h3 * std::min(5.0f,H2->data[r][c] / std::max(1e-6f,flow1[0]->data[r][c]));
                }else
                {
                    temp1->data[r][c] = h3 * 1.0;
                }
                temp2->data[r][c] = h3;
                temp3->data[r][c] = (0.01f/std::max(0.0001f,S2->data[r][c])) * std::max(H2->data[r][c],flow1[0]->data[r][c]);
                temp4->data[r][c] = 0.99 * (1.0 - std::exp(-std::max(0.0,H2->data[r][c] - 0.25 * cellsize)));//std::min(0.9f,3.0f * std::max(0.0f,H2->data[r][c]-0.5f * h_afv));
            }
        }
    }

    std::cout << "conductive diffusive spread " << std::endl;

    std::vector<cTMap *> duo = ConductiveFeedbackDiffuseDuo(temp1,temp2,DEM,temp3,temp4,iter,0.25);
    cTMap * spread = duo[0];
    cTMap * spreadh = duo[1];

    //re-scale original height

    #pragma omp paralell for collapse(2)
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                float ratio_final = spread->data[r][c]/std::max(1e-6f,spreadh->data[r][c]);
                spread->data[r][c] = ratio_final;
                H2->data[r][c] = flow1[0]->data[r][c] * ratio_final;

                //get velocity based on new heights
            }
        }
    }

    std::cout << "get velocity and discharge from final height " << std::endl;

    #pragma omp parallel for collapse(2)
    for(int r = 0; r < DEM->data.nr_rows();r++)
    {
        for(int c = 0; c < DEM->data.nr_cols();c++)
        {
            if(!pcr::isMV(DEM->data[r][c]))
            {
                float dem = DEM->Drc;
                float h = H2->Drc;
                float demx1 = !OUTORMV(DEM,r,c-1)? DEM->data[r][c-1] : dem;
                float demx2 = !OUTORMV(DEM,r,c+1)? DEM->data[r][c+1] : dem;
                float demy1 = !OUTORMV(DEM,r-1,c)? DEM->data[r-1][c] : dem;
                float demy2 = !OUTORMV(DEM,r+1,c)? DEM->data[r+1][c] : dem;

                float hx1 = !OUTORMV(DEM,r,c-1)? flow1[0]->data[r][c-1] : h;
                float hx2 = !OUTORMV(DEM,r,c+1)? flow1[0]->data[r][c+1] : h;
                float hy1 = !OUTORMV(DEM,r-1,c)? flow1[0]->data[r-1][c] : h;
                float hy2 = !OUTORMV(DEM,r+1,c)? flow1[0]->data[r+1][c] : h;

                demx1 = demx1 + hx1;
                demx2 = demx2 + hx2;
                demy1 = demy1 + hy1;
                demy2 = demy2 + hy2;

                float Sx1 = (demx1 - (dem+h))/(_dx);
                float Sx2 = ((dem+h)- (demx2))/(_dx);
                float Sy1 = (demy1 - (dem+h))/(_dx);
                float Sy2 = ((dem+h)- (demy2))/(_dx);


                float sx = 0.0;
                float sy = 0.0;
                if(demx1 < demx2)
                {
                    sx = Sx1;
                }else
                {
                    sx = Sx2;
                }

                if(demy1 < demy2)
                {
                    sy = Sy1;
                }else
                {
                    sy = Sy2;
                }

                float slope = (std::fabs(sx) + std::fabs(sy) + 1e-8f);

                Vel->data[r][c] = std::pow(h,3.0f/2.0f)*std::sqrt(slope)/N->data[r][c];
            }
        }
    }

    delete Zero;
    delete One;
    delete S;
    delete temp1;
    delete temp2;
    delete temp3;
    delete temp4;
    //delete flow1[0];

    delete S2;
    //delete H2;
    delete DN;
    //delete TT;
    //delete spread;
    delete spreadh;
    delete hn_af;
    delete h_af;

    return{H2       ,Vel       ,SS         ,B,     q_af
         ,at_af        , spread, flow1[0],  H3, HInitial,
          HmN, flow1[1],  flow1[2]  ,flow1[3]  ,flow1[4]        };
}

//returns height, vel, discharge, accuflux, arrival time, flood effort, QX1, QX2, QY1, QY2
inline std::vector<cTMap * > AS_FastFloodBoundaryCondition(cTMap * DEM, cTMap * N, cTMap * rain, float duration, cTMap * FixedH, float scale_initial = 1.0)
{

    //run non-limited accuflux-2d


    //run limited accuflux-2d

    //get boundary condition

    //get accuflux

    //get steady-state compensation

    //weight steady-state compensation based on rainfall and boundary condition flux

    //invert accuflux


    //get ratio


    //do anistotropic diffusion


    //re-scale original height

    return {};
}


//returns height, vel, discharge, accuflux, arrival time, flood effort, QX1, QX2, QY1, QY2
inline std::vector<cTMap * > AS_FastFloodBoundaryCondition(cTMap * DEM, cTMap * N, cTMap * rain, float duration, cTMap * FixedH, cTMap * InitialH)
{

    //run non-limited accuflux-2d


    //run limited accuflux-2d

    //get boundary condition

    //get accuflux

    //get steady-state compensation

    //weight steady-state compensation based on rainfall and boundary condition flux

    //invert accuflux


    //get ratio


    //do anistotropic diffusion


    //re-scale original height

    return {};
}



#endif // RASTERFASTFLOW_H

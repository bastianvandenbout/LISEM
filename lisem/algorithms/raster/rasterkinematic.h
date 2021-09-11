#ifndef RASTERKINEMATIC_H
#define RASTERKINEMATIC_H

#include "raster/rastercommon.h"

#define LISEM_KIN_MIN_FLUX 1e-7
#define LISEM_KIN_MAX_ITERS 100

//---------------------------------------------------------------------------
/**
 * @fn double TWorld::complexSedCalc(double Qj1i1, double Qj1i, double Qji1,double Sj1i, double Sji1, double alpha, double dt,double dx)
 * @brief Complex calculation of sediment outflux from a cell
 *
 * Complex calculation of sediment outflux from a cell based on a explicit solution of the time/space matrix,
 * j = time and i = place: j1i1 is the new output, j1i is the new flux at the upstream 'entrance' flowing into the gridcell
 *
 * @param Qj1i1 : result kin wave for this cell ( Qj+1,i+1 )  ;j = time, i = place )
 * @param Qj1i : sum of all upstreamwater from kin wave ( Qj+1,i )
 * @param Qji1 : incoming Q for kinematic wave (t=j) in this cell, map Qin in LISEM (Qj,i+1)
 * @param Sj1i : sum of all upstream sediment (Sj+1,i)
 * @param Sji1 : incoming Sed for kinematic wave (t=j) in this cell, map Qsin in LISEM (Si,j+1)
 * @param alpha : alpha calculated in LISEM from before kinematic wave
 * @param dt : timestep
 * @param dx : length of the cell, corrected for slope (DX map in LISEM)
 * @return sediment outflow in next timestep
 *
 */
inline static double complexSedCalc(double Qj1i1, double Qj1i, double Qji1,double Sj1i, double Sji1, double alpha, double dt,double dx)
{
    double Sj1i1, Cavg, Qavg, aQb, abQb_1, A, B, C, s = 0;
    const double beta = 0.6;


    if (Qj1i1 < LISEM_KIN_MIN_FLUX)
        return (0);

    Qavg = 0.5*(Qji1+Qj1i);
    if (Qavg <= LISEM_KIN_MIN_FLUX)
        return (0);

    Cavg = (Sj1i+Sji1)/(Qj1i+Qji1);
    aQb = alpha*pow(Qavg,beta);
    abQb_1 = alpha*beta*pow(Qavg,beta-1);

    A = dt*Sj1i;
    B = -dx*Cavg*abQb_1*(Qj1i1-Qji1);
    C = (Qji1 <= LISEM_KIN_MIN_FLUX ? 0 : dx*aQb*Sji1/Qji1);
    if (Qj1i1 > LISEM_KIN_MIN_FLUX)
        Sj1i1 = (dx*dt*s+A+C+B)/(dt+dx*aQb/Qj1i1);
    else
        Sj1i1 = 0;

    return std::max(0.0 ,Sj1i1);
}


inline static double IterateToQnew(double Qin, double Qold, double q, double alpha, double deltaT, double deltaX)
{
    /* Using Newton-Raphson Method */
    double  ab_pQ, deltaTX, C;  //auxillary vars
    int   count;
    double Qkx; //iterated discharge, becomes Qnew
    double fQkx = 1.0; //function
    double dfQkx;  //derivative
    const double _epsilon = 1e-12;
    const double beta = 0.6;

    /* common terms */
     // ab_pQ = alpha*beta*pow(((Qold+Qin)/2),beta-1);
    // derivative of diagonal average (space-time)

    deltaTX = deltaT/deltaX;
    C = deltaTX*Qin + alpha*pow(Qold,beta) + deltaT*q;
    //dt/dx*Q = m3/s*s/m=m2; a*Q^b = A = m2; q*dt = s*m2/s = m2
    //C is unit volume of water
    // can be negative because of q

    // if C < 0 than all infiltrates, return 0, if all fluxes 0 then return
    if (C < 0)
    {
        return(0);
    }

    // pow function sum flux must be > 0
    if (Qold+Qin > 0)
    {
        ab_pQ = alpha*beta*pow(((Qold+Qin)/2),beta-1);
        // derivative of diagonal average (space-time), must be > 0 because of pow function
        Qkx = (deltaTX * Qin + Qold * ab_pQ + deltaT * q) / (deltaTX + ab_pQ);
        // explicit first guess Qkx, VERY important
        Qkx   = std::max(Qkx, 0.0);
    }
    else
        Qkx =  0;

    Qkx   = std::isnan(Qkx) ? 0.0 : std::max(Qkx, 0.0);
    if (Qkx < LISEM_KIN_MIN_FLUX)
        return(0);

    // avoid spurious iteration
    count = 0;
    do {
        fQkx  = deltaTX * Qkx + alpha * pow(Qkx, beta) - C;   /* Current k */ //m2
        dfQkx = deltaTX + alpha * beta * pow(Qkx, beta - 1);  /* Current k */
        Qkx   -= fQkx / dfQkx;                                /* Next k */

        Qkx   = std::isnan(Qkx) ? 0.0 : std::max(Qkx, 0.0);
        count++;
    } while(fabs(fQkx) > _epsilon && count < LISEM_KIN_MAX_ITERS);

    return Qkx;
}
//---------------------------------------------------------------------------
/**
 * @fn void TWorld::Kinematic(int pitRowNr, int pitColNr, cTMap *_LDD,cTMap *_Q, cTMap *_Qn, cTMap *_q, cTMap *_Alpha, cTMap *_DX,cTMap *_Vol,cTMap *_StorVol)
 * @brief Spatial implementation of the kinematic wave
 *
 * Kinematic wave spatial part, used for slope, channel and tiledrain system:
 * Kinematic is called for each pit (i.e. a cell with value 5 in the LDD):
 * A linked list of all cells connected to the pit is made, after that it 'walks' through the list
 * calculating the water fluxes from upstream to downstream.
 *
 * @param pitRowNr : row nr of the current pit (can be more than one, LISEM loops through all the pits)
 * @param pitColNr : col nr of the current pit (can be more than one, LISEM loops through all the pits)
 * @param _LDD : LDD map, can be slope, channel or tiledrain system
 * @param _Q : incoming discharge at the start of the timestep (m3/s)
 * @param _Qn : Outgoing new discharge at the end of the timestep (m3/s)
 * @param _q : infiltration surplus from infiltration functions, 0 or negative value
 * @param _Alpha : a in A=aQ^b
 * @param _DX : dx corrected for slope
 * @param _Vol : water volume in cell (m3)
 * @param _StorVol : water volume in buffers (m3)
 * @return new water discharge
 *
 * @see TWorld::IterateToQnew
 * @see TWorld::LDD
 */
inline static void Kinematic(int pitRowNr, int pitColNr, cTMap *_LDD,
                       cTMap *_Q, cTMap *_Qn,
                       cTMap *_q, cTMap *_Alpha, float _DX,
                        double _dt)//,cTMap *_StorVol)
{
    int dx[10] = {0, -1, 0, 1, -1, 0, 1, -1, 0, 1};
    int dy[10] = {0, 1, 1, 1, 0, 0, 0, -1, -1, -1};

    /// Linked list of cells in order of LDD flow network, ordered from pit upwards
    LDD_LINKEDLIST *list = nullptr, *temp = nullptr;
    list = (LDD_LINKEDLIST *)malloc(sizeof(LDD_LINKEDLIST));

    list->prev = nullptr;
    /// start gridcell: outflow point of area
    list->rowNr = pitRowNr;
    list->colNr = pitColNr;

    //    if (SwitchErosion)
    //        _Qsn->fill(0);
    // set output sed flux to 0
    //   _Qn->setMV();
    // flag all Qn gridcell with MV
    // does not work with multtple pits because earlier iterations are set to zero

    while (list != nullptr)
    {
        int i = 0;
        bool  subCachDone = true; // are sub-catchment cells done ?
        int rowNr = list->rowNr;
        int colNr = list->colNr;

        /** put all points that have to be calculated to calculate the current point in the list,
         before the current point */
        for (i=1; i<=9; i++)
        {
            int r, c;
            int ldd = 0;

            // this is the current cell
            if (i==5)
                continue;

            r = rowNr+dy[i];
            c = colNr+dx[i];

            if (INSIDE(_LDD,r, c) && !pcr::isMV(_LDD->Drc))
                ldd = (int) _LDD->Drc;
            else
                continue;

            // check if there are more cells upstream, if not subCatchDone remains true
            if (pcr::isMV(_Qn->Drc) &&
                    FLOWS_TO(ldd, r, c, rowNr, colNr) &&
                    INSIDE(_LDD,r, c))
            {
                temp = (LDD_LINKEDLIST *)malloc(sizeof(LDD_LINKEDLIST));
                temp->prev = list;
                list = temp;
                list->rowNr = r;
                list->colNr = c;
                subCachDone = false;
            }
        }

        // all cells above a cell are linked in a "sub-catchment or branch
        // continue with water and sed calculations
        // rowNr and colNr are the last upstreM cell linked
        if (subCachDone)
        {
            double Qin=0.0;//, Sin=0.0;

            // for all incoming cells of a cell, sum Q and Sed and put in Qin and Sin
            // note these are values of Qn and Qsn so the new flux
            for (i=1;i<=9;i++)
            {
                int r, c, ldd = 0;

                if (i==5)  // Skip current cell
                    continue;

                r = rowNr+dy[i];
                c = colNr+dx[i];

                if (INSIDE(_LDD,r, c) && !pcr::isMV(_LDD->Drc))
                    ldd = (int) _LDD->Drc;
                else
                    continue;

                if (INSIDE(_LDD,r, c) &&
                        FLOWS_TO(ldd, r,c,rowNr, colNr) &&
                        !pcr::isMV(_LDD->Drc) )
                {
                    Qin += _Qn->Drc;
                    //QinKW->data[rowNr][colNr] = Qin;

                    /*if (SwitchErosion)
                        Sin += _Qsn->Drc;*/
                }
            }

            _Qn->data[rowNr][colNr] = IterateToQnew(Qin, _Q->data[rowNr][colNr], _q->data[rowNr][colNr],
                                                    _Alpha->data[rowNr][colNr], _dt, _DX);
            // Newton Rapson iteration for water of current cell

              /* cell rowN, colNr is now done */

            temp=list;
            list=list->prev;
            free(temp);
            // go to the previous cell in the list

        }/* eof subcatchment done */
    } /* eowhile list != nullptr */
}
//---------------------------------------------------------------------------
/**
 * @fn void TWorld::routeSubstance(int pitRowNr, int pitColNr, cTMap *_LDD, cTMap *_Q, cTMap *_Qn, cTMap *_Qs, cTMap *_Qsn, cTMap *_Alpha, cTMap *_DX, cTMap*  _Vol , cTMap*_Sed ,cTMap *_StorVol, cTMap *_StorSed)
 * @brief Spatial implementation of the kinematic wave for sediment
 *
 * Kinematic wave spatial sediment part, used for slope, channel and tiledrain system:
 * using the known old incoming and new outgoing discharges
 * And the old incoming sediment discharge values
 *
 * @param pitRowNr : row nr of the current pit (can be more than one, LISEM loops through all the pits, r_outlet and c_oulet is the main pit)
 * @param pitColNr : col nr of the current pit (can be more than one, LISEM loops through all the pits, r_outlet and c_oulet is the main pit)
 * @param _LDD : LDD map, can be slope, channel or tiledrain system
 * @param _Q : incoming discharge at the start of the timestep (m3/s)
 * @param _Qn : Outgoing new discharge at the end of the timestep (m3/s)
 * @param _Qs : incoming sediment discharge at the start of the timestep (kg/s)
 * @param _Qsn : Outgoing new sediment discharge at the end of the timestep (kg/s)
 * @param _q : infiltration surplus from infiltration functions, 0 or negative value
 * @param _Alpha : a in A=aQ^b
 * @param _DX : dx corrected for slope
 * @param _Vol : water volume in cell (m3)
 * @param _Sed : sediment volume in cell (m3)
 * @param _StorVol : water volume in buffers (m3)
 * @param _StorSed : sediment volume in buffers (m3)
 * @return new water discharge
 *
 * @see TWorld::complexSedCalc
 * @see TWorld::LDD
 */
inline static void routeSubstance(int pitRowNr, int pitColNr, cTMap *_LDD,
                            cTMap *_Q, cTMap *_Qn, cTMap *_Qs, cTMap *_Qsn,
                            cTMap *_Alpha, cTMap*  _Vol , cTMap *_Sed, float _dt)// ,cTMap *_StorVol, cTMap *_StorSed)
{
    int dx[10] = {0, -1, 0, 1, -1, 0, 1, -1, 0, 1};
    int dy[10] = {0, 1, 1, 1, 0, 0, 0, -1, -1, -1};

    /// Linked list of cells in order of LDD flow network, ordered from pit upwards
    LDD_LINKEDLIST *list = nullptr, *temp = nullptr;
    list = (LDD_LINKEDLIST *)malloc(sizeof(LDD_LINKEDLIST));
    list->prev = nullptr;
    /// start gridcell: outflow point of area
    list->rowNr = pitRowNr;
    list->colNr = pitColNr;

    double _dx = std::fabs(_LDD->cellSizeX());
    // _Qsn->fill(0);
    // NOT wrong when multiple outlets! DO this befiore the loop
    //VJ 12/12/12 set output substance to zero

    while (list != nullptr)
    {
        int i = 0;
        bool  subCachDone = true; // are sub-catchment cells done ?
        int rowNr = list->rowNr;
        int colNr = list->colNr;

        /** put all points that have to be calculated to calculate the current point in the list,
         before the current point */
        for (i=1; i<=9; i++)
        {
            int r, c;
            int ldd = 0;

            // this is the current cell
            if (i==5)
                continue;

            r = rowNr+dy[i];
            c = colNr+dx[i];

            if (INSIDE(_LDD,r, c) && !pcr::isMV(_LDD->Drc))
                ldd = (int) _LDD->Drc;
            else
                continue;

            // check if there are more cells upstream, if not subCatchDone remains true
            if (pcr::isMV(_Qsn->Drc) &&
                    FLOWS_TO(ldd, r, c, rowNr, colNr) &&
                    INSIDE(_LDD,r, c))
            {
                temp = (LDD_LINKEDLIST *)malloc(sizeof(LDD_LINKEDLIST));
                temp->prev = list;
                list = temp;
                list->rowNr = r;
                list->colNr = c;
                subCachDone = false;
            }
        }

        // all cells above a cell are linked in a "sub-catchment or branch
        // continue with water and sed calculations
        // rowNr and colNr are the last upstreM cell linked
        if (subCachDone)
        {
            double Qin=0.0, Sin=0.0;

            // for all incoming cells of a cell, sum Q and Sed and put in Qin and Sin
            // note these are values of Qn and Qsn so the new flux
            for (i=1;i<=9;i++)
            {
                int r, c, ldd = 0;

                if (i==5)  // Skip current cell
                    continue;

                r = rowNr+dy[i];
                c = colNr+dx[i];

                if (INSIDE(_LDD,r, c) && !pcr::isMV(_LDD->Drc))
                    ldd = (int) _LDD->Drc;
                else
                    continue;

                if (INSIDE(_LDD,r, c) &&
                        FLOWS_TO(ldd, r,c,rowNr, colNr) &&
                        !pcr::isMV(_LDD->Drc) )
                {
                    Qin += _Qn->Drc;
                    Sin += _Qsn->Drc;
                }
            }



                            //if (!SwitchSimpleSedKinWave)
            _Qsn->data[rowNr][colNr] = complexSedCalc(_Qn->data[rowNr][colNr], Qin, _Q->data[rowNr][colNr],
                                                      Sin, _Qs->data[rowNr][colNr], _Alpha->data[rowNr][colNr], _dt, _dx);
            /*else
                                _Qsn->data[rowNr][colNr] = simpleSedCalc(_Qn->data[rowNr][colNr], Qin, Sin, _dt,
                                                                         _Vol->data[rowNr][colNr], _Sed->data[rowNr][colNr]);
                            */
            _Qsn->data[rowNr][colNr] = std::min((double)_Qsn->data[rowNr][colNr], Sin+_Sed->data[rowNr][colNr]/_dt);
            // no more sediment outflow than total sed in cell

            _Sed->data[rowNr][colNr] = std::max(0.0, Sin*_dt + _Sed->data[rowNr][colNr] - _Qsn->data[rowNr][colNr]*_dt);
            // new sed volume based on all fluxes and org sed present

            /* cell rowN, colNr is now done */

            temp=list;
            list=list->prev;
            free(temp);
            // go to the previous cell in the list

        }/* eof subcatchment done */
    } /* eowhile list != nullptr */
}


inline static cTMap * AS_FlowKinematic(cTMap * LDD, cTMap * Slope, cTMap * Width, cTMap * N, cTMap * H, float dt)
{

    //convert height to discharge

    cTMap * Alpha = Slope->GetCopy0();
    cTMap * Q = Slope->GetCopy0();
    cTMap * Qn = Slope->GetCopy0();
    cTMap * infil = Slope->GetCopy0();

    for(int r = 0; r < Alpha->nrRows(); r++)
    {
        for(int c = 0; c < Alpha->nrCols(); c++)
        {
            if(!pcr::isMV(H->data[r][c]))
            {

                double Perim, Radius, Area;
                const double beta = 0.6;
                const double _23 = 2.0/3.0;
                double beta1 = 1/beta;
                double wh = H->data[r][c];
                double FW = Width->data[r][c];
                double grad = sqrt(sin(atan(std::max(Slope->data[r][c],1e-12f))));

                {
                    Perim = FW + 2.0*wh;
                    Area = FW*wh;
                }

                Radius = (Perim > 0 ? Area/Perim : 0);

                Alpha->Drc = std::pow(N->Drc/grad * std::pow(Perim, _23),beta);

                if (Alpha->Drc > 0)
                    Q->Drc = std::pow(Area/Alpha->Drc, beta1);
                else
                    Q->Drc = 0;

            }

            pcr::setMV(Qn->Drc);
        }

    }


    //now run the kinematic algorithm to get new q

    for(int r = 0; r < LDD->nrRows(); r++)
    {
        for(int c = 0; c < LDD->nrCols(); c++)
        {
            if(!pcr::isMV(LDD->data[r][c]))
            {

                if((int) LDD->data[r][c] == 5)
                {
                    Kinematic(r,c,LDD,Q,Qn,infil,Alpha,LDD->cellSize(),dt);

                }
            }
        }
    }

    //now convert back to flow height

    for(int r = 0; r < LDD->nrRows(); r++)
    {
        for(int c = 0; c < LDD->nrCols(); c++)
        {
            if(!pcr::isMV(LDD->data[r][c]))
            {
                Q->Drc = (Alpha->Drc*pow(Qn->Drc, 0.6))/Width->data[r][c];
            }
        }
    }

    delete Qn;
    delete infil;
    delete Alpha;

    return Q;
}


inline static cTMap * AS_FlowKinematicSubstance(cTMap * LDD, cTMap * Slope, cTMap * Width, cTMap * N, cTMap * H, cTMap * S,float dt)
{

    //convert height to discharge

    cTMap * Alpha = Slope->GetCopy0();
    cTMap * Q = Slope->GetCopy0();
    cTMap * Qn = Slope->GetCopy0();
    cTMap * infil = Slope->GetCopy0();
    cTMap * Sn = S->GetCopy0();
    cTMap * Qs = Slope->GetCopy0();
    cTMap * Qsn = Slope->GetCopy0();
    cTMap * Watervol = Slope->GetCopy0();

    for(int r = 0; r < Alpha->nrRows(); r++)
    {
        for(int c = 0; c < Alpha->nrCols(); c++)
        {
            if(!pcr::isMV(H->data[r][c]))
            {

                double Perim, Radius, Area;
                const double beta = 0.6;
                const double _23 = 2.0/3.0;
                double beta1 = 1/beta;
                double wh = H->data[r][c];
                double FW = Width->data[r][c];
                double grad = sqrt(sin(atan(std::max(Slope->data[r][c],1e-12f))));

                {
                    Perim = FW + 2.0*wh;
                    Area = FW*wh;
                }

                Radius = (Perim > 0 ? Area/Perim : 0);

                Alpha->Drc = std::pow(N->Drc/grad * std::pow(Perim, _23),beta);

                if (Alpha->Drc > 0)
                    Q->Drc = std::pow(Area/Alpha->Drc, beta1);
                else
                    Q->Drc = 0;



                double watervol = H->Drc * Width->data[r][c] * std::fabs(LDD->cellSizeX());
                double conc = S->Drc /watervol;
                Watervol->Drc = watervol;
                Qs->Drc = Q->Drc * conc;

            }

            pcr::setMV(Qn->Drc);

            pcr::setMV(Qsn->Drc);
        }

    }


    //now run the kinematic algorithm to get new q

    for(int r = 0; r < LDD->nrRows(); r++)
    {
        for(int c = 0; c < LDD->nrCols(); c++)
        {
            if(!pcr::isMV(LDD->data[r][c]))
            {

                if((int) LDD->data[r][c] == 5)
                {
                    Kinematic(r,c,LDD,Q,Qn,infil,Alpha,LDD->cellSize(),dt);

                }
            }
        }
    }


    for(int r = 0; r < LDD->nrRows(); r++)
    {
        for(int c = 0; c < LDD->nrCols(); c++)
        {
            if(!pcr::isMV(LDD->data[r][c]))
            {

                if((int) LDD->data[r][c] == 5)
                {
                    routeSubstance(r,c,LDD,Q,Qn,Qs,Qsn,Alpha,Watervol,Sn,dt);

                }
            }
        }
    }


    delete Q;
    delete Watervol;
    delete Qn;
    delete Qs;
    delete Qsn;
    delete infil;
    delete Alpha;

    return Sn;
}


#endif // RASTERKINEMATIC_H

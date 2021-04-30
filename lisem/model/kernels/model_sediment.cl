
#define MAXCONC 848.0f

#ifndef LISEM_KERNEL_CPUCODE
#define GRAV 9.81

__constant int ch_dx[10] = {0, -1, 0, 1, -1, 0, 1, -1, 0, 1};
__constant int ch_dy[10] = {0, 1, 1, 1, 0, 0, 0, -1, -1, -1};

#define nonkernel

#define int2(x,y) ((int2)((x),(y)))
#define float3(x,y,z) ((float3)((x),(y),(z)))
#else

#define nonkernel inline static
#endif

#ifndef LISEM_KERNEL_CPUCODE



nonkernel float minmod(float a, float b)
{
        float rec = 0.0f;
        if (a >= 0.0f && b >= 0.0f)
	{
        rec = min(a, b);
        }else if(a <= 0.0f && b <= 0.0f)
	{
        rec = max(a, b);
	}
	return rec;
}


nonkernel bool flow_to(int x,int y, int x2, int y2, int ldd)
{

        if(x + ch_dx[ldd] == x2 && y + ch_dy[ldd] == y2)
        {
                return true;
        }else
        {
                return false;
        }

}

#endif

nonkernel float GetSV(float d)
{
    //Stokes range settling velocity
    if(d < 100)
    {
        return 2.0*(2650.0-1000.0)*9.80*pow(d/2000000.0, 2.0)/(9.0*0.001);

    //Settling velocity by Zanke (1977)
    }else
    {
        float dm = d/1000.0;
        return 10.0 *(sqrt(1.0 + 0.01 *((2650.0-1000.0)/1000.0)* 9.81 *dm*dm*dm )-1.0)/(dm);
    }

}

nonkernel float GetTC(float hf, float v, float d50, float d90, float slope)
{

    float TC = 0.0;
    //if(hf < 0.25f)
    {

        float cg = pow((d50+5.0f)/0.32f, -0.6f);
        float dg = pow((d50+5.0f)/300.0f, 0.25f);

        //Calc transport capacity
        float omega = 100.0f* v*sin(atan(max(0.001f,slope)));

        // V in cm/s in this formula assuming grad is SINE
        float omegacrit = 0.4f;

        // critical unit streampower in cm/s
        TC = max(0.0f,min(MAXCONC, 2650.0f * cg * pow(max(0.0f, omega - omegacrit), dg)));
        TC = isnan(TC)? 0.0:TC;
        return TC;
        // not more than 2650*0.32 = 848 kg/m3


    }//else
    {
        //van rijn simple suspended load
        float ucr;
        float d50m = (d50/1000000.0);
        float d90m = (d90/1000000.0);
        float ps = 2400.0;
        float pw = 1000.0;
        float mu = 1.0;
        if( d50m < 0.0005)
        {
           ucr  = 0.19f * pow(d50m, 0.1f) * log10(4.0f* hf/d90m);
        }else
        {
           ucr  = 8.5f * pow(d50m, 0.6f) * log10(4.0f* hf/d90m);
        }
        float me = max((v - ucr)/sqrt(9.81f * d50m * (ps/pw - 1.0f)),0.0f);
        float ds = d50m * 9.81f * ((ps/pw)-1.0f)/(mu*mu);
        float qs = hf * 0.008f * ps*v * d50m * pow(me, 2.4f) * pow(ds, -0.6f);

        float tc =  qs/ (v * hf);
        tc = isnan(tc)? 0.0f:tc;
        return max(min(tc,MAXCONC),0.0f);
     }
}


kernel
void sediment(
#ifdef LISEM_KERNEL_CPUCODE
int id_1d_cpum,
#endif
int dim0,
                                int dim1,
                                float dx,
                                float dt,
                                __read_only image2d_t LOCX,
                                __read_only image2d_t LOCY,
                                __read_only image2d_t DEM,
                                __read_only image2d_t H,
                                __read_only image2d_t Vx,
                                __read_only image2d_t Vy,
                                __read_only image2d_t HS,
                                __read_only image2d_t VSx,
                                __read_only image2d_t VSy,
                                __read_only image2d_t SIfa,
                                __read_only image2d_t SRocksize,
                                __read_only image2d_t SDensity,
                                __read_only image2d_t CH_H,
                                __read_only image2d_t CH_V,
                                __read_only image2d_t CH_HS,
                                __read_only image2d_t CH_VS,
                                __read_only image2d_t CH_SIfa,
                                __read_only image2d_t CH_SRocksize,
                                __read_only image2d_t CH_SDensity,
                                __read_only image2d_t CH_MASK,
                                __read_only image2d_t CH_WIDTH,
                                __read_only image2d_t CH_HEIGHT,
                                __read_only image2d_t D50,
                                __read_only image2d_t D90,
                                __read_only image2d_t COH,
                                __read_only image2d_t CHCOH,
                                __read_only image2d_t Y,
                                __read_only image2d_t SD,
                                __read_only image2d_t MD,
                                __read_only image2d_t MIFA,
                                __read_only image2d_t MROCKSIZE,
                                __read_only image2d_t MDENS,
                                __read_only image2d_t SED,
                                __read_only image2d_t CHSED,
                                __read_only image2d_t DETTot,
                                __read_only image2d_t DEPTot,
                                __read_only image2d_t F_InfilAct,
                                __read_only image2d_t GW_US,
                                __read_only image2d_t GW_S,
                                __read_only image2d_t ENTRAINMENTOT,
                                __write_only image2d_t GW_WFN,
                                __write_only image2d_t GW_USN,
                                __write_only image2d_t GW_SN,
                                __write_only image2d_t SEDN,
                                __write_only image2d_t CHSEDN,
                                __write_only image2d_t DETTotN,
                                __write_only image2d_t DEPTotN,
                                __write_only image2d_t SDN,
                                __write_only image2d_t MDN,
                                __write_only image2d_t ENTRAINMENT,
                                __write_only image2d_t ENTRAINMENTCH,
                                __write_only image2d_t ENTRAINMENTOTN,
                                __write_only image2d_t DEMN,
                                __read_only image2d_t QSEDOUT,
                                __write_only image2d_t QSEDOUTN,
                                __write_only image2d_t OUTPUT_UI,
                                int ui_image,
                                int do_addsolids,
                                __write_only image2d_t HSN,
                                __write_only image2d_t VSxN,
                                __write_only image2d_t VSyN,
                                __write_only image2d_t SIfaN,
                                __write_only image2d_t SRocksizeN,
                                __write_only image2d_t SDensityN,
                                __write_only image2d_t CH_HSN,
                                __write_only image2d_t CH_VSN,
                                __write_only image2d_t CH_SIfaN,
                                __write_only image2d_t CH_SRocksizeN,
                                __write_only image2d_t CH_SDensityN,
                                __read_only image2d_t N,
                                __read_only image2d_t QFX1,
                                __read_only image2d_t QFX2,
                                __read_only image2d_t QFY1,
                                __read_only image2d_t QFY2,
                                __read_only image2d_t QFCHX1,
                                __read_only image2d_t QFCHX2,
                                __read_only image2d_t QFCHIN,
                                float rain_m,
                                __read_only image2d_t HARDCOVER,
                                __read_only image2d_t VEGCOVER,
                                __read_only image2d_t VEGH,
                                int do_erosion,
                                __write_only image2d_t HN,
                                __write_only image2d_t VFxN,
                                __write_only image2d_t VFyN
                                )
{

#ifdef LISEM_KERNEL_CPUCODE

#pragma omp parallel for
for(int id_1d_cpu = 0; id_1d_cpu < id_1d_cpum; id_1d_cpu++)
{
#endif

    ///////
    //Cell Location Calculations
    //Here we go from the invocation id to a row and column number for the cell and its neighbors
    //////

    float tx = dt/dx;

    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

    const int id_1d = (int)(get_global_id(0));

    const int idx = trunc((float)(id_1d)/(float)(dim1));
    const int idy = id_1d % dim1;

    const int gx_x = min(dim0-(int)(1),max((int)(0),(int)(idx)));
    const int gy_y = min(dim1-(int)(1),max((int)(0),(int)(idy)));

    int x = trunc(read_imagef(LOCX,sampler, int2(gx_x,gy_y)).x);
    int y = trunc(read_imagef(LOCY,sampler, int2(gx_x,gy_y)).x);

    const int gy = min(dim1-(int)(1),max((int)(0),(int)(x)));
    const int gx = min(dim0-(int)(1),max((int)(0),(int)(y)));

    const int gx_x1 = min(dim0-(int)(1),max((int)(0),(int)(gx - 1 )));
    const int gy_x1 = gy;
    const int gx_x2 = min(dim0-(int)(1),max((int)(0),(int)(gx + 1)));
    const int gy_x2 = gy;
    const int gx_y1 = gx;
    const int gy_y1 = min(dim1-(int)(1),max((int)(0),(int)(gy - 1)));
    const int gx_y2 = gx;
    const int gy_y2 = min(dim1-(int)(1),max((int)(0),(int)(gy + 1)));

    ///////
    //Actual model code
    //////

    float z = read_imagef(DEM,sampler, int2(gx,gy)).x;
    float hf = read_imagef(H,sampler, int2(gx,gy)).x;
    float vfx = read_imagef(Vx,sampler, int2(gx,gy)).x;
    float vfy = read_imagef(Vy,sampler, int2(gx,gy)).x;
    float hs = read_imagef(HS,sampler, int2(gx,gy)).x;
    float vsx = read_imagef(VSx,sampler, int2(gx,gy)).x;
    float vsy = read_imagef(VSy,sampler, int2(gx,gy)).x;

    float vx = (hf * vfx + hs * vsx)/max(0.001f,(hs+hf));
    float vy = (hf * vfy + hs * vsy)/max(0.001f,(hs+hf));
    float vel = sqrt(max(0.00001f,vfx*vfx+vfy*vfy));
    float svel = sqrt(max(0.00001f,vsx*vsx+vsy*vsy));

    float rocksizes = read_imagef(SRocksize,sampler, int2(gx,gy)).x;
    float densitys =read_imagef(SDensity,sampler, int2(gx,gy)).x;
    float ifas =read_imagef(SIfa,sampler, int2(gx,gy)).x;

    float d50 = read_imagef(D50,sampler, int2(gx,gy)).x;
    float d90 = read_imagef(D90,sampler, int2(gx,gy)).x;
    float dettot = read_imagef(DETTot,sampler, int2(gx,gy)).x;
    float deptot = read_imagef(DEPTot,sampler, int2(gx,gy)).x;

    float manning = read_imagef(N,sampler, int2(gx,gy)).x;
    float hardcover = read_imagef(HARDCOVER,sampler, int2(gx,gy)).x;
    float vegcover = read_imagef(VEGCOVER,sampler, int2(gx,gy)).x;
    float vegh = read_imagef(VEGH,sampler, int2(gx,gy)).x;
    float Coh = read_imagef(COH,sampler, int2(gx,gy)).x;
    float ch_Coh = read_imagef(CHCOH,sampler, int2(gx,gy)).x;

    float wf = read_imagef(F_InfilAct,sampler, int2(gx,gy)).x;
    float ush = read_imagef(GW_US,sampler, int2(gx,gy)).x;
    float gwh = read_imagef(GW_S,sampler, int2(gx,gy)).x;
    float sd = read_imagef(SD,sampler, int2(gx,gy)).x;

    float enttot = read_imagef(ENTRAINMENTOT,sampler, int2(gx,gy)).x;
    float materialdepth = read_imagef(MD,sampler, int2(gx,gy)).x;
    float mifa = read_imagef(MIFA,sampler, int2(gx,gy)).x;
    float mrocksize = read_imagef(MROCKSIZE,sampler, int2(gx,gy)).x;
    float mdens = read_imagef(MDENS,sampler, int2(gx,gy)).x;

    float sed = max(0.0f,read_imagef(SED,sampler, int2(gx,gy)).x);
    float ch_sed = read_imagef(CHSED,sampler, int2(gx,gy)).x;

    float qsedout = read_imagef(QSEDOUT,sampler, int2(gx,gy)).x;

    float watervol =max(0.0001f,(hf * dx * dx));
    float conc = sed /(watervol);

    float z_x1 = read_imagef(DEM,sampler, int2(gx_x1,gy)).x;
    float z_x2 = read_imagef(DEM,sampler, int2(gx_x2,gy)).x;
    float z_y1 = read_imagef(DEM,sampler, int2(gx,gy_y1)).x;
    float z_y2 = read_imagef(DEM,sampler, int2(gx,gy_y2)).x;

    if(gx + 1 > dim0-1)
    {
        z_x2 = -99999;
    }
    if(gx - 1 < 0)
    {
        z_x1 = -99999;
    }
    if(gy + 1 > dim1-1)
    {
        z_y2 = -99999;
    }
    if(gy - 1 < 0)
    {
        z_y1 = -99999;
    }

    float zc_x1 = z_x1 < -1000? z : z_x1;
    float zc_x2 = z_x2 < -1000? z : z_x2;
    float zc_y1 = z_y1 < -1000? z : z_y1;
    float zc_y2 = z_y2 < -1000? z : z_y2;

    float slopex = (zc_x2 - zc_x1)/(2.0f*dx);
    float slopey = (zc_y2 - zc_y1)/(2.0f*dx);

    float slope = (slopex <0.0f? -slopex:slopex) + (slopey <0.0f? -slopey:slopey) ;

    float depth_available_x = vx > 0.0f? max(0.0f,z-z_x2):max(0.0f,z-z_x1);
    float depth_available_y = vy > 0.0f? max(0.0f,z-z_y2):max(0.0f,z-z_y1);
    float depth_available = max(depth_available_x,depth_available_y);
    ///////
    //Actual model code
    //////

    float SDChange = 0.0f;

    //////
    //SPLASH DETACHMENT
    //////

    if(do_erosion)
    {


        float b= 0.0f;
        float strength=0.0f;
        float DetDT1 = 0.0f;
        float DetDT2 = 0.0f;
        float DetLD1 = 0.0f;
        float DetLD2 = 0.0f;
        float g_to_kg = 0.001f;

        float SplashDelivery = 0.1f;

        float KEParamater_a = 8.950f;
        float KEParamater_b = 0.520f;
        float KEParamater_c = 0.042f;

        float Int = rain_m * 3600.0f/dt * 1000.0f;
        // intensity in mm/h, Rain is in m

        float KE_DT = KEParamater_a*(1.0f-(KEParamater_b*exp(-KEParamater_c*Int)));
        // kin energy in J/m2/mm

       //case KE_EXPFUNCTION: KE_DT = KEParamater_a1*(1-(KEParamater_b1*exp(-KEParamater_c1*Int))); break;
       //case KE_LOGFUNCTION: KE_DT = (Int > 1 ? KEParamater_a2 + KEParamater_b2*log10(Int) : 0); break;
       //case KE_POWERFUNCTION: KE_DT = KEParamater_a3*pow(Int, KEParamater_b3); break;



        float directrain = (1.0f -vegcover)*rain_m * 1000.0f;
        // rainfall between plants in mm

        float KE_LD = max(15.3f*sqrt(vegh)-5.87f, 0.0f);
        // kin energy in J/m2/mm
        float throughfall = vegcover * rain_m * 1000.0f;

        float WH0 = exp(-1.48f*hf*1000.0f);
        // water buffer effect on surface, WH in mm in this empirical equation from Torri ?

        strength = 0.1033f/max(0.001f,Coh);
        b = 3.58f;
        // empirical analysis based on Limburg data, dating 1989 (Victor Jetten)


        // Between plants, directrain is already with 1-cover
        DetDT1 = g_to_kg *(strength*KE_DT+b)*WH0 * directrain;
        //ponded areas, kg/m2/mm * mm = kg/m2
        DetDT2 = g_to_kg *(strength*KE_DT+b) * directrain * SplashDelivery;
        //dry areas, kg/m2/mm * mm = kg/m2

        // Under plants, throughfall is already with cover
        DetLD1 = g_to_kg * (strength*KE_LD+b)*WH0 * throughfall;
        //ponded areas, kg/m2/mm * mm = kg/m2
        DetLD2 = g_to_kg *(strength*KE_LD+b) * throughfall * SplashDelivery;
        //dry areas, kg/m2/mm * mm = kg/m2

        float DETSplash = DetLD1 + DetLD2 + DetDT1 + DetDT2;
        // Total splash kg/m2

        // Deal with all exceptions:

        DETSplash *= dx * dx * (1.0f-hardcover);
        // kg/cell, only splash over soilwidth, not roads and channels
        // FROM KG/M2 TO KG/CELL

        DETSplash = max(0.0f,DETSplash);
        DETSplash = isnan(DETSplash)? 0.0f:DETSplash;


        dettot = dettot + DETSplash;
        sed = sed + DETSplash;
        //////
        //Flow Detachment and Deposition
        //////

        float detachment = 0.0f;
        float deposition = 0.0f;
        float dep_force = 0.0f;

        //deposite anything above MAXCONC
        {
            float sed_max = hf *dx*dx * MAXCONC;
            dep_force = max(0.0f,sed - sed_max);
            if(sed > sed_max)
            {
                sed = sed_max;
            }
            conc = sed /(watervol);



            float ErosiveEfficiency = min(1.0f, 0.79f*exp(-0.85f*Coh));
            //Y->Drc = std::min(1.0, 1.0/(0.89+0.56*CohesionSoil->Drc));
            //Y->Drc = std::min(1.0, 0.79*exp(-0.85*CohesionSoil->Drc));
            //Y->Drc = std::min(1.0, 1.0/(2.0*CohesionSoil->Drc));

            float sv = GetSV(d50);
            float TC = GetTC(hf, vel, d50,d90,slope);

            float maxTC = max(TC - conc,0.0f) ;
            // positive difference: TC deficit becomes detachment (ppositive)
            float minTC = min(TC - conc,0.0f) ;

            //deposition based on settling velocity
            float TransportFactor = (1.0f-exp(-dt*sv/max(0.0001f,hf))) * watervol;
            deposition = TransportFactor * minTC;
            deposition = min(deposition <0.0f? -deposition:deposition,((minTC * watervol)<0.0f?-1.0f:1.0f)*(minTC * watervol));

            TransportFactor = dt*sv * dx * dx;
            //correct detachment for grass strips, hard surfaces and houses
            detachment = ErosiveEfficiency* maxTC * TransportFactor;
            detachment = max(0.0f,min(detachment,watervol * maxTC));

            SDChange += (detachment/(dx*dx))*(1.0f/1800.0f);
        }

        //later we add the sediment to the total, and to depostion and detachment map

        //////
        //Sediment Movement
        //////

        float C = 0.1;

        float h_x1 = read_imagef(H,sampler, int2(gx_x1,gy)).x;
        float h_x2 = read_imagef(H,sampler, int2(gx_x2,gy)).x;
        float h_y1 = read_imagef(H,sampler, int2(gx,gy_y1)).x;
        float h_y2 = read_imagef(H,sampler, int2(gx,gy_y2)).x;

        float sed_x1 = read_imagef(SED,sampler, int2(gx_x1,gy)).x;
        float sed_x2 = read_imagef(SED,sampler, int2(gx_x2,gy)).x;
        float sed_y1 = read_imagef(SED,sampler, int2(gx,gy_y1)).x;
        float sed_y2 = read_imagef(SED,sampler, int2(gx,gy_y2)).x;

        float conc_x1 = sed_x1/max(0.01f,(h_x1 * dx * dx));
        float conc_x2 = sed_x2/max(0.01f,(h_x2 * dx * dx));
        float conc_y1 = sed_y1/max(0.01f,(h_y1 * dx * dx));
        float conc_y2 = sed_y2/max(0.01f,(h_y2 * dx * dx));

        float flux_x1 = read_imagef(QFX1,sampler, int2(gx,gy)).x;
        float flux_x2 = read_imagef(QFX2,sampler, int2(gx,gy)).x;
        float flux_y1 = read_imagef(QFY1,sampler, int2(gx,gy)).x;
        float flux_y2 = read_imagef(QFY2,sampler, int2(gx,gy)).x;

        float sflux_x1 = flux_x1 < 0.0f? min(C * sed,conc * flux_x1) : min(C * sed_x1,conc_x1 * flux_x1);
        float sflux_x2 = flux_x2 < 0.0f? min(C * sed,conc * flux_x2) : min(C * sed_x2,conc_x2 * flux_x2);
        float sflux_y1 = flux_y1 < 0.0f? min(C * sed,conc * flux_y1) : min(C * sed_y1,conc_y1 * flux_y1);
        float sflux_y2 = flux_y2 < 0.0f? min(C * sed,conc * flux_y2) : min(C * sed_y2,conc_y2 * flux_y2);

        sflux_x1 = isnan(sflux_x1)?0.0f:sflux_x1;
        sflux_x2 = isnan(sflux_x2)?0.0f:sflux_x2;
        sflux_y1 = isnan(sflux_y1)?0.0f:sflux_y1;
        sflux_y2 = isnan(sflux_y2)?0.0f:sflux_y2;

        float qfsout = 0.0f;
        qfsout = qfsout +z_x1 < -1000? sflux_x1 : 0.0f;
        qfsout = qfsout +z_x2 < -1000? sflux_x2 : 0.0f;
        qfsout = qfsout +z_y1 < -1000? sflux_y1 : 0.0f;
        qfsout = qfsout +z_y2 < -1000? sflux_y2 : 0.0f;
        qsedout += qfsout;

        sed = sed + sflux_x1 + sflux_x2 + sflux_y1 + sflux_y2;
        sed = sed + detachment - deposition;
        sed = max(0.0f,isnan(sed)?  0.0f:(isfinite(sed)? sed:0.0f));

        dettot = dettot + detachment;
        deptot = deptot + deposition + dep_force;

        //if channel is present, calculate channel interactions and channel flow
        //channel mask is also the ldd value
        int chmask = round(read_imagef(CH_MASK,sampler, int2(gx,gy)).x);
        if(chmask > 0 && chmask < 10)
        {
            float chflux_x1 = read_imagef(QFCHX1,sampler, int2(gx,gy)).x;
            float chflux_x2 = read_imagef(QFCHX2,sampler, int2(gx,gy)).x;

            //cell itself
            float ch_h = read_imagef(CH_H,sampler, int2(gx,gy)).x;
            float ch_height = read_imagef(CH_WIDTH,sampler, int2(gx,gy)).x;
            float ch_width = read_imagef(CH_HEIGHT,sampler, int2(gx,gy)).x;
            float ch_v = read_imagef(CH_V,sampler, int2(gx,gy)).x;
            float ch_vol = ch_h * ch_width * dx;
            float ch_conc = ch_sed / max(0.001f,ch_vol);

            //next cell in network
            int gx_next = min(dim0-(int)(1),max((int)(0),(int)(gx + ch_dx[chmask])));
            int gy_next = min(dim1-(int)(1),max((int)(0),(int)(gy + ch_dy[chmask])));

            float chn_z = read_imagef(DEM,sampler, int2(gx_next,gy_next)).x;
            float chn_h = read_imagef(CH_H,sampler, int2(gx_next,gy_next)).x;
            float chn_height = read_imagef(CH_WIDTH,sampler, int2(gx_next,gy_next)).x;
            float chn_width = read_imagef(CH_HEIGHT,sampler, int2(gx_next,gy_next)).x;
            float chn_v = read_imagef(CH_V,sampler, int2(gx_next,gy_next)).x;
            float chn_vol = chn_h * chn_width * dx;
            float chn_sed = read_imagef(CHSED,sampler, int2(gx_next,gy_next)).x;
            float chn_conc = chn_sed / max(0.001f,chn_vol);

            if(chmask == 5)
            {//allways flux out

                float chsflux_x2 = min(C * ch_sed,ch_conc * chflux_x2);
                ch_sed = ch_sed - chsflux_x2;
                qsedout +=chsflux_x2;
            }else
            {
                float chsflux_x2 = chflux_x2 > 0.0f? min(C * ch_sed,ch_conc * chflux_x2) : min(C * chn_sed,chn_conc * chflux_x2);
                ch_sed = ch_sed - chsflux_x2;
            }

            float hchtotal = 0.0;
            //previous cells in network
            for(int i = 1; i < 10; i++)
            {
                    if(i == 5) {continue;}
                    int gx_prev = min(dim0-(int)(1),max((int)(0),(int)(gx + ch_dx[i])));
                    int gy_prev = min(dim1-(int)(1),max((int)(0),(int)(gy + ch_dy[i])));
                    int lddp = round(read_imagef(CH_MASK,sampler, int2(gx_prev,gy_prev)).x);
                    float chp_z = read_imagef(DEM,sampler, int2(gx_next,gy_next)).x;
                    if(lddp > 0 && lddp < 10 && lddp != 5 && chp_z > -1000)
                    {
                            if(flow_to(gx_prev,gy_prev,gx,gy,lddp))
                            {
                                hchtotal += read_imagef(CH_H,sampler, int2(gx_prev,gy_prev)).x;
                            }
                   }
             }
            //previous cells in network
            for(int i = 1; i < 10; i++)
            {
                    if(i == 5)
                    {
                            continue;
                    }
                    int gx_prev = min(dim0-(int)(1),max((int)(0),(int)(gx + ch_dx[i])));
                    int gy_prev = min(dim1-(int)(1),max((int)(0),(int)(gy + ch_dy[i])));

                    int lddp = round(read_imagef(CH_MASK,sampler, int2(gx_prev,gy_prev)).x);
                    float chp_z = read_imagef(DEM,sampler, int2(gx_next,gy_next)).x;

                    if(lddp > 0 && lddp < 10 && lddp != 5 && chp_z > -1000)
                    {
                            if(flow_to(gx_prev,gy_prev,gx,gy,lddp))
                            {

                                float chp_h = read_imagef(CH_H,sampler, int2(gx_prev,gy_prev)).x;
                                float chp_height = read_imagef(CH_WIDTH,sampler, int2(gx_prev,gy_prev)).x;
                                float chp_width = read_imagef(CH_HEIGHT,sampler, int2(gx_prev,gy_prev)).x;
                                float chp_v = read_imagef(CH_V,sampler, int2(gx_prev,gy_prev)).x;
                                float chp_vol = chp_h * chp_width * dx;
                                float chp_sed = read_imagef(CHSED,sampler, int2(gx_prev,gy_prev)).x;
                                float chp_conc = chp_sed / max(0.001f,chp_vol);

                                float chsflux_x1 = chflux_x1 < 0.0f? min(C * ch_sed,ch_conc * chflux_x1) : min(C * chp_sed,chp_conc * chflux_x1);
                                ch_sed = ch_sed + (chp_h/max(0.001f,hchtotal))*chsflux_x1;
                            }
                   }
             }

            //////
            //Channel erosion and deposition
            //////

            float ch_detachment = 0.0f;
            float ch_deposition = 0.0f;

            //deposite anything above MAXCONC
            float ch_sed_max = ch_h *dx*dx * MAXCONC;
            float ch_dep_force = max(0.0f,ch_sed - ch_sed_max);
            if(ch_sed > ch_sed_max)
            {
                ch_sed = ch_sed_max;
            }
            ch_conc = ch_sed /(ch_vol);

            float ErosiveEfficiency = min(1.0f, 0.79f*exp(-0.85f*ch_Coh));
            //Y->Drc = std::min(1.0, 1.0/(0.89+0.56*CohesionSoil->Drc));
            //Y->Drc = std::min(1.0, 0.79*exp(-0.85*CohesionSoil->Drc));
            //Y->Drc = std::min(1.0, 1.0/(2.0*CohesionSoil->Drc));

            float sv = GetSV(d50);
            float TC = GetTC(ch_h, ch_v<0? -ch_v:ch_v, d50,d90,slope);

            float maxTC = max(TC - ch_conc,0.0f) ;
            // positive difference: TC deficit becomes detachment (ppositive)
            float minTC = min(TC - ch_conc,0.0f) ;

            //deposition based on settling velocity
            float TransportFactor = (1.0f-exp(-dt*sv/max(0.0001f,ch_h))) * ch_vol;
            ch_deposition = TransportFactor * minTC;
            ch_deposition = min(ch_deposition <0.0f? -deposition:deposition,((minTC * watervol)<0.0f?-1.0f:1.0f)*(minTC * watervol));

            TransportFactor = dt*sv * dx * dx;
            //correct detachment for grass strips, hard surfaces and houses
            ch_detachment = ErosiveEfficiency* maxTC * TransportFactor;
            ch_detachment = max(0.0f,min(ch_detachment,watervol * maxTC));

            ch_sed = ch_sed + ch_detachment - ch_deposition;
            ch_sed = max(0.0f,isnan(ch_sed)?  0.0f:(isfinite(ch_sed)? ch_sed:0.0f));

            dettot = dettot + ch_detachment;
            deptot = deptot + ch_deposition + ch_dep_force;

            //channel inflow and outflow
            float chflux_in = read_imagef(QFCHIN,sampler, int2(gx,gy)).x;

            if(chflux_in < 0)
            {//into channel
                float schflux_in = min(C * sed,conc * chflux_in);
                sed = sed + schflux_in;
                ch_sed = ch_sed - schflux_in;
            }else
            {//onto surface
                float schflux_in = min(C * ch_sed,ch_conc * chflux_in);
                sed = sed + schflux_in;
                ch_sed = ch_sed - schflux_in;
            }
        }
    }


   //////
   //Entrainment by solid phase
   //////

    float UF_ENTRAINMENTCCONSTANT = 1.0f;
    float UF_ENTRAINMENTCONSTANT = 0.5f;

    if(false)//materialdepth > 0.001f)
    {
        float MaxCSF = max(0.1f,min(0.8f,(UF_ENTRAINMENTCCONSTANT)*slope > tan(mifa)? 1.0f:(1000.0f * (UF_ENTRAINMENTCCONSTANT)*slope)/((mdens - 1000.0f)*(tan(mifa)-(UF_ENTRAINMENTCCONSTANT)*slope))));

        //shear stress
        float gamma = min(1.0f,mdens > 0.0001f? 1000.0f/mdens : 1.0f);
        float pbs = (1.0f-gamma)*(-9.81f * (hf+hs));
        //float dc = UF_DragCoefficient(_f/(_f+_s),_sc,gamma ,visc,rocksize,d);

        float ff = hf/max(0.0001f,hf+hs);
        float sf = hs/max(0.0001f,hf+hs);
        float sc = hs/max(0.0001f,hf);

        float t = 9.81f * (hf*1000.0f + hs * densitys) * ((ff*vel + sf*svel)*manning/(pow(max(0.1f,hf+hs),4.0f/3.0f)) + sf * mifa);

        float Coeff_Susp = 0.5f;

        //critical shear stress
        float tc = (Coh + (1.0f-Coeff_Susp) *sf * (mdens - 1000.0f) * 9.81f * (hf+hs) * (cos(slope)*cos(slope) * tan(mifa)));

        //get the actual scouring rate
        float scourat = max(0.0f,1.0f * UF_ENTRAINMENTCONSTANT * (t- 1.0f*tc));
        //get entrainment in cubic meters
        float entrainment = max(0.0f,min(0.5f * (MaxCSF - sc)*(dx*dx) * (hf+hs),scourat *(dx*dx)*dt));

        if((hf + hs) < 0.1f)
        {
            entrainment = 0.0f;
        }
        //returns volume of entrainment
        entrainment = min(0.5f * depth_available,max(0.0f,entrainment));
        SDChange += entrainment;
        enttot = enttot + entrainment;

        //////
        //Update Solid Phase properties
        //////

        float sadd = entrainment * 0.6f;
        float fadd = entrainment * 0.4f;

        ifas = (ifas * hs + max(0.0f,sadd) * mifa)/(max(0.0001f,hs + sadd));
        rocksizes = (rocksizes * hs + max(0.0f,sadd) * mrocksize)/(max(0.0001f,hs + sadd));
        densitys = (densitys * hs + max(0.0f,sadd) * mdens)/(max(0.0001f,hs + sadd));

        hs = hs + entrainment * 0.6f;
        hf = hf + entrainment * 0.4f;
    }



    //////
    //Update soil depth and ground water properties
    //////

    sd = max(0.01f,sd - SDChange);
    float frac_left = 1.0f-(SDChange/max(0.01f,sd));
    float frac_add = SDChange/max(0.01f,sd);

    float gw_add = frac_add *(gwh + ush + wf);
    gwh = frac_left * gwh;
    ush = frac_left * ush;
    wf = frac_left * wf;

    //hf = hf + gw_add;

    //////
    //output to other kernel
    //////
    write_imagef(HN, int2(gx,gy), hf);
    write_imagef(VFxN, int2(gx,gy), vfx);
    write_imagef(VFyN, int2(gx,gy), vfy);
    write_imagef(HSN, int2(gx,gy), hs);
    write_imagef(VSxN, int2(gx,gy), vsx);
    write_imagef(VSyN, int2(gx,gy), vsy);
    write_imagef(SRocksizeN, int2(gx,gy), rocksizes);
    write_imagef(SDensityN, int2(gx,gy),densitys);
    write_imagef(SIfaN, int2(gx,gy), ifas);

    write_imagef(MDN, int2(gx,gy), materialdepth);
    write_imagef(SDN, int2(gx,gy), sd);
    write_imagef(GW_SN, int2(gx,gy), gwh);
    write_imagef(GW_USN, int2(gx,gy), ush);
    write_imagef(GW_WFN, int2(gx,gy), wf);

    write_imagef(ENTRAINMENTOTN, int2(gx,gy), enttot);
    write_imagef(CHSEDN, int2(gx,gy), ch_sed);
    write_imagef(DETTotN, int2(gx,gy), dettot);
    write_imagef(DEPTotN, int2(gx,gy), deptot);
    write_imagef(SEDN, int2(gx,gy), sed);

    write_imagef(QSEDOUTN, int2(gx,gy),qsedout);

    //////
    //Output for UI
    //////

    float ui = 0;
    if(ui_image == 25)
    {
            ui = dettot - deptot;
            write_imagef(OUTPUT_UI, int2(gx,gy), ui);
    }else if(ui_image == 26)
    {
            ui = conc;
            write_imagef(OUTPUT_UI, int2(gx,gy), ui);
    }else if(ui_image == 27)
    {
            ui = sed + ch_sed;
            write_imagef(OUTPUT_UI, int2(gx,gy), ui);
    }else if(ui_image == 28)
    {
            ui = 3.0f;
            write_imagef(OUTPUT_UI, int2(gx,gy), ui);
    }

#ifdef LISEM_KERNEL_CPUCODE
}
#endif

}


kernel
void sediment2(
#ifdef LISEM_KERNEL_CPUCODE
int id_1d_cpum,
#endif

int dim0,
                                int dim1,
                                float dx,
                                float dt,
                                __read_only image2d_t LOCX,
                                __read_only image2d_t LOCY,
                                __write_only image2d_t DEM,
                                __write_only image2d_t H,
                                __write_only image2d_t Vx,
                                __write_only image2d_t Vy,
                                __write_only image2d_t HS,
                                __write_only image2d_t VSx,
                                __write_only image2d_t VSy,
                                __write_only image2d_t SIfa,
                                __write_only image2d_t SRocksize,
                                __write_only image2d_t SDensity,
                                __read_only image2d_t CH_H,
                                __read_only image2d_t CH_V,
                                __write_only image2d_t CH_HS,
                                __write_only image2d_t CH_VS,
                                __write_only image2d_t CH_SIfa,
                                __write_only image2d_t CH_SRocksize,
                                __write_only image2d_t CH_SDensity,
                                __read_only image2d_t CH_MASK,
                                __read_only image2d_t CH_WIDTH,
                                __read_only image2d_t CH_HEIGHT,
                                __read_only image2d_t D50,
                                __read_only image2d_t D90,
                                __read_only image2d_t COH,
                                __read_only image2d_t CHCOH,
                                __read_only image2d_t Y,
                                __write_only image2d_t SD,
                                __write_only image2d_t MD,
                                __read_only image2d_t MIFA,
                                __read_only image2d_t MROCKSIZE,
                                __read_only image2d_t MDENS,
                                __write_only image2d_t SED,
                                __write_only image2d_t CHSED,
                                __write_only image2d_t DETTot,
                                __write_only image2d_t DEPTot,
                                __write_only image2d_t F_InfilAct,
                                __write_only image2d_t GW_US,
                                __write_only image2d_t GW_S,
                                __write_only image2d_t ENTRAINMENTOT,
                                __read_only image2d_t GW_WFN,
                                __read_only image2d_t GW_USN,
                                __read_only image2d_t GW_SN,
                                __read_only image2d_t SEDN,
                                __read_only image2d_t CHSEDN,
                                __read_only image2d_t DETTotN,
                                __read_only image2d_t DEPTotN,
                                __read_only image2d_t SDN,
                                __read_only image2d_t MDN,
                                __read_only image2d_t ENTRAINMENT,
                                __read_only image2d_t ENTRAINMENTCH,
                                __read_only image2d_t ENTRAINMENTOTN,
                                __read_only image2d_t DEMN,
                                __write_only image2d_t QSEDOUT,
                                __read_only image2d_t QSEDOUTN,
                                __write_only image2d_t OUTPUT_UI,
                                int ui_image,
                                int do_addsolids,
                                __read_only image2d_t HSN,
                                __read_only image2d_t VSxN,
                                __read_only image2d_t VSyN,
                                __read_only image2d_t SIfaN,
                                __read_only image2d_t SRocksizeN,
                                __read_only image2d_t SDensityN,
                                __read_only image2d_t CH_HSN,
                                __read_only image2d_t CH_VSN,
                                __read_only image2d_t CH_SIfaN,
                                __read_only image2d_t CH_SRocksizeN,
                                __read_only image2d_t CH_SDensityN,
                                __read_only image2d_t HN,
                                __read_only image2d_t VFxN,
                                __read_only image2d_t VFyN
                                )
{

#ifdef LISEM_KERNEL_CPUCODE

#pragma omp parallel for
for(int id_1d_cpu = 0; id_1d_cpu < id_1d_cpum; id_1d_cpu++)
{
#endif

    float tx = dt/dx;

    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

    const int id_1d = (int)(get_global_id(0));

    const int idx = trunc((float)(id_1d)/(float)(dim1));
    const int idy = id_1d % dim1;

    const int gx_x = min(dim0-(int)(1),max((int)(0),(int)(idx)));
    const int gy_y = min(dim1-(int)(1),max((int)(0),(int)(idy)));

    int x = trunc(read_imagef(LOCX,sampler, int2(gx_x,gy_y)).x);
    int y = trunc(read_imagef(LOCY,sampler, int2(gx_x,gy_y)).x);

    const int gy = min(dim1-(int)(1),max((int)(0),(int)(x)));
    const int gx = min(dim0-(int)(1),max((int)(0),(int)(y)));

    const int gx_x1 = min(dim0-(int)(1),max((int)(0),(int)(gx - 1 )));
    const int gy_x1 = gy;
    const int gx_x2 = min(dim0-(int)(1),max((int)(0),(int)(gx + 1)));
    const int gy_x2 = gy;
    const int gx_y1 = gx;
    const int gy_y1 = min(dim1-(int)(1),max((int)(0),(int)(gy - 1)));
    const int gx_y2 = gx;
    const int gy_y2 = min(dim1-(int)(1),max((int)(0),(int)(gy + 1)));

    float z = read_imagef(DEMN,sampler, int2(gx,gy)).x;
    float sedn = read_imagef(SEDN,sampler, int2(gx,gy)).x;
    float chsedn = read_imagef(CHSEDN,sampler, int2(gx,gy)).x;
    float deptot = read_imagef(DEPTotN,sampler, int2(gx,gy)).x;
    float dettot = read_imagef(DETTotN,sampler, int2(gx,gy)).x;
    float enttot = read_imagef(ENTRAINMENTOTN,sampler, int2(gx,gy)).x;

    float materialdepth = read_imagef(MDN,sampler, int2(gx,gy)).x;
    float sd = read_imagef(SDN,sampler, int2(gx,gy)).x;
    float gwh = read_imagef(GW_SN,sampler, int2(gx,gy)).x;
    float ush = read_imagef(GW_USN,sampler, int2(gx,gy)).x;
    float wf = read_imagef(GW_WFN,sampler, int2(gx,gy)).x;

    float h = read_imagef(HN,sampler, int2(gx,gy)).x;

    float hs = read_imagef(HSN,sampler, int2(gx,gy)).x;
    float vsx = read_imagef(VSxN,sampler, int2(gx,gy)).x;
    float vsy = read_imagef(VSyN,sampler, int2(gx,gy)).x;
    float vfx = read_imagef(VFxN,sampler, int2(gx,gy)).x;
    float vfy = read_imagef(VFyN,sampler, int2(gx,gy)).x;

    float rocksizes = read_imagef(SRocksizeN,sampler, int2(gx,gy)).x;
    float densitys =read_imagef(SDensityN,sampler, int2(gx,gy)).x;
    float ifas =read_imagef(SIfaN,sampler, int2(gx,gy)).x;

    float qsedout =read_imagef(QSEDOUTN,sampler, int2(gx,gy)).x;

    write_imagef(H, int2(gx,gy), h);
    write_imagef(Vx, int2(gx,gy), vfx);
    write_imagef(Vy, int2(gx,gy), vfy);
    write_imagef(HS, int2(gx,gy), hs);
    write_imagef(VSx, int2(gx,gy), vsx);
    write_imagef(VSy, int2(gx,gy), vsy);
    write_imagef(SRocksize, int2(gx,gy), rocksizes);
    write_imagef(SDensity, int2(gx,gy),densitys);
    write_imagef(SIfa, int2(gx,gy), ifas);

    write_imagef(MD, int2(gx,gy), materialdepth);
    write_imagef(SD, int2(gx,gy), sd);
    write_imagef(GW_S, int2(gx,gy), gwh);
    write_imagef(GW_US, int2(gx,gy), ush);
    write_imagef(F_InfilAct, int2(gx,gy), wf);

    write_imagef(ENTRAINMENTOT, int2(gx,gy), enttot);
    write_imagef(DETTot, int2(gx,gy), dettot);
    write_imagef(DEPTot, int2(gx,gy), deptot);
    write_imagef(SED, int2(gx,gy), sedn);
    write_imagef(CHSED, int2(gx,gy), chsedn);

    write_imagef(QSEDOUT, int2(gx,gy),qsedout);

#ifdef LISEM_KERNEL_CPUCODE
}
#endif
}


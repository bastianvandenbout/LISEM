
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

//int dx[10] = {0, -1, 0, 1, -1, 0, 1, -1, 0, 1};
//int dy[10] = {0, 1, 1, 1, 0, 0, 0, -1, -1, -1};

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


nonkernel float3 F_HLL2(float h_L,float u_L,float v_L,float h_R,float u_R,float v_R)
{
		float3 ret;
		
		//h_L = max(0.0001f,h_L);
		//h_R = max(0.0001f,h_R);
		
		float grav_h_L = GRAV*h_L;
        float grav_h_R = GRAV*h_R;
        float sqrt_grav_h_L = sqrt((float)(grav_h_L));  // wave velocity
        float sqrt_grav_h_R = sqrt((float)(grav_h_R));
        float q_R = u_R*h_R;
        float q_L = u_L*h_L;

        float sc1 = min((float)(u_L - sqrt_grav_h_L),(float)(u_R - sqrt_grav_h_R));
        float sc2 = max((float)(u_L + sqrt_grav_h_L),(float)(u_R + sqrt_grav_h_R)); 
        float tmp = 1.0f/max((float)0.1f,(float)(sc2-sc1));
        float t1 = (min((float)(sc2),(float)(0.0f)) - min((float)(sc1),(float)(0.0f)))*tmp;
        float t2 = 1.0f - t1;
        float t3_1 = fabs((float)(sc1));
		float t3_2 = fabs((float)(sc2));
                float t3 =(sc2* t3_1 - sc1* t3_2)*0.5f*tmp;
		
        ret.x = t1*q_R + t2*q_L - t3*(h_R - h_L);
        ret.y = t1*(q_R*u_R + grav_h_R*h_R*0.5f) + t2*(q_L*u_L + grav_h_L*h_L*0.5f) - t3*(q_R - q_L);
        ret.z = t1*q_R*v_R + t2*q_L*v_L - t3*(h_R*v_R - h_L*v_L);
		
		return ret;
}

#endif


nonkernel float3 F_HLL2S(float h_L,float u_L,float v_L,float h_R,float u_R,float v_R)
{
		float3 ret;
		
		//h_L = max(0.0001f,h_L);
		//h_R = max(0.0001f,h_R);
		
		float grav_h_L = GRAV*h_L;
        float grav_h_R = GRAV*h_R;
        float sqrt_grav_h_L = sqrt((float)(grav_h_L));  // wave velocity
        float sqrt_grav_h_R = sqrt((float)(grav_h_R));
        float q_R = u_R*h_R;
        float q_L = u_L*h_L;

        float sc1 = min((float)(u_L - sqrt_grav_h_L),(float)(u_R - sqrt_grav_h_R));
        float sc2 = max((float)(u_L + sqrt_grav_h_L),(float)(u_R + sqrt_grav_h_R)); 
        float tmp = 1.0f/max((float)0.1f,(float)(sc2-sc1));
        float t1 = (min((float)(sc2),(float)(0.f)) - min((float)(sc1),(float)(0.f)))*tmp;
        float t2 = 1.0f - t1;
        float t3_1 = fabs((float)(sc1));
		float t3_2 = fabs((float)(sc2));
                float t3 =(sc2* t3_1 - sc1* t3_2)*0.5f*tmp;
		
        ret.x = t1*q_R + t2*q_L - t3*(h_R - h_L);
        ret.y = t1*(q_R*u_R + grav_h_R*h_R*0.5f) + t2*(q_L*u_L + grav_h_L*h_L*0.5f) - t3*(q_R - q_L);
        ret.z = t1*q_R*v_R + t2*q_L*v_L - t3*(h_R*v_R - h_L*v_L);
		
		return ret;
}


nonkernel float3 F_HLL2FS(float h_L,float ho_L,float u_L,float v_L,float h_R,float ho_R,float u_R,float v_R,float sifa,float srocksize,float sdensity,float dragmult)
{
    float3 retnw;


    float c = max(fabs(u_L),fabs(u_R));
    float cd = c*0.5f;
    float q_R = u_R*h_R;
    float q_L = u_L*h_L;
    retnw.x = (q_L+q_R)*0.5f-cd*(h_R-h_L);
    retnw.y = ((u_L*q_L)+(u_R*q_R))*0.5f-cd*(q_R-q_L);
    retnw.z = (q_L*v_L+q_R*v_R)*0.5f-cd*(h_R*v_R-h_L*v_L);



    float3 ret;

        //force ratio to approximate the influence of the wave-velocity in riemann solution
        float ff_dev_L = max(0.001f,h_L/max(0.0001f,(h_L + ho_L)));
        float sf_dev_L = max(0.001f,ho_L/max(0.0001f,(h_L + ho_L)));
        float dc_L = dragmult * sf_dev_L * ff_dev_L * (1.0/(max(0.0001f,srocksize)))* max(0.0f,sdensity-1000.0f)/max(0.1f,1000.0f);//den > 0.0f? 100.0*ff_dev * sf_dev* (1.0f - (1.0f/max(1.0f,density)))/den : 0.0f;
        float fac_dc_L = max(0.0f,1.0f - (4.0f *(1.0f-min(1.0f,max(max(0.0f,(ff_dev_L)),(exp(-(dc_L /ff_dev_L)) + ((sf_dev_L *sdensity)/(1000.0f))*tan(sifa))/tan(sifa))))));

        float ff_dev_R = max(0.001f,h_R/max(0.0001f,(h_R + ho_R)));
        float sf_dev_R = max(0.001f,ho_R/max(0.0001f,(h_R + ho_R)));
        float dc_R = dragmult * sf_dev_R * ff_dev_R * (1.0f/(max(0.0001f,srocksize)))* max(0.0f,sdensity-1000.0f)/max(0.1f,1000.0f);//den > 0.0f? 100.0*ff_dev * sf_dev* (1.0f - (1.0f/max(1.0f,density)))/den : 0.0f;
        float fac_dc_R = max(0.0f,1.0f - (4.0f *(1.0f-min(1.0f,max(max(0.0f,(ff_dev_R)),(exp(-(dc_R /ff_dev_R)) + ((sf_dev_R *sdensity)/(1000.0f))*tan(sifa))/tan(sifa))))));

        //and pressure-momentum based on the solid content
        float grav_h_L = GRAV*h_L;//*  pow(min(1.0f,max(0.0f,(1.0f-1.25f*sf_dev_L))),4.0f);
        float grav_h_R = GRAV*h_R;//*  pow(min(1.0f,max(0.0f,(1.0f-1.25f*sf_dev_R))),4.0f) ;
        float sqrt_grav_h_L = sqrt((float)(grav_h_L)) * fac_dc_L;  // wave velocity
        float sqrt_grav_h_R = sqrt((float)(grav_h_R)) * fac_dc_R ;
        q_R = u_R*h_R;
        q_L = u_L*h_L;

        float sc1 = min((float)(u_L - sqrt_grav_h_L),(float)(u_R - sqrt_grav_h_R));
        float sc2 = max((float)(u_L + sqrt_grav_h_L),(float)(u_R + sqrt_grav_h_R));
        float tmp = 1.0f/max((float)0.1f,(float)(sc2-sc1));
        float t1 = (min((float)(sc2),(float)(0.)) - min((float)(sc1),(float)(0.f)))*tmp;
        float t2 = 1.0f - t1;
        float t3_1 = fabs((float)(sc1));
        float t3_2 = fabs((float)(sc2));
        float t3 =(sc2* t3_1 - sc1* t3_2)*0.5f*tmp;

        ret.x = t1*q_R + t2*q_L - t3*(h_R - h_L);
        ret.y = t1*(q_R*u_R + grav_h_R*h_R*0.5f) + t2*(q_L*u_L + grav_h_L*h_L*0.5f) - t3*(q_R - q_L);
        ret.z = t1*q_R*v_R + t2*q_L*v_L - t3*(h_R*v_R - h_L*v_L);

                return ret;
}



nonkernel float3 F_HLL2SF(float h_L,float ho_L,float u_L,float v_L,float h_R,float ho_R,float u_R,float v_R,float sifa,float srocksize,float sdensity,float dragmult)
{
        float3 retnw;
		
		
		float c = max(fabs(u_L),fabs(u_R));
        float cd = c*0.5f;
        float q_R = u_R*h_R;
        float q_L = u_L*h_L;
        retnw.x = (q_L+q_R)*0.5f-cd*(h_R-h_L);
        retnw.y = ((u_L*q_L)+(u_R*q_R))*0.5f-cd*(q_R-q_L);
        retnw.z = (q_L*v_L+q_R*v_R)*0.5f-cd*(h_R*v_R-h_L*v_L);



        float3 ret;

        //h_L = max(0.0001f,h_L);
        //h_R = max(0.0001f,h_R);


        //force ratio to approximate the influence of the wave-velocity in riemann solution
        float ff_dev_L = max(0.001f,ho_L/max(0.0001f,(h_L + ho_L)));
        float sf_dev_L = max(0.001f,h_L/max(0.0001f,(h_L + ho_L)));
        float dc_L = dragmult * sf_dev_L * ff_dev_L * (1.0/(max(0.0001f,srocksize)))* max(0.0f,sdensity-1000.0f)/max(0.1f,1000.0f);//den > 0.0f? 100.0*ff_dev * sf_dev* (1.0f - (1.0f/max(1.0f,density)))/den : 0.0f;
        float fac_dc_L = max(0.0f,1.0f - (4.0f *(1.0f-min(1.0f,max(max(0.0f,(ff_dev_L)),(exp(-(dc_L /ff_dev_L)) + ((sf_dev_L *sdensity)/(1000.0f))*tan(sifa))/tan(sifa))))));

        float ff_dev_R = max(0.001f,ho_R/max(0.0001f,(h_R + ho_R)));
        float sf_dev_R = max(0.001f,h_R/max(0.0001f,(h_R + ho_R)));
        float dc_R = dragmult * sf_dev_R * ff_dev_R * (1.0f/(max(0.0001f,srocksize)))* max(0.0f,sdensity-1000.0f)/max(0.1f,1000.0f);//den > 0.0f? 100.0*ff_dev * sf_dev* (1.0f - (1.0f/max(1.0f,density)))/den : 0.0f;
        float fac_dc_R = max(0.0f,1.0f - (4.0f *(1.0f-min(1.0f,max(max(0.0f,(ff_dev_R)),(exp(-(dc_R /ff_dev_R)) + ((sf_dev_R *sdensity)/(1000.0f))*tan(sifa))/tan(sifa))))));

        //and pressure-momentum based on the solid content
        float grav_h_L = GRAV*h_L;//*  pow(min(1.0f,max(0.0f,(1.0f-1.25f*sf_dev_L))),4.0f);
        float grav_h_R = GRAV*h_R;//*  pow(min(1.0f,max(0.0f,(1.0f-1.25f*sf_dev_R))),4.0f) ;
        float sqrt_grav_h_L = sqrt((float)(grav_h_L)) * fac_dc_L;  // wave velocity
        float sqrt_grav_h_R = sqrt((float)(grav_h_R)) * fac_dc_R ;
        q_R = u_R*h_R;
        q_L = u_L*h_L;

        float sc1 = min((float)(u_L - sqrt_grav_h_L),(float)(u_R - sqrt_grav_h_R));
        float sc2 = max((float)(u_L + sqrt_grav_h_L),(float)(u_R + sqrt_grav_h_R));
        float tmp = 1.0f/max((float)0.1f,(float)(sc2-sc1));
        float t1 = (min((float)(sc2),(float)(0.)) - min((float)(sc1),(float)(0.f)))*tmp;
        float t2 = 1.0f - t1;
        float t3_1 = fabs((float)(sc1));
        float t3_2 = fabs((float)(sc2));
        float t3 =(sc2* t3_1 - sc1* t3_2)*0.5f*tmp;

        ret.x = t1*q_R + t2*q_L - t3*(h_R - h_L);
        ret.y = t1*(q_R*u_R + grav_h_R*h_R*0.5f) + t2*(q_L*u_L + grav_h_L*h_L*0.5f) - t3*(q_R - q_L);
        ret.z = t1*q_R*v_R + t2*q_L*v_L - t3*(h_R*v_R - h_L*v_L);

        return ret;
}




nonkernel float GetVNFX(float v,float hn, float dt, float dx, float n,
			   float z, float z_x1, float z_x2, float z_y1, float z_y2,
			   float hs, float hs_x1, float hs_x2, float hs_y1, float hs_y2,
			   float us, float us_x1, float us_x2, float us_y1, float us_y2,
			   float vs, float vs_x1, float vs_x2, float vs_y1, float vs_y2,
			   float us_x1y1, float us_x1y2, float us_x2y1, float us_x2y2,
			   float vs_x1y1, float vs_x1y2, float vs_x2y1, float vs_x2y2,
			   float hf, float hf_x1, float hf_x2, float hf_y1, float hf_y2,
			   float uf, float uf_x1, float uf_x2, float uf_y1, float uf_y2,
			   float vf, float vf_x1, float vf_x2, float vf_y1, float vf_y2,
			   float uf_x1y1, float uf_x1y2, float uf_x2y1, float uf_x2y2,
			   float vf_x1y1, float vf_x1y2, float vf_x2y1, float vf_x2y2,
                           float ifa, float rocksize, float density, float dragmult)
{			   
			   
	float vn = v;
        float vnold = v;

	float hsurf = z +hs + hf;
	float hsurfs = z +hs;
        float sx_zh_x2 = min((float)(0.5f),max((float)(-0.5f),(float)((z_x2 + hs_x2 + hf_x2-hsurf)/dx)));
        float sy_zh_y1 = min((float)(0.5f),max((float)(-0.5f),(float)((hsurf-z_y1 - hs_y1 - hf_y1)/dx)));
        float sx_zh_x1 = min((float)(0.5f),max((float)(-0.5f),(float)((hsurf-z_x1 - hs_x1 - hf_x1)/dx)));
        float sy_zh_y2 = min((float)(0.5f),max((float)(-0.5f),(float)((z_y2 + hs_y2 + hf_y2-hsurf)/dx)));
	
	float sx_zh = min(1.0f,max(-1.0f,minmod(sx_zh_x1,sx_zh_x2)));
	float sy_zh = min(1.0f,max(-1.0f,minmod(sy_zh_y1,sy_zh_y2)));
	
	
        if(hs < 0.1f || (hs < 0.1f && hs_x1 < 0.01f && hs_x2 < 0.01f && hs_y1 < 0.01f && hs_y2 < 0.01f))
	{
		float acc_x = -0.5 * GRAV * sx_zh;
		
		vn = vn + acc_x * dt;
	
	}else
	{
	
		float dx2 =(dx*dx);
			
		float h_dev = max(0.001f,hs + hf);
		float ff_dev = max(0.001f,hf/max(0.001f,(hf + hs)));
		float sf_dev = max(0.001f,hs/max(0.001f,(hf + hs)));
		float ff = max(0.0f,hf/max(0.001f,(hf + hs)));
		float sf = max(0.0f,hs/max(0.001f,(hf + hs)));
		
		float ddemhdx = minmod((z_x2 + hs_x2 + hf_x2 - z + hs + hf)/dx,(z + hs + hf - z_x1 - hs_x1 - hf_x1)/dx);
		
		float ddfudxx = (uf_x2 - 2.0f * uf + uf_x1)/(dx2);
		float ddfvdxy = (vf_x1y1 + vf_x2y2 - vf_x1y2 - vf_x2y1)/(4.0f*dx2);
		float ddfudyy = (uf_y2 - 2.0f * uf + uf_y1)/(dx2);

		float sf_x1 = hs_x1/max(0.001f,(hf_x1 + hs_x1));
		float sf_x2 = hs_x2/max(0.001f,(hf_x2 + hs_x2));
		float sf_y1 = hs_y1/max(0.001f,(hf_y1 + hs_y1));
		float sf_y2 = hs_y2/max(0.001f,(hf_y2 + hs_y2));
		
		float dsfdx = 0.5f * (sf_x2 - sf_x1)/dx;
		float dsfdy = 0.5f * (hs_y2/max(0.001f,(hf_y2 + hs_y2)) - hs_y1/max(0.001f,(hf_y1 + hs_y1)))/dx;
		
		float dfudx = 0.5f*(uf_x2 - uf_x1)/dx;
		float dsudx = 0.5f*(us_x2 - us_x1)/dx;
		float dfvdy = 0.5f*(vf_y2 - vf_y1)/dx;
		float dsvdy = 0.5f*(vs_y2 - vs_y1)/dx;
		float dfudy = 0.5f*(uf_y2 - uf_y1)/dx;
		float dsudy = 0.5f*(us_y2 - us_y1)/dx;
		
		float ddsfdxx = (sf_x2 - 2.0f * sf_dev + sf_x1)/(dx2);
		float ddsfdxy = 0.0f;
		float ddsfdyy = (sf_y2 - 2.0f * sf_dev + sf_y1)/(dx2);
			
		float UF_Aspect = 1.0f;
		float UF_Ksi = 3.0f;
		float UF_Chi = 3.0f;
		float UF_j = 2.0f;
		float gamma = 1000.0f/density;
		float UT = 0.1f;
		float visc = min(100000.0f,max(1.0f,1.0f * exp(20.0f * max(0.0f,min(1.0f,sf_dev)))));
		float Nr = 10000.0f;
		float Nra = 10000.0f;	
		float F = (gamma/180.0f)* pow(ff_dev/sf_dev,3.0f) * Nr;
		float G = pow(ff_dev,2.5f);
		float P = sf_dev;
		float den = pow((UF_Aspect * UT*( P * F + (1.0f-P) * G) + (P/sf_dev + (1.0f-P)/ff_dev) * 10.0f),UF_j);
                float dc = dragmult * sf_dev * ff_dev * (1.0/(max(0.0001f,rocksize)))* max(0.0f,density-1000.0f)/max(0.1f,1000.0f);//den > 0.0f? 100.0*ff_dev * sf_dev* (1.0f - (1.0f/max(1.0f,density)))/den : 0.0f;
		float aspect = 1.0;
		

        float acc_x =
				(-GRAV * sx_zh
						-UF_Aspect
						*(
                                                        -(1.0f/(ff_dev * Nr))*(
                                                                2.0f*ddfudxx + ddfvdxy + ddfudyy
								)
                                                        +(1.0f/(ff_dev * Nra))*(
                                                                2.0f *(dsfdx*(dfudx - dsudx) + ddsfdxx*(uf - us))
								+(dsfdx*(dfvdy - dsvdy) + ddsfdxy*(vf - vs))
								+(dsfdy*(dfudy - dsudy) + ddsfdyy*(uf - us))
								+ UF_Chi * uf/(UF_Aspect*UF_Aspect*ff_dev*Nra*h_dev*h_dev)
								)
							-(UF_Ksi*sf_dev*(uf - us)/(UF_Aspect*UF_Aspect*ff_dev*Nra*h_dev*h_dev))
							)
						);
		
		vn = vn + acc_x * dt;
		
		//drag
		float vpow = pow((vn - us) * (vn - us) + (vf - vs) * (vf - vs),UF_j - 1.0f);
		float lfacu = max(0.0f,dt * dc);
                float fac_dc = min(1.0f,max(0.0f,(exp(-(dc * vpow/ff_dev)) + ((sf_dev *density)/(1000.0f))*tan(ifa))/tan(ifa)));
                float v_balance = (1.0f - fac_dc) *(ff * vn + sf * us) + (fac_dc) * us;
		vn = v_balance + (vn - v_balance) * exp(-lfacu);
	}

        {
            float vsq = sqrt((float)(uf * uf + vf * vf));
            float nsq1 = (0.001+n)*(0.001+n)*GRAV/max(0.01f,pow((float)(hn),(float)(4.0/3.0)));
            float nsq = nsq1*vsq*dt;
            vn = (float)((vn/(1.0f+nsq)));
        }

        float threshold = 0.01 * dx;
        if(hn < threshold)
        {
            float kinfac = max(0.0f,(threshold - hn) / (0.025f * dx));
            float acc_eff = (vn -vnold)/max(0.0001f,dt);

            float v_kin = (sx_zh>0?1:-1) * hn * sqrt(hn) * max(0.001f,sqrt(sx_zh>0?sx_zh:-sx_zh))/(0.001f+n);

            vn = kinfac * v_kin + vn*(1.0f-kinfac);

        }



	return vn;
}
nonkernel float GetVNFY(float v,float hn, float dt, float dx, float n,
			   float z, float z_x1, float z_x2, float z_y1, float z_y2,
			   float hs, float hs_x1, float hs_x2, float hs_y1, float hs_y2,
			   float us, float us_x1, float us_x2, float us_y1, float us_y2,
			   float vs, float vs_x1, float vs_x2, float vs_y1, float vs_y2,
			   float us_x1y1, float us_x1y2, float us_x2y1, float us_x2y2,
			   float vs_x1y1, float vs_x1y2, float vs_x2y1, float vs_x2y2,
			   float hf, float hf_x1, float hf_x2, float hf_y1, float hf_y2,
			   float uf, float uf_x1, float uf_x2, float uf_y1, float uf_y2,
			   float vf, float vf_x1, float vf_x2, float vf_y1, float vf_y2,
			   float uf_x1y1, float uf_x1y2, float uf_x2y1, float uf_x2y2,
			   float vf_x1y1, float vf_x1y2, float vf_x2y1, float vf_x2y2,
                           float ifa, float rocksize, float density, float dragmult)
{
	float vn = v;
        float vnold = v;
	float hsurf = z +hs + hf;
	float hsurfs = z +hs;
        float sx_zh_x2 = min((float)(0.5f),max((float)(-0.5f),(float)((z_x2 + hs_x2 + hf_x2-hsurf)/dx)));
        float sy_zh_y1 = min((float)(0.5f),max((float)(-0.5f),(float)((hsurf-z_y1 - hs_y1 - hf_y1)/dx)));
        float sx_zh_x1 = min((float)(0.5f),max((float)(-0.5f),(float)((hsurf-z_x1 - hs_x1 - hf_x1)/dx)));
        float sy_zh_y2 = min((float)(0.5f),max((float)(-0.5f),(float)((z_y2 + hs_y2 + hf_y2-hsurf)/dx)));
	
	float sx_zh = min(1.0f,max(-1.0f,minmod(sx_zh_x1,sx_zh_x2)));
	float sy_zh = min(1.0f,max(-1.0f,minmod(sy_zh_y1,sy_zh_y2)));
	
        if(hs < 0.1f || (hs < 0.1f && hs_x1 < 0.01f && hs_x2 < 0.01f && hs_y1 < 0.01f && hs_y2 < 0.01f))
	{
		float acc_y = -0.5 * GRAV * sy_zh;
		
		vn = vn + acc_y * dt;
	
	}else
	{
	
		float dx2 =(dx*dx);
			
		float h_dev = max(0.001f,hs + hf);
		float ff_dev = max(0.001f,hf/max(0.001f,(hf + hs)));
		float sf_dev = max(0.001f,hs/max(0.001f,(hf + hs)));
		float ff = max(0.0f,hf/max(0.001f,(hf + hs)));
		float sf = max(0.0f,hs/max(0.001f,(hf + hs)));

		float ddfvdxx = (vf_x2 - 2.0f * vf + vf_x1)/(dx2);
		float ddfudxy = (uf_x1y1 + uf_x2y2 - uf_x1y2 - uf_x2y1)/(4.0f*dx2);
		float ddfvdyy = (vf_y2 - 2.0f * vf + vf_y1)/(dx2);

		float sf_x1 = hs_x1/max(0.001f,(hf_x1 + hs_x1));
		float sf_x2 = hs_x2/max(0.001f,(hf_x2 + hs_x2));
		float sf_y1 = hs_y1/max(0.001f,(hf_y1 + hs_y1));
		float sf_y2 = hs_y2/max(0.001f,(hf_y2 + hs_y2));
		
		float dsfdx = 0.5f * (sf_x2 - sf_x1)/dx;
		float dsfdy = 0.5f * (hs_y2/max(0.001f,(hf_y2 + hs_y2)) - hs_y1/max(0.001f,(hf_y1 + hs_y1)))/dx;
		
		float dfvdy = 0.5f*(vf_y2 - vf_y1)/dx;
		float dsvdy = 0.5f*(vs_y2 - vs_y1)/dx;
		float dfvdx = 0.5f*(vf_x2 - vf_x1)/dx;
		float dsvdx = 0.5f*(vs_x2 - vs_x1)/dx;
		float dfudx = 0.5f*(uf_x2 - uf_x1)/dx;
		float dsudx = 0.5f*(us_x2 - us_x1)/dx;
		
		float ddsfdxx = (sf_x2 - 2.0f * sf_dev + sf_x1)/(dx2);
		float ddsfdxy = 0.0f;
		float ddsfdyy = (sf_y2 - 2.0f * sf_dev + sf_y1)/(dx2);
			
		float UF_Aspect = 1.0f;
		float UF_Ksi = 3.0f;
		float UF_Chi = 3.0f;
		float UF_j = 2.0f;
		float gamma = 1000.0f/density;
		float UT = 0.1f;
		float visc = min(100000.0f,max(1.0f,1.0f * exp(20.0f * max(0.0f,min(1.0f,sf_dev)))));
		float Nr = 10000.0f;
		float Nra = 10000.0f;	
		float F = (gamma/180.0f)* pow(ff_dev/sf_dev,3.0f) * Nr;
		float G = pow(ff_dev,2.5f);
		float P = sf_dev;
		float den = pow((UF_Aspect * UT*( P * F + (1.0f-P) * G) + (P/sf_dev + (1.0f-P)/ff_dev) * 10.0f),UF_j);
                float dc = dragmult * sf_dev * ff_dev * (1.0/(max(0.0001f,rocksize)))* max(0.0f,density-1000.0f)/max(0.1f,1000.0f);//den > 0.0f? 100.0*ff_dev * sf_dev* (1.0f - (1.0f/max(1.0f,density)))/den : 0.0f;
		float aspect = 1.0;
		

        float acc_y =
				(-GRAV * sy_zh
						-UF_Aspect
						*(
                                                        -(1.0f/(ff_dev * Nr))*(
                            2.0f*ddfvdyy + ddfudxy + ddfvdxx
                            )
                        +(1.0f/(ff_dev * Nra))*(
                            2.0f *(dsfdy*(dfvdy - dsvdy) + ddsfdyy*(vf - vs))
                            +(dsfdy*(dfudx - dsudx) + ddsfdxy*(uf - us))
                            +(dsfdx*(dfvdx - dsvdx) + ddsfdxx*(vf - vs))
                            + UF_Chi * vf/(UF_Aspect*UF_Aspect*ff_dev*Nra*h_dev*h_dev)
                            )
                        -(UF_Ksi*sf*(vf - vs)/(UF_Aspect*UF_Aspect*ff_dev*Nra*h_dev*h_dev))
                        )
                    );
		
		vn = vn + acc_y * dt;
		
		//drag
		float vpow = pow((vn - vs) * (vn - vs) + (uf - us) * (uf - us),UF_j - 1.0f);
		float lfacv = max(0.0f,dt * dc);
                float fac_dc = min(1.0f,max(0.0f,(exp(-(dc * vpow/ff_dev)) + ((sf_dev *density)/(1000.0f))*tan(ifa))/tan(ifa)));
                float v_balance = (1.0f - fac_dc) *(ff * vn + sf * vs) + (fac_dc) * vs;
		vn = v_balance + (vn - v_balance) * exp(-lfacv);
	}
	
        {

            float vsq = sqrt((float)(uf * uf + vf * vf));
            float nsq1 = (0.001f+n)*(0.001f+n)*GRAV/max(0.01f,pow((float)(hn),(float)(4.0f/3.0f)));
            float nsq = nsq1*vsq*dt;
            vn = (float)((vn/(1.0f+nsq)));
        }
	
        float threshold = 0.01f * dx;
        if(hn < threshold)
        {
            float kinfac = max(0.0f,(threshold - hn) / (0.025f * dx));
            float acc_eff = (vn -vnold)/max(0.0001f,dt);

            float v_kin = (sy_zh>0.0f?1.0f:-1.0f) * hn * sqrt(hn) * max(0.001f,sqrt(sy_zh>0.0f?sy_zh:-sy_zh))/(0.001f+n);

            vn = kinfac * v_kin + vn*(1.0f-kinfac);

        }


	return vn;
}

nonkernel float GetVNSX(float v,float hn, float dt, float dx, float n,
			   float z, float z_x1, float z_x2, float z_y1, float z_y2,
			   float hs, float hs_x1, float hs_x2, float hs_y1, float hs_y2,
			   float us, float us_x1, float us_x2, float us_y1, float us_y2,
			   float vs, float vs_x1, float vs_x2, float vs_y1, float vs_y2,
			   float us_x1y1, float us_x1y2, float us_x2y1, float us_x2y2,
			   float vs_x1y1, float vs_x1y2, float vs_x2y1, float vs_x2y2,
			   float hf, float hf_x1, float hf_x2, float hf_y1, float hf_y2,
			   float uf, float uf_x1, float uf_x2, float uf_y1, float uf_y2,
			   float vf, float vf_x1, float vf_x2, float vf_y1, float vf_y2,
			   float uf_x1y1, float uf_x1y2, float uf_x2y1, float uf_x2y2,
			   float vf_x1y1, float vf_x1y2, float vf_x2y1, float vf_x2y2,
                           float ifa, float rocksize, float density, float dragmult)
{

	if(hs < 0.0001f)
	{
                return 0.0f;
	}

        if(ifa > 1.5f)
        {
            ifa = 1.5f;
        }
	
	float vn = v;

	float dx2 =(dx*dx);
		
			
	float hsurf = z +hs + hf;
	float hsurfs = z +hs;
	
        float sxs_zh_x2 = min((float)(0.5f),max((float)(-0.5f),(float)((z_x2 + hs_x2 -hsurfs)/dx)));
        float sys_zh_y1 = min((float)(0.5f),max((float)(-0.5f),(float)((hsurfs-z_y1 - hs_y1)/dx)));
        float sxs_zh_x1 = min((float)(0.5f),max((float)(-0.5f),(float)((hsurfs-z_x1 - hs_x1)/dx)));
        float sys_zh_y2 = min((float)(0.5f),max((float)(-0.5f),(float)((z_y2 + hs_y2 -hsurfs)/dx)));
	
	float sxs_zh = min(1.0f,max(-1.0f,minmod(sxs_zh_x1,sxs_zh_x2)));
	float sys_zh = min(1.0f,max(-1.0f,minmod(sys_zh_y1,sys_zh_y2)));
	
	float h_dev = max(0.001f,hs + hf);
	float ff_dev = max(0.001f,hf/max(0.001f,(hf + hs)));
	float sf_dev = max(0.001f,hs/max(0.001f,(hf + hs)));
	float ff = max(0.0f,hf/max(0.001f,(hf + hs)));
	float sf = max(0.0f,hs/max(0.001f,(hf + hs)));
	
	float ddemhdx = minmod(z_x2 + hs_x2 - z + hs,z + hs - z_x1 - hs_x1)/dx;
	float SlopeX = minmod(z_x2 - z ,z- z_x1)/dx;
	float dhdx = minmod(hs_x2 - hs ,hs- hs_x1)/dx;
	

	float UF_Aspect = 1.0f;
	float UF_Ksi = 3.0f;
	float UF_Chi = 3.0f;
	float UF_j = 2.0f;
	float gamma = 1000.0f/density;
	float UT = 0.1f;
	float visc = min(100000.0f,max(1.0f,1.0f * exp(20.0f * max(0.0f,min(1.0f,sf_dev)))));
	float Nr = 10000.0f;
	float Nra = 10000.0f;	
	float F = (gamma/180.0f)* pow(ff_dev/sf_dev,3.0f) * Nr;
	float G = pow(ff_dev,2.5f);
	float P = sf_dev;
	float den = pow((UF_Aspect * UT*( P * F + (1.0f-P) * G) + (P/sf_dev + (1.0f-P)/ff_dev) * 10.0f),UF_j);
        float dc = dragmult * sf_dev * ff_dev *  (1.0f/(max(0.0001f,rocksize)))*max(0.0f,density-1000.0f)/max(0.1f,1000.0f);//den > 0.0f? 100.0*ff_dev * sf_dev* (1.0f - (1.0f/max(1.0f,density)))/den : 0.0f;
	float pbf = -GRAV;
        float pbs = (1.0f-gamma) * pbf;

    float acc_x =(-GRAV * sxs_zh + UF_Aspect*pbs*( dhdx)
					+UF_Aspect * gamma * pbf * (SlopeX + dhdx)
					 );
	
	vn = vn + acc_x * dt;
	
    //drag
    float vpow = pow((vn - uf) * (vn - uf) + (vf - vs) * (vf - vs),UF_j - 1.0f);
    float lfacv = max(0.0f,dt * dc);
    float v_balance = ff * uf + sf * vn;
    vn = v_balance + (vn - v_balance) * exp(-lfacv);

    float vsq = sqrt((float)(us * us + vs * vs));
    float nsq1 = 0.1f * (0.001f+n)*(0.001f+n)*GRAV/max(0.01f,pow((float)(hn),(float)(4.0f/3.0f)));
    float nsq = nsq1*vsq*dt;
    vn = (float)((vn/(1.0f+nsq)));
	
    float vn_ifa = vn > 0.0f? max(0.0f,(float)(vn - sf * dt *GRAV * tan(ifa))) : min(0.0f,(float)(vn + sf * dt * GRAV * tan(ifa)));
    float fac_dc = min(1.0f,max(0.0f,(exp(-(dc * vpow/ff_dev)))/tan(ifa)));// + ((sf_dev *density)/(1000.0f))*tan(ifa)
    vn = vn_ifa * fac_dc + (1.0f-fac_dc) * vn;

    return vn;
}

nonkernel float GetVNSY(float v,float hn, float dt, float dx, float n,
			   float z, float z_x1, float z_x2, float z_y1, float z_y2,
			   float hs, float hs_x1, float hs_x2, float hs_y1, float hs_y2,
			   float us, float us_x1, float us_x2, float us_y1, float us_y2,
			   float vs, float vs_x1, float vs_x2, float vs_y1, float vs_y2,
			   float us_x1y1, float us_x1y2, float us_x2y1, float us_x2y2,
			   float vs_x1y1, float vs_x1y2, float vs_x2y1, float vs_x2y2,
			   float hf, float hf_x1, float hf_x2, float hf_y1, float hf_y2,
			   float uf, float uf_x1, float uf_x2, float uf_y1, float uf_y2,
			   float vf, float vf_x1, float vf_x2, float vf_y1, float vf_y2,
			   float uf_x1y1, float uf_x1y2, float uf_x2y1, float uf_x2y2,
			   float vf_x1y1, float vf_x1y2, float vf_x2y1, float vf_x2y2,
                           float ifa, float rocksize, float density, float dragmult)
{
	if(hs < 0.0001f)
	{
                return 0.0f;
	}
        if(ifa > 1.5f)
        {
            ifa = 1.5f;
        }
	float hsurf = z +hs + hf;
	float hsurfs = z +hs;
	
        float sxs_zh_x2 = min((float)(0.5f),max((float)(-0.5f),(float)((z_x2 + hs_x2 -hsurfs)/dx)));
        float sys_zh_y1 = min((float)(0.5f),max((float)(-0.5f),(float)((hsurfs-z_y1 - hs_y1)/dx)));
        float sxs_zh_x1 = min((float)(0.5f),max((float)(-0.5f),(float)((hsurfs-z_x1 - hs_x1)/dx)));
        float sys_zh_y2 = min((float)(0.5f),max((float)(-0.5f),(float)((z_y2 + hs_y2 -hsurfs)/dx)));
	
	float sxs_zh = min(1.0f,max(-1.0f,minmod(sxs_zh_x1,sxs_zh_x2)));
	float sys_zh = min(1.0f,max(-1.0f,minmod(sys_zh_y1,sys_zh_y2)));
	
	float vn = v;

	float dx2 =(dx*dx);
		
	float h_dev = max(0.001f,hs + hf);
	float ff_dev = max(0.001f,hf/max(0.001f,(hf + hs)));
	float sf_dev = max(0.001f,hs/max(0.001f,(hf + hs)));
	float ff = max(0.0f,hf/max(0.001f,(hf + hs)));
	float sf = max(0.0f,hs/max(0.001f,(hf + hs)));
	
	float SlopeY = minmod(z_y2 - z ,z- z_y1)/dx;
	float dhdy = minmod(hs_y2 - hs ,hs- hs_y1)/dx;
	

	float UF_Aspect = 1.0f;
	float UF_Ksi = 3.0f;
	float UF_Chi = 3.0f;
	float UF_j = 2.0f;
	float gamma = 1000.0f/density;
	float UT = 0.1f;
	float visc = min(100000.0f,max(1.0f,1.0f * exp(20.0f * max(0.0f,min(1.0f,sf_dev)))));
	float Nr = 10000.0f;
	float Nra = 10000.0f;	
	float F = (gamma/180.0f)* pow(ff_dev/sf_dev,3.0f) * Nr;
	float G = pow(ff_dev,2.5f);
	float P = sf_dev;
	float den = pow((UF_Aspect * UT*( P * F + (1.0f-P) * G) + (P/sf_dev + (1.0f-P)/ff_dev) * 10.0f),UF_j);
        float dc = dragmult * sf_dev * ff_dev * (1.0/(max(0.0001f,rocksize)))* max(0.0f,density-1000.0f)/max(0.1f,1000.0f);//den > 0.0f? 100.0*ff_dev * sf_dev* (1.0f - (1.0f/max(1.0f,density)))/den : 0.0f;
	float pbf = -GRAV;
	float pbs = (1.0-gamma) * pbf;

    float acc_y =(-GRAV * sys_zh + UF_Aspect*pbs*( dhdy)
					+UF_Aspect * gamma * pbf * (SlopeY + dhdy)
					 );
	
	vn = vn + acc_y * dt;
	
        //drag
        float vpow = pow((vn - vf) * (vn - vf) + (uf - us) * (uf - us),UF_j - 1.0f);
        float lfacv = max(0.0f,dt * dc);
        float v_balance = ff * vf  + sf * vn;
        vn = v_balance + (vn - v_balance) * exp(-lfacv);

	float vsq = sqrt((float)(us * us + vs * vs));
        float nsq1 = (0.001+n)*(0.001+n)*GRAV/max(0.01f,pow((float)(hn),(float)(4.0/3.0)));
        float nsq = nsq1*vsq*dt;
        vn = (float)((vn/(1.0f+nsq)));
	
        float vn_ifa = vn > 0? max(0.0f,(float)(vn - sf * dt *GRAV * tan(ifa))) : min(0.0f,(float)(vn + sf * dt * GRAV * tan(ifa)));
        float fac_dc = min(1.0f,max(0.0f,(exp(-(dc * vpow/ff_dev)))/tan(ifa)));// + ((sf_dev *density)/(1000.0f))*tan(ifa)
        vn = vn_ifa * fac_dc + (1.0f-fac_dc) * vn;
	return vn;
}

kernel
void flowsolid(
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
				__read_only image2d_t InfilPot,
				__read_only image2d_t InfilCum,
				__read_only image2d_t CH_MASK,
				__read_only image2d_t CH_WIDTH,
				__read_only image2d_t CH_HEIGHT,
				__read_only image2d_t CH_H,
				__read_only image2d_t CH_V,
				__read_only image2d_t CH_HS,
				__read_only image2d_t CH_VS,
				__read_only image2d_t CH_SIfa,
				__read_only image2d_t CH_SRocksize,
				__read_only image2d_t CH_SDensity,
				__write_only image2d_t Hn,
				__write_only image2d_t Vxn,
				__write_only image2d_t Vyn,
				__write_only image2d_t HSn,
				__write_only image2d_t VSxn,
				__write_only image2d_t VSyn,
				__write_only image2d_t SIfan,
				__write_only image2d_t SRocksizen,
				__write_only image2d_t SDensityn,
				__write_only image2d_t InfilAct,
				__write_only image2d_t CH_HN,
				__write_only image2d_t CH_VN,
				__write_only image2d_t CH_HSN,
				__write_only image2d_t CH_VSN,
				__write_only image2d_t CH_SIfaN,
				__write_only image2d_t CH_SRocksizeN,
				__write_only image2d_t CH_SDensityN,
				__write_only image2d_t OUTPUT_UI,
				int ui_image,
                                float dt_hydro,
                                __read_only image2d_t QFOUT,
                                __write_only image2d_t QFOUTN,
                                __read_only image2d_t QSOUT,
                                __write_only image2d_t QSOUTN,
                                __write_only image2d_t DTREQ,
                                __write_only image2d_t RAIN,
                                __read_only image2d_t MANNING,
                                float dragmult,
                                __write_only image2d_t QFX1,
                                __write_only image2d_t QFX2,
                                __write_only image2d_t QFY1,
                                __write_only image2d_t QFY2,
                                __write_only image2d_t QFCHX1,
                                __write_only image2d_t QFCHX2,
                                __write_only image2d_t QFCHIN
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

        float infilpot = (dt/dt_hydro)*read_imagef(InfilPot,sampler, int2(gx,gy)).x;
        float infilcum = read_imagef(InfilCum,sampler, int2(gx,gy)).x;
	
        float z = read_imagef(DEM,sampler, int2(gx,gy)).x;
        float man_n = read_imagef(MANNING,sampler, int2(gx,gy)).x;
        float n = man_n + 0.001f;
        if(z > -1000.0f)
        {

            float qfout = read_imagef(QFOUT,sampler, int2(gx,gy)).x;
            float qsout = read_imagef(QSOUT,sampler, int2(gx,gy)).x;


            float rain = read_imagef(RAIN,sampler, int2(gx_x1,gy)).x;


            float z_x1 = read_imagef(DEM,sampler, int2(gx_x1,gy)).x;
            float z_x2 = read_imagef(DEM,sampler, int2(gx_x2,gy)).x;
            float z_y1 = read_imagef(DEM,sampler, int2(gx,gy_y1)).x;
            float z_y2 = read_imagef(DEM,sampler, int2(gx,gy_y2)).x;

            if(gx + 1 > dim0-1)
            {
                z_x2 = -99999.0f;
            }
            if(gx - 1 < 0)
            {
                z_x1 = -99999.0f;
            }
            if(gy + 1 > dim1-1)
            {
                z_y2 = -99999.0f;
            }
            if(gy - 1 < 0)
            {
                z_y1 = -99999.0f;
            }

            float h = read_imagef(H,sampler, int2(gx,gy)).x;
            float h_x1 = read_imagef(H,sampler, int2(gx_x1,gy)).x;
            float h_x2 = read_imagef(H,sampler, int2(gx_x2,gy)).x;
            float h_y1 = read_imagef(H,sampler, int2(gx,gy_y1)).x;
            float h_y2 = read_imagef(H,sampler, int2(gx,gy_y2)).x;

            float hs = read_imagef(HS,sampler, int2(gx,gy)).x;
            float hs_x1 = read_imagef(HS,sampler, int2(gx_x1,gy)).x;
            float hs_x2 = read_imagef(HS,sampler, int2(gx_x2,gy)).x;
            float hs_y1 = read_imagef(HS,sampler, int2(gx,gy_y1)).x;
            float hs_y2 = read_imagef(HS,sampler, int2(gx,gy_y2)).x;

            float vmax = 0.1f * dx/dt;

            float sifa = min(3.0f,max(0.00001f,read_imagef(SIfa,sampler, int2(gx,gy)).x));
            float srocksize = min(100.0f,max(0.00001f,read_imagef(SRocksize,sampler, int2(gx,gy)).x));
            float sdensity = min(3000.0f,max(1000.00f,read_imagef(SDensity,sampler, int2(gx,gy)).x));

            float sifa_x1 = read_imagef(SIfa,sampler, int2(gx_x1,gy)).x;
            float sifa_x2 = read_imagef(SIfa,sampler, int2(gx_x2,gy)).x;
            float sifa_y1 = read_imagef(SIfa,sampler, int2(gx,gy_y1)).x;
            float sifa_y2 = read_imagef(SIfa,sampler, int2(gx,gy_y2)).x;
            float srocksize_x1 = read_imagef(SRocksize,sampler, int2(gx_x1,gy)).x;
            float srocksize_x2 = read_imagef(SRocksize,sampler, int2(gx_x2,gy)).x;
            float srocksize_y1 = read_imagef(SRocksize,sampler, int2(gx,gy_y1)).x;
            float srocksize_y2 = read_imagef(SRocksize,sampler, int2(gx,gy_y2)).x;
            float sdensity_x1 = read_imagef(SDensity,sampler, int2(gx_x1,gy)).x;
            float sdensity_x2 = read_imagef(SDensity,sampler, int2(gx_x2,gy)).x;
            float sdensity_y1 = read_imagef(SDensity,sampler, int2(gx,gy_y1)).x;
            float sdensity_y2 = read_imagef(SDensity,sampler, int2(gx,gy_y2)).x;

            float vsx = min(vmax,max(-vmax,read_imagef(VSx,sampler, int2(gx,gy)).x));
            float vsy = min(vmax,max(-vmax,read_imagef(VSy,sampler, int2(gx,gy)).x));

            float vsx_x1 = min(vmax,max(-vmax,read_imagef(VSx,sampler, int2(gx_x1,gy)).x));
            float vsy_x1 = min(vmax,max(-vmax,read_imagef(VSy,sampler, int2(gx_x1,gy)).x));
            float vsx_x2 = min(vmax,max(-vmax,read_imagef(VSx,sampler, int2(gx_x2,gy)).x));
            float vsy_x2 = min(vmax,max(-vmax,read_imagef(VSy,sampler, int2(gx_x2,gy)).x));
            float vsx_y1 = min(vmax,max(-vmax,read_imagef(VSx,sampler, int2(gx,gy_y1)).x));
            float vsy_y1 = min(vmax,max(-vmax,read_imagef(VSy,sampler, int2(gx,gy_y1)).x));
            float vsx_y2 = min(vmax,max(-vmax,read_imagef(VSx,sampler, int2(gx,gy_y2)).x));
            float vsy_y2 = min(vmax,max(-vmax,read_imagef(VSy,sampler, int2(gx,gy_y2)).x));

            float vsx_x1y1 = min(vmax,max(-vmax,read_imagef(VSx,sampler, int2(gx_x1,gy_y1)).x));
            float vsy_x1y1 = min(vmax,max(-vmax,read_imagef(VSy,sampler, int2(gx_x1,gy_y1)).x));
            float vsx_x2y2 = min(vmax,max(-vmax,read_imagef(VSx,sampler, int2(gx_x2,gy_y2)).x));
            float vsy_x2y2 = min(vmax,max(-vmax,read_imagef(VSy,sampler, int2(gx_x2,gy_y2)).x));
            float vsx_x1y2 = min(vmax,max(-vmax,read_imagef(VSx,sampler, int2(gx_x1,gy_y2)).x));
            float vsy_x1y2 = min(vmax,max(-vmax,read_imagef(VSy,sampler, int2(gx_x1,gy_y2)).x));
            float vsx_x2y1 = min(vmax,max(-vmax,read_imagef(VSx,sampler, int2(gx_x2,gy_y1)).x));
            float vsy_x2y1 = min(vmax,max(-vmax,read_imagef(VSy,sampler, int2(gx_x2,gy_y1)).x));

            float vx = min(vmax,max(-vmax,read_imagef(Vx,sampler, int2(gx,gy)).x));
            float vy = min(vmax,max(-vmax,read_imagef(Vy,sampler, int2(gx,gy)).x));

            float vx_x1 = min(vmax,max(-vmax,read_imagef(Vx,sampler, int2(gx_x1,gy)).x));
            float vy_x1 = min(vmax,max(-vmax,read_imagef(Vy,sampler, int2(gx_x1,gy)).x));
            float vx_x2 = min(vmax,max(-vmax,read_imagef(Vx,sampler, int2(gx_x2,gy)).x));
            float vy_x2 = min(vmax,max(-vmax,read_imagef(Vy,sampler, int2(gx_x2,gy)).x));
            float vx_y1 = min(vmax,max(-vmax,read_imagef(Vx,sampler, int2(gx,gy_y1)).x));
            float vy_y1 = min(vmax,max(-vmax,read_imagef(Vy,sampler, int2(gx,gy_y1)).x));
            float vx_y2 = min(vmax,max(-vmax,read_imagef(Vx,sampler, int2(gx,gy_y2)).x));
            float vy_y2 = min(vmax,max(-vmax,read_imagef(Vy,sampler, int2(gx,gy_y2)).x));

            float vx_x1y1 = min(vmax,max(-vmax,read_imagef(Vx,sampler, int2(gx_x1,gy_y1)).x));
            float vy_x1y1 = min(vmax,max(-vmax,read_imagef(Vy,sampler, int2(gx_x1,gy_y1)).x));
            float vx_x2y2 = min(vmax,max(-vmax,read_imagef(Vx,sampler, int2(gx_x2,gy_y2)).x));
            float vy_x2y2 = min(vmax,max(-vmax,read_imagef(Vy,sampler, int2(gx_x2,gy_y2)).x));
            float vx_x1y2 = min(vmax,max(-vmax,read_imagef(Vx,sampler, int2(gx_x1,gy_y2)).x));
            float vy_x1y2 = min(vmax,max(-vmax,read_imagef(Vy,sampler, int2(gx_x1,gy_y2)).x));
            float vx_x2y1 = min(vmax,max(-vmax,read_imagef(Vx,sampler, int2(gx_x2,gy_y1)).x));
            float vy_x2y1 = min(vmax,max(-vmax,read_imagef(Vy,sampler, int2(gx_x2,gy_y1)).x));

            float hsurf = z +hs + h;
            float hsurfs = z +hs;
            float sx_zh_x2 = min((float)(0.5f),max((float)(-0.5f),(float)((z_x2 + hs_x2 + h_x2-hsurf)/dx)));
            float sy_zh_y1 = min((float)(0.5f),max((float)(-0.5f),(float)((hsurf-z_y1 - hs_y1 - h_y1)/dx)));
            float sx_zh_x1 = min((float)(0.5f),max((float)(-0.5f),(float)((hsurf-z_x1 - hs_x1 - h_x1)/dx)));
            float sy_zh_y2 = min((float)(0.5f),max((float)(-0.5f),(float)((z_y2 + hs_y2 + h_y2-hsurf)/dx)));

            float sx_zh = min(1.0f,max(-1.0f,minmod(sx_zh_x1,sx_zh_x2)));
            float sy_zh = min(1.0f,max(-1.0f,minmod(sy_zh_y1,sy_zh_y2)));

            float sxs_zh_x2 = min((float)(0.5f),max((float)(-0.5f),(float)((z_x2 + hs_x2 -hsurfs)/dx)));
            float sys_zh_y1 = min((float)(0.5f),max((float)(-0.5f),(float)((hsurfs-z_y1 - hs_y1)/dx)));
            float sxs_zh_x1 = min((float)(0.5f),max((float)(-0.5f),(float)((hsurfs-z_x1 - hs_x1)/dx)));
            float sys_zh_y2 = min((float)(0.5f),max((float)(-0.5f),(float)((z_y2 + hs_y2 -hsurfs)/dx)));

            float sxs_zh = min(1.0f,max(-1.0f,minmod(sxs_zh_x1,sxs_zh_x2)));
            float sys_zh = min(1.0f,max(-1.0f,minmod(sys_zh_y1,sys_zh_y2)));

            float C = 0.1f;
            float err = 0.0000f;


            float3 hlls_x1 = z_x1 < -1000.0f? float3(0.0f,0.0f,0.0f):F_HLL2SF(max(0.0f,hs_x1 -max(0.0f,z - z_x1)),max(0.0f,h_x1 -max(0.0f,z - z_x1)),vsx_x1,vsy_x1,max(0.0f,hs -max(0.0f,z_x1-z)) ,max(0.0f,h -max(0.0f,z_x1-z)),vsx,vsy,sifa, srocksize, sdensity, dragmult);
            float3 hlls_x2 = z_x2 < -1000.0f? float3(0.0f,0.0,0.0):F_HLL2SF(max(0.0f,hs -max(0.0f,z_x2 - z)),max(0.0f,h -max(0.0f,z_x2 - z)),vsx,vsy,max(0.0f,hs_x2 -max(0.0f,z - z_x2)),max(0.0f,h_x2 -max(0.0f,z - z_x2)),vsx_x2,vsy_x2,sifa, srocksize, sdensity, dragmult);
            float3 hlls_y1 = z_y1 < -1000.0f? float3(0.0f,0.0f,0.0f):F_HLL2SF(max(0.0f,hs_y1 -max(0.0f,z - z_y1)),max(0.0f,h_y1 -max(0.0f,z - z_y1)),vsy_y1,vsx_y1,max(0.0f,hs -max(0.0f,z_y1 - z)),max(0.0f,h -max(0.0f,z_y1 - z)),vsy,vsx,sifa, srocksize, sdensity, dragmult);
            float3 hlls_y2 = z_y2 < -1000.0f? float3(0.0f,0.0f,0.0f):F_HLL2SF(max(0.0f,hs -max(0.0f,z_y2 - z)),max(0.0f,h -max(0.0f,z_y2 - z)),vsy,vsx,max(0.0f,hs_y2 -max(0.0f,z - z_y2)),max(0.0f,h_y2 -max(0.0f,z - z_y2)),vsy_y2,vsx_y2,sifa, srocksize, sdensity, dragmult);

            //float3 hlls_x1 = z_x1 < -1000? float3(0.0f,0.0f,0.0f):F_HLL2SF(hs_x1,h_x1,vsx_x1,vsy_x1,hs,h,vsx,vsy);
            //float3 hlls_x2 = z_x2 < -1000? float3(0.0f,0.0f,0.0f):F_HLL2SF(hs,h,vsx,vsy,hs_x2,h_x2,vsx_x2,vsy_x2);
            //float3 hlls_y1 = z_y1 < -1000? float3(0.0f,0.0f,0.0f):F_HLL2SF(hs_y1,h_y1,vsy_y1,vsx_y1,hs,h,vsy,vsx);
            //float3 hlls_y2 = z_y2 < -1000? float3(0.0f,0.0f,0.0f):F_HLL2SF(hs,h,vsy,vsx,hs_y2,h_y2,vsy_y2,vsx_y2);
            float fluxs_x1 = z_x1 < -1000.0f? max(-hs * C,(float)(-hs * sqrt(hs) *dt* sqrt(hs)/(dx*(0.001f+n)))):max(-max(err,hs) * C,min(max(err,hs_x1) * C,(float)(+tx*(hlls_x1.x))));
            float fluxs_x2 = z_x2 < -1000.0f? max(-hs * C,(float)(-hs * sqrt(hs) *dt* sqrt(hs)/(dx*(0.001f+n)))):max(-max(err,hs) * C,min(max(err,hs_x2) * C,(float)(-tx*(hlls_x2.x))));
            float fluxs_y1 = z_y1 < -1000.0f? max(-hs * C,(float)(-hs * sqrt(hs) *dt* sqrt(hs)/(dx*(0.001f+n)))):max(-max(err,hs) * C,min(max(err,hs_y1) * C,(float)(+tx*(hlls_y1.x))));
            float fluxs_y2 = z_y2 < -1000.0f? max(-hs * C,(float)(-hs * sqrt(hs) *dt* sqrt(hs)/(dx*(0.001f+n)))):max(-max(err,hs) * C,min(max(err,hs_y2) * C,(float)(-tx*(hlls_y2.x))));

            qsout = qsout +z_x1 < -1000.0f? fluxs_x1 : 0.0f;
            qsout = qsout +z_x2 < -1000.0f? fluxs_x2 : 0.0f;
            qsout = qsout +z_y1 < -1000.0f? fluxs_y1 : 0.0f;
            qsout = qsout +z_y2 < -1000.0f? fluxs_y2 : 0.0f;

            float hsold = hs;
            float hsn = ((max(0.00f,(float)(hs + fluxs_x1 + fluxs_x2 + fluxs_y1 + fluxs_y2))));
            float a_sx = 0.5f * GRAV *sx_zh;
            float a_sy = 0.5f * GRAV *sy_zh;
            float qsxn = hs * vsx - tx*(hlls_x2.y - hlls_x1.y) - tx*(hlls_y2.z - hlls_y1.z);
            float qsyn = hs * vsy - tx*(hlls_x2.z - hlls_x1.z) - tx*(hlls_y2.y - hlls_y1.y);
            float vsxn = (float)((qsxn))/max(0.01f,(float)(hsn));
            float vsyn = (float)((qsyn))/max(0.01f,(float)(hsn));

            vsxn = GetVNSX(vsxn, hsn,dt, dx, n,
                               z, z_x1, z_x2, z_y1, z_y2,
                               hs, hs_x1, hs_x2, hs_y1, hs_y2,
                               vsx, vsx_x1, vsx_x2, vsx_y1, vsx_y2,
                               vsy, vsy_x1, vsy_x2, vsy_y1, vsy_y2,
                               vsx_x1y1, vsx_x1y2, vsx_x2y1, vsx_x2y2,
                               vsy_x1y1, vsy_x1y2, vsy_x2y1, vsy_x2y2,
                               h, h_x1, h_x2, h_y1, h_y2,
                               vx, vx_x1, vx_x2, vx_y1, vx_y2,
                               vy, vy_x1, vy_x2, vy_y1, vy_y2,
                               vx_x1y1, vx_x1y2, vx_x2y1, vx_x2y2,
                               vy_x1y1, vy_x1y2, vy_x2y1, vy_x2y2,
                               sifa, srocksize, sdensity, dragmult);

            vsyn = GetVNSY(vsyn,hsn, dt, dx, n,
                               z, z_x1, z_x2, z_y1, z_y2,
                               hs, hs_x1, hs_x2, hs_y1, hs_y2,
                               vsx, vsx_x1, vsx_x2, vsx_y1, vsx_y2,
                               vsy, vsy_x1, vsy_x2, vsy_y1, vsy_y2,
                               vsx_x1y1, vsx_x1y2, vsx_x2y1, vsx_x2y2,
                               vsy_x1y1, vsy_x1y2, vsy_x2y1, vsy_x2y2,
                               h, h_x1, h_x2, h_y1, h_y2,
                               vx, vx_x1, vx_x2, vx_y1, vx_y2,
                               vy, vy_x1, vy_x2, vy_y1, vy_y2,
                               vx_x1y1, vx_x1y2, vx_x2y1, vx_x2y2,
                               vy_x1y1, vy_x1y2, vy_x2y1, vy_x2y2,
                               sifa, srocksize, sdensity, dragmult);

            float ff_dev_dc = max(0.001f,h/max(0.001f,(h + hs)));
            float sf_dev_dc = max(0.001f,hs/max(0.001f,(h + hs)));

            float dc = dragmult * ff_dev_dc * sf_dev_dc * (1.0f/(max(0.0001f,srocksize)))* max(0.0f,sdensity-1000.0f)/max(0.1f,1000.0f);

            float3 hll_x1 = z_x1 < -1000.0f? float3(0.0f,0.0f,0.0f):F_HLL2FS(max(0.0f,h_x1 -max(0.0f,z - z_x1)),max(0.0f,hs_x1 -max(0.0f,z - z_x1)),vx_x1,vy_x1,max(0.0f,h -max(0.0f,z_x1-z)) ,max(0.0f,hs -max(0.0f,z_x1-z)),vx,vy,sifa, srocksize, sdensity, dragmult);
            float3 hll_x2 = z_x2 < -1000.0f? float3(0.0f,0.0f,0.0f):F_HLL2FS(max(0.0f,h -max(0.0f,z_x2 - z)),max(0.0f,hs -max(0.0f,z_x2 - z)),vx,vy,max(0.0f,h_x2 -max(0.0f,z - z_x2)),max(0.0f,hs_x2 -max(0.0f,z - z_x2)),vx_x2,vy_x2,sifa, srocksize, sdensity, dragmult);
            float3 hll_y1 = z_y1 < -1000.0f? float3(0.0f,0.0f,0.0f):F_HLL2FS(max(0.0f,h_y1 -max(0.0f,z - z_y1)),max(0.0f,hs_y1 -max(0.0f,z - z_y1)),vy_y1,vx_y1,max(0.0f,h -max(0.0f,z_y1 - z)),max(0.0f,hs -max(0.0f,z_y1 - z)),vy,vx,sifa, srocksize, sdensity, dragmult);
            float3 hll_y2 = z_y2 < -1000.0f? float3(0.0f,0.0f,0.0f):F_HLL2FS(max(0.0f,h -max(0.0f,z_y2 - z)),max(0.0f,hs -max(0.0f,z_y2 - z)),vy,vx,max(0.0f,h_y2 -max(0.0f,z - z_y2)),max(0.0f,hs_y2 -max(0.0f,z - z_y2)),vy_y2,vx_y2,sifa, srocksize, sdensity, dragmult);
            float flux_x1 = z_x1 < -1000.0f? max(-h * C,(float)(-h * sqrt(h) *dt* sqrt(h)/(dx*(0.001f+n)))):max(-max(err,h) * C,min(max(err,h_x1) * C,(float)(+tx*(hll_x1.x))));
            float flux_x2 = z_x2 < -1000.0f? max(-h * C,(float)(-h * sqrt(h) *dt* sqrt(h)/(dx*(0.001f+n)))):max(-max(err,h) * C,min(max(err,h_x2) * C,(float)(-tx*(hll_x2.x))));
            float flux_y1 = z_y1 < -1000.0f? max(-h * C,(float)(-h * sqrt(h) *dt* sqrt(h)/(dx*(0.001f+n)))):max(-max(err,h) * C,min(max(err,h_y1) * C,(float)(+tx*(hll_y1.x))));
            float flux_y2 = z_y2 < -1000.0f? max(-h * C,(float)(-h * sqrt(h) *dt* sqrt(h)/(dx*(0.001f+n)))):max(-max(err,h) * C,min(max(err,h_y2) * C,(float)(-tx*(hll_y2.x))));

            qfout = qfout +z_x1 < -1000.0f? flux_x1 : 0.0f;
            qfout = qfout +z_x2 < -1000.0f? flux_x2 : 0.0f;
            qfout = qfout +z_y1 < -1000.0f? flux_y1 : 0.0f;
            qfout = qfout +z_y2 < -1000.0f? flux_y2 : 0.0f;

            write_imagef(QFX1, int2(gx,gy), flux_x1 *(dx*dx));
            write_imagef(QFX2, int2(gx,gy), flux_x2 *(dx*dx));
            write_imagef(QFY1, int2(gx,gy), flux_y1 *(dx*dx));
            write_imagef(QFY2, int2(gx,gy), flux_y2 *(dx*dx));

            float hold = h;
            float hn = ((max(0.00f,(float)(h + flux_x1 + flux_x2 + flux_y1 + flux_y2))));
            float a_fx = 0.5f * GRAV *sx_zh;
            float a_fy = 0.5f * GRAV *sy_zh;
            float qxn = h * vx - tx*(hll_x2.y - hll_x1.y) - tx*(hll_y2.z - hll_y1.z);
            float qyn = h * vy - tx*(hll_x2.z - hll_x1.z) - tx*(hll_y2.y - hll_y1.y);
            float vxn = (float)((qxn))/max(0.01f,(float)(hn));
            float vyn = (float)((qyn))/max(0.01f,(float)(hn));

            vxn = GetVNFX(vxn,hn, dt, dx, n,
                               z, z_x1, z_x2, z_y1, z_y2,
                               hs, hs_x1, hs_x2, hs_y1, hs_y2,
                               vsx, vsx_x1, vsx_x2, vsx_y1, vsx_y2,
                               vsy, vsy_x1, vsy_x2, vsy_y1, vsy_y2,
                               vsx_x1y1, vsx_x1y2, vsx_x2y1, vsx_x2y2,
                               vsy_x1y1, vsy_x1y2, vsy_x2y1, vsy_x2y2,
                               h, h_x1, h_x2, h_y1, h_y2,
                               vx, vx_x1, vx_x2, vx_y1, vx_y2,
                               vy, vy_x1, vy_x2, vy_y1, vy_y2,
                               vx_x1y1, vx_x1y2, vx_x2y1, vx_x2y2,
                               vy_x1y1, vy_x1y2, vy_x2y1, vy_x2y2,
                               sifa, srocksize, sdensity, dragmult);

            vyn = GetVNFY(vyn,hn, dt, dx, n,
                               z, z_x1, z_x2, z_y1, z_y2,
                               hs, hs_x1, hs_x2, hs_y1, hs_y2,
                               vsx, vsx_x1, vsx_x2, vsx_y1, vsx_y2,
                               vsy, vsy_x1, vsy_x2, vsy_y1, vsy_y2,
                               vsx_x1y1, vsx_x1y2, vsx_x2y1, vsx_x2y2,
                               vsy_x1y1, vsy_x1y2, vsy_x2y1, vsy_x2y2,
                               h, h_x1, h_x2, h_y1, h_y2,
                               vx, vx_x1, vx_x2, vx_y1, vx_y2,
                               vy, vy_x1, vy_x2, vy_y1, vy_y2,
                               vx_x1y1, vx_x1y2, vx_x2y1, vx_x2y2,
                               vy_x1y1, vy_x1y2, vy_x2y1, vy_x2y2,
                               sifa, srocksize, sdensity, dragmult);

            float infil_act = min((float)(infilpot),(float)(hn));
            hn = hn - infil_act;


            int edges = ((z_x1 < -1000)?1:0) +((z_x2 < -1000)?1:0)+((z_y1 < -1000)?1:0)+((z_y2 < -1000)?1:0);

            bool edge = (z_x1 < -1000 || z_x2 < -1000 || z_y1 < -1000 || z_y2 < -1000);


            hsn = edges > 2? 0.0f:isnan(hsn)? 0.0f:hsn;
            vsxn = edges > 2? 0.0f:isnan(vsxn)? 0.0f:vsxn;
            vsyn = edges > 2? 0.0f:isnan(vsyn)? 0.0f:vsyn;
            hn = edges > 2? 0.0f:isnan(hn)? 0.0f:hn;
            vxn = edges > 2? 0.0f:isnan(vxn)? 0.0f:vxn;
            vyn = edges > 2? 0.0f:isnan(vyn)? 0.0f:vyn;

            float hsleft = hs + min(0.0f,fluxs_x1) +min(0.0f,fluxs_x2) + min(0.0f,fluxs_y1) + min(0.0f,fluxs_y2);
            float sifan = (sifa * hsleft + max(0.0f,fluxs_x1) * sifa_x1 + max(0.0f,fluxs_x2) * sifa_x2 + max(0.0f,fluxs_y1) * sifa_y1 + max(0.0f,fluxs_y2) * sifa_y2)/(max(0.0001f,hsn));
            float srocksizen = (srocksize * hsleft + max(0.0f,fluxs_x1) * srocksize_x1 + max(0.0f,fluxs_x2) * srocksize_x2 + max(0.0f,fluxs_y1) * srocksize_y1 + max(0.0f,fluxs_y2) * srocksize_y2)/(max(0.0001f,hsn));
            float sdensityn = (sdensity * hsleft + max(0.0f,fluxs_x1) * sdensity_x1 + max(0.0f,fluxs_x2) * sdensity_x2 + max(0.0f,fluxs_y1) * sdensity_y1 + max(0.0f,fluxs_y2) * sdensity_y2)/(max(0.0001f,hsn));

            sifan = isnan(sifan)? 0.3f : sifan;
            srocksizen = isnan(srocksizen)? 0.1f : srocksizen;
            sdensityn = isnan(sdensityn)? 2000.0f : sdensityn;

            hn = hn + rain * dt_hydro/max(1e-10f,dt);

            write_imagef(InfilAct, int2(gx,gy), infilcum + infil_act);
            write_imagef(Hn, int2(gx,gy), hn);
            write_imagef(Vxn, int2(gx,gy), vxn);
            write_imagef(Vyn, int2(gx,gy), vyn);

            write_imagef(HSn, int2(gx,gy),hsn);
            write_imagef(VSxn, int2(gx,gy), vsxn);
            write_imagef(VSyn, int2(gx,gy), vsyn);

            write_imagef(SIfan, int2(gx,gy), sifan);
            write_imagef(SRocksizen, int2(gx,gy), srocksizen);
            write_imagef(SDensityn, int2(gx,gy), sdensityn);


            float dt_req = 0.25f *dx/( min(100.0f,max(0.01f,max(sqrt(vsxn * vsxn + vsyn*vsyn),sqrt(vxn*vxn + vyn * vyn)))));

            float ui = 0;
            if(ui_image == 1)
            {
                    ui = hn;
                    write_imagef(OUTPUT_UI, int2(gx,gy), ui);
            }else if(ui_image == 14)
            {
                    ui = sqrt(vxn * vxn + vyn * vyn);
                    write_imagef(OUTPUT_UI, int2(gx,gy), ui);
            }else if(ui_image == 15)
            {
                    ui =read_imagef(CH_V,sampler, int2(gx,gy)).x;
                    write_imagef(OUTPUT_UI, int2(gx,gy), ui);
            }else if(ui_image == 5)
            {
                    ui = read_imagef(CH_H,sampler, int2(gx,gy)).x;
                    write_imagef(OUTPUT_UI, int2(gx,gy), ui);
            }else if(ui_image == 8)
            {
                    ui = hsn;
                    write_imagef(OUTPUT_UI, int2(gx,gy), ui);
            }else if(ui_image == 9)
            {
                    ui = sqrt(vxn * vxn + vyn * vyn);
                    write_imagef(OUTPUT_UI, int2(gx,gy), ui);
            }else if(ui_image == 10)
            {
                    ui = sqrt(vsxn * vsxn + vsyn * vsyn);
                    write_imagef(OUTPUT_UI, int2(gx,gy), ui);
            }else if(ui_image == 11)
            {
                    ui = read_imagef(CH_V,sampler, int2(gx,gy)).x;
                    write_imagef(OUTPUT_UI, int2(gx,gy), ui);
            }else if(ui_image == 12)
            {
                    ui = read_imagef(CH_HS,sampler, int2(gx,gy)).x;
                    write_imagef(OUTPUT_UI, int2(gx,gy), ui);
            }else if(ui_image == 13)
            {
                    ui = read_imagef(CH_VS,sampler, int2(gx,gy)).x;
                    write_imagef(OUTPUT_UI, int2(gx,gy), ui);
            }else if(ui_image == 17)
            {
                    ui = sifa;
                    write_imagef(OUTPUT_UI, int2(gx,gy), ui);
            }else if(ui_image == 18)
            {
                    ui = srocksize;
                    write_imagef(OUTPUT_UI, int2(gx,gy), ui);
            }else if(ui_image == 19)
            {
                    ui = sdensity;
                    write_imagef(OUTPUT_UI, int2(gx,gy), ui);
            }else if(ui_image == 19)
            {
                    ui = sdensity;
                    write_imagef(OUTPUT_UI, int2(gx,gy), ui);
            }else if(ui_image == 40)
            {
                    ui = dc;
                    write_imagef(OUTPUT_UI, int2(gx,gy), ui);
            }




            float flux_chx1 = 0.0f;
            float flux_chx2 = 0.0f;


            //if channel is present, calculate channel interactions and channel flow
            int chmask = round(read_imagef(CH_MASK,sampler, int2(gx,gy)).x);
            if(chmask > 0 && chmask < 10)
            {
                    //channel properties
                    float ch_h = read_imagef(CH_H,sampler, int2(gx,gy)).x;
                    float ch_height = read_imagef(CH_WIDTH,sampler, int2(gx,gy)).x;
                    float ch_width = read_imagef(CH_HEIGHT,sampler, int2(gx,gy)).x;
                    float ch_v = read_imagef(CH_V,sampler, int2(gx,gy)).x;


                    //channel mask dubbles as ldd
                    int gx_next = min(dim0-(int)(1),max((int)(0),(int)(gx + ch_dx[chmask])));
                    int gy_next = min(dim1-(int)(1),max((int)(0),(int)(gy + ch_dy[chmask])));


                    float ch_z = z;
                    float ch_z_x1 = 0.0;
                    float ch_z_x2 = read_imagef(DEM,sampler, int2(gx_next,gy_next)).x;
                    float ch_hs = read_imagef(CH_HS,sampler, int2(gx,gy)).x;
                    float ch_hs_x1 = 0.0;
                    float ch_hs_x2 = read_imagef(CH_HS,sampler, int2(gx_next,gy_next)).x;
                    float ch_us = read_imagef(CH_VS,sampler, int2(gx,gy)).x;
                    float ch_us_x1 = 0.0;
                    float ch_us_x2 = read_imagef(CH_VS,sampler, int2(gx_next,gy_next)).x;
                    float ch_hf = read_imagef(CH_H,sampler, int2(gx,gy)).x;
                    float ch_hf_x1 = 0.0;
                    float ch_hf_x2 = read_imagef(CH_H,sampler, int2(gx_next,gy_next)).x;
                    float ch_uf = read_imagef(CH_V,sampler, int2(gx,gy)).x;
                    float ch_uf_x1 = 0.0;
                    float ch_uf_x2 = read_imagef(CH_V,sampler, int2(gx_next,gy_next)).x;
                    float ch_ifa = read_imagef(CH_SIfa,sampler, int2(gx,gy)).x;
                    float ch_ifa_x1 = 0.0;
                    float ch_ifa_x2 = read_imagef(CH_SIfa,sampler, int2(gx_next,gy_next)).x;
                    float ch_rocksize = read_imagef(CH_SRocksize,sampler, int2(gx,gy)).x;
                    float ch_rocksize_x1 = 0.0;
                    float ch_rocksize_x2 = read_imagef(CH_SRocksize,sampler, int2(gx_next,gy_next)).x;
                    float ch_density = read_imagef(CH_SDensity,sampler, int2(gx,gy)).x;
                    float ch_density_x1 = 0.0;
                    float ch_density_x2 = read_imagef(CH_SDensity,sampler, int2(gx_next,gy_next)).x;

                    float ch_vol = ch_h * ch_width * dx;
                    float ch_vols = ch_hs * ch_width * dx;

                    float chn_z = read_imagef(DEM,sampler, int2(gx_next,gy_next)).x;
                    float chn_h = read_imagef(CH_H,sampler, int2(gx_next,gy_next)).x;
                    float chn_height = read_imagef(CH_WIDTH,sampler, int2(gx_next,gy_next)).x;
                    float chn_width = read_imagef(CH_HEIGHT,sampler, int2(gx_next,gy_next)).x;
                    float chn_v = read_imagef(CH_V,sampler, int2(gx_next,gy_next)).x;
                    float chn_vol = chn_h * chn_width * dx;
                    float chn_hs = read_imagef(CH_HS,sampler, int2(gx_next,gy_next)).x;
                    float chn_us = read_imagef(CH_VS,sampler, int2(gx_next,gy_next)).x;
                    float chn_vols = chn_hs * chn_width * dx;

                    float chhn = ch_h;
                    float chvn = ch_v;

                    float chhsn = ch_hs;
                    float chvsn = ch_us;
                    float chrocksizen = ch_rocksize;
                    float chdensityn = ch_density;
                    float chifan = ch_ifa;

                    float chw_total = 0.0f;


                    float ch_vadd = 0.0f;
                    float ch_vaddw = 0.5f;


                    //flow out to next cell
                    if(chmask == 5)
                    {
                            float ch_slope = (ch_h)/dx;

                            //fluid
                            float ch_q = max(0.0f,dt * ch_h * ch_width * sqrt((float)(0.001+ch_h))/(dx * (0.05f)));
                            ch_q = max(0.0f,min(0.25f * ch_vol,ch_q));
                            chhn = chhn - 0.5f * ch_q/(ch_width * dx);

                            flux_chx2 = flux_chx2 + 0.5f * ch_q;

                            ch_vadd = ch_vadd + dt * 0.5f * GRAV * max(-1.0f,min(1.0f,(float)(ch_slope)));

                            //solid
                            float ch_qs = max(0.0f,dt * ch_hs * ch_width * sqrt((float)(0.001+ch_hs))/(dx * (0.05f)));
                            ch_qs = max(0.0f,min(0.25f * ch_vols,ch_q));
                            chhsn = chhsn - 0.5f * ch_qs/(ch_width * dx);

                            qfout = qfout + 0.5f * ch_q;
                            qsout = qsout + 0.5f * ch_qs;

                    }else
                    {
                            //fluid
                            float3 hll_x1 = F_HLL2(ch_h,ch_v,0.0f,chn_h,chn_v,0.0f);
                            float ch_q = (dt/dx)*(min(ch_width,chn_width)/dx)*((dx * 0.5f*(chn_width +ch_width)) *hll_x1.x);
                            ch_q = min(0.25f * ch_vol,ch_q);
                            ch_q = max(-0.25f * chn_vol,ch_q);
                            ch_q = ch_q * 0.5;

                            float ch_slope = max(0.0f,(float)((z + ch_h - chn_z - chn_h)/dx));
                            ch_vadd = ch_vadd + dt * 0.5f * GRAV * max(-1.0f,min(1.0f,(float)(ch_slope)));
                            if(ch_q < 0.0f)
                            {
                                    float new_ch_vol = chhn*(ch_width*dx);
                                    chvn = (chvn * new_ch_vol - chn_v *(ch_q))/max(0.01f,new_ch_vol - ch_q);
                            }
                            chhn = chhn - ch_q/(ch_width * dx);

                            //solid
                            float3 hlls_x1 = F_HLL2SF(ch_hs,ch_hf, ch_us,0,chn_hs,chn_h,chn_us,0,ch_ifa,ch_rocksize,ch_density,dragmult);
                            float ch_qs = (dt/dx)*(min(ch_width,chn_width)/dx)*((dx * 0.5f*(chn_width +ch_width)) *hlls_x1.x);
                            ch_qs = min(0.25f * ch_vols,ch_qs);
                            ch_qs = max(-0.25f * chn_vols,ch_qs);
                            ch_qs = ch_qs * 0.5;

                            float ch_slopes = max(0.0f,(float)((z + ch_hs - chn_z - chn_hs)/dx));
                            if(ch_qs < 0)
                            {
                                    float new_ch_vol = chhsn*(ch_width*dx);
                                    chvsn = (chvsn * new_ch_vol - chn_us *(ch_qs))/max(0.01f,new_ch_vol - ch_qs);
                            }
                            chhsn = chhsn - ch_qs/(ch_width * dx);
                            flux_chx2 = flux_chx2 + ch_q;

                    }

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
                                            float chp_width = read_imagef(CH_WIDTH,sampler, int2(gx_prev,gy_prev)).x;
                                            float chp_height = read_imagef(CH_HEIGHT,sampler, int2(gx_prev,gy_prev)).x;
                                            float chp_v = read_imagef(CH_V,sampler, int2(gx_prev,gy_prev)).x;


                                            chw_total += chp_width;

                                            float chp_hs = read_imagef(CH_HS,sampler, int2(gx_prev,gy_prev)).x;
                                            float chp_us = read_imagef(CH_VS,sampler, int2(gx_prev,gy_prev)).x;
                                            float chp_hf = read_imagef(CH_H,sampler, int2(gx_prev,gy_prev)).x;
                                            float chp_uf = read_imagef(CH_V,sampler, int2(gx_prev,gy_prev)).x;
                                            float chp_ifa = read_imagef(CH_SIfa,sampler, int2(gx_prev,gy_prev)).x;
                                            float chp_rocksize = read_imagef(CH_SRocksize,sampler, int2(gx_prev,gy_prev)).x;
                                            float chp_density = read_imagef(CH_SDensity,sampler, int2(gx_prev,gy_prev)).x;

                                            ch_z_x1 = ch_z_x1 + chp_width * chp_z;
                                            ch_hs_x1 = ch_hs_x1 + chp_width * chp_hs;
                                            ch_us_x1 = ch_us_x1 + chp_width * chp_us;
                                            ch_hf_x1 = ch_hf_x1 + chp_width * chp_hf;
                                            ch_uf_x1 = ch_uf_x1 + chp_width * chp_uf;
                                            ch_ifa_x1 = ch_ifa_x1 + chp_width * chp_ifa;
                                            ch_rocksize_x1 = ch_rocksize_x1 + chp_width * chp_rocksize;
                                            ch_density_x1 = ch_density_x1 + chp_width * chp_density;

                                            float chp_vol = chp_h * chp_width * dx;
                                            float chp_vols = chp_hs * chp_width * dx;

                                            //flow in from previous cell

                                            //fluid
                                            float3 hll_x2 = F_HLL2(chp_h,chp_v,0.0f,ch_h,ch_v,0.0f);
                                            float chp_q = (dt/dx)*(min(ch_width,chp_width)/dx)*((dx * 0.5f*(chn_width +ch_width)) *hll_x2.x);
                                            chp_q = min(0.25f * chp_vol,chp_q);
                                            chp_q = max(-0.25f * ch_vol,chp_q);
                                            chp_q = chp_q * 0.5f;

                                            float chp_slope = max(0.0f,(float)((chp_z + chp_h- z - ch_h)/dx));
                                            if(chp_q > 0)
                                            {

                                                    float new_ch_vol = chhn*(ch_width*dx);
                                                    chvn = (chvn * new_ch_vol + chp_v *(chp_q))/max(0.01f,new_ch_vol +chp_q);
                                            }
                                            ch_vadd = ch_vadd + dt * 0.5f * GRAV * max(-1.0f,min(1.0f,(float)(chp_slope)));
                                            ch_vaddw = ch_vaddw + 0.5f * chp_width/ch_width;

                                            chhn = chhn +chp_q/(ch_width * dx);
                                            flux_chx1 = flux_chx1 + chp_q;

                                            //solid
                                            float3 hlls_x2 = F_HLL2SF(chp_hs,chp_hf,chp_us,0,ch_hs,ch_h,ch_us,0,ch_ifa,ch_rocksize,ch_density,dragmult);
                                            float chp_qs = (dt/dx)*(min(ch_width,chp_width)/dx)*((dx * 0.5f*(chn_width +ch_width)) *hlls_x2.x);
                                            chp_qs = min(0.25f * chp_vols,chp_qs);
                                            chp_qs = max(-0.25f * ch_vols,chp_qs);
                                            chp_qs = chp_qs * 0.5f;

                                            float chp_slopes = max(0.0f,(float)((chp_z + chp_hs- z - ch_hs)/dx));
                                            if(chp_qs > 0.0f)
                                            {

                                                    float new_ch_vol = chhsn*(ch_width*dx);
                                                    chvsn = (chvsn * new_ch_vol + chp_us *(chp_qs))/max(0.01f,new_ch_vol +chp_qs);
                                            }
                                            chhsn = chhsn +chp_qs/(ch_width * dx);




                                    }
                            }
                    }
                    if(ch_vaddw > 1)
                    {
                            ch_vadd = ch_vadd/ch_vaddw;
                    }

                    if(chw_total > 0.001f)
                    {

                            ch_z_x1 = ch_z_x1/chw_total;
                            ch_hs_x1 = ch_hs_x1/chw_total;
                            ch_us_x1 = ch_us_x1/chw_total;
                            ch_hf_x1 = ch_hf_x1/chw_total;
                            ch_uf_x1 = ch_uf_x1/chw_total;
                            ch_ifa_x1 = ch_ifa_x1/chw_total;
                            ch_rocksize_x1 = ch_rocksize_x1/chw_total;
                            ch_density_x1 = ch_density_x1/chw_total;

                            ch_z = max(ch_z_x1,ch_z);
                            ch_z_x2 = max(ch_z_x2,ch_z);
                    }else
                    {
                            ch_z_x1 = ch_z;
                    }


                    chvn = GetVNFX(chvn,chhn, dt, dx, 0.05f,
                               ch_z, ch_z_x1, ch_z_x2,0.0f, 0.0f,
                               ch_hs, ch_hs_x1, ch_hs_x2,0.0f, 0.0f,
                               ch_us, ch_us_x1, ch_us_x2, 0.0f, 0.0f,
                               0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                               0.0f,0.0f,0.0f,0.0f,
                               0.0f,0.0f,0.0f,0.0f,
                               ch_hf, ch_hf_x1, ch_hf_x2, 0.0f,0.0f,
                               ch_uf, ch_uf_x1, ch_uf_x2, 0.0f,0.0f,
                               0.0f, 0.0f, 0.0f, 0.0f,0.0f,
                               0.0f,0.0f,0.0f,0.0f,
                               0.0f,0.0f,0.0f,0.0f,
                               ch_ifa, ch_rocksize, ch_density, dragmult);

                    chvsn = GetVNSX(chvsn,chhsn, dt, dx, 0.05,
                               ch_z, ch_z_x1, ch_z_x2,0.0f, 0.0f,
                               ch_hs, ch_hs_x1, ch_hs_x2,0.0f, 0.0f,
                               ch_us, ch_us_x1, ch_us_x2, 0.0f, 0.0f,
                               0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                               0.0f,0.0f,0.0f,0.0f,
                               0.0f,0.0f,0.0f,0.0f,
                               ch_hf, ch_hf_x1, ch_hf_x2, 0.0f,0.0f,
                               ch_uf, ch_uf_x1, ch_uf_x2, 0.0f,0.0f,
                               0.0f, 0.0f, 0.0f, 0.0f,0.0f,
                               0.0f,0.0f,0.0f,0.0f,
                               0.0f,0.0f,0.0f,0.0f,
                               ch_ifa, ch_rocksize, ch_density, dragmult);

                    if(chhsn < 0.1f)
                    {
                        chvsn = min(chhsn* 5.0f,chvsn);
                    }


                    chvn = chvn + ch_vadd;
                    float chvsq = sqrt((float)(chvn * chvn));
                    float chnsq1 = (0.1f)*(0.1f)*GRAV/pow((float)(chhn),(float)(4.0f/3.0f));
                    float chnsq = chnsq1*chvsq*dt;
                    chvn = min(25.0f,max(-25.0f,(float)(chvn/(1.0f+chnsq))));

                    chvsn = chvsn;// + ch_vadd;
                    chvsq = sqrt((float)(chvsn * chvsn));
                    chnsq1 = (1.0)*(1.0)*GRAV/pow((float)(chhsn),(float)(4.0/3.0));
                    chnsq = chnsq1*chvsq*dt;
                    chvsn = min(25.0f,max(-25.0f,(float)(chvsn/(1.0f+chnsq))));


                    chhn = min(99.0f,max(0.0f,(float)(chhn)));
                    chhsn = min(99.0f,max(0.0f,(float)(chhsn)));

                    float chh_total = chhn + chhsn;

                    //channel inflow and outflow
                    if(chh_total < ch_height)
                    {
                            float vol_room = (ch_height - chh_total)*ch_width;
                            float v_chin = hn * sqrt(hn) * sqrt(hn/dx)/(0.1f * 0.1f);
                            float flux_in = max(0.0f,min((float)(vol_room), hn *(dx*dx) *min(0.05f,max(0.0f,(float)( dt * v_chin/max(1.0f,(float)(dx - ch_width)))))));

                            float frac_s = hsn / max(0.001f,hsn + hn);
                            float frac_f = hn / max(0.001f,hsn + hn);

                            chhn = chhn + frac_f * flux_in/(dx * ch_width);
                            chhsn = chhsn + frac_s * flux_in/(dx * ch_width);
                            hn = hn - frac_f *flux_in/(dx * dx);
                            hsn = hsn - frac_s *flux_in/(dx * dx);
                            write_imagef(QFCHIN, int2(gx,gy),- frac_f *flux_in);

                    }else if(chh_total > ch_height)
                    {
                            float vol_tomuch = max(0.0f,(chh_total - ch_height)*ch_width);
                            float flux_out = vol_tomuch;
                            float frac_s = chhsn / max(0.001f,chhsn + chhn);
                            float frac_f = chhn / max(0.001f,chhsn + chhn);
                            chhn = chhn - frac_f * flux_out;
                            chhsn = chhsn - frac_s * flux_out;
                            hn = hn + frac_f*vol_tomuch/(dx * dx);
                            hsn = hsn + frac_s*vol_tomuch/(dx * dx);
                            write_imagef(QFCHIN, int2(gx,gy), frac_f*vol_tomuch);
                    }else
                    {
                            write_imagef(QFCHIN, int2(gx,gy), 0.0f);
                    }

                    write_imagef(CH_HN, int2(gx,gy), chhn);
                    write_imagef(CH_VN, int2(gx,gy), chvn);
                    write_imagef(CH_HSN, int2(gx,gy), chhsn);
                    write_imagef(CH_VSN, int2(gx,gy), chvsn);
                    write_imagef(CH_SDensityN, int2(gx,gy), chdensityn);
                    write_imagef(CH_SRocksizeN, int2(gx,gy), chrocksizen);
                    write_imagef(CH_SIfaN, int2(gx,gy), chifan);

                    dt_req = 0.25f * min(dt_req,dx/(min(100.0f,max(0.01f,max(sqrt(chvn*chvn),sqrt(chvsn*chvsn))))));

            }else //end if(channel)
            {
                    write_imagef(CH_HN, int2(gx,gy), 0.0f);
                    write_imagef(CH_VN, int2(gx,gy), 0.0f);
                    write_imagef(CH_HSN, int2(gx,gy), 0.0f);
                    write_imagef(CH_VSN, int2(gx,gy), 0.0f);
                    write_imagef(CH_SDensityN, int2(gx,gy), 0.0f);
                    write_imagef(CH_SRocksizeN, int2(gx,gy), 0.0f);
                    write_imagef(CH_SIfaN, int2(gx,gy), 0.0f);
            } //end if(channel)else

            write_imagef(QFCHX1, int2(gx,gy), flux_chx1);
            write_imagef(QFCHX2, int2(gx,gy), flux_chx2);

            write_imagef(QFOUTN, int2(gx,gy),qfout);

            write_imagef(QSOUTN, int2(gx,gy),qsout);

            write_imagef(DTREQ, int2(gx,gy),dt_req);
    }

#ifdef LISEM_KERNEL_CPUCODE
}
#endif

}

kernel
void flowsolid2(
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
				__write_only image2d_t H,
				__write_only image2d_t Vx,
				__write_only image2d_t Vy,
				__write_only image2d_t HS,
				__write_only image2d_t VSx,
				__write_only image2d_t VSy,
				__write_only image2d_t SIfa,
				__write_only image2d_t SRocksize,
				__write_only image2d_t SDensity,
				__write_only image2d_t InfilCum,
				__write_only image2d_t CH_H,
				__write_only image2d_t CH_V,
				__write_only image2d_t CH_HS,
				__write_only image2d_t CH_VS,
				__write_only image2d_t CH_SIfa,
				__write_only image2d_t CH_SRocksize,
				__write_only image2d_t CH_SDensity,
				__read_only image2d_t HN,
				__read_only image2d_t VxN,
				__read_only image2d_t VyN,
				__read_only image2d_t HSN,
				__read_only image2d_t VSxN,
				__read_only image2d_t VSyN,
				__read_only image2d_t SIfaN,
				__read_only image2d_t SRocksizeN,
				__read_only image2d_t SDensityN,
				__read_only image2d_t InfilAct,
				__read_only image2d_t CH_HN,
				__read_only image2d_t CH_VN,
				__read_only image2d_t CH_HSN,
				__read_only image2d_t CH_VSN,
				__read_only image2d_t CH_SIfaN,
				__read_only image2d_t CH_SRocksizeN,
                                __read_only image2d_t CH_SDensityN,
                                __read_only image2d_t HFMAX,
                                __read_only image2d_t VFMAX,
                                __write_only image2d_t HFMAXN,
                                __write_only image2d_t VFMAXN,
                                __read_only image2d_t HSMAX,
                                __read_only image2d_t VSMAX,
                                __write_only image2d_t HSMAXN,
                                __write_only image2d_t VSMAXN
				)
{

#ifdef LISEM_KERNEL_CPUCODE

#pragma omp parallel for
for(int id_1d_cpu = 0; id_1d_cpu < id_1d_cpum; id_1d_cpu++)
{
#endif

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
	
        float h = max(0.0f,(float)(read_imagef(HN,sampler, int2(gx,gy)).x));
        float infil_left = read_imagef(InfilAct,sampler, int2(gx,gy)).x;
        float vx = read_imagef(VxN,sampler, int2(gx,gy)).x;
        float vy = read_imagef(VyN,sampler, int2(gx,gy)).x;

        float chh = read_imagef(CH_HN,sampler, int2(gx,gy)).x;
        float chv = read_imagef(CH_VN,sampler, int2(gx,gy)).x;
	
        float chhs = read_imagef(CH_HSN,sampler, int2(gx,gy)).x;
        float chvs = read_imagef(CH_VSN,sampler, int2(gx,gy)).x;
	
        float chsdensity = read_imagef(CH_SDensityN,sampler, int2(gx,gy)).x;
        float chsifa = read_imagef(CH_SIfaN,sampler, int2(gx,gy)).x;
        float chsrocksize = read_imagef(CH_SRocksizeN,sampler, int2(gx,gy)).x;
	
        float hs = read_imagef(HSN,sampler, int2(gx,gy)).x;
        float vsx = read_imagef(VSxN,sampler, int2(gx,gy)).x;
        float vsy = read_imagef(VSyN,sampler, int2(gx,gy)).x;
        float sifa = read_imagef(SIfaN,sampler, int2(gx,gy)).x;
        float srocksize = read_imagef(SRocksizeN,sampler, int2(gx,gy)).x;
        float sdensity = read_imagef(SDensityN,sampler, int2(gx,gy)).x;
	
        write_imagef(HS, int2(gx,gy), hs);
        write_imagef(VSx, int2(gx,gy), vsx);
        write_imagef(VSy, int2(gx,gy), vsy);
	
        write_imagef(SIfa, int2(gx,gy), sifa);
        write_imagef(SRocksize, int2(gx,gy), srocksize);
        write_imagef(SDensity, int2(gx,gy), sdensity);
	
        write_imagef(CH_HS, int2(gx,gy), chhs);
        write_imagef(CH_VS, int2(gx,gy), chvs);
	
        write_imagef(CH_SDensity, int2(gx,gy), chsdensity);
        write_imagef(CH_SIfa, int2(gx,gy), chsifa);
        write_imagef(CH_SRocksize, int2(gx,gy), chsrocksize);
	
        write_imagef(InfilCum, int2(gx,gy), infil_left);
        write_imagef(H, int2(gx,gy), h);
        write_imagef(Vx, int2(gx,gy), vx);
        write_imagef(Vy, int2(gx,gy), vy);
        write_imagef(CH_H, int2(gx,gy), chh);
        write_imagef(CH_V, int2(gx,gy), chv);



        float hfmax = read_imagef(HFMAX,sampler, int2(gx,gy)).x;
        float vfmax = read_imagef(VFMAX,sampler, int2(gx,gy)).x;
        float vf =(vx*vx + vy*vy > 0)? sqrt(vx*vx + vy*vy) : 0.0f;
        write_imagef(HFMAXN, int2(gx,gy), max(h,hfmax));
        write_imagef(VFMAXN, int2(gx,gy), max(vf,vfmax));

        float hsmax = read_imagef(HSMAX,sampler, int2(gx,gy)).x;
        float vsmax = read_imagef(VSMAX,sampler, int2(gx,gy)).x;
        float vs =(vsx*vsx + vsy*vsy > 0)? sqrt(vsx*vsx + vsy*vsy) : 0.0f;
        write_imagef(HSMAXN, int2(gx,gy), max(hs,hsmax));
        write_imagef(VSMAXN, int2(gx,gy), max(vs,vsmax));

#ifdef LISEM_KERNEL_CPUCODE
}
#endif
}



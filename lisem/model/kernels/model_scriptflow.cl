
#define GRAV 9.81
#define nonkernel




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
#endif


nonkernel float maxmod(float x, float y)
{
    if(y < 0.0f)
    {
        return min(-x,y);
    }else
    {
        return max(x,y);
    }
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

nonkernel float fcabs(float f)
{
    if(f > 0.0)
    {
        return f;
    }else
    {
        return -f;
    }
}

nonkernel float3 F_HLL2FB(float h_L,float u_L,float v_L, float fb_L, float h_R,float u_R,float v_R, float fb_R)
{
    float3 ret;

    float grav_h_L = GRAV*h_L;
    float grav_h_R = GRAV*h_R;
    float sqrt_grav_h_L = sqrt((float)(grav_h_L));  // wave velocity
    float sqrt_grav_h_R = sqrt((float)(grav_h_R));
    float q_R = fb_R * u_R*h_R;
    float q_L = fb_L * u_L*h_L;

    float sc1 = min((float)(u_L - sqrt_grav_h_L),(float)(u_R - sqrt_grav_h_R));
    float sc2 = max((float)(u_L + sqrt_grav_h_L),(float)(u_R + sqrt_grav_h_R));
    float tmp = 1.0/max((float)0.1,(float)(sc2-sc1));
    float t1 = (min((float)(sc2),(float)(0.)) - min((float)(sc1),(float)(0.)))*tmp;
    float t2 = 1.0 - t1;
    float t3_1 = fabs((float)(sc1));
    float t3_2 = fabs((float)(sc2));
    float t3 =(sc2* t3_1 - sc1* t3_2)*0.5*tmp;

    ret.x = t1*q_R + t2*q_L - t3*0.5*(fb_R + fb_L)*(h_R - h_L);
    ret.y = t1*(q_R*u_R + grav_h_R*h_R*0.5) + t2*(q_L*u_L + grav_h_L*h_L*0.5) - t3*0.5*(fb_R + fb_L)*(q_R - q_L);
    ret.z = t1*q_R*v_R + t2*q_L*v_L - t3*0.5*(fb_R + fb_L)*(h_R*v_R - h_L*v_L);

    return ret;
}

nonkernel float3 F_HLL2(float h_L,float u_L,float v_L,float h_R,float u_R,float v_R)
{
		float3 ret;
		
        float grav_h_L = GRAV*h_L;
        float grav_h_R = GRAV*h_R;
        float sqrt_grav_h_L = sqrt((float)(grav_h_L));  // wave velocity
        float sqrt_grav_h_R = sqrt((float)(grav_h_R));
        float q_R = u_R*h_R;
        float q_L = u_L*h_L;

        float sc1 = min((float)(u_L - sqrt_grav_h_L),(float)(u_R - sqrt_grav_h_R));
        float sc2 = max((float)(u_L + sqrt_grav_h_L),(float)(u_R + sqrt_grav_h_R)); 
        float tmp = 1.0/max((float)0.1,(float)(sc2-sc1));
        float t1 = (min((float)(sc2),(float)(0.)) - min((float)(sc1),(float)(0.)))*tmp;
        float t2 = 1.0 - t1;
        float t3_1 = fabs((float)(sc1));
        float t3_2 = fabs((float)(sc2));
        float t3 =(sc2* t3_1 - sc1* t3_2)*0.5*tmp;
		
        ret.x = t1*q_R + t2*q_L - t3*(h_R - h_L);
        ret.y = t1*(q_R*u_R + grav_h_R*h_R*0.5) + t2*(q_L*u_L + grav_h_L*h_L*0.5) - t3*(q_R - q_L);
        ret.z = t1*q_R*v_R + t2*q_L*v_L - t3*(h_R*v_R - h_L*v_L);
		
		return ret;
}




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


        float dc = (dc_L * h_L + dc_R * h_R)/max(0.0001f,(h_L + h_R));
        float ff_tot = max(0.001f,max(ho_L,ho_R)/max(0.0001f,max(h_L + ho_L,h_R + ho_R)));

        ff_tot = max(0.0f,min(1.0f, (ff_tot -0.2f) * 10.0f));

        float ffac_L = (1.0f- ff_tot) * max(0.0f,min(1.0f,exp(-dc_L)));
        float ffac_R = (1.0f- ff_tot) * max(0.0f,min(1.0f,exp(-dc_R)));

        //and pressure-momentum based on the solid content
        float grav_h_L = GRAV*h_L;//*  pow(min(1.0f,max(0.0f,(1.0f-1.25f*sf_dev_L))),4.0f);
        float grav_h_R = GRAV*h_R;//*  pow(min(1.0f,max(0.0f,(1.0f-1.25f*sf_dev_R))),4.0f) ;
        float sqrt_grav_h_L = sqrt((float)(grav_h_L));// * fac_dc_L;  // wave velocity
        float sqrt_grav_h_R = sqrt((float)(grav_h_R));// * fac_dc_R ;
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

        ret.x = ret.x * (1.0f- ff_tot) + ff_tot*retnw.x;
        ret.y = ret.y * (1.0f- ff_tot) + ff_tot*retnw.y;
        ret.z = ret.z * (1.0f- ff_tot) + ff_tot*retnw.z;

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


        float dc = (dc_L * h_L + dc_R * h_R)/max(0.0001f,(h_L + h_R));
        float ff_tot = max(0.001f,max(h_L,h_R)/max(0.0001f,max(h_L + ho_L,h_R + ho_R)));

        ff_tot = max(0.0f,min(1.0f,(ff_tot -0.2f) * 10.0f));

        float ffac_L = (1.0f- ff_tot) * max(0.0f,min(1.0f,exp(-dc_L)));
        float ffac_R = (1.0f- ff_tot) * max(0.0f,min(1.0f,exp(-dc_R)));



        //and pressure-momentum based on the solid content
        float grav_h_L = GRAV*h_L;//*  pow(min(1.0f,max(0.0f,(1.0f-1.25f*sf_dev_L))),4.0f);
        float grav_h_R = GRAV*h_R;//*  pow(min(1.0f,max(0.0f,(1.0f-1.25f*sf_dev_R))),4.0f) ;
        float sqrt_grav_h_L = sqrt((float)(grav_h_L));  // wave velocity
        float sqrt_grav_h_R = sqrt((float)(grav_h_R));
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


        ret.x = ret.x * (1.0f- ff_tot) + ff_tot*retnw.x;
        ret.y = ret.y * (1.0f- ff_tot) + ff_tot*retnw.y;
        ret.z = ret.z * (1.0f- ff_tot) + ff_tot*retnw.z;

        //switch betweeen non-shock capturing gudanov and shock-capturing hll solver based on solid content
        return retnw;
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
        float v_balance = us;//(fac_dc )*(ff * vn + sf * us) + (1.0f - fac_dc) * us;
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
        float v_balance = vs;//(fac_dc )*(ff * vn + sf * vs) + (1.0f - fac_dc) * vs;

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

    float vn_ifa = vn > 0.0f? max(0.0f,(float)(vn - max(0.0f,(sf - ff * (1000.0f/density))) * dt *GRAV * tan(ifa))) : min(0.0f,(float)(vn + max(0.0f,(sf - ff * (1000.0f/density))) * dt * GRAV * tan(ifa)));
    float fac_dc = min(1.0f,max(0.0f,(exp(-(dc * vpow/ff_dev)))/tan(ifa)));// + ((sf_dev *density)/(1000.0f))*tan(ifa)
    vn = vn_ifa *fac_dc + vn_ifa*(1.0f-fac_dc);

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

        float vn_ifa = vn > 0.0? max(0.0f,(float)(vn - max(0.0f,(sf - ff * (1000.0f/density))) * dt *GRAV * tan(ifa))) : min(0.0f,(float)(vn + max(0.0f,(sf - ff * (1000.0f/density))) * dt * GRAV * tan(ifa)));
        float fac_dc = min(1.0f,max(0.0f,(exp(-(dc * vpow/ff_dev)))/tan(ifa)));// + ((sf_dev *density)/(1000.0f))*tan(ifa)
        vn = vn_ifa *fac_dc + vn_ifa*(1.0f-fac_dc);
        return vn;
}





kernel
void flow_dt( int dim0,
              int dim1,
              float dx,
              __read_only image2d_t DEM,
              __read_only image2d_t N,
              __read_only image2d_t H,
              __read_only image2d_t Vx,
              __read_only image2d_t Vy)
{

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

    const int gx_x11 = min(dim0-(int)(1),max((int)(0),(int)(gx - 2 )));
        const int gy_x11 = gy;
    const int gx_x22 = min(dim0-(int)(1),max((int)(0),(int)(gx + 2)));
        const int gy_x22 = gy;
    const int gx_y11 = gx;
        const int gy_y11 = min(dim1-(int)(1),max((int)(0),(int)(gy - 2)));
    const int gx_y22 = gx;
        const int gy_y22 = min(dim1-(int)(1),max((int)(0),(int)(gy + 2)));

        float h = max(0.0f,read_imagef(H,sampler, int2(gx,gy)).x);

        float vx = min(vmax,max(-vmax,read_imagef(Vx,sampler, int2(gx,gy)).x));
        float vy = min(vmax,max(-vmax,read_imagef(Vy,sampler, int2(gx,gy)).x));


    float dt_req = 0.25f *dx/( min(100.0f,max(0.01f,(sqrt(vx*vx + vy * vy)))));




}

kernel
void flow( int dim0,
              int dim1,
              float dx,
              float dt
              __read_only image2d_t DEM,
              __read_only image2d_t N,
              __read_only image2d_t H,
              __read_only image2d_t Vx,
              __read_only image2d_t Vy,
              __write_only image2d_t HN,
              __write_only image2d_t VxN,
              __write_only image2d_t VyN)
{

    float tx = dt/dx;


    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

    const int id_1d = (int)(get_global_id(0));

    const int idx = trunc((float)(id_1d)/(float)(dim1));
    const int idy = id_1d % dim1;

    const int gx_x = min(dim0-(int)(1),max((int)(0),(int)(idx)));
    const int gy_y = min(dim1-(int)(1),max((int)(0),(int)(idy)));

    const int gy = min(dim1-(int)(1),max((int)(0),(int)(gy_y)));
    const int gx = min(dim0-(int)(1),max((int)(0),(int)(gx_x)));

    //const int gx = min(dim0-(int)(1),max((int)(0),(int)(get_global_id(0))));
    //const int gy = min(dim1-(int)(1),max((int)(0),(int)(get_global_id(1))));

    const int gx_x1 = min(dim0-(int)(1),max((int)(0),(int)(gx - 1 )));
    const int gy_x1 = gy;
    const int gx_x2 = min(dim0-(int)(1),max((int)(0),(int)(gx + 1)));
    const int gy_x2 = gy;
    const int gx_y1 = gx;
    const int gy_y1 = min(dim1-(int)(1),max((int)(0),(int)(gy - 1)));
    const int gx_y2 = gx;
    const int gy_y2 = min(dim1-(int)(1),max((int)(0),(int)(gy + 1)));

    const int gx_x11 = min(dim0-(int)(1),max((int)(0),(int)(gx - 2 )));
        const int gy_x11 = gy;
    const int gx_x22 = min(dim0-(int)(1),max((int)(0),(int)(gx + 2)));
        const int gy_x22 = gy;
    const int gx_y11 = gx;
        const int gy_y11 = min(dim1-(int)(1),max((int)(0),(int)(gy - 2)));
    const int gx_y22 = gx;
        const int gy_y22 = min(dim1-(int)(1),max((int)(0),(int)(gy + 2)));

    float z = read_imagef(DEM,sampler, int2(gx,gy)).x;
    float man_n = read_imagef(N,sampler, int2(gx,gy)).x;
    float n = man_n + 0.001f;
    if(z > -1000)
    {

        float vmax = 0.25 * dx/dt;

        float z_x1 = zhc_x1 + read_imagef(DEM,sampler, int2(gx_x1,gy)).x;
        float z_x2 = zhc_x2 + read_imagef(DEM,sampler, int2(gx_x2,gy)).x;
        float z_y1 = zhc_y1 + read_imagef(DEM,sampler, int2(gx,gy_y1)).x;
        float z_y2 = zhc_y2 + read_imagef(DEM,sampler, int2(gx,gy_y2)).x;

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


        float h = max(0.0f,read_imagef(H,sampler, int2(gx,gy)).x);
        float h_x1 = max(0.0f,read_imagef(H,sampler, int2(gx_x1,gy)).x);
        float h_x2 = max(0.0f,read_imagef(H,sampler, int2(gx_x2,gy)).x);
        float h_y1 = max(0.0f,read_imagef(H,sampler, int2(gx,gy_y1)).x);
        float h_y2 = max(0.0f,read_imagef(H,sampler, int2(gx,gy_y2)).x);

        float h_x11 = max(0.0f,read_imagef(H,sampler, int2(gx_x11,gy)).x);
        float h_x22 = max(0.0f,read_imagef(H,sampler, int2(gx_x22,gy)).x);
        float h_y11 = max(0.0f,read_imagef(H,sampler, int2(gx,gy_y11)).x);
        float h_y22 = max(0.0f,read_imagef(H,sampler, int2(gx,gy_y22)).x);


        float zcc_x1 =max(z,zc_x1);
        float zcc_x2 =max(z,zc_x2);
        float zcc_y1 =max(z,zc_y1);
        float zcc_y2 =max(z,zc_y2);

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


        float factor_flowx1f = 1.0-min(1.0f,max(0.0f,z-z_x1)/max(1e-6f,h_x1));
        float factor_flowy1f = 1.0-min(1.0f,max(0.0f,z-z_y1)/max(1e-6f,h_y1));
        float factor_flowx2f = 1.0-min(1.0f,max(0.0f,z-z_x2)/max(1e-6f,h_x2));
        float factor_flowy2f = 1.0-min(1.0f,max(0.0f,z-z_y2)/max(1e-6f,h_y2));

        float factor_flowx1t = 1.0-min(1.0f,max(0.0f,z_x1-z)/max(1e-6f,h));
        float factor_flowy1t = 1.0-min(1.0f,max(0.0f,z_y1-z)/max(1e-6f,h));
        float factor_flowx2t = 1.0-min(1.0f,max(0.0f,z_x2-z)/max(1e-6f,h));
        float factor_flowy2t = 1.0-min(1.0f,max(0.0f,z_y2-z)/max(1e-6f,h));

        float sx_zh_x2 = min((float)(0.5),max((float)(-0.5),(float)((zc_x2 + h_x2-z - h)/dx)));
        float sy_zh_y1 = min((float)(0.5),max((float)(-0.5),(float)((z + h-zc_y1 - h_y1)/dx)));
        float sx_zh_x1 = min((float)(0.5),max((float)(-0.5),(float)((z + h-zc_x1 - h_x1)/dx)));
        float sy_zh_y2 = min((float)(0.5),max((float)(-0.5),(float)((zc_y2 + h_y2-z - h)/dx)));

        float sx_zh = min(1.0f,max(-1.0f,minmod(sx_zh_x1,sx_zh_x2)));
        float sy_zh = min(1.0f,max(-1.0f,minmod(sy_zh_y1,sy_zh_y2)));

        float sx_z_x2 = min((float)(0.5),max((float)(-0.5),(float)((zc_x2 -z)/dx)));
        float sy_z_y1 = min((float)(0.5),max((float)(-0.5),(float)((z -zc_y1 )/dx)));
        float sx_z_x1 = min((float)(0.5),max((float)(-0.5),(float)((z -zc_x1 )/dx)));
        float sy_z_y2 = min((float)(0.5),max((float)(-0.5),(float)((zc_y2 -z )/dx)));

        float sx_z = min(1.0f,max(-1.0f,minmod(sx_z_x1,sx_z_x2)));
        float sy_z = min(1.0f,max(-1.0f,minmod(sy_z_y1,sy_z_y2)));

        float h_corr_x1 = max(0.0f,(h_x1 -max(0.0f,z - z_x1)));
        float h_corr_x1b = max(0.0f,(h -max(0.0f,z_x1-z)));
        float h_corr_x2 = max(0.0f,(h -max(0.0f,z_x2 - z)));
        float h_corr_x2b = max(0.0f,(h_x2 -max(0.0f,z - z_x2)));
        float h_corr_y1 = max(0.0f,(h_y1 -max(0.0f,z - z_y1)));
        float h_corr_y1b = max(0.0f,(h -max(0.0f,z_y1 - z)));
        float h_corr_y2 = max(0.0f,(h -max(0.0f,z_y2 - z)));
        float h_corr_y2b = max(0.0f,(h_y2 -max(0.0f,z - z_y2)));
        float3 hll_x1 = z_x1 < -1000? float3(0.0,0.0,0.0):F_HLL2(h_corr_x1,vx_x1,vy_x1,h_corr_x1b,vx,vy);
        float3 hll_x2 = z_x2 < -1000? float3(0.0,0.0,0.0):F_HLL2(h_corr_x2,vx,vy,h_corr_x2b,vx_x2,vy_x2);
        float3 hll_y1 = z_y1 < -1000? float3(0.0,0.0,0.0):F_HLL2(h_corr_y1,vy_y1,vx_y1,h_corr_y1b,vy,vx);
        float3 hll_y2 = z_y2 < -1000? float3(0.0,0.0,0.0):F_HLL2(h_corr_y2,vy,vx,h_corr_y2b,vy_y2,vx_y2);


        float C = 0.25f;

        float fluxo_x1 = +tx*(hll_x1.x)
        float fluxo_x2 = -tx*(hll_x2.x) + (blockvelx > 0? -h * dt *capture_x * blockvelx/dx : 0.0) + (blockvelx_x2 < 0? -h_x2 * dt *capture_x2 * blockvelx_x2/dx : 0.0);
        float fluxo_y1 = +tx*(hll_y1.x) + (blockvely < 0? h * dt *capture_y * blockvely/dx : 0.0) + (blockvely_y1 > 0? h_y1 * dt *capture_y1 * blockvely_y1/dx : 0.0);
        float fluxo_y2 = -tx*(hll_y2.x) + (blockvely > 0? -h * dt *capture_y * blockvely/dx : 0.0) + (blockvely_y2 < 0? -h_y2 * dt *capture_y2 * blockvely_y2/dx : 0.0);

        float flux_x1 = z_x1 < -1000? max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):max(-h  * factor_flowx1t * C,min((float)(fluxo_x1),h_x1 *factor_flowx1f *  C));//max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
        float flux_x2 = z_x2 < -1000? max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):max(-h  * factor_flowx2t * C,min((float)(fluxo_x2),h_x2 *factor_flowx2f *  C));//max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
        float flux_y1 = z_y1 < -1000? max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):max(-h  * factor_flowy1t * C,min((float)(fluxo_y1),h_y1 *factor_flowy1f *  C));//max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
        float flux_y2 = z_y2 < -1000? max(-h * C,(float)(-h * sqrt(h) *dt*  sqrt(h)/(dx*(0.001+n)))):max(-h  * factor_flowy2t * C,min((float)(fluxo_y2),h_y2 *factor_flowy2f *  C));//max(-h * C,(float)(-h * sqrt(h) *0.0*dt*  sqrt(h)/(dx*(0.001+n))))
/*

        qfout = qfout +z_x1 < -1000? flux_x1 : 0.0;
        qfout = qfout +z_x2 < -1000? flux_x2 : 0.0;
        qfout = qfout +z_y1 < -1000? flux_y1 : 0.0;
        qfout = qfout +z_y2 < -1000? flux_y2 : 0.0;

        write_imagef(QFX1, int2(gx,gy), flux_x1 *(dx*dx));
        write_imagef(QFX2, int2(gx,gy), flux_x2 *(dx*dx));
        write_imagef(QFY1, int2(gx,gy), flux_y1 *(dx*dx));
        write_imagef(QFY2, int2(gx,gy), flux_y2 *(dx*dx));*/

        int edges = ((z_x1 < -1000)?1:0) +((z_x2 < -1000)?1:0)+((z_y1 < -1000)?1:0)+((z_y2 < -1000)?1:0);

        bool edge = (z_x1 < -1000 || z_x2 < -1000 || z_y1 < -1000 || z_y2 < -1000);
        float hold = h;
        float hn = ((max(0.00f,(float)(h + flux_x1 + flux_x2 + flux_y1 + flux_y2))));

        //float qxn = h * vx - 0.5 * GRAV *hn*sx_z * dt- tx*(hll_x2.y - hll_x1.y) - tx*(hll_y2.z - hll_y1.z);
        //float qyn = h * vy - 0.5 * GRAV *hn*sy_z * dt- tx*(hll_x2.z - hll_x1.z) - tx*(hll_y2.y - hll_y1.y);



        float f_centre_x = 0.5 * GRAV*(h)*(((h - h_corr_x2) - (h - h_corr_x1)) > 0.0? 1.0:-1.0)*fcabs((h - h_corr_x2) - (h - h_corr_x1));
        float f_centre_y = 0.5 * GRAV*(h)*(((h - h_corr_y2) - (h - h_corr_y1)) > 0.0? 1.0:-1.0)*fcabs((h - h_corr_y2) - (h - h_corr_y1));

        float qxn = h * vx - tx*(hll_x2.y - hll_x1.y + f_centre_x)  - tx*(hll_y2.z - hll_y1.z);// + fluxmc_x1x + fluxmc_x2x + fluxmc_y1x + fluxmc_y2x ;//- 0.5 * GRAV *hn*sx_z * dt
        float qyn = h * vy - tx*(hll_x2.z - hll_x1.z) - tx*(hll_y2.y - hll_y1.y + f_centre_y);// + fluxmc_x1y + fluxmc_x2y + fluxmc_y1y + fluxmc_y2y ;//- 0.5 * GRAV *hn*sy_z * dt

        float vsq = sqrt((float)(vx * vx + vy * vy));
        float nsq1 = (0.001+n)*(0.001+n)*GRAV/max(0.01f,pow((float)(hn),(float)(4.0/3.0)));
        float nsq = nsq1*vsq*dt;

        float vxn = (float)((qxn/(1.0f+nsq)))/max(0.01f,(float)(hn));
        float vyn = (float)((qyn/(1.0f+nsq)))/max(0.01f,(float)(hn));


        float threshold = 0.01 * dx;
        if(hn < threshold)
        {
            float kinfac = max(0.0f,(threshold - hn) / (0.025f * dx));
            float acc_eff = (vxn -vx)/max(0.0001f,dt);

            float v_kin = (sx_zh>0.0?1.0:-1.0) * hn * sqrt(hn) * max(0.001f,sqrt(sx_zh>0.0?sx_zh:-sx_zh))/(0.001f+n);

            vxn = kinfac * v_kin + vxn*(1.0f-kinfac);

        }

        if(hn < threshold)
        {
            float kinfac = max(0.0f,(threshold - hn) / (0.025f * dx));
            float acc_eff = (vyn -vy)/max(0.0001f,dt);

            float v_kin = (sy_zh>0?1:-1) * hn * sqrt(hn) * max(0.001f,sqrt(sy_zh>0?sy_zh:-sy_zh))/(0.001f+n);

            vyn = kinfac * v_kin + vyn*(1.0f-kinfac);

        }

        hn = edges > 2? 0.0f:(isnan(hn)? 0.0:hn);
        vxn = edges > 2? 0.0f:isnan(vxn)? 0.0:vxn;
        vyn = edges > 2? 0.0f:isnan(vyn)? 0.0:vyn;

        vxn = min(vmax,max(-vmax,vxn));
        vyn = min(vmax,max(-vmax,vyn));


        write_imagef(HN, int2(gx,gy), hn);
        write_imagef(VxN, int2(gx,gy), vxn);
        write_imagef(VyN, int2(gx,gy), vyn);
    }

}

kernel
void flowsolid( int dim0,
                int dim1,
                float dx,
                float dt,
                __read_only image2d_t DEM,
                __read_only image2d_t N,
                __read_only image2d_t H,
                __read_only image2d_t Vx,
                __read_only image2d_t Vy,
                __read_only image2d_t HS,
                __read_only image2d_t VSx,
                __read_only image2d_t VSy,
                __read_only image2d_t SIfa,
                __read_only image2d_t SRocksize,
                __read_only image2d_t SDensity,
                __write_only image2d_t Hn,
                __write_only image2d_t Vxn,
                __write_only image2d_t Vyn,
                __write_only image2d_t HSn,
                __write_only image2d_t VSxn,
                __write_only image2d_t VSyn,
                __write_only image2d_t SIfan,
                __write_only image2d_t SRocksizen,
                __write_only image2d_t SDensityn)
{


    float tx = dt/dx;

    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

    const int id_1d = (int)(get_global_id(0));

    const int idx = trunc((float)(id_1d)/(float)(dim1));
    const int idy = id_1d % dim1;

    const int gx_x = min(dim0-(int)(1),max((int)(0),(int)(idx)));
    const int gy_y = min(dim1-(int)(1),max((int)(0),(int)(idy)));

    const int gy = min(dim1-(int)(1),max((int)(0),(int)(gy_y)));
    const int gx = min(dim0-(int)(1),max((int)(0),(int)(gx_x)));

    const int gx_x1 = min(dim0-(int)(1),max((int)(0),(int)(gx - 1 )));
    const int gy_x1 = gy;
    const int gx_x2 = min(dim0-(int)(1),max((int)(0),(int)(gx + 1)));
    const int gy_x2 = gy;
    const int gx_y1 = gx;
    const int gy_y1 = min(dim1-(int)(1),max((int)(0),(int)(gy - 1)));
    const int gx_y2 = gx;
    const int gy_y2 = min(dim1-(int)(1),max((int)(0),(int)(gy + 1)));

    float z = read_imagef(DEM,sampler, int2(gx,gy)).x;
    float man_n = read_imagef(N,sampler, int2(gx,gy)).x;
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
        float3 hlls_x2 = z_x2 < -1000.0f? float3(0.0f,0.0f,0.0f):F_HLL2SF(max(0.0f,hs -max(0.0f,z_x2 - z)),max(0.0f,h -max(0.0f,z_x2 - z)),vsx,vsy,max(0.0f,hs_x2 -max(0.0f,z - z_x2)),max(0.0f,h_x2 -max(0.0f,z - z_x2)),vsx_x2,vsy_x2,sifa, srocksize, sdensity, dragmult);
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

    }

}

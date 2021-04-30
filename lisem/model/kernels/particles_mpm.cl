
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



#define PARTICLE_CELL_MAX 250				//maximum number of particles we interact with within a single cell


#ifndef LISEM_KERNEL_CPUCODE


//within sph discretization, we need a smoothing kernel -> "kernel" meaning a spatial weight function
nonkernel float weightfunction(float dis, float dis_ref)
{
        //kernel function should be normalized, differentiable, continuous
        //cubic spline kernel by monaghan (1992)
        float dis_norm = dis/dis_ref;

        //normalizing factor (for two dimensions) = 10.0f/(7.0f*pi*h*h)
        float norm = 10.0f/(7.0f*3.14159f*dis_ref*dis_ref);

        //actual weight
        return dis_norm < 1.0f? norm*(1.0f - (3.0f/2.0f)*dis_norm*dis_norm*(1.0f-0.5f*dis_norm)): ((dis_norm < 2.0f) ? ((norm/4.0f)*(2.0f-dis_norm)*(2.0f-dis_norm)*(2.0f-dis_norm)) : 0.0f);
}

//within sph discretization, we need a smoothing kernel -> "kernel" meaning a spatial weight function
nonkernel float weightfunctionnn(float dis, float dis_ref)
{
        //kernel function should be normalized, differentiable, continuous
        //cubic spline kernel by monaghan (1992)
        float dis_norm = dis/dis_ref;

        //normalizing factor (for two dimensions) = 10.0f/(7.0f*pi*h*h)
        float norm = 0.5;

        //actual weight
        return dis_norm < 1.0f? norm*(1.0f - (3.0f/2.0f)*dis_norm*dis_norm*(1.0f-0.5f*dis_norm)): ((dis_norm < 2.0f) ? ((norm/4.0f)*(2.0f-dis_norm)*(2.0f-dis_norm)*(2.0f-dis_norm)) : 0.0f);
}


//within sph discretization, we need a smoothing kernel -> "kernel" meaning a spatial weight function
nonkernel float weightfunctionderivative(float dis, float dis_ref)
{
        //kernel function should be normalized, differentiable, continuous
        //cubic spline kernel by monaghan (1992)
        float dis_norm = dis/dis_ref;

        //normalizing factor (for two dimensions) = 10.0f/(7.0f*pi*h*h)
        float norm = 10.0f/(7.0f*3.14159f*dis_ref*dis_ref);

        //actual weight
        return dis_norm < 1.0f? norm*((9.0f/4.0f)*dis_norm*dis_norm - 3.0f*dis_norm): ((dis_norm < 2.0f) ? ((norm*3.0f/4.0f)*(2.0f-dis_norm)*(2.0f-dis_norm)) : 0.0f);
}


#endif


//kernel that converts particle data to a raster 
kernel
void _particletomaps(
#ifdef LISEM_KERNEL_CPUCODE
int id_1d_cpum,
#endif
int dim0, 				//horizontal nr of cells
			int dim1, 						//vertical nr of cells
			float dx, 						//cell size (meters)
			float dt,						//time step (seconds)
			__read_only image2d_t LOCX,		//x locations that are part of the simulation
			__read_only image2d_t LOCY,		//y locations that are part of the simulation
			__global float* p_xy,				//location of particles (size 2*n_particles)
			__global float* p_uv,				//velocity of particles (size 2*n_particles)
                        __global float* p_props,
                        __global float* pn_physprops,				//phyiscal parameters of particles (size 4*n_particles)
                        __global int* p_index,			//index to particles sorted by cell in which they lie
			__global int* p_indexc,			//the actual cell in which a particle lies (stored for sorting)
			int i_length,					//total number of particles
			__read_only image2d_t T_INDEX,	//map: for each cell-> within the sorted list, what is the first index at which a particle occurs that has this specific cell as location
			__write_only image2d_t SH,		//map: output solid height
			__write_only image2d_t SU,		//map: output solid velocity in x-direction
                        __write_only image2d_t SV,		//map: output solid velocity in y-direction
                        __write_only image2d_t SRockSize,	//map: output solid velocity in y-direction
                        __write_only image2d_t SDensity,        //map: output solid velocity in y-direction
                        __write_only image2d_t SIfa,            //map: output solid velocity in y-direction
                        __write_only image2d_t SCohesion,        //map: output solid velocity in y-direction
                        __write_only image2d_t SCohesionFrac,        //map: output solid velocity in y-direction
                        __global float* p_fluidprops,		//fluid properties of particles (size 4*n_particles)
                        __write_only image2d_t T_SFLUIDH,
                        __write_only image2d_t T_SFLUIDHADD,
                        float particle_factor
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

	float hsn = 0.0;
	float usn = 0.0;
	float vsn = 0.0;

        float ifan = 0.0;
        float rocksizen = 0.0;
        float densityn = 0.0;
        float cohesionn = 0.0;
        float cohesionfrac = 0.0;
        float fluidvol = 0.0;
        float fluidvoladd = 0.0;

	//we have stored the index of the first particle that was sorted as part of each particular cells
	//lets get these indices for each of the 9 surrounding cells (including center)	
	//we assume here that the maximum radius of influence is less than a single cell width/height
	
        int indexstart = read_imagef(T_INDEX,sampler, int2(gx,gy)).x;
        int indexstart_x1 = read_imagef(T_INDEX,sampler, int2(gx_x1,gy)).x;
        int indexstart_y1 = read_imagef(T_INDEX,sampler, int2(gx,gy_y1)).x;
        int indexstart_x2 = read_imagef(T_INDEX,sampler, int2(gx_x2,gy)).x;
        int indexstart_y2 = read_imagef(T_INDEX,sampler, int2(gx,gy_y2)).x;
        int indexstart_x1y1 = read_imagef(T_INDEX,sampler, int2(gx_x1,gy_y1)).x;
        int indexstart_x1y2 = read_imagef(T_INDEX,sampler, int2(gx_x1,gy_y2)).x;
        int indexstart_x2y1 = read_imagef(T_INDEX,sampler, int2(gx_x2,gy_y1)).x;
        int indexstart_x2y2 = read_imagef(T_INDEX,sampler, int2(gx_x2,gy_y2)).x;

	//for each of these cells, we can start at that index, and keep going untill the particle we check is no longer within that cell
	//we should also check the first one, since new starting indices are only written when a particle is present
	//there are 9 total cells we need to check, so put these in an array to simplify
	
	int indices[9] = {indexstart,indexstart_x1,indexstart_y1,indexstart_x2,indexstart_y2,indexstart_x1y1,indexstart_x1y2,indexstart_x2y1,indexstart_x2y2};
	int indices_gx[9] = {0,-1,0,1,0,-1,-1,1,1};
	int indices_gy[9] = {0,0,-1,0,1,-1,1,-1,1};
	
        float x_c = (0.5 +gx) * dx;
        float y_c = (0.5 +gy) * dx;
				
        float wtotal = 0.0000;
        float whtotal = 0.0000;

	//iterate over each neighbor
	float n_part = 0;
	for(int i = 0; i < 9; i++)
	{
		float n_cell = 0;
		bool in_cell = true;
		int id_iter = indices[i];
				
		int gx_should = gx + indices_gx[i];
		int gy_should = gy + indices_gy[i];
		
		//if the previous particle was in the right cell, we did not go over maximum particles, and are within the length of the array
		while(in_cell && n_cell < PARTICLE_CELL_MAX && id_iter < i_length)
		{
			//now check if the particle we get at the index is in the right cell
			int index_n = p_index[id_iter];
			float x_n = p_xy[2*index_n+0];
			float y_n = p_xy[2*index_n+1];
			
			float u_n = p_uv[2*index_n+0];
			float v_n = p_uv[2*index_n+1];
			

                        float input_ifa = pn_physprops[4*index_n+0];
                        float input_rs = pn_physprops[4*index_n+1];
                        float input_d = pn_physprops[4*index_n+2];
                        float input_coh= pn_physprops[4*index_n+3];
                        float input_fluidvol = p_fluidprops[5*index_n+0];
                        float input_fluidvoladd = p_fluidprops[5*index_n+1];

                        float input_cohfrac= p_props[4*index_n+3];

			int gy_n = min(dim1-(int)(1),max((int)(0),(int)(floor(y_n/dx))));
			int gx_n = min(dim0-(int)(1),max((int)(0),(int)(floor(x_n/dx))));
			
			//is the cell of this particle the right one?
			if(gx_n == gx_should && gy_n == gy_should)
			{
				//found one!
				
				n_part = n_part + 1;
				
				//thus, we have a neighboring particle
				//now we can continue to do actual calculations
				
				float disx = x_c - x_n;
				float disy = y_c - y_n;
				
				float dis = sqrt(disx*disx + disy*disy);
				
				float weight =	weightfunction(dis,0.5*dx);
                float weightnn = weightfunctionnn(dis,0.5*dx);


                                wtotal = wtotal + weight;
                                whtotal = whtotal + 1.0f;

                                hsn = hsn + weightnn * (1.0f);
                                usn = usn + weight * (u_n);
                                vsn = vsn + weight * (v_n);
				
                                ifan = ifan + weight * (input_ifa);
                                rocksizen = rocksizen + weight * (input_rs);
                                densityn = densityn + weight * (input_d);
                                cohesionn = cohesionn + weight * (input_coh);
                                cohesionfrac = cohesionfrac + weight *(input_cohfrac);
                                fluidvol = fluidvol + weight * input_fluidvol;
                                fluidvoladd = fluidvoladd + weight * input_fluidvoladd;

				n_cell = n_cell + 1;
				id_iter = id_iter+1;
				
			}else
                in_cell = false;
                {
                        //since the particles are sorted, end of particles for this cell!

			}
		}	
	}
	
        write_imagef(SH, int2(gx,gy), hsn * 3.584f *dx/particle_factor);
        write_imagef(SU, int2(gx,gy), usn/max(0.00001f,(float)( wtotal)));
        write_imagef(SV, int2(gx,gy), vsn/max(0.00001f,(float)( wtotal)));
	
        write_imagef(T_SFLUIDH, int2(gx,gy), fluidvol*dx*dx);
        write_imagef(T_SFLUIDHADD, int2(gx,gy), fluidvoladd*dx*dx);

        write_imagef(SIfa, int2(gx,gy), ifan/max(0.00001f,(float)( wtotal)));
        write_imagef(SRockSize, int2(gx,gy), rocksizen/max(0.00001f,(float)( wtotal)));
        write_imagef(SDensity, int2(gx,gy), densityn/max(0.00001f,(float)( wtotal)));
        write_imagef(SCohesion , int2(gx,gy), cohesionn/max(0.00001f,(float)( wtotal)));
        write_imagef(SCohesionFrac , int2(gx,gy), cohesionfrac/max(0.00001f,(float)( wtotal)));


#ifdef LISEM_KERNEL_CPUCODE
}
#endif

}

//kernel that uses map data to set particle properties
kernel
void _particlefrommaps(
#ifdef LISEM_KERNEL_CPUCODE
    int id_1d_cpum,
#endif

int dim0, 			//horizontal nr of cells
			int dim1, 						//vertical nr of cells
			float dx, 						//cell size (meters)
			float dt,						//time step (seconds)
                        __global float* p_xy,				//location of particles (size 2*n_particles)
                        __global float* p_uv,				//velocity of particles (size 2*n_particles)
                        __global float* p_sigma,			//stress tensor of particles (size 4*n_particles, 4 = number of dimensions squared)
                        __global float* p_props,			//deviatoric stress tensor of particles (size 4*n_particles, 4 = number of dimensions squared)
                        __global int* p_index,			//index to particles sorted by cell in which they lie
                        __global int* p_indexc,			//the actual cell in which a particle lies (stored for sorting)
			int i_length,					//total number of particles
			__read_only image2d_t T_INDEX,	//map: for each cell-> within the sorted list, what is the first index at which a particle occurs that has this specific cell as location
			__read_only image2d_t SH,		//map: output solid height
			__read_only image2d_t SU,		//map: output solid velocity in x-direction
                        __read_only image2d_t SV,		//map: output solid velocity in y-direction
                        __read_only image2d_t SUN,		//map: output solid velocity in x-direction
                        __read_only image2d_t SVN,		//map: output solid velocity in y-direction
                        __read_only image2d_t HF,
                        __read_only image2d_t UF,
                        __read_only image2d_t VF,
                        __global float* p_fluidprops,
                        float particle_factor
                        )
{

#ifdef LISEM_KERNEL_CPUCODE

#pragma omp parallel for
for(int id_1d_cpu = 0; id_1d_cpu < id_1d_cpum; id_1d_cpu++)
{
#endif

	
	//we require sampling of maps using real-world coordinates and non-interpolated values (keep integers as-is). If interpolation is needed, we do it ourselfs
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

        //this is a factor determining wether we copy the velocities, or the apply identical accalerations
        //these approaches are a numerical implemenation difference on which the literature provides varying solutions
        //typically, alpha is some low value (< 0.5 and > 0.01)
        float alpha = 0.05;

	//id is the thread id
        int id = get_global_id(0);
	//we stored a reference to the sorted index
	//this is not required, since all we do here is change the order in which we process particles
	//all the other code would still work with index = id, however, it might improve performence due to memory read/write pattern optimization within the gpu
	int index = p_index[id];

	//the location is a two-component vector so we read its component as such
	float x = p_xy[2*index+0];
	float y = p_xy[2*index+1];
	
	float usn = p_uv[2*index+0];
	float vsn = p_uv[2*index+1];
	
	//calculate the grid cell location of this particle
	const int gy = min(dim1-(int)(1),max((int)(0),(int)(floor(y/dx))));
	const int gx = min(dim0-(int)(1),max((int)(0),(int)(floor(x/dx))));

	//calculate the grid cell locations of neighbors
	const int gx_x1 = min(dim0-(int)(1),max((int)(0),(int)(gx - 1 )));
	const int gy_x1 = gy;
	const int gx_x2 = min(dim0-(int)(1),max((int)(0),(int)(gx + 1)));
	const int gy_x2 = gy;
	const int gx_y1 = gx;
	const int gy_y1 = min(dim1-(int)(1),max((int)(0),(int)(gy - 1)));
	const int gx_y2 = gx;
	const int gy_y2 = min(dim1-(int)(1),max((int)(0),(int)(gy + 1)));
	
	
	//we have stored the index of the first particle that was sorted as part of each particular cells
	//lets get these indices for each of the 9 surrounding cells (including center)	
	//we assume here that the maximum radius of influence is less than a single cell width/height
	
        int indexstart = read_imagef(T_INDEX,sampler, int2(gx,gy)).x;
        int indexstart_x1 = read_imagef(T_INDEX,sampler, int2(gx_x1,gy)).x;
        int indexstart_y1 = read_imagef(T_INDEX,sampler, int2(gx,gy_y1)).x;
        int indexstart_x2 = read_imagef(T_INDEX,sampler, int2(gx_x2,gy)).x;
        int indexstart_y2 = read_imagef(T_INDEX,sampler, int2(gx,gy_y2)).x;
        int indexstart_x1y1 = read_imagef(T_INDEX,sampler, int2(gx_x1,gy_y1)).x;
        int indexstart_x1y2 = read_imagef(T_INDEX,sampler, int2(gx_x1,gy_y2)).x;
        int indexstart_x2y1 = read_imagef(T_INDEX,sampler, int2(gx_x2,gy_y1)).x;
        int indexstart_x2y2 = read_imagef(T_INDEX,sampler, int2(gx_x2,gy_y2)).x;

	//for each of these cells, we can start at that index, and keep going untill the particle we check is no longer within that cell
	//we should also check the first one, since new starting indices are only written when a particle is present
	//there are 9 total cells we need to check, so put these in an array to simplify
	
	int indices[9] = {indexstart,indexstart_x1,indexstart_y1,indexstart_x2,indexstart_y2,indexstart_x1y1,indexstart_x1y2,indexstart_x2y1,indexstart_x2y2};
	int indices_gx[9] = {0,-1,0,1,0,-1,-1,1,1};
	int indices_gy[9] = {0,0,-1,0,1,-1,1,-1,1};
	
        float x_c = (gx+0.5) * dx;
        float y_c = (gy+0.5) * dx;

        float uvelweightedavg = 0.0;
        float vvelweightedavg = 0.0;
        float uaccweightedavg = 0.0;
        float vaccweightedavg = 0.0;
        float hfweightedavg = 0.0;
        float ufweightedavg = 0.0;
        float vfweightedavg = 0.0;
        float weight_total = 0.0;

	//iterate over each neighbor
	float n_part = 0;
	for(int i = 0; i < 9; i++)
	{
		float n_cell = 0;
		bool in_cell = true;
		int id_iter = indices[i];
				
		int gx_should = min(dim0-(int)(1),max((int)(0),(int)(gx + indices_gx[i])));
		int gy_should = min(dim1-(int)(1),max((int)(0),(int)(gy + indices_gy[i])));
	
		//thus, we have a neighboring particle
		//now we can continue to do actual calculations
		
		float disx = x_c - x;
		float disy = y_c - y;
		
		float dis = sqrt(disx*disx + disy*disy);
		
		float weight = weightfunction(dis,0.5*dx);

                int us_c = read_imagef(SU,sampler, int2(gx_should,gy_should)).x;
                int vs_c = read_imagef(SV,sampler, int2(gx_should,gy_should)).x;
                int usn_c = read_imagef(SUN,sampler, int2(gx_should,gy_should)).x;
                int vsn_c = read_imagef(SVN,sampler, int2(gx_should,gy_should)).x;

                int hfn_c = read_imagef(HF,sampler, int2(gx_should,gy_should)).x;
                int vfn_c = read_imagef(VF,sampler, int2(gx_should,gy_should)).x;
                int ufn_c = read_imagef(UF,sampler, int2(gx_should,gy_should)).x;


            uvelweightedavg = uvelweightedavg + weight * usn_c;
            vvelweightedavg = vvelweightedavg + weight * vsn_c;
            uaccweightedavg = uaccweightedavg + weight * (usn_c - us_c);
            vaccweightedavg = vaccweightedavg + weight * (vsn_c- vs_c);
            hfweightedavg = hfweightedavg + weight * (hfn_c)/4.0f;
            ufweightedavg = ufweightedavg + weight * (ufn_c)/4.0f;
            vfweightedavg = vfweightedavg + weight * (vfn_c)/4.0f;

            weight_total = weight_total + weight;
	}
	
        weight_total = max(weight_total,0.00001f);
        uvelweightedavg = uvelweightedavg/weight_total;
        vvelweightedavg = vvelweightedavg/weight_total;
        uaccweightedavg = uaccweightedavg/weight_total;
        vaccweightedavg = vaccweightedavg/weight_total;
        hfweightedavg = hfweightedavg/weight_total;
        ufweightedavg = ufweightedavg/weight_total;
        vfweightedavg = vfweightedavg/weight_total;

        //p_uv[2*index+0] = (p_uv[2*index+0]+uaccweightedavg);//alpha * uvelweightedavg+ (1.0 - alpha)*
        //p_uv[2*index+1] = (p_uv[2*index+1]+vaccweightedavg);//alpha * vvelweightedavg+ (1.0 - alpha)*
	
        p_fluidprops[5*index + 2] = hfweightedavg;
        p_fluidprops[5*index + 3] = ufweightedavg;
        p_fluidprops[5*index + 4] = vfweightedavg;

#ifdef LISEM_KERNEL_CPUCODE
}
#endif

}


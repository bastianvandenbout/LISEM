
#ifndef LISEM_KERNEL_CPUCODE
#define GRAV 9.81

__constant int ch_dx[10] = {0, -1, 0, 1, -1, 0, 1, -1, 0, 1};
__constant int ch_dy[10] = {0, 1, 1, 1, 0, 0, 0, -1, -1, -1};

#define nonkernel

#define int2(x,y) ((int2)((x),(y)))
#define float3(x,y,z) ((float3)((x),(y),(z)))
#else
#define __global
#define nonkernel inline static
#endif


//maximum number of particles we interact with within a single cell
//this effectively limits the maximum height of the flow being simulated
//hower, we can get away with a bit more, since the sorting algorithm scrambles the cell indices,
//each timestep, the cells taken out of the total to calculate the interactions are therefore varied
//averaged through time, we get a similar average force applied
//of course, if possible, keep this value high enough to take into account all neighbor particles
#define PARTICLE_CELL_MAX 250


////Here follows a description of the meaning of all the floating point arrays that are input and outut for the kernels in particles.cl and particles_mpm.cl
////Input and or output can be written within the same kernel (although this is multithreaded code, so no unsafe writing to other locations)
////
////
//__global float* p_xy,			//location of particles (size 2*n_particles)
//[2 * index + 0] ->    x component
//[2 * index + 1] ->    y component
//__global float* p_uv,			//velocity of particles (size 2*n_particles)
//[2 * index + 0] ->    x component
//[2 * index + 1] ->    y component
//__global float* p_props,		//deviatoric stress tensor of particles (size 4*n_particles, 4 = number of dimensions squared)
//[2 * index + 0] ->
//[2 * index + 1] ->
//[2 * index + 0] ->
//[2 * index + 1] ->
//__global float* p_sigma,		//stress tensor of particles (size 4*n_particles, 4 = number of dimensions squared)
//[2 * index + 0] ->    xx component (matrix index 11)
//[2 * index + 1] ->    xy component (matrix index 12)
//[2 * index + 2] ->    yx component (matrix index 21)
//[2 * index + 3] ->    yy component (matrix index 22)
//__global int* p_index,		//index to particles sorted by cell in which they lie
//[2 * index + 0] ->
//__global int* p_indexc,		//the actual cell in which a particle lies (stored for sorting)
//[2 * index + 0] ->
//__global float* p_ui,			//this is a two-float vector that is given to the opengl vertex shader for drawing the particle properties, content can depend on ui choice
//[2 * index + 0] ->
//[2 * index + 1] ->
//__global float* pn_xy,               	//location of particles (size 2*n_particles)
//[2 * index + 0] ->    x component
//[2 * index + 1] ->    y component
//__global float* pn_uv,                //velocity of particles (size 2*n_particles)
//[2 * index + 0] ->    x component
//[2 * index + 1] ->    y component
//__global float* pn_physprops,		//deviatoric stress tensor of particles (size 4*n_particles)
//[4 * index + 0] ->    internal friction angle
//[4 * index + 1] ->    effective rock size
//[4 * index + 2] ->    density
//[4 * index + 3] ->    cohesion
//__global float* pn_sigma,		//stress tensor of particles (size 4*n_particles, 4 = number of dimensions squared)
//[2 * index + 0] ->    xx component (matrix index 11)
//[2 * index + 1] ->    xy component (matrix index 12)
//[2 * index + 2] ->    yx component (matrix index 21)
//[2 * index + 3] ->    yy component (matrix index 22)
//__global float* pn_epsilon,		//stress tensor of particles (size 4*n_particles, 4 = number of dimensions squared)
//[2 * index + 0] ->    xx component (matrix index 11)
//[2 * index + 1] ->    xy component (matrix index 12)
//[2 * index + 2] ->    yx component (matrix index 21)
//[2 * index + 3] ->    yy component (matrix index 22)
//__global float* p_fluidprops          //fluid properties of particles (size 5*n_particles)
//[5 * index + 0] ->    fluid loss from particles
//[5 * index + 1] ->    fluid as part of particles
//[5 * index + 2] ->    average height
//[5 * index + 3] ->    average u velocity
//[5 * index + 4] ->    average v velocity


//all the 2x2 dimensional tensors in cartesion coordinates (so no conversion required between lower and upper indices, or co- and contravariant indices of the tensors)
//
//	( [0] [1] )   ( M11 M12 )
// M = 	( [2] [3] ) = ( M21 M22 )
//



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

nonkernel float signf(float x)
{
        return x > 0.0f? 1.0f:-1.0f;
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



//main kernel for particle dynamics (here kernel indicates a gpu-compiled code functions)s
kernel
void _particledynamics(
#ifdef LISEM_KERNEL_CPUCODE
int id_1d_cpum,
#endif


                        int dim0,                                //horizontal nr of cells
                        int dim1,                               //vertical nr of cells
                        float dx, 				//cell size (meters)
                        float dt,				//time step (seconds)
                        __global float* p_xy,			//location of particles (size 2*n_particles)
                        __global float* p_uv,			//velocity of particles (size 2*n_particles)
                        __global float* p_props,		//deviatoric stress tensor of particles (size 4*n_particles, 4 = number of dimensions squared)
                        __global float* p_sigma,		//stress tensor of particles (size 4*n_particles, 4 = number of dimensions squared)
                        __global int* p_index,			//index to particles sorted by cell in which they lie
                        __global int* p_indexc,			//the actual cell in which a particle lies (stored for sorting)
                        __global float* p_ui,			//this is a two-float vector that is given to the opengl vertex shader for drawing the particle properties, content can depend on ui choice
                        int i_length,				//total number of particles
                        __read_only image2d_t DEM,		//map: Digital elevation model
                        __read_only image2d_t T_INDEX,          //map: for each cell-> within the sorted list, what is the first index at which a particle occurs that has this specific cell as location
                        __global float* pn_xy,			//location of particles (size 2*n_particles)
                        __global float* pn_uv,			//velocity of particles (size 2*n_particles)
                        __global float* pn_physprops,		//deviatoric stress tensor of particles (size 4*n_particles)
                        __global float* pn_sigma,		//stress tensor of particles (size 4*n_particles, 4 = number of dimensions squared)
                        __global float* pn_epsilon,		//stress tensor of particles (size 4*n_particles, 4 = number of dimensions squared)
                        __global float* p_fluidprops,		//fluid properties of particles (size 5*n_particles)
                        __read_only image2d_t MANNING,
                        float dragmult,
                        float particle_factor,
                        float in_par_shear,
                        float in_par_elastic)
{
#ifdef LISEM_KERNEL_CPUCODE

#pragma omp parallel for
for(int id_1d_cpu = 0; id_1d_cpu < id_1d_cpum; id_1d_cpu++)
{
#endif



float fac = 1.0f;
        dt =  fac*dt;

        float kernel_dist = 0.5 * dx;

        //we require sampling of maps using real-world coordinates and non-interpolated values (keep integers as-is). If interpolation is needed, we do it ourselfs
        const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

        //id is the thread id
        int id = get_global_id(0);
        //we stored a reference to the sorted index
        //this is not required, since all we do here is change the order in which we process particles
        //all the other code would still work with index = id, however, it might improve performence due to memory read/write pattern optimization within the gpu
        int index = p_index[id];
        int indexcell = p_indexc[index];

        float reportf = 0.0;

        //the location is a two-component vector so we read its component as such
        float x = p_xy[2*index+0];
        float y = p_xy[2*index+1];

        float ppu = 5.0;
        float vol_ref = dx*dx*(dx/ppu);
        float vol_unit = dx*dx*dx;

        //same for velocity
        float u = p_uv[2*index+0];
        float v = p_uv[2*index+1];

        float havg = p_props[4*index+0];
        float uavg = p_props[4*index+1];
        float vavg = p_props[4*index+2];

        float fluidvol = p_fluidprops[5*index+0];

        float fluidhavg = p_fluidprops[5*index + 2];
        float fluiduavg = p_fluidprops[5*index + 3];
        float fluidvavg = p_fluidprops[5*index + 4];

        float fluidhconf = havg * fluidvol/vol_ref;

        float havg_c = havg + fluidhconf;

        float havg_all = havg + fluidhconf + fluidhavg;

        float sf = max(0.001f,havg/max(0.0001f,havg_all));
        float ff = max(0.001f,(fluidhavg+fluidhconf)/max(0.0001f,havg_all));

        float sf_drag = max(0.001f,(havg + fluidhconf)/max(0.0001f,havg_all));
        float ff_drag = max(0.001f,(fluidhavg)/max(0.0001f,havg_all));

        float cf = max(0.001f,(havg + fluidhconf)/max(0.0001f,havg_all));
        float ucf = max(0.001f,fluidhavg/max(0.0001f,havg_all));
        float f_sc = p_props[4*index+3];
        float f_fc = fluidhconf/max(0.00001f,fluidhavg+fluidhconf);
        float input_ifa = pn_physprops[4*index+0];
        float input_rs = pn_physprops[4*index+1];
        float input_d = pn_physprops[4*index+2];
        float input_coh = pn_physprops[4*index+3];

        //define properties and new parameters
        float c = p_props[4*index+3] * max(1.0f,input_coh * 1000.0f );
        float cohfrac = p_props[4*index+3];
        float ifa = input_ifa;

        float ifasin = sin(ifa);

        float density = input_d;
        float densitysq = input_d*input_d;
        float density_newt = density*9.81;
        float mass = vol_ref * density;
        float gamma = 1000.0f/density;
        float gamma_c = 1000.0f/density;

        float ShearModulus = in_par_shear;//1000000000000.0f;
        float ElasticBulkModulus = in_par_elastic;//10000000000000.0f;
        float tanifa = tan(ifa);
        float alphaifa = tanifa/(sqrt( 9.0f + 12.0f *tanifa*tanifa));
        float kc = 3.0f *c/(sqrt(9.0 + 12.0f *tanifa*tanifa));

        float alphaifab = tanifa/(sqrt( 9.0f + 12.0f *tanifa*tanifa));
        float kcb = 10.0f * kc;

        float dilatencyangle = 0.0f;
        float sindilatency = sin(dilatencyangle);
        float epsilon0 = 0.5f;
        float artificial_power = 3.0f;

        float un = u;
        float vn = v;

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

        //get basic rasterized input data
        float z = read_imagef(DEM,sampler, int2(gx,gy)).x;
        float z_x1 = read_imagef(DEM,sampler, int2(gx_x1,gy)).x;
        float z_x2 = read_imagef(DEM,sampler, int2(gx_x2,gy)).x;
        float z_y1 = read_imagef(DEM,sampler, int2(gx,gy_y1)).x;
        float z_y2 = read_imagef(DEM,sampler, int2(gx,gy_y2)).x;

        float man_n = read_imagef(MANNING,sampler, int2(gx,gy)).x;

        //all the 2x2 dimensional tensors in cartesion coordinates (so no conversion required between lower and upper indices, or co- and contravariant indices of the tensors)
        //
        //	( [0] [1] )   ( M11 M12 )
        // M = 	( [2] [3] ) = ( M21 M22 )
        //

        float stressdev[4] = {0.0f,0.0f,0.0f,0.0f};
        float dstressdevdt[4] = {0.0f,0.0f,0.0f,0.0f};
        float stress[4] = {p_sigma[4*index+0],p_sigma[4*index+1],p_sigma[4*index+2],p_sigma[4*index+3]};
        float dstressdt[4] = {0.0f,0.0f,0.0f,0.0f};
        float epsilon[4] = {0.0f,0.0f,0.0f,0.0f};
        float epsilondev[4] = {0.0f,0.0f,0.0f,0.0f};
        float omega[4] = {0.0f,0.0f,0.0f,0.0f};

        //generate accalerations
        float duvdt[2] = {0.0,0.0};





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


        float dhdx = 0.0f;
        float dhdy = 0.0f;
        float acc_px = 0.0f;
        float acc_py = 0.0f;
        float acc_pfx = 0.0f;
        float acc_pfy = 0.0f;
        float acc_stressx = 0.0f;
        float acc_stressy = 0.0f;
        float acc_pcorrx = 0.0f;
        float acc_pcorry = 0.0f;
        float acc_nviscx = 0.0f;
        float acc_nviscy = 0.0f;
        float weight_total = 0.0f;
        float weight_dtotalx = 0.0f;
        float weight_dtotaly = 0.0f;
        float weight_dtotalnx = 0.0f;
        float weight_dtotalny = 0.0f;
        float weight_dtotalxy[2] = {0.0f,0.0f};
        float sums[8] = {0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};

        //iterate over each neighbor
        float n_part = 0;
        float n_cell_total= 0;
        for(int i = 0; i < 9; i++)
        {

                float n_cell = 0;
                bool in_cell = true;
                int id_iter = indices[i];

                //if the previous particle was in the right cell, we did not go over maximum particles, and are within the length of the array
                while(in_cell && n_cell < PARTICLE_CELL_MAX && id_iter < i_length)
                {
                        //now check if the particle we get at the index is in the right cell
                        int index_n = p_index[id_iter];
                        float x_n = p_xy[2*index_n+0];
                        float y_n = p_xy[2*index_n+1];



                        int gx_should = gx + indices_gx[i];
                        int gy_should = gy + indices_gy[i];

                        int gy_n = min(dim1-(int)(1),max((int)(0),(int)(floor(y_n/dx))));
                        int gx_n = min(dim0-(int)(1),max((int)(0),(int)(floor(x_n/dx))));

                        //is the cell of this particle the right one?
                        if(gx_n == gx_should && gy_n == gy_should)
                        {
                                //found one!

                                n_part = n_part + 1;

                                //thus, we have a neighboring particle
                                //now we can continue to do actual calculations

                                float u_n = p_uv[2*index_n+0];
                                float v_n = p_uv[2*index_n+1];

                                float havg_n = p_props[4*index_n+0];
                                float uavg_n = p_props[4*index_n+1];
                                float vavg_n = p_props[4*index_n+2];

                                float cohfrac_n = p_props[4*index+3];

                                float stress_n[4] = {p_sigma[4*index_n+0],p_sigma[4*index_n+1],p_sigma[4*index_n+2],p_sigma[4*index_n+3]};

                                float distancex = (x_n - x);
                                float distancey = (y_n - y);
                                float distancesq = distancex*distancex + distancey * distancey;
                                float distance  = max(0.01f,sqrt(distancesq));

                                float weight =	weightfunction(distance,kernel_dist);

                                float mdivp = 1.0f;
                                float dwdr = weightfunctionderivative(distance,kernel_dist);
                                float dwdx = (distancex/distance)*dwdr;
                                float dwdy = (distancey/distance)*dwdr;
                                float dwndx = (distancex/distance)*weight;
                                float dwndy = (distancey/distance)*weight;

                                float area = 0.5f * mdivp;
                                weight_total = weight_total + weight;
                                weight_dtotalx =  weight_dtotalx + dwdx*(dwdx > 0.0?1.0:-1.0);
                                weight_dtotaly =  weight_dtotaly + dwdy*(dwdy > 0.0?1.0:-1.0);
                                weight_dtotalnx =  weight_dtotalnx + dwndx*(dwndx > 0.0?1.0:-1.0);
                                weight_dtotalny =  weight_dtotalny + dwndy*(dwndy > 0.0?1.0:-1.0);

                                //this is a numerical correcting pressure, not actual pressure
                                float distance_rel = distance/dx;
                                acc_px = acc_px - (1.0f/100.0f)* (1.0f/dx) * (x_n - x) *(1.0f/(distance_rel*distance_rel*distance_rel));
                                acc_py = acc_py - (1.0f/100.0f)* (1.0f/dx) * (y_n - y) *(1.0f/(distance_rel*distance_rel*distance_rel));

                                acc_pfx = acc_pfx - (10.0f/(7.0f*3.14159f*0.25*dx*dx))* (1.0f-cohfrac_n)*(9.81f * 10.0f/(dx))*area*(x_n - x) *(1.0f/(distance));
                                acc_pfy = acc_pfy - (10.0f/(7.0f*3.14159f*0.25*dx*dx))* (1.0f-cohfrac_n)*(9.81f * 10.0f/(dx))*area*(y_n - y) *(1.0f/(distance));

                                dhdx = dhdx + area * ((havg_n + havg) * dwndx); //index 11
                                dhdy = dhdy + area * ((havg_n + havg) * dwndy); //index 12

                                sums[0] = sums[0] + area * ((u_n - u) * dwdx); //index 11
                                sums[1] = sums[1] + area * ((u_n - u) * dwdy); //index 12
                                sums[2] = sums[2] + area * ((v_n - v) * dwdx); //index 21
                                sums[3] = sums[3] + area * ((v_n - v) * dwdy); //index 22

                                sums[4] = sums[4] + area * ((u_n - u) * dwdx); //index 11
                                sums[5] = sums[5] + area * ((v_n - v) * dwdx); //index 12
                                sums[6] = sums[6] + area * ((u_n - u) * dwdy); //index 21
                                sums[7] = sums[7] + area * ((v_n - v) * dwdy); //index 22

                                float dwdxy[2] = {dwdx,dwdy};

                                //stress tensor diagonalization according to kronenburg (2015)

                                float ldis = max(0.001f,(stress[0]-stress[3])*(stress[0]-stress[3]) +4.0f * stress[1]*stress[1]);
                                float ldet = sqrt(ldis);
                                float l1 = 0.5f *( stress[0] + stress[3] - ldet );
                                float l2 = 0.5f *( stress[0] + stress[3] + ldet );

                                //stress tensor diagonalization according to kronenburg (2015)

                                float ldis_n = max(0.001f,(stress_n[0]-stress_n[3])*(stress_n[0]-stress_n[3]) +4.0f * stress_n[1]*stress_n[1]);
                                float ldet_n = sqrt(ldis_n);
                                float l1_n = 0.5f *( stress_n[0] + stress_n[3] - ldet_n );
                                float l2_n = 0.5f *( stress_n[0] + stress_n[3] + ldet_n );

                                float weight0 =	max(0.01f,weightfunction(kernel_dist,kernel_dist));

                                float density_den =9.81f*density;//max(0.1f*dx,havg)*
                                float densitysq_den = density_den*density_den;

                                float density_den_n = 9.81f*density;//max(0.1f*dx,havg)*
                                float densitysq_den_n = density_den_n*density_den_n;

                                float Fn = pow((weight/weight0),artificial_power);
                                float Rab0 = 0;
                                Rab0 = l1 > 0? max(0.0f,Rab0 - epsilon0*l1/ densitysq_den):max(0.0f,Rab0 + epsilon0*l1/ densitysq_den);
                                Rab0 = l1_n > 0? max(0.0f,Rab0 - epsilon0*l1_n/ densitysq_den_n):max(0.0f,Rab0 + epsilon0*l1_n/ densitysq_den_n);
                                float Rab1 = 0;
                                Rab1 = l2 > 0? max(0.0f,Rab1 - epsilon0*l2/ densitysq_den):max(0.0f,Rab1 + epsilon0*l2/ densitysq_den);
                                Rab1 = l2_n > 0? max(0.0f,Rab1 - epsilon0*l2_n/ densitysq_den_n):max(0.0f,Rab1+epsilon0*l2_n/ densitysq_den_n);

                                acc_pcorrx = acc_pcorrx - Fn * dwdxy[0]*Rab0;
                                acc_pcorry = acc_pcorry - Fn * dwdxy[1]*Rab1;

                                float alpha_PI = 1.0f;
                                float beta_PI = 1.0f;
                                float c_vel = 600.0f;//0.5f *( sqrt(10000000000.0/(max(0.1f,havg*density))) + sqrt(10000000000.0/(max(0.1f,havg_n*density))));// * max(0.0f,min(1.0f,min(p_props[4*index+3],c/100.0f)));
                                //float udif = ;// > 0? 1.0:-1.0)*min(10000000.0f,(u - u_n > 0? 1.0f:-1.0f)*(u - u_n));
                                //float vdif = ;// > 0? 1.0:-1.0)*min(10000000.0f,(v - v_n > 0? 1.0f:-1.0f)*(v - v_n));
                                float uvxydot = ((u - u_n)*(x - x_n) +  (v - v_n)*(y - y_n) );
                                float phi_ij = (0.5f * dx *  uvxydot)/max(0.01f,(distancesq  + 0.01f * 0.5f *dx));
                                float PIx_ij = (uvxydot < 0.0f? (-alpha_PI * c_vel * phi_ij + beta_PI *phi_ij*phi_ij)/(density): 0.0f );//(0.5*(max(0.01f,havg)*density+max(0.01f,havg_n)*density))
                                acc_nviscx = acc_nviscx +PIx_ij * dwndx;
                                acc_nviscy = acc_nviscy +PIx_ij * dwndy;

                                //stress tensor based accallerations
                                float Rab[4] = {-0.5f*Fn * Rab0,0.0,0.0,-0.5f*Fn * Rab1};
                                for(int a = 0; a < 2; a++)
                                {
                                        for(int b = 0; b < 2; b++)
                                        {
                                                int i_ab = a*2 + b;

                                                //duvdt[a] = duvdt[a] -((stress[i_ab]/(max(0.01f,havg)*densitysq) + stress_n[i_ab]/(max(0.01f,havg)*densitysq))  - Rab[i_ab] )* (dwdxy[b]);
                                                duvdt[a] = duvdt[a] - (density)*((stress[i_ab]/( densitysq_den) + stress_n[i_ab]/( densitysq_den_n))- Rab[i_ab])* (dwdxy[b] );
                                                weight_dtotalxy[a] = weight_dtotalxy[a] + dwdxy[b]*(dwdxy[b] > 0.0f?1.0f:-1.0f);
                                        }
                                }

                                reportf+= havg_n;

                                n_cell = n_cell + 1;
                                id_iter = id_iter+1;
                        }else
                        {
                                //since the particles are sorted, end of particles for this cell!

                                in_cell = false;
                                break;
                        }
                }

                n_cell_total += n_cell;
        }

        weight_total = max(0.0001f,weight_total);
        weight_dtotalx = max(0.0001f,weight_dtotalx);
        weight_dtotaly = max(0.0001f,weight_dtotaly);
        weight_dtotalnx = max(0.0001f,weight_dtotalnx);
        weight_dtotalny = max(0.0001f,weight_dtotalny);

        weight_dtotalxy[0] = max(0.0001f,weight_dtotalxy[0]);
        weight_dtotalxy[1] = max(0.0001f,weight_dtotalxy[1]);

        acc_px = acc_px;
        acc_py = acc_py;

        acc_pfx = acc_pfx;
        acc_pfy = acc_pfy;

        acc_pcorrx = acc_pcorrx;
        acc_pcorry = acc_pcorry;

        acc_nviscx = acc_nviscx/weight_dtotalnx;
        acc_nviscy = acc_nviscy/weight_dtotalny;

        //duvdt[0] = duvdt[0]/weight_dtotalx;
        //duvdt[1] = duvdt[1]/weight_dtotaly;

        duvdt[0] = duvdt[0]/weight_dtotalxy[0];
        duvdt[1] = duvdt[1]/weight_dtotalxy[1];

        dhdx = dhdx/weight_dtotalnx; //index 11
        dhdy = dhdy/weight_dtotalny; //index 12

        dhdx = dhdx/(dx); //index 11
        dhdy = dhdy/(dx); //index 12

        sums[0] = sums[0]/weight_dtotalx; //index 11
        sums[1] = sums[1]/weight_dtotaly; //index 12
        sums[2] = sums[2]/weight_dtotalx;  //index 21
        sums[3] = sums[3]/weight_dtotaly;  //index 22

        sums[4] = sums[4]/weight_dtotalx; //index 11
        sums[5] = sums[5]/weight_dtotalx; //index 12
        sums[6] = sums[6]/weight_dtotaly;  //index 21
        sums[7] = sums[7]/weight_dtotaly;  //index 22

        float fac_dev = 1.0f;

        epsilon[0] = epsilon[0] + fac_dev *(sums[0] + sums[4]); //index 11
        epsilon[1] = epsilon[1] + fac_dev *(sums[1] + sums[5]); //index 12
        epsilon[2] = epsilon[2] + fac_dev *(sums[2] + sums[6]); //index 21
        epsilon[3] = epsilon[3] + fac_dev *(sums[3] + sums[7]); //index 22

        omega[0] = omega[0] + fac_dev *(sums[0] - sums[4]); //index 11
        omega[1] = omega[1] + fac_dev *(sums[1] - sums[5]); //index 12
        omega[2] = omega[2] + fac_dev *(sums[2] - sums[6]); //index 21
        omega[3] = omega[3] + fac_dev *(sums[3] - sums[7]); //index 22

        //compute the rate of change for the stress tensor
        //compute the rate of change for the deviatoric stress tensor

        for(int a = 0; a < 2; a++)
        {
                for(int b = 0; b < 2; b++)
                {
                        int i_ab = a*2 + b;
                        float dab = a==b?1.0f:0.0f;
                        epsilondev[i_ab] = epsilon[i_ab]-dab*0.5f * (epsilon[0] + epsilon[3]);
                }
        }

        //compute the rate of change for the stress tensor
        //compute the rate of change for the deviatoric stress tensor

        for(int a = 0; a < 2; a++)
        {
                for(int b = 0; b < 2; b++)
                {
                        int i_ab = a*2 + b;
                        float dab = a==b?1.0f:0.0f;
                        stressdev[i_ab] =stress[i_ab]-dab*0.5f * (stress[0] + stress[3]);
                }
        }

        float stress_I1 = stress[0] + stress[3];//0.5 * (max(0.0f,signf(stress[0])*stress[0]) + max(0.0f,signf(stress[3])*stress[3]));
        float stress_J2 = 0.5 * (stressdev[0]*stressdev[0] + stressdev[1]*stressdev[2] + stressdev[2]*stressdev[1] + stressdev[3]*stressdev[3]);


        float test = 0.0f;

        float smnemn = 0.0f;
        for(int a = 0; a < 2; a++)
        {
                for(int b = 0; b < 2; b++)
                {
                        int i_ab = a*2 + b;
                        smnemn = smnemn + stressdev[i_ab] * epsilondev[i_ab];

                }
        }


        for(int a = 0; a < 2; a++)
        {
                for(int b = 0; b < 2; b++)
                {
                        int i_ab = a*2 + b;
                        int i_ba = b*2 + a;
                        int i_ay1 = a*2 + 1;
                        int i_ay2 = a*2 + 2;
                        int i_yb1 = 1*2 + b;
                        int i_yb2 = 2*2 + b;
                        int i_by1 = b*2 + 1;
                        int i_by2 = b*2 + 2;
                        int i_yy1 = 0;
                        int i_yy2 = 3;
                        float dab = a==b?1.0f:0.0f;

                        dstressdt[i_ab] = dstressdt[i_ab] + stress[i_ay1] * omega[i_by1] + stress[i_ay2] * omega[i_by2];
                        dstressdt[i_ab] = dstressdt[i_ab] + stress[i_yb1] * omega[i_ay1] + stress[i_yb2] * omega[i_ay2];
                        dstressdt[i_ab] = dstressdt[i_ab] + (ShearModulus * epsilondev[i_ab] + ElasticBulkModulus *dab *(epsilon[i_yy1] + epsilon[i_yy2]));

                        float lambda = (3.0f * alphaifa*ElasticBulkModulus*(epsilon[i_yy1] + epsilon[i_yy2]) + (ShearModulus/sqrt(max(0.01f,stress_J2)))*stressdev[i_ab] * epsilon[i_ab])/(27.0f*alphaifa*ElasticBulkModulus *sindilatency + ShearModulus);

                        //dstressdt[i_ab] = dstressdt[i_ab] - lambda * (9.0f * ElasticBulkModulus*sindilatency*dab + (ShearModulus/sqrt(max(0.01f,stress_J2)))*stressdev[i_ab]);

                        test = test + dstressdt[i_ab];

                }
        }

        //calculate local raster-based slope
        float sz_x = 0.5f*(z_x2 - z_x1)/dx;
        float sz_y = 0.5f*(z_y2 - z_y1)/dx;




        float stressmax = 100000000000000000000.0f;
        //update deviatoric and normal stress tensors

        float decrease = 1.0f;
        float outside = z< -1000? 0.0f:1.0f;

        stress[0] = stress[0]+ outside*dt * dstressdt[0];
        stress[1] = stress[1]+ outside*dt * dstressdt[1];
        stress[2] = stress[2]+ outside*dt * dstressdt[2];
        stress[3] = stress[3]+ outside*dt * dstressdt[3];

        stress[0] = decrease*((float)((isnan(stress[0])? 0.0f:min((float)(stressmax),max((float)(-1.0f*stressmax),(float)(stress[0]))))));
        stress[1] = decrease*((float)((isnan(stress[1])? 0.0f:min((float)(stressmax),max((float)(-1.0f*stressmax),(float)(stress[1]))))));
        stress[2] = decrease*((float)((isnan(stress[2])? 0.0f:min((float)(stressmax),max((float)(-1.0f*stressmax),(float)(stress[2]))))));
        stress[3] = decrease*((float)((isnan(stress[3])? 0.0f:min((float)(stressmax),max((float)(-1.0f*stressmax),(float)(stress[3]))))));


        //recalculate deviatoric stress tensor for elastic-plastic material
        for(int a = 0; a < 2; a++)
        {
                for(int b = 0; b < 2; b++)
                {
                        int i_ab = a*2 + b;
                        float dab = a==b?1.0f:0.0f;
                        stressdev[i_ab] =stress[i_ab]-dab*0.5f * (stress[0] + stress[3]);
                }
        }


        //we must re-calculate the stress if the current state is outside of the yield surface
        //the criteria we use is the ducker-prager failure criteria fitted to a mohr-coulomb yielding surface

        stress_I1 = stress[0] + stress[3];//0.5 * (max(0.0f,signf(stress[0])*stress[0]) + max(0.0f,signf(stress[3])*stress[3]));
        stress_J2 = 0.5f * (stressdev[0]*stressdev[0] + stressdev[1]*stressdev[2] + stressdev[2]*stressdev[1] + stressdev[3]*stressdev[3]);

        //check for stress state left of yield surface apex, we first make sure it is at least located prependicular to this apex as seen relative to the I1 axis
        if(-alphaifa * stress_I1 + kc < 0)
        {
            stress[0] = stress[0] - (1.0f/2.0f)*(stress_I1-kc/alphaifa);
            stress[3] = stress[3] - (1.0f/2.0f)*(stress_I1-kc/alphaifa);
        }

        stress_I1 = stress[0] + stress[3];//0.5 * (min(0.0f,signf(stress[0])*stress[0]) + min(0.0f,signf(stress[3])*stress[3]));
        stress_J2 = 0.5f * (stressdev[0]*stressdev[0] + stressdev[1]*stressdev[2] + stressdev[2]*stressdev[1] + stressdev[3]*stressdev[3]);
        //recalculate deviatoric stress tensor for elastic-plastic material
        for(int a = 0; a < 2; a++)
        {
                for(int b = 0; b < 2; b++)
                {
                        int i_ab = a*2 + b;
                        float dab = a==b?1.0f:0.0f;
                        stressdev[i_ab] =stress[i_ab]-dab*0.5f * (stress[0] + stress[3]);
                }
        }

        stress_I1 = 0.5f * (min(0.0f,signf(stress[0])*stress[0]) + min(0.0f,signf(stress[3])*stress[3]));
        stress_J2 = 0.5f * (stressdev[0]*stressdev[0] + stressdev[1]*stressdev[2] + stressdev[2]*stressdev[1] + stressdev[3]*stressdev[3]);
        if(-alphaifa * stress_I1 + kc < sqrt(stress_J2))
        {

                //set new fraction of material that is effected by cohesion
                float abs_depsilon = fabs(epsilon[0]) + fabs(epsilon[1]) + fabs(epsilon[2]) + fabs(epsilon[3]);
                float abs_totalepsilon = (fabs(pn_epsilon[4*index+0]) + fabs(pn_epsilon[4*index+1]) + fabs(pn_epsilon[4*index+2])+fabs(pn_epsilon[4*index+3]));
                float fac_coh = 1.0;
                if(abs_totalepsilon > 0.1 * dx)
                {
                    fac_coh = exp(-(dt * abs_depsilon/(0.1*dx)));
                }
                p_props[4*index+3] = p_props[4*index+3] * fac_coh;

                p_fluidprops[5*index+1] = p_fluidprops[5*index+0] * (1.0-fac_coh);
                p_fluidprops[5*index+0] = p_fluidprops[5*index+0] * fac_coh;



            float scaling = max(-1.0f,min(1.0f,(-alphaifa * stress_I1 + kc)/sqrt(max(0.01f,stress_J2))));
            stress[0] = scaling * stressdev[0] + (1.0f/2.0f)*stress_I1;
            stress[3] = scaling * stressdev[3] + (1.0f/2.0f)*stress_I1;
            stress[1] = scaling * stressdev[1];
            stress[2] = scaling * stressdev[2];


        }else
        {
            p_fluidprops[5*index+1] = 0.0;
        }


        //recalculate deviatoric stress tensor for elastic-plastic material
        for(int a = 0; a < 2; a++)
        {
                for(int b = 0; b < 2; b++)
                {
                        int i_ab = a*2 + b;
                        float dab = a==b?1.0f:0.0f;
                        stressdev[i_ab] =stress[i_ab]-dab*0.5f * (stress[0] + stress[3]);
                }
        }
        stress_I1 = (stress[0] + stress[3]);//0.5 * (max(0.0f,signf(stress[0])*stress[0]) + max(0.0f,signf(stress[3])*stress[3]));
        stress_J2 = 0.5 * (stressdev[0]*stressdev[0] + stressdev[1]*stressdev[2] + stressdev[2]*stressdev[1] + stressdev[3]*stressdev[3]);

        //store the new stress tensorinto data buffer

        pn_sigma[4*index+0] = stress[0];
        pn_sigma[4*index+1] = stress[1];
        pn_sigma[4*index+2] = stress[2];
        pn_sigma[4*index+3] = stress[3];

        float epsilonold[4] = {pn_epsilon[4*index],pn_epsilon[4*index+1],pn_epsilon[4*index+2],pn_epsilon[4*index+3]};

        pn_epsilon[4*index+0] = epsilonold[0] + dt *epsilon[0];
        pn_epsilon[4*index+1] = epsilonold[1] + dt *epsilon[1];
        pn_epsilon[4*index+2] = epsilonold[2] + dt *epsilon[2];
        pn_epsilon[4*index+3] = epsilonold[3] + dt *epsilon[3];

        //now do the actual temporal integration, and some limiting for safety


        //un = un + outside*dt*acc_px;
        //vn = vn + outside*dt*acc_py;

        //un = un + dt *acc_pcorrx;
        //vn = vn + dt *acc_pcorry;

        float amax_stress = 10000000000.0f * dt;

        float acc_sx = min(amax_stress,max(-amax_stress,dt *1.0f*duvdt[0]));
        float acc_sy = min(amax_stress,max(-amax_stress,dt *1.0f*duvdt[1]));


        if(!((ShearModulus < 1e-6) && (ElasticBulkModulus < 1e-6)))
        {
            un = un - outside*acc_sx;
            vn = vn - outside*acc_sy;

            un = un - outside*acc_nviscx;
            vn = vn - outside*acc_nviscy;

        }

        //parameters for non-stress related forces
        float p_b_f = 1.0f;
        float p_b_ucs = 1.0f;
        float p_b_cs = 1.0f;

        float k_act = (1.0f- ifasin)/(1.0f+ifasin);
        float k_pass = (1.0f+ifasin)/(1.0f-ifasin);

        float k_x = k_act;
        float k_y = k_act;

        //Apply cohesion-adapted bells-rankine stress

        un = un- sf*(1.0f-gamma)*(k_act)*(1.0f/fac)*9.81f* outside*dt *(((dhdx)));
        vn = vn- sf*(1.0f-gamma)*(k_act)*(1.0f/fac)*9.81f* outside*dt *(((dhdy)));


        //apply buoyant pressure accaleration
        un = un- ff*(1.0f/fac)*9.81f* outside*dt *(((dhdx)));
        vn = vn- ff*(1.0f/fac)*9.81f* outside*dt *(((dhdy)));

        //un = un + outside*dt*acc_pfx;
        //vn = vn + outside*dt*acc_pfy;

        //apply graviational accaleration
        float sz_xc = sz_x /sqrt(max(0.00001f,sz_x * sz_x + sz_y * sz_y));
        float sz_yc = sz_y /sqrt(max(0.00001f,sz_x * sz_x + sz_y * sz_y));
        float sz_c = fabs(sin(atan(fabs(sz_x) + fabs(sz_y))));

        un = un-1.0f *(1.0f/fac)*9.81f* outside*dt *((sz_xc *sz_c));
        vn = vn-1.0f *(1.0f/fac)*9.81f* outside*dt *((sz_yc * sz_c));

        //apply drag force
        float UF_j = 2.0f;
        float vpow = pow((vn - fluidvavg) * (vn - fluidvavg) + (un - fluiduavg) * (un - fluiduavg),UF_j - 1.0f);
        float dc = dragmult * sf;
        float lfacv = max(0.0f,dt * (1.0f/fac)*dc);
        float v_balance = ff_drag * fluidvavg  + sf_drag * vn;
        vn = v_balance + (vn - v_balance) * exp(-lfacv);
        float u_balance = ff_drag * fluiduavg  + sf_drag * un;
        un = u_balance + (un - u_balance) * exp(-lfacv);

        //apply fluid-like friction

        //float vsq = sqrt((float)(un * un + vn * vn));
        //float nsq1 = (man_n)*(man_n)*9.81f/pow((float)(max(0.01f,havg)),(float)(4.0f/3.0f));
        //float nsq = 0.1 * nsq1*vsq*(1.0f/fac)*dt;
        //vn = (float)((vn/(1.0f+nsq)));
        //un = (float)((un/(1.0f+nsq)));

        //apply basal boundary condition according to mohr-coulomb material

        float u_xc = fabs(un/sqrt(max(0.00001f,un*un + vn*vn)));
        float v_yc = fabs(vn/sqrt(max(0.00001f,un*un + vn*vn)));

        vn = vn > 0? max(0.0f,(float)(vn - sf * dt *(1.0f/fac)* v_yc * 9.81f * tan(ifa))) : min(0.0f,(float)(vn + sf * dt *(1.0f/fac)*v_yc * 9.81f * tan(ifa)));
        un = un > 0? max(0.0f,(float)(un - sf * dt *(1.0f/fac) *u_xc *9.81f * tan(ifa))) : min(0.0f,(float)(un + sf * dt * (1.0f/fac)*u_xc * 9.81f * tan(ifa)));

        un = isnan(un)? 0.0:un;
        vn = isnan(vn)? 0.0:vn;

        //un = (un > 0? 1.0:-1.0)*min(100.0f,(un > 0? 1.0f:-1.0f)*(un));
        //vn = (vn > 0? 1.0:-1.0)*min(100.0f,(vn > 0? 1.0f:-1.0f)*(vn));

        float vmax = 200.0f;

        un = min(vmax,max(-vmax,un ));
        vn = min(vmax,max(-vmax,vn ));

        //calculate new locations
        float xn = x + outside*u*(1.0f/fac)* dt;
        float yn = y + outside*v*(1.0f/fac)* dt;

        //store new locations and velocities (note safely at the moment? need to allocate seperate list for this?)
        pn_xy[2*index+0] = xn;
        pn_xy[2*index+1] = yn;

        pn_uv[2*index+0] = un;
        pn_uv[2*index+1] = vn;

        const int gyn = min(dim1-(int)(1),max((int)(0),(int)(floor(yn/dx))));
        const int gxn = min(dim0-(int)(1),max((int)(0),(int)(floor(xn/dx))));
        p_indexc[index] = gyn * dim0 + gxn;

        reportf = cohfrac;
        //reportf +=pn_epsilon[4*index+0] > 0.0? pn_epsilon[4*index+0] : -pn_epsilon[4*index+0];
        //reportf += pn_epsilon[4*index+1] > 0.0? pn_epsilon[4*index+1] : -pn_epsilon[4*index+1];
        //reportf += pn_epsilon[4*index+2] > 0.0? pn_epsilon[4*index+2] : -pn_epsilon[4*index+2];
        //reportf += pn_epsilon[4*index+3] > 0.0? pn_epsilon[4*index+3] : -pn_epsilon[4*index+3];

        reportf = fabs(u_xc) + fabs(v_yc);//sf_drag;//(!isnormal(stress_J2))?0.0:log(stress_J2 * (stress_J2 < 0? -1.0f:1.0f))/60.0f;
        p_ui[2*index+0] = reportf;////p_props[4*index+3];//(fabs(pn_epsilon[4*index+0]) + fabs(pn_epsilon[4*index+1]) + fabs(pn_epsilon[4*index+2])+fabs(pn_epsilon[4*index+3]));//log(stress_J2 * (stress_J2 < 0? -1.0f:1.0f))/40;//p_props[4*index+3];// ;//
        p_ui[2*index+1] = isnan(test)? 10000.0f:0.0f;



#ifdef LISEM_KERNEL_CPUCODE
}
#endif
}



//main kernel for particle dynamics (here kernel indicates a gpu-compiled code functions)s
kernel
void _particledynamics2(
#ifdef LISEM_KERNEL_CPUCODE
int id_1d_cpum,
#endif

__global float* p_xy,                                   //location of particles (size 2*n_particles)
                                __global float* p_uv,			//velocity of particles (size 2*n_particles)
                                __global float* p_props,		//deviatoric stress tensor of particles (size 4*n_particles, 4 = number of dimensions squared)
                                __global float* p_sigma,		//stress tensor of particles (size 4*n_particles, 4 = number of dimensions squared)
                                __global int* p_index,			//index to particles sorted by cell in which they lie
                                __global float* pn_xy,			//location of particles (size 2*n_particles)
                                __global float* pn_uv,			//velocity of particles (size 2*n_particles)
                                __global float* pn_physprops,		//deviatoric stress tensor of particles (size 4*n_particles, 4 = number of dimensions squared)
                                __global float* pn_sigma,		//stress tensor of particles (size 4*n_particles, 4 = number of dimensions squared)
                                __read_only image2d_t T_INDEX,
                                float dx,
                                int dim0,                                //horizontal nr of cells
                                int dim1,                               //vertical nr of cells
                                int i_length,
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

        float kernel_dist = 0.5 * dx;

        //id is the thread id
        int id = get_global_id(0);
        //we stored a reference to the sorted index
        //this is not required, since all we do here is change the order in which we process particles
        //all the other code would still work with index = id, however, it might improve performence due to memory read/write pattern optimization within the gpu
        int index = p_index[id];

        float w0 = weightfunction(0,kernel_dist);

        //the location is a two-component vector so we read its component as such
        float x = p_xy[2*index+0];
        float y = p_xy[2*index+1];

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


        //store new locations and velocities (note safely at the moment? need to allocate seperate list for this?)
        p_xy[2*index+0] = pn_xy[2*index+0];
        p_xy[2*index+1] = pn_xy[2*index+1];

        p_uv[2*index+0] = pn_uv[2*index+0];
        p_uv[2*index+1] = pn_uv[2*index+1];

        p_sigma[4*index+0] = pn_sigma[4*index+0];
        p_sigma[4*index+1] = pn_sigma[4*index+1];
        p_sigma[4*index+2] = pn_sigma[4*index+2];
        p_sigma[4*index+3] = pn_sigma[4*index+3];

        float hsn = 0.25;
        float usn = w0* pn_uv[2*index+0];
        float vsn = w0* pn_uv[2*index+1];

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

        float x_c = pn_xy[2*index+0];
        float y_c = pn_xy[2*index+1];

        float wtotal = w0;

        float test = 9999999999;

        float n_cell_total= 0.0;
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

                                float weight =	max(0.0f,weightfunction(dis,kernel_dist));
                                float weightnn =	max(0.0f,weightfunctionnn(dis,kernel_dist));

                                wtotal = wtotal + weight;

                                hsn = hsn + weightnn * (1.0);
                                usn = usn + weight * (u_n);
                                vsn = vsn + weight * (v_n);

                                n_cell = n_cell + 1;
                                id_iter = id_iter+1;

                                test = min(test,weight);

                        }else
                        {
                                //since the particles are sorted, end of particles for this cell!
                                in_cell = false;

                        }
                }

                n_cell_total += n_cell;
        }

        p_props[4*index+0] = hsn * 3.584f *dx/particle_factor;
        p_props[4*index+1] = usn/max(0.01f,wtotal);
        p_props[4*index+2] = vsn/max(0.01f,wtotal);


#ifdef LISEM_KERNEL_CPUCODE
}
#endif


}




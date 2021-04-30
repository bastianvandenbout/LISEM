#define PARTICLE_CELL_MAX 10				//maximum number of particles we interact with within a single cell

kernel
void _particledynamics(int dim0, 						//horizontal nr of cells
			int dim1, 						//vertical nr of cells
			float dx, 						//cell size (meters)
			float dt,						//time step (seconds)
			global float* p_xy,				//location of particles (size 2*n_particles)
			global float* p_uv,				//velocity of particles (size 2*n_particles)
			global float* p_sigma,			//stress tensor of particles (size 4*n_particles, 4 = number of dimensions squared)
			global float* p_sigmad,			//deviatoric stress tensor of particles (size 4*n_particles, 4 = number of dimensions squared)
			global int* p_index,			//index to particles sorted by cell in which they lie
			global int* p_indexc,			//the actual cell in which a particle lies (stored for sorting)
			global float* p_ui,				//this is a two-float vector that is given to the opengl vertex shader for drawing the particle properties, content can depend on ui choice
			int i_length,					//total number of particles
			__read_only image2d_t DEM,		//map: Digital elevation model
			__read_only image2d_t T_INDEX	//map: for each cell-> within the sorted list, what is the first index at which a particle occurs that has this specific cell as location
			)
{
	//we require sampling of maps using real-world coordinates and non-interpolated values (keep integers as-is). If interpolation is needed, we do it ourselfs
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

	//id is the thread id
    int id = get_global_id(0);
	//we stored a reference to the sorted index
	//this is not required, since all we do here is change the order in which we process particles
	//all the other code would still work with index = id, however, it might improve performence due to memory read/write pattern optimization within the gpu
	int index = p_index[id];

	//the location is a two-component vector so we read its component as such
	float x = p_xy[2*index+0];
	float y = p_xy[2*index+1];
	
	//same for velocity
	float u = p_uv[2*index+0];
	float v = p_uv[2*index+1];
	
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
	float z = read_imagef(DEM,sampler, (int2)(gx,gy)).x;
	float z_x1 = read_imagef(DEM,sampler, (int2)(gx_x1,gy)).x;
	float z_x2 = read_imagef(DEM,sampler, (int2)(gx_x2,gy)).x;
	float z_y1 = read_imagef(DEM,sampler, (int2)(gx,gy_y1)).x;
	float z_y2 = read_imagef(DEM,sampler, (int2)(gx,gy_y2)).x;
	
	
	
	
	
	//we have stored the index of the first particle that was sorted as part of each particular cells
	//lets get these indices for each of the 9 surrounding cells (including center)	
	//we assume here that the maximum radius of influence is less than a single cell width/height
	
	int indexstart = read_imagef(T_INDEX,sampler, (int2)(gx,gy)).x;
	int indexstart_x1 = read_imagef(T_INDEX,sampler, (int2)(gx_x1,gy)).x;
	int indexstart_y1 = read_imagef(T_INDEX,sampler, (int2)(gx,gy_y1)).x;
	int indexstart_x2 = read_imagef(T_INDEX,sampler, (int2)(gx_x2,gy)).x;
	int indexstart_y2 = read_imagef(T_INDEX,sampler, (int2)(gx,gy_y2)).x;
	int indexstart_x1y1 = read_imagef(T_INDEX,sampler, (int2)(gx_x1,gy_y1)).x;
	int indexstart_x1y2 = read_imagef(T_INDEX,sampler, (int2)(gx_x1,gy_y2)).x;
	int indexstart_x2y1 = read_imagef(T_INDEX,sampler, (int2)(gx_x2,gy_y1)).x;
	int indexstart_x2y2 = read_imagef(T_INDEX,sampler, (int2)(gx_x2,gy_y2)).x;

	//for each of these cells, we can start at that index, and keep going untill the particle we check is no longer within that cell
	//we should also check the first one, since new starting indices are only written when a particle is present
	//there are 9 total cells we need to check, so put these in an array to simplify
	
	int indices[9] = {indexstart,indexstart_x1,indexstart_y1,indexstart_x2,indexstart_y2,indexstart_x1y1,indexstart_x1y2,indexstart_x2y1,indexstart_x2y2};
	int indices_gx[9] = {0,-1,0,1,0,-1,-1,1,1};
	int indices_gy[9] = {0,0,-1,0,1,-1,1,-1,1};
	
	//iterate over each neighbor
	float n_part = 0;
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
				
				float distancex = (x_n - x);
				float distancey = (y_n - y);
				float distance  = max(0.01f,sqrt(distancex*distancex + distancey * distancey));
				
				un = un - dt *(x_n - x) *(1.0f/(distance*distance*distance));
				vn = vn - dt *(y_n - y) *(1.0f/(distance*distance*distance)); 
				
				
				n_cell = n_cell + 1;
				id_iter = id_iter+1;
			}else
			{
				//since the particles are sorted, end of particles for this cell!
				
				in_cell = false;
				break;
			}
		}	
	}


	//calculate local raster-based slope
	float sz_x = 0.5*(z_x2 - z_x1)/dx;
	float sz_y = 0.5*(z_y2 - z_y1)/dx;

		
	//calculate new locations
	float xn = x + u * dt;
	float yn = y + v * dt;

	un = un - dt *sz_x;
	vn = vn - dt *sz_y;
	
	//store new locations and velocities (note safely at the moment? need to allocate seperate list for this?)
	p_xy[2*index+0] = xn;
	p_xy[2*index+1] = yn;
	
	p_uv[2*index+0] = un * 0.9;
	p_uv[2*index+1] = vn * 0.9;

	const int gyn = min(dim1-(int)(1),max((int)(0),(int)(floor(yn/dx))));
	const int gxn = min(dim0-(int)(1),max((int)(0),(int)(floor(xn/dx))));
	p_indexc[index] = gyn * dim0 + gxn;
	
	p_ui[2*index+0] = n_part;//indexstart;
	p_ui[2*index+1] = n_part;
	

	
	
}

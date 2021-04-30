
#define GRAV 9.81

float minmod(float a, float b)
{
	float rec = 0;
	if (a >= 0 && b >= 0)
	{
        rec = min(a, b);
	}else if(a <= 0 && b <= 0)
	{
        rec = max(a, b);
	}
	return rec;
}



kernel
void flow( int dim0, 
				int dim1, 
				float dx,
				float dt,
				__read_only image2d_t DEM,
				__read_only image2d_t H,
				__read_only image2d_t Vx,
				__read_only image2d_t Vy,
				__read_only image2d_t InfilPot,
				__read_only image2d_t InfilCum,
				__write_only image2d_t Qx,
				__write_only image2d_t Qy,
				__write_only image2d_t Hn,
				__write_only image2d_t Vxn,
				__write_only image2d_t Vyn,
				__write_only image2d_t InfilAct
				)
{

}

kernel
void flow2( int dim0, 
				int dim1, 
				float dx,
				float dt,
				__read_only image2d_t DEM,
				__write_only image2d_t H,
				__write_only image2d_t Vx,
				__write_only image2d_t Vy,
				__write_only image2d_t InfilCum,
				__read_only image2d_t Qx,
				__read_only image2d_t Qy,
				__read_only image2d_t Hn,
				__read_only image2d_t Vxn,
				__read_only image2d_t Vyn,
				__read_only image2d_t InfilAct
				)
{

}



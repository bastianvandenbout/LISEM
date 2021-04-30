//bitonic sort is an algorithm that is easy to run in parallel, since the read and write location on memory are pre-known and dont overlap at each step.
//the algorithm is nlog(n) efficient and requires log(n) calls from this kernel. 


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



kernel
void _kbitonic_stl_sort(
#ifdef LISEM_KERNEL_CPUCODE
int id_1d_cpum,
#endif

            __global int *input_ptr, 
			__global int *input_cptr,			
            const int in_stage,        
            const int passOfStage,
			const int n_padded,
			const int n)           
{

#ifdef LISEM_KERNEL_CPUCODE

#pragma omp parallel for
for(int id_1d_cpu = 0; id_1d_cpu < id_1d_cpum; id_1d_cpu++)
{
#endif


	 unsigned int  stage = (unsigned int)(in_stage);
     unsigned int  threadId = get_global_id(0);  					
     unsigned int  pairDistance = 1 << (stage - passOfStage);   	
     unsigned int  blockWidth = 2 * pairDistance;    				
     unsigned int  temp;  											
                                                         			
     int compareResult;                                      		
     unsigned int  leftId = (threadId & (pairDistance - 1)) + 		 
			(threadId >> (stage - passOfStage) ) * blockWidth;  		 
     unsigned int  rightId = leftId + pairDistance;  				 
       																 
     int leftElement[1];  							 
	 int rightElement[1];
	 
   	 int leftCElement[1];  							 
	 int rightCElement[1];
	 
	 int *greater, *lesser;  						 
													
		leftElement[0] = input_ptr[leftId*1 + 0];
		rightElement[0] = input_ptr[rightId*1 + 0];
		
		if(leftElement[0] > (n)-1)
		{
			leftCElement[0] =  1000000000;
		}else{
			leftCElement[0] = input_cptr[leftElement[0]];//leftElement[0]*2 + 1];
		}
		if(rightElement[0] > (n)-1)
		{
			rightCElement[0] = 1000000000;
		}else{
			rightCElement[0] = input_cptr[rightElement[0]];//rightElement[0]*2 + 1];
		}
												
     unsigned int sameDirectionBlockWidth = threadId >> stage;   	 
     unsigned int sameDirection = sameDirectionBlockWidth & 0x1; 	 
     																 
     temp = sameDirection ? rightId : temp; 						 
     rightId = sameDirection ? leftId : rightId; 					 
     leftId = sameDirection ? temp : leftId;						 
      																 
     compareResult = (leftCElement[0] < rightCElement[0]); 			 
      																 
     greater = compareResult ? rightElement : leftElement; 		 
     lesser = compareResult ? leftElement : rightElement; 		 
      																 
	input_ptr[leftId*1 + 0] = lesser[0];
	input_ptr[rightId*1 + 0] = greater[0];


#ifdef LISEM_KERNEL_CPUCODE
}
#endif

}     		

#include <stdio.h>
#include "map.h"
#include "parallel.h"

int main() {
  printf("Loaded wasm module for fastflood algorithm\n");
  return 0;
}


void do_algo_fastflood(float*data, int w, int h, float rain, float ksat, float n)
{
	cTMap * m = new cTMap();
	
	
	
	
	
}
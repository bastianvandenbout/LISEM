---
title: GPU
permalink: /docs/gpu/
---

The LISEM software can use a graphical processing unit (GPU).
Supported are the majorty of simple raster calculations, and the LISEM model.

The following code example benchmarks a simple GPU calculation.
Note that the data needs to be uploaded and downloaded to the gpu.
However, with larger maps, a 10x speed increase is common.

```
void main()
{	
	//first create a temporary empty gpu map
	//this will initialize all the gpu-side code, but this is done only once 
	GpuMap temp = GpuMap();
    
	//now we do the benchmark
    GPUMap m = ToGPU(dem_lidar_5ml.map);
    double t = GetAppTime();
    GPUMap m2 = pow(sqrt(sqrt(m)),2.0);
    Print("GPU TIME " + ToString(GetAppTime() - t));
    
    test.map = ToCPU(m2);
   
    Map mc = dem_lidar_5ml.map;
    t = GetAppTime();
    Map mc2 = sqrt(sqrt(mc));
    Print("CPU TIME " + ToString(GetAppTime() - t));

}
```

For the lisem model, the gpu can be toggled with the "use-gpu" option in the interface.
You can see which gpu is active by pressing the device setting in the top right.
Here, all found devices and their properties are listed.

The model uses openGL/openCL interoperability functionality, which requires the graphics and opencl to run on the same device.
Often, laptops start the application with the Intel Integrated graphics, which have lower performence than a possible dedicated gpu.
If such a gpu is present, you can inform your operating system to use it with LISEM by going to either NVIDIA control panel, and setting software-specific options, or going to AMD Catalyst, and similarly going to software-specific options.

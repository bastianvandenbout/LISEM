---
title: Flooding
permalink: /docs/flooding/
redirect_from: /docs/index.html
---
### Flooding within hydrology

If you have followed the descriptions for modelling hydrology or groundwater, 
flooding is automatically set up. That is, if there is sufficient precipitation to cause flooding, the model will simulate this.


### Simple water-flow simulation
To do a flood simulation without hydrology, little data and setup is required. 
#### Elevation and Mannings N
First, get an elevation model and cut it to the desired extent.
Second, define a Mannings N map. This paramter described surface roughness and determines friction for surface flows.
This map can be made homogeneous (one single value for the entire model domain).
If you wish to have a more detailed description, see the page about modelling hydrology.

#### Initial conditions
Besides terrain and surface roughness, you need to specify the initial water height, including river, lakes and other surface flows.
We will use the following initial fluid height (in meters).

#### Modelling
There are two options to run this simulation. You can use the script environment to call the FlowDynamic function in a loop.
This function provides highly accurate saint-venant surface flow with wave behavior.
Such a script is provided below.

```


void main()
{
    
    Map dem = dem.map; 
    Map whinitial = initialwaterheight.map;
    Map n= dem * 0.0 + 0.1; //homogeneous mannings n of 0.1
    
    Print("Create Initial Properties");
    
	//a list with {height, x-velocity, y-velocity}
    array<Map> flowprops = {whinitial,whinitial * 0.0,whinitial*0.0};
    
    Print("Add Layers");
    UILayer l = AddViewLayer( whinitial, "elevation",true);
    for(int i = 0; i < 5000; i++)
    {   
        flowprops = FlowDynamic(dem, n,flowprops[0],flowprops[1],flowprops[2],0.1);
        
        ReplaceViewLayer(l, flowprops[0]);
		
		//every 50 timesteps, store the flow height and velocity
        if( i % 50== 0)
        {
            whelev.tif = dem+flowprops[0];
            whvel.tif = sqrt(flowprops[2] *flowprops[2]  + flowprops[1] *flowprops[1]) ;
        
        }
        Print("Timestep : " + ToString(i));
        
    } 
}
```
The example data can be downloaded for: [Elevation](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/ExampleGeoData/Dijkring41/dem_cut.map/download), [Initial Water](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/ExampleGeoData/Dijkring41/initialwh.map/download) and [Mannings N](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/ExampleGeoData/Dijkring41/n.map/download).
The map view now shows the real-time results:

![Model](/assets/img/model_flood.png)

#### Ready, Set, Go!
Alternatively, you can use the LISEM Hazard model.
Set the processes to use initial water.

![Model](/assets/img/model_processes2.png)

For more details, see also the page on modelling hydrology.
First set the input and output directories.
Then, set the map names.
Finally, set the desired output options.
You can specify timeseries output for every x timesteps.

Press run and see the simulation progress!






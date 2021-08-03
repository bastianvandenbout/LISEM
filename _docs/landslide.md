---
title: Landslide and Debris Flow runout
permalink: /docs/landslide/
redirect_from: /docs/index.html
---

This is about landslide and debris flow runout

## Simple runout modelling with LISEM
A simple runout modelling can be performed without the need for modelling slope stability, hydrology or slope failure.
This works very similar to a simple flood setup. 
The required data is elevation, mannings N and the initial material for the flow simulation.
Because this consists of a mixture of water and solids, both initial water height, initial solid height, solid internal friction angle, solid density and solid rocksize are required.

### Initial Conditions
The initial condition maps can be downloaded below:

* [Elevation](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/ExampleGeoData/GrandBay/dem.map/download)
* [Mannings N](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/ExampleGeoData/GrandBay/n.map/download)
* [Initial Water Height](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/ExampleGeoData/GrandBay/initialfh.map/download)
* [Initial Solid Height](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/ExampleGeoData/GrandBay/initialsh.map/download)
* [Initial Solid Cohesion](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/ExampleGeoData/GrandBay/initialscohesion.map/download)
* [Initial Solid Internal Friction Angle](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/ExampleGeoData/GrandBay/initialsifa.map/download)
* [Initial Solid Density](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/ExampleGeoData/GrandBay/initialsd.map/download)
* [Initial Solid Rocksize](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/ExampleGeoData/GrandBay/initialsrocksize.map/download)

### Ready, Set, Go!
Set the initial water and initial solids in the processes.
![Model](/assets/img/model_processes4.png)

After providing the input maps, you can run the simulation for the mass movement runout!

![Model](/assets/img/model_runout.png)

## Modelling of runout of simulated slope failures

If you want to simulate runout after hydrology-induced slope failures, select the slope failure option in the processes.
This requires a full hydrology and slope stability simulation, but will automatically predict failure volumes and initiate runout.
Interactions with water flow (rivers or lakes) is also automatically taken into account.
Note that the presence of a channel influences runout, as material often deposits in the channel, resulting in additional flooding.
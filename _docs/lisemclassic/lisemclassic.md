---
title: Lisem Classic
permalink: /docs/lisemclassic/
redirect_from: /docs/index.html
---

The classic lisem software is a physically-based modelling tool for hydrology, erosion and flooding at various spatial scales (plots to catchments and countries).
The software features a rich history of application and continuous development since 1993.

The main interface of the software shows the model settings. 

![LisemClassic](/LISEM/assets/img/lisem_classic.png)

Here, the model controls, process settings and output settings are visible.
When opening the map database, an overview of required input maps is available.
Double-clicking a map name allows you to select a file, although it is usually easy to use the default filenames.
The lighter text indicates an input map is not required for the simulation, as the relevant process has not been activated.
Input maps are usually constructed using a geo-spatial scripting tool such as PCRaster+Nutshell or the new LISEM software.
All input maps must be of identical rows and columns, but might be of any format supported by GDAL (.map, GeoTiff, ascii, etc..).

![LisemClassic](/LISEM/assets/img/lisem_classic2.png)

When the simulation is running, real-time output can be visualized in the simulation tab.
Here, hydrological statistics of the simulation are provided in catchment-average mm equivalent.
Additionally, a spatial view of many model variables can be shown, such as infiltration, flow heights, flow velocities and soil loss.
![LisemClassic](/LISEM/assets/img/lisem_classic3.png)

When viewing soil loss, a spatially-distributed balance between erosion and deposition due to splash and flow erosive processes is shown.
![LisemClassic](/LISEM/assets/img/lisem_classic4.png)

For specified points in the simulation, such as channel outlets or bridges, a discharge curve (hydrograph) can be viewed during the simulation.
![LisemClassic](/LISEM/assets/img/lisem_classic5.png)


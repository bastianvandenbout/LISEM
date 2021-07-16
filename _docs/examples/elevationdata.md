---
title: Elevation data
permalink: /docs/elevationdata/
---

Lisem can do various types of analysis with elevation data.
![Elevation manipulation](/LISEM/assets/img/calc_1.png)
### Elevation derivatives

```
slope.tif = Slope(dem.tif)
```
![Elevation manipulation](/LISEM/assets/img/calc_2.png)
```
hillshade.tif = HillShade(dem.tif)
```
![Elevation manipulation](/LISEM/assets/img/calc_4.png)
```
aspect.tif = Aspect(dem.tif)
```
![Elevation manipulation](/LISEM/assets/img/calc_3.png)
### Drainage

```
ldd.tif = DrainageNetwork(dem.tif,1e4,1e4,1e4,1e4)
```
![Elevation manipulation](/LISEM/assets/img/calc_5.png)
```
accuflux.tif = Accuflux(ldd.tif,1.0)
```
![Elevation manipulation](/LISEM/assets/img/calc_6.png)
```
accuflux2d.tif = Accuflux2D(dem.map,dem.map * 0.0 + 1.0,dem.map * 0.0,0.3,1.0)
```
![Elevation manipulation](/LISEM/assets/img/calc_7.png)
### Channels
We can select an estimate of channel locations by using a threshold on the accuflux data.
```
channel.tif = MapIf(accuflux.tif >> 1000.0,1.0,0.0)
```
![Elevation manipulation](/LISEM/assets/img/calc_9.png)

We can use global relationships from Frasson et al., 2019 (global average analysis) to predict the depth and width of the channel.
These functions take the accuflux in square kilometers, while our original map has a cellsize in meters, so we need to convert to kilometer.
```
channelwidth.tif = 9.68*(accuflux.tif/1e6)**0.32
channeldepth.tif = 11.3*(accuflux.tif/1e6)**0.083
```
![Elevation manipulation](/LISEM/assets/img/calc_10.png)


### Fixing local depressions
In order to fix local depression, you can use the DrainageNetworkDEM function.
This applies all pit filling performed when creating an LDD to the DEM.
Note that the ldd considers diagonal connection valid.
```
dem_fix.tif = DrainageNetworkDEM(dem.tif,1e4,1e4,1e4,1e4)
```
Highlight the filled depressions by calculating the difference between the maps.
```
dif.tif = dem_fix.tif - dem.tif
```

When there is an abundance of noise in the elevation data, this can disturb flow models on flat terrain.
Edge-preserving image filters can be a solution in these cases. 
The bilateral filter only filters flat terrain, and keeps ridges, edges and valleys intact (depending on the input parameters).
```
dem_fix.tif = BilateralFilter(dem.tif,0.2,0.2)
```
![Elevation manipulation](/LISEM/assets/img/calc_11.png)


### Contour Lines
```
contour.shp = RasterContour(dem.tif,50.0)
```

![Elevation manipulation](/LISEM/assets/img/calc_9.png)


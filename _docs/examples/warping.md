---
title: Warping
permalink: /docs/warping/
---

Warping raster can be done in various ways:
You can get and set Coordinate Reference Systems (CRS) for layers using:
```
test.tif = SetCRS(tes.tif,GetCRS(test2.tif)); //get CRS from other file
test.tif = SetCRS(tes.tif,GeoProjectionFromWGS84UTM(30,true)); //use WGS84 UTM 30 N
test.tif = SetCRS(tes.tif,GeoProjectionFromEPSG(4326)); //use WGS84 (EPSG code 4326)

```

The SetCRS function does not do any warping to the data. If you wish to re-project the data, use the following code:
```
warped.tif = RasterProject(dem.tif,GeoProjectionFromWGS84UTM(30,true));
warped.shp = VectorWarp(rivers.shp,GeoProjectionFromWGS84UTM(30,true));
```
![Warping](/LISEM/assets/img/warp_calc1.png)

![Warping](/LISEM/assets/img/warp_calc2.png)

If you have a target raster (existing file with extent, crs and cell sizes), you can directly warp to this.
```
warped.tif = RasterWarp(target.tif,GeoProjectionFromWGS84UTM(30,true));
```

In order to select a subset of a raster dataset, use the ```RasterCut``` function.
This function takes either a ```Region```, or a pixel index.
```
section_custom.tif = RasterCut(dem.tif,Region(0,1.0,0.0,1.0));
section.tif = RasterCut(dem.tif,GetRegion(shape.shp));
```
![Warping](/LISEM/assets/img/warp_calc3.png)

You can convert a raster dataset to a vector dataset using the ```Vectorize``` function.
```
rivers.shp = Vectorize(channelwidth.map,"width");
```

![Warping](/LISEM/assets/img/warp_calc4.png)

You can also convert from vector to raster by using the ```Rasterize``` function.
Depending on the options, this will rasterize an attribute of the vector dataset, or burn a specified value.
```
channelwidth.tif = Rasterize(target.map,rivers.shp,"width"); //put attribute value of "width"
channelwidth_burn.tif = Rasterize(target.map,rivers.shp,"",true,1.0); //burn value 1.0
```

![Warping](/LISEM/assets/img/warp_calc5.png)
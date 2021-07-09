---
title: Data Manipulation
permalink: /docs/datamanipulation/
redirect_from: /docs/index.html
---

The scripting environment provides tools for manipulating geo-data.

![ScriptTool](/LISEM/assets/img/explain_scripttool.png)
 
The scripting language used by LISEM is specifically aimed at manipulating geodata files. It extends the language by allowing in-line references to geo-data files from your working directory. Otherwise, it is based on the c-style language Angelscript, and features classes, functions, loops, if/else, function callbacks and many other things.
The Command line allows you to perform single-line calculations. 
Here are some example calculations you can do in the command line

•	```dem.tif = dem.asc```

Loads a map called dem (in asc format) and saves it to GeoTiff format.

•	```slope.map = Slope(dem.map)```

Calculates the Slope of a map.

•	```calc.tif = dem.tif + slope.map^2.0 + sin(atan(sqrt(dem.map)))```

Compound calculations with differing map formats. Note that all raster must have the same number of rows and columns.

•	```flux.map = Accuflux(ldd.map,Slope.map)```

More complex geo-spatial algorithms such as flux accumulation

•	```dem_warped.tif = RasterWarp(dem.map,GeoProjectionFromESPG(3350))```

Warping using the GDAL libraries.

•	```AddViewLayer(dem.asc)```

Add a layer to the mapviewer.

•	```layers.tif = {dem.map,slope.map,WindowAverage(flux.map,100))```

Use curly brackets to save a list of raster to a multi-band file.

•	```layer.map = layers.tif[0] + layer.tif[1]```

Use square brackets to use a specific band of a multi-band raster file

•	```lines.shp = LineDetect(Slope(dem.map))```

Detect linear features in terrain, and store them as a shapefile.

•	```bridges.shp = rivers.shp | roads.shp```

Logical operators for shapefiles. This finds the intersection between rivers and roads.

•	```test.tbl[3] = test.tbl[0] + test.tbl[1] * test.tbl[2]```

Table objects also support operators. The type of the table determines what operators do.

•	```Print(MapAverageRed(dem.map))```

Print a number to the output window. The Map...Red functions act to reduce a map to a single number.

•	```test.tif += 100```

Collapsed operators applied directly to a file.

•	```test.tif += dem.map```

Collapsed operators applied directly to a file.


Some notes on these examples:
•	The syntax here differs from the scripting language in the sense that no main() function is needed. Additionally, there is no need to put the semicolon (;) at the end of the line. 
•	The full scripting language is available. Keep in mind that for most calculations, the number of rows and columns for each map must be equal.

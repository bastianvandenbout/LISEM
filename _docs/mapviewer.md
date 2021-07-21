---
title: Mapviewer
permalink: /docs/mapviewer/
redirect_from: /docs/index.html
---

The map viewer tool is part of the tool window, and features various options for opening and displaying layers of geo-spatial data.

![Mapview Tool](/assets/img/explain_mapviewtool.png)

You can save your currently loaded layers and style settings using the load and save buttons. 
The viewer supports the following types of geo-spatial data files:

1. Rasters
2. Vector data
3. PointCloud
4. Web tile raster layers
5. 3D (textured) Model Objects (Limited Support)

Additionally, on-the-fly hillshading and coordinate warping is supported.
When both the layer and the viewer have a coordinate referency system (CRS) selected, 
warping is automatically applied.

Opening a raster layer will add it to the map viewing window.

![Mapview](/assets/img/explain_mapviewer.png)

There are render to image/video options where resolution can be selected.
Additionally, it will add a layer widget with several options for styling and coordinate refernce systems.

![Mapview](/assets/img/explain_mapviewtoolitem.png)

By setting the minimum and maximum value, more detail can be drawn out of raster data.
The color gradient can be customized, while the visualization changes in real-time.
For vector data, attribute-dependent colorization can be selected.

When the shift button is pressed, dragging is disabled, and data can be inspected. 
This will highlight the exact location and current data value for the selected layer.

![Mapview](/assets/img/explain_inspect.png)

Other shortcuts are

**Shift + Click** = Highlight location

**Shift + Drag** = Highlight Square

**Shift + c + Click** = Copy geolocation to clip-board

**Shift + c + Drag** = Copy square to clip-board

**Shift + f + Click** = Highlight connected line segments


When the 3d view is toggled, and elevation data is present, it will be used to provide elevation for all other layers. 
Drag to rotate the camera, and use the w,a,s,d keys to move. When the ctrl-button is checked, 
the movement speed is increased. 
The inspection and editing functionality are all available in 3d mode.

![Mapview](/assets/img/explain_mapview3d.png)

Some data types (Raster and Vector) can be edited. This functionality simiarly works in 2d and 3d.
The raster editor functions as a paint tool, with paint, multiplication, blur and liquify brushes.
The vector editor allows removal, editing and addition of shapes to a file.



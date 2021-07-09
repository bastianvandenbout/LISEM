---
title: Getting Started
permalink: /docs/gettingstarted/
redirect_from: /docs/index.html
---

Getting started with LISEM can be done by first downloading the software from the [download page](/LISEM/docs/downloads/).
The sofware consist of two windows, the map viewer (left) and the tool window (right).

![Interface Overview](/LISEM/assets/img/interface.png)

The map viewer provides a geo-spatial view of all opened layers. 
You can use the mouse to drag the view around.
The tool windows provides access to all view settings, data manipulation and the LISEM model.
You can add a layer to the map view by selecting one of the buttons in the top right. 
Alternatively, double-clicking a supported file in the scriptool or drag and dropping a file to the map viewer will automatically add it as a layer.
The script tool proides the primary means of manipulating data.

![Interface Overview](/LISEM/assets/img/explain_scripttool.png)

Manipulating data can be done in the script tool. On the bottom right, a command line is available for simple calculations.
Browse to a folder with some geo-data files, and press the working-director to set the working directory. All files within the current working directory are available for calculation.
A simple calculation might be performed with 

```slope.tif = Slope(dem.tif)```

This will read a file dem.tif from the working directory, calculate the slope, and write to the file slope.tif.
If you wish to find details on a specific function, use the toolbox tool, where you can search all available functions.
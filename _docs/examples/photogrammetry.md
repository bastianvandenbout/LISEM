---
title: Web Data
permalink: /docs/webdata/
---

You can download files within the LISEM script engine, convert them to an object or save them as a file, and open them in the map viewer.
HTMl request are supported, so any webservice with WCS-service can be called upon.
Note that the region specified in the url must conform to the Coordinate Reference system used by the WCS-Service.
This can be checked in the url, as the EPSG code is often specified.

So, to get soil classifications from SOILGRIDS
```
soilclass.tif = ToMap(Download("http://maps.isric.org/mapserv?map=/map/wrb.map&SERVICE=WCS&VERSION=1.0.0&REQUEST=GetCoverage&coverage=MostProbable&CRS=EPSG:3978&BBOX=-2200000,-712631,3072800,3840000&WIDTH=3199&HEIGHT=2833&FORMAT=GTiff"));
```
![Interpolation](/assets/img/web_calc1.png)

Or, to get elevation data 
Using this function (repeatedly) leads to the following produced elevation data.
```
soilclass.tif = ToMap(Download("http://maps.isric.org/mapserv?map=/map/wrb.map&SERVICE=WCS&VERSION=1.0.0&REQUEST=GetCoverage&coverage=MostProbable&CRS=EPSG:3978&BBOX=-2200000,-712631,3072800,3840000&WIDTH=3199&HEIGHT=2833&FORMAT=GTiff"));
```
![Interpolation](/assets/img/web_calc2.png)

HTML with a POST-request are also supported.
To obtain OpenStreetMaps data you can use
```
void main()
{
	DownloadFilePost("http://overpass-api.de/api/interpreter",OSMExtentToXMLPost(Region(0.0,1.0,0.0,1.0),"osm.osm"));
	WaitForAllDownloads();
}
```


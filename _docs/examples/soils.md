---
title: Soils
permalink: /docs/soils/
---

A steady state soil depth model is available, that finds a balance between weathering, creep and movement according to a flux accumulation estimate.
This method requires calibration before being reliable.
```
soildepth.map = SteadyStateSoil(dem.map,dem.map * 0.0 + 0.1, dem.map * 0.0 + 0.2,100,0.2,1.0);
```

![Warping](/assets/img/soils_calc1.png)

The method above has troubles with larger channel networks due to the simple flow accumulation scheme.
A more extensive option is available where a custom accuflux map can be provided. Using the AccuFlux2D provides better spatial patterns.

```
soildepthn.map = AccufluxSoil(dem.map,AccuFlux2D(dem.map,dem.map * 0.0 + 1.0,dem.map * 0.0 + 1.0,500,0.2,1.0),dem.map * 0.0 + 0.001,0.0000001,2,500,0.2);
```

![Warping](/assets/img/soils_calc2.png)

The standardized USGS soil texture classification is implemented as a function.
The input should be in volume fraction (between 0 and 1, 0 meaning absent and 1 meaning 100 %).
The output is a class between 0 and 11 based on the USGS soil texture classes.


```
//both inputs as fractions between 0 and 1
textclass.tif = TextureClass(sand.tif, clay.tif)
```

![Warping](/assets/img/soils_calc3.png)


In order to facilitate the modelling of several types of hydrological processes,
the Saxton et al (2006) pedotransfer functions are provided.

```
ksat.tif = SaxtonKSat(sand.map,clay.map,organic.map,gravel.map);
sucton.tif = SaxtonSuction(sand.map,clay.map,organic.map,saturation.map,gravel.map);
thetar.tif = SaxtonFieldCapcity(sand.map,clay.map,organic.map,gravel.map);
thetas.tif = SaxtonPorosity(sand.map,clay.map,organic.map,gravel.map);
A.tif = SaxtonA(sand.map,clay.map,organic.map,gravel.map);
B.tif = SaxtonB(sand.map,clay.map,organic.map,gravel.map);
```

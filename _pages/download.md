---
title: Downloads
permalink: /docs/downloads/
---

You can find below all the available packages as part of the LISEM software.

PLEASE NOTE: LISEM classic and LISEM are different softwares. 
Dont know which one you want? See the [documentation](/docs/home)!



#### LISEM Classic
LISEM Classis provides a focus on detailed modelling of hydrology, flow and erosion.
See the [github page](https://github.com/vjetten/openlisem) and [classic downloads](https://github.com/vjetten/openlisem/releases/tag/openLisem) for more details.

Version 6.845

| [Windows-64x Portable executable](https://github.com/vjetten/openlisem/releases/download/openLisem/lisem-v6.846-220321.zip) | ![LISEM](/assets/img/openLisem.png) |


#### LISEM
The new LISEM software, a geo-spatial modelling and analysis tool focusing on multi-hazards, is now public in alpha-relase. 
WARNING: Contains various bugs!! 

Version 0.2.3

| [Windows-64x Portable zipped executable](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Portable/lisem_0.2.3.zip/download) | ![LISEM](/assets/img/logonav.png) |
| [Windows-64x installer](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Installer/LISEM-0.2.3-win64.exe/download) | ![LISEM](/assets/img/logonav.png) |
| [Windows-64x Python Wheel](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Python%20Wheel/lisem-0.2.3-py3-none-any.whl/download) | ![LISEM](/assets/img/logonav.png) |

Version 0.2.2

| [Windows-64x Portable zipped executable](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Portable/lisem_0.2.2.zip/download) | ![LISEM](/assets/img/logonav.png) |
| [Windows-64x installer](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Installer/LISEM-0.2.2-win64.exe/download) | ![LISEM](/assets/img/logonav.png) |
| [Windows-64x Python Wheel](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Python%20Wheel/lisem-0.2.2-py3-none-any.whl/download) | ![LISEM](/assets/img/logonav.png) |

Version 0.2.1

| [Windows-64x Portable zipped executable](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Portable/lisem_0.2.1.zip/download) | ![LISEM](/assets/img/logonav.png) |
| [Windows-64x installer](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Installer/LISEM-0.2.1-win64.exe/download) | ![LISEM](/assets/img/logonav.png) |
| [Windows-64x Python Wheel](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Python%20Wheel/lisem-0.2.1-py3-none-any.whl/download) | ![LISEM](/assets/img/logonav.png) |

Version 0.1.1

| [Windows-64x Portable zipped executable](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Portable/lisem_0.1.1.zip/download) | ![LISEM](/assets/img/logonav.png) |
| [Windows-64x installer](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Installer/LISEM-0.1.1-win64.exe/download) | ![LISEM](/assets/img/logonav.png) |
| [Windows-64x Python Wheel](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Python%20Wheel/lisem-0.1.1-py3-none-any.whl/download) | ![LISEM](/assets/img/logonav.png) |

#### OpenLISEM Hazard
The OpenLISEM Hazard model does not receive active support. It has become pretty stable, and is available here:

[Windows-64x Portable executable](https://sourceforge.net/projects/lisem/files/OpenLISEM%20Hazard%20%28public%20beta%29/)


### Additional files

#### Machine learning models
The following machine learning models have been made for the purpose of creating high-resolution elevation data from low-resolution source data.
Use the following code to apply the model to an elevation file:
```
hr.tif = ApplyMLModel({lr.tif},"model.onnx",32,32,{0,3,1,2});
```
Where "model.onnx" should be replaced with the model that you want to use.

[Dahal-EBRN-Austria](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Neural%20Networks/ebrn_at_dahal_model.onnx/download)

[Dahal-EBRN-Dominica (Best option)](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Neural%20Networks/ebrn_dm_dahal_model.onnx/download)

The following models are currently not yet working within LISEM, but could be used with the ONNX or Tensorflow API.
[Dahal-ESRGAN-Austria](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Neural%20Networks/esrgan_at_dahal_model.onnx/download)

[Dahal-ESRGAN-Dominica](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Neural%20Networks/esrgan_dm_dahal_model.onnx/download)

#### Example dataset for the LISEM model

Below are the example datasets for the LISEM tutorials on the youtube channel:

[LISEM model landslide simulation for Acheron](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/ExampleGeoData/acheron.zip/download)
[Dominica Grande-Bay data](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/ExampleGeoData/dominica.zip/download)
[Test Lake](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/ExampleGeoData/lake.zip/download)

The following links provide some more example datasets, featuring terrain elevation, geophysical parameters, land cover, soil types and precipitation data.
These can be used to test the physically-based model. They were originally created for the old OpenLISEM Hazard software,
but provide enough information to run LISEM or LISEM classic.

[Example datasets](https://sourceforge.net/projects/lisem/files/Example%20Datasets/)

#### Additional files

[Old OpenLISEM Hazard manual](https://sourceforge.net/projects/lisem/files/Documentation%20and%20Manual/documentation15.pdf/download)

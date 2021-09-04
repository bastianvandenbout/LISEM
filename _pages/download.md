---
title: Downloads
permalink: /docs/downloads/
---

Please find below all the available packages as part of the LISEM software 

### LISEM
The LISEM software is now public in alpha-relase. WARNING: Contains various bugs!! 

Version 0.1.1

| [Windows-64x Portable zipped executable](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Portable/lisem_0.1.1.zip/download) | ![LISEM](/assets/img/logonav.png) |
| [Windows-64x installer](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Installer/LISEM-0.1.1-win64.exe/download) | ![LISEM](/assets/img/logonav.png) |
| [Windows-64x Python Wheel](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Python%20Wheel/lisem-0.1.1-py3-none-any.whl/download) | ![LISEM](/assets/img/logonav.png) |

Version 0.1.0

| [Windows-64x Portable zipped executable](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Portable/lisem.zip/download) | ![LISEM](/assets/img/logonav.png) |
| [Windows-64x installer](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Installer/LISEM-0.1.0-win64.exe/download) | ![LISEM](/assets/img/logonav.png) |
| [Windows-64x Python Wheel](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Python%20Wheel/lisem-0.1.0-py3-none-any.whl/download) | ![LISEM](/assets/img/logonav.png) |

### LISEM Classic
LISEM Classis provides a focus on detailed modelling of hydrology, flow and erosion.

| [Windows-64x Portable executable](https://sourceforge.net/projects/lisem/files/openLISEM6.62beta.zip/download) | ![LISEM](/assets/img/openLisem.png) |

### OpenLISEM Hazard
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

[Dahal-ESRGAN-Austria](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Neural%20Networks/esrgan_at_dahal_model.onnx/download)

[Dahal-ESRGAN-Dominica (Best option)](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Neural%20Networks/esrgan_dm_dahal_model.onnx/download)

[Dahal-EBRN-Austria](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Neural%20Networks/ebrn_at_dahal_model.onnx/download)

[Dahal-EBRN-Dominica](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Neural%20Networks/ebrn_dm_dahal_model.onnx/download)


#### Example dataset for the LISEM model

The following links provide example datasets, featuring terrain elevation, geophysical parameters, land cover, soil types and precipitation data.
These can be used to test the physically-based model. They were originally created for the old OpenLISEM Hazard software,
but provide enough information to run LISEM or LISEM classic.

[Example datasets](https://sourceforge.net/projects/lisem/files/Example%20Datasets/)

#### Additional files

[Old OpenLISEM Hazard manual](https://sourceforge.net/projects/lisem/files/Documentation%20and%20Manual/documentation15.pdf/download)

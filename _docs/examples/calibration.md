---
title: Calibration
permalink: /docs/calibration/
---


Calibration Script
```
double Calibrate(array<double> params)
{​​​


RunModel("run/Couli_AutoCalib.run","Internal Friction Angle_cal_mult="+ToString(params[0])+"|" + "Cohesion Bottom_cal_mult="+ToString(params[1]));

Map impact = LoadMap("res/safetyfactor.map");
Map inventory = 1.0-LoadMap("maps/inventory.tif");
double errorval = MapContinuousCohensKappa(inventory,impact,1.0);

Print("Running model with ifa= " + ToString(params[0]) +"& coh= " + ToString(params[1]) + " score = " + ToString(1.0-errorval));

return (1.0-errorval);
}​​​


void main()
{​​​
OptimizeCustom({​​​1.0,1.0}​​​,{​​​true,true}​​​,@Calibrate,0.01,0.05);
}​​​
```

ContinuousCohensKappa

```
double errorval = MapContinuousCohensKappa(inventory,impact,1.0);
```

Gradient Descent
```
OptimizeCustom({​​​1.0,1.0}​​​,{​​​true,true}​​​,@Calibrate,0.01,0.05); 
```
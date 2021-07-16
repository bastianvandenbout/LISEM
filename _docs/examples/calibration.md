---
title: Calibration
permalink: /docs/calibration/
---

The LISEM software includes an automated calibration procedure based on gradient descent with Armijo-backtracking.
The function is called OptimizeCustom. It takes as an argument a list of initial parameter values, a list of true/false indicating wether the values must be postive-only.
The third argument is a custom function that return an error number. This error number will be minimized during the gradient descent.
Finally, there are the finite step and gradient descent step.
```
OptimizeCustom({​​​1.0,1.0}​​​,{​​​true,true}​​​,@Calibrate,0.01,0.05); 
```
The gradient descent algorithm attempt to find the gradient of the error function using variations in the parameter values (the size of the variation is the infinite step).
Once the gradient is determined, the algorithm moves towards lower error with a step size determined by the gradient descent step parameter.

In the example below, the Calibrate function runs the LISEM model, and determines the error by comparing predicted slope failures to mapped inventory.
The calibrate function in fact do any type of calculation, as long as a consistent error value is returned.

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

In the example above, the error is estimated by using the Continuous Cohens Kappa value. This is a continous adaptation of the Cohens Kappa performence metric.
Other options, such as percentage accuracy, or root mean square error can also be included for one or more output indicators.

```
double errorval = MapContinuousCohensKappa(inventory,impact,1.0);
```


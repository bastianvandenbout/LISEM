---
title: Dominica
permalink: /docs/showcase1/
---

Using the OpenLISEM model, detailed multi-hazard simulations for the impact of hurricane Maria on Dominica were carried out.
Dominica is a small carribean island (~750 km2) with a population of around 70.000 poeple. 
The vulcanic slopes are draped with rich tropical forests and mixed tropical agriculture such as sugar cane and bananas.

In the past decades, several high-category hurricanes have hit the island. 
Most notably, Hurricane David (1972), Hurricane Erika (2012), and finally Hurricane Maria (2017). 
On September 19, 2017, the category 5 hurricane named Maria made landfall on the Island of Dominica. 
Just before making landfall, there was a short and intense period of intensity growth, unpredicted by local and regional forecasters.
The hurricane lead to extreme wind speeds, storm surges and cumulative rainfall throughout the island. 
Damages where caused to virtually all buildings, both due to winds damaging the roofs or walls, or floods and landslides damaging the buildings. 
Over 70 percent of all buildings where reported to have been significantly damaged during the event. Hurricane Maria lead to a total of 65 fatalities (Dominica News Online, 2018). 
The total damage was assessed to be 1.37 billion USD (ReliefWeb, 2018), over twice the GDP of the island. 

The elevation of the island, as well as the impact inventory (featuring debris flows, landslides and flash flood processes) are shown below.
![Dominica-Maria](/assets/img/dominica_inventory.png)

The precipitation patterns for hurricane Maria are shown below, from ground-based radar data from Guadaloupe and Martinique. The total precipitation highlights the extremity of the event, with in most locations a total rainfall of over 0.5 meters in a single day.
![Dominica-Maria](/assets/img/rain.png)

In the south-east of Dominica lies the towns of Grande-Bay and pichelin. This area was struck most severly, featuring an extremely high density of slope failures throughout.
Over 95 percent of all houses were damaged by the event, either through wind or flow processes. 
The input data as used for the model is shown below.
![Dominica-Maria](/assets/img/map_70.png)

The damage data for hurricane Maria in the Grande-Bay area was based on the process inventory and building damages.
![Dominica-Maria](/assets/img/map_damageall_70.png)

A full simulation of physically-based prediction of the event was carried out with the LISEM model. 
Using the depth-averaged generalized debris flow equations from state-of-the-art literature, complex interactions between mass movements, hydrology and flooding are captured.
This includes processes such as slope failures, transition from landslide to debris flows, channel blocking, and mass movement dilution due to flash flooding.
![Dominica-Maria](/assets/img/figure9.png)

The automated calibration process with LISEM allowed for an efficient project setup. 
The gradient descent algorithm linked with the Cohens Kappa accuracy metric converged rapidly to an optimal reproduction of the event.
Final accuracies of over 88 percent, with a Cohens Kappa of 0.6 were obtained.
![Dominica-Maria](/assets/img/dominica_calibration.png)

Predictions for the capital of the island, Roseau, show the need for strengtened bride design and a wholistic approach to multi-hazard risk.
![Dominica-Maria](/assets/img/dominica_roseau.png)


See also the following video, which highlights the simulation in 3d.

<iframe width="560" height="315" src="https://www.youtube.com/embed/2CmwgLJNZuY" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
 
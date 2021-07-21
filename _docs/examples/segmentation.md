---
title: Segmentation
permalink: /docs/segmentation/
---

LISEM includes several segmentation algorithms such as mean-shift segmentation and watershed segmentation.

```
segmentation.tif = SegmentizeMS(sentinel.tif,100,500,20,200)
```
![Segmentation](/assets/img/segment_calc1.png)


![Segmentation](/assets/img/segment_calc2.png)



The final segments can be colorized using a colorization function.

```
segments.tif = ClassToRGB(segmentation.tif)
```

![Segmentation](/assets/img/segment_calc3.png)



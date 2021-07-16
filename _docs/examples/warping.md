---
title: Warping
permalink: /docs/warping/
---

```
void main()
{
    
    array<string> names = GetItemsContaining(GetFilesInDir(),{"B01","B02","B03","B04","B05","B06","B07","B08"});
    
    Map mask = RasterCut(LoadMap(names[1]),Region(301575,334651,5.72216e+06,5.75079e+06));
    array<Map> bands;
    for(int i = 0; i < names.length(); i++)
    {
        bands.Append(RasterWarp(mask,RasterCut(LoadMap(names[i]),Region(301575,334651,5.72216e+06,5.75079e+06))));
    }
    test.tif = bands;
}
```


	
RasterWarp

RasterCut


RasterToVector

VectorToRaster

PointCloudToDem


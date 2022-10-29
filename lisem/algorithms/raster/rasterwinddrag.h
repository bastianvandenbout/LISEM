#ifndef RASTERWINDDRAG_H
#define RASTERWINDDRAG_H

#include "rastercommon.h"
#include "geo/raster/map.h"
#include <vector>

inline std::vector<cTMap *> AS_FlowWindDrag(cTMap * H, cTMap * UX, cTMap * UY, cTMap * WUX, cTMap * WUY, cTMap * L, cTMap * rho_air, float chi, float cw, float dt, float h_wind = 10.0)
{
    CheckMapsSameSizeError("FlowWindDrag",{"Flow Height","Flow x velocity","Flow y velocity","Wind x velocity","wind y velocity","local shielding factor","density of the air"},{H,UX,UY,WUX,WUY,L,rho_air})

    std::vector<cTMap *> res;
    //u_wind = (u/k)log(z/z0)
    //k = von Karmen constant
    //what is chi? 1/(1-exp(-9.81 * x))
    //
    //du/dt = (1/h)(rho_air/(rho_water *V)) * chi*chi * cw * |u_wind/chi - u|*(u_wind/chi - u)
    //dv/dt = (1/h)(rho_air/(rho_water *V)) * chi*chi * cw * |v_wind/chi - v|*(v_wind/chi - v)




    return res;

}

//sewer model
//we have manholes, connection with pipe networks of specified diameter, direction and mannings N
//each of the manholes has an associated depth
//slope for the pipes is based on the manhole depth differences
//manhole info as raster?
//manhole variables (height in particular as raster)
//flow paths


#endif // RASTERWINDDRAG_H

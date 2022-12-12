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


#endif // RASTERWINDDRAG_H

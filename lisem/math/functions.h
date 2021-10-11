#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <boost/math/special_functions/gamma.hpp>
#include "math.h"

inline double GammaUpperIncomplete(double a, double z)
{
    return boost::math::tgamma(a,z);

}
inline double GammaLowerIncomplete(double a, double z)
{
    return boost::math::tgamma_lower(a,z);

}

#endif // FUNCTIONS_H

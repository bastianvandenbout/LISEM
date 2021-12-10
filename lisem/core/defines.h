#ifndef DEFINES_H
#define DEFINES_H

#ifdef LISEM_BUILD
#ifdef _MSC_VER
#  define LISEM_API __declspec(dllexport)
#elif defined(__GNUC__)
#  define LISEM_API __attribute__((visibility("default" )))

#else
#  define LISEM_API
#endif
#else
#  define LISEM_API __declspec(dllimport)
#endif

#define LSMFLOAT float

#include "qmetatype.h"
#include "QString"
#include "armadillo"

namespace arma
{
  extern thread_local arma_rng_cxx11 arma_rng_cxx11_instance;
}


#define WIN32

#define ARMA_DONT_USE_SUPERLU
#define LISEM_PI 3.141592653589793238462643383279502884197169399375105820974944
#define LISEM_1DIVPI 0.31830988618
#define LISEM_PIdiv180 (LISEM_PI/180.0)
#define M_PI 3.14159265358979323846/* pi */
#define M_PI_2 1.57079632679489661923 /* pi/2 */
#define M_PI_4 0.78539816339744830962 /* pi/4 */
#define M_1_PI 0.31830988618379067154
#define M_2_PI 0.63661977236758134308
#define M_2_SQRTPI 1.12837916709551257390
#define M_E 2.7182818284590452354
#define M_LN10 2.30258509299404568402
#define M_LN2 0.69314718055994530942
#define M_LOG10E 0.43429448190325182765
#define M_LOG2E 1.4426950408889634074
#define M_SQRT1_2 0.70710678118654752440
#define M_SQRT2 1.41421356237309504880

extern QString splitterSheetH;

extern QString splitterSheetV;

extern QString splitterSheet;


template<class T>
inline const T& min_(const T& a, const T& b)
{
 return a < b ? a : b;
}

template<class T>
inline const T& min_(const T& a, const T& b, const T& c)
{
 return a < b ? min_(a, c) : min_(b, c);
}

template<class T>
inline const T& max_(const T& a, const T& b)
{
 return a < b ? b : a;
}

template<class T>
inline const T& max_(const T& a, const T& b, const T& c)
{
 return a < b ? max_(b, c) : max_(a, c);
}

template<class T>
inline T abs_(const T& a)
{
 return a < (T)0 ? -a : a;
}

template<class T>
inline T lerp(const T& a, const T& b, const float t)
{
return (T)(a*(1.f-t)) + (b*t);
}

template<class T>
inline T lerp(const T& a, const T& b, const double t)
{
return (T)(a*(1.f-t)) + (b*t);
}

template <class T>
inline const T clamp (const T& value, const T& low, const T& high)
{
 return min_ (max_(value,low), high);
}
inline const float clampf(float value, float low, float high)
{
 return min_(max_(value,low), high);
}


#endif // DEFINES_H

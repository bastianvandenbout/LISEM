#ifndef DEFINES_H
#define DEFINES_H


#ifdef LISEM_BUILD
#  define LISEM_API __declspec(dllexport)
#else
#  define LISEM_API __declspec(dllimport)
#endif


#define LISEM_PI 3.141592653589793238462643383279502884197169399375105820974944
#define LISEM_1DIVPI 0.31830988618
#define LISEM_PIdiv180 (LISEM_PI/180.0)

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

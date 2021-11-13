#ifndef QUANTILE_H
#define QUANTILE_H

#include <algorithm>
#include <cmath>
#include <vector>

template<typename T>
static inline double Lerp(T v0, T v1, T t)
{
    return (1 - t)*v0 + t*v1;
}

template<typename T>
static inline std::vector<T> Quantile(std::vector<T>& inData, std::vector<T>& probs)
{
    if (inData.empty())
    {
        return std::vector<T>();
    }
std::cout << "AS_Quantile1"<< std::endl;
    if (1 == inData.size())
    {
        return std::vector<T>(1, inData[0]);
    }
std::cout << "AS_Quantile2"<< std::endl;
    std::vector<T> data = inData;
    std::sort(data.begin(), data.end());
    std::vector<T> quantiles;
std::cout << "AS_Quantile3"<< std::endl;
    for (size_t i = 0; i < probs.size(); ++i)
    {
        if(probs[i] > 1.0)
        {
            probs[i] = 1.0;
        }
        T poi = Lerp<T>(-0.5, data.size() - 0.5, probs[i]);

        size_t left = std::max(int64_t(std::floor(poi)), int64_t(0));
        size_t right = std::min(int64_t(std::ceil(poi)), int64_t(data.size() - 1));

        T datLeft = data.at(left);
        T datRight = data.at(right);

        T quantile = Lerp<T>(datLeft, datRight, poi - left);

        quantiles.push_back(quantile);
    }
std::cout << "AS_Quantile4"<< std::endl;
    return quantiles;
}


static inline std::vector<double> AS_Quantile(std::vector<double> data, std::vector<double> probs)
{

    std::cout << "AS_Quantile"<< std::endl;
    return Quantile<double>(data,probs);
}
static inline std::vector<double> AS_Quantilef(std::vector<float> data, std::vector<float> probs)
{
    std::cout << "AS_Quantile"<< std::endl;
    std::vector<float> ret = Quantile<float>(data,probs);
    std::vector<double> retd;
    for(int i = 0; i < ret.size(); i++)
    {
        retd.push_back(ret[i]);
    }
    return retd;
}

static inline double AS_Averagef(std::vector<float> data)
{

    int n = data.size();
    double totalSum = 0.0;
    // Sum all the entries in the matrix.
     #pragma omp parallel for reduction(+: totalSum) if(n> 1000)
     for (int i = 0; i < n; ++i) {

        totalSum += data[i];
     }
    return totalSum;
}

static inline double AS_Average(std::vector<double> data)
{
    int n = data.size();
    double totalSum = 0.0;
    // Sum all the entries in the matrix.
     #pragma omp parallel for reduction(+: totalSum) if(n> 1000)
     for (int i = 0; i < n; ++i) {

        totalSum += data[i];
     }
    return totalSum;
}

static inline double AS_StdDevf(std::vector<float> data)
{
    int n = data.size();
    double totalSum = 0.0;
    // Sum all the entries in the matrix.
     #pragma omp parallel for reduction(+: totalSum) if(n> 1000)
     for (int i = 0; i < n; ++i) {

        totalSum += data[i];
     }
    double average = totalSum /std::max(1.0,((double)(n)));

    totalSum = 0.0;
    // Sum all the entries in the matrix.
     #pragma omp parallel for reduction(+: totalSum) if(n> 1000)
     for (int i = 0; i < n; ++i) {

        totalSum += (data[i] - average)*(data[i] - average);
     }
    return std::sqrt(totalSum /std::max(1.0,((double)(n))));
}

static inline double AS_StdDev(std::vector<double> data)
{
    int n = data.size();
    double totalSum = 0.0;
    // Sum all the entries in the matrix.
     #pragma omp parallel for reduction(+: totalSum) if(n> 1000)
     for (int i = 0; i < n; ++i) {

        totalSum += data[i];
     }
    double average = totalSum /std::max(1.0,((double)(n)));

    totalSum = 0.0;
    // Sum all the entries in the matrix.
     #pragma omp parallel for reduction(+: totalSum) if(n> 1000)
     for (int i = 0; i < n; ++i) {

        totalSum += (data[i] - average)*(data[i] - average);
     }
    return std::sqrt(totalSum /std::max(1.0,((double)(n))));
}

#endif // QUANTILE_H

#ifndef CORRELATIONS_H
#define CORRELATIONS_H

#include "matrixtable.h"
#include <iostream>
#include <vector>
#include <numeric>      // std::iota
#include <algorithm>    // std::sort, std::stable_sort
#include <QList>


inline QList<double> sort_indexes_d(const QList<double> &v)
{

  // initialize original index locations
  QList<double> idx;
  for(int i = 0; i < v.length(); i++)
  {
      idx.append(i);
  }

  // sort indexes based on comparing values in v
  // using std::stable_sort instead of std::sort
  // to avoid unnecessary index re-orderings
  // when v contains elements of equal values
  qSort(idx.begin(), idx.end(),
       [&v](double i1, double i2) {return v[i1] < v[i2];});

  return idx;
}

inline double PearsonCorrelation(QList<double> x, QList<double> y)
{
    //get average for x and y;

    double x_avg = 0.0;
    double y_avg = 0.0;

    double n = double(std::max(1,x.length()));

    for(int i = 0; i < x.length(); i++)
    {
        x_avg += x[i];
        y_avg += y[i];
    }
    x_avg = x_avg/n;
    y_avg = y_avg/n;
    //get summations

    double sum_1 = 0.0;
    double sum_2 = 0.0;
    double sum_3 = 0.0;

    for(int i = 0; i < x.length(); i++)
    {
        sum_1 += (x[i] - x_avg)*(y[i] - y_avg);
        sum_2 += (x[i] - x_avg)*(x[i] - x_avg);
        sum_3 += (y[i] - y_avg)*(y[i] - y_avg);
    }

    sum_2 = std::sqrt(std::max(1e-30,sum_2));
    sum_3 = std::sqrt(std::max(1e-30,sum_3));

    return sum_1/(sum_2*sum_3);
}


inline float TableCorrelationPearson(MatrixTable * table)
{
   if(table->GetNumberOfCols() != 2)
   {
        LISEMS_ERROR("There are not exactly 2 columns in the table. Correlation calculation requires 2 columns for x and y values respectively.")
       throw 1;
       return 0.0;
   }

   int n = std::max(1,table->GetNumberOfRows());
   QList<double> x;
   QList<double> y;
   for(int i = 0; i < table->GetNumberOfRows(); i++)
   {
        x.append(table->GetValueDouble(i,0));
        y.append(table->GetValueDouble(i,1));
   }

   return PearsonCorrelation(x,y);

}

inline float TableCorrelationSpearman(MatrixTable * table)
{

    std::cout << "table correlation " << table->GetNumberOfCols() <<std::endl;
    std::cout << table->GetNumberOfRows() <<std::endl;
    if(table->GetNumberOfCols() != 2)
    {
         LISEMS_ERROR("There are not exactly 2 columns in the table. Correlation calculation requires 2 columns for x and y values respectively.")
        throw 1;
        return 0.0;
    }

    int n = std::max(1,table->GetNumberOfRows());
    QList<double> x;
    QList<double> y;
    for(int i = 0; i < table->GetNumberOfRows(); i++)
    {
         x.append(table->GetValueDouble(i,0));
         y.append(table->GetValueDouble(i,1));
    }

    float corr = PearsonCorrelation(sort_indexes_d(x),sort_indexes_d(y));

    std::cout << "foudn corr " << corr << std::endl;
    return corr;

}

#endif // CORRELATIONS_H

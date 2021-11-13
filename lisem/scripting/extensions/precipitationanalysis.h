#ifndef PRECIPITATIONANALYSIS_H
#define PRECIPITATIONANALYSIS_H


#include "matrixtable.h"
#include "optimization/leastsquares.h"
#include "stat/quantile.h"
#include "scriptarrayhelpers.h"

inline static MatrixTable * CreateAveragedHyetograph(CScriptArray * data_precipitation, double timestep, bool is_intensity, double event_mm, int timesteps_event, int break_timesteps)
{
    MatrixTable * t = new MatrixTable();





    return t;

}



inline static MatrixTable * CreateAlternatingBlockHyetograph(MatrixTable *  data_idf)
{

   MatrixTable * t = new MatrixTable();

   t->SetSize(data_idf->GetNumberOfRows(),data_idf->GetNumberOfCols());


   int n_return_periods = t->GetNumberOfCols() - 1;
   int n_times = t->GetNumberOfRows();

   double timestep = t->GetValueDouble(0,0);
   //header
   for(int i = 0; i < n_times; i++)
   {

       t->SetValue(i,0,t->GetValueDouble(i,0));
   }


   for(int i = 0; i < n_return_periods; i++)
   {
       //get values
       QList<double> vals;
       for(int j = 0; j < n_times; j++)
       {
            vals.append(data_idf->GetValueDouble(j,i+1));
       }

       //construct curve
       QList<double> curve;
       double rain_total =0.0;


       for(int j = 0; j < n_times; j++)
       {
           double rain = vals.at(j) * ((double)(j+1)) - rain_total;
           rain_total += rain;
           if(j % 2 == 0)
           {
               curve.prepend(rain);
           }else{
                 curve.append(rain);
            }
       }


       //sort in descending order
       std::sort(curve.rbegin(),curve.rend());

       QList<double> curve2 = curve;
       curve.clear();
       for(int j = 0; j < n_times; j++)
       {
           if(j % 2 == 0)
           {
               curve.prepend(curve2[j]);
           }else{
                 curve.append(curve2[j]);
            }

       }

       //place data
       for(int j = 0; j < n_times; j++)
       {
            t->SetValue(j,i+1,curve.at(j));
       }

   }

   return t;
}



inline static MatrixTable * CreateIDF(CScriptArray * data_precipitation, double timestep /*hour*/ , bool intensities, int n_durations, CScriptArray * return_periods )
{
    //check and translate input data

    if(!(data_precipitation->IsNumberType() && return_periods->IsNumberType()))
    {
        LISEMS_ERROR("input array<> does not contain numbers");
        throw 1;
    }

    if(n_durations < 1)
    {
        LISEMS_ERROR("Number of durations should be higher then 0");
        throw 1;
    }

    QList<double> *ListP = data_precipitation->ToQListDouble();
    QList<double> *ListRP = return_periods->ToQListDouble();


    if(!intensities)
    {
        for(int i = 0; i < ListP->length() ; i++)
        {
            ListP->replace(i,ListP->at(i)/timestep);
        }
    }

    if(ListP->length() < 2)
    {
        LISEMS_ERROR("Precipitation list must be of length > 2 (current length " + QString::number(ListP->length()) + ")");
        throw 1;
    }

    if(ListRP->length() < 1)
    {
        LISEMS_ERROR("Return period list must be of length > 0");
        throw 1;
    }


    QList<double> ListPC;
    for(int i = 0; i < ListP->length() ; i++)
    {
        ListPC.append(ListP->length());
    }

    int n_return_periods = ListRP->length();

    int n_full_years = (ListP->length() * timestep)/(24.0*365.0);

    //reserve matrix table

    MatrixTable * t = new MatrixTable();
    t->SetSize(n_durations,n_return_periods+1);

    for(int j = 0; j < n_durations; j++)
    {
        t->SetValue(j,0,(double)(j+1) * timestep);

    }

    //for each of the durations
    for(int i = 0; i < n_durations; i++)
    {
        int duration_mult = i+1;

        ListPC.clear();

        std::cout << "resample timesereies " << i << std::endl;

        //resample timeseries
        for(int j = 0; j <ListP->length() ; j++)
        {
            double val_resamp = 0.0;
            int jsub = std::floor(std::max(0,duration_mult-1)/2.0);
            int jadd = std::floor(duration_mult/2.0) + 1;
            int n = 0;
            for(int k = std::max(0,j - jsub);  k < std::min(ListP->length(),j + jadd); k++)
            {
                n++;
                val_resamp += ListP->at(k);
            }
            if(n > 0)
            {
                val_resamp = val_resamp/((double)(n));
            }
            ListPC.append(val_resamp);
        }

        std::cout <<"yearly maxima "<<std::endl;

        //get yearly maxima
        int n_in_year = 24.0*365.0/timestep;

        QList<double> yearmax;
        for(int k = 0; k < n_full_years; k++)
        {
            double maxval = 0.0;
            for(int j = 0; j < n_in_year; j++)
            {
                maxval = std::max(maxval,ListPC.at(k * n_in_year + j));
            }
            yearmax.append(maxval);
        }

        std::cout << "sort "<< std::endl;

        //sort list
        std::sort(yearmax.begin(),yearmax.end());
        QList<double> rp;
        for(int k = 0; k < n_full_years; k++)
        {
            rp.append(1.0 - ((double)(1+k))/(1.0 + ((double)(n_full_years))));
        }

        std::cout << "fit" << std::endl;

        //fit gumbell
        QList<double> coefficients = OptimizeGumbel(yearmax,rp);


        double mu = 0.0;
        double beta = 0.0;

        std::cout << "get coefficients "<< std::endl;

        if(coefficients.length() > 1)
        {
            mu = coefficients.at(0);
            beta = coefficients.at(1);
        }

        std::cout << "get results " << std::endl;
        //get values at return periods
        for(int j = 0; j < n_return_periods; j++)
        {
            double val = mu - beta * std::log(-std::log(1.0 -(1.0/((double)(ListRP->at(j))))));

            t->SetValue(i,1+j,val);

        }
    }

    std::cout << "return " << std::endl;
    delete ListP;
    delete ListRP;

    return t;
}



inline static void RegisterPrecipitationAnalysisToScriptEngine(LSMScriptEngine * sm)
{

    int r = sm->RegisterGlobalFunction("Table @IDFCurves(array<double> & in precipitation, double timestep, bool is_intensities, int n_durations, array<double> & in return_periods)",asFUNCTIONPR(CreateIDF,(CScriptArray*, double , bool, int, CScriptArray* ),MatrixTable*),asCALL_CDECL);  assert( r >= 0 );
    sm->RegisterGlobalFunction("Table @ABHyetograph(Table & in idf)",asFUNCTIONPR(CreateAlternatingBlockHyetograph,(MatrixTable*),MatrixTable*),asCALL_CDECL);  assert( r >= 0 );

    sm->RegisterGlobalSTDFunction("array<double> @Quantiles(array<double> &in data, array<double> &in probs)", GetFuncConvert(AS_Quantile));
    sm->RegisterGlobalSTDFunction("array<double> @Quantiles(array<float> &in data, array<float> &in probs)", GetFuncConvert(AS_Quantilef));
    sm->RegisterGlobalSTDFunction("double Average(array<double> &in data)", GetFuncConvert(AS_Average));
    sm->RegisterGlobalSTDFunction("double Average(array<float> &in data)", GetFuncConvert(AS_Averagef));
    sm->RegisterGlobalSTDFunction("double StdDev(array<double> &in data)", GetFuncConvert(AS_StdDev));
    sm->RegisterGlobalSTDFunction("double StdDev(array<float> &in data)", GetFuncConvert(AS_StdDevf));

    //r = sm->RegisterGlobalFunction("array<double> @FitGumbel(array<double> & in x, array<double> & in y)",asFUNCTIONPR(OptimizeGumbel,(CScriptArray*,CScriptArray*),CScriptArray*),asCALL_CDECL);  assert( r >= 0 );
    //if(r < 0){LISEM_DEBUG("error in registering scripting array<double>  &FitGumbel(array<double> x, array<double> y)");};



}



#endif // PRECIPITATIONANALYSIS_H

#pragma once

#include <mlpack/core.hpp>
#include <mlpack/methods/logistic_regression/logistic_regression.hpp>
#include <mlpack/methods/linear_regression/linear_regression.hpp>


#include <ensmallen.hpp>

#include "geo/raster/map.h"


//using namespace std;
using namespace mlpack;
using namespace mlpack::regression;
using namespace mlpack::util;


static MathFunction MapLogisticRegression(std::vector<cTMap*> covariates, cTMap * data)
{

    //get map list

    std::vector<cTMap*> covariatemaps = covariates;
    std::vector<cTMap*> covariatemapsnorm;
    std::vector<double> covariatemin;
    std::vector<double> covariatemax;


    if(covariatemaps.size() < 1)
    {
        LISEMS_ERROR("Can not run logistic regression without co-variates");
        throw 1;
    }


    for(int i = 0; i < covariatemaps.size(); i++)
    {
        std::cout << "covariate i " << data->nrCols() << " " << covariatemaps.at(i)->nrCols() << "  " <<data->nrRows() << "  " << covariatemaps.at(i)->nrRows() << std::endl;
        if((data->nrCols() == covariatemaps.at(i)->nrCols()) && (data->nrRows() == covariatemaps.at(i)->nrRows()))
        {
            double min = 0.0;
            double max = 0.0;
            cTMap * mnorm = NormalizeMap(covariatemaps.at(i),min,max);
            covariatemapsnorm.push_back(mnorm);
            covariatemin.push_back(min);
            covariatemax.push_back(max);
        }else
        {
            LISEMS_ERROR("Not all co-variates are of identical size to the observation data");
            for(int k = 0; k < covariatemapsnorm.size(); k++)
            {
                delete covariatemapsnorm.at(k);
            }
            throw 1;
        }
    }

    int n = 0;

    for(int r = 0; r < data->data.nr_rows();r++)
    {
        for(int c = 0; c < data->data.nr_cols();c++)
        {
            if(!pcr::isMV(data->data[r][c]))
            {
                bool ismv = false;
                for(int k = 0; k < covariatemapsnorm.size(); k++)
                {
                    if(pcr::isMV(covariatemapsnorm.at(k)->data[r][c]))
                    {
                        ismv = true;
                    }
                }
                if(!ismv)
                {
                    n++;
                }
            }
        }
    }

    //prepare arma matrix data for covariates and labels

    arma::mat regressors = arma::mat(covariatemapsnorm.size(),n);
    arma::Row<size_t> responses = arma::Row<size_t>(n);

    int ni = 0;
    for(int r = 0; r < data->data.nr_rows();r++)
    {
        for(int c = 0; c < data->data.nr_cols();c++)
        {
            if(!pcr::isMV(data->data[r][c]))
            {
                bool ismv = false;
                for(int k = 0; k < covariatemapsnorm.size(); k++)
                {
                    if(pcr::isMV(covariatemapsnorm.at(k)->data[r][c]))
                    {
                        ismv = true;
                    }
                }
                if(!ismv)
                {
                    for(int k = 0; k < covariatemapsnorm.size(); k++)
                    {
                        regressors(k,ni) = covariatemapsnorm.at(k)->data[r][c];
                    }
                    responses(ni) = data->data[r][c] > 0.5? 1:0;

                    ni++;
                }
            }
        }
    }

    const double lambda = 0.0;
    const string optimizerType = "lbfgs";
    const double tolerance =  1e-10;
    const double stepSize = 0.01;
    const size_t batchSize =  64;
    const size_t maxIterations = (size_t) 10000;
    const double decisionBoundary = 0.5;


    LogisticRegression<>* model;

    model = new LogisticRegression<>(0, 0);

    model->Parameters() = arma::zeros<arma::rowvec>(regressors.n_rows);

    model->Lambda() = lambda;

    if (optimizerType == "sgd")
    {
      ens::SGD<> sgdOpt;
      sgdOpt.MaxIterations() = maxIterations;
      sgdOpt.Tolerance() = tolerance;
      sgdOpt.StepSize() = stepSize;
      sgdOpt.BatchSize() = batchSize;
      //Log::Info << "Training model with SGD optimizer." << endl;

      // This will train the model.
      model->Train(regressors, responses, sgdOpt);
    }
    else if (optimizerType == "lbfgs")
    {
      ens::L_BFGS lbfgsOpt;
      lbfgsOpt.MaxIterations() = maxIterations;
      lbfgsOpt.MinGradientNorm() = tolerance;
      //Log::Info << "Training model with L-BFGS optimizer." << endl;

      // This will train the model.
      model->Train(regressors, responses, lbfgsOpt);
    }


    arma::rowvec pars = model->Parameters();

    MathFunction f;
    QString exp = " 1.0/(1.0 + exp(-( ";


    for(int k = 0; k < pars.n_cols; k++)
    {   if(k == 0)
        {
            exp += QString::number(pars(k));
        }else
        {
            exp += QString::number(pars(k)) + "* (x" + QString::number(k-1) + "^" + QString::number(k) + ")";
        }

        if(k != pars.n_cols-1)
        {
            exp += " + ";
        }

    }

    exp += ")))";
    f.SetExpression(exp);

    return f;
}

arma::rowvec TrainLinear(const arma::mat& predictors,
                               const arma::rowvec& responses,
                               const arma::rowvec& weights,
                               double lambda,
                               const bool intercept)
{


  /*
   * We want to calculate the a_i coefficients of:
   * \sum_{i=0}^n (a_i * x_i^i)
   * In order to get the intercept value, we will add a row of ones.
   */

  // We store the number of rows and columns of the predictors.
  // Reminder: Armadillo stores the data transposed from how we think of it,
  //           that is, columns are actually rows (see: column major order).
  const size_t nCols = predictors.n_cols;

  arma::mat p = predictors;
  arma::rowvec r = responses;

  // Here we add the row of ones to the predictors.
  // The intercept is not penalized. Add an "all ones" row to design and set
  // intercept = false to get a penalized intercept.
  if (intercept)
  {
    p.insert_rows(0, arma::ones<arma::mat>(1, nCols));
  }

  if (weights.n_elem > 0)
  {
    p = p * diagmat(sqrt(weights));
    r = sqrt(weights) % responses;
  }

  // Convert to this form:
  // a * (X X^T) = y X^T.
  // Then we'll use Armadillo to solve it.
  // The total runtime of this should be O(d^2 N) + O(d^3) + O(dN).
  // (assuming the SVD is used to solve it)
  arma::mat cov = p * p.t() +
      lambda * arma::eye<arma::mat>(p.n_rows, p.n_rows);

  arma::rowvec parameters = arma::solve(cov, p * r.t());
  return parameters;
}

static MathFunction MapLinearRegression(std::vector<cTMap *> covariatemaps, cTMap * data)
{

    //get map list

    std::vector<cTMap *> covariatemapsnorm;
    std::vector<double> covariatemin;
    std::vector<double> covariatemax;


    if(covariatemaps.size() < 1)
    {
        LISEMS_ERROR("Can not run logistic regression without co-variates");
        throw 1;
    }


    for(int i = 0; i < covariatemaps.size(); i++)
    {
        std::cout << "covariate i " << data->nrCols() << " " << covariatemaps.at(i)->nrCols() << "  " <<data->nrRows() << "  " << covariatemaps.at(i)->nrRows() << std::endl;
        if((data->nrCols() == covariatemaps.at(i)->nrCols()) && (data->nrRows() == covariatemaps.at(i)->nrRows()))
        {
            double min = 0.0;
            double max = 0.0;
            cTMap * mnorm = NormalizeMap(covariatemaps.at(i),min,max);
            covariatemapsnorm.push_back(mnorm);
            covariatemin.push_back(min);
            covariatemax.push_back(max);
        }else
        {
            LISEMS_ERROR("Not all co-variates are of identical size to the observation data");
            for(int k = 0; k < covariatemapsnorm.size(); k++)
            {
                delete covariatemapsnorm.at(k);
            }
            throw 1;
        }
    }

    int n = 0;

    for(int r = 0; r < data->data.nr_rows();r++)
    {
        for(int c = 0; c < data->data.nr_cols();c++)
        {
            if(!pcr::isMV(data->data[r][c]))
            {
                bool ismv = false;
                for(int k = 0; k < covariatemapsnorm.size(); k++)
                {
                    if(pcr::isMV(covariatemapsnorm.at(k)->data[r][c]))
                    {
                        ismv = true;
                    }
                }
                if(!ismv)
                {
                    n++;
                }
            }
        }
    }

    //prepare arma matrix data for covariates and labels

    arma::mat regressors = arma::mat(covariatemapsnorm.size(),n);
    arma::rowvec responses = arma::rowvec(n);

    int ni = 0;
    for(int r = 0; r < data->data.nr_rows();r++)
    {
        for(int c = 0; c < data->data.nr_cols();c++)
        {
            if(!pcr::isMV(data->data[r][c]))
            {
                bool ismv = false;
                for(int k = 0; k < covariatemapsnorm.size(); k++)
                {
                    if(pcr::isMV(covariatemapsnorm.at(k)->data[r][c]))
                    {
                        ismv = true;
                    }
                }
                if(!ismv)
                {
                    for(int k = 0; k < covariatemapsnorm.size(); k++)
                    {
                        regressors(k,ni) = covariatemapsnorm.at(k)->data[r][c];
                    }
                    responses(ni) = data->data[r][c] ;

                    ni++;
                }
            }
        }
    }

    const double lambda = 0.0;
    const string optimizerType = "lbfgs";
    const double tolerance =  1e-10;
    const double stepSize = 0.01;
    const size_t batchSize =  64;
    const size_t maxIterations = (size_t) 10000;
    const double decisionBoundary = 0.5;


    arma::rowvec pars = TrainLinear(regressors, responses, arma::rowvec(),0.0,true);

    MathFunction f;
    QString exp = "";


    for(int k = 0; k < pars.n_cols; k++)
    {   if(k == 0)
        {
            exp += QString::number(pars(k));
        }else
        {
            exp += QString::number(pars(k)) + "* (x" + QString::number(k-1) + "^" + QString::number(k) + ")";
        }

        if(k != pars.n_cols-1)
        {
            exp += " + ";
        }

    }

    exp += "";
    f.SetExpression(exp);

    return f;
}

#ifndef GRIDDING_H
#define GRIDDING_H

#include "geo/raster/map.h"

#include "raster/rasterrandom.h"
#include <GsTL/kriging/kriging_weights.h>
#include <GsTL/kriging/SK_constraints.h>
#include <GsTL/kriging/OK_constraints.h>
#include <GsTL/kriging/KT_constraints.h>
#include <GsTL/kriging/linear_combination.h>
#include <GsTL/kriging/cokriging_weights.h>

#include <GsTL/cdf_estimator/gaussian_cdf_Kestimator.h>
#include <GsTL/cdf/gaussian_cdf.h>

#include <GsTL/geometry/covariance.h>

#include <GsTL/kriging/kriging_constraints.h>
#include <GsTL/kriging/kriging_combiner.h>

#include <iostream>
#include <math.h>
#include <vector>
#include <algorithm>

#include "mathfunction.h"
#include "optimization/leastsquares.h"

class Point{
public:
  typedef double coordinate_type;

  Point(){X=0;Y=0;};
  Point(int x, int y): X(x), Y(y) {};
  int& operator[](int i){
    if(i==1) return X;
    else     return Y;
  }

  const int& operator[](int i) const {
    if(i==1) return X;
    else     return Y;
  }

  int X;
  int Y;
};


inline double id(Point loc){
  return 1.0 ;}

inline double fx(Point loc){
  return loc.X;}

inline double fy(Point loc){
  return loc.Y;}

inline double fxy(Point loc){
  return loc.X * loc.Y;}

class functor{
public:
  functor() : fp(0) {}
  functor(double (*f)(Point)) : fp(f) {}

  inline double operator()(Point loc) const {
    return (*fp)(loc);}

private:
  double (*fp) (Point);
};




class Node{
public:
  typedef double property_type;
  typedef Point location_type;

  Node():pval_(0) {loc_.X=0; loc_.Y=0;};
  Node(int x, int y, double pval){loc_.X=x; loc_.Y=y;pval_=pval;};
  inline Point location() const { return loc_;};
  inline double& property_value(){return pval_;}
  inline const double& property_value() const {return pval_;}

private:
  Point loc_;
  double pval_;
};

inline std::ostream& operator << (std::ostream& os, Node& N){
  std::cout << N.location().X << " "
        << N.location().Y << " ";

  return os;
};


class neighborhood{
public:
  typedef std::vector<Node>::iterator iterator;
  typedef std::vector<Node>::const_iterator const_iterator;

  typedef Node value_type;

  void add_node(Node n){neigh_.push_back(n);};
  iterator begin(){return neigh_.begin();};
  iterator end(){return neigh_.end();};
  const_iterator begin() const {return neigh_.begin();};
  const_iterator end() const {return neigh_.end();};
  unsigned int size() const {return neigh_.size();}
  bool is_empty() const {return neigh_.empty();}

private:
  void find_neighbors(Point& u);

  std::vector<Node> neigh_;
};



class covariance{
public:

    double c0 = 1.0;
    inline covariance(double c)
    {
        c0 = c;

    }
  inline double operator()(const Point& P1,const Point& P2) const {
    double h=sqrt(pow(P1.X-P2.X,2)+pow(P1.Y-P2.Y,2));
    double l=c0;
    double a=0.75;
    if(h < 1e-30)
    {
        return 1.0;
    }
    return std::pow(1+h*h/(2*a*l),-a);
  }
};



class covarianceGaussian{
public:
    double c1 = 1.0;
    double c2 = 1.0;

    inline covarianceGaussian(double a, double b)
    {
        c1 = a;
        c2 = b*b;
    }

  inline double operator()(const Point& P1,const Point& P2) const {
    double h=pow(P1.X-P2.X,2)+pow(P1.Y-P2.Y,2);
    double l=50;
    double a=0.75;
    if(h < 1e-30)
    {
        return 1.0;
    }
    return c1 - c1 * (1.0-exp(-h/(c2)));
  }
};

inline MathFunction AS_SemiVarioGram(cTMap * data, float area, int max_points)
{

    int n_raster = data->nrCols() * data->nrRows();

    std::vector<float> variance;
    std::vector<float> dist;

    double dist_max = 0.0;

    int n_skip = std::max(1,(int)(n_raster/(float)(max_points)));
    if(max_points == 0)
    {
        n_skip = 0;

    }

    int n_done = 0;

    //we go through the raster interleaved, to get a better representation of the total dataset
    for(int i = 0; i < n_skip; i++)
    {
        if(n_done >= max_points)
        {
            break;
        }
        for(int j = i; j < n_raster; j+= n_skip)
        {
            //get r and c from j

            if(n_done >= max_points)
            {
                break;
            }
            int c = j / data->nrRows();
            int r = j - c * data->nrRows();

            if(r < data->nrRows() && c< data->nrCols())
            {
                if(!pcr::isMV(data->data[r][c]))
                {
                    int cellsx = std::fabs(std::fabs(area)/data->cellSizeX());
                    int cellsy = std::fabs(std::fabs(area)/data->cellSizeY());
                    int rbegin = std::max(r-cellsy,0);
                    int rend = std::min(r+cellsy,data->nrRows()-1);
                    int cbegin = std::max(c-cellsx,0);
                    int cend = std::min(c+cellsx,data->nrCols()-1);

                    for(int rn = rbegin; rn < rend + 1; rn++)
                    {
                        for(int cn = cbegin; cn < cend + 1; cn++)
                        {
                            if(!pcr::isMV(data->data[rn][cn]))
                            {
                                if(!(rn == r && cn == c))
                                {
                                    n_done ++;
                                    float var = data->data[r][c] - data->data[rn][cn];
                                    float disx = (rn-r)*data->cellSizeY();
                                    float disy = (cn-c)*data->cellSizeX();

                                    double dist_this = std::sqrt(disx*disx + disy*disy);

                                    dist_max = std::max(dist_max,dist_this);
                                    variance.push_back(var*var);
                                    dist.push_back(dist_this);
                                }
                            }
                        }
                    }
                }

            }else {
                break;
            }
        }
    }

    if(n_done == 0)
    {
        LISEMS_ERROR("Can not do semi-variogram based on 0 points");
        throw 1;
    }
    //put the data into average bins
    //a constant number of 25 bins for now
    int n_bins = 25;

    std::vector<float> bdistances;
    std::vector<float> bvariances;
    std::vector<int> bn;


    for(int i = 0; i < n_bins; i++)
    {
        bdistances.push_back(((float)(i) + 0.5)*dist_max/((float)(n_bins)));
        bvariances.push_back(0.0);
        bn.push_back(0.0);
    }

    for(int i = 0; i < dist.size(); i++)
    {
        double dist_this = dist.at(i);

        int bin = std::max(0,std::min(n_bins-1,n_bins-1 - (int)(dist_max/dist_this)));
        bvariances.at(bin) += variance.at(i);
        bn.at(bin) += 1.0;
    }

    QList<double> x;
    QList<double> y;
    for(int i = 0; i < n_bins; i++)
    {
        if(bn.at(i) > 0.0)
        {
            bvariances.at(i) = bvariances.at(i)/bn.at(i);
            std::cout << "bin " << i << " " <<bdistances.at(i)<< " "<<bvariances.at(i) << std::endl;
            x.append(bdistances.at(i));
            y.append(bvariances.at(i));
        }
    }

    //now fit the data to a gaussian model

    QList<double> pars = OptimizeGaussianSemiVariogram(x,y);

    MathFunction f;
    QString exp = "";
    exp += QString::number(pars.at(0));
    exp += "*(1.0 - exp(-x0*x0/(";
    exp += QString::number(pars.at(1)*pars.at(1));
    exp += ")))";
    exp += "";
    f.SetExpression(exp);

    return f;
}

inline cTMap * AS_KrigingGaussianCovariance(cTMap * data, float area, double c0, double c1)
{

    cTMap *map = data->GetCopy();



    int nwarn = 0;
    bool warn = true;

    int n2 = 0;
    for(int r = 0; r < data->nrRows(); r++)
    {
        std::cout << "progress r: " << r << + "/" <<  data->nrRows() << std::endl;
        for(int c = 0; c < data->nrCols(); c++)
        {
            if(pcr::isMV(data->data[r][c]))
            {
                //do kriging

                neighborhood voisin;
                //get all the points into a neighborhood

                int n = 0;
                int cellsx = std::fabs(std::fabs(area)/data->cellSizeX());
                int cellsy = std::fabs(std::fabs(area)/data->cellSizeY());
                int rbegin = std::max(r-cellsy,0);
                int rend = std::min(r+cellsy,map->nrRows()-1);
                int cbegin = std::max(c-cellsx,0);
                int cend = std::min(c+cellsx,map->nrCols()-1);

                for(int rn = rbegin; rn < rend + 1; rn++)
                {
                    for(int cn = cbegin; cn < cend + 1; cn++)
                    {
                        if(!pcr::isMV(data->data[rn][cn]))
                        {
                            if(!(rn == r && cn == c))
                            {
                                n++;
                                voisin.add_node(Node((float)(cn),(float)(rn),data->data[rn][cn]));

                            }
                        }
                    }
                }

                if(n > 1)
                {

                    typedef TNT_lib<double> TNT;
                    typedef matrix_lib_traits<TNT>::Vector TNTvector;

                    covarianceGaussian covar(c0,c1);

                    Point center((float)(c),(float)(r));

                    TNTvector OK_weights;
                      OK_constraints OK = OK_constraints();
                      double ok_variance;

                      std::vector<functor> functArray(2);
                        functor f0(id);
                        functor f1(fx);
                        functor f2(fy);
                        functor f3(fxy);
                        functArray[0] = f0;
                        functArray[1] = f1;
                        functArray.push_back(f2);
                        functArray.push_back(f3);

                        typedef std::vector<functor>::iterator functIter;
                        KT_constraints<functIter> KT(functArray.begin(),functArray.end());


                    typedef Kriging_combiner<TNTvector::iterator, neighborhood> KCombiner;
                      typedef Kriging_combiner_impl<TNTvector::iterator, neighborhood> KCombiner_base;

                      KCombiner ok_combiner( new KCombiner_base() );
                      KCombiner kt_combiner( ok_combiner );


                      int status = kriging_weights(OK_weights, ok_variance,
                                     center, voisin,
                                     covar, KT);
                      if(status > 0)
                      {
                          nwarn ++;
                          if(nwarn > 10)
                          {
                              if(warn)
                              {
                                 LISEM_WARNING("Too many errors during kriging, stopping further output, continueing operation");
                                  warn = false;
                              }

                          }else {
                              LISEMS_WARNING("Error encountered during krigin at r:" + QString::number(r) + " c:" + QString::number(c) +  "  error "+ status);
                             }
                      }

                      double OK_mean = kt_combiner(OK_weights.begin(), OK_weights.end(), voisin);

                      //std::cout << "val " << OK_mean << " " << r << " " << c << " " << n <<  std::endl;
                      n2 ++;
                      map->data[r][c] = OK_mean;

                }



            }
        }
    }


  return map;



}

inline cTMap * AS_Kriging(cTMap * data, float area)
{
      cTMap *map = data->GetCopy();

      int nwarn = 0;
      bool warn = true;

      int n2 = 0;
      for(int r = 0; r < data->nrRows(); r++)
      {
          std::cout << "progress r: " << r << + "/" <<  data->nrRows() << std::endl;
          for(int c = 0; c < data->nrCols(); c++)
          {
              if(pcr::isMV(data->data[r][c]))
              {
                  //do kriging

                  neighborhood voisin;
                  //get all the points into a neighborhood

                  int n = 0;
                  int cellsx = std::fabs(std::fabs(area)/data->cellSizeX());
                  int cellsy = std::fabs(std::fabs(area)/data->cellSizeY());
                  int rbegin = std::max(r-cellsy,0);
                  int rend = std::min(r+cellsy,map->nrRows()-1);
                  int cbegin = std::max(c-cellsx,0);
                  int cend = std::min(c+cellsx,map->nrCols()-1);

                  for(int rn = rbegin; rn < rend + 1; rn++)
                  {
                      for(int cn = cbegin; cn < cend + 1; cn++)
                      {
                          if(!pcr::isMV(data->data[rn][cn]))
                          {
                              if(!(rn == r && cn == c))
                              {
                                  n++;
                                  voisin.add_node(Node((float)(cn),(float)(rn),data->data[rn][cn]));

                              }
                          }
                      }
                  }

                  if(n > 1)
                  {

                      typedef TNT_lib<double> TNT;
                      typedef matrix_lib_traits<TNT>::Vector TNTvector;

                      covariance covar(area);

                      Point center((float)(c),(float)(r));

                      TNTvector OK_weights;
                        OK_constraints OK = OK_constraints();
                        double ok_variance;

                        std::vector<functor> functArray(2);
                          functor f0(id);
                          functor f1(fx);
                          functor f2(fy);
                          functor f3(fxy);
                          functArray[0] = f0;
                          functArray[1] = f1;
                          functArray.push_back(f2);
                          functArray.push_back(f3);

                          typedef std::vector<functor>::iterator functIter;
                          KT_constraints<functIter> KT(functArray.begin(),functArray.end());


                      typedef Kriging_combiner<TNTvector::iterator, neighborhood> KCombiner;
                        typedef Kriging_combiner_impl<TNTvector::iterator, neighborhood> KCombiner_base;

                        KCombiner ok_combiner( new KCombiner_base() );
                        KCombiner kt_combiner( ok_combiner );


                        int status = kriging_weights(OK_weights, ok_variance,
                                       center, voisin,
                                       covar, KT);
                        if(status > 0)
                        {
                            nwarn ++;
                            if(nwarn > 10)
                            {
                                if(warn)
                                {
                                   LISEM_WARNING("Too many errors during kriging, stopping further output, continueing operation");
                                    warn = false;
                                }

                            }else {
                                LISEMS_WARNING("Error encountered during krigin at r:" + QString::number(r) + " c:" + QString::number(c) +  "  error "+ status);
                               }
                        }

                        double OK_mean = kt_combiner(OK_weights.begin(), OK_weights.end(), voisin);

                        //std::cout << "val " << OK_mean << " " << r << " " << c << " " << n <<  std::endl;
                        n2 ++;
                        map->data[r][c] = OK_mean;

                  }



              }
          }
      }


    return map;


}

inline cTMap * KrigingUpSample(cTMap * data,float scale, QString type)
{


    return nullptr;


}




#endif // GRIDDING_H

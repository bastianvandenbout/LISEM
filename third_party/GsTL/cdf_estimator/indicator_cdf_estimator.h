/* GsTL: the Geostatistics Template Library
 * 
 * Author: Nicolas Remy
 * Copyright (c) 2000 The Board of Trustees of the Leland Stanford Junior University
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 * 
 *   1.Redistributions of source code must retain the above copyright notice, this 
 *     list of conditions and the following disclaimer. 
 *   2.Redistributions in binary form must reproduce the above copyright notice, this 
 *     list of conditions and the following disclaimer in the documentation and/or other
 *     materials provided with the distribution. 
 *   3.The name of the author may not be used to endorse or promote products derived 
 *     from this software without specific prior written permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

 

#ifndef __GSTL_INDICATOR_CDF_ESTIMATOR_H__
#define __GSTL_INDICATOR_CDF_ESTIMATOR_H__
#ifdef __GNUC__
#pragma interface
#endif

#include <GsTL/cdf/cdf_basics.h>
#include <GsTL/kriging.h>
#include <GsTL/math/indicator_coding.h>
#include <GsTL/matrix_library/matrix_lib_traits.h>
#include <GsTL/matrix_library/gstl_tnt_lib.h>
#include <GsTL/univariate_stats/build_cdf.h>
#include <GsTL/utils/gstl_error_messages.h>

#include <iterator>
#include <vector>





/** This class models both a Single Variable Cdf Estimator and a Multiple 
 * Variables Cdf Estimator. It estimates a non-parametric cdf using indicator
 * kriging. The conditioning data are turned on-the-fly into indicator 
 * information (while a Const_indicator_cdf_estimator assumes that each variable
 * supplied is the indicator coding of the conditioning data). 
 * Indicator_cdf_estimator allows to supply multiple neighborhoods to estimate the
 * probability associated with each threshold of the cdf. Each neighborhood must
 * refer to the same property value, which will be coded into indicator value on
 * the fly. The function used to do the indicator coding can be supplied to the
 * constructor. It must be a model of concept BinaryPredicate (ie it must be
 * Assignable and have a function that given 2 input parameters returns true or 
 * false).  
 * The default indicator function is I(z,zk) = 1 if z<= zk, 0 otherwise.
 *
 */

template
<
  class InputIterator,
  class BinaryPredicate=Indicator_function<double>,
  class MatrixLibrary=GSTL_TNT_lib
 >
class Indicator_cdf_estimator{
  
  public:
  
  template< class InputIterator2 >
  Indicator_cdf_estimator( InputIterator first_covar, InputIterator last_covar,
			   InputIterator2 first_marginal, 
			   InputIterator2 last_marginal ) 
  : covariances_( std::distance(first_covar, last_covar) ),
    indicator_( BinaryPredicate() ),
    marginal_probas_( first_marginal, last_marginal ),
    weights_(50) {
    
      // make sure at least one covariance function is supplied
      gstl_assert(first_covar!=last_covar);
      // copy the covariances into internal storage.
      std::copy( first_covar, last_covar, covariances_.begin() );
    }


  template< class InputIterator2 >
  Indicator_cdf_estimator(
			  InputIterator first_covar, InputIterator last_covar,
			  InputIterator2 first_marginal, 
			  InputIterator2 last_marginal,
			  BinaryPredicate indicator_function
			  ) 
  : covariances_( std::distance(first_covar, last_covar) ),
    indicator_( indicator_function ),
    marginal_probas_( first_marginal, last_marginal ),
    weights_(50) {
    
      // make sure at least one covariance function is supplied
      gstl_assert(first_covar!=last_covar);
      // copy the covariances into internal storage.
      std::copy( first_covar, last_covar, covariances_.begin() );
    }
		
  
  Indicator_cdf_estimator(const Indicator_cdf_estimator& rhs) 
  : covariances_(rhs.covariances_),
    indicator_( rhs.indicator_function ),
    marginal_probas_( rhs.marginal_probas_ ),
    weights_(rhs.weights_.size()) {
  }

  Indicator_cdf_estimator& operator=(const Indicator_cdf_estimator& rhs) {
    covariances_  = rhs.covariances_;
    indicator_ = rhs.indicator_function ;
    marginal_probas_ = rhs.marginal_probas_ ;
    weights_.reserve(rhs.weights_.size());
  }
  
  /** Two definitions of operator() are provided. One implements a Single Variable
   * Cdf Estimator, the second one implements a Multiple Variables Cdf Estimator.
   */

  /** In this first implementation, the same neighborhood is used to compute the 
   * probability of exceeding each threshold. The data values are coded into binary
   * variables 0 or 1. 
   */
  template
  <
    class Geovalue_,
    class Neighborhood,
    class NonParametricCdf
  >
  inline int operator()(
			const Geovalue_& u,
			const Neighborhood& neighbors,
			NonParametricCdf& ccdf
			) const;


  /** This version allows to use different types of neighborhoods for each 
   * threshold. If the number of neighborhoods supplied is lesser than the 
   * number of thresholds, the last neighborhood is used for all the remaining
   * thresholds.
   *
   * @param [first_neigh,last_neigh) is a range of Neighborhoods.
   */
  template
  <
    class Geovalue_,
    class InputIterator2,
    class NonParametricCdf
  >
  inline int operator()(
			const Geovalue_& u,
			InputIterator2 first_neigh, InputIterator2 last_neigh,
			NonParametricCdf& ccdf
			) const ;



 //private:
 protected :

  typedef typename std::iterator_traits<InputIterator>::value_type Covariance;
  typedef typename std::vector<Covariance>::const_iterator Covariance_iterator;

  std::vector<Covariance> covariances_;
  BinaryPredicate indicator_;
  std::vector<double> marginal_probas_;

  /* This is here to avoid allocating a vector of weights each time
   * an indicator kriging is used. When an indicator kriging estimator is
   * copied, the content of weights is not copied. 
   * weights_ can contain 50 weights by default (it is automatically resized
   * if more weights are needed).
   */
  mutable std::vector<double> weights_;

  
}; // end of class Indicator_cdf_estimator.








template
<
  class InputIterator,
  class BinaryPredicate,
  class MatrixLibrary
>
template
<
  class Geovalue_,
  class Neighborhood,
  class NonParametricCdf
>
inline int 
Indicator_cdf_estimator<
                        InputIterator,
                        BinaryPredicate,
                        MatrixLibrary
                       >::
operator()(
	   const Geovalue_& u,
	   const Neighborhood& neighbors,
	   NonParametricCdf& ccdf
	   ) const {


  SK_constraints Kconstraints;

  // Find the kriging weights, using the first covariance function (if other
  // covariances were supplied, they are ignored).
  int status = kriging_weights<MatrixLibrary>( weights_,
					       u.location(), neighbors,
					       covariances_[0], Kconstraints );
  if( status != 0 ) return status;

  // Loop on all the thresholds and compute the corresponding probability
  // always using the same kriging weights. 
  typename NonParametricCdf::p_iterator p_it=ccdf.p_begin();
  
  int ithresh = 0;
  for(typename NonParametricCdf::z_iterator z_it=ccdf.z_begin();
      z_it != ccdf.z_end(); ++z_it, ++p_it, ++ithresh ) {

    // Combine the kriging weights and the indicator values of the conditioning
    // data.
    typedef typename Neighborhood::const_iterator const_iterator;
    
    int ind = 0;
    double prob = 0;
    double weight_sum = 0;
    for(const_iterator neigh_it = neighbors.begin(); 
	neigh_it != neighbors.end() ; ++neigh_it, ++ind ) {
      gstl_assert( ind < (int)weights_.size() );
      prob += 
	double(indicator_( neigh_it->property_value(), *z_it )) * weights_[ind];
      weight_sum += weights_[ind];
    }
    *p_it = prob + (1.0 - weight_sum)*marginal_probas_[ithresh];
  }

  // Now ccdf has been estimated, correct order relation poblems
  bool ok = ccdf.make_valid();
  
  return !ok ;
}







template
<
  class InputIterator,
  class BinaryPredicate,
  class MatrixLibrary
>
template
<
  class Geovalue_,
  class InputIterator2,
  class NonParametricCdf
>
inline int 
Indicator_cdf_estimator<
                        InputIterator,
                        BinaryPredicate,
                        MatrixLibrary
                       >::
operator()(
	   const Geovalue_& u,
	   InputIterator2 first_neigh, InputIterator2 last_neigh,
	   NonParametricCdf& ccdf
	   ) const {
			  
  SK_constraints Kconstraints;

  Covariance_iterator current_covar = covariances_.begin();
  Covariance_iterator end_covar = covariances_.end();

  int current_neigh_id = -1;
  int neighborhoods_nb = std::distance(first_neigh, last_neigh);

  /* Iterate on all the thresholds defining the cdf
   * For each threshold, we either use a new set of conditioning data or
   * use the same ones as for the previous threshold. 
   * If we use different ones, we need to recompute the kriging weights.
   * If we use the same conditioning data, the kriging weights don't have
   * to be recomputed.
  */
  typename NonParametricCdf::p_iterator p_it=ccdf.p_begin();

  int ithresh=0;
  for(typename NonParametricCdf::z_iterator z_it=ccdf.z_begin();
      z_it != ccdf.z_end(); ++z_it, ++p_it, ++ithresh) {
    
    if(current_neigh_id+1 != neighborhoods_nb) {
      /* we use a different set of conditioning data for this threshold
       * hence we recompute the kriging weights, using another covariance
       * function, if enough were supplied.
       */
      ++current_neigh_id;
      if( current_neigh_id != 0 )
        ++first_neigh;

      int status = 
      	kriging_weights<MatrixLibrary>( weights_,
			                u.location(), *first_neigh,
					*current_covar, Kconstraints );
      
      // If the kriging system could not be solved, use the marginal
      if(status!=0) {
      	*p_it = marginal_probas_[ithresh];
        continue;
      }
      
      // If there are covariances left, select the next one
      if(current_covar+1 != end_covar)
      	current_covar++;
      
    }

    // Combine the kriging weights and the indicator values of the conditioning
    // data.
    typedef typename std::iterator_traits<InputIterator2>::value_type NeighborhoodType;
    typedef typename NeighborhoodType::const_iterator const_iterator;
    
    int ind = 0;
    double prob = 0;
    double weights_sum = 0;
    for( const_iterator neigh_it = first_neigh->begin(); 
      	 neigh_it != first_neigh->end() ; ++neigh_it, ++ind ) {
      gstl_assert( ind < (int)weights_.size() );
      prob +=
      	double(indicator_( neigh_it->property_value(), *z_it )) * weights_[ind];
      weights_sum += weights_[ind];
    }
    *p_it = prob + (1 - weights_sum)*marginal_probas_[ithresh];

  }

  // Now ccdf has been estimated, correct order relation poblems
  bool ok = ccdf.make_valid();

  
  return !ok ;
}





//=========================================================

/** Helper function. This function creates an instance of 
 * Const_indicator_cdf_estimator. 
 */
/*
template
<
  class InputIterator,
  class KrigingConstraint,
  class MatrixLibrary
>
Indicator_cdf_estimator<
                        InputIterator,
                        KrigingConstraint,
                        MatrixLibrary
                       >
Indicator_cdf_estimator(
			InputIterator first_covar, InputIterator last_covar,
			KrigingConstraint& Kconstraints 
			)
{
  return Indicator_cdf_estimator<
             InputIterator,
             KrigingConstraint,
             MatrixLibrary
           >(first_covar, last_covar, Kconstraints) ;
} 



//=======================
// This version of the helper function uses TNT_lib by default.
template
<
  class InputIterator,
  class KrigingConstraint,
>
Indicator_cdf_estimator<
                        InputIterator,
                        KrigingConstraint,
                        GSTL_TNT_lib
                       >
Indicator_cdf_estimator(
			InputIterator first_covar, InputIterator last_covar,
			KrigingConstraint& Kconstraints 
			)
{
  return Indicator_cdf_estimator<
             InputIterator,
             KrigingConstraint,
             GSTL_TNT_lib
           >(first_covar, last_covar, Kconstraints) ;
} 
*/
#endif

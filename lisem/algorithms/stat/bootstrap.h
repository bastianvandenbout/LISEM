#ifndef BOOT_HPP
#define BOOT_HPP

// Includes
#include <complex>
#include <vector>
#include <random>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iostream>
#include <memory>

/// std::vector
template <typename T>
using vec = std::vector<T>;

/// Matrix type as #vec of #vec
template <typename T>
using mat = vec<vec<T>>;

/// std::complex<double>
typedef std::complex<double> cdouble;

/// Class used for bootstrapping data ensembles of several variables
/**
 *  \note that this class does not provide any type of checks, e.g.,
 *  if all #vec in the input #mat have the same length. Thus you have to know
 *  what you are doing.
 */
template <typename T>
class Bootstrapper {
//---------Members--------------
  /// The number of to be generated bootstrap samples.
  const size_t NSamples;
  /// The number of bins contained in each individual bootstrap sample.
  const size_t NSize;
  /// The number of configurations contained in one bin.
  const size_t NBinSize;
  /// The number of configurations in the ensemble. This is the first dimension of the input array.
  const size_t NConfigs;
  /// The number of variables in the ensemble. Second dimension of the input array.
  const size_t NVars;
  /// The number of bins given by #NConfigs/#NBinSize.
  /** In case mod(#NConfigs, #NBinSize) != 0, the remainder is skipped at the
   *  beginning of the input data array.
   */
  const size_t NBins;
  /// The binned data of size #NVars x #NBins.
  /** Note that this is not the input data.*/
  const mat<T> data;
  /// Random number generator pointer.
  /** Random numbers of type size_t are generated in the range from 0 to
    * #NBins -1.
    * Uses the `mt19937` engine and a uniform distribution.
    */
  std::shared_ptr< std::function<size_t()> > dist;
  /// The bootstrap indicies of size #NSamples x #NSize.
  const mat<size_t> indices;

//---------Private member functions--------------
  /// Compute the mean of a vector.
  /** Averages over all entries of the vector and divides by the length.
   * \param vals Input #vec
   */
  const T    getMean(const vec<T> &vals) const;
  /// Compute the mean of a matrix.
  /** Iterates the zeroth dimension of the matrix and calls
   * #getMean() for the contains vectors. In other words,
   * keeps the zeroth dimension and averages the first.
   * \param vals Input #mat
   */
  const vec<T> getMean(const mat<T> &vals) const;


//---------Public access--------------
public:
//---------Member access--------------
  /// Returns #NSamples.
  size_t getNSamples() const {return NSamples;};
  /// Returns #NSize.
  size_t getNSize()    const {return NSize;   };
  /// Returns #NBinSize.
  size_t getNBinSize() const {return NBinSize;};
  /// Returns #NConfigs.
  size_t getNConfigs() const {return NConfigs; };
  /// Returns #NVars.
  size_t getNVars()    const {return NVars;   };
  /// Returns #NBins.
  size_t getNBins()    const {return NBins;   };
  /// Returns #data.
  /** \note This array is the binned data --- and not the input data. */
  const mat<T>      &getData()    const {return data;   };
  /// Returns #indices.
  const mat<size_t> &getIndices() const {return indices;};

//---------Public member functions--------------
  /// Returns the mean of the #data.
  /** \note This mean is also equal to the mean of the input data modulo the
   *  binning cutoff.
   */
  const vec<T> getMean() const {return getMean(data);};
  /// Compute the bootstrap samples of size #NVars x #NSamples.
  /** This routines uses #indices to reshape #data.
   *  The averaged out dimension is #NSize.
   *  \note
   *  This is the most expensive computation. The output array is not stored
   *  within this class. Make sure, if you want to use it, to store it elsewhere.
   */
  const mat<T> getSamples() const;
  /// Computes the covariance matrix form the bootstrap samples.
  /** \note
   * This function also calls #getSamples() in case you do not specify the
   * covariance matrix. Thus, in case you are interested in the samples as well,
   * better call #getSamples() and feed it to this method.
   */
  const mat<T> getCov() const;
  /// Computes the covariance matrix for given bootstrap samples.
  /** \param samples Bootstrap samples computed by #getSamples().*/
  const mat<T> getCov(const mat<T> & samples) const;

//---------Constructors--------------
  /// Empty constructor (not available).
  Bootstrapper() = delete;
  /// Destructor.
  ~Bootstrapper() = default;
  /// List constructor (default constructor)
  /** Constructs the class from data input and bootstrap parameters
   * \param Indata mat containing the ensemble dara of shape #NVars x #NConfigs
   * \param NSamples the number of bootstrap samples after binning and
   *        averaging, e.g., the resulting array after bootstrapping is of size
   *        #NVars x #NSamples
   * \param NSize the size of individual bootstrap samples. This is the dimension
   *        which will be averaged out.
   * \param NBinSize size of the bins will be grouped before storing.
   *        Before bootstrapping, the data will be reshaped to size
   *        #Nvars x #NBins, where #NBins = #NConfigs / #NBinSize.
   *        The remainder is dropped at the beginning of the array.
   *
   * \note
   * This initialisation provides no checks, e.g., shape of #Input data,
   * size of bins or what so ever. Make sure you input makes sense.
   */
  Bootstrapper(
    const mat<T> &Indata,
    const size_t NSamples,
    const size_t NSize,
    const size_t NBinSize
  );
  /// List constructor (from bootstrap indices)
  /** Constructs the class from data input bin size and indices
   * \param Indata mat containing the ensemble dara of shape #NVars x #NConfigs
   * \param inIndices a set of bootstrap indices for given data of size
            #NSamples x #NSize. #NSamples is the number of bootstrap
            samples after binning and averaging. #NSize is the size of
            individual bootstrap samples. This array is used to compute the
            bootstrap samples of size #NVars x #NSamples.
   * \param NBinSize size of the bins will be grouped before storing.
   *        Before bootstrapping, the data will be reshaped to size
   *        #Nvars x #NBins, where #NBins = #NConfigs / #NBinSize.
   *        The remainder is dropped at the beginning of the array.
   *
   * \note
   * This initialisation provides no checks, e.g., shape of #Input data,
   * size of bins or what so ever. Make sure you input makes sense.
   */
  Bootstrapper(
    const mat<T> &Indata,
    const mat<size_t> &inIndices,
    const size_t NBinSize
  );
  /// Copy constructor.
  Bootstrapper(const Bootstrapper &boot) = default;
  /// Move constructor.
  Bootstrapper & operator=(const Bootstrapper &boot) = default;
  /// Move assignment operator.
  Bootstrapper(Bootstrapper &&boot) = default;
};

#endif /* BOOT_HPP */

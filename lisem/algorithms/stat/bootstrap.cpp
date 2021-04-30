#include "bootstrap.h"

//---------------------------------------------
// List constructor
template<typename T>
Bootstrapper<T>::Bootstrapper(
  const mat<T> &Indata,
  const size_t NSamples,
  const size_t NSize,
  const size_t NBinSize
) :
  NSamples(NSamples),
  NSize(NSize),
  NBinSize(NBinSize),
  NConfigs(Indata[0].size()),
  NVars(Indata.size()),
  NBins(NConfigs/NBinSize),
  data([&](){
    // Bin data according to shape NVars x NBins
    if(NBins != 1){
      mat<T> binnedData(NVars, vec<T>(NBins, 0));
      std::transform( // Iterate NVars
        Indata.begin(),
        Indata.end(),
        binnedData.begin(),
        [&](const vec<T> &rowData){ // Output is binned vector of size NBins
          vec<T> binnedRow(NBins, 0);
          size_t mod(NConfigs%NBinSize); // Initial offset
          for(size_t nb=0; nb<NBins; nb++){ // Execute binning
            for(size_t nc=0; nc<NBinSize; nc++){ // Average bins
              binnedRow[nb] += rowData[mod+nb*NBinSize+nc]/static_cast<T>(NBinSize);
            };
          };
          return binnedRow;
        }
      );
      return binnedData;
    } else { // Return just data if no bins.
      return Indata;
    };
  }()),
  dist([&](){
    // Start engine with random seed
    std::random_device seed;
    std::mt19937 engine(seed());
    // Get distribution in range of number of bins
    std::uniform_int_distribution<size_t> distribution(0, NBins-1);
    // Bind distribution to engine
    return std::make_shared< std::function<size_t()> >(
      std::bind( distribution, engine )
    );
  }()),
  indices([&](){
    // Set indicies of matrix using random uniform distribution
    mat<size_t> temp(NSamples, vec<size_t>(NSize, 0));
    std::for_each(
      temp.begin(),
      temp.end(),
      [&](vec<size_t> &rowIndices){
        std::for_each(
          rowIndices.begin(),
          rowIndices.end(),
          [&](size_t &val){val=(*dist)();}
        );
      }
    );
    return temp;
  }())
{}

//---------------------------------------------
// List constructor from indices
template<typename T>
Bootstrapper<T>::Bootstrapper(
    const mat<T> &Indata,
    const mat<size_t> &inIndices,
    const size_t NBinSize
) :
  NSamples(inIndices.size()),
  NSize(inIndices[0].size()),
  NBinSize(NBinSize),
  NConfigs(Indata[0].size()),
  NVars(Indata.size()),
  NBins(NConfigs/NBinSize),
  data([&](){
    // Bin data according to shape NVars x NBins
    if(NBins != 1){
      mat<T> binnedData(NVars, vec<T>(NBins, 0));
      std::transform( // Iterate NVars
        Indata.begin(),
        Indata.end(),
        binnedData.begin(),
        [&](const vec<T> &rowData){ // Output is binned vector of size NBins
          vec<T> binnedRow(NBins, 0);
          size_t mod(NConfigs%NBinSize); // Initial offset
          for(size_t nb=0; nb<NBins; nb++){ // Execute binning
            for(size_t nc=0; nc<NBinSize; nc++){ // Average bins
              binnedRow[nb] += rowData[mod+nb*NBinSize+nc]/static_cast<T>(NBinSize);
            };
          };
          return binnedRow;
        }
      );
      return binnedData;
    } else { // Return just data if no bins.
      return Indata;
    };
  }()),
  dist([&](){
    // Start engine with random seed
    std::random_device seed;
    std::mt19937 engine(seed());
    // Get distribution in range of number of bins
    std::uniform_int_distribution<size_t> distribution(0, NBins-1);
    // Bind distribution to engine
    return std::make_shared< std::function<size_t()> >(
      std::bind( distribution, engine )
    );
  }()),
  indices(inIndices)
{}


//---------------------------------------------
// getMean
template<typename T>
const inline vec<T> Bootstrapper<T>::getMean(const mat<T> &vals) const {
  vec<T> meanVec(vals.size());
  std::transform( // Iterate over variables
    vals.begin(),
    vals.end(),
    meanVec.begin(),
    [&](const vec<T> &rowData){ // And return mean of the rows
      return this->getMean(rowData);
    }
  );
  return meanVec;
}


//---------------------------------------------
// getMean
template<typename T>
const inline T Bootstrapper<T>::getMean(const vec<T> &vals) const {
  return std::accumulate(
    vals.begin(),
    vals.end(),
    T(0)
  )/static_cast<T>(vals.size());
}


//---------------------------------------------
// getSamples
template<typename T>
const mat<T> Bootstrapper<T>::getSamples() const {
  // declare temporary variables
  mat<T> VarSampleMat(NVars, vec<T>(NSamples, 0));
  mat<T> SampleSizeMat(NSamples, vec<T>(NSize, 0));
  vec<T> SizeVec(NSize, 0);

  std::transform( // iterate variables
    data.begin(),
    data.end(),
    VarSampleMat.begin(),
    [&](const vec<T> &dataConfigs) ->vec<T> { // outputs are  averaged vecs of size NSample

      std::transform( // iterate samples
        indices.begin(),
        indices.end(),
        SampleSizeMat.begin(),
        [&](const vec<size_t> &indexRow) ->vec<T>& { // outputs are random vecs of size NSize

            std::transform(
              indexRow.begin(),
              indexRow.end(),
              SizeVec.begin(),
              [&](const size_t index) ->const T& {  // Set entry data[index]
                return dataConfigs[index];
              }
            );
            return SizeVec;

        }
      );
      return getMean(SampleSizeMat); // average over size to get Sample vecs
    }
  );
  return VarSampleMat;
}


//---------------------------------------------
// getCov
template<typename T>
const mat<T> Bootstrapper<T>::getCov() const {
  return getCov(getSamples()); // call method which takes input
}


//---------------------------------------------
// getCov
template<typename T>
const mat<T> Bootstrapper<T>::getCov(const mat<T> &samples) const {
  mat<T> cov(NVars, vec<T>(NVars, 0));
  const T zero(0);
  const T NSm1(NSamples-1);
  const vec<T> muVec(getMean(samples));

  std::transform(  // iterate NVars (row)
    samples.begin(),
    samples.end(),
    muVec.begin(),
    cov.begin(),
    [&](const vec<T> &sampleRow, const T muRow){  // output is vec of size NVars (row var)
      vec<T> covRow(NVars, 0);

      std::transform(  // iterate NVars (cols)
        samples.begin(),
        samples.end(),
        muVec.begin(),
        covRow.begin(),
        [&](const vec<T> &sampleCol, const T muCol){  // output is entry of covariance matrix

          return std::inner_product( // iterate samples
            sampleRow.begin(),
            sampleRow.end(),
            sampleCol.begin(),
            zero,
            std::plus<>(),
            [&](const T &elRow, const T &elCol){ // return covariance el over NSamples
              return (elCol - muCol)*(elRow - muRow)/NSm1;
            }
          );

        }
      );
      return covRow;
    }
  );

  return cov;
}

//---------------------------------------------
// getCov for complex overload
template<>
const mat<cdouble> Bootstrapper<cdouble>::getCov(const mat<cdouble> &samples) const {
  mat<cdouble> cov(NVars, vec<cdouble>(NVars, 0));
  const cdouble zero(0);
  const cdouble NSm1(NSamples-1);
  const vec<cdouble> muVec(getMean(samples));

  std::transform(  // iterate NVars (row)
    samples.begin(),
    samples.end(),
    muVec.begin(),
    cov.begin(),
    [&](const vec<cdouble> &sampleRow, const cdouble &muRow){  // output is vec of size NVars (row var)
      vec<cdouble> covRow(NVars, 0);

      std::transform(  // iterate NVars (cols)
        samples.begin(),
        samples.end(),
        muVec.begin(),
        covRow.begin(),
        [&](const vec<cdouble> &sampleCol, const cdouble &muCol){  // output is entry of covariance matrix

          return std::inner_product( // iterate samples
            sampleRow.begin(),
            sampleRow.end(),
            sampleCol.begin(),
            zero,
            std::plus<>(),
            [&](const cdouble &elRow, const cdouble &elCol){ // return covariance el over NSamples
              return std::conj(elCol - muCol)*(elRow - muRow)/NSm1;
            }
          );

        }
      );
      return covRow;
    }
  );

  return cov;
}

//---------------------------------------------
// template instantiations
template class Bootstrapper<double>;
template class Bootstrapper<cdouble>;

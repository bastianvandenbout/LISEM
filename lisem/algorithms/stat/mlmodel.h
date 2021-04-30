#pragma once

#include "gdal.h"
#include "ogr_spatialref.h"
#include "ogr_api.h"
#include "gdal_utils.h"
#include "geo/shapes/shapefile.h"
#include "geo/shapes/shapelayer.h"
#include <QString>
#include "error.h"
#include <iostream>
#include "ogr_api.h"
#include "ogrsf_frmts.h"
#include "ogr_spatialref.h"
#include "gdal_utils.h"
#include "gdal.h"
#include "vector"
#include "geo/raster/map.h"

#include <mlpack/core.hpp>
#include <mlpack/methods/ann/layer_names.hpp>
#include <mlpack/methods/neighbor_search/neighbor_search.hpp>


#include <iostream>
#include <fstream>
#include <sstream>

#include <mlpack/core.hpp>
#include <mlpack/methods/ann/layer/layer.hpp>
#include <mlpack/methods/ann/layer/layer_types.hpp>
#include <mlpack/methods/ann/init_rules/random_init.hpp>
#include <mlpack/methods/ann/init_rules/const_init.hpp>
#include <mlpack/methods/ann/init_rules/nguyen_widrow_init.hpp>
#include <mlpack/methods/ann/ffn.hpp>
#include <mlpack/methods/ann/rnn.hpp>
#include <mlpack/methods/ann/init_rules/gaussian_init.hpp>
#include <mlpack/methods/ann/loss_functions/sigmoid_cross_entropy_error.hpp>
#include <mlpack/methods/ann/gan/gan.hpp>
#include <mlpack/methods/ann/layer/layer.hpp>
#include <mlpack/methods/softmax_regression/softmax_regression.hpp>


#define features_num 256
#define samples_num  797

#include <mlpack/prereqs.hpp>
//#include <mlpack/core/util/cli.hpp>
//#include <mlpack/core/util/mlpack_main.hpp>

#include <ensmallen.hpp>

using namespace std;
using namespace mlpack;
//using namespace mlpack::regression;
using namespace mlpack::util;
using namespace mlpack::metric; // ManhattanDistance
using namespace mlpack::ann;
using namespace mlpack;
using namespace mlpack::neighbor; // NeighborSearch and NearestNeighborSort

using namespace mlpack::math;
using namespace mlpack::regression;
using namespace std::placeholders;

using std::endl;
using std::cout;


//the model, which can be trained, loaded, restored and served
class SRGANMLModel
{
    arma::mat trainData;

public:

    inline SRGANMLModel()
    {
        size_t dNumKernels = 32;
        size_t discriminatorPreTrain = 5;
        size_t batchSize = 5;
        size_t noiseDim = 100;
        size_t generatorUpdateStep = 1;
        size_t numSamples = 10;
        double stepSize = 0.0003;
        double eps = 1e-8;
        size_t numEpoches = 1;
        double tolerance = 1e-5;
        int datasetMaxCols = 10;
        bool shuffle = true;
        double multiplier = 10;



        size_t numIterations = trainData.n_cols * numEpoches;


        // Create the Discriminator network.
        FFN<SigmoidCrossEntropyError<> > discriminator;
        discriminator.Add<Convolution<> >(1, dNumKernels, 5, 5, 1, 1, 2, 2, 28, 28);
        discriminator.Add<ReLULayer<> >();
        discriminator.Add<MeanPooling<> >(2, 2, 2, 2);
        discriminator.Add<Convolution<> >(dNumKernels, 2 * dNumKernels, 5, 5, 1, 1,
            2, 2, 14, 14);
        discriminator.Add<ReLULayer<> >();
        discriminator.Add<MeanPooling<> >(2, 2, 2, 2);
        discriminator.Add<Linear<> >(7 * 7 * 2 * dNumKernels, 1024);
        discriminator.Add<ReLULayer<> >();
        discriminator.Add<Linear<> >(1024, 1);

        // Create the Generator network.
        FFN<SigmoidCrossEntropyError<> > generator;
        generator.Add<Linear<> >(noiseDim, 3136);
        generator.Add<BatchNorm<> >(3136);
        generator.Add<ReLULayer<> >();
        generator.Add<Convolution<> >(1, noiseDim / 2, 3, 3, 2, 2, 1, 1, 56, 56);
        generator.Add<BatchNorm<> >(39200);
        generator.Add<ReLULayer<> >();
        generator.Add<BilinearInterpolation<> >(28, 28, 56, 56, noiseDim / 2);
        generator.Add<Convolution<> >(noiseDim / 2, noiseDim / 4, 3, 3, 2, 2, 1, 1,
            56, 56);
        generator.Add<BatchNorm<> >(19600);
        generator.Add<ReLULayer<> >();
        generator.Add<BilinearInterpolation<> >(28, 28, 56, 56, noiseDim / 4);
        generator.Add<Convolution<> >(noiseDim / 4, 1, 3, 3, 2, 2, 1, 1, 56, 56);
        generator.Add<TanHLayer<> >();


        // Create GAN.
          GaussianInitialization gaussian(0, 1);
          ens::Adam optimizer(stepSize, batchSize, 0.9, 0.999, eps, numIterations,
              tolerance, shuffle);
          std::function<double()> noiseFunction = [] () {
              return math::RandNormal(0, 1);};
          GAN<FFN<SigmoidCrossEntropyError<> >, GaussianInitialization,
              std::function<double()> > gan(generator, discriminator,
              gaussian, noiseFunction, noiseDim, batchSize, generatorUpdateStep,
              discriminatorPreTrain, multiplier);

        std::stringstream stream;
        double objVal = gan.Train(trainData, optimizer, ens::ProgressBar(70, stream));

    }



    inline void AS_Train(cTMap * data)
    {


    }

    inline void AS_Save(QString file)
    {

    }

    inline void AS_Restore(QString file)
    {

    }

    inline cTMap * AS_Apply(cTMap * data)
    {

        return nullptr;
    }

};

void read_from_file(std::string filename, arma::mat& trainData, arma::mat& trainLabels){
    std::ifstream infile;
    infile.open(filename);
    if(infile.fail()){
        std::cerr << "ERROR: couldn't open the file " << filename << endl;
        exit(1);
    }
    for(int i = 0; i < samples_num; i++){
        for(int j = 0; j < features_num; j++){
            std::string data;
            std::getline(infile, data, ' ');
            trainData(j, i) = std::stod(data);
        }
        std::string label;
        std::getline(infile, label, ' ');
        trainLabels(i) = std::stod(label);
        std::getline(infile, label);
    }
}

static void LogisticRegression(arma::mat& parameters, arma::mat& labels)
{



}

static void AS_SpatialLogisticRegression(std::vector<cTMap*> parameters, cTMap * labels)
{

    //check input


    //get input

    /*QList<cTMap *> inputmapsq;

    for(int i = 0; i < parameters->GetSize(); i++)
    {
        inputmapsq.append((cTMap*) parameters->At(i));
    }*/

    //prepare data matrices


    //do regression


    //return result

}

static void test_function()
{
    bool x = true;
    x = true;
    if(!x)
    {
        // Load the data from data.csv (hard-coded).  Use CLI for simple command-line
        // parameter handling.
        arma::mat data;

        //fill the matrix

        // Use templates to specify that we want a NeighborSearch object which uses
        // the Manhattan distance.
        NeighborSearch<NearestNeighborSort, ManhattanDistance> nn(data);
        // Create the object we will store the nearest neighbors in.
        arma::Mat<size_t> neighbors;
        arma::mat distances; // We need to store the distance too.
        // Compute the neighbors.
        nn.Search(1, neighbors, distances);
        // Write each neighbor and distance using Log.
        for (size_t i = 0; i < neighbors.n_elem; ++i)
        {
          std::cout << "Nearest neighbor of point " << i << " is point "
              << neighbors[i] << " and the distance is " << distances[i] << ".\n";
        }
    }

    if(!x)
    {
        arma::mat trainData(features_num, samples_num);
        arma::mat trainLabels(1, samples_num);
        //arma::vec trainLabels(samples_num);
        read_from_file("train", trainData, trainLabels);

        FFN<> model;
        model.Add<Linear<>>(trainData.n_rows, 8);
        model.Add<SigmoidLayer<>>();                    // activation
        model.Add<Linear<>>(8, 1);
        model.Add<LogSoftMax<>>();                      // regression

        model.Train(trainData, trainLabels);
    }
}

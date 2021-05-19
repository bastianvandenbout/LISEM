/*
 * Copyright (c) <2016>, <Robert Reinecke>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef GLOBAL_FLOW_OPTIONS_HPP
#define GLOBAL_FLOW_OPTIONS_HPP

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional/optional.hpp>

namespace GlobalFlow {
namespace Simulation {

//using namespace std;

enum Stepsize {
    DAILY,
    MONTHLY
};

/**
 * @class Options
 * Reads simulation options from a JSON file
 * Defines getters and setters for options
 */
class Options {
public:
        //++Input data++//
        std::vector<std::string> ELEVATION_a;
        std::vector<std::string> EFOLDING_a;
        std::vector<std::string> SLOPE_a;
        std::vector<std::string> EQ_WTD_a;
        std::vector<std::string> BLUE_ELEVATION_a;

        std::string ELEVATION{""};
        std::string EFOLDING{""};
        std::string SLOPE{""};
        std::string EQ_WTD{""};
        std::string BLUE_ELEVATION{""};

        std::string RECHARGE{""};
        std::string LITHOLOGY{""};
        std::string RIVERS{""};
        std::string GLOBAL_LAKES{""};
        std::string GLOBAL_WETLANDS{""};
        std::string LOCAL_LAKES{""};
        std::string LOCAL_WETLANDS{""};
        std::string K_DIR{""};
        std::string RIVER_K_DIR{""};
        std::string OCEAN_K_DIR{""};
        std::string SS_FILE{""};
        std::string SY_FILE{""};
        std::string AQ_DEPTH{""};

        //++Special mappings++//
        std::string NODEID_ARCID{""};

        //++General configuration++//
        long NUMBER_OF_NODES{0};
        int LAYERS{0};
        int IITER{0};
        int I_ITTER{0};
        double RCLOSE{0.1};
        std::string SOLVER{"PCG"};
        std::string NODES{""};
        int THREADS{0};
        bool CACHE{false};
        bool ADAPTIVE_STEPSIZE{false};
        Stepsize stepsize{DAILY};
        std::string WETTING_APPROACH{"nwt"};
        int INITAL_HEAD{0};
        double K{0.001};
        double OCEAN_K{0.1};
        std::vector<int> AQUIFER_DEPTH{100};
        double ANISOTROPY{10};
        double SPECIFIC_YIELD{0.15};
        double SPECIFIC_STORAGE{0.000015};
        std::string BOUNDARY_CONDITION{"SeaLevel"};
        bool SENSITIVITY{false};
        std::vector<bool> CONFINED{};
    std::string BASE_PATH{"data"};
        bool k_from_lith{true};
        bool k_ocean_from_file{false};
        bool specificstorage_from_file{false};
        bool specificyield_from_file{false};
        bool k_river_from_file{false};
        bool aquifer_depth_from_file{false};

        bool ROW_COLS{false};
        double MAX_HEAD_CHANGE{0.01};
        bool DAMPING{false};
        double MIN_DAMP{0.01};
        double MAX_DAMP{0.5};

    public:

        enum BoundaryCondition {
            CONSTANT_HEAD_SEA_LEVEL,
            CONSTANT_HEAD_NEIGHBOUR,
            STATIC_HEAD_SEA_LEVEL
        };

        void setClosingCrit(double crit) { RCLOSE = crit; }

        void setDamping(bool set) { DAMPING = set; }

        bool isDampingEnabled() { return DAMPING; }

        double getMinDamp() { return MIN_DAMP; }

        double getMaxDamp() { return MAX_DAMP; }

        double getMaxHeadChange() { return MAX_HEAD_CHANGE; }

        bool isConfined(int layer) { return CONFINED[layer]; }

        std::vector<bool> getConfinements() { return CONFINED; }

        BoundaryCondition getBoundaryCondition() {
            if (BOUNDARY_CONDITION == "SeaLevel") {
                return BoundaryCondition::CONSTANT_HEAD_SEA_LEVEL;
            }
            if (BOUNDARY_CONDITION == "NeighbourHead") {
                return BoundaryCondition::CONSTANT_HEAD_NEIGHBOUR;
            }
            return BoundaryCondition::STATIC_HEAD_SEA_LEVEL;
        }

        bool isSensitivity() { return SENSITIVITY; }

        bool isKFromLith() { return k_from_lith; }

        bool isKOceanFile() { return k_ocean_from_file; }

        bool isSpecificStorageFile() { return specificstorage_from_file; }

        bool isSpecificYieldFile() { return specificyield_from_file; }

        bool isKRiverFile() { return k_river_from_file; }

        bool isAquiferDepthDile() { return aquifer_depth_from_file; }

        std::string getKDir() { return K_DIR; }

        std::string getKRiverDir() { return RIVER_K_DIR; }

        std::string getKOceanDir() { return OCEAN_K_DIR; }

        std::string getSSDir() { return SS_FILE; }

        std::string getSYDir() { return SY_FILE; }

        std::string getAQDepthDir() { return AQ_DEPTH; }

        bool isRowCol() { return ROW_COLS; }

        int getInnerItter() { return I_ITTER; }

        long
        getNumberOfNodes() {
            return NUMBER_OF_NODES;
        };

        int
        getNumberOfLayers() {
            return LAYERS;
        }

        int
        getMaxIterations() {
            return IITER;
        }

        double
        getConverganceCriteria() {
            return RCLOSE;
        }

        std::string
        getSolverName() {
            return SOLVER;
        }

        bool disableDryCells() {
            if (WETTING_APPROACH == "nwt") {
                return false;
            }
            if (WETTING_APPROACH == "classic") {
                return true;
            }
            return false;
        }

        //string getBasePath() {
        //    return BASE_PATH;
        //}

        std::string
        getNodesDir() {
            return NODES;
        }

        std::string
        getElevation() {
            return ELEVATION;
        }

        std::string
        getEfolding() {
            return EFOLDING;
        }

        std::string
        getEqWTD() {
            return EQ_WTD;
        }

        std::string getSlope() {
            return SLOPE;
        }

        std::string getBlue() {
            return BLUE_ELEVATION;
        }

        std::vector<std::string>
        getElevation_A() {
            return ELEVATION_a;
        }

        std::vector<std::string>
        getEfolding_a() {
            return EFOLDING_a;
        }

        std::vector<std::string>
        getEqWTD_a() {
            return EQ_WTD_a;
        }

        std::vector<std::string>
        getSlope_a() {
            return SLOPE_a;
        }

        std::vector<std::string>
        getBlue_a() {
            return BLUE_ELEVATION_a;
        }

        std::string
        getRecharge() {
            return RECHARGE;
        }

        std::string
        getLithology() {
            return LITHOLOGY;
        }

        std::string
        getRivers() {
            return RIVERS;
        }

        std::string
        getGlobalLakes() {
            return GLOBAL_LAKES;
        }

        std::string
        getGlobalWetlands() {
            return GLOBAL_WETLANDS;
        }

        std::string
        getLocalLakes() {
            return LOCAL_LAKES;
        }

        std::string
        getLocalWetlands() {
            return LOCAL_WETLANDS;
        }

        std::string
        getMapping() {
            return NODEID_ARCID;
        }

        int
        getThreads() {
            return THREADS;
        }

        const bool
        adaptiveStepsizeEnabled() {
            return ADAPTIVE_STEPSIZE;
        }

        //Computations are all based on daily
        const int
        getStepsizeModifier(){
            switch (stepsize) {
                case DAILY:
                    return 1;
                case MONTHLY:
                    return 31;
            }
            throw std::out_of_range("No valid stepsize\n");
        }

        bool
        cacheEnabled() {
            return CACHE;
        }

        int
        getInitialHead() {
            return INITAL_HEAD;
        }

        double
        getInitialK() {
            return K;
        }

        double
        getOceanConduct() {
            return OCEAN_K;
        }

        std::vector<int>
        getAquiferDepth() {
            return AQUIFER_DEPTH;
        }

        double
        getAnisotropy() {
            return ANISOTROPY;
        }

        double
        getSpecificYield() {
            return SPECIFIC_YIELD;
        }

        double
        getSpecificStorage() {
            return SPECIFIC_STORAGE;
        }

        void
        load(const std::string &filename);

        //FIXME implement node serialization
        void
        save(const std::string &filename);
};

}
}//ns
#endif //COVERAGE_OPTIONS_H

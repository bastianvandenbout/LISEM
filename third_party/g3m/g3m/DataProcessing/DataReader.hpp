/*
 * Copyright (c) <2016>, <Robert Reinecke>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or other
 * materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef GLOBAL_FLOW_DATAREADER_HPP
#define GLOBAL_FLOW_DATAREADER_HPP

#include "../csv.h"
#include "../Model/Node.hpp"
#include "../Logging/Logging.hpp"
#include "../Simulation/Options.hpp"
#include <boost/filesystem.hpp>
#include <fstream>
#include <vector>


namespace GlobalFlow {
    using NodeVector = std::shared_ptr<std::vector<std::unique_ptr<Model::NodeInterface>>>;
    namespace fs = boost::filesystem;
    using large_num = unsigned long int;

    /**
     * @class DataReader
     * @brief Interface that needs to be implemented for reading in required data for the model
     */
    class DataReader {
    protected:
        NodeVector nodes;
        int stepMod;
        /**
         * @var basePath the standard path for data to be read from
         * relative to executable or absolute path
         */
        std::string basePath{"data"};
        fs::path data_dir{basePath};
        /** @var lookupglobIDtoID <GlobalID, ID>*/
        std::unordered_map<int, int> lookupglobIDtoID;
        /** @var lookupZeroPointFivetoFiveMinute <ARCID(0.5°), vector<GlobalID(5')>>*/
        std::unordered_map<int, std::vector<int>> lookupZeroPointFivetoFiveMinute;
    public:
        /** Virt destructor -> interface*/
        virtual ~DataReader() {}

        /**
         * @brief Initialize internal ref to node vetor
         * @param nodes The vector of nodes
         */
        void initNodes(NodeVector nodes) { this->nodes = nodes; }

        /**
         * @brief Entry point for reading simulation data
         * @attention This method needs to be implemented!
         * @note readData() is called by simulation at startup
         * @param op Options object
         */
        virtual void readData(Simulation::Options op) = 0;

        /**
         * @brief Generic method for looping through files inside a directory and applying a generic function
         * @param path the directory
         * @param files a vector of files
         * @param fun a function that is applied e.g. reading the data
         */
        template<class Fun>
        void loopFiles(std::string path, std::vector<std::string> files, Fun fun) {
            for (std::string file : files) {
                std::string real_path = path + '/' + file;
                fun(real_path);
            }
        }

        /**
         * @brief Check weather id exists in the simulation
         * @param globid Global identifier, can be different from position in node vector
         * @return i the position in the node vector
         */
        inline int check(int globid) {
            int i{0};
            try {
                i = lookupglobIDtoID.at(globid);
            }
            catch (const std::out_of_range &ex) {
                return -1;
            }
            return i;
        }

        /**
         * @brief Read data from a two-column csv file and apply function to data
         * @param path to the csv file
         * @param processData A processing function e.g. upscaling of data
         */
        template<class ProcessDataFunction>
        void readTwoColumns(std::string path, ProcessDataFunction processData) {
            io::CSVReader<2, io::trim_chars<' ', '\t'>, io::no_quote_escape<','>> in(path);
            in.read_header(io::ignore_no_column, "global_ID", "data");
            int globid = 0;
            double data = 0;
            int pos = 0;
            while (in.read_row(globid, data)) {
                pos = check(globid);
                if (pos == -1) {
                    continue;
                }
                if (nodes->at(pos)->getProperties().get<large_num, Model::ArcID>() != globid) {
                    throw "Error in reading globID";
                }
                processData(data, pos);
            }
        }

        /**
         * @brief Creates a mapping of 0.5° ArcIDs to a list of contained 5' GlobIDs
         * @param path to file
         */
        void readZeroPointFiveToFiveMin(std::string path) {
            io::CSVReader<2, io::trim_chars<' ', '\t'>, io::no_quote_escape<','>> in(path);
            in.read_header(io::ignore_no_column, "GLOBALID", "ARC_ID");
            int globID = 0;
            int arcID = 0;

            while (in.read_row(globID, arcID)) {
                lookupZeroPointFivetoFiveMinute[arcID].push_back(std::move(globID));
            }
        }


        /**
         * @brief provides acccess to mapping of different resolutions
         * @return <ARCID(0.5°), vector<GlobalID(5')>>
         */
        const std::unordered_map<int, std::vector<int>> &getArcIDMapping() {
            return lookupZeroPointFivetoFiveMinute;
        };

        /**
         * @brief provides access to mapping of data ids to position in node vector
         * @return <GlobalID, ID>
         */
        const std::unordered_map<int, int> &getGlobIDMapping() {
            return lookupglobIDtoID;
        };

        /**
         * @brief Builds a corect path from the base dir
         * @param path The relative path from the config
         * @return A path based on the base dir
         */
        std::string buildDir(std::string path) {
            return (data_dir / fs::path(path)).string();
        };
    };
}
#endif //DATAREADER_HPP

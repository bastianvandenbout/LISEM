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

#ifndef GLOBAL_FLOW_OUTPUTFACT_HPP
#define GLOBAL_FLOW_OUTPUTFACT_HPP

#include "Converter.hpp"

#include <ctime>
#include <iomanip>

namespace GlobalFlow {
    namespace DataProcessing {
        namespace DataOutput {

            /**
             * @enum Output Type
             * @brief What kind of output format
             */
            enum class OutputType {
                CSV,
                NET_CDF,
                GFS_JSON,
                NON_VALID
            };

            /**
             * @var outputMapping
             * Mapping of string representation to internal field
             */
            const std::unordered_map<std::string, OutputType> outputMapping{
                    {"csv",      OutputType::CSV},
                    {"netcdf",   OutputType::NET_CDF},
                    {"gfs-json", OutputType::GFS_JSON}
            };

            using path = std::string;
            using pos_v = std::vector<std::pair<double, double>>;
            using a_vector = std::vector<large_num>;

            /**
             * @class OutputInterface
             * Writes data to a file
             */
            template<typename T>
            class OutputInterface {
            public:
                virtual ~OutputInterface() {};

                /**
                 * Needs to be implemented
                 * @param filePath
                 * @param printID Bool
                 * @param printXY Bool
                 * @param data Data vector
                 * @param p Position vector
                 */
                virtual void
                write(path filePath, bool printID, bool printXY, std::vector<T> data, pos_v p, a_vector ids) = 0;
            };

            /**
             * @class CSVOutput
             * Writes data to a CSV file
             */
            template<typename T>
            class CSVOutput : public OutputInterface<T> {
            public:
                void
                write(path filePath, bool printID, bool printXY, std::vector<std::pair<double, double>> data, pos_v p,
                      a_vector ids) {}

                void write(path filePath, bool printID, bool printXY, std::vector<bool> data, pos_v p, a_vector ids) {}

                void write(path filePath, bool printID, bool printXY, std::vector<double> data, pos_v p, a_vector ids) {
                    std::vector<std::string> d;
                    d.reserve(data.size());
                    for (int i = 0; i < data.size(); ++i) {
                        std::ostringstream out;
                        out << std::scientific << std::setprecision(17) << data[i];
                        d.emplace_back(out.str());
                    }
                    write(filePath, printID, printXY, d, p, ids);
                }

                void
                write(path filePath, bool printID, bool printXY, std::vector<std::string> data, pos_v p, a_vector ids) {
                    std::ofstream ofs;
                    ofs.open(filePath + ".csv", std::ofstream::out | std::ofstream::trunc);
                    if (printID) {
                        ofs << "ID,";
                    }
                    if (printXY) {
                        ofs << "X,Y,";
                    }
                    ofs << "data";
                    ofs << std::endl;

                    for (int i = 0; i < data.size(); ++i) {
                        if (printID) { ofs << ids[i] << ","; }
                        if (printXY) { ofs << p[i].first << "," << p[i].second << ","; }

                        ofs << data[i] << std::endl;
                    }
                    ofs.close();
                }
            };

            /**
             * @class NETCDFOutput
             * Writes data to a NETCDF file
             */
            template<typename T>
            class NETCDFOutput : public OutputInterface<T> {
            public:
                void write(path filePath, bool printID, bool printXY, std::vector<T> data, pos_v p, a_vector ids) {}
            };

            /**
            * JSON-ified GRIB files. Example:
            *
            *     [
            *       {
            *         "header": {
            *           "refTime": "2013-11-30T18:00:00.000Z",
            *           "parameterCategory": 2,
            *           "parameterNumber": 2,
            *           "surface1Type": 100,
            *           "surface1Value": 100000.0,
            *           "forecastTime": 6,
            *           "scanMode": 0,
            *           "nx": 360,
            *           "ny": 181,
            *           "lo1": 0,
            *           "la1": 90,
            *           "lo2": 359,
            *           "la2": -90,
            *           "dx": 1,
            *           "dy": 1
            *         },
            *         "data": [3.42, 3.31, 3.19, 3.08, 2.96, 2.84, 2.72, 2.6, 2.47, ...]
            *       }
            *     ]
            *
            * TODO rebuild me with boost ptree
            */

            using vector = std::pair<std::string, std::string>;
            using d_pair = std::pair<double, double>;
            using vector_d = std::vector<double>;

            /**
             * @class Write data to a pseudo GFS json-like file
             * @note provides additonal methods for downscaling
             */
            template<typename T>
            class GFS_JSONOutput : public OutputInterface<T> {
            private:
                template<typename Container_T>
                string str(Container_T begin, Container_T end) {
                    stringstream ss;
                    bool first = true;
                    for (; begin != end; begin++) {
                        double val = *begin;
                        if (!first)
                            ss << ",";
                        if (std::isnan(val)) {
                            ss << "null";
                        } else {
                            ss << std::scientific << setprecision(17) << val;
                        }
                        first = false;
                    }
                    return ss.str();
                }

                vector buildData(std::vector<double> data, pos_v p, std::ofstream &ofs) {
                    vector v = scaleData(data, p);
                    ofs << "[\n" << buildHeader() << v.first << "]\n}]";
                    return v;
                }

                vector buildData(std::vector<bool> data, pos_v p, std::ofstream &ofs) { return vector(); }

                vector buildData(std::vector<std::string> data, pos_v p, std::ofstream &ofs) {
                    vector v = scaleData(data, p);
                    ofs << "[\n" << buildHeader() << v.first << "]\n}]";
                    return v;
                }

                template<typename InputIterator>
                inline double accumulate(InputIterator first, InputIterator last, int b_max) {
                    double init{0.0};
                    double boundary{0};
                    for (; first != last; ++first) {
                        if (boundary > b_max) { return NAN; }
                        if (std::isnan(*first)) {
                            boundary++;
                            continue;
                        }
                        init = init + *first;
                    }
                    return init;
                };

                /**
                 * Scan mode 0 assumed. Longitude increases from λ0, and latitude decreases from φ0.
                 * http://www.nco.ncep.noaa.gov/pmb/docs/grib2/grib2_table3-4.shtml
                 * @return a string of an array containing data in steps
                 *
                 * First data of U-component then data of V-component
                 * TODO support non vector data
                 */
                vector buildData(std::vector<d_pair> data, pos_v p, std::ofstream &ofs) {
                    assert(data.size() == p.size() && "Size of positions and vector don't match!");
                    std::deque<d_pair> d_data(data.begin(), data.end());
                    std::deque<d_pair> d_pos(p.begin(), p.end());

                    vector v = scaleData(data, p);
                    ofs << "[\n" << buildHeader() << v.first << "]\n}," << buildHeader() << v.second << "]\n}]";
                    return v;
                }

                void push(vector_d &u, vector_d &v, string x) {
                    u.push_back(::atof(x.c_str()));
                    //FIXME dummy push
                    v.push_back(::atof(x.c_str()));
                }

                void push(vector_d &u, vector_d &v, double x) {
                    u.push_back(x);
                    v.push_back(x);
                }

                void push(vector_d &u, vector_d &v, d_pair x) {
                    u.push_back(x.first);
                    v.push_back(x.second);
                }

                vector scaleData(std::vector<T> data, pos_v p) {
                    assert(data.size() == p.size() && "Size of positions and vector don't match!");
                    std::deque<T> d_data(data.begin(), data.end());
                    std::deque<d_pair> d_pos(p.begin(), p.end());

                    bool down_scale{true};

                    /*
                     * Very simple epsilon function - !do not use to compare floats in general
                     */
                    std::function<bool(double, double)> almost_near = [](const double a, const double b) {
                        constexpr double EPSILON_FIVE_MIN{0.0421}; //0.0421 = 1/2 5'
                        return (std::abs(a - b) < EPSILON_FIVE_MIN);
                    };

                    std::function<bool(double, double, d_pair)> check = [almost_near](const double x, const double y,
                                                                                      d_pair pos) {
                        return almost_near(pos.first, x) and almost_near(pos.second, y);
                    };

                    std::function<double(double)> log_mod = [](double x) {
                        return std::signbit(x) ? -std::log(std::abs(x) + 1) : std::log(std::abs(x) + 1);
                    };

                    vector_d u;
                    vector_d v;

                    /**
                     * vector contains "U-comp,V-comp"
                     * We need to split and fill with null for empty frame
                     * Currently only support for 5' resolution
                     * 360° = 21600' /5 = 4320 W-E
                     * 180° = 10800' /5 = 2160 N-S
                     * Start: 0.0W , 90.0N
                     *
                     * If downscaling enabled:
                     * 1° = 12 * 5'
                     * 12 x 12 -> 144 5' in a 1° cell
                     * 360 W-E
                     * 180 N-S
                     *
                     * 0,5° = 6 * 5'
                     * 6 x 6 = 36 5' in 0.5°
                     * 720 W-E
                     * 360 N-S
                     */
                    int ni{4320}; // grid points in W-E
                    int nj{2160}; // grid points in N-S
                    double step = 0.0833333; //5' steps in arc-degree
                    double currentX{-180};
                    double currentY{90};


                    for (int j = 0; j < nj; ++j) {
                        //Moving from N to S
                        for (int k = 0; k < ni; ++k) {
                            //Moving from W-E
                            d_pair pos = d_pos.front();
                            if (check(currentX, currentY, pos)) {
                                if (d_data.empty()) { break; }
                                auto d = d_data.front();
                                d_data.pop_front();
                                d_pos.pop_front();
                                push(u, v, d);
                            } else {
                                u.push_back(NAN);
                                v.push_back(NAN);
                            }
                            currentX = currentX + step;
                        }
                        currentY = currentY - step;
                        currentX = -180;
                    }

                    if (down_scale) {
                        vector_d u_down;
                        vector_d v_down;
                        const int steps{6};
                        const int total{36};
                        const int boundary{25};
                        assert(u.size() == v.size() && "Vector components don't have the same size");
                        int l{1};
                        for (int j = 0; j < u.size();) {
                            double val_u{0};
                            double val_v{0};
                            for (int k = 0; k < steps; ++k) {
                                val_u = val_u +
                                        accumulate(u.begin() + (j + (ni * k)), u.begin() + (j + (steps - 1) + (ni * k)),
                                                   boundary);
                                val_v = val_v +
                                        accumulate(v.begin() + (j + (ni * k)), v.begin() + (j + (steps - 1) + (ni * k)),
                                                   boundary);
                            }
                            val_u = val_u / total;
                            val_v = val_v / total;
                            u_down.push_back(val_u);
                            v_down.push_back(val_v);
                            j = j + steps;
                            if (j % ni == 0) {
                                j = (steps * ni) * l;
                                l++;
                            }
                        }
                        return std::make_pair(str(u_down.begin(), u_down.end()), str(v_down.begin(), v_down.end()));
                    }
                    //LOG(debug) << "Number of leftovers" << d_pos.size();
                    return std::make_pair(str(u.begin(), u.end()), str(v.begin(), v.end()));
                }

                std::string buildHeader() {
                    std::time_t result = std::time(nullptr);
                    std::string t = std::asctime(std::localtime(&result));
                    t.pop_back();
                    const std::string time =
                            "\"refTime\":\"" + t + "\",\n" + "\"forecastTime\": 0,";
                    //nx, ny  number of grid points W-E and N-S
                    //const std::string params = "\"lo1\": -180, \"la1\": 90, \"dx\":0.0833333, \"dy\":0.0833333, \"nx\": 4320, \"ny\":2160";
                    //const std::string params = "\"lo1\": -180, \"la1\": 90, \"dx\":1, \"dy\":1, \"nx\": 360, \"ny\":180";
                    const std::string params = "\"lo1\": -180, \"la1\": 90, \"dx\":0.5, \"dy\":0.5, \"nx\": 720, \"ny\":360";
                    const std::string header = "{\"header\": {\n" + time + params + "}, \"data\":[";
                    return header;
                }

            public:
                void write(path filePath, bool printID, bool printXY, std::vector<T> data, pos_v p, a_vector ids) {
                    std::ofstream ofs;
                    ofs.open(filePath + ".json", std::ofstream::out | std::ofstream::trunc);
                    buildData(data, p, ofs);
                    ofs.close();
                }
            };

            /**
             * @class Factory for yielding correct data output file
             */
            template<typename T>
            class OutputFactory {
            private:
                static OutputInterface<T> *create(OutputType type) {
                    switch (type) {
                        case OutputType::CSV:
                            return (OutputInterface<T> *) new CSVOutput<T>();
                        case OutputType::NET_CDF :
                            return (OutputInterface<T> *) new NETCDFOutput<T>();
                        case OutputType::GFS_JSON :
                            return (OutputInterface<T> *) new GFS_JSONOutput<T>();
                        case OutputType::NON_VALID:
                            throw std::bad_function_call();
                    }
                }

            public:
                static OutputInterface<T> *getOutput(OutputType type) {
                    return create(type);
                };
            };

        }
    }
}
#endif
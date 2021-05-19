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

#ifndef GLOBAL_FLOW_SIMULATION_H
#define GLOBAL_FLOW_SIMULATION_H

#include "../Solver/Equation.hpp"
#include "../DataProcessing/DataReader.hpp"
#include "../Logging/Logging.hpp"
#include "../Model/Node.hpp"
#include "../DataProcessing/Neighbouring.hpp"
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/tracking.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/multiprecision/gmp.hpp>
#include <type_traits>
#include <boost/filesystem.hpp>

namespace GlobalFlow {
namespace Simulation {
using NodeVector = std::shared_ptr<std::vector<std::unique_ptr<Model::NodeInterface>>>;
using namespace boost::multiprecision;
using namespace boost::units;
namespace fs = boost::filesystem;

/**
 * @class MassError
 * Simple container for the mass error calulations
 */
class MassError {
    public:
        MassError(mpf_float_1000 OUT, mpf_float_1000 IN, mpf_float ERR) : OUT(OUT), IN(IN), ERR(ERR) {}

        mpf_float_1000 OUT;
        mpf_float_1000 IN;
        mpf_float ERR = 0;
};

/**
 * @class Simulation
 * The simulation class which holds the equation, options and data instance
 * Further contains methods for calulating the mass balance and sensitivity methods
 * TODO: Clean me up!
 */
class Simulation {
        Solver::Equation *eq;
        Logging::LoggerInterface *logger = Logging::Logger::instance();
        DataReader *reader;
        Options op;

    public:
        NodeVector nodes;
        Simulation(Options op, DataReader *reader);
        Simulation() {}
        Solver::Equation *getEquation();
        void save();

        /**
         * Get basic node information by its id
         * @param nodeID
         * @return A string of information
         */
        std::string NodeInfosByID(unsigned long nodeID) {
            Model::NodeInterface *nodeInterface = nodes->at(nodeID).get();
            std::string out("\n");
            out += "IN: ";
            out += to_string(nodeInterface->getCurrentIN().value());
            out += "\nOUT: ";
            out += to_string(nodeInterface->getCurrentOUT().value());
            out += "\nElevation: ";
            out +=
                    to_string(nodeInterface->getProperties().get < quantity < Model::Meter > ,
                              Model::Elevation > ().value());
            out += "\nRHS: ";
            out += to_string(nodeInterface->getRHS().value());
            out += "\nHEAD: ";
            out += to_string(nodeInterface->getProperties().get < quantity < Model::Meter > , Model::Head > ().value());
            out += "\nArea: ";
            out +=
                    to_string(nodeInterface->getProperties().get < quantity < Model::SquareMeter > ,
                              Model::Area > ().value());
            out += "\nStorageFlow: ";
            out += to_string(nodeInterface->getTotalStorageFlow().value());
            out += "\nNONStorageFlowIN: ";
            out += to_string(nodeInterface->getNonStorageFlow([](double a) -> bool {
                return a > 0;
            }).value());
            out += "\nNONStorageFlowOUT: ";
            out += to_string(nodeInterface->getNonStorageFlow([](double a) -> bool {
                return a < 0;
            }).value());
            out += "\n";
            std::ostringstream strs;
            strs << nodeInterface;
            out += strs.str();

            return out;
        }

        /**
         * Get budget per node
         * @param ids
         * @return
         */
        template<int FieldNum>
        std::string getFlowSumByIDs(std::array<int, FieldNum> ids) {
            double in{0};
            double out{0};
            for (int j = 0; j < nodes->size(); ++j) {
                auto id = std::find(std::begin(ids), std::end(ids), nodes->at(j)->getID());
                if (id != std::end(ids)) {
                    in += nodes->at(j)->getIN().value();
                    out += nodes->at(j)->getOUT().value();
                }
            }
            std::ostringstream output;
            output << to_string(in) << "\n";
            output << to_string(out);
            return output.str();
        }

        /**
         * Return all external flows seperatly
         */
        std::string NodeFlowsByID(unsigned long nodeID) {
            long id{0};
            for (int j = 0; j < nodes->size(); ++j) {
                if (nodes->at(j)->getID() == nodeID) {
                    id = j;
                }
            }
            Model::NodeInterface *nodeInterface = nodes->at(id).get();
            std::string out("");
            //Flows Budget for
            //ID, Elevation, Head, IN, OUT, Recharge, River_MM, Lake, Wetland
            out += to_string(nodeID);
            out += ",";
            out += to_string(nodeInterface->getProperties().get < quantity < Model::Meter > ,
                             Model::Elevation > ().value());
            out += ",";
            out += to_string(nodeInterface->getProperties().get < quantity < Model::Meter > ,
                             Model::Head > ().value());
            out += ",";
            out += to_string(nodeInterface->getIN().value());
            out += ",";
            out += to_string(nodeInterface->getOUT().value());
            out += ",";
            out += to_string(nodeInterface->getExternalFlowVolumeByName(Model::RECHARGE).value());
            out += ",";
            out += to_string(nodeInterface->getExternalFlowVolumeByName(Model::RIVER_MM).value());
            out += ",";
            out += to_string(nodeInterface->getExternalFlowVolumeByName(Model::LAKE).value());
            out += ",";
            out += to_string(nodeInterface->getExternalFlowVolumeByName(Model::WETLAND).value());
            return out;
        }

        /**
         * Calulate the mass error
         * @param fun1 Function to get OutFlow
         * @param fun2 Function to get InFlow
         * @return
         */
        template<class FunOut, class FunIn>
        MassError getError(FunOut fun1, FunIn fun2) {
            mpf_float_1000 out = 0;
            mpf_float_1000 in = 0;
            mpf_float error = 0;
            for (int j = 0; j < nodes->size(); ++j) {
                out = out + fun1(j);
                in = in + fun2(j);
            }
            if (abs(in - out) > 0.00001) {
                error = ((100 * (in - out)) / ((in + out) / 2));
            }
            MassError err(out, in, error);
            return err;
        }

        /**
         * Get the total mass balance
         * @return
         */
        MassError getMassError() {
            return getError([this](int pos) {
                                return nodes->at(pos)->getOUT().value();
                            },
                            [this](int pos) {
                                return nodes->at(pos)->getIN().value();
                            });
        }

        /**
         * Get the mass balance for the current step
         * @return
         */
        MassError getCurrentMassError() {
            return getError([this](int pos) {
                                return nodes->at(pos)->getCurrentOUT().value();
                            },
                            [this](int pos) {
                                return nodes->at(pos)->getCurrentIN().value();
                            });
        }

        /**
         * Get the flow lost to external flows
         * @return
         */
        double getLossToRivers() {
            double out = 0.0;
            for (int j = 0; j < nodes->size(); ++j) {
                out += nodes->at(j)->getNonStorageFlow([](double a) -> bool {
                    return a < 0;
                }).value();
            }
            return out;
        }

        enum Flows {
            RIVERS = 1,
            DRAINS,
            RIVER_MM,
            LAKES,
            WETLANDS,
            GLOBAL_WETLANDS,
            RECHARGE,
            FASTSURFACE,
            NAG,
            STORAGE,
            GENERAL_HEAD_BOUNDARY
        };

        /**
         * Decide if its an In or Outflow
         * @param fun
         * @return
         */
        template<class Fun>
        MassError inline getError(Fun fun) {
            return getError([this, fun](int pos) {
                                double flow = fun(pos);
                                return flow < 0 ? flow : 0;
                            },
                            [this, fun](int pos) {
                                double flow = fun(pos);
                                return flow > 0 ? flow : 0;
                            });
        }

        /**
         * Helper function for printing the mass balance for each flow
         * @param flow
         * @return
         */
        std::string getFlowByName(Flows flow) {
            std::ostringstream stream;
            MassError tmp(0, 0, 0);
            switch (flow) {
                case GENERAL_HEAD_BOUNDARY:
                    tmp = getError([this](int i) {
                        return nodes->at(i)->getExternalFlowVolumeByName(Model::GENERAL_HEAD_BOUNDARY).value();
                    });
                    break;
                case RECHARGE:
                    tmp = getError([this](int i) {
                        return nodes->at(i)->getExternalFlowVolumeByName(Model::RECHARGE).value();
                    });
                    break;
                case FASTSURFACE:
                    tmp = getError([this](int i) {
                        return nodes->at(i)->getExternalFlowVolumeByName(Model::FAST_SURFACE_RUNOFF).value();
                    });
                    break;
                case NAG:
                    tmp = getError([this](int i) {
                        return nodes->at(i)->getExternalFlowVolumeByName(Model::NET_ABSTRACTION).value();
                    });
                    break;
                case RIVERS:
                    tmp = getError(
                            [this](int i) { return nodes->at(i)->getExternalFlowVolumeByName(Model::RIVER).value(); });
                    break;
                case DRAINS:
                    tmp = getError(
                            [this](int i) { return nodes->at(i)->getExternalFlowVolumeByName(Model::DRAIN).value(); });
                    break;
                case RIVER_MM:
                    tmp = getError([this](int i) {
                        return nodes->at(i)->getExternalFlowVolumeByName(Model::RIVER_MM).value();
                    });
                    break;
                case LAKES:
                    tmp = getError(
                            [this](int i) { return nodes->at(i)->getExternalFlowVolumeByName(Model::LAKE).value(); });
                    break;
                case WETLANDS:
                    tmp = getError([this](int i) {
                        return nodes->at(i)->getExternalFlowVolumeByName(Model::WETLAND).value();
                    });
                    break;
                case GLOBAL_WETLANDS:
                    tmp = getError([this](int i) {
                        return nodes->at(i)->getExternalFlowVolumeByName(Model::GLOBAL_WETLAND).value();
                    });
                    break;
                case STORAGE:
                    tmp = getError([this](int i) { return nodes->at(i)->getTotalStorageFlow().value(); });
                    break;
            }

            stream << "IN :" << tmp.IN << "  OUT :" << tmp.OUT;
            return stream.str();
        }

        /**
         * Prints all mass balances
         */
        void printMassBalances() {
            MassError currentErr = getCurrentMassError();
            MassError totalErr = getMassError();
            LOG(userinfo) << "All units in meter per stepsize";
            LOG(userinfo) << "Current Total Error: " << currentErr.ERR << "IN: " << currentErr.IN << "Out: "
                          << currentErr.OUT;
            LOG(userinfo) << "Total Error: " << totalErr.ERR << "IN: " << totalErr.IN << "Out: "
                          << totalErr.OUT;
            LOG(userinfo) << "General Head Boundary" << getFlowByName(GENERAL_HEAD_BOUNDARY);
            LOG(userinfo) << "Rivers: " << getFlowByName(RIVERS);
            LOG(userinfo) << "Drains: " << getFlowByName(DRAINS);
            LOG(userinfo) << "Dynamic Rivers: " << getFlowByName(RIVER_MM);
            LOG(userinfo) << "Lakes: " << getFlowByName(LAKES);
            LOG(userinfo) << "Wetlands: " << getFlowByName(WETLANDS);
            LOG(userinfo) << "Global wetlands: " << getFlowByName(GLOBAL_WETLANDS);
            LOG(userinfo) << "Recharge: " << getFlowByName(RECHARGE);
            //LOG(userinfo) << "Fast Surface Runoff: " << getFlowByName(FASTSURFACE) << "\n";
            LOG(userinfo) << "Net abstraction from groudnwater: " << getFlowByName(NAG);
            LOG(userinfo) << "Storage: " << getFlowByName(STORAGE);
        }

        inline const double calcRecharge(const double recharge, const double &area) {
            return (recharge * area) / 360;
        }

        DataReader *getDataReader() {
            return this->reader;
        }

        NodeVector &
        getNodes() {
            return nodes;
        }

        void scaleByIds(std::vector<int> ids, std::string field, double mult) {
            for (auto id : ids) {
                int i{-1};
                try {
                    i = reader->getGlobIDMapping().at(id);
                } catch (const std::out_of_range &ex) {
                    continue;
                }
                if (field == "RECHARGE") {
                    try {
                        Model::ExternalFlow flow = nodes->at(i)->getExternalFlowByName(Model::RECHARGE);
                        double val = flow.getRecharge().value() * mult;
                        nodes->at(i)->updateUniqueFlow(val, Model::RECHARGE);
                    }
                    catch (const std::out_of_range &ex) {
                        continue;
                    }
                }
            }
        }

        template<class Fun, class ChangeFunction>
        void scaleByFunction(Fun fun, ChangeFunction apply) {
            for (int i = 0; i < nodes->size(); ++i) {
                if (fun(nodes->at(i))) {
                    apply(nodes->at(i));
                }
            }
        };

        /**
         * Helper function for sensitivity
         * @param fun
         * @param field
         * @param mult
         */
        template<class Fun>
        void scaleByFunction(Fun fun, std::string field, double mult) {
            for (int i = 0; i < nodes->size(); ++i) {
                if (fun(nodes->at(i))) {
                    if (field == "K") {
                        quantity <Model::Velocity> k = nodes->at(
                                i)->getProperties().get<Model::quantity<Model::Velocity>, Model::K>();
                        nodes->at(i)->setK(k * mult);
                    }
                    if (field == "Anisotropy") {
                        quantity < Model::Dimensionless > k =
                                nodes->at(i)->getProperties().get < quantity < Model::Dimensionless >,
                                Model::Anisotropy > ();
                        nodes->at(i)->getProperties().set < quantity < Model::Dimensionless > ,
                                Model::Anisotropy > (k * mult);
                    }
                    if (field == "Depth_0") {
                        if (nodes->at(i)->getProperties().get<int, Model::Layer>() == 0) {
                            quantity < Model::Meter > d = nodes->at(i)->getProperties().get < quantity < Model::Meter >,
                                    Model::VerticalSize > ();
                            nodes->at(i)->getProperties().set < quantity < Model::Meter > , Model::VerticalSize >
                                                                                            (d * mult);
                            //TODO fix elevation for nodes below..
                        }
                    }
                    if (field == "RECHARGE") {
                        try {
                            Model::ExternalFlow flow = nodes->at(i)->getExternalFlowByName(Model::RECHARGE);
                            //LOG(debug) << "Multiplier: " << mult;
                            double val = flow.getRecharge().value() * mult;
                            nodes->at(i)->updateUniqueFlow(val, Model::RECHARGE);
                        }
                        catch (const std::out_of_range &ex) {
                            continue;
                        }
                    }
                    if (field == "K_Wetlands") {
                        try {
                            nodes->at(i)->updateExternalFlowConduct(mult, Model::WETLAND);
                        }
                        catch (const std::out_of_range &ex) {
                            continue;
                        }
                    }
                    if (field == "K_Lakes") {
                        try {
                            nodes->at(i)->updateExternalFlowConduct(mult, Model::LAKE);
                        }
                        catch (const std::out_of_range &ex) {
                            continue;
                        }
                    }
                    if (field == "Lakes_bottom") {
                        try {
                            nodes->at(i)->updateLakeBottoms(mult);
                        }
                        catch (const std::out_of_range &ex) {
                            continue;
                        }
                    }
                    if (field == "River_Mult") {
                        try {
                            nodes->at(i)->scaleDynamicRivers(mult);
                        }
                        catch (const std::out_of_range &ex) {
                            continue;
                        }
                    }
                    if (field == "HEAD") {
                        quantity <Model::Meter> m = nodes->at(
                                i)->getProperties().get<Model::quantity<Model::Meter>, Model::Head>();
                        nodes->at(i)->getProperties().set<Model::quantity<Model::Meter>, Model::Head>(m * mult);
                    }

                }
            }

        }


    /**
     * Expects a file with global_ID, parameter, multiplier
     * Multiplier is log scaled
     * @param path to csv file
     * @returns a vector of node-ids (used to write out heads in correct order)
     */
    std::vector<int> readMultipliersPerID(std::string path) {
        std::vector<int> ids;
        io::CSVReader<3, io::trim_chars<' ', '\t'>, io::no_quote_escape<','>> in(path);
        in.read_header(io::ignore_no_column, "global_ID", "mult", "parameter");
        int id;
        std::string parameter;
        std::string prev_parameter;
        bool n_param{false};
        double mult;
        while (in.read_row(id, mult, parameter)) {
            int i = reader->getGlobIDMapping().at(id);

            //Only save head ids once
            if (not n_param) {
                prev_parameter = parameter;
                n_param = true;
                ids.push_back(i);
            } else if (prev_parameter == parameter) {
                ids.push_back(i);
            }

            mult = pow(10, mult);
            NANChecker(mult, "Sensitivity multiplier");

            if (parameter == "K") {
                quantity<Model::Velocity> k = nodes->at(
                        i)->getProperties().get<Model::quantity<Model::Velocity>, Model::K>();
                nodes->at(i)->setK(k * mult);
            }
            if (parameter == "K_Lakes") {
                try {
                    nodes->at(i)->updateExternalFlowConduct(mult, Model::LAKE);
                }
                catch (const std::out_of_range &ex) {}
            }
            if (parameter == "K_Wetlands") {
                try {
                    nodes->at(i)->updateExternalFlowConduct(mult, Model::WETLAND);
                }
                catch (const std::out_of_range &ex) {}
            }
            if (parameter == "K_Global_Wet") {
                try {
                    nodes->at(i)->updateExternalFlowConduct(mult, Model::GLOBAL_WETLAND);
                }
                catch (const std::out_of_range &ex) {}
            }
            if (parameter == "RE") {
                try {
                    Model::ExternalFlow flow = nodes->at(i)->getExternalFlowByName(Model::RECHARGE);
                    double val = flow.getRecharge().value() * mult;
                    nodes->at(i)->updateUniqueFlow(val, Model::RECHARGE);
                }
                catch (const std::out_of_range &ex) {}
            }
            if (parameter == "River_Mult") {
                try {
                    nodes->at(i)->scaleDynamicRivers(mult);
                }
                catch (const std::out_of_range &ex) {}
            }
            if (parameter == "AqThickness") {
                quantity<Model::Meter> d = nodes->at(i)->getProperties().get<quantity<Model::Meter>,
                        Model::VerticalSize>();
                nodes->at(i)->getProperties().set<quantity<Model::Meter>, Model::VerticalSize>(d * mult);
            }
            if (parameter == "SWB_Elevation") {
                try {
                    nodes->at(i)->updateExternalFlowFlowHead(mult, Model::GLOBAL_WETLAND);
                } catch (const std::out_of_range &ex) {}
                try {
                    nodes->at(i)->updateExternalFlowFlowHead(mult, Model::WETLAND);
                } catch (const std::out_of_range &ex) {}
                try {
                    nodes->at(i)->updateExternalFlowFlowHead(mult, Model::LAKE);
                } catch (const std::out_of_range &ex) {}
                try {
                    nodes->at(i)->updateExternalFlowFlowHead(mult, Model::RIVER_MM);
                } catch (const std::out_of_range &ex) {}
            }
        }
        return ids;
    }

        /**
         * Scale values for sensitivity analysis
         * @param path
         */
        void readMultipliers(std::string path) {
            io::CSVReader<2, io::trim_chars<' ', '\t'>, io::no_quote_escape<','>> in(path);
            in.read_header(io::ignore_no_column, "name", "val");
            std::string name;
            double value;
            while (in.read_row(name, value)) {
	 	value = pow(10, value);
            	NANChecker(value, "Sensitivity multiplier");
                if (name == "K") {
                    scaleByFunction([](const std::unique_ptr<Model::NodeInterface> &node) {
                        return true;
                    }, "K", value);
                } else if (name == "RE") {
                    scaleByFunction([](const std::unique_ptr<Model::NodeInterface> &node) {
                        return true;
		    }, "RECHARGE", value);
                } else if (name == "K_Wetlands") {
                    scaleByFunction([](const std::unique_ptr<Model::NodeInterface> &node) {
                        return true;
                    }, "K_Wetlands", value);
                } else if (name == "K_Lakes") {
                    scaleByFunction([](const std::unique_ptr<Model::NodeInterface> &node) {
                        return true;
                    }, "K_Lakes", value);
                } else if (name == "Lakes_bottom") {
                    scaleByFunction([](const std::unique_ptr<Model::NodeInterface> &node) {
                        return true;
                    }, "Lakes_Bottom", value);
                } else if (name == "River_Mult") {
                    scaleByFunction([](const std::unique_ptr<Model::NodeInterface> &node) {
                        return true;
                    }, "Dyn_River_Mult", value);
                } else if (name == "AqThickness") {
                    scaleByFunction([](const std::unique_ptr<Model::NodeInterface> &node) {
                        return true;
                    }, "Depth0", value);
                }
            }
        }

        /**
         * Get the residuals of the current iteration
         * @param path
         */
        void writeResiduals(std::string path) {
            Eigen::VectorXd vector = eq->getResiduals();
            std::ofstream ofs;
            ofs.open(path, std::ofstream::out | std::ofstream::trunc);
            ofs << "X,Y,data" << "\n";
            for (int i = 0; i < vector.size(); ++i) {
                //i is node position ->get X and Y
                auto lat = nodes->at(i)->getProperties().get<double, Model::Lat>();
                auto lon = nodes->at(i)->getProperties().get<double, Model::Lon>();
                double val = vector[i];
                ofs << lat << "," << lon << "," << val << "\n";
            }
            ofs.close();
        }


        /**
         * Coupling function for WaterGAP
         * @note Under development
         * @param field
         * @param month
         * @param numberOfGridCells
         */
        template<typename DataArray>
        void updateGWRechargeFromWaterGAP(DataArray field, short month, int numberOfGridCells) {
            double amount = 0;
            std::vector<int> tmp;
            //Resolution Iterator is more bautiful but still very inefficient :/
            //#pragma omp parallel for !currently breaks code!
            for (int k = 0; k < numberOfGridCells; ++k) {
                try {
                    tmp = reader->getArcIDMapping().at(k);
                }
                catch (const std::out_of_range &ex) {
                    //should not happen
                    continue;
                }

                //All GW nodes associated with that waterGAP cell
                for (int id : tmp) {
                    int i = 0;
                    try {
                        i = reader->getGlobIDMapping().at(id);
                    }
                    catch (const std::out_of_range &ex) {
                        //if Node does not exist ignore entry
                        LOG(critical) << "Should not happen";
                        continue;
                    }
                    amount = calcRecharge(field->operator[](k)[month],
                                          nodes->at(i)->getProperties().get<quantity<Model::SquareMeter>,
                                          Model::Area>().value());
                    nodes->at(i)->updateUniqueFlow(amount, Model::RECHARGE);
                }
            }
        };

    private:

        int initNodes();

        bool serialize = false;
        bool loadNodes = false;
};

}
}//ns
#endif

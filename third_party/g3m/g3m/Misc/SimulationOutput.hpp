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
#ifndef GLOBAL_FLOW_SIMULATION_OUTPUT_H
#define GLOBAL_FLOW_SIMULATION_OUTPUT_H

#include "../../Model/Units.hpp"

/**
 * @deprecated use methods from DataProcessing
 */

namespace GlobalFlow {
using NodeVector = std::shared_ptr <std::vector<std::unique_ptr < Model::NodeInterface>>>;
namespace Simulation {
class SimulationOutput {
    public:

        SimulationOutput(int field_count) : field_count(field_count) {}

        template<typename Query, class ...FieldNames>
        void
        query(NodeVector nodes, Query query, FieldNames...names);

    private:

        const int field_count;

        vector<int> nodeIDs;
        vector <string> field_names;
        NodeVector nodes;

        template<class ...Names>
        void
        set_field_names(std::string s, Names...cols) {
            field_names.push_back(s);
            set_field_names(std::forward<Names>(cols)...);
        }

        //Needed for recursive def of template function
        void
        set_field_names() {}

        template<typename Query>
        vector<int>
        search(Query searchFor) {
            vector<int> out;

            for_each(nodes->begin(), nodes->end(),
                     [&out, &searchFor](std::unique_ptr <Model::NodeInterface> const &node) {
                         if (searchFor(node)) {
                             out.push_back(node->getProperties().get<long, Model::ID>());
                         }
                     });
            return out;
        }

        friend ostream &
        operator<<(ostream &os, SimulationOutput const &simulation) {
            int p = 0;
            for (string field : simulation.field_names) {
                os << field;
                if (p < simulation.field_count - 1) {
                    os << ",";
                    p++;
                }
            }
            os << "\n";
            for (int i : simulation.nodeIDs) {
                p = 0;
                for (string field : simulation.field_names) {
                    if (field == "ID") {
                        os << simulation.nodes->at(i)->getProperties().get<long, Model::ID>();
                    }
                    if (field == "ArcID") {
                        os << simulation.nodes->at(i)->getProperties().get<long, Model::ArcID>();
                    }
                    if (field == "Area") {
                        os << simulation.nodes->at(i)->getProperties().get < Model::quantity < Model::SquareMeter > ,
                                Model::Area > ().value();
                    }
                    if (field == "Conduct") {
                        os << simulation.nodes->at(i)->getK().value();
                    }
                    if (field == "Elevation") {
                        os << simulation.nodes->at(i)->getProperties().get < Model::quantity < Model::Meter > ,
                                Model::Elevation > ().value();
                    }
                    if (field == "Slope") {
                        os << simulation.nodes->at(i)->getProperties().get < Model::quantity < Model::Dimensionless > ,
                                Model::Slope > ().value();
                    }
                    if (field == "X") {
                        os << simulation.nodes->at(i)->getProperties().get<double, Model::Lat>();
                    }
                    if (field == "Y") {
                        os << simulation.nodes->at(i)->getProperties().get<double, Model::Lon>();
                    }
                    if (field == "Head") {
                        os << simulation.nodes->at(i)->getProperties().get < Model::quantity < Model::Meter > ,
                                Model::Head > ().value();
                    }
                    if (field == "IN") {
                        os << simulation.nodes->at(i)->getIN().value();
                    }
                    if (field == "OUT") {
                        os << simulation.nodes->at(i)->getOUT().value();
                    }
                    if (field == "EqFlow") {
                        os << simulation.nodes->at(i)->getEqFlow().value();
                    }
                    if (field == "LATERAL_FLOWS") {
                        os << (simulation.nodes->at(i)->getLateralFlows().value() /
                               simulation.nodes->at(
                                       i)->getProperties().get < Model::quantity < Model::SquareMeter > , Model::Area >
                                                                                                          ().value()) *
                              1000;
                    }
                    if (field == "EqHead") {
                        os << simulation.nodes->at(i)->getProperties().get < Model::quantity < Model::Meter > ,
                                Model::EQHead > ().value();
                    }
                    if (field == "Wetlands") {
                        if (simulation.nodes->at(i)->hasTypeOfExternalFlow(Model::WETLAND)) {
                            os << 1;
                        } else {
                            os << 0;
                        }
                    }
                    if (field == "Lakes") {
                        if (simulation.nodes->at(i)->hasTypeOfExternalFlow(Model::LAKE)) {
                            os << 1;
                        } else {
                            os << 0;
                        }
                    }
                    if (field == "FlowHead") {
                        double out{NAN};
                        try {
                            out = simulation.nodes->at(i)->getExternalFlowByName(Model::RIVER_MM).getFlowHead().value();
                        }
                        catch (exception &e) {
                        }
                        os << out;
                    }
                    if (field == "Recharge") {
                        double out{NAN};
                        try {
                            out = simulation.nodes->at(i)->getExternalFlowVolumeByName(Model::RECHARGE).value();
                            out = (out / simulation.nodes->at(
                                    i)->getProperties().get < Model::quantity < Model::SquareMeter > , Model::Area >
                                                                                                       ().value()) *
                                  1000;
                        }
                        catch (exception &e) {
                        }
                        os << out;
                    }
                    if (field == "EqFlow+Recharge") {
                        double out{NAN};
                        try {
                            out = (simulation.nodes->at(i)->getEqFlow().value()
                                   + simulation.nodes->at(i)->getExternalFlowByName(
                                    Model::RECHARGE).getRecharge().value());
                        }
                        catch (exception &e) {
                        }
                        os << out;
                    }
                    if (field == "DynamicRiverConduct") {
                        double out{NAN};
                        try {
                            out =
                                    simulation.nodes->at(i)->getExternalFlowByName(Model::RIVER_MM).getDyn(
                                            simulation.nodes->at(i)->getExternalFlowVolumeByName(Model::RECHARGE),
                                            simulation.nodes->at(i)->getProperties().get < Model::quantity <
                                            Model::Meter > , Model::EQHead > (),
                                            simulation.nodes->at(
                                                    i)->getProperties().get < Model::quantity < Model::Meter > ,
                                            Model::Head > (),
                                            simulation.nodes->at(i)->getEqFlow()
                                    ).value();
                        }
                        catch (exception &e) {

                        }
                        os << out;
                    }
                    if (field == "DynamicDrainConduct") {
                        double out{NAN};
                        try {
                            out =
                                    simulation.nodes->at(i)->getExternalFlowByName(Model::DRAIN).getDyn(
                                            simulation.nodes->at(i)->getExternalFlowVolumeByName(Model::RECHARGE),
                                            simulation.nodes->at(i)->getProperties().get < Model::quantity <
                                            Model::Meter > , Model::EQHead > (),
                                            simulation.nodes->at(
                                                    i)->getProperties().get < Model::quantity < Model::Meter > ,
                                            Model::Head > (),
                                            simulation.nodes->at(i)->getEqFlow()
                                    ).value();
                        }
                        catch (exception &e) {

                        }
                        os << out;
                    }
                    if (field == "RiverConduct") {
                        double out{NAN};
                        try {
                            out = simulation.nodes->at(i)->getExternalFlowByName(
                                    Model::RIVER_MM).getConductance().value();
                        }
                        catch (exception &e) {
                        }
                        os << out;
                    }
                    if (field == "NodeVelocity") {
                        std::pair<double, double> out = simulation.nodes->at(i)->getVelocityVector();
                        os << out.first << ";" << out.second;
                    }
                    if (field == "RIVER_IN") {
                        double out{0};
                        try {
                            out += simulation.nodes->at(i)->getExternalFlowVolumeByName(Model::RIVER_MM).value();
                            //out += simulation.nodes->at(i)->getExternalFlowVolumeByName(Model::RIVER).value();
                            out = (out / simulation.nodes->at(
                                    i)->getProperties().get < Model::quantity < Model::SquareMeter > , Model::Area >
                                                                                                       ().value()) *
                                  1000;
                        }
                        catch (exception &e) {
                        }
                        if (out < 0) {
                            out = 0;
                        }

                        os << out;
                    }
                    if (field == "RIVER_OUT") {
                        double out{0};
                        try {
                            out += simulation.nodes->at(i)->getExternalFlowVolumeByName(Model::RIVER_MM).value();
                            //out += simulation.nodes->at(i)->getExternalFlowVolumeByName(Model::RIVER).value();
                            out = (out / simulation.nodes->at(
                                    i)->getProperties().get < Model::quantity < Model::SquareMeter > , Model::Area >
                                                                                                       ().value()) *
                                  1000;
                        }
                        catch (exception &e) {
                        }
                        if (out > 0) {
                            out = 0;
                        }

                        os << out;
                    }
                    if (field == "drain_OUT") {
                        double out{0};
                        try {
                            out += simulation.nodes->at(i)->getExternalFlowVolumeByName(Model::DRAIN).value();
                            //out += simulation.nodes->at(i)->getExternalFlowVolumeByName(Model::RIVER).value();
                            out = (out / simulation.nodes->at(
                                    i)->getProperties().get < Model::quantity < Model::SquareMeter > , Model::Area >
                                                                                                       ().value()) *
                                  1000;
                        }
                        catch (exception &e) {
                        }
                        if (out > 0) {
                            out = 0;
                        }

                        os << out;
                    }
                    if (field == "wetlands_OUT") {
                        double out{0};
                        try {
                            out += simulation.nodes->at(i)->getExternalFlowVolumeByName(Model::WETLAND).value();
                            out = (out / simulation.nodes->at(
                                    i)->getProperties().get < Model::quantity < Model::SquareMeter > , Model::Area >
                                                                                                       ().value()) *
                                  1000;
                        }
                        catch (exception &e) {
                        }
                        if (out > 0) {
                            out = 0;
                        }
                        os << out;
                    }
                    if (field == "global_wetlands_OUT") {
                        double out{0};
                        try {
                            out += simulation.nodes->at(i)->getExternalFlowVolumeByName(Model::GLOBAL_WETLAND).value();
                            out = (out / simulation.nodes->at(
                                    i)->getProperties().get < Model::quantity < Model::SquareMeter > , Model::Area >
                                                                                                       ().value()) *
                                  1000;
                        }
                        catch (exception &e) {
                        }
                        if (out > 0) {
                            out = 0;
                        }
                        os << out;
                    }
                    if (field == "global_wetlands_IN") {
                        double out{0};
                        try {
                            out += simulation.nodes->at(i)->getExternalFlowVolumeByName(Model::GLOBAL_WETLAND).value();
                            out = (out / simulation.nodes->at(
                                    i)->getProperties().get < Model::quantity < Model::SquareMeter > , Model::Area >
                                                                                                       ().value()) *
                                  1000;
                        }
                        catch (exception &e) {
                        }
                        if (out < 0) {
                            out = 0;
                        }
                        os << out;
                    }
                    if (field == "ocean_OUT") {
                        double out{0};
                        try {
                            out += simulation.nodes->at(i)->getExternalFlowVolumeByName(
                                    Model::GENERAL_HEAD_BOUNDARY).value();
                            out = (out / simulation.nodes->at(
                                    i)->getProperties().get < Model::quantity < Model::SquareMeter > , Model::Area >
                                                                                                       ().value()) *
                                  1000;
                        }
                        catch (exception &e) {
                        }
                        if (out > 0) {
                            out = 0;
                        }
                        os << out;
                    }
                    if (field == "lakes_OUT") {
                        double out{0};
                        try {
                            out += simulation.nodes->at(i)->getExternalFlowVolumeByName(Model::LAKE).value();
                            out = (out / simulation.nodes->at(
                                    i)->getProperties().get < Model::quantity < Model::SquareMeter > , Model::Area >
                                                                                                       ().value()) *
                                  1000;
                        }
                        catch (exception &e) {
                        }
                        if (out > 0) {
                            out = 0;
                        }
                        os << out;
                    }
                    if (field == "BELOW") {
                        int out{0};
                        out = simulation.nodes->at(i)->getExternalFlowByName(Model::RIVER_MM).flowIsHeadDependant(
                                simulation.nodes->at(i)->getProperties().get < Model::quantity < Model::Meter > ,
                                Model::Head > ());
                        os << out;
                    }
                    if (field == "NumOfNeighbours") {
                        os << simulation.nodes->at(i)->getNumofNeighbours();
                    }
                    if (field == "DiffToRiverElevation") {
                        double out{NAN};
                        try {
                            out =
                                    simulation.nodes->at(i)->getExternalFlowByName(Model::RIVER_MM).getRiverDiff(
                                            simulation.nodes->at(
                                                    i)->getProperties().get < Model::quantity < Model::Meter > ,
                                            Model::EQHead > ()).value();
                        }
                        catch (exception &e) {
                        }
                        os << out;
                    }
                    if (p < simulation.field_count - 1) {
                        os << ",";
                        p++;
                    }
                }
                os << "\n";
            }
            return os;
        };
};

template<typename Query, class ...FieldNames>
void SimulationOutput::query(NodeVector nodes, Query query, FieldNames... names) {
    this->nodes = nodes;
    assert(sizeof...(names) != (field_count + 1) && "Number of specified fields does not match");
    field_names.reserve(field_count);
    set_field_names(std::forward<FieldNames>(names)...);
    nodeIDs = search(query);
}
}
}//ns

#endif

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

#ifndef GLOBAL_FLOW_OUTPUT_MANAGER_HPP
#define GLOBAL_FLOW_OUTPUT_MANAGER_HPP

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "boost/variant.hpp"

#include "FieldCollector.hpp"
#include "OutputFactory.hpp"

namespace GlobalFlow {
    namespace DataProcessing {
        namespace DataOutput {

            using path = std::string;
            using pos_v = std::vector<std::pair<double, double>>;

            class Outputvisitor : public boost::static_visitor<> {
            public:
                template<typename T>
                void operator()(T &operand) const {
                    operand.write();
                }
            };

/**
 * @class Abstract representation of a data output
 */
            template<typename T>
            class OutputField {
            private:
                const path filePath;
                const bool printID;
                const bool printXY;
                const OutputType outputType;
                const std::vector<T> data;
                pos_v p;
                a_vector ids;

            public:
                OutputField(path filePath, bool printID, bool printXY, Simulation::Simulation const &sim,
                            OutputType outputType,
                            FieldType fieldType)
                        : filePath(filePath), printID(printID), printXY(printXY),
                          outputType(outputType),
                          data(FieldCollector(fieldType).get<T>(sim)), p(FieldCollector().getPositions(sim)),
                          ids(FieldCollector().getIds(sim)) {}

                void write() {
                    OutputInterface<T> *oi = OutputFactory<T>().getOutput(outputType);
                    oi->write(filePath, printID, printXY, data, p, ids);
                };
            };

            using field_vector = std::vector<boost::variant<
                    OutputField<double>,
                    OutputField<bool>,
                    OutputField<std::string>,
                    OutputField<std::pair<double, double>>
            >>;

            enum class InternalType {
                STRING, DOUBLE, VECTOR, BOOL
            };

            struct InternalTypeHash {
                template<typename T>
                std::size_t operator()(T t) const {
                    return static_cast<std::size_t>(t);
                }
            };

            const std::unordered_map<FieldType, InternalType, InternalTypeHash> typeMapping{
                    {FieldType::ID,                 InternalType::DOUBLE},
                    {FieldType::ARCID,              InternalType::DOUBLE},
                    {FieldType::AREA,               InternalType::DOUBLE},
                    {FieldType::CONDUCT,            InternalType::DOUBLE},
                    {FieldType::ELEVATION,          InternalType::DOUBLE},
                    {FieldType::SLOPE,              InternalType::DOUBLE},
                    {FieldType::X,                  InternalType::DOUBLE},
                    {FieldType::Y,                InternalType::DOUBLE},
                    {FieldType::HEAD,             InternalType::DOUBLE},
                    {FieldType::EQ_HEAD,          InternalType::DOUBLE},
                    {FieldType::IN,               InternalType::DOUBLE},
                    {FieldType::OUT,              InternalType::DOUBLE},
                    {FieldType::EQ_FLOW,          InternalType::DOUBLE},
                    {FieldType::LATERAL_FLOW,     InternalType::DOUBLE},
                    {FieldType::LATERAL_OUT_FLOW, InternalType::DOUBLE},
                    {FieldType::WETLANDS,         InternalType::DOUBLE},
                    {FieldType::LAKES,            InternalType::DOUBLE},
                    {FieldType::FLOW_HEAD,        InternalType::DOUBLE},
                    {FieldType::RECHARGE,         InternalType::DOUBLE},
                    {FieldType::DYN_RIVER,        InternalType::DOUBLE},
                    {FieldType::NODE_VELOCITY,    InternalType::VECTOR},
                    {FieldType::RIVER_OUT,        InternalType::DOUBLE},
                    {FieldType::RIVER_IN,           InternalType::DOUBLE},
                    {FieldType::WTD,                InternalType::DOUBLE},
                    {FieldType::RIVER_CONDUCT,      InternalType::DOUBLE},
                    {FieldType::DRAIN_CONDUCT,      InternalType::DOUBLE},
                    {FieldType::WETLAND_CONDUCT,    InternalType::DOUBLE},
                    {FieldType::GL_WETLAND_CONDUCT, InternalType::DOUBLE},
                    {FieldType::LAKE_CONDUCT,       InternalType::DOUBLE},
                    {FieldType::OCEAN_OUT,          InternalType::DOUBLE},
                    {FieldType::GL_WETLAND_OUT,     InternalType::DOUBLE},
                    {FieldType::WETLAND_OUT,        InternalType::DOUBLE},
                    {FieldType::LAKE_OUT,           InternalType::DOUBLE},
                    {FieldType::GL_WETLAND_IN,      InternalType::DOUBLE},
                    {FieldType::WETLAND_IN,         InternalType::DOUBLE},
                    {FieldType::LAKE_IN,            InternalType::DOUBLE}
            };

            class FieldFactory {
            private:
                static FieldType getTemplateType(std::string type) {
                    FieldType fieldType{FieldType::NON_VALID};
                    try { fieldType = fieldMapping.at(type); }
                    catch (exception &e) { LOG(error) << "No such field" << type; }
                    return fieldType;
                }

                static OutputType getTemplateOutput(std::string field) {
                    OutputType outputType{OutputType::NON_VALID};
                    try { outputType = outputMapping.at(field); }
                    catch (exception &e) { LOG(error) << "No such output type" << field; }
                    return outputType;
                }

            public:
                /**
                 *
                 * @param tree
                 */
                static field_vector build(boost::property_tree::ptree tree, Simulation::Simulation const &sim) {
                    field_vector f;
                    BOOST_FOREACH(const boost::property_tree::ptree::value_type &child, tree) {
                                    assert(child.first.empty());
                                    boost::property_tree::ptree item = child.second.get_child("");

                                    auto type = getTemplateOutput(item.get<std::string>("type"));
                                    auto field = getTemplateType(item.get<std::string>("field"));

                                    std::string name = item.get<std::string>("name");
                                    bool id = item.get<bool>("ID");
                                    bool pos = item.get<bool>("position");
                                    switch (typeMapping.at(field)) {
                                        case InternalType::DOUBLE : {
                                            f.emplace_back(OutputField<double>(name, id, pos, sim, type, field));
                                            break;
                                        }
                                        case InternalType::STRING : {
                                            f.emplace_back(OutputField<std::string>(name, id, pos, sim, type, field));
                                            break;
                                        }
                                        case InternalType::VECTOR : {
                                            f.emplace_back(
                                                    OutputField<std::pair<double, double>>(name, id, pos, sim, type,
                                                                                           field));
                                            break;
                                        }
                                        case InternalType::BOOL :
                                            f.emplace_back(OutputField<bool>(name, id, pos, sim, type, field));
                                            break;
                                    }
                                }
                    return f;
                };
            };

/**
 *
 */

            class OutputManager {
            private:
                field_vector fields;
                const Simulation::Simulation &sim;

                /**
                 * Read specification
                 */
                void readJSON(path o_path) {
                    boost::property_tree::ptree tree;
                    boost::property_tree::read_json(o_path, tree);
                    tree = tree.get_child("output");

                    boost::property_tree::ptree stat = tree.get_child("StaticResult");
                    fields = FieldFactory().build(stat, sim);
                }

            public:
                OutputManager(path output_spec_path, Simulation::Simulation const &sim) : sim(sim) {
                    readJSON(output_spec_path);
                }

                /**
                 * @brief Visits all registered output options and triggers write
                 */
                void write() {
                    Outputvisitor visitor;
                    std::for_each(
                            fields.begin(), fields.end(), boost::apply_visitor(visitor)
                    );
                }
            };
        }
    }
}
#endif
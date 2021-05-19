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

#ifndef GLOBAL_FLOW_NODE_HPP
#define GLOBAL_FLOW_NODE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <numeric>
#include <future>
#include <forward_list>
#include "ExternalFlows.hpp"
#include "FluidMechanics.hpp"
#include "PhysicalProperties.hpp"
#include "../Misc/Helpers.hpp"

namespace GlobalFlow {
namespace Model {
/**
 * Neighbouring positions for cells
 *     TOP
 * LEFT * RIGHT
 *     DOWN
 *
 * In Z (Top view):
 * FRONT (larger ID) * BACK (smaler ID)
 */
enum NeighbourPosition {
    TOP = 1,
    DOWN,
    RIGHT,
    LEFT,
    FRONT,
    BACK
};
}
}

namespace std {
template<>
struct hash<GlobalFlow::Model::NeighbourPosition> {
    typedef GlobalFlow::Model::NeighbourPosition argument_type;
    typedef std::underlying_type<argument_type>::type underlying_type;
    typedef std::hash<underlying_type>::result_type result_type;

    result_type
    operator()(const argument_type &arg) const {
        std::hash<underlying_type> hasher;
        return hasher(static_cast< underlying_type >( arg ));
    }
};
}

namespace GlobalFlow {
namespace Model {

//using namespace std;
using namespace boost::units;

class NodeInterface;

using p_node = std::unique_ptr<GlobalFlow::Model::NodeInterface>;
using NodeVector = std::shared_ptr<std::vector<std::unique_ptr<GlobalFlow::Model::NodeInterface>>>;

/**
 * Interface defining required fields for a node.
 * A node is the central comutational and spatial unit.
 * A simulated area is seperated into a discrete raster of cells or nodes
 * (seperate computational units which stay in contact to ech other).
 * Is equal to 'cell'.
 *
 * Nodes can be of different physical property e.g. different size.
 */
class NodeInterface {
    private:
        virtual void
        __setHead(t_meter head) = 0;

        virtual t_meter
        __calcInitialHead(t_meter initialParam) = 0;

        virtual bool
        __isStaticNode() = 0;

        /**
         * @brief Calculates e-folding depth from input data.
         * @param z The vertical size of the computing cell.
         * @return A dimensionless factor that can be used to modify
         * hydraulic conductance depending on depth.
         *
         * E-Folding function as defined by Ying Fan et al. e^(-(Depth - Factor)).
         */
        t_dim efoldingFromData(t_meter z) {
            t_meter folding = get<t_meter, EFolding>();
            if (folding == 0.0 * si::meter)
                return 1 * si::si_dimensionless;
            z = (z / (2 * si::si_dimensionless));
            t_dim out = exp(-z / folding);
            if (out == 0 * si::si_dimensionless)
                return 1e-7 * si::si_dimensionless;
            return out;
        }

        /**
         * @brief Apply function to all layers of the model.
         * @param &&...p A list of functions forwarded to setAttribute.
         *
         * Apply an atribute to all layers at same position as the node.
         * A function passed in addition modifies the attribute depending on the layer.
         * Stops on last layer.
         */
        template<class... Params>
        void
        applyToAllLayers(Params &&...p) {
            try {
                this->getNeighbour(DOWN)->setAttribute(std::forward<Params>(p)...);
            } catch (...) {} //We are on last layer nothing to do
        };

        template<class ApplyFunction>
        void
        setAttribute(ApplyFunction fun) {
            fun(this);
            applyToAllLayers(std::forward<ApplyFunction>(fun));
        };

    public:
        const std::shared_ptr<std::vector<std::unique_ptr<NodeInterface>>> nodes;
        std::unordered_map<NeighbourPosition, large_num> neighbours;
        std::unordered_map<FlowType, ExternalFlow, FlowTypeHash> externalFlows;
        int numOfExternalFlows{0};
        bool nwt{false};
        bool initial_head{true};
        bool simpleDistance{false};
        bool simpleK{false};
        bool steadyState{false};

        friend class FluidMechanics;

        FluidMechanics mechanics;
        PhysicalProperties fields;

        /*
        * Short helper functions to make the code more readable
        */
        template<typename T, typename F>
        T get() { return fields.get<T, F>(); }

        template<typename T, typename F>
        void set(const T &value) { return fields.set<T, F>(value); }

        template<typename T, typename F>
        T getFrom(NodeInterface *nodeInterface, NeighbourPosition pos) {
            return nodeInterface->getNeighbour(pos)->getProperties().get<T, F>();
        }

        using map_itter = std::unordered_map<NeighbourPosition, large_num>::const_iterator;

        p_node &at(map_itter pos) { return nodes->at(pos->second); }

        template<typename T, typename F>
        T getAt(map_itter pos) {
            return at(pos)->get<T, F>();
        }

        /**
         * @brief Uses specific storage to caluclate storativity
         * @return Flow budget for cell depending on head change
         */
        t_s_meter getStorageCapacity__Primary() noexcept {
            t_s_meter out = get<quantity<perUnit>, SpecificStorage>().value()
                            * get<t_c_meter, VolumeOfCell>().value() * si::square_meter;
            return out;
        }

        /**
         * @brief Uses specific yield to caluclate storativity
         * @return Flow budget for cell depending on head change
         */
        t_s_meter getStorageCapacity__Secondary() noexcept {
            return get<t_dim, SpecificYield>() * get<t_s_meter, Area>();
        }

        /**
         * @brief Uses specific yield with NWT smoother
         * @return Flow budget for cell depending on head change
         * Assumes that smooth function is linear
         */
        t_s_meter getStorageCapacity__SecondaryNWT() noexcept {
            return getStorageCapacity__Secondary()
                   * mechanics.smoothFunction__NWT(get<t_meter, Elevation>(), get<t_meter, VerticalSize>(),
                                                   get<t_meter, Head>())
                   + getStorageCapacity__Secondary();
        }

        /**
         * @brief Flow volume of cell
         * @return Flow volume
         */
        template<typename CompareFunction>
        t_vol_t getFlow(CompareFunction compare) noexcept {
            t_vol_t flow_v = 0.0 * si::cubic_meter / day;
            t_vol_t stFlow = 0.0 * si::cubic_meter / day;
            if (not steadyState) {
                stFlow = getTotalStorageFlow();
                if (compare(stFlow.value()))
                    flow_v += boost::units::abs(stFlow);
            }
            flow_v += getNonStorageFlow(std::forward<CompareFunction>(compare));
            return flow_v.value() * si::cubic_meter / day;
        }

    public:
        /**
         * @brief Constructor of abstract class NodeInterface
         * @param nodes Vector of all other existing nodes
         * @param lat The latitude
         * @param lon The Longitude
         * @param area Area in m²
         * @param ArcID Unique ARC-ID specified by Kassel
         * @param ID Internal ID = Position in vector
         * @param K Hydraulic conductivity in meter/day (default)
         * @param stepModifier Modfies default step size of day (default=1)
         * @param aquiferDepth Vertical size of the cell
         * @param anisotropy Modifier for vertical conductivity based on horizontal
         * @param specificYield Yield of storage for dewatered conditions
         * @param specificStorage Specific storage - currently for confined and unconfined
         * @param confined Is node in a confined layer
         *
         */
        NodeInterface(NodeVector nodes,
                      double lat,
                      double lon,
                      t_s_meter area,
                      large_num ArcID,
                      large_num ID,
                      t_vel K,
                      int stepModifier,
                      double aquiferDepth,
                      double anisotropy,
                      double specificYield,
                      double specificStorage,
                      bool confined);

        virtual ~NodeInterface() = default;

        large_num getID() { return get<large_num, ArcID>(); }

/*****************************************************************
Modify Properties
******************************************************************/

        /**
         * @brief Set elevation on top layer and propagate to lower layers
         * @param elevation The top elevation (e.g. from DEM)
         */
        void setElevation(t_meter elevation) {
            set < t_meter, Elevation > (elevation);
            set < t_meter, TopElevation > (elevation);
            applyToAllLayers([this](NodeInterface *node) {
                try {
                    node->getProperties().set<t_meter, TopElevation>(getFrom<t_meter, TopElevation>(node, TOP));
                    node->getProperties().set<t_meter, Elevation>(
                            getFrom<t_meter, Elevation>(node, TOP) - getFrom<t_meter, VerticalSize>(node, TOP));
                }
                catch (...) {}
            });
        };

        /**
         * @brief Set slope from data on all layers
         * Slope input is in % but is required as absolut
         * thus: slope = sloper_percent / 100
         * @param slope
         */
        void
        setSlope(double slope_percent) {
            set < t_dim, Slope > ((slope_percent / 100) * si::si_dimensionless);
            applyToAllLayers([slope_percent](NodeInterface *nodeInterface) {
                try {
                    nodeInterface->setSlope(slope_percent);
                }
                catch (...) {}
            });
        };

        /**
         * @brief Set e-folding factor from data on all layers
         * @param e-fold
         */
        void setEfold(double efold) {
            set < t_meter, EFolding > (efold * si::meter);
            applyToAllLayers([efold](NodeInterface *nodeInterface) {
                try { nodeInterface->setEfold(efold); }
                catch (...) {}
            });
        };

        /**
         * @brief Calculated equilibrium groundwater-head from eq_wtd
         * Assumes that if initialhead = false that the eq_head is also used as initial head
         * @param head
         */
        void setEqHead(t_meter wtd) {
            t_meter eqhead = get<t_meter, Elevation>() - wtd;
            //if (eqhead.value() > 500) {
            //	eqhead = 500 * si::meter;
            //}
            set < t_meter, EQHead > (eqhead);
            setHead_direct(eqhead.value());
            //setHead_direct(get<t_meter, Elevation>().value());
            applyToAllLayers([eqhead](NodeInterface *nodeInterface) {
                try {
                    nodeInterface->setHead_direct(eqhead.value());
                    nodeInterface->set<t_meter, EQHead>(eqhead);
                }
                catch (...) {}
            });
        }

        /**
         * Calculated equilibrium flow to neighbouring cells
         * Static thus calculated only once.
         *
         * Depends on: K in cell and eq_head in all 6 neighbours
         */
        bool cached{false};
        t_vol_t eq_flow{0 * si::cubic_meter / day};

        template<class HeadType>
        FlowInputHor createDataTuple(map_itter got) {
            return std::make_tuple(at(got)->getK(),
                                   getK(),
                                   getAt<t_meter, EdgeLenght>(got),
                                   get<t_meter, EdgeLenght>(),
                                   getAt<t_meter, HeadType>(got),
                                   get<t_meter, HeadType>(),
                                   getAt<t_meter, Elevation>(got),
                                   get<t_meter, Elevation>(),
                                   getAt<t_meter, VerticalSize>(got),
                                   get<t_meter, VerticalSize>(),
                                   get<bool, Confinement>());
        }

        FlowInputVert createDataTuple(map_itter got) {
            return std::make_tuple(at(got)->getK_vertical(),
                                   getK_vertical(),
                                   get<t_meter, VerticalSize>(),
                                   get<t_meter, Head>(),
                                   get<t_meter, Elevation>(),
                                   get<t_s_meter, Area>(),
                                   getAt<t_meter, Elevation>(got),
                                   getAt<t_meter, VerticalSize>(got),
                                   getAt<t_meter, Head>(got),
                                   get<bool, Confinement>());
        }


        /**
         * Calculate the lateral groundwater flow to the neighbouring nodes
         * Generic function used for calulating equlibrium and current step flow
         * @return
         */
        template<class HeadType>
        t_vol_t calcLateralFlows(bool onlyOut) {
            t_vol_t lateral_flow{0 * si::cubic_meter / day};
            std::forward_list<NeighbourPosition> possible_neighbours =
                    {NeighbourPosition::BACK, NeighbourPosition::FRONT, NeighbourPosition::LEFT,
                     NeighbourPosition::RIGHT};

            for (const auto &position: possible_neighbours) {
                std::unordered_map<NeighbourPosition, large_num>::const_iterator got = neighbours.find(position);
                if (got == neighbours.end()) {//No neighbouring node
                } else {
                    //There is a neighbour node
                    t_s_meter_t conductance = mechanics.calculateHarmonicMeanConductance(
                            createDataTuple<HeadType>(got));
                    t_vol_t flow = conductance * (get<t_meter, HeadType>() - getAt<t_meter, HeadType>(got));
                    if (onlyOut) {
                        if (flow.value() > 0) { lateral_flow = lateral_flow - flow; }
                    } else { lateral_flow = lateral_flow - flow; }
                }
            }
            return lateral_flow;
        }

        /**
         * Calculate the equlibrium lateral flows
         * @return eq lateral flow
         */
        t_vol_t getEqFlow() noexcept {
            if (not cached) {
                t_vol_t lateral_flow = calcLateralFlows<EQHead>(false);
                NANChecker(lateral_flow.value(), "Eq Flow");
                eq_flow = lateral_flow;
                cached = true;
            }
            return eq_flow;
        }

        /**
         * Get the current lateral flow
         * @return
         */
        t_vol_t getLateralFlows() {
            return calcLateralFlows<Head>(false) * get<t_dim, StepModifier>();
        }

        /**
         * Get the current lateral out flows
         * @return
         */
        t_vol_t getLateralOutFlows() {
            return calcLateralFlows<Head>(true) * get<t_dim, StepModifier>();
        }


        /**
         * @brief Cuts off all heads above surface elevation
         * @warning Should only be used in spinn up phase!
         * @return Bool if node was reset
         */
        bool resetFloodingHead() noexcept {
            auto elevation = get<t_meter, Elevation>();
            if (get<t_meter, Head>() > elevation) {
                set < t_meter, Head > (elevation);
                return true;
            }
            return false;
        }

        /**
         * @brief Scales river conduct by 50%
         * @warning Should only be used in spinn up phase
         */
        void scaleRiverConduct() {
            eq_flow = eq_flow * 1.5;
        }

        /**
         * @brief Update the current head change (in comparison to last time step)
         * @note Should only be caled at end of timestep
         */
        void updateHeadChange() noexcept {
            set < t_meter, HeadChange_TZero > (
                    get<t_meter, Head>() - get<t_meter, Head_TZero>());
            set < t_meter, Head_TZero > (get<t_meter, Head>());
        }

        void initHead_t0() noexcept { set < t_meter, Head_TZero > (get<t_meter, Head>()); }

        void setHead_direct(double head) noexcept { set < t_meter, Head > (head * si::meter); }

        t_vel getK__pure() noexcept { return get<t_vel, K>(); }

        /**
         * @brief Get hydraulic conductivity
         * @return hydraulic conductivity (scaled by e-folding)
         */
        t_vel getK() noexcept {
            if (simpleK) { return get<t_vel, K>() * get<t_dim, StepModifier>(); }
            t_dim e_fold = 1 * si::si_dimensionless;
            if (get<int, Layer>() > 0) {
                e_fold = efoldingFromData(get<t_meter, VerticalSize>());
            }
            t_vel out = get<t_vel, K>() * e_fold * get<t_dim, StepModifier>();
            if (out < 1e-20 * si::meter / day) {
                out = 1e-20 * si::meter / day;
            }
            return out;
        }

        /**
         * @brief Get hydraulic vertical conductivity
         * @return hydraulic conductivity scaled by anisotropy (scaled by e-folding)
         */
        t_vel getK_vertical() noexcept { return (getK() / get<t_dim, Anisotropy>()) * get<t_dim, StepModifier>(); }

        /**
         * @brief Modify hydraulic conductivity (applied to all layers below)
         * @param New conductivity (if e-folding enabled scaled on layers)
         */
        void setK(t_vel conduct) {
            setK_direct(conduct);
            applyToAllLayers([&conduct](NodeInterface *nodeInterface) {
                nodeInterface->getProperties().set<t_vel, K>(conduct);
            });
        }

        /**
         * @brief Modify hydraulic conductivity (no e-folding, no layers)
         * @param New conductivity
         */
        void setK_direct(t_vel conduct) { set < t_vel, K > (conduct); }

        /**
         * @brief Get all outflow since simulation start
         */
        t_c_meter getOUT() noexcept { return get<t_c_meter, OUT>(); }

        /**
         * @brief Get all inflow since simulation start
         */
        t_c_meter getIN() noexcept { return get<t_c_meter, IN>(); }

        /**
         * @brief Toogle steady state simulation
         * @param onOFF true=on
         * Turns all storage equations to zero with no timesteps
         */
        void toogleStadyState(bool onOFF) { this->steadyState = onOFF; }

        void updateStepSize(double mod) { set < t_dim, StepModifier > (mod * si::si_dimensionless); }

        /**
         * @brief Storage capacity based on yield or specific storage
         * @return Potential flow budget when multiplied by head change
         * Uses an 0.001m epsilon to determine if a water-table condition is present.
         * If the layer is confined or not in water-table condition returns primary capacity.
         */
        t_s_meter getStorageCapacity() noexcept {
            t_meter epsilon = 0.001 * si::meter;
            if (get<bool, Confinement>()) { return getStorageCapacity__Primary(); }
            if (get<t_meter, Head>() + epsilon < get<t_meter, Elevation>()) {
                //water-table condition
                if (nwt) { return getStorageCapacity__SecondaryNWT(); }
                return getStorageCapacity__Secondary();
            } else {
                return getStorageCapacity__Primary();
            }
        }

        /**
         * @brief Get and external flow by its FlowType
         * @param type The flow type
         * @return Ref to external flow
         * @throw OutOfRangeException
         */
        ExternalFlow &getExternalFlowByName(FlowType type){
            if (externalFlows.find(type) == externalFlows.end())
                throw std::out_of_range("No such flow");
            return externalFlows.at(type);
        }

        /**
         * @brief Get and external flow volume by its FlowType
         * @param type The flow type
         * @return Flow volume
         */
        t_vol_t getExternalFlowVolumeByName(FlowType type) {
            for (const auto &flow : externalFlows) {
                if (type == flow.second.getType()) {
                    t_vol_t out = calculateExternalFlowVolume(flow.second);
                    return out;
                }
            }
            return 0 * si::cubic_meter / day;
        }

        /**
         * @brief Get flow budget based on head change
         * @return Flow volume
         * Note: Water entering storage is treated as an outflow (-), that is a loss of water from the flow system
         * while water released from storage is treated as inflow (+), that is a source of water to the flow system
         */
        t_vol_t getTotalStorageFlow() noexcept {
            return -getStorageCapacity() * get<t_meter, HeadChange_TZero>() / day * get<t_dim, StepModifier>();
        }

        /**
         * @brief Get flow budget of a specific external flows
         * @param &flow A external flow
         * @return Flow volume
         * Note: Water entering storage is treated as an outflow (-), that is a loss of water from the flow system
         * while water released from storage is treated as inflow (+), that is a source of water to the flow system
         */
        t_vol_t calculateExternalFlowVolume(const ExternalFlow &flow) {
            if (is(flow.getType()).in(RECHARGE, NET_ABSTRACTION)) {
                return flow.getRecharge() * get<t_dim, StepModifier>();
            }
            t_vol_t ex;
            t_meter eq_head = get<t_meter, EQHead>();
            t_meter head = get<t_meter, Head>();
            t_vol_t recharge = 0 * si::cubic_meter / day;
            try {
                recharge = getExternalFlowByName(RECHARGE).getRecharge();
            } catch (const std::out_of_range &e) {
                //ignore me there is no special_flow in this cell
            }
            t_dim slope = get<t_dim, Slope>();
            t_vol_t eqFlow = getEqFlow();
            if (is(flow.getType()).in(RIVER, DRAIN, RIVER_MM, LAKE, WETLAND, GLOBAL_WETLAND)) {
                if (flow.flowIsHeadDependant(head)) {
                    ex = flow.getP(eq_head, head, recharge, slope, eqFlow) * head * get<t_dim, StepModifier>()
                         + flow.getQ(eq_head, head, recharge, slope, eqFlow) * get<t_dim, StepModifier>();
                } else {
                    ex = (flow.getP(eq_head, head, recharge, slope, eqFlow) * flow.getBottom()
                          +
                          flow.getQ(eq_head, head, recharge, slope, eqFlow)) * get<t_dim, StepModifier>();
                }
            } else {
                ex = (flow.getP(eq_head, head, recharge, slope, eqFlow) * head +
                      flow.getQ(eq_head, head, recharge, slope, eqFlow)) * get<t_dim, StepModifier>();
            }
            return ex;
        }

        /**
         * @brief Caluclate dewatered flow
         * @return Flow volume per time
         * If a cell is dewatered but below a saturated or partly saturated cell:
         * this calculates the needed additional exchange volume
         */
        t_vol_t calculateDewateredFlow() noexcept {
            map_itter hasDown = neighbours.find(DOWN);
            map_itter hasUp = neighbours.find(TOP);
            t_vol_t out = 0 * si::cubic_meter / day;

            if (hasDown != neighbours.end()) {
                t_meter elev = getAt<t_meter, Elevation>(hasDown);
                t_meter head_n = getAt<t_meter, Head>(hasDown);
                //Check if a dewatered condition is present
                if (head_n < elev and get<t_meter, Head>() > elev) {
                    t_s_meter_t conductance_below = mechanics.calculateVerticalConductance(createDataTuple(hasDown));
                    out += conductance_below * (head_n - elev) * get<t_dim, StepModifier>();
                }
            }

            if (hasUp != neighbours.end()) {
                t_meter elev = getAt<t_meter, Elevation>(hasUp);
                t_meter head_n = getAt<t_meter, Head>(hasUp);
                //Check if a dewatered condition is present
                if (get<t_meter, Head>() < get<t_meter, Elevation>() and head_n > elev) {
                    t_s_meter_t conductance_above =
                            mechanics.calculateVerticalConductance(createDataTuple(hasUp));
                    out += conductance_above * (get<t_meter, Elevation>() - get<t_meter, Head>()) *
                           get<t_dim, StepModifier>();
                }
            }
            NANChecker(out.value(), "Dewatered flow");
            return out;
        }

        /**
         * @brief Get all current IN flow
         * @return Flow volume
         */
        t_vol_t getCurrentIN() noexcept { return getFlow([](double a) -> bool { return a > 0; }); }

        /**
         * @brief Get all current OUT flow
         * @return Flow volume
         */
        t_vol_t getCurrentOUT() noexcept { return getFlow([](double a) -> bool { return a < 0; }); }

        /**
         * @brief Tell cell to save its flow budget
         */
        void saveMassBalance() noexcept {
            fields.addTo<t_c_meter, OUT>(getCurrentOUT().value() * si::cubic_meter);
            fields.addTo<t_c_meter, IN>(getCurrentIN().value() * si::cubic_meter);
        }

        /**
         * @brief Add a neighbour
         * @param ID The internal ID and position in vector
         * @param neighbour The position relative to the cell
         */
        void setNeighbour(large_num ID, NeighbourPosition neighbour) { neighbours[neighbour] = ID; }

        int getNumofNeighbours() { return (int) neighbours.size(); }

        class NodeNotFoundException : public std::exception {
                virtual const char *what() const throw() { return "Node does not exist"; }
        };

        /**
         * @brief Get a neighbour by position
         * @param neighbour The position relative to the cell
         * @return Pointer to cell object
         */
        NodeInterface *getNeighbour(NeighbourPosition neighbour) noexcept(false) {
            try {
                large_num pos = neighbours.at(neighbour);
                if (nodes->at(pos)->get<large_num, ID>() != pos) { throw NodeNotFoundException(); }
                return nodes->at(pos).get();
            } catch (...) {
                throw NodeNotFoundException();
            }
        }

        /**
         * @brief At an external flow to the cell
         * @param type The flow type
         * @param flowHead The flow head
         * @param cond The conductance
         * @param bottom The bottom of the flow (e.g river bottom)
         * @return Number assigned by cell to flow
         */
        int addExternalFlow(FlowType type, t_meter flowHead, double cond, t_meter bottom) {
            if (type == RECHARGE or type == FAST_SURFACE_RUNOFF or type == NET_ABSTRACTION) {
                externalFlows.insert(std::make_pair(type,
                                                    ExternalFlow(numOfExternalFlows, cond * (si::cubic_meter / day),
                                                                 type)));
            } else if (type == EVAPOTRANSPIRATION) {
                externalFlows.insert(std::make_pair(type,
                                                    ExternalFlow(numOfExternalFlows, flowHead, bottom,
                                                                 cond * (si::cubic_meter / day))));
            } else if (type == FLOODPLAIN_DRAIN) {
                externalFlows.insert(std::make_pair(type,
                                                    ExternalFlow(numOfExternalFlows, type,
                                                                 get<t_meter, Elevation>(),
                                                                 get<t_vel, K>()
                                                                 * get<t_meter, VerticalSize>(),
                                                                 get<t_meter, EdgeLenght>())));
            } else {
                externalFlows.insert(std::make_pair(type,
                                                    ExternalFlow(numOfExternalFlows,
                                                                 type,
                                                                 flowHead,
                                                                 cond * (si::square_meter / day),
                                                                 bottom)));
            }
            numOfExternalFlows++;
            return numOfExternalFlows;
        }

        /**
         * @brief Remove an external flow to the cell by id
         * @param ID The flow id
         */
        void removeExternalFlow(FlowType type) {
            if (externalFlows.erase(type)) {
                numOfExternalFlows--;
            }
        }

        /**
         * @brief Check for an external flow by type
         * @param type The flow type
         * @return bool
         */
        bool hasTypeOfExternalFlow(FlowType type) {
            return (externalFlows.find(type) != externalFlows.end());
        }

        /**
         * @brief Updates GW recharge
         * Curently assumes only one recharge as external flow!
         * @param amount The new flow amount
         */
        void updateUniqueFlow(double amount, FlowType flow = RECHARGE) {
            if (hasTypeOfExternalFlow(flow)) {
                //LOG(debug) << "current: " << getExternalFlowByName(RECHARGE).getRecharge().value();
                removeExternalFlow(flow);
            }
            if (amount == 0) {
                return;
            }
            addExternalFlow(flow, 0 * si::meter, amount, 0 * si::meter);
            //LOG(debug) << "new: " << getExternalFlowByName(RECHARGE).getRecharge().value();
        }


        /**
         * Scale dyn rivers for sensitivity
         * @param mult
         */
        void scaleDynamicRivers(double mult) {
            if (hasTypeOfExternalFlow(RIVER_MM)) {
                getExternalFlowByName(RIVER_MM).setMult(mult);
            }
            return;
        }

        /**
         * @brief Update wetlands, lakes
         * @param amount
         * @param type
         */
        void updateExternalFlowConduct(double amount, FlowType type) {
            if (hasTypeOfExternalFlow(type)) {
                t_meter flowHead = getExternalFlowByName(type).getFlowHead();
                double conduct = getExternalFlowByName(type).getConductance().value() * amount;
                t_meter bottom = getExternalFlowByName(type).getBottom();
                removeExternalFlow(type);
                addExternalFlow(type, flowHead, conduct, bottom);
            }
        }

        /**
        * @brief Multiplies flow head for Sensitivity An. wetlands, lakes, rivers
        * @param amount
        * @param type
        */
        void updateExternalFlowFlowHead(double amount, FlowType type) {
            if (hasTypeOfExternalFlow(type)) {
                t_meter flowHead = getExternalFlowByName(type).getFlowHead() * amount;
                double conduct = getExternalFlowByName(type).getConductance().value();
                t_meter bottom = getExternalFlowByName(type).getBottom();
                removeExternalFlow(type);
                addExternalFlow(type, flowHead, conduct, bottom);
            }
        }

        /**
         * @brief Update lake bottoms
         * Used for sensitivity
         * @param amount
         */
        void updateLakeBottoms(double amount) {
            if (hasTypeOfExternalFlow(LAKE)) {
                t_meter flowHead = getExternalFlowByName(LAKE).getFlowHead();
                double conduct = getExternalFlowByName(LAKE).getConductance().value();
                t_meter bottom = getExternalFlowByName(LAKE).getBottom() * amount;
                removeExternalFlow(LAKE);
                addExternalFlow(LAKE, flowHead, conduct, bottom);
            }
        }

        /**
         * @brief Check for type river
         * @return bool
         */
        bool hasRiver() { return hasTypeOfExternalFlow(RIVER); }

        /**
         * @brief Check for type ocean
         * @return bool
         */
        bool hasOcean() { return hasTypeOfExternalFlow(GENERAL_HEAD_BOUNDARY); }


        /**
         * @brief Get Q part of flow equations
         * @return volume over time
         */
        t_vol_t getQ() noexcept {
            t_meter eq_head = get<t_meter, EQHead>();
            t_meter head = get<t_meter, Head>();
            t_vol_t recharge = 0 * si::cubic_meter / day;
            try {
                recharge = getExternalFlowByName(RECHARGE).getRecharge();
            } catch (const std::out_of_range &e) {//ignore me cell has no special_flow
            }
            t_dim slope = get<t_dim, Slope>();
            t_vol_t eqFlow = getEqFlow();
            t_vol_t out = 0.0 * (si::cubic_meter / day);
            for (const auto &flow : externalFlows) {
                out += flow.second.getQ(eq_head, head, recharge, slope, eqFlow) * get<t_dim, StepModifier>();
            }
            return out;
        }

        /**
         * @brief Get P part of flow equations
         * @return volume over time
         */
        t_s_meter_t getP() noexcept {
            t_s_meter_t out = 0.0 * (si::square_meter / day);
            t_meter eq_head = get<t_meter, EQHead>();
            t_meter head = get<t_meter, Head>();
            t_vol_t recharge = 0 * si::cubic_meter / day;
            try {
                recharge = getExternalFlowByName(RECHARGE).getRecharge();
            } catch (const std::out_of_range &e) {//ignore me
            }
            t_dim slope = get<t_dim, Slope>();
            t_vol_t eqFlow = getEqFlow();
            for (const auto &flow : externalFlows) {
                if (is(flow.second.getType()).in(RIVER, DRAIN, RIVER_MM, LAKE, WETLAND, GLOBAL_WETLAND)) {
                    if (flow.second.flowIsHeadDependant(get<t_meter, Head>())) {
                        out += flow.second.getP(eq_head, head, recharge, slope, eqFlow) * get<t_dim, StepModifier>();
                    }
                } else {
                    out += flow.second.getP(eq_head, head, recharge, slope, eqFlow) * get<t_dim, StepModifier>();
                }
            }
            return out;
        }

        /**
         * @brief Get flow which is not groundwater head dependent
         * @return volume over time
         * Flow can be added to constant flows on right side of the equations
         * If head is above river bottom for example
         */
        t_vol_t calculateNotHeadDependandFlows() noexcept {
            t_meter eq_head = get<t_meter, EQHead>();
            t_meter head = get<t_meter, Head>();
            t_vol_t recharge = 0 * si::cubic_meter / day;
            try {
                recharge = getExternalFlowByName(RECHARGE).getRecharge();
            } catch (const std::out_of_range &e) {//ignore me
            }
            t_dim slope = get<t_dim, Slope>();
            t_vol_t eqFlow = getEqFlow();
            t_vol_t out = 0.0 * (si::cubic_meter / day);
            //Q part is already substracted in RHS
            for (const auto &flow : externalFlows) {
                if (is(flow.second.getType()).in(RIVER, DRAIN, RIVER_MM, LAKE, WETLAND, GLOBAL_WETLAND)) {
                    if (not flow.second.flowIsHeadDependant(get<t_meter, Head>())) {
                        out += flow.second.getP(eq_head, head, recharge, slope, eqFlow) * get<t_dim, StepModifier>() *
                               flow.second.getBottom();
                    }
                }
            }
            return out;
        }

        /**
         * @brief The jacobian entry for the cell (NWT approach)
         * @return map <CellID,Conductance>
         */
        std::unordered_map<large_num, t_s_meter_t> getJacobian() noexcept {
            std::unordered_map<large_num, t_s_meter_t> out;
            size_t numC = 7;
            out.reserve(numC);
            std::unordered_map<large_num, t_s_meter_t> map = getConductance();
            t_s_meter_t tmp_hcofCRVC = map[get<large_num, ID>()];
            double head_diff{0.0};
            for (auto &ele : map) {
                map[ele.first] = ele.second * mechanics.getDerivate__NWT(
                        nodes->at(ele.first)->get<t_meter, Elevation>(),
                        nodes->at(ele.first)->get<t_meter, VerticalSize>(),
                        nodes->at(ele.first)->get<t_meter, Head>()
                );
                if (ele.first != get<large_num, ID>()) {
                    head_diff = nodes->at(ele.first)->get<t_meter, Head>().value()
                                - get<t_meter, Head>().value();
                    out[ele.first] =
                            (ele.second.value() + ele.second.value() * mechanics.getDerivate__NWT(
                                    nodes->at(ele.first)->get<t_meter, Elevation>(),
                                    nodes->at(ele.first)->get<t_meter, VerticalSize>(),
                                    nodes->at(ele.first)->get<t_meter, Head>()
                            ) * head_diff) * si::square_meter / day;
                    out[get<large_num, ID>()] += map[ele.first].value() * head_diff * si::square_meter / day;;
                }
            }
            out[get<large_num, ID>()] +=
                    ((mechanics.getHCOF(steadyState,
                                        get<t_dim, StepModifier>(),
                                        getStorageCapacity(),
                                        getP()).value() * mechanics.getDerivate__NWT(
                            get<t_meter, Elevation>(),
                            get<t_meter, VerticalSize>(),
                            get<t_meter, Head>()
                    ) * get<t_meter, Head>().value())
                     + tmp_hcofCRVC.value() - (getRHS().value() * mechanics.getDerivate__NWT(
                            get<t_meter, Elevation>(),
                            get<t_meter, VerticalSize>(),
                            get<t_meter, Head>()
                    ))) * si::square_meter / day;;
            return out;
        }

        /**
         * @brief The matrix entry for the cell
         * @return map <CellID,Conductance>
         * The left hand side of the equation
         */
        std::unordered_map<large_num, t_s_meter_t> getConductance() {
            size_t numC = 7;
            std::unordered_map<large_num, t_s_meter_t> out;
            out.reserve(numC);
            //Get all conductances from neighbouring cells
            std::forward_list<NeighbourPosition> possible_neighbours =
                    {NeighbourPosition::TOP, NeighbourPosition::BACK, NeighbourPosition::DOWN, NeighbourPosition::FRONT,
                     NeighbourPosition::LEFT, NeighbourPosition::RIGHT};

            for (const auto &position: possible_neighbours) {
                map_itter got = neighbours.find(position);
                t_vel K = 0;
                t_s_meter_t conduct = 0;
                if (got == neighbours.end()) {
                    //No neighbouring node
                } else {
                    //There is a neighbour node
                    if (got->first == TOP or got->first == DOWN) {
                        conduct = mechanics.calculateVerticalConductance(createDataTuple(got));
                    } else {
                        K = nodes->at(got->second)->getK();
                        conduct = mechanics.calculateHarmonicMeanConductance(createDataTuple<Head>(got));
                    }
                    NANChecker(conduct.value(), "Conductances");
                    /*if(conduct.value() == 0){
                        LOG(numerics) << "conductance to neighbour is 0";
                    }*/
                    out[nodes->at(got->second)->get<large_num, ID>()] = std::move(conduct);
                }
            }

            t_s_meter_t tmp_c = 0;

            for (const auto &c : out) { tmp_c = tmp_c - c.second; }
            t_s_meter_t hcof = mechanics.getHCOF(steadyState,
                                                 get<t_dim, StepModifier>(),
                                                 getStorageCapacity(),
                                                 getP());
            tmp_c = tmp_c + hcof;
            NANChecker(tmp_c.value(), "HCOF");
            //if(tmp_c.value() == 0){
            //	LOG(numerics) << "HCOF term is 0";
            //}
            out[get<large_num, ID>()] = tmp_c;
            return out;
        };

        /**
         * @brief The right hand side of the equation
         * @return volume per time
         */
        t_vol_t getRHS() noexcept {
            t_vol_t externalFlows = -getQ();
            t_vol_t dwateredFlow = calculateDewateredFlow();
            t_vol_t rivers = calculateNotHeadDependandFlows();
            t_vol_t storageFlow =
                    (getStorageCapacity() * get<t_meter, Head_TZero>()) / (day* get<t_dim, StepModifier>());
            if (steadyState) {
                storageFlow = 0 * (si::cubic_meter / day);
            }
            t_vol_t out = externalFlows + dwateredFlow - rivers - storageFlow;
            NANChecker(out.value(), "RHS");
            return out;
        }

        /**
         * @brief The right hand side of the equation (NWT)
         * @return volume per time
         */
        double getRHS__NWT() noexcept {
            double out{0.0};
            const std::unordered_map<large_num, t_s_meter_t> &map = getConductance();
            const double sum = std::accumulate(std::begin(map), std::end(map), 0,
                                               [this](const size_t previous,
                                                      const std::pair<int, t_s_meter_t>
                                                      &p) {
                                                   return previous + (p.second.value() *
                                                                      mechanics.smoothFunction__NWT(
                                                                              nodes->at(
                                                                                      p.first)->get<t_meter, Elevation>(),
                                                                              nodes->at(
                                                                                      p.first)->get<t_meter, VerticalSize>(),
                                                                              nodes->at(
                                                                                      p.first)->get<t_meter, Head>()));
                                               });
            out = sum - getRHS().value();
            return out;
        }

        void setHead(t_meter head) noexcept {
            __setHead(head);
        }

        t_meter calcInitialHead(t_meter initialParam) noexcept { return __calcInitialHead(initialParam); }

        bool isStaticNode() noexcept { return __isStaticNode(); }

        PhysicalProperties &getProperties() { return fields; }

        void enableNWT() { nwt = true; }

        /**
         * @brief Caluclate non storage related in and out flow
         * @return Flow volume
         */
        template<typename CompareFunction>
        t_vol_t getNonStorageFlow(CompareFunction compare) noexcept {
            t_vol_t out = 0.0 * si::cubic_meter / day;
            t_vol_t ex;
            for (const auto &flow : externalFlows) {
                ex = calculateExternalFlowVolume(flow.second);
                if (compare(ex.value())) {
                    out += boost::units::abs(ex);
                }
            }
            t_vol_t dwateredFlow = -calculateDewateredFlow();
            if (compare(dwateredFlow.value())) {
                out += boost::units::abs(dwateredFlow);
            }
            return out;
        }

        /**
         * Calculate the lateral flow velocity
         * @param pos
         * @return
         */
        quantity<Velocity> getVelocity(map_itter pos) {
            t_vol_t lateral_flow{0 * si::cubic_meter / day};
            t_meter vert_size = get<t_meter, VerticalSize>();
            t_s_meter_t conductance = mechanics.calculateHarmonicMeanConductance(createDataTuple<Head>(pos));
            lateral_flow = conductance * (get<t_meter, Head>() - getAt<t_meter, Head>(pos));
            return lateral_flow / (vert_size * vert_size);
        }

        /**
         * @brief Calculate flow velocity for flow tracking
         * Vx and Vy represent the flow velocity in x and y direction.
         * A negative value represents a flow in the oposite direction.
         * @return Velocity vector (x,y)
         */
        std::pair<double, double> getVelocityVector() {
            quantity<Velocity> Vx{0};
            quantity<Velocity> Vy{0};

            std::forward_list<NeighbourPosition> possible_neighbours =
                    {NeighbourPosition::BACK, NeighbourPosition::FRONT,
                     NeighbourPosition::LEFT, NeighbourPosition::RIGHT};
            for (const auto &position: possible_neighbours) {
                map_itter got = neighbours.find(position);
                if (got == neighbours.end()) {
                    continue;
                }
                if (got->first == NeighbourPosition::LEFT) {
                    Vx += -getVelocity(got);
                }
                if (got->first == NeighbourPosition::BACK) {
                    Vy += -getVelocity(got);
                }

                if (got->first == NeighbourPosition::RIGHT) {
                    Vx += getVelocity(got);
                }
                if (got->first == NeighbourPosition::FRONT) {
                    Vy += getVelocity(got);
                }
            }

            return std::make_pair(Vx.value(), Vy.value());
        };

};

/**
 * @class StandardNode
 * A standard groundwater node
 */
class StandardNode : public NodeInterface {
    public:
        StandardNode(std::shared_ptr<std::vector<std::unique_ptr<NodeInterface>>> nodes,
                     double lat,
                     double lon,
                     t_s_meter area,
                     large_num ArcID,
                     large_num ID,
                     t_vel K,
                     int stepmodifier,
                     double aquiferDepth,
                     double anisotropy,
                     double specificYield,
                     double specificStorage, bool confined)
                : NodeInterface(nodes, lat, lon, area, ArcID, ID, K, stepmodifier, aquiferDepth,
                                anisotropy, specificYield, specificStorage, confined) {}

    private:
        // implementation
        friend class NodeInterface;

        //Learning weight
        t_dim weight = 0.1;

        /**
         * @brief Update heads after one or multiple inner iterations
         * @param head
         */
        virtual void __setHead(t_meter delta) {
            NANChecker(delta.value(), "Set Head");
            t_meter deltaH__old = get<t_meter, HeadChange>();
            t_meter current_head = get<t_meter, Head>();
            //t_meter delta = head - current_head;
            set<t_meter, HeadChange>(delta);
            set<t_meter, Head>(current_head + delta);

            /*	if (nwt){
                    //TODO move to Numerics
                    //Underrelaxation with delta-bar-delta from Smith 1993:

                    t_dim gamma = 0 * si::si_dimensionless;
                    t_dim theta = 0.9;
                    t_dim kappa = 0.00001;
                    t_dim momentum = 0.1;
                    t_meter deltaH = (1 - gamma) * get<t_meter, HeadChange
                    >() + gamma * deltaH__old;
                    if (weight < 1)
                    {
                        weight = weight + kappa;
                    }
                    else
                    {
                        weight = weight - theta * weight;
                    }
                    t_meter h = head + weight * get<t_meter, HeadChange>() + momentum * deltaH;
                    set<t_meter, HeadChange>(h - get<t_meter, Head>());
                    set<t_meter, Head>(h);
                }*/
        };

        virtual t_meter
        __calcInitialHead(t_meter initialParam) {
            t_meter elevation = get<t_meter, TopElevation>();
            if (elevation >= initialParam) {
                return elevation - initialParam;
            }
            return elevation;
        }

        virtual bool
        __isStaticNode() { return false; }
};

/**
 * @class SaticHeadNode
 * A node without changing head
 * Can be used as boundary condition
 */
class StaticHeadNode : public NodeInterface {
    public:
        StaticHeadNode(std::shared_ptr<std::vector<std::unique_ptr<NodeInterface>>> nodes,
                       large_num ID,
                       t_s_meter area)
                : NodeInterface(
                nodes,
                0,
                0,
                area,
                ID,
                ID,
                0.3 * (si::meter / day), 1, 100, 10, 0.15, 0.000015, true) {}

    private:
        friend class NodeInterface;

        virtual void __setHead(t_meter head) {
            set<t_meter, HeadChange>(0 * si::meter);
        };

        virtual t_meter __calcInitialHead(t_meter initialParam) { return 0; }

        virtual bool __isStaticNode() { return true; }
};

}
}//ns
#endif //NODE_HPP

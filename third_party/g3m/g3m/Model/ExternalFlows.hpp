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

#ifndef GLOBAL_FLOW_EXTERNALFLOWS_HPP
#define GLOBAL_FLOW_EXTERNALFLOWS_HPP

#include "Units.hpp"
#include "../Misc/Helpers.hpp"

namespace GlobalFlow {
namespace Model {
/**
 * @enum External Flow Types
 * Flow types as defined in MODFLOW:
 *  - RECHARGE
 *  - RIVER
 *  - DRAIN
 *  - GENERAL_HEAD_BOUNDARY
 *
 * Additional flows:
 *
 * RECHARGE:
 *
 * RIVER_MM:
 *  dynamic river conductance as defined by Miguez-Macho 2007
 *
 * FLOODPLAIN_DRAIN:
 *  as defined in Inge de Graaf. 2014
 *
 *
 * LAKE, WETLAND
 *  similar to modflow river definition
 */
enum FlowType : int {
    RECHARGE = 1,
    FAST_SURFACE_RUNOFF,
    NET_ABSTRACTION,
    EVAPOTRANSPIRATION,
    RIVER,
    RIVER_MM,
    DRAIN,
    FLOODPLAIN_DRAIN,
    WETLAND,
    GLOBAL_WETLAND,
    LAKE,
    GENERAL_HEAD_BOUNDARY
};

struct FlowTypeHash {
    template<typename T>
    std::size_t operator()(T t) const {
        return static_cast<std::size_t>(t);
    }
};

/**
 * @class ExternalFlow
 *
 * TODO add flow equation here
 */
class ExternalFlow {
    public:
        ExternalFlow(int id,
                     FlowType type,
                     t_meter flowHead,
                     t_s_meter_t cond,
                     t_meter bottom)
                : ID(id), type(type), flowHead(flowHead), conductance(cond), bottom(bottom) {}

        /**
         * Only for RECHARGE
        FAST_SURFACE_RUNOFF
         */
        ExternalFlow(int id, t_vol_t recharge, FlowType type)
                : ID(id), type(type), flowHead(0), conductance(0), bottom(0), special_flow(recharge) {}

        /**
         * @brief Constructor for Evapotranspiration
         * @param id
         * @param flowHead
         * @param bottom
         * @param evapotrans
         * @return
         */
        ExternalFlow(int id, t_meter flowHead, t_meter bottom, t_vol_t evapotrans)
                : ID(id), type(EVAPOTRANSPIRATION), flowHead(0), conductance(0), bottom(0), special_flow(evapotrans) {}

        /**
         * Check if flow can be calculated on the right hand side
         * @param head The current hydraulic head
         * @return Bool
         */
        bool flowIsHeadDependant(t_meter head) const noexcept {
            return (head > bottom);
        }

        /**
         * The head dependant part of the external flow equation
         * @param head The current hydraulic head
         * @param eq_head The equilibrium head
         * @param recharge The current recharge
         * @param slope
         * @param eqFlow
         * @return
         */
        t_s_meter_t getP(t_meter head,
                         t_meter eq_head,
                         t_vol_t recharge,
                         t_dim slope,
                         t_vol_t eqFlow) const noexcept;

        /**
         * The head independant part of the external flow equation
         * @param head
         * @param eq_head
         * @param recharge
         * @param slope
         * @param eqFlow
         * @return
         */
        t_vol_t getQ(t_meter head,
                     t_meter eq_head,
                     t_vol_t recharge,
                     t_dim slope,
                     t_vol_t eqFlow) const noexcept;

        FlowType getType() const noexcept { return type; }

        t_meter getBottom() const noexcept { return bottom; }

        t_vol_t getRecharge() const noexcept { return special_flow; }

        t_meter getFlowHead() const noexcept { return flowHead; }

        t_s_meter_t getDyn(t_vol_t current_recharge,
                           t_meter eq_head,
                           t_meter head,
                           t_vol_t eq_flow) const noexcept {
            t_s_meter_t out = calcERC(current_recharge, eq_head, head, eq_flow);
            return out;
        }

        t_meter getRiverDiff(t_meter eqHead) const noexcept;

        t_s_meter_t getConductance() const noexcept { return conductance; }

        int getID() const noexcept { return ID; }

        void setMult(double mult) {
            this->mult = mult;
            return;
        }

    private:
        const int ID;
        const FlowType type;
        const t_meter flowHead;
        const t_s_meter_t conductance; //for special_flow same as q
        const t_vol_t special_flow;
        const t_meter bottom;
        t_dim mult{1 * si::si_dimensionless}; //Multiplier only used for SA

        t_vol_t
        calculateFloodplaindDrainage(t_meter head) const noexcept;

        /**
        * Calculate river conductance as in Miguez-Macho 2007
        * RC = ERC * F
        * Input: F-data, ERC
        * Output: RC
        */
        t_s_meter_t
        dynamicRiverConductance(t_meter head,
                                t_vol_t current_recharge,
                                t_dim slope,
                                t_vol_t eq_flow) const noexcept;

        /**
        * Calculate ERC (must be repeated every time step)
        * ERC = (GW_Recharge + eq_flow) / (eq_head - river_elevation)
        */
        t_s_meter_t
        calcERC(t_vol_t current_recharge,
                t_meter eq_head,
                t_meter current_head,
                t_vol_t eq_flow) const noexcept;
};

}
}//ns
#endif //EXTERNALFLOWS_HPP

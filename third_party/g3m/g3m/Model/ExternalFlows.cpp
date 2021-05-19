#include "ExternalFlows.hpp"

namespace GlobalFlow {
namespace Model {

t_s_meter_t ExternalFlow::getP(t_meter eq_head, t_meter head,
                               t_vol_t recharge,
                               t_dim slope,
                               t_vol_t eqFlow) const noexcept {
    t_s_meter_t out = 0.0 * (si::square_meter / day);
    switch (type) {
        case RECHARGE:
            return out;
        case FAST_SURFACE_RUNOFF:
            return out;
        case NET_ABSTRACTION:
            return out;
        case EVAPOTRANSPIRATION:
            //flowHead = surface, bottom = extinction depth
            if ((head < flowHead - bottom) xor (head > flowHead)) {
                return out;
            } else {
                return -special_flow / bottom;
            }
        case FLOODPLAIN_DRAIN:
            return out;
        case RIVER:
            return -conductance;
        case RIVER_MM:
            return -calcERC(recharge, eq_head, head, eqFlow);
        case WETLAND:
            return -conductance;
        case GLOBAL_WETLAND:
            return -conductance;
        case LAKE:
            return -conductance;
        case DRAIN:
            if (head > flowHead) {
                return -calcERC(recharge, eq_head, head, eqFlow);
            } else {
                return out;
            }
        case GENERAL_HEAD_BOUNDARY:
            return -conductance;
    }
    return out;
}

t_vol_t ExternalFlow::getQ(t_meter eq_head, t_meter head,
                           t_vol_t recharge,
                           t_dim slope,
                           t_vol_t eqFlow) const noexcept {
    quantity<VolumePerTime, double> out = 0.0 * (si::cubic_meter / day);
    switch (type) {
        case RECHARGE:
            return this->special_flow;
        case FAST_SURFACE_RUNOFF:
            return this->special_flow;
        case NET_ABSTRACTION:
            return this->special_flow;
        case EVAPOTRANSPIRATION:
            if (head < flowHead - bottom) {
                return out;
            } else if (flowHead - bottom <= head and head <= flowHead) {
                return -this->special_flow + (this->special_flow * flowHead) / bottom;
            } else {
                return -this->special_flow;
            }
        case FLOODPLAIN_DRAIN:
            return -calculateFloodplaindDrainage(head);
        case RIVER:
            return conductance * flowHead;
        case RIVER_MM:
            return calcERC(recharge, eq_head, head, eqFlow) * flowHead;
        case WETLAND:
            return conductance * flowHead;
        case GLOBAL_WETLAND:
            return conductance * flowHead;
        case LAKE:
            return conductance * flowHead;
        case DRAIN:
            if (head > flowHead) {
                return calcERC(recharge, eq_head, head, eqFlow) * flowHead;
            } else {
                return out;
            }
        case GENERAL_HEAD_BOUNDARY:
            return conductance * flowHead;
    }
    return out;
}

t_vol_t ExternalFlow::calculateFloodplaindDrainage(t_meter head) const noexcept {
    quantity<VolumePerTime, double> out = 0.0 * (si::cubic_meter / day);
    t_meter headAboveFloodplain = head - flowHead;
    if (headAboveFloodplain > 0 * si::meter) {
        const double PI = std::atan(1.0) * 4;
        double J = (PI * conductance.value()) / (4 * 0.15 * (500 * 500));
        return bottom * bottom * headAboveFloodplain * (J * 1 / day);
    }
    return out;
}

t_meter ExternalFlow::getRiverDiff(t_meter eqHead) const noexcept {
    return eqHead - flowHead;
}

/*
 * From ATI pixel shaders
 */
double clamp(double x, double lowerlimit, double upperlimit) {
    if (x < lowerlimit) x = lowerlimit;
    if (x > upperlimit) x = upperlimit;
    return x;
}

double smoothstep(double edge0, double edge1, double x) {
    // Scale, bias and saturate x to 0..1 range
    x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    // Evaluate polynomial
    return x * x * (3 - 2 * x);
}

t_s_meter_t ExternalFlow::calcERC(t_vol_t current_recharge,
                                  t_meter eq_head,
                                  t_meter current_head,
                                  t_vol_t eq_flow) const noexcept {

    //LOG(debug) << "recharge:" << current_recharge.value() << "head:" << eq_head.value() << "StreamStage: " << flowHead.value() << "Bottom" << bottom.value() << "EQFlow" << eq_flow.value() << "AltConduct" << conductance.value();

    t_s_meter_t out = 0 * si::square_meter / day;

    //Static MM
    t_meter stage = eq_head - flowHead;

    double p = 1;
    if (stage.value() <= 0) {
        stage = 1 * si::meter;
    }

    NANChecker(out.value(), "Recharge problem");

    out = (current_recharge * (p * si::si_dimensionless) + eq_flow) / stage;
    NANChecker(out.value(), "ERC Problem");

    if (out < conductance) {
        //Only happens if cell was loosing in eq and is now gaining
        out = conductance;
    }

    if (current_head < flowHead - 1 * si::meter) {
        t_s_meter_t tmp_conduct = conductance;
        if (tmp_conduct.value() > 1e+10) {
            tmp_conduct = 1e+10 * si::square_meter / day;
        }
        NANChecker(tmp_conduct.value(), "ERC Problem");
        if (tmp_conduct.value() <= 0) {
            LOG(critical) << "conductance <= 0";
        }
        return tmp_conduct * mult;
    } else if (current_head > flowHead + 1 * si::meter) {
        if (out.value() > 1e+10) {
            out = 1e+10 * si::square_meter / day;
        }
        NANChecker(out.value(), "ERC Problem");
        if (out.value() <= 0) {
            LOG(critical) << "conductance <= 0";
        }
        return out * mult;
    }

    double delta = smoothstep(flowHead.value() - 1, flowHead.value() + 1, current_head.value());
    double range = std::abs(out.value() - conductance.value());
    double lower_bound = out.value() > conductance.value() ? out.value() : conductance.value();
    out = (lower_bound + range * delta) * si::square_meter / day;

    NANChecker(out.value(), "ERC Problem");

    if (out.value() > 1e+10) {
        out = 1e+10 * si::square_meter / day;
    }
    if (out.value() <= 0) {
        LOG(critical) << "conductance <= 0";
    }
    return out * mult;
}
}
}//ns

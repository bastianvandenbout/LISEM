#include "FluidMechanics.hpp"
#include "../Misc/Helpers.hpp"

namespace GlobalFlow {
    namespace Model {

        t_meter FluidMechanics::calcDeltaV(t_meter head, t_meter elevation, t_meter depth) noexcept {
            t_meter epsilon{1e-4 * si::meter};
            t_meter bottom = elevation - depth;
            if (head >= elevation) { return depth; }
            if (elevation > head and head + epsilon > bottom) { return head - bottom; }
            if (head + epsilon <= bottom) {
                return 0.0 * si::meter;
            }
            return 0.0 * si::meter;
        };

        quantity<MeterSquaredPerTime> FluidMechanics::calculateHarmonicMeanConductance(FlowInputHor flow)noexcept {
            t_vel k_neig;
            t_vel k_self;
            t_meter edgeLength_neig;
            t_meter edgeLength_self;
            t_meter head_neig;
            t_meter head_self;
            t_meter ele_neig;
            t_meter ele_self;
            t_meter deltaV_neig;
            t_meter deltaV_self;
            bool confined;
            std::tie(k_neig, k_self, edgeLength_neig, edgeLength_self, head_neig, head_self, ele_neig, ele_self,
                     deltaV_neig, deltaV_self, confined) = flow;

            quantity<MeterSquaredPerTime> out = 0 * si::square_meter / day;
            //Used if non dry-out approach is used
            // FIXME need to be checked here
            t_meter threshhold_saturated_thickness{1e-5 * si::meter};

            //Cell is not confined layer -> we need to calculate transmissivity dependant on head
            enum e_dry {
                NONE, SELF, NEIG
            };
            e_dry dry{NONE};
            if (not confined) {
                deltaV_self = calcDeltaV(head_self, ele_self, deltaV_self);
                deltaV_neig = calcDeltaV(head_neig, ele_neig, deltaV_neig);
                if (deltaV_self == 0 * si::meter) {
                    deltaV_self = threshhold_saturated_thickness;
                    dry = SELF;
                }
                if (deltaV_neig == 0 * si::meter) {
                    deltaV_neig = threshhold_saturated_thickness;
                    dry = NEIG;
                }
                //TODO One of the cells is "dry" - Upstream weighting instead of harmonic mean
            }

            //Transmissivity = K * thickness of prism
            quantity<MeterSquaredPerTime> transmissivity_self = deltaV_self * k_self;
            quantity<MeterSquaredPerTime> transmissivity_neig = deltaV_neig * k_neig;

            if (transmissivity_neig != out and transmissivity_self != out) {
                out = (2.0 * edgeLength_self) * ((transmissivity_self * transmissivity_neig)
                                                 / (transmissivity_self * edgeLength_neig +
                                                    transmissivity_neig * edgeLength_self));
            }

            NANChecker(out.value(), "Harmonic Mean Conductance");
            return out;
        };

        double FluidMechanics::smoothFunction__NWT(t_meter elevation, t_meter verticalSize, t_meter head) {
            t_meter bottom = elevation - verticalSize;
            double X = (head - bottom) / verticalSize;
            double sigma(1.0e-20);
            double A(1 / (1 - sigma));

            if (X <= 0)
                return 1.0e-20;
            if (0 < X and X <= sigma)
                return (0.5 * A * pow(X, 2) / sigma);
            if (sigma < X and X <= (1 - sigma))
                return (A * X + 0.5 * (1 - A));
            if ((1 - sigma) < X and X < 1)
                return (1 - (0.5 * A * pow((1 - X), 2) / sigma));
            if (X >= 1)
                return 1;

            //This should not happen
            return 1;
        }

        quantity<MeterSquaredPerTime>
        FluidMechanics::calculateVerticalConductance(FlowInputVert flowInputVer) noexcept {
            t_vel k_vert_neig;
            t_vel k_vert_self;
            t_meter verticalSize_self;
            t_meter head_self;
            t_meter elevation_self;
            t_s_meter area_self;
            t_meter elevation_neig;
            t_meter depth_neig;
            t_meter head_neig;
            bool confined;

            std::tie(k_vert_neig,
                     k_vert_self,
                     verticalSize_self,
                     head_self,
                     elevation_self,
                     area_self,
                     elevation_neig,
                     depth_neig,
                     head_neig,
                     confined) = flowInputVer;


            quantity<MeterSquaredPerTime> out = 0.0 * si::square_meter / day;
            t_meter deltaV_self = verticalSize_self;
            t_meter deltaV_neig = depth_neig;

            //Used if non dry-out approach is used
            // FIXME need to be checked here
            t_meter threshhold_saturated_thickness{1e-5 * si::meter};

            enum e_dry {
                NONE, SELF, NEIG
            };
            e_dry dry{NONE};
            if (not confined) {
                deltaV_self = calcDeltaV(head_self, elevation_self, verticalSize_self);
                deltaV_neig = calcDeltaV(head_neig, elevation_neig, depth_neig);
                if (deltaV_self == 0 * si::meter) {
                    deltaV_self = threshhold_saturated_thickness;
                    dry = SELF;
                }
                if (deltaV_neig == 0 * si::meter) {
                    deltaV_neig = threshhold_saturated_thickness;
                    dry = NEIG;
                }
                //TODO One of the cells is "dry" - Upstream weighting instead of harmonic mean
            }

            if (deltaV_self != 0.0 * si::meter and deltaV_neig != 0.0 * si::meter) {
                out = area_self / (((deltaV_self * 0.5) / k_vert_self) + ((deltaV_neig * 0.5) / k_vert_neig));
            }

            NANChecker(out.value(), "Vertical Conductance");
            return out;
        };

        quantity<MeterSquaredPerTime> FluidMechanics::getHCOF(bool steadyState, quantity<Dimensionless> stepModifier,
                                                              t_s_meter storageCapacity,
                                                              quantity<MeterSquaredPerTime> P) noexcept {
            if (steadyState)
                return P;
            quantity<MeterSquaredPerTime> out =
                    P - (storageCapacity / day);
            //* stepModifier
            NANChecker(out.value(), "HCOF");
            return out;
        }

        double FluidMechanics::getDerivate__NWT(t_meter elevation,
                                                t_meter verticalSize,
                                                t_meter head) {
            t_meter bottom = elevation - verticalSize;
            double X = (head - bottom) / verticalSize;
            double sigma(1.0e-20);
            double A(1 / (1 - sigma));

            if (X <= 0)
                return 0;
            if (0 < X and X <= sigma)
                return A * X / (sigma * (verticalSize.value()));
            if (sigma < X and X <= (1 - sigma))
                return A / verticalSize.value();
            if ((1 - sigma) < X and X < 1)
                return (1 - (-A * (1.0 - X) / (sigma * verticalSize.value())));
            if (X >= 1)
                return 0;
            //This should not happen
            return 0;
        }
    }
}//ns

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

#ifndef GLOBAL_FLOW_EQUATION_HPP
#define GLOBAL_FLOW_EQUATION_HPP

//This disables boundary checks!
//#define EIGEN_NO_DEBUG

#include <stdio.h>
#include <cstring>
#include <unordered_map>
#include <algorithm>
#include <cfenv>
#include <future>
#include <iostream>
#include <unordered_set>

#include "Eigen/Sparse"
#include "Eigen/Core"
#include "../Model/Node.hpp"
#include "../Simulation/Options.hpp"
#include "Numerics.hpp"

namespace GlobalFlow {
namespace Solver {
using namespace boost::units;
using namespace Eigen;

using NodeVector = std::shared_ptr<std::vector<std::unique_ptr<Model::NodeInterface>>>;
using large_num = unsigned long int;

/**
 * @class Equation The internal finite difference equation
 * Should only be accessed through the stepper
 */
class Equation {
    public:
        Equation(large_num numberOfNodes, NodeVector nodes, Simulation::Options options);

        ~Equation();

        /**
         * Solve the current iteration step
         */
        void solve();

        /**
         * @return The number of iterations
         */
        int getItter();

        /**
         * @return The current residual error
         */
        double getError();

        //No copy and copy assign for Equations
        Equation(const Equation &) = delete;

        Equation &
        operator=(const Equation &) = delete;

        /**
         * Helper to write out current residuals
         * @param os
         * @param eq
         * @return
         */
        friend std::ostream &
        operator<<(std::ostream &os, Equation &eq) {
            IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
            os << eq.getResults().format(CleanFmt);
            return os;
        };

        VectorXd getResults() {
            return this->x;
        }

        /**
         * Toogle the steady-state in all nodes
         * @return
         */
        bool toogleSteadyState() {
            SteadyState = !SteadyState;
            bool state = SteadyState;
            std::for_each(nodes->begin(),
                          nodes->end(),
                          [state](std::unique_ptr<Model::NodeInterface> const &node) {
                              node->toogleStadyState(state);
                          });
            return SteadyState;
        }

        /**
         * Set the correct stepsize (default is DAY)
         * @param mod
         */
        void updateStepSize(size_t mod) {
            std::for_each(nodes->begin(),
                          nodes->end(),
                          [mod](std::unique_ptr<Model::NodeInterface> const &node) { node->updateStepSize(mod); });
        }

        typedef typename Eigen::MatrixXd::Scalar Scalar;
        typedef Matrix<Scalar, Dynamic, 1> VectorType;

        VectorType getResiduals() {
            return VectorType();//scg.getResiduals();
        }

        void updateClosingCrit(double crit) { cg.setTolerance(crit); }

    private:
        bool initalized = false;

        large_num numberOfNodes;
        int initialHead;

        /**
         * _var_ only used if disabling of cells is required
         */
        NodeVector nodes;
        VectorXd x;
        VectorXd _x_;
        VectorXd b;
        VectorXd _b_;
        SparseMatrix<double> A;
        SparseMatrix<double> _A_;

        const Simulation::Options options;

        bool isAdaptiveDamping{true};
        AdaptiveDamping adaptiveDamping;

        int IITER{0};
        double RCLOSE{0};

        //From current run
        int __itter{0};
        double __error{0};

        bool isCached{false};

        double maxHeadChange{0.01};
        double dampMin{0.01};
        double dampMax{0.01};

        bool disable_dry_cells{false};
        //Maybe rename me :D
        std::unordered_set<large_num> disabled_nodes;
        //Real -> Current
        std::unordered_map<large_num, long long> index_mapping;
        bool dry_have_changed{true};

        template<typename Set>
        bool set_compare(Set const &lhs, Set const &rhs) {
            return lhs.size() == rhs.size()
                   && std::equal(lhs.begin(), lhs.end(), rhs.begin());
        }

        //Need to be at same place as matrix A !!!!
        //By pointer or rev breaks calculation!!
        //ConjugateGradient<SparseMatrix<double>, Lower | Upper, IncompleteCholesky<SparseMatrix<double>::Scalar, Lower | Upper>> cg;
        //ConjugateGradient<SparseMatrix<double>, Lower | Upper, DiagonalPreconditioner<SparseMatrix<double>::Scalar>> cg;
        ConjugateGradient<SparseMatrix<double>, Lower | Upper, IncompleteLUT<SparseMatrix<double>::Scalar>> cg;
        //ConjugateGradient<SparseMatrix<double>, Lower | Upper> cg;

        BiCGSTAB<SparseMatrix<double>, IncompleteLUT<SparseMatrix<double>::Scalar>> bicgstab;
        //Used for NWT
        bool nwt{false};

        /**
         * Helper for updating the matrix
         * @param node
         * @param cached
         */
        void addToA(std::unique_ptr<Model::NodeInterface> const &node, bool cached);

        /**
         * Update the matrix for the current iteration
         */
        void inline updateMatrix();

        /**
         * Reallocate matrix and vectors absed on dried nodes
         * @bug This is currently missing reenabling of deactivated nodes!
         * Reenable if:
         * 1) head in cell below needs to be higher than threshhold
         * 2) head in one of 4 neighbours higher than threshhold
         */
        void inline reallocateMatrix();

        /**
         * Run the preconditioner
         */
        void inline preconditioner();

        /**
         * Update heads in inner iteration
         */
        void inline updateIntermediateHeads();

        /**
         * Calculate the final budget
         */
        void inline updateBudget();

        /**
         * Write the final head to the nodes
         */
        void inline updateFinalHeads();

        bool SteadyState = false;
        //Only for testin purposes
        bool simpelHead = true;
};

}
}//ns
#endif

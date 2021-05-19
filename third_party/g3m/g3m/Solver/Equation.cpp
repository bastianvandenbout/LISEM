#include "Equation.hpp"

#include <limits>

namespace GlobalFlow {
namespace Solver {

Equation::Equation(large_num numberOfNodes, NodeVector nodes, Simulation::Options options) : options(options) {
    LOG(userinfo) << "Setting up Equation for " << numberOfNodes << "\n";

    this->numberOfNodes = numberOfNodes;
    this->IITER = options.getMaxIterations();
    this->RCLOSE = options.getConverganceCriteria();
    this->initialHead = options.getInitialHead();
    this->maxHeadChange = options.getMaxHeadChange();
    this->isAdaptiveDamping = options.isDampingEnabled();
    this->dampMin = options.getMinDamp();
    this->dampMax = options.getMaxDamp();
    this->disable_dry_cells = options.disableDryCells();
    this->nwt = (options.getSolverName().compare("NWT") == 0);
    if (nwt)
        LOG(userinfo) << "Running with NWT solver" << std::endl;

    int innerItter = options.getInnerItter();

    this->nodes = nodes;

    VectorXd __x(numberOfNodes);
    VectorXd __b(numberOfNodes);

    x = std::move(__x);
    b = std::move(__b);

    Eigen::SparseMatrix<double> __A(numberOfNodes, numberOfNodes);
    A = std::move(__A);
    A.reserve(Eigen::VectorXd::Constant(numberOfNodes, 7));

    //Init first result vector
    //Initial head should be positive
    //resulting head is the real hydraulic head

    double tmp = 0;
#pragma omp parallel for
    for (int i = 0; i < numberOfNodes; ++i) {
        if (nwt) {
            nodes->at(i)->enableNWT();
        }
        if (not simpelHead) {
            tmp = nodes->at(i)->calcInitialHead(initialHead * si::meter).value();
            nodes->at(i)->setHead_direct(tmp);
            NANChecker(tmp, "Initial Head");
            x[nodes->at(i)->getProperties().get<large_num, Model::ID>()] = std::move(tmp);
        } else {
            x[nodes->at(i)->getProperties().get<large_num, Model::ID>()] =
                    nodes->at(i)->getProperties().get<quantity<Model::Meter>, Model::Head>().value();
            nodes->at(i)->initHead_t0();
        }
    }

    if (nwt) {
        bicgstab.setMaxIterations(innerItter);
        bicgstab.setTolerance(RCLOSE);
    } else {
        //set inner iterrations
        cg.setMaxIterations(innerItter);
        cg.setTolerance(RCLOSE);
        //cg.preconditioner().setInitialShift(1e-8);
    }
};

Equation::~Equation() {
    LOG(debug) << "Destroying equation\n";
}

void inline
Equation::addToA(std::unique_ptr<Model::NodeInterface> const &node, bool cached) {
    std::unordered_map<large_num, quantity<Model::MeterSquaredPerTime>> map;
    if (nwt) {
        map = node->getJacobian();
    } else {
        map = node->getConductance();
    }
    auto nodeID = node->getProperties().get<large_num, Model::ID>();

    if (disable_dry_cells) {
        if (not map[nodeID].value()) {
            disabled_nodes.insert(nodeID);
        }
    }

    for (const auto &conductance : map) {
        double tmp = conductance.second.value();
        NANChecker(tmp, "Add to A");
        if (cached) {
            A.coeffRef(nodeID, conductance.first) = std::move(tmp);
        } else {
            A.insert(nodeID, conductance.first) = std::move(tmp);
        }
    }
}

void inline Equation::reallocateMatrix() {
    if (not disable_dry_cells) {
        return;
    }

    if (disabled_nodes.empty()) {
        _A_ = A;
        _b_ = b;
        _x_ = x;
        return;
    }

    large_num __missing{0};
    long size = A.rows() - disabled_nodes.size();
    Matrix2Xd new_matrix(size, size);

    if (dry_have_changed) {
        index_mapping.clear();
    }

    for (large_num i = 0; i < A.rows(); i++) {
        if (not dry_have_changed) {
            auto m = index_mapping[i];
            if (m != -1) {
                new_matrix.row(i) = A.row(i);
            }
        } else {
            const bool is_in = disabled_nodes.find(i) != disabled_nodes.end();
            if (not is_in) {
                //Save all non-zero
                new_matrix.row(i) = A.row(i);
                index_mapping[i] = i - __missing;
            } else {
                __missing++;
                index_mapping[i] = -1;
            }
        }
    }
    _A_ = new_matrix.sparseView();

    VectorXd buffer_b;
    VectorXd buffer_x;
    for (int j = 0; j < b.size(); ++j) {
        auto m = index_mapping[j];
        if (m != -1) {
            buffer_b[m] = b[j];
            if (dry_have_changed) {
                buffer_x[m] = x[j];
            }
        }
    }
    _b_ = buffer_b;
    if (dry_have_changed) {
        _x_ = buffer_x;
    }
}

void inline
Equation::updateMatrix() {
    Index n = A.outerSize();
    std::unordered_set<large_num> tmp_disabled_nodes = disabled_nodes;
    disabled_nodes.clear();

#ifdef EIGEN_HAS_OPENMP
    Eigen::initParallel();
    Index threads = Eigen::nbThreads();
#endif
#pragma omp parallel for schedule(dynamic,(n+threads*4-1)/(threads*4)) num_threads(threads)
    for (large_num j = 0; j < numberOfNodes; ++j) {
        //---------------------Left
        addToA(nodes->at(j), isCached);
        //---------------------Right
        double rhs{0.0};
        if (nwt) {
            rhs = nodes->at(j)->getRHS__NWT();
            b[nodes->at(j)->getProperties().get<large_num, Model::ID>()] = std::move(rhs);
            NANChecker(rhs, "Right hand side");
        } else {
            rhs = nodes->at(j)->getRHS().value();
            b[nodes->at(j)->getProperties().get<large_num, Model::ID>()] = std::move(rhs);
            NANChecker(rhs, "Right hand side");
        }
    }

    if (not disable_dry_cells) {
        dry_have_changed = not set_compare(tmp_disabled_nodes, disabled_nodes);
    }

    //some nodes are in dried condition
    //reallocate matrix, and a,b
    reallocateMatrix();

    //A is up to date and b contains only rhs term
    //Update with current heads
    if (nwt) {
        if (disable_dry_cells) {
            _b_ = _b_ + _A_ * _x_;
        } else {
            b = b + A * x;
        }
    }

    //Check if after iteration former 0 values turned to non-zero
    if (disable_dry_cells) {
        if ((not _A_.isCompressed()) and isCached) {
            LOG(numerics) << "Recompressing Matrix \n";
            _A_.makeCompressed();
        }
    } else {
        if ((not A.isCompressed()) and isCached) {
            LOG(numerics) << "Recompressing Matrix \n";
            A.makeCompressed();
        }
    }
}

void inline Equation::preconditioner() {
    LOG(numerics) << "Decomposing Matrix\n";
    if (nwt) {
        if (disable_dry_cells) {
            bicgstab.compute(_A_);
        } else {
            bicgstab.compute(A);
        }
        if (bicgstab.info() != Success) {
            LOG(numerics) << "Fail in decomposing matrix\n";
            throw "Fail in decomposing matrix";
        }
    } else {
        if (disable_dry_cells) {
            cg.compute(_A_);
        } else {
            cg.compute(A);
        }
        if (cg.info() != Success) {
            LOG(numerics) << "Fail in decomposing matrix\n";
            throw "Fail in decomposing matrix";
        }
    }
}

void inline
Equation::updateIntermediateHeads() {
    Eigen::VectorXd resid = getResiduals();
    VectorXd changes;
    if (disable_dry_cells) {
        changes = adaptiveDamping.getDamping(resid, _x_, isAdaptiveDamping);
    } else {
        changes = adaptiveDamping.getDamping(resid, x, isAdaptiveDamping);
    }

    bool reduced = disabled_nodes.empty();
#pragma omp parallel for
    for (large_num k = 0; k < numberOfNodes; ++k) {
        large_num id = nodes->at(k)->getProperties().get<large_num, Model::ID>();

        if (reduced) {
            nodes->at(k)->setHead(changes[id] * si::meter);
        } else {
            auto m = index_mapping[id];
            if (m != -1) {
                nodes->at(k)->setHead(changes[m] * si::meter);
            }
        }
    }
}

void inline
Equation::updateFinalHeads() {
#pragma omp parallel for
    for (large_num k = 0; k < numberOfNodes; ++k) {
        nodes->at(k)->updateHeadChange();
    }
}

void inline
Equation::updateBudget() {
#pragma omp parallel for
    for (large_num k = 0; k < numberOfNodes; ++k) {
        nodes->at(k)->saveMassBalance();
    }
}

/**
 * Solve Equation
 *
 */
void
Equation::solve() {

    LOG(numerics) << "Updating Matrix\n";
    updateMatrix();

    if (!isCached) {
        LOG(numerics) << "Compressing matrix\n";
        if (disable_dry_cells) {
            _A_.makeCompressed();
        } else {
            A.makeCompressed();
        }
        LOG(numerics) << "Cached Matrix\n";
        isCached = true;
    }

    preconditioner();
    if (disable_dry_cells) {
        adaptiveDamping = AdaptiveDamping(dampMin, dampMax, maxHeadChange, _x_);
    } else {
        adaptiveDamping = AdaptiveDamping(dampMin, dampMax, maxHeadChange, x);
    }

    std::cout << "Running Time Step\n";
    // Returns true if max headchange is greater as defined val
    auto isHeadChangeGreater = [this]() -> bool {
        double lowerBound = maxHeadChange;
        double changeMax = 0;
#pragma omp parallel for
        for (large_num k = 0; k < numberOfNodes; ++k) {
            double
                    val = std::abs(
                    nodes->at(k)->getProperties().get<quantity<Model::Meter>, Model::HeadChange>().value());
            changeMax = (val > changeMax) ? val : changeMax;
        }
        LOG(numerics) << "MAX Head Change: " << changeMax;
        return changeMax > lowerBound;
    };

    long iterations{0};
    bool headFail{false};
    char smallHeadChanges{0};
    bool headConverged{false};
    while (iterations < IITER) {
        LOG(numerics) << "Outer iteration: " << iterations;

        //Solve inner iterations
        if (nwt) {
            if (disable_dry_cells) {
                _x_ = bicgstab.solveWithGuess(_b_, _x_);
            } else {
                x = bicgstab.solveWithGuess(b, x);
            }
        } else {
            if (disable_dry_cells) {
                _x_ = cg.solveWithGuess(_b_, _x_);
            } else {
                x = cg.solveWithGuess(b, x);
            }
        }

        updateIntermediateHeads();
        int innerItter{0};
        if (nwt) {
            innerItter = bicgstab.iterations();
        } else {
            innerItter = cg.iterations();
        }

        if (innerItter == 0 and iterations == 0) {
            LOG(numerics) << "convergance criterion to small - no iterations";
            break;
        }

        /**
         * @brief head change convergance
         */
        headFail = isHeadChangeGreater();
        if (headFail) {
            //convergence is not reached
            //reset counter
            smallHeadChanges = 0;
        } else {
            //itter converged with head criterion
            if (headConverged and smallHeadChanges == 0) {
                LOG(numerics) << "Conditional convergence - check mass balance";
                break;
            } else {
                headConverged = true;
                smallHeadChanges++;

                if (smallHeadChanges >= 3) {
                    LOG(numerics) << "Reached head change convergence";
                    break;
                }
            }
        }

        /**
         * @brief residual norm convergance
         */
        if (nwt) {
            LOG(numerics) << "Inner iterations: " << innerItter;
            if (bicgstab.info() == Success) {
                break;
            }
        } else {
            LOG(numerics) << "Inner iterations: " << innerItter;
            if (cg.info() == Success and iterations != 0) {
                break;
            }
        }

        if (nwt) {
            //LOG(numerics) << "Residual squared norm error " << bicgstab.error();
            //LOG(numerics) << "Head change bigger: " << headFail;
        } else {
            //LOG(numerics) << "Residual Inf norm error " << cg.error_inf();
            //LOG(numerics) << "Head change bigger: " << headFail;
        }

        updateMatrix();
        preconditioner();

        /*IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");

        MatrixXd dMat( A );
        std::cout << dMat.format(CleanFmt);
        std::cout << "-----\n------\n------\n";
        std::cout << b.format(CleanFmt);*/

        iterations++;
    }

    if (iterations == IITER) {
        std::cerr << "Fail in solving matrix with max iterations \n";
        if (nwt) {
            //LOG(numerics) << "Residual squared norm error " << bicgstab.error();
        } else {
            //LOG(numerics) << "Residual Inf norm error " << cg.error_inf();
        }
    }

    updateFinalHeads();
    updateBudget();

    __itter = iterations;
    __error = nwt ? bicgstab.error() : std::numeric_limits<float>::max();
}

int
Equation::getItter() {
    return __itter;
}

double
Equation::getError() {
    return __error;
}

}
}//ns

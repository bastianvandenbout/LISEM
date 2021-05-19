#include "Simulation.hpp"
using namespace std;
namespace GlobalFlow {
namespace Simulation {

Simulation::Simulation(Options op, DataReader *reader) : op(op), reader(reader) {
    Eigen::setNbThreads(op.getThreads());

    if (op.cacheEnabled()) {
        serialize = true;
        loadNodes = true;
    }

    NodeVector ptr(new vector<unique_ptr<Model::NodeInterface>>);
    nodes = std::move(ptr);

    int numOfStaticNodes = initNodes();
    LOG(userinfo) << "Creating Equation.. \n";
    eq = new Solver::Equation(numOfStaticNodes, nodes, op);
}

void
Simulation::save() {
    //Serialize current node state
    if (serialize) {
        cout << "Saving nodes for faster reboot .. \n";
        {
            std::ofstream ofs("savedNodes", ios::out | ios::binary);
            boost::archive::binary_oarchive outStream(ofs);
            // write class instance to archive
            outStream << nodes;
        }
        cout << "Nodes saved\n";
    }
}

int Simulation::initNodes() {
    LOG(userinfo) << "Starting G³M";
    nodes->reserve(op.getNumberOfNodes() * op.getNumberOfLayers());
    reader->initNodes(nodes);
    reader->readData(op);
    return op.getNumberOfNodes() * op.getNumberOfLayers();
}

Solver::Equation *
Simulation::getEquation() {
    return eq;
}

}
}

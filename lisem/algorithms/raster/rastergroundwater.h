#ifndef RASTERGROUNDWATER_H
#define RASTERGROUNDWATER_H

#include "geo/raster/map.h"
#include "g3m/Simulation/Simulation.hpp"
#include "g3m/Simulation/Stepper.hpp"
namespace GlobalFlow {
namespace DataProcessing {


class SPHDataReader : public DataReader {
    public:

    std::vector<cTMap * > m_Head;
    std::vector<cTMap * > m_KSat;
    std::vector<cTMap * > m_Yield;
    std::vector<cTMap * > m_Capacity;
    std::vector<cTMap * > m_Confined;
    std::vector<cTMap * > m_Recharge;
    cTMap * m_RiverWidth;
    cTMap * m_RiverKSat;
    cTMap * m_RiverWH;
    cTMap * m_DEM;
    float m_dz;
    int m_stepMod;
    float m_Aniso;


    inline SPHDataReader(cTMap * DEM,std::vector<cTMap * > Head, std::vector<cTMap * > KSat, std::vector<cTMap * > Yield, std::vector<cTMap * > Capacity, std::vector<cTMap * > Confined, std::vector<cTMap * > Recharge,cTMap * RiverWidth, cTMap * RiverKSat, cTMap * RiverWH, float dz, int stepmod, float aniso)
    {
        m_Head = Head;
        m_KSat = KSat;
        m_Yield = Yield;
        m_Capacity =Capacity;
        m_Confined = Confined;
        m_RiverWH = RiverWH;
        m_RiverKSat = RiverKSat;
        m_RiverWidth = m_RiverWidth;
        m_dz = dz;
        m_stepMod = stepmod;
        m_Aniso = aniso;
        m_DEM = DEM;
        m_Recharge = Recharge;

    }

    virtual void readData(Simulation::Options op) {

        std::vector<std::vector<int>> grid;
        grid = readGrid(nodes);

        DataProcessing::buildByGrid(nodes, grid, op.getNumberOfLayers(), op.getOceanConduct(),
                                    op.getBoundaryCondition());
    }

    template<class T>
    using Matrix = std::vector<std::vector<T>>;

    Matrix<int>
    readGrid(NodeVector nodes)
    {
        Matrix<int> out = Matrix<int>(m_Head.at(0)->nrRows(), std::vector<int>(m_Head.at(0)->nrCols()));


        int layersize = m_Head.at(0)->nrRows() * m_Head.at(0)->nrCols();

        double dx = std::fabs(m_Head.at(0)->cellSizeX()) * 180.0/(3.14159 * 6371000.0);
        double dy = std::fabs(m_Head.at(0)->cellSizeY()) * 180.0/(3.14159 * 6371000.0);

        lookupglobIDtoID.reserve(m_Head.at(0)->nrRows() * m_Head.at(0)->nrCols() * m_Head.size());

        for(int r = 0; r < m_Head.at(0)->nrRows(); r++)
        {
            for(int c = 0; c < m_Head.at(0)->nrCols(); c++)
            {
                for(int l = 0; l < m_Head.size(); l++)
                {
                    int i = r * m_Head.at(0)->nrCols() + c;
                    int id = i + l * layersize;

                     if( l == 0)
                     {
                         out[r][c] = i;
                     }

                     double x = ((double)(c))*dx;
                     double y = ((double)(c))*dy;

                     double areat = std::fabs(m_Head.at(0)->cellSizeX()) * std::fabs(m_Head.at(0)->cellSizeY());


                     lookupglobIDtoID[id] = id;
                     Model::StandardNode * node = new Model::StandardNode(nodes,
                                                                                x,
                                                                                y,
                                                                                areat *  Model::si::square_meter,
                                                                            (unsigned long) id,
                                                                            id,
                                                                            m_KSat.at(l)->data[r][c]* (Model::si::meter / Model::day),
                                                                            m_stepMod,
                                                                            m_dz,
                                                                            m_Aniso,
                                                                            m_Yield.at(l)->data[r][c],
                                                                            m_Capacity.at(l)->data[r][c], m_Confined.at(l)->data[r][c]);
                     if(l == 0)
                     {
                         node->setElevation(m_DEM->data[r][c]* Model::si::meter);

                         if(m_RiverWidth->data[r][c] > 0.0)
                         {
                             node->addExternalFlow(Model::RIVER, m_RiverWH->data[r][c] * Model::si::meter, m_RiverKSat->data[r][c],
                                                           m_RiverWidth->data[r][c] * Model::si::meter);

                         }
                         if(m_Recharge.at(l)->data[r][c] > 0.0)
                         {
                             node->addExternalFlow(Model::RECHARGE,
                                                             0 * Model::si::meter,
                                                             m_Recharge.at(l)->data[r][c] * node->getProperties().get<Model::quantity<Model::SquareMeter>,                                                           Model::Area > ().value(),
                                                             0 * Model::si::meter);
                         }
                     }


                     if (l > 1) {
                         //Layer above is not top layer
                         nodes->at(id)->setNeighbour(i + ((l-1) * layersize), Model::TOP);
                         nodes->at(i + ((l-1) * layersize))->setNeighbour(id, Model::DOWN);
                     } else if( l > 0){
                         //Layer above is top layer
                         nodes->at(id)->setNeighbour(i, Model::TOP);
                         nodes->at(i)->setNeighbour(id, Model::DOWN);
                     }
                }

            }
        }





        return out;
    }

};


}
}

using namespace GlobalFlow;

inline static std::vector<cTMap *> AS_GroundWater3DModel(cTMap * DEM,std::vector<cTMap *> in_Head, std::vector<cTMap *> in_KSat, std::vector<cTMap *> in_SpecificYield,std::vector<cTMap *> in_SpecificCapacity, std::vector<cTMap *> in_Confined, std::vector<cTMap *> in_Recharge, QString BoundaryCondition, cTMap * RiverWidth, cTMap * RiverKSat, cTMap * RiverWH, float dz, bool steadystate, bool boundary_sea)
{
    Simulation::Options op;
    Simulation::Simulation sim;

    op = Simulation::Options();

    std::vector<cTMap *> Head = in_Head;
    std::vector<cTMap *> KSat = in_KSat;
    std::vector<cTMap *> Yield = in_SpecificYield;
    std::vector<cTMap *> Capacity = in_SpecificCapacity;
    std::vector<cTMap *> Confined = in_Confined;
    std::vector<cTMap *> Recharge = in_Recharge;


    op.ROW_COLS = true;
    op.NUMBER_OF_NODES = Head.at(0)->nrCols() * Head.at(0)->nrRows();
    op.THREADS = omp_get_num_threads();
    op.LAYERS = Head.size();
    op.SOLVER = "PCG";
    op.IITER = 500;
    op.I_ITTER = 10;
    op.RCLOSE = 1e-8;
    op.MAX_HEAD_CHANGE = 0.0001;
    op.DAMPING = false;
    op.MAX_DAMP = 0.5;
    op.MIN_DAMP = 0.01;
    op.stepsize = Simulation::Stepsize::DAILY;
    op.WETTING_APPROACH = "nwt";
    op.CACHE = false;
    op.SENSITIVITY = false;
    if(boundary_sea)
    {
        op.BOUNDARY_CONDITION = Simulation::Options::BoundaryCondition::CONSTANT_HEAD_SEA_LEVEL;

    }else
    {
        op.BOUNDARY_CONDITION =  Simulation::Options::BoundaryCondition::CONSTANT_HEAD_NEIGHBOUR;
    }
    op.ADAPTIVE_STEPSIZE = false;
    op.OCEAN_K = 800;
    op.ANISOTROPY = 10.0;

    DataProcessing::SPHDataReader *reader = new DataProcessing::SPHDataReader(DEM,Head,KSat,Yield,Capacity,Confined,Recharge,RiverWidth,RiverKSat,RiverWH,dz,op.getStepsizeModifier(),op.getAnisotropy());
    sim = Simulation::Simulation(op,reader);
    Solver::Equation *_eq = sim.getEquation();


    Simulation::Stepper stepper = Simulation::Stepper(_eq, Simulation::DAY, 1);
    for (Simulation::step step : stepper) {
        if(steadystate)
        {
            step.first->toogleSteadyState();
        }
        step.first->solve();
    }


    std::vector<cTMap *> head_out;

    for(int i = 0; i < Head.size(); i++)
    {

        head_out.push_back(Head.at(i)->GetCopy());
        for(int r = 0; r < head_out.at(i)->nrRows(); r++)
        {
            for(int c = 0;c < head_out.at(i)->nrCols(); c++)
            {
                //set pressure head value for output
                Model::NodeInterface * n = sim.getNodes()->at(r*Head.at(0)->nrCols() + c + (i * Head.at(0)->nrRows() * Head.at(0)->nrCols())).get();
                GlobalFlow::Model::quantity<GlobalFlow::Model::Meter> h = n->get<GlobalFlow::Model::quantity<GlobalFlow::Model::Meter>,GlobalFlow::Model::Head>();
                 head_out.at(i)->data[r][c] = h.value();
            }
        }
    }

    // Obtain a pointer to the engine
    return head_out;

}





/* pt::ptree tree;
pt::read_json(filename, tree);
tree = tree.get_child("config");

pt::ptree config = tree.get_child("model_config");
NODES = config.get<string>("nodes");
ROW_COLS = config.get<bool>("row_cols");
NUMBER_OF_NODES = config.get<long>("numberofnodes");
THREADS = config.get<int>("threads");
LAYERS = config.get<int>("layers");
CONFINED = getTypeArray<bool>("confinement", config);
if (LAYERS != CONFINED.size()) {
    LOG(critical) << "mismatching layers";
    exit(3);
}
CACHE = config.get<bool>("cache");
ADAPTIVE_STEPSIZE = config.get<bool>("adaptivestepsize");
BOUNDARY_CONDITION = config.get<string>("boundarycondition");
SENSITIVITY = config.get<bool>("sensitivity");

pt::ptree numerics = tree.get_child("numerics");
SOLVER = numerics.get<string>("solver");
IITER = numerics.get<int>("iterations");
I_ITTER = numerics.get<int>("inner_itter");
RCLOSE = numerics.get<double>("closingcrit");
MAX_HEAD_CHANGE = numerics.get<double>("headchange");
DAMPING = numerics.get<bool>("damping");
MIN_DAMP = numerics.get<double>("min_damp");
MAX_DAMP = numerics.get<double>("max_damp");
string tmp = numerics.get<string>("stepsize");
if (tmp == "DAILY") {
    stepsize = DAILY;
}
if (tmp == "MONTHLY") {
    stepsize = MONTHLY;
}
WETTING_APPROACH = numerics.get<string>("wetting_approach");

pt::ptree input = tree.get_child("input");

//BASE_PATH = input.get<string>("base_path");

pt::ptree data_config = input.get_child("data_config");
k_from_lith = data_config.get<bool>("k_from_lith");
k_ocean_from_file = data_config.get<bool>("k_ocean_from_file");
specificstorage_from_file = data_config.get<bool>("specificstorage_from_file");
specificyield_from_file = data_config.get<bool>("specificyield_from_file");
k_river_from_file = data_config.get<bool>("k_river_from_file");
aquifer_depth_from_file = data_config.get<bool>("aquifer_depth_from_file");
//heads_from_file = data_config.get<bool>("initial_head_from_file");

bool asArray = data_config.get<bool>("data_as_array");

pt::ptree default_data = input.get_child("default_data");
K = default_data.get<double>("K");
INITAL_HEAD = default_data.get<double>("initial_head");
OCEAN_K = default_data.get<double>("oceanK");
AQUIFER_DEPTH = getTypeArray<int>("aquifer_thickness", default_data);
ANISOTROPY = default_data.get<double>("anisotropy");
SPECIFIC_YIELD = default_data.get<double>("specificyield");
SPECIFIC_STORAGE = default_data.get<double>("specificstorage");

pt::ptree data = input.get_child("data");

if (asArray) {
    //ELEVATION_a = getArray("Elevation", data.get_child("elevation"));
    SLOPE_a = getArray("Slope", data.get_child("slope"));
    //EQ_WTD_a = getArray("WTD", data.get_child("eq_wtd"));
    EFOLDING_a = getArray("E-Folding", data.get_child("e-folding"));
    //BLUE_ELEVATION_a = getArray("Blue", data.get_child("blue_cells"));
} else {
    EFOLDING = getOptional("e-folding", data);
    SLOPE = getOptional("slope", data);
}

ELEVATION = getOptional("elevation", data);
EQ_WTD = getOptional("eq_wtd", data);
BLUE_ELEVATION = getOptional("blue_cells", data);


LITHOLOGY = getOptional("lithologie", data);
RECHARGE = getOptional("recharge", data);
RIVERS = getOptional("rivers", data);
GLOBAL_WETLANDS = getOptional("globalwettlands", data);
GLOBAL_LAKES = getOptional("globallakes", data);
LOCAL_LAKES = getOptional("lokallakes", data);
LOCAL_WETLANDS = getOptional("lokalwettlands", data);

//Optional
K_DIR = getOptional("conductance", data);
RIVER_K_DIR = getOptional("river_conductance", data);
OCEAN_K_DIR = getOptional("ocean_conductance", data);
SS_FILE = getOptional("specificstorage", data);
SY_FILE = getOptional("specificyield", data);
AQ_DEPTH = getOptional("aquiferdepth", data);

boost::optional<pt::ptree &> mappings = input.get_child_optional("mapping");
if (mappings) {
    NODEID_ARCID = mappings.value().get<string>("nodeID-arcID");
}
*/


//aquifer thickness?

/*{
  "config": {
    "model_config": {
      "nodes": "grid_simple.csv",
      "row_cols": "true",
      "stadystate": "true",
      "numberofnodes": 100,
      "threads": 1,
      "layers": 2,
      "confinement": [
        "false",
        "true"
      ],
      "cache": "false",
      "adaptivestepsize": "false",
      "boundarycondition": "SeaLevel",
      "sensitivity": "false"
    },
    "numerics": {
      "solver": "PCG",
      "iterations": 500,
      "inner_itter": 10,
      "closingcrit": 1e-8,
      "headchange": 0.0001,
      "damping": "false",
      "min_damp": 0.01,
      "max_damp": 0.5,
      "stepsize": "daily",
      "wetting_approach": "nwt"
    },
  "input": {
    "data_config": {
      "k_from_lith": "true",
      "k_ocean_from_file": "false",
      "specificstorage_from_file": "false",
      "specificyield_from_file": "false",
      "k_river_from_file": "true",
      "aquifer_depth_from_file": "false",
      "initial_head_from_file": "true",
      "data_as_array": "false"
    },
    "default_data": {
      "initial_head": 5,
      "K": 0.008,
      "oceanK": 800,
      "aquifer_thickness": [
        10,
        10
      ],
      "anisotropy": 10,
      "specificyield": 0.15,
      "specificstorage": 0.000015
    },
    "data": {
      "recharge": "recharge_simple.csv",
      "elevation": "elevation_simple.csv",
      "rivers": "rivers_simple.csv",
      "lithologie": "lithology_simple.csv",
      "river_conductance": "rivers_simple.csv",
      "initial_head": "heads_simple.csv"
    }
  }
  }
}*/








#endif // RASTERGROUNDWATER_H

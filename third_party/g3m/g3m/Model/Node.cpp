#include "Node.hpp"

BOOST_CLASS_IMPLEMENTATION(GlobalFlow::Model::ExternalFlow, boost::serialization::object_serializable);
BOOST_CLASS_IMPLEMENTATION(GlobalFlow::Model::NodeInterface, boost::serialization::object_serializable);

namespace GlobalFlow {
namespace Model {

/**
 * Initialize the physical properties with default values
 * @return
 */
PhysicalProperties initProperties() {
    PhysicalProperties fields;
    fields.emplace < unsigned
    long
    int, ID > (0);
    fields.emplace < unsigned
    long
    int, ArcID > (0);
    fields.emplace<double, Lat>(0);
    fields.emplace<double, Lon>(0);
    fields.emplace<int, Layer>(0);
    fields.emplace<quantity<Dimensionless>, StepModifier>(1 * si::si_dimensionless);
    fields.emplace<quantity<SquareMeter>, Area>(0 * si::square_meter);
    fields.emplace<quantity<Meter>, Elevation>(5 * si::meter);
    fields.emplace<quantity<Meter>, TopElevation>(5 * si::meter);
    fields.emplace<quantity<Meter>, VerticalSize>(10 * si::meter);
    fields.emplace<quantity<Dimensionless>, Slope>(0 * si::si_dimensionless);
    fields.emplace<quantity<Meter>, EFolding>(1 * si::meter);
    fields.emplace<bool, Confinement>(true);
    fields.emplace<quantity<Velocity>, K>(0.03 * (si::meter / day));
    fields.emplace<quantity<Dimensionless>, Anisotropy>(10 * si::si_dimensionless);
    fields.emplace<quantity<d_time>, StepSize>(1 * day);
    fields.emplace<quantity<CubicMeter>, OUT>(0.0 * si::cubic_meter);
    fields.emplace<quantity<CubicMeter>, IN>(0.0 * si::cubic_meter);
    fields.emplace<quantity<Meter>, Head>(3 * si::meter);
    fields.emplace<quantity<Meter>, EQHead>(1 * si::meter);
    fields.emplace<quantity<Meter>, HeadChange>(0 * si::meter);
    fields.emplace<quantity<Meter>, Head_TZero>(0 * si::meter);
    fields.emplace<quantity<Meter>, HeadChange_TZero>(0 * si::meter);

    return fields;
}

NodeInterface::NodeInterface(NodeVector nodes,
                             double lat,
                             double lon,
                             quantity<SquareMeter> area,
                             unsigned long int arcID,
                             unsigned long int identifier,
                             quantity<Velocity> conduct,
                             int stepModifier,
                             double aquiferDepth,
                             double anisotropy,
                             double specificYield,
                             double specificStorage,
                             bool confined) : nodes(nodes) {
    fields = initProperties();
    fields.set<double, Lat>(lat);
    fields.set<double, Lon>(lon);
    fields.set<quantity<SquareMeter>, Area>(area);
    fields.set < unsigned
    long
    int, ArcID > (arcID);
    fields.set < unsigned
    long
    int, ID > (identifier);
    fields.set<quantity<Velocity>, K>(conduct);
    fields.set<bool, Confinement>(confined);
    fields.set<quantity<Dimensionless>, StepModifier>(stepModifier * si::si_dimensionless);
    fields.emplace<quantity<Dimensionless>, SpecificYield>(specificYield * si::si_dimensionless);
    fields.emplace<quantity<perUnit>, SpecificStorage>(specificStorage * perMeter);
    fields.emplace<quantity<Meter>, VerticalSize>(aquiferDepth * si::meter);
    fields.emplace<quantity<Dimensionless>, Anisotropy>(anisotropy * si::si_dimensionless);

    fields.emplace<quantity<Meter>, EdgeLenght>(sqrt(fields.get<quantity<SquareMeter>, Area>()));
    fields.emplace<quantity<SquareMeter>, SideSurface>(
            fields.get<quantity<Meter>, EdgeLenght>() * fields.get<quantity<Meter>, VerticalSize>());
    fields.emplace<quantity<CubicMeter>, VolumeOfCell>(
            fields.get<quantity<SquareMeter>, Area>() * fields.get<quantity<Meter>, VerticalSize>());
}
}
}//ns

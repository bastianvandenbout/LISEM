#ifndef PARAMETERMANAGER_H
#define PARAMETERMANAGER_H
#include "defines.h"
#include <QString>
#include <QList>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <iostream>
#include <QCoreApplication>
#include "error.h"
#include "site.h"

enum ParamterType
{
    LISEM_PARAMETER_LABEL,
    LISEM_PARAMETER_DIR,
    LISEM_PARAMETER_FILE,
    LISEM_PARAMETER_MAP,
    LISEM_PARAMETER_FLOAT,
    LISEM_PARAMETER_INT,
    LISEM_PARAMETER_BOOL,
    LISEM_PARAMETER_MULTICHECK


};

class ParameterManager;

struct SPHParameter
{
    ParamterType m_Type;
    QString m_Name;
    QList<QString> m_Labels;
    QString m_Description;
    QString m_ValueDefault;
    QString m_Value;
    double m_minrange = 0;
    double m_maxrange = 1;
    double m_mult = 0;
    bool m_hasmult = false;
    int m_level = 0;

    std::function<bool(ParameterManager*)> lambda_req;
    std::function<bool(ParameterManager*, QString)> lambda_act;

};

class LISEM_API ParameterManager
{
public:

    QList<SPHParameter> m_Parameters;


public:

    ParameterManager();

    inline void InitParameters()
    {
        m_Parameters.clear();

        QString cd = GetSite()+"/";

        AddLabel(-1,"","");
        AddLabel(0,"Settings","Global, non-spatial settings for the simulation");
        AddLabel(1,"Time","Temporal domain of simulation");
        AddParameter(2, LISEM_PARAMETER_FLOAT, "Start Time",             QList<QString>(),                                   "Starting time for the simulation (minutes)",               0.0f,               0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_FLOAT, "End Time",               QList<QString>(),                                   "Ending time for the simulation (minutes)",                 100.0f,             0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_FLOAT, "Timestep",               QList<QString>(),                                   "Timestep for the simulation (seconds)",                    10.0f,              0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_FLOAT, "Timestep Flow",          QList<QString>(),                                   "Timestep for the flow simulation (seconds)",               1.0f,              0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_FLOAT, "Timestep Particle Flow",          QList<QString>(),                                   "Timestep for the flow simulation (seconds)",      0.001f,              0.0f,   0.0f,   0.0f ,false,[](ParameterManager*p){return p->GetParameterValueBool("Use Material Point Method");} );

        AddLabel(1,"TimeInput Directory","Folder containing temporal input");
        AddParameter(2, LISEM_PARAMETER_DIR,   "TimeInput Directory",    QList<QString>(),                             "",                                                         cd +"time/",        0.0f,   0.0f,   0.0f   );
        AddLabel(1,"Map Directory","Folder containing input maps");
        AddParameter(2, LISEM_PARAMETER_DIR,   "Map Directory",          QList<QString>(),                                   "",                                                         cd +"maps/",        0.0f,   0.0f);
        AddLabel(1,"Output Directory","Folder containing output maps and files");
        AddParameter(2, LISEM_PARAMETER_DIR,   "Output Directory",       QList<QString>(),                                   "",                                                         cd +"res/",        0.0f,   0.0f,   0.0f   );

        AddLabel(1,"Processes","Select the active processes during the simulation");
        AddParameter(2, LISEM_PARAMETER_BOOL, "Include Rainfall",        QList<QString>(),                                   "Include Rainfall",                                     "0",                0.0f,   0.0f,   0.0f  );
        AddParameter(2, LISEM_PARAMETER_BOOL, "Include Infiltration",    QList<QString>(),                                   "Include Infiltration",                                     "0",                0.0f,   0.0f,   0.0f  ,false, [](ParameterManager*p){return true;}, [](ParameterManager*p, QString val){ if(val.toInt() == 1){if(p->GetParameterValueBool("Include Hydrology") == true){ p->SetParameterValue("Include Hydrology",false); return true;}}return false;} );
        AddParameter(2, LISEM_PARAMETER_BOOL, "Include Hydrology",       QList<QString>(),                                   "Include Infiltration and surface storages",                "0",                0.0f,   0.0f,   0.0f  ,false,  [](ParameterManager*p){return true;}, [](ParameterManager*p, QString val){ if(val.toInt() == 1){if(p->GetParameterValueBool("Include Infiltration") == true){ p->SetParameterValue("Include Infiltration",false); return true;}}return false;} );
        AddParameter(2, LISEM_PARAMETER_BOOL, "Custom Infiltration",       QList<QString>(),                                   "Do not use emprical relatinoships but ask for infiltration properties directly",                "0",                0.0f,   0.0f,   0.0f  ,false,  [](ParameterManager*p){return true;}, [](ParameterManager*p, QString val){ if(val.toInt() == 1){if(p->GetParameterValueBool("Include Hydrology") == true){return true;}}return false;} );
        AddParameter(2, LISEM_PARAMETER_BOOL, "Include Evapotranspiration",QList<QString>(),                                   "Include Evapotranspiration",                            "0",                0.0f,   0.0f,   0.0f  ,false, [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology");} );
        AddParameter(2, LISEM_PARAMETER_BOOL, "Include GroundWater Flow",QList<QString>(),                                   "Include Ground water flow",                                "0",                0.0f,   0.0f,   0.0f ,false, [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology");} );
        AddParameter(2, LISEM_PARAMETER_BOOL, "Include Channel",         QList<QString>(),                                   "Include 1-d Channel Network water flow",                   "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL, "Include Erosion",         QList<QString>(),                                   "Include surface sediment erosion",                         "0",                0.0f,   0.0f,   0.0f   );
        //AddParameter(2, LISEM_PARAMETER_BOOL, "Include Second Soil Layer", QList<QString>(),                                 "Describe a full second layer of soil with properties",     "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL, "Include Loose Material Layer", QList<QString>(),                            "Describe a saturated layer of loose material on top of soil", "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL, "Include Slope Stability", QList<QString>(),                                   "Include Slope Stability",                                  "0",                0.0f,   0.0f,   0.0f   ,false, [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology");} );
        AddParameter(2, LISEM_PARAMETER_BOOL, "Include Slope Failure",   QList<QString>(),                                   "Include Slope Failure",                                    "0",                0.0f,   0.0f,   0.0f  ,false, [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology") &&p->GetParameterValueBool("Include Slope Stability")  ;} );
        AddParameter(2, LISEM_PARAMETER_BOOL, "Include Initial Stability",QList<QString>(),                                  "Include Initial Stability",                                    "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL, "Include Initial Water",   QList<QString>(),                                   "Include Initial water",                                    "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL, "Include Initial Solids",  QList<QString>(),                                   "Include Initial solids",                                   "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL, "Include Seismic Trigger",  QList<QString>(),                                  "Include Seismic Trigger",                                   "0",                0.0f,   0.0f,   0.0f  ,false, [](ParameterManager*p){return p->GetParameterValueBool("Include Slope Stability");} );
        AddParameter(2, LISEM_PARAMETER_BOOL, "Include Rigid-Body Phyisics",  QList<QString>(),                              "Include Rigid-Body Phyisics",                                   "0",                0.0f,   0.0f,   0.0f   );
        AddLabel(1,"Flow Settings","Numerical Settings for Flow Simulation");
        AddParameter(2, LISEM_PARAMETER_BOOL, "Use Material Point Method",  QList<QString>(),                                "Simulate Solids using Material Point Method",              "0",                0.0f,   0.0f,   0.0f   );
        AddLabel(1,"Compute Settings","Computational Settings");
        AddParameter(2, LISEM_PARAMETER_BOOL, "Use CPU",  QList<QString>(),                                "Runs the simulation on the CPU (central processing unit) instead of the GPU (graphical processing unit). Usefull when no dedicated, fast, GPU is present in the system.",              "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL, "Limit Memory Usage",  QList<QString>(),                                "Limits memory usage by showing less output during the simulation (can aid simulation of larger areas)",              "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL, "Simulate Subsection",  QList<QString>(),                                "simulate only a subset of the input data (requires a mask input map)",              "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_INT,  "SubSection Value",           QList<QString>(),                         "The value within the mask input map for which to perform the simulation",                    1,                 0.0f,   0.0f,   0.0f   );

        AddLabel(-1,"","");
        AddLabel(0,"Parameters","Global, physical parameters for the simulation");
        AddLabel(1,"Water Flow","Physical parameters for water flow");
        AddParameter(2, LISEM_PARAMETER_FLOAT, "Drag coefficient multiplier",             QList<QString>(),                  "Multiplier for the drag coefficient",               1.0f,               0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_FLOAT, "Reynolds Number Multiplier",             QList<QString>(),                   "Reynollds number multiplier",               1.0f,               0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL, "Custom Channel Slope",             QList<QString>(),                   "Custom channel slope",               1.0f,               0.0f,   0.0f,   0.0f   );
        AddLabel(1,"Solid Flow","Physical parameters for solid flow");
        AddParameter(2, LISEM_PARAMETER_FLOAT, "Initial Stability Margin",             QList<QString>(),                     "Initial Stability Margin (-)",               0.005f,               0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_FLOAT, "Elastic Modulus",             QList<QString>(),                              "Elastic Modulus (mPa)",               10000000000000.0f,               0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_FLOAT, "Shear Modulus",             QList<QString>(),                                "Shear Modulus (mPa)",               1000000000000.0f,               0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_FLOAT, "Particle Fractional Dimension",             QList<QString>(),                "fraction of cell size to use for particle volume (-)",              0.2f,              0.0001f,   100.0f,  0.0f   );
        AddLabel(1,"Numerical","Numerical settings for ");

        AddLabel(-1,"","");
        AddLabel(0,"Time Input","Temporal input describing climate and other boundary conditions");
        AddLabel(1,"Climate Input","Files containing timeseries describing weather");
        AddParameter(2, LISEM_PARAMETER_FILE,   "Rainfall",          QList<QString>(),                                  "",                                                         "rain",              0.001f,   999.00f,  1.0,true,[](ParameterManager*p){return p->GetParameterValueBool("Include Rainfall");} );
        AddParameter(2, LISEM_PARAMETER_FILE,   "Temperature",          QList<QString>(),                                  "",                                                      "temp",             0.001f,   999.00f,  1.0,true ,[](ParameterManager*p){return p->GetParameterValueBool("Include Evapotranspiration");} );
        AddParameter(2, LISEM_PARAMETER_FILE,   "Radiation",          QList<QString>(),                                  "",                                                        "rad",              0.001f,   999.00f,  1.0,true ,[](ParameterManager*p){return p->GetParameterValueBool("Include Evapotranspiration");} );
        AddParameter(2, LISEM_PARAMETER_FILE,   "Wind",          QList<QString>(),                                  "",                                                             "wind",             0.001f,   999.00f,  1.0,true ,[](ParameterManager*p){return p->GetParameterValueBool("Include Evapotranspiration");} );
        AddParameter(2, LISEM_PARAMETER_FILE,   "Vapor Pressure",          QList<QString>(),                                  "",                                                   "vap",              0.001f,   999.00f,  1.0,true ,[](ParameterManager*p){return p->GetParameterValueBool("Include Evapotranspiration");} );
        AddParameter(2, LISEM_PARAMETER_FILE,   "ndvi",          QList<QString>(),                                  "",                                                             "ndvi",              0.001f,   999.00f,  1.0,true ,[](ParameterManager*p){return p->GetParameterValueBool("Include Evapotranspiration");} );

        AddLabel(-1,"","");
        AddLabel(0,"Input Maps","Spatial input data that describes terrain, surface properties and boundary conditions");
        AddLabel(1,"Topography","Topography maps");
        AddParameter(2, LISEM_PARAMETER_MAP,   "DEM",                    QList<QString>({QString("Elevation Model")}),       "Elevation model (meters)",                                 "dem.map",          0.0f,   0.0f );
        AddParameter(2, LISEM_PARAMETER_MAP,   "MASK",                    QList<QString>({QString("MASK")}),       "indicates the subsection to be simulated. Only required when this option is selected. Otherwise missing values within the DEM are used for masking.",                                 "mask.map",          0.0f,   0.0f,1.0,false, [](ParameterManager*p){return p->GetParameterValueBool("Simulate Subsection");} );
        AddLabel(1,"Channels","Channels");
        AddParameter(2, LISEM_PARAMETER_MAP,   "Channel",                QList<QString>({QString("Channel Mask")}),           "Elevation model (meters)",                                "channelldd.map",          0.0f,   0.0f,1.0,false, [](ParameterManager*p){return p->GetParameterValueBool("Include Channel");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Channel Width",          QList<QString>({QString("Channel Width")}),          "Channel Width (meters)",                                  "channelwidth.map",                  0.001f,   999.00f,  1.0,true, [](ParameterManager*p){return p->GetParameterValueBool("Include Channel");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Channel Depth",          QList<QString>({QString("Channel Depth")}),          "Channel Depth (meters)",                                  "channeldepth.map",           0.001f,   999.00f,  1.0,true, [](ParameterManager*p){return p->GetParameterValueBool("Include Channel");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Channel Slope",          QList<QString>({QString("Channel Slope")}),          "Channel Depth (meters), only used when custom channel slope is selected",                                  "channelslope.map",           0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Channel");} );

        AddLabel(1,"Surface","Surface (Land use/Land Cover) description");
        AddParameter(2, LISEM_PARAMETER_MAP,   "Manning",                QList<QString>({QString("Mannings N")}),             "Mannings N (in metric units)",                                               "n.map",                     0.001f,   999.00f,  1.0,true  );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Building Cover",         QList<QString>({QString("Building Cover")}),         "Building Cover (fractional)",                                           "buildingcover.map",         0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Road Width",             QList<QString>({QString("Road Width")}),             "Road Width (meters, less then cell size)",                                               "buildingcover.map",         0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Vegetation Height",      QList<QString>({QString("Vegetation Canopy Height")}),       "Vegetation Canopy Height (meters)",                         "vegheight.map",              0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Vegetation Cover",       QList<QString>({QString("Vegetation Cover")}),       "Vegetation Cover (fractional)",                                         "vegcover.map",              0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "SMAX Surface",           QList<QString>({QString("Maximum Surfa. Storage")}), "Maximum Surfa. Storage (meters)",                                   "smaxsurface.map",           0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "SMAX Canopy",            QList<QString>({QString("Maximum Canopy Storage")}), "Maximum Canopy Storage (meters)",                                   "smaxcanopy.map",            0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "CropF",                  QList<QString>({QString("Crop Factor")}), "acts as a multiplication for evapotranspiration depending on crop type",                                   "cropf.map",            0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Evapotranspiration");} );


        AddParameter(2, LISEM_PARAMETER_MAP,   "Tree Density",              QList<QString>({QString("Tree Density")}), "Tree Density",                                   "treedens.map",            0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Tree Instancing");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Tree Length",              QList<QString>({QString("Tree Length")}), "Tree Length",                                   "treelength.map",            0.001f,   999.00f,  1.0,true  , [](ParameterManager*p){return p->GetParameterValueBool("Tree Instancing");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Tree Radius",              QList<QString>({QString("Tree Radius")}), "Tree Radius",                                   "treeradius.map",            0.001f,   999.00f,  1.0,true  , [](ParameterManager*p){return p->GetParameterValueBool("Tree Instancing");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Tree Count",              QList<QString>({QString("Tree Count")}), "Tree Count",                                   "treecount.map",            0.001f,   999.00f,  1.0,true  , [](ParameterManager*p){return p->GetParameterValueBool("Tree Instancing");} );

        AddLabel(1,"SubSurface","SubSurface description");

        AddParameter(2, LISEM_PARAMETER_MAP,   "KSatSimple",             QList<QString>({QString("KSat of soil in top 10 cm")}), "Saturated hydraulic conductivity Value (mm/h)",            "ksatsimple.map",                0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return (p->GetParameterValueBool("Include Infiltration") && !p->GetParameterValueBool("Include Hydrology")) || (p->GetParameterValueBool("Include Hydrology")&& p->GetParameterValueBool("Custom Infiltration"));} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "KSatBottom",             QList<QString>({QString("Ksat of soil that is below top 10 cm")}), "Saturated hydraulic conductivity Value (mm/h)",            "ksatbottom.map",                0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return  (p->GetParameterValueBool("Include Hydrology")&& p->GetParameterValueBool("Custom Infiltration"));} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Clay",                   QList<QString>({QString("Clay content")}),           "Clay content (fraction)",                                  "clay.map",                0.001f,   999.00f,  1.0,true  , [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology") && !p->GetParameterValueBool("Custom Infiltration");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Sand",                   QList<QString>({QString("Sand content")}),           "Sand content (fraction)",                                  "sand.map",                 0.001f,   999.00f,  1.0,true  , [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology")&& !p->GetParameterValueBool("Custom Infiltration");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Gravel",                 QList<QString>({QString("Gravel content")}),         "Gravel content (volumetric cubic meter per cubic meter)",                                           "gravel.map",               0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology")&& !p->GetParameterValueBool("Custom Infiltration");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Organic",                QList<QString>({QString("Organic Matter")}),         "Organic Matter (kg per square meter)",                                           "om.map",                  0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology")&& !p->GetParameterValueBool("Custom Infiltration");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Density",                QList<QString>({QString("Density")}),                "Density (kg/m3)",                                                  "density.map",             0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology")&& !p->GetParameterValueBool("Custom Infiltration");} );

        AddParameter(2, LISEM_PARAMETER_MAP,   "MatricSuction",          QList<QString>({QString("Matric suction")}),                "Matric suction head, meter water equivalent pressure(m)",                                                  "psi.map",             0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology") && p->GetParameterValueBool("Custom Infiltration");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Porosity",               QList<QString>({QString("Porosity")}),                "Porosity (fractional, 0-1)",                                                  "thetas.map",             0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology")&& p->GetParameterValueBool("Custom Infiltration");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "A",                      QList<QString>({QString("A")}),                    "Emperical coefficient in Saxton 2006 pedotransfer functions",                                                  "a.map",             0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology")&& p->GetParameterValueBool("Custom Infiltration");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "B",                      QList<QString>({QString("B")}),                     "Emperical coefficient in Saxton 2006 pedotransfer functions",                                                  "b.map",             0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology")&& p->GetParameterValueBool("Custom Infiltration");} );

        AddParameter(2, LISEM_PARAMETER_MAP,   "Soil Depth",             QList<QString>({QString("Soil Depth")}),            "Soil Depth (m)",                                           "soildepth.map",               0.001f,   999.00f,  1.0,true  , [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Ground Water Height",    QList<QString>({QString("Ground Water Height")}),    "Ground Water Height (m)",                                      "gwh.map",                 0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Theta Initial",          QList<QString>({QString("Theta Initial")}),          "Theta Initial (fraction)",                                            "thetaieff.map",               0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Hydrology");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Cohesion Top",           QList<QString>({QString("Cohesion Top Soil")}),      "Cohesion Top soil Layer (Pa)",                                  "cohtop.map",                0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Erosion") || p->GetParameterValueBool("Include Slope Stability");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Cohesion Channel",       QList<QString>({QString("Cohesion Channel")}),       "Cohesion Top soil Layer within Channel (Pa)",                   "cohch.map",                0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Slope Stability");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Aggregate Stability",    QList<QString>({QString("Aggregate Stability")}),    "Aggregate Stability",                                      "aggrst.map",                 0.001f,   999.00f,  1.0,true ,[](ParameterManager*p){return p->GetParameterValueBool("Include Erosion");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Internal Friction Angle",QList<QString>({QString("Internal Friction Angle")}),"Internal Friction Angle (radians)",                                  "ifa.map",                    0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Slope Stability");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Cohesion Bottom",        QList<QString>({QString("Cohesion Bottom")}),        "Cohesion Bottom (Pa)",                                          "coh.map",                      0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Slope Stability");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Density Bottom",         QList<QString>({QString("Density Bottom")}),         "Density Bottom (Kg/m3)",                                           "densityb.map",                0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Slope Stability");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Rocksize",               QList<QString>({QString("Rocksize")}),               "Rocksize (meters)",                                                 "rocksize.map",                0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Slope Failure");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Material Depth",         QList<QString>({QString("Material Depth")}),         "Depth of saturated material on top of soil",               "materialdepth.map",               0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Loose Material Layer");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Material Internal Friction Angle",QList<QString>({QString("Material Internal Friction Angle")}),"Material Internal Friction Angle (radians)",                                  "materialifa.map",                    0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Loose Material Layer");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Material Density Bottom",         QList<QString>({QString("Material Density Bottom")}),         "Material Density Bottom (kg/m3)",                                           "materialdensity.map",                0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Loose Material Layer");} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Material Rocksize",               QList<QString>({QString("Material Rocksize")}),               "Material Rocksize (meters)",                                                 "materialrocksize.map",                0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Loose Material Layer");} );
        /*AddParameter(2, LISEM_PARAMETER_MAP,   "Clay2",                   QList<QString>({QString("Clay content 2nd Layer")}),    "Clay content (fraction) for second layer",             "clay.map",                0.001f,   999.00f,  1.0,true  );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Sand2",                   QList<QString>({QString("Sand content 2nd Layer")}),    "Sand content (fraction) for second layer",             "sand.map",                 0.001f,   999.00f,  1.0,true  );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Gravel2",                 QList<QString>({QString("Gravel content 2nd Layer")}),  "Gravel content for second layer",                      "gravel.map",               0.001f,   999.00f,  1.0,true  );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Organic2",                QList<QString>({QString("Organic Matter 2nd Layer")}),  "Organic Matter for second layer",                      "om.map",                  0.001f,   999.00f,  1.0,true  );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Density2",                QList<QString>({QString("Density 2nd Layer")}),         "Density for second layer",                             "density.map",             0.001f,   999.00f,  1.0,true  );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Soil Depth2",              QList<QString>({QString("Soil Depth 2nd Layer")}),     "Soil Depth (m) for second layer",                      "soildepth.map",               0.001f,   999.00f,  1.0,true  );
        */


        AddLabel(1,"Boundary Conditions","Boundary conditions for simulation");
        AddParameter(2, LISEM_PARAMETER_MAP,   "Initial Fluid Height",   QList<QString>({QString("Initial Fluid Heigh")}),    "Initial Fluid Heigh (meters)",                                      "initialfh.map",               0.001f,   999.00f,  1.0,true ,[](ParameterManager*p){return p->GetParameterValueBool("Include Initial Water") ;} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Initial Solid Height",   QList<QString>({QString("Initial Solid Height")}),   "Initial Solid Height (meters)",                                     "initialsh.map",                 0.001f,   999.00f,  1.0,true  ,[](ParameterManager*p){return p->GetParameterValueBool("Include Initial Solids") ;} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Initial Solid Density",  QList<QString>({QString("Initial Solid Density")}),  "Initial Solid Density (kg/m3)",                                    "initialsd.map",                0.001f,   999.00f,  1.0,true  ,[](ParameterManager*p){return p->GetParameterValueBool("Include Initial Solids") ;} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Initial Solid IFA",      QList<QString>({QString("Initial Solid IFA")}),      "Initial Solid IFA (radians)",                                        "initialsifa.map",             0.001f,   999.00f,  1.0,true  ,[](ParameterManager*p){return p->GetParameterValueBool("Include Initial Solids") ;} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Initial Solid Rocksize", QList<QString>({QString("Initial Solid Rocksize")}), "Initial Solid Rocksize (meters)",                                   "initialsrocksize.map",          0.001f,   999.00f,  1.0,true  ,[](ParameterManager*p){return p->GetParameterValueBool("Include Initial Solids") ;} );
        AddParameter(2, LISEM_PARAMETER_MAP,   "Initial Cohesion",       QList<QString>({QString("Initial Solid Cohesion")}), "Initial Solid Cohesion (Pa)",                                   "initialscohesion.map",          0.001f,   999.00f,  1.0,true  ,[](ParameterManager*p){return p->GetParameterValueBool("Include Initial Solids") &&  p->GetParameterValueBool("Use Material Point Method");} );

        AddLabel(1,"Seismic","Boundary conditions for seismic activity");
        AddParameter(2, LISEM_PARAMETER_MAP,   "Peak Ground Accaleration",       QList<QString>({QString("Peak Ground Accaleration (m/s2)")}), "Peak Ground Accaleration (m/s2)",                                   "pga.map",          0.001f,   999.00f,  1.0,true , [](ParameterManager*p){return p->GetParameterValueBool("Include Seismic Trigger");} );

        AddLabel(-1,"","");
        AddLabel(0,"Rigid Elements","Elements during the simulation that ");
        AddLabel(1,"Scene","Pre-saves scene which is imported");
        AddLabel(1,"Density Instancing","Add similar objects many times based on density");
        AddParameter(2, LISEM_PARAMETER_BOOL,  "Tree Instancing", QList<QString>(),                                    "Automatically place trees based on tree count, tree radius and tree length input maps",        "0",        0.0f,   0.0f,   0.0f  ,false, [](ParameterManager*p){return p->GetParameterValueBool("Include Rigid-Body Phyisics");} );
        AddParameter(2, LISEM_PARAMETER_FLOAT, "Friction Coefficient",             QList<QString>(),                     "Friction coefficient for trees",               0.005f,               0.0f,   0.0f,   0.0f ,false, [](ParameterManager*p){return p->GetParameterValueBool("Include Rigid-Body Phyisics");} );


        AddLabel(-1,"","");
        AddLabel(0,"Output","Both temporal and non-temporal (min, max or average) output settings for the model");
        AddLabel(1,"Output", "General output settings");
        AddParameter(2, LISEM_PARAMETER_BOOL,  "Output GeoTIFF format", QList<QString>(),                                    "Output maps as GeoTiff (.tif) format, instead of PCRaster (.map)",        "0",        0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL,  "Calculate Max per sub-step", QList<QString>(),                               "Calculate max flow height/vel per calculation sub-step on the gpu (requires more memory)",        "0",        0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL,  "Output Max Height/Vel only", QList<QString>(),                               "Only output maps for maximum flow height/velocity",        "0",        0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL,  "Output totals",          QList<QString>(),                                   "Output totals to map and text file",                       "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL,  "Output hydrographs",          QList<QString>(),                              "Output hydrographs for indicated locations",               "0",                0.0f,   0.0f,   0.0f   );
        AddLabel(1,"Timeseries", "output consisting of lists of maps");
        AddParameter(2, LISEM_PARAMETER_BOOL,  "Output timeseries",      QList<QString>(),                                   "Output timeseries",                                        "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_INT,   "Report every",           QList<QString>(),                                   "How ofter to report dynamic variables",                    1,                 0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_INT,   "Report particles every", QList<QString>(),                                   "How ofter to report particle variables",                   1000,                 0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL,  "Output timeseries of H", QList<QString>(),                                   "Output timeseries of H",                                   "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL,  "Output timeseries of HS",QList<QString>(),                                   "Output timeseries of HS",                                   "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL,  "Output timeseries of HT (total height)", QList<QString>(),                  "Output timeseries of Total Height",                                   "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL,  "Output timeseries of SFRAC",QList<QString>(),                               "Output timeseries of the solid fraction",                                   "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL,  "Output timeseries of V", QList<QString>(),                                   "Output timeseries of V",                                   "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL,  "Output timeseries of VS",QList<QString>(),                                   "Output timeseries of VS",                                   "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL,  "Output timeseries of HT (total averaged velocity)",QList<QString>(),                 "Output timeseries of Total Averaged Velocity",                                   "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL,  "Output timeseries of Infil",QList<QString>(),                                "Output timeseries of Infiltration",                                   "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL,  "Output timeseries of GWH",QList<QString>(),                                  "Output timeseries of Ground Water Height",                                   "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL,  "Output timeseries of Soil Loss",QList<QString>(),                                  "Output timeseries of Ground Water Height",                                   "0",                0.0f,   0.0f,   0.0f   );
        AddParameter(2, LISEM_PARAMETER_BOOL,  "Output timeseries of Sediment Load",QList<QString>(),                                  "Output timeseries of Ground Water Height",                                   "0",                0.0f,   0.0f,   0.0f   );

    }

    inline void Destroy()
    {
        m_Parameters.clear();
    }

private:
    inline void AddParameter(int level, ParamterType type, QString name, QList<QString> Labels, QString Description, double ValueDefault, double minrange=0, double maxrange=0, double mult=1.0, bool has_mult = false, std::function<bool(ParameterManager*)> lambda = [](ParameterManager*p){return true;}, std::function<bool(ParameterManager*, QString)> lambda2 = [](ParameterManager*p, QString val){return false;})
    {

       AddParameter(level, type, name, Labels, Description, QString::number(ValueDefault), minrange, maxrange, mult, has_mult,lambda,lambda2);
    }
    inline void AddParameter(int level, ParamterType type, QString name, QList<QString> Labels, QString Description, QString ValueDefault, double minrange=0, double maxrange=0, double mult=1.0, bool has_mult = false, std::function<bool(ParameterManager*)> lambda = [](ParameterManager*p){return true;}, std::function<bool(ParameterManager*,QString)> lambda2 = [](ParameterManager*p, QString val){return false;})
    {


        SPHParameter p;
        p.m_level = level;
        p.m_Name = name;
        p.m_Type = type;
        p.m_Labels = Labels;
        p.m_Description = Description;
        p.m_ValueDefault = ValueDefault;
        p.m_Value = ValueDefault;
        p.m_minrange = minrange;
        p.m_maxrange = maxrange;
        p.m_mult = mult;
        p.m_hasmult = has_mult;
        p.lambda_act = lambda2;
        p.lambda_req = lambda;

        m_Parameters.append(p);

    }
    inline void AddLabel(int level, QString name, QString Description)
    {
        AddParameter(level, LISEM_PARAMETER_LABEL, name, QList<QString>(), Description,"");
    }

public:

    inline QList<QString> GetParameterList()
    {
        QList<QString> plist;

        for(int i = 0; i < m_Parameters.length();i++)
        {
            SPHParameter p = (m_Parameters.at(i));
            if(p.m_Name.length() > 0 && p.m_Type != LISEM_PARAMETER_LABEL)
            {
                plist.append(p.m_Name + "=" + p.m_Value);
            }
            if((p.m_Type == LISEM_PARAMETER_MAP || p.m_Type == LISEM_PARAMETER_FILE) && p.m_hasmult)
            {
                plist.append(p.m_Name + "_cal_mult" + "=" + QString::number(p.m_mult));
            }

        }

        return plist;

    }

    inline QList<QString> GetParameterListFromFile(QString Path)
    {
        QList<QString> plist;


        QFile fin(Path);
        if (!fin.open(QFile::ReadOnly | QFile::Text)) {

            LISEMS_ERROR("Could not open runfile: " + Path);
            LISEM_ERROR("Could not open runfile: " + Path);
            return plist;
        }

        while (!fin.atEnd())
        {
            QString S = fin.readLine().trimmed();

                if (S.contains("=") && !S.startsWith("["))
                {
                    QStringList SL = S.split(QRegExp("="));

                    if(SL.length() > 1)
                    {
                        for(int i= 0; i < m_Parameters.length(); i++)
                        {
                            SPHParameter p = (m_Parameters.at(i));

                            if(p.m_Name.compare(SL[0].trimmed()) == 0 && p.m_Type != LISEM_PARAMETER_LABEL)
                            {

                                p.m_Value = SL[1].trimmed();

                                if(p.m_hasmult == true)
                                {
                                    if(SL.length() > 2)
                                    {
                                        bool ok = true;
                                        p.m_mult = SL[2].toDouble(&ok);

                                        if(!ok)
                                        {
                                            LISEM_ERROR("Could not read multiplier value in runfile for parameter : " + S);
                                        }
                                    }else {
                                        p.m_mult =1.0f;
                                    }

                                    plist.append(p.m_Name + "_cal_mult" + "=" + QString::number(p.m_mult));
                                }

                                plist.append(p.m_Name + "=" + p.m_Value);
                            }
                        }
                    }else
                    {
                        LISEM_ERROR("Could not read parameter in runfile : " + S);
                    }
                }
        }

        return plist;
    }
    inline QList<QString> GetParameterListFromFile(QString Path, QString ExtraOptions)
    {
        QList<QString> plist = GetParameterListFromFile(Path);

        std::cout << "print all parameters " << std::endl;
        for(int j = 0; j < plist.length();j++)
        {
            std::cout << "par " << j << "  " << plist.at(j).toStdString()<<std::endl;
        }
        QStringList SL = ExtraOptions.split(QRegExp("\\|"));
        for(int i = 0; i < SL.length(); i++)
        {
            QString si = SL[i].trimmed();

            QStringList SL1 = si.split(QRegExp("="));

            if(SL1.length() > 1)
            {
                bool found = false;
                for(int j = 0; j < plist.length();j++)
                {
                     QStringList SL2 = plist.at(j).split(QRegExp("="));
                     if(SL1.length() > 1)
                     {
                        if(SL1[0].compare(SL2[0]) == 0)
                        {
                            found = true;
                            plist.replace(j,si);
                            break;

                        }
                     }
                }

                if(!found)
                {
                    LISEM_ERROR("Could not find additional parameter :" + si);
                }


            }else
            {
                LISEM_ERROR("Could not read additional parameter :" + si);
            }

        }

        return plist;

    }

    inline void SetAllDefault()
    {
        int i = 0;
        for(int i = 0; i < m_Parameters.length();i++)
        {
            SPHParameter p = (m_Parameters.at(i));
            p.m_Value = p.m_ValueDefault;
            m_Parameters.replace(i,p);
        }

    }

    inline int SaveToRunFile(QString name)
    {
        QFile fp(name);
        if (!fp.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            return 1;
        }

        QTextStream out(&fp);
        out << QString("[SPHAZARD RUNFILE VERSION 0.1a]\n");
        for(int i= 0; i < m_Parameters.length(); i++)
        {

            SPHParameter p = (m_Parameters.at(i));
            if(p.m_Name.length() > 0)
            {
                if(p.m_Type == LISEM_PARAMETER_LABEL)
                {
                    out << "\n"<<"[" << p.m_Name << "]"<< "\n";
                }else if(p.m_hasmult == true)
                {
                    out << p.m_Name << "=" << p.m_Value << "=" << QString::number(p.m_mult) <<  "\n";
                }else {
                    out << p.m_Name << "=" << p.m_Value  << "\n";
                }
            }

        }
        fp.close();
        return 0;
    }

    inline int LoadFromRunFile(QString path)
    {
        QFile fin(path);
        if (!fin.open(QFile::ReadOnly | QFile::Text)) {

            return 1;
        }

        while (!fin.atEnd())
        {
            QString S = fin.readLine().trimmed();


                if (S.contains("=") && !S.startsWith("["))
                {


                    QStringList SL = S.split(QRegExp("="));

                    if(SL.length() > 1)
                    {
                        for(int i= 0; i < m_Parameters.length(); i++)
                        {
                            SPHParameter p = (m_Parameters.at(i));

                            if(p.m_Name.compare(SL[0].trimmed()) == 0 && p.m_Type != LISEM_PARAMETER_LABEL)
                            {

                                p.m_Value = SL[1].trimmed();

                                if(p.m_hasmult == true)
                                {
                                    if(SL.length() > 2)
                                    {
                                        bool ok = true;
                                        p.m_mult = SL[2].toDouble(&ok);

                                        if(!ok)
                                        {
                                            LISEM_ERROR("Could not read multiplier value in runfile for parameter : " + S);
                                        }
                                    }else {
                                        p.m_mult =1.0f;
                                    }
                                }

                                m_Parameters.replace(i,p);
                                break;
                            }

                        }
                    }else
                    {
                        LISEM_ERROR("Could not read parameter in runfile : " + S);
                    }

                }

        }

        return 0;

    }

    inline QString GetParameterValueString(QString Name)
    {
        for(int i= 0; i < m_Parameters.length(); i++)
        {
            if(m_Parameters.at(i).m_Name == Name)
            {
                SPHParameter p = (m_Parameters.at(i));
                return p.m_Value;
            }
        }
        return QString("");//QString(LISEM_PARAMETER_ERROR_VALUE);

    }

    inline QString GetParameterValueString(int index)
    {
        if(index > -1 && index < m_Parameters.length())
        {
            SPHParameter p = (m_Parameters.at(index));

            return p.m_Value;
        }
        return QString("");//QString(LISEM_PARAMETER_ERROR_VALUE);

    }


    inline double GetParameterValueDouble(int index)
    {
        if(index > -1 && index < m_Parameters.length())
        {
            SPHParameter p = (m_Parameters.at(index));

            return p.m_Value.toDouble();
        }
        return 0;

    }

    inline float GetParameterValueFloat(int index)
    {
        if(index > -1 && index < m_Parameters.length())
        {
            SPHParameter p = (m_Parameters.at(index));

            return p.m_Value.toDouble();
        }
        return 0;

    }
    inline int GetParameterValueInt(int index)
    {
        if(index > 0 && index < m_Parameters.length())
        {
            SPHParameter p = (m_Parameters.at(index));

            return p.m_Value.toInt();
        }
        return 0;

    }
    inline bool GetParameterValueBool(int index)
    {
        if(index > -1 && index < m_Parameters.length())
        {
            SPHParameter p = (m_Parameters.at(index));

            return p.m_Value.toInt() == 1;
        }
        return 0;

    }


    inline bool GetParameterValueBool(QString Name)
    {
        return GetParameterValueString(Name).toInt() == 1;
    }
    inline int SetParameterValue(int index, QString value)
    {
        if(index > -1 && index < m_Parameters.length())
        {
            SPHParameter p = (m_Parameters.at(index));

            p.m_Value = value;
            m_Parameters.replace(index,p);
            return 0;
        }
        return 1;
    }

    inline int SetParameterValue(int index, int value)
    {
        if(index > -1 && index < m_Parameters.length())
        {
            SPHParameter p = (m_Parameters.at(index));

            p.m_Value = QString::number(value);

            m_Parameters.replace(index,p);
            return 0;
        }
        return 1;
    }

    inline int SetParameterValue(int index, float value)
    {
        if(index > -1 && index < m_Parameters.length())
        {
            SPHParameter p = (m_Parameters.at(index));

            p.m_Value = QString::number(value);

            m_Parameters.replace(index,p);
            return 0;
        }
        return 1;
    }

    inline int SetParameterValue(int index, double value)
    {
        if(index > -1 && index < m_Parameters.length())
        {
            SPHParameter p = (m_Parameters.at(index));

            p.m_Value = QString::number(value);

            m_Parameters.replace(index,p);
            return 0;
        }
        return 1;
    }

    inline int SetParameterValue(int index, bool value)
    {
        if(index > -1 && index < m_Parameters.length())
        {
            SPHParameter p = (m_Parameters.at(index));

            p.m_Value = value? QString("1") : QString("0");

            m_Parameters.replace(index,p);
            return 0;
        }
        return 1;
    }

    template<typename A>
    inline int SetParameterValue(QString name, A value)
    {
        int i = 0;
        for(int i = 0; i < m_Parameters.length();i++)
        {
            if(m_Parameters.at(i).m_Name.compare(name)==0)
            {
                SetParameterValue(i,value);
                return 0;
            }

        }

        return 1;

    }


    inline int SetParameterMultValue(int index, double value)
    {
        if(index > 0 && index < m_Parameters.length())
        {
            SPHParameter p = (m_Parameters.at(index));

            p.m_mult = (value);

            m_Parameters.replace(index,p);
            return 0;
        }
        return 1;
    }

    inline double GetParameterMultValueDouble(int index)
    {
        if(index > 0 && index < m_Parameters.length())
        {
            SPHParameter p = (m_Parameters.at(index));

            return p.m_mult;
        }
        return 0;

    }

    inline bool GetParameterShouldBeActive(int index)
    {
        if(index > 0 && index < m_Parameters.length())
        {
            SPHParameter p = (m_Parameters.at(index));

            return p.lambda_req(this);
        }
        return true;

        //return wether or not the parameter should be active
    }

    inline bool DoParameterActivity(int index)
    {
        if(index > 0 && index < m_Parameters.length())
        {
            SPHParameter p = (m_Parameters.at(index));

            return p.lambda_act(this,p.m_Value);
        }
        return false;


        //returns wether or not a parameter has been altered
    }




};

#endif // PARAMETERMANAGER_H

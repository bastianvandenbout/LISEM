#include "model.h"
#include <iostream>
#include "iogdal.h"
//#include "io.h"
//#include "rastermath.h"
#include "geo/raster/map.h"
#include "error.h"
void LISEMModel::ModelRunInitialize()
{

    LISEM_STATUS("Initial Calculations");
    InitialCalculations();


    //get timeseries

    m_Rain.Clear();
    m_Temp.Clear();
    m_Rad.Clear();
    m_Wind.Clear();
    m_Vapr.Clear();
    m_NDVI.Clear();

    m_Rain.LoadFromFile(m_Dir_Time + GetOptionString("Rainfall"),true,true,GetMapMult("Rainfall"));

    if(m_DoEvapoTranspiration)
    {
        m_Temp.LoadFromFile(m_Dir_Time + GetOptionString("Temperature"),true,true,GetMapMult("Temperature"));
        m_Rad.LoadFromFile(m_Dir_Time + GetOptionString("Radiation"),true,true,GetMapMult("Radiation"));
        m_Wind.LoadFromFile(m_Dir_Time + GetOptionString("Wind"),true,true,GetMapMult("Wind"));
        m_Vapr.LoadFromFile(m_Dir_Time + GetOptionString("Vapor Pressure"),true,true,GetMapMult("Vapor Pressure"));
        m_NDVI.LoadFromFile(m_Dir_Time + GetOptionString("ndvi"),true,true,GetMapMult("ndvi"));
    }

    if(m_DoRigidWorld)
    {
        LISEM_STATUS("Create Rigid Body Physics World ");
        if(m_RigidWorld == nullptr)
        {
            m_RigidWorld = new RigidPhysicsWorld();
        }
        m_RigidWorld->SetSimpleGeoOrigin(LSMVector2(DEM->west(),DEM->north()));

        m_RigidWorld->SetTerrain(DEM);

        if(m_DoTree)
        {
            /*for(float r = 82.5; r < 90; r+= 0.5)
            {
                for(float c = 72.5; c < 80; c += 0.5)
                {
                    if(!pcr::isMV(DEM->data[(int)r][(int)c]))
                    {
                        float elevation = DEM->data[(int)r][(int)c];
                        RigidPhysicsObject *obj = RigidPhysicsObject::RigidPhysicsObject_AsCilinder(1.0,13.75,600.0,LSMVector3(c * DEM->cellSizeX(),elevation + 55.0f,r * DEM->cellSizeY()),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),0.4,0.0,0.0,0.2,"Trees");
                        m_RigidWorld->AddObject(obj);
                    }
                }
            }

            {
                float r = 86;
                float c = 94;

                RigidPhysicsObject *obj = RigidPhysicsObject::RigidPhysicsObject_AsBox(LSMVector3(2.0,2.0,90.0),1000.0,LSMVector3(c * DEM->cellSizeX(),0.0,r * DEM->cellSizeY()),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),0.4,0.0,0.0,0.2,"",true);
                m_RigidWorld->AddObject(obj);
            }


            {
                float r = 82;
                float c = 94;

                RigidPhysicsObject *obj = RigidPhysicsObject::RigidPhysicsObject_AsBox(LSMVector3(2.0,30.0,4.0),1000.0,LSMVector3(c * DEM->cellSizeX(),-15.0,r * DEM->cellSizeY()),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),0.4,0.0,0.0,0.2,"",true);
                m_RigidWorld->AddObject(obj);
            }

            {
                float r = 83;
                float c = 94;

                RigidPhysicsObject *obj = RigidPhysicsObject::RigidPhysicsObject_AsBox(LSMVector3(2.0,30.0,4.0),1000.0,LSMVector3(c * DEM->cellSizeX(),-15.0,r * DEM->cellSizeY()),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),0.4,0.0,0.0,0.2,"",true);
                m_RigidWorld->AddObject(obj);
            }


            {
                float r = 84;
                float c = 94;

                RigidPhysicsObject *obj = RigidPhysicsObject::RigidPhysicsObject_AsBox(LSMVector3(2.0,30.0,4.0),1000.0,LSMVector3(c * DEM->cellSizeX(),-15.0,r * DEM->cellSizeY()),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),0.4,0.0,0.0,0.2,"",true);
                m_RigidWorld->AddObject(obj);
            }


            {
                float r = 85;
                float c = 94;

                RigidPhysicsObject *obj = RigidPhysicsObject::RigidPhysicsObject_AsBox(LSMVector3(2.0,30.0,4.0),1000.0,LSMVector3(c * DEM->cellSizeX(),-15.0,r * DEM->cellSizeY()),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),0.4,0.0,0.0,0.2,"",true);
                m_RigidWorld->AddObject(obj);
            }


            {
                float r = 86;
                float c = 94;

                RigidPhysicsObject *obj = RigidPhysicsObject::RigidPhysicsObject_AsBox(LSMVector3(2.0,30.0,4.0),1000.0,LSMVector3(c * DEM->cellSizeX(),-15.0,r * DEM->cellSizeY()),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),0.4,0.0,0.0,0.2,"",true);
                m_RigidWorld->AddObject(obj);
            }

            {
                float r = 87;
                float c = 94;

                RigidPhysicsObject *obj = RigidPhysicsObject::RigidPhysicsObject_AsBox(LSMVector3(2.0,30.0,4.0),1000.0,LSMVector3(c * DEM->cellSizeX(),-15.0,r * DEM->cellSizeY()),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),0.4,0.0,0.0,0.2,"",true);
                m_RigidWorld->AddObject(obj);
            }

            {
                float r = 88;
                float c = 94;

                RigidPhysicsObject *obj = RigidPhysicsObject::RigidPhysicsObject_AsBox(LSMVector3(2.0,30.0,4.0),1000.0,LSMVector3(c * DEM->cellSizeX(),-15.0,r * DEM->cellSizeY()),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),0.4,0.0,0.0,0.2,"",true);
                m_RigidWorld->AddObject(obj);
            }

            {
                float r = 89;
                float c = 94;

                RigidPhysicsObject *obj = RigidPhysicsObject::RigidPhysicsObject_AsBox(LSMVector3(2.0,30.0,4.0),1000.0,LSMVector3(c * DEM->cellSizeX(),-15.0,r * DEM->cellSizeY()),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),0.4,0.0,0.0,0.2,"",true);
                m_RigidWorld->AddObject(obj);
            }

            {
                float r = 90;
                float c = 94;

                RigidPhysicsObject *obj = RigidPhysicsObject::RigidPhysicsObject_AsBox(LSMVector3(2.0,30.0,4.0),1000.0,LSMVector3(c * DEM->cellSizeX(),-15.0,r * DEM->cellSizeY()),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),0.4,0.0,0.0,0.2,"",true);
                m_RigidWorld->AddObject(obj);
            }*/




            {
                float r = 86;
                //float c = 94.5;
                float c = 68.5;

                RigidPhysicsObject *obj = RigidPhysicsObject::RigidPhysicsObject_AsBox(LSMVector3(1.0,50.0,100.0),1000.0,LSMVector3(c * DEM->cellSizeX(),10.0 ,r * DEM->cellSizeY()),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),0.4,0.0,0.0,0.2,"",false);
                m_RigidWorld->AddObject(obj);
            }

            {
                for(int i = 0; i < 10; i++)
                {
                    for(int j = 0; j < 10; j++)
                    {

                        float r = 83 + i * 0.5;
                        float c = 94.5;
                        //float c = 74.5;

                        float elevation = DEM->data[int(r)][(int)(c)];

                        RigidPhysicsObject *obj = RigidPhysicsObject::RigidPhysicsObject_AsBox(LSMVector3(3.0,5.0,5.0),500.0,LSMVector3(c * DEM->cellSizeX(),elevation + j * 5.0 + 5.0 ,r * DEM->cellSizeY()),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),0.4,0.0,0.0,0.2,"",false);
                        //m_RigidWorld->AddObject(obj);
                    }
                }



            }



            {
                int r = 83;
                int c = 73;

                float elevation = DEM->data[r][c];
                RigidPhysicsObject *obj = RigidPhysicsObject::RigidPhysicsObject_AsCilinder(1.0,28.75,1000.0,LSMVector3(c * DEM->cellSizeX(),elevation + 25.0f,r * DEM->cellSizeY()),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),LSMVector3(0.0,0.0,0.0),0.4,0.0,0.0,0.2,"");
                //m_RigidWorld->AddObject(obj);
            }

            /*r = 84;
            c = 75;

            elevation = DEM->data[r][c];
            obj = RigidPhysicsObject::RigidPhysicsObject_AsCilinder(0.75,15,1.0,LSMVector3(c * DEM->cellSizeX(),elevation + 50.0f,r * DEM->cellSizeY()),LSMVector3(0.0,0.0,0.0),0.4,0.0,0.0,0.2,"");
            m_RigidWorld->AddObject(obj);*/
        }

    }



    //create textures
    LISEM_STATUS("Create OpenGLCL Textures");


    CreateMapBuffers();


    if(m_DoParticleSolids)
    {
        CreateDataBuffers();

    }

    //set kernel input

    LISEM_STATUS("Setting kernel input");

    LISEM_STATUS("map width and height " + QString::number(DEM->nrCols()) + " " +  QString::number(DEM->nrRows()));

    SetUIMaps();
    SetKernelInput();


}

void LISEMModel::ModelRunLoadData()
{

    m_UIMapNames.clear();
    m_UIMapNums.clear();

    LISEM_STATUS("Get Directories and settings");

    //Get Main Settings
    m_Dir_Maps = GetOptionString("Map Directory");
    m_Dir_Time = GetOptionString("TimeInput Directory");
    m_Dir_Res = GetOptionString("Output Directory");

    this->factor_particle = 1.0/GetOptionDouble("Particle Fractional Dimension");

    FinishResult.Dir_Maps = m_Dir_Maps;
    FinishResult.Dir_Time = m_Dir_Time;
    FinishResult.Dir_Output = m_Dir_Res;


    m_MemorySave = GetOptionInt("Limit Memory Usage") == 1;
    m_SimulateSubSection = GetOptionInt("Simulate Subsection") == 1;
    m_SimulateSubSectionIndex = GetOptionInt("SubSection Value");


    //Get Directories and settingss
    m_DoInfiltration =  GetOptionInt("Include Infiltration") == 1;
    //m_DoDoubleLayer = GetOptionInt("Include Second Soil Layer") == 1;
    m_DoHydrology = GetOptionInt("Include Hydrology") == 1;
    m_DoCustomInfiltration = GetOptionInt( "Custom Infiltration") == 1;
    m_DoEvapoTranspiration = GetOptionInt("Include Evapotranspiration") == 1;
    m_DoGroundWater = GetOptionInt("Include GroundWater Flow") == 1;
    m_DoChannel = GetOptionInt("Include Channel") == 1;
    m_DoErosion = GetOptionInt("Include Erosion") == 1;
    m_DoEntrainment = GetOptionInt("Include Loose Material Layer") == 1;
    m_DoSlopeStability = GetOptionInt("Include Slope Stability") == 1;
    m_DoSlopeFailure = GetOptionInt("Include Slope Failure") == 1;
    m_DoInitialWater = GetOptionInt("Include Initial Water") == 1;
    m_DoInitialSolids = GetOptionInt("Include Initial Solids") == 1;
    m_DoParticleSolids = GetOptionInt("Use Material Point Method") == 1;
    m_DoInitialStability =GetOptionInt("Include Initial Stability") == 1;
    m_DoSeismic =GetOptionInt("Include Seismic Trigger") == 1;

    m_DoRigidWorld =GetOptionInt("Include Rigid-Body Phyisics") == 1;
    m_DoTree =GetOptionInt("Tree Instancing") == 1;

    m_ElasticModulus = GetOptionDouble("Elastic Modulus");
    m_ShearModulus = GetOptionDouble("Shear Modulus");

    m_DragMult = GetOptionDouble("Drag coefficient multiplier");
    m_ReynolldsMult = GetOptionDouble("Reynolds Number Multiplier");
    m_InitialStabMargin = GetOptionDouble( "Initial Stability Margin");




    //get output options
    m_DoOutputTiff = GetOptionInt("Output GeoTIFF format") == 1;
    m_DoOutputOnlyHV = GetOptionInt("Output Max Height/Vel only") == 1;
    m_DoOutputTotals = GetOptionInt("Output totals") == 1;
    m_DoOutputHydrographs = GetOptionInt("Output hydrographs") == 1;
    m_DoOutputTimeseries = GetOptionInt("Output timeseries") == 1;
    m_DoOutputTimeseriesEvery = std::max(1,GetOptionInt("Report every"));
    m_DoOutputParticleTimeseriesEvery = std::max(1,GetOptionInt("Report particles every"));
    m_DoOutputTimeseriesH = GetOptionInt("Output timeseries of H") == 1;
    m_DoOutputTimeseriesHS = GetOptionInt( "Output timeseries of HS") == 1;
    m_DoOutputTimeseriesHT = GetOptionInt( "Output timeseries of HT (total height)") == 1;
    m_DoOutputTimeseriesSFRAC =  GetOptionInt("Output timeseries of SFRAC") == 1;
    m_DoOutputTimeseriesV = GetOptionInt("Output timeseries of V") == 1;
    m_DoOutputTimeseriesVS = GetOptionInt("Output timeseries of VS") == 1;
    m_DoOutputTimeseriesVT = GetOptionInt("Output timeseries of HT (total averaged velocity)") == 1;
    m_DoOutputTimeseriesInfil = GetOptionInt("Output timeseries of Infil") == 1;
    m_DoOutputTimeseriesGWH = GetOptionInt("Output timeseries of GWH") == 1;
    m_DoOutputTimeseriesErosion = GetOptionInt("Output timeseries of Soil Loss") == 1;
    m_DoOutputTimeseriesSediment = GetOptionInt("Output timeseries of Sediment Load") == 1;


    m_SetOutputArrays = false;

    //Get Maps

    LISEM_STATUS("Reading maps");

    //prepare subsection stuff

    if(m_SimulateSubSection)
    {
        LISEM_STATUS("Selection of SubSection");

        SetMaskMap(m_Dir_Maps + GetOptionString("MASK"),m_SimulateSubSectionIndex);

    }

    DEM = GetBaseMap(m_Dir_Maps + GetOptionString("DEM"));
    DEMM = GetMapWithMultByName(m_Dir_Maps,"DEM");
    MUI = GetMap(0.0);
    MUIH = GetMap(0.0);
    MUIUX = GetMap(0.0);
    MUIUY = GetMap(0.0);
    MUIHALL = GetMap(0.0);



    MUIHS = GetMap(0.0);
    N = GetMapWithMultByName(m_Dir_Maps,"Manning");
    SLOPE = GetMap(0.0);
    HILLSHADE = GetMap(0.0);

    RAIN = GetMap(0.0);
    OUTLETS = GetMap(0.0);

    LOC_R = GetMap(0.0);
    LOC_C = GetMap(0.0);


    FluxFTotal = GetMap(0.0);
    FluxSTotal = GetMap(0.0);
    FluxFPeakMomentum = GetMap(0.0);
    FluxSPeakMomentum = GetMap(0.0);
    PeakMomentum = GetMap(0.0);

    HF = GetMap(0.0);
    Q = GetMap(0.0);
    VFX = GetMap(0.0);
    VFY = GetMap(0.0);
    HS = GetMap(0.0);
    VSX = GetMap(0.0);
    VSY = GetMap(0.0);

    if(m_DoOutputTimeseriesEvery && m_DoOutputTimeseriesHT)
    {
        HT = GetMap(0.0);
    }
    if(m_DoOutputTimeseriesEvery && m_DoOutputTimeseriesVT)
    {
        VT = GetMap(0.0);
    }

    VELF = GetMap(0.0);
    VELS = GetMap(0.0);

    SFrac = GetMap(0.0);
    HFMax = GetMap(0.0);
    HSMax = GetMap(0.0);
    HTMax = GetMap(0.0);
    VTMax = GetMap(0.0);
    VFMax = GetMap(0.0);
    VSMax = GetMap(0.0);
    SFracMax = GetMap(0.0);
    FLOWTIMING = GetMap(0.0);
    FLOWDURATION = GetMap(0.0);
    DTREQ = GetMap(0.0);
    QFOUT = GetMap(0.0);

    if(m_DoInfiltration && !m_DoHydrology)
    {
        KSATTOPSIMPLE = GetMapWithMultByName(m_Dir_Maps,"KSatSimple");
    }

    if(m_DoHydrology && !m_DoCustomInfiltration)
    {
        CLAY = GetMapWithMultByName(m_Dir_Maps,"Clay");
        SAND = GetMapWithMultByName(m_Dir_Maps,"Sand");
        GRAVEL = GetMapWithMultByName(m_Dir_Maps,"Gravel");
        ORGANIC = GetMapWithMultByName(m_Dir_Maps,"Organic");
        DENSITY = GetMapWithMultByName(m_Dir_Maps,"Density");

        ROAD =GetMapWithMultByName(m_Dir_Maps,"Density");
        BUILDING = GetMapWithMultByName(m_Dir_Maps,"Building Cover");
        SD = GetMapWithMultByName(m_Dir_Maps,"Soil Depth");
        GW_US = GetMapWithMultByName(m_Dir_Maps,"Theta Initial");
        THETAS = GetMap(0.0);
        KSAT_T = GetMap(0.0);
        INFIL = GetMap(0.0);
        INFILTOTAL = GetMap(0.0);
        SURFACESTORAGE = GetMap(0.0);
        CANOPYSTORAGE = GetMap(0.0);
        EVAPOTRANSPIRATION = GetMap(0.0);
    }

    if(m_DoHydrology && m_DoCustomInfiltration)
    {
        CLAY = GetMap(0.0);//GetMapWithMultByName(m_Dir_Maps,"Clay");
        SAND = GetMap(0.0);//GetMapWithMultByName(m_Dir_Maps,"Sand");
        GRAVEL = GetMap(0.0);//GetMapWithMultByName(m_Dir_Maps,"Gravel");
        ORGANIC = GetMap(0.0);//GetMapWithMultByName(m_Dir_Maps,"Organic");
        DENSITY = GetMap(0.0);//GetMapWithMultByName(m_Dir_Maps,"Density");

        ROAD =GetMapWithMultByName(m_Dir_Maps,"Density");
        BUILDING = GetMapWithMultByName(m_Dir_Maps,"Building Cover");
        SD = GetMapWithMultByName(m_Dir_Maps,"Soil Depth");
        GW_US = GetMapWithMultByName(m_Dir_Maps,"Theta Initial");
        THETAS = GetMapWithMultByName(m_Dir_Maps,"Porosity");
        KSAT_T =  GetMapWithMultByName(m_Dir_Maps,"KSatSimple");
        INFIL = GetMap(0.0);
        INFILTOTAL = GetMap(0.0);
        SURFACESTORAGE = GetMap(0.0);
        CANOPYSTORAGE = GetMap(0.0);
        EVAPOTRANSPIRATION = GetMap(0.0);
    }


    if(m_DoEvapoTranspiration)
    {
        CROPF = GetMapWithMultByName(m_Dir_Maps,"CropF");
    }
    if(m_DoHydrology && !m_DoCustomInfiltration)
    {
        SMAX_CANOPY = GetMapWithMultByName(m_Dir_Maps,"SMAX Canopy");
        SMAX_SURFACE = GetMapWithMultByName(m_Dir_Maps,"SMAX Surface");

        GW_S = GetMapWithMultByName(m_Dir_Maps,"Ground Water Height");

        THETAR = GetMap(0.0);
        KSAT_B = GetMap(0.0);
        SWR_A = GetMap(0.0);
        SWR_B = GetMap(0.0);
    }
    if(m_DoHydrology && m_DoCustomInfiltration)
    {
        SMAX_CANOPY = GetMapWithMultByName(m_Dir_Maps,"SMAX Canopy");
        SMAX_SURFACE = GetMapWithMultByName(m_Dir_Maps,"SMAX Surface");

        GW_S = GetMapWithMultByName(m_Dir_Maps,"Ground Water Height");

        THETAR = GetMap(0.1);
        KSAT_B = GetMapWithMultByName(m_Dir_Maps,"KSatBottom");
        SWR_A = GetMapWithMultByName(m_Dir_Maps,"A");
        SWR_B = GetMapWithMultByName(m_Dir_Maps,"B");
    }

    if(((m_DoHydrology && m_DoErosion) || (((m_DoHydrology && m_DoSlopeFailure) || m_DoInitialSolids) && m_DoEntrainment)))
    {

        D50 = GetMap(0.0);
        D90 = GetMap(0.0);

        Y = GetMap(0.0);
        HARDCOVER = GetMap(0.0);

        if(m_DoErosion)
        {

            VEGCOVER = GetMapWithMultByName(m_Dir_Maps,"Vegetation Cover");
            VEGH = GetMapWithMultByName(m_Dir_Maps,"Vegetation Height");
            COH = GetMapWithMultByName(m_Dir_Maps,"Cohesion Top");
            if(m_DoChannel)
            {
                CHCOH = GetMapWithMultByName(m_Dir_Maps,"Cohesion Channel");
            }else
            {
                CHCOH = GetMap(0.0);
            }
        }else
        {
            VEGCOVER = GetMap(0.0);
            VEGH = GetMap(0.0);
            COH = GetMap(0.0);
            CHCOH = GetMap(0.0);
        }

        //ENTRAINMENTTOT = GetMap(0.0);
        //ENTRAINMENTTOTN = GetMap(0.0);

        if(m_DoEntrainment)
        {
            MIFA = GetMapWithMultByName(m_Dir_Maps,"Material Internal Friction Angle");
            MROCKSIZE = GetMapWithMultByName(m_Dir_Maps,"Material Rocksize");
            MDENSITY = GetMapWithMultByName(m_Dir_Maps,"Material Density Bottom");
            MD = GetMapWithMultByName(m_Dir_Maps,"Material Depth");
            MDN = GetMapWithMultByName(m_Dir_Maps,"Material Depth");
        }else
        {
            MIFA = GetMap(0.0);
            MROCKSIZE = GetMap(0.0);
            MDENSITY = GetMap(0.0);
            MD = GetMap(0.0);
            MDN = GetMap(0.0);
        }
    }

    if(m_DoGroundWater)
    {
        GWOUT = GetMap(0.0);
    }
    if(m_DoChannel)
    {
        CH_M = GetMap(m_Dir_Maps + GetOptionString("Channel"));
        CH_WIDTH = GetMapWithMultByName(m_Dir_Maps,"Channel Width");
        CH_HEIGHT = GetMapWithMultByName(m_Dir_Maps,"Channel Depth");

        QCH = GetMap(0.0);
        CHHF = GetMap(0.0);
        CHVFX = GetMap(0.0);
        CHHS = GetMap(0.0);
        CHVSX = GetMap(0.0);

    }else
    {
        CH_M =GetMap(0.0);
        CH_WIDTH = GetMap(0.0);
        CH_HEIGHT = GetMap(0.0);
    }
    if(m_DoErosion)
    {
        SLTOT = GetMap(0.0);
        SED = GetMap(0.0);
        CHSED = GetMap(0.0);
        DETTOT = GetMap(0.0);
        DEPTOT = GetMap(0.0);
    }
    if(m_DoSlopeStability)
    {
        COHESION = GetMapWithMultByName(m_Dir_Maps,"Cohesion Bottom",1000.0);
        IFA = GetMapWithMultByName(m_Dir_Maps,"Internal Friction Angle");
        DENSITY_BOTTOM = GetMapWithMultByName(m_Dir_Maps,"Density Bottom");
        ROCKSIZE = GetMapWithMultByName(m_Dir_Maps,"Rocksize");
    }
    if(m_DoInitialWater)
    {
        INITIALHF = GetMapWithMultByName(m_Dir_Maps,"Initial Fluid Height");
        INITIALHFPART = GetMap(0.0);
    }
    if(m_DoInitialSolids)
    {
        INITIALHS = GetMapWithMultByName(m_Dir_Maps,"Initial Solid Height");
        INITIALIFA = GetMapWithMultByName(m_Dir_Maps,"Initial Solid IFA");
        INITIALRS = GetMapWithMultByName(m_Dir_Maps,"Initial Solid Rocksize");
        INITIALD = GetMapWithMultByName(m_Dir_Maps,"Initial Solid Density");

        if(m_DoParticleSolids && m_DoInitialWater)
        {
            FOR_ROW_COL_MV(DEM)
            {
                if(INITIALHS->Drc > 0.0000001f)
                {
                    INITIALHFPART->Drc = INITIALHF->Drc;
                    INITIALHF->Drc = 0.0;
                }else {
                    INITIALHFPART->Drc = 0.0f;
                }
            }
        }
        INITIALCOH = GetMapWithMultByName(m_Dir_Maps,"Initial Cohesion");
    }

    if(m_DoSlopeStability || m_DoInitialSolids)
    {
        QSOUT = GetMap(0.0);
    }

    if(m_DoSlopeStability && m_DoHydrology)
    {
        SF = GetMap(0.0);
        SFMIN = GetMap(1000.0);
        SFC = GetMap(0.0);
    }

    if(m_DoSlopeStability && m_DoHydrology && m_DoSeismic)
    {
        PGA = GetMapWithMultByName(m_Dir_Maps,"Peak Ground Accaleration");
    }else if(m_DoSlopeStability && m_DoHydrology && m_DoImprovedIterative)
    {
        PGA = GetMap(0.0);
    }


    if(m_DoSlopeStability && m_DoHydrology && m_DoSlopeFailure)
    {
        SLOPEFAILURETEMP = GetMap(0.0);
        SLOPEFAILURE = GetMap(0.0);
        SLOPEFAILURETOTAL = GetMap(0.0);
    }

    if(m_DoRigidWorld)
    {
        m_BlockX = GetMap(0.0);
        m_BlockY = GetMap(0.0);
    }

    /*SED = GetMap(0.0);
    CHSED = GetMap(0.0);
    DETTOT = GetMap(0.0);
    DEPTOT = GetMap(0.0);
    SEDN = GetMap(0.0);
    CHSEDN = GetMap(0.0);
    DETTOTN = GetMap(0.0);
    DEPTOTN = GetMap(0.0);
    QSEDOUT = GetMap(0.0);
    QSEDOUTN = GetMap(0.0);*/

}





void LISEMModel::InitialCalculations()
{
    float maxDem = -1e9;
    float minDem = 1e9;

    int n_cells = 0;
    int rn = 0;
    int cn = 0;

    FOR_ROW_COL_MV(DEM)
    {
        n_cells++;
        rn++;
        if(rn > DEM->nrRows()-1)
        {
            rn = 0;
            cn = cn+1;
        }

        LOC_R->data[rn][cn] = (float)(r);
        LOC_C->data[rn][cn] = (float)(c);


        if(m_DoChannel)
        {
            if(!(CH_M->data[r][c] > 0 && CH_M->data[r][c] < 10))
            {
                CH_M->data[r][c] = 0;
            }
        }

        float mat[9];
        float dx, dy;//, aspect;
        float factor = 1.0;

        minDem = std::min(DEM->data[r][c], minDem);
        maxDem = std::max(DEM->data[r][c], maxDem);

        for (int i = 0; i < 9; i++)
        {
            mat[i] = DEM->data[r][c];
        }
        if (r > 0 && r < DEM->nrRows()-1 && c > 0 && c < DEM->nrCols()-1)
        {
            if(!pcr::isMV(DEM->data[r-1][c-1]))
            {
                mat[0] = DEM->data[r-1][c-1];
            }
            if(!pcr::isMV(DEM->data[r-1][c  ]))
            {
                mat[1] = DEM->data[r-1][c  ];
            }
            if(!pcr::isMV(DEM->data[r-1][c+1]))
            {
                mat[2] = DEM->data[r-1][c+1];
            }
            if(!pcr::isMV(DEM->data[r  ][c-1]))
            {
                mat[3] = DEM->data[r  ][c-1];
            }

            if(!pcr::isMV(DEM->data[r  ][c+1]))
            {
                mat[5] = DEM->data[r  ][c+1];
            }
            if(!pcr::isMV(DEM->data[r+1][c-1]))
            {
                mat[6] = DEM->data[r+1][c-1];
            }
            if(!pcr::isMV(DEM->data[r+1][c  ]))
            {
                mat[7] = DEM->data[r+1][c  ];
            }
            if(!pcr::isMV(DEM->data[r+1][c+1]))
            {
                mat[8] = DEM->data[r+1][c+1];
            }
        }
        for (int i = 0; i < 9; i++)
        {
            mat[i] *= factor;
        }

        dx = (mat[2] + 2*mat[5] + mat[8] - mat[0] -2*mat[3] - mat[6])/(8*_dx);
        dy = (mat[0] + 2*mat[1] + mat[2] - mat[6] -2*mat[7] - mat[8])/(8*_dx);

        //http://edndoc.esri.com/arcobjects/9.2/net/shared/geoprocessing/spatial_analyst_tools/how_hillshade_works.htm
        //Burrough, P. A. and McDonell, R.A., 1998. Principles of Geographical Information Systems (Oxford University Press, New York), p. 190.
        float z_factor = 2.0;
        float Slope_rad = atan( z_factor * sqrt ( dx*dx+dy*dy) );
        float Aspect_rad = 0;
        if( dx != 0)
        {
            Aspect_rad = atan2(dy, -dx);
            if (Aspect_rad < 0)
                Aspect_rad = 2.0f*MAPMATH_PI + Aspect_rad;
        }
        else
        {
            if(dy > 0)
            {
                Aspect_rad = MAPMATH_PI/2.0f;
            }
            else
            {
                Aspect_rad = 2.0f*MAPMATH_PI - MAPMATH_PI/2.0f;
            }
        }
        float Zenith_rad = 70.0f * MAPMATH_PI / 180.0f;
        float Azimuth_rad = 240.0f * MAPMATH_PI / 180.0f;
        HILLSHADE->Drc = 255.0f * ( ( cos(Zenith_rad) * cos(Slope_rad) ) + ( sin(Zenith_rad) * sin(Slope_rad) * cos(Azimuth_rad - Aspect_rad) ) );


        if(!m_DoHydrology && m_DoInfiltration)
        {
            KSATTOPSIMPLE->data[r][c] = KSATTOPSIMPLE->data[r][c] * (1.0f/3600000.0f);

        }

        if(m_DoHydrology)
        {

            if(!m_DoCustomInfiltration)
            {

                float Densityfactor = 0.9;


                    ORGANIC->data[r][c] = ORGANIC->data[r][c] * 1.72/100.0;
                    GRAVEL->data[r][c] = GRAVEL->data[r][c]/100.0f;

                    float S = std::min(0.9f,std::max(0.1f,SAND->data[r][c]));
                    float C = std::min(0.9f,std::max(0.1f,CLAY->data[r][c]));
                    float OM = std::min(1000.0f,std::max(0.1f,ORGANIC->data[r][c]));
                    float Gravel = std::min(1000.0f,std::max(0.1f,GRAVEL->data[r][c]));

                    float M1500 =-0.024*S+0.487*C+0.006*OM+0.005*S*OM-0.013*C*OM+0.068*S*C+0.031;
                    float M1500adj =M1500+0.14*M1500-0.02;
                    float M33  =-0.251*S+0.195*C+0.011*OM+0.006*S*OM-0.027*C*OM+0.452*S*C+0.299;
                    float M33adj = M33+(1.283*M33*M33-0.374*M33-0.015);
                    float PM33    = 0.278*S+0.034*C+0.022*OM-0.018*S*OM-0.027*C*OM-0.584*S*C+0.078;
                    float PM33adj = PM33+(0.636*PM33-0.107);
                    float SatPM33 = M33adj + PM33adj;
                    float SatSadj = -0.097*S+0.043;
                    float SadjSat = SatPM33  + SatSadj;
                    float Dens_om = (1-SadjSat)*2.65;
                    float Dens_comp = Dens_om * Densityfactor;
                    float PORE_comp =(1-Dens_om/2.65)-(1-Dens_comp/2.65);
                    float M33comp = M33adj - 0.2*PORE_comp;
                    float thetast = 1.0-(Dens_comp/2.65);
                    float PoreMcomp = thetast-M33comp;
                    float LAMBDA = (std::log(M33comp)-std::log(M1500adj))/(std::log(1500)-std::log(33));
                    float GravelRedKsat =(1.0-Gravel)/(1.0-Gravel*(1.0-1.5*(Dens_comp/2.65)));

                    float Ksattop =(1930.0*std::pow((PoreMcomp),(3.0-LAMBDA))*GravelRedKsat);

                    S = SAND->data[r][c];
                    C = CLAY->data[r][c];
                    OM = 0;
                    Gravel = GRAVEL->data[r][c];

                    M1500 =-0.024*S+0.487*C+0.006*OM+0.005*S*OM-0.013*C*OM+0.068*S*C+0.031;
                    M1500adj =M1500+0.14*M1500-0.02;
                    M33  =-0.251*S+0.195*C+0.011*OM+0.006*S*OM-0.027*C*OM+0.452*S*C+0.299;
                    M33adj = M33+(1.283*M33*M33-0.374*M33-0.015);
                    PM33    = 0.278*S+0.034*C+0.022*OM-0.018*S*OM-0.027*C*OM-0.584*S*C+0.078;
                    PM33adj = PM33+(0.636*PM33-0.107);
                    SatPM33 = M33adj + PM33adj;
                    SatSadj = -0.097*S+0.043;
                    SadjSat = SatPM33  + SatSadj;
                    Dens_om = (1.0f-SadjSat)*2.65;
                    Dens_comp = Dens_om * Densityfactor;
                    PORE_comp =(1.0f-Dens_om/2.65)-(1.0f-Dens_comp/2.65);
                    M33comp = M33adj - 0.2*PORE_comp;
                    float thetas1 = 1.0-(Dens_comp/2.65);
                    PoreMcomp = thetast-M33comp;
                    LAMBDA = (std::log(M33comp)-std::log(M1500adj))/(std::log(1500)-std::log(33));
                    GravelRedKsat =(1.0-Gravel)/(1.0-Gravel*(1.0-1.5*(Dens_comp/2.65)));

                    float Ksat1 =(1930.0*std::pow((PoreMcomp),(3.0-LAMBDA))*GravelRedKsat);

                    float BD1 = Gravel*2.65+(1.0f-Gravel)*Dens_comp;
                    float WP1 = M1500adj;
                    float FC1 = M33adj;
                    float PAW1 = (M33adj - M1500adj)*(1.0f-Gravel);

                    float bB = (std::log(1500.0f)-std::log(33.0f))/((std::log(FC1)-std::log(WP1)));
                    float aA = exp(std::log(33.0f) + bB*std::log(FC1));

                    float B = bB;
                    float A = aA;

                    float psi1 = std::max(10.0f,(float) std::pow(aA*(FC1 + 0.7 * (thetas1 - FC1)),-bB));
                    float thetai1 = (FC1 + GW_US->data[r][c] * (thetas1 - FC1));

                    THETAS->data[r][c] = thetas1;
                    KSAT_T->data[r][c] = Ksattop * (1.0f/3600000.0f);

                    THETAR->data[r][c] = FC1;
                    KSAT_B->data[r][c] = Ksat1 * (1.0f/3600000.0f);
                    SWR_A->data[r][c] = A;
                    SWR_B->data[r][c] = B;

            }


            if(m_DoHydrology )
            {
                GW_S->data[r][c] = GW_S->data[r][c]*THETAS->data[r][c];
                //convert groundwater effective volume fraction of unsaturated layer to meter equivalet
                GW_US->data[r][c] = std::max(0.0f,(SD->data[r][c]* THETAS->data[r][c] - GW_S->data[r][c])) * GW_US->data[r][c];
            }




        }

        if(((m_DoHydrology && m_DoErosion) || (((m_DoHydrology && m_DoSlopeFailure) || m_DoInitialSolids) && m_DoEntrainment)))
        {
            //weighted average (weighting by relative volumes, since the median is by number, so a volume fraction of smaller grains coutns as more because it is more induvidual grains)
            D50->Drc = (SAND->Drc * 1000.0f + 100.0f *CLAY->Drc* 5.0f + 20.0f*(1.0f - SAND->Drc-CLAY->Drc) * 50.0f)/std::max(0.01f,(SAND->Drc + 100.0f *CLAY->Drc + 20.0f*(1.0f - SAND->Drc-CLAY->Drc)));
            D90->Drc = D50->Drc * 2.5f;
        }

    }

    for(int r = 0; r < DEM->nrRows(); r++)
    {
        for (int c = 0; c < DEM->nrCols(); c++)
        {
            if(pcr::isMV(DEM->data[r][c]))
            {
                DEMM->data[r][c] = -9999;
            }
        }
    }

    float MaxV = 1.0f;
    float MinV = 0.0f;
    bool hsmaxfirst = false;

    FOR_ROW_COL_MV(HILLSHADE)
    {
        if(!hsmaxfirst)
        {
            MaxV = HILLSHADE->data[r][c];
            MinV = HILLSHADE->data[r][c];
            hsmaxfirst = true;
        }
        MaxV = std::max(MaxV,HILLSHADE->data[r][c]);
        MinV = std::min(MinV,HILLSHADE->data[r][c]);



    }

    FOR_ROW_COL_MV(DEM)
    {
        HILLSHADE->data[r][c] = (HILLSHADE->data[r][c]-MinV)/(MaxV-MinV);
        HILLSHADE->data[r][c] = 0.8f*HILLSHADE->data[r][c]+0.2f*(DEM->data[r][c] - minDem)/(maxDem-minDem);
    }

    m_NCells = n_cells;
}

void LISEMModel::CreateDataBuffers()
{


    T_P_Index = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);

    int particle_req = 0;

    if(m_DoInitialSolids)
    {
        for(int r = 0; r < DEM->nrRows(); r++)
        {
            for (int c = 0; c < DEM->nrCols(); c++)
            {
                if(!pcr::isMV(DEM->data[r][c]) &&!pcr::isMV(INITIALHS->data[r][c]) )
                {
                    float hs = INITIALHS->data[r][c];
                    particle_req += std::floor(((hs/ _dx)*factor_particle));
                }
            }
        }
    }

    //make initial estimates of amount of solid particles required during simulation
    int n_particles_init = 0.05 * std::floor(m_NCells * 4 * 0.25);

    CreateParticleBuffers(std::max(particle_req,n_particles_init),true);


    if(m_DoInitialSolids && particle_req > 0)
    {
        LISEM_DEBUG(QString("Set particles from initial solids n: %1 ").arg(particle_req));
        CreateParticleBuffers(particle_req,false);

        int i_total = 0;
        for(int r = 0; r < DEM->nrRows(); r++)
        {
            for (int c = 0; c < DEM->nrCols(); c++)
            {
                if(!pcr::isMV(DEM->data[r][c]) &&!pcr::isMV(INITIALHS->data[r][c]) )
                {
                    float hs = INITIALHS->data[r][c];
                    float hf = 0.0;
                    if(m_DoInitialWater)
                    {
                        hf = INITIALHFPART->data[r][c];
                    }

                    int particle_req_here = std::floor((hs/ _dx)*factor_particle);

                    float volf_particle = hf * _dx * _dx/((float)(particle_req));

                    float x_c = r*_dx;
                    float y_c = c*_dx;

                    float nfit = sqrt((float) particle_req_here);
                    float spacing = _dx/(1.0f + ((float) particle_req_here)/nfit);
                    for(int i_cell = 0; i_cell < particle_req_here; i_cell++)
                    {

                        float movedfromlefttop = i_cell * spacing + spacing * ((float)(std::floor(i_cell/nfit)));

                        float g = 1.32471795724474602596;
                        float a1 = 1.0/g;
                        float a2 = 1.0/(g*g);
                        float xreln = fmod((0.5+a1*((float)(i_cell))),1.0f) * _dx;
                        float yreln = fmod((0.5+a2*((float)(i_cell))),1.0f) * _dx;

                        float x_particle = x_c + xreln;//std::fmod(movedfromlefttop,_dx);
                        float y_particle = y_c + yreln;//spacing *(std::floor(movedfromlefttop/_dx));

                        int index = i_total + i_cell;

                        m_DataStressTensor->m_Data[index*4] = 0.0;
                        m_DataProps->m_Data[index*4] = 0.0;
                        m_DataStressTensor->m_Data[index*4+1] = 0.0;
                        m_DataProps->m_Data[index*4+1] = 0.0;
                        m_DataStressTensor->m_Data[index*4+2] = 0.0;
                        m_DataProps->m_Data[index*4+2] = 0.0;
                        m_DataStressTensor->m_Data[index*4+3] = 0.0;
                        m_DataProps->m_Data[index*4+3] = 1.0;

                        m_DataDispTensor->m_Data[index*4] = 0.0;
                        m_DataDispTensor->m_Data[index*4+1] = 0.0;
                        m_DataDispTensor->m_Data[index*4+2] = 0.0;
                        m_DataDispTensor->m_Data[index*4+3] = 0.0;

                        m_DataFluidProps->m_Data[index*5] = volf_particle;
                        m_DataFluidProps->m_Data[index*5+1] = 0.0;
                        m_DataFluidProps->m_Data[index*5+2] = 0.0;
                        m_DataFluidProps->m_Data[index*5+3] = 0.0;
                        m_DataFluidProps->m_Data[index*5+4] = 0.0;

                        m_DataUI->m_Data[index*2] = index;
                        m_DataVel->m_Data[index*2] = 0.0;
                        m_DataVel->m_Data[index*2+1] = 0.0;
                        m_DataPos->m_Data[index*2] =y_particle;
                        m_DataPos->m_Data[index*2+1] =x_particle;


                        m_DataStressTensorN->m_Data[index*4] = 0.0;
                        m_DataPropsPhysical->m_Data[index*4] = INITIALIFA->Drc;
                        m_DataStressTensorN->m_Data[index*4+1] = 0.0;
                        m_DataPropsPhysical->m_Data[index*4+1] = INITIALRS->Drc;
                        m_DataStressTensorN->m_Data[index*4+2] = 0.0;
                        m_DataPropsPhysical->m_Data[index*4+2] = INITIALD->Drc;
                        m_DataStressTensorN->m_Data[index*4+3] = 0.0;
                        m_DataPropsPhysical->m_Data[index*4+3] = INITIALCOH->Drc;

                    }
                    i_total += particle_req_here;
                }
            }

        }

        LISEM_DEBUG(QString("Set particles from initial solids n: %1 ").arg(i_total));


        std::cout << "update buffers after placing initial" << std::endl;
        m_DataPos->UpdateBufferFromData(m_OpenGLCLManager->q);
        m_DataUI->UpdateBufferFromData(m_OpenGLCLManager->q);

        if(!m_DoCPU)
        {
            m_DataVel->UpdateBufferFromData(m_OpenGLCLManager->q);
            m_DataStressTensor->UpdateBufferFromData(m_OpenGLCLManager->q);
            m_DataDispTensor->UpdateBufferFromData(m_OpenGLCLManager->q);
            m_DataFluidProps->UpdateBufferFromData(m_OpenGLCLManager->q);
            m_DataProps->UpdateBufferFromData(m_OpenGLCLManager->q);
            m_DataFluidProps->UpdateBufferFromData(m_OpenGLCLManager->q);

            m_DataStressTensorN->UpdateBufferFromData(m_OpenGLCLManager->q);
            m_DataPropsPhysical->UpdateBufferFromData(m_OpenGLCLManager->q);
        }

    }

}


inline ModelTexture * LISEMModel::CreateModelTexture(int cols,int rows,float val)
{
    ModelTexture * ret = new ModelTexture();
    ret->m_IsCPU = m_DoCPU;
    if(m_DoCPU)
    {
        MaskedRaster<float> *raster_data = new MaskedRaster<float>(rows,cols,0.0,0.0,1.0);

        FOR_ROW_COL_RASTER(raster_data)
        {
            (*raster_data)[r][c] = val;
        }
        ret->m_TextureCPU = raster_data;
    }else
    {

       ret->m_TextureGPU =  m_OpenGLCLManager->ModelGetMCLGLTexture(cols,rows,val);

    }

    m_ModelTextureList.append(ret);
    return ret;
}
inline ModelTexture * LISEMModel::CreateModelTexture(cTMap *m)
{

    ModelTexture * ret = new ModelTexture();
    ret->m_IsCPU = m_DoCPU;
    if(m_DoCPU)
    {
        ret->m_TextureCPU = new MaskedRaster<float>(m->data);

    }else
    {
        ret->m_TextureGPU = m_OpenGLCLManager->ModelGetMCLGLTexture(&m->data);
    }

    m_ModelTextureList.append(ret);
    return ret;
}



void LISEMModel::CreateMapBuffers()
{
    T_LOCR = CreateModelTexture(LOC_R);
    T_LOCC = CreateModelTexture(LOC_C);

    T_DEM = CreateModelTexture(DEMM);

    T_N = CreateModelTexture(N);



    if( m_DoInitialWater)
    {
        T_H = CreateModelTexture(INITIALHF);
    }else {
        T_H = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    }
    if(m_DoParticleSolids || !m_MemorySave)
    {
        T_HALL = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    }
    T_VX = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_VY = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_HN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_VXN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_VYN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);

    T_UI = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);

    T_F_INFILACT = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_F_INFILFLOWCUM = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_F_INFILPOT = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);

    T_HFMax = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_HSMax = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_VFMax = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_VSMax = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_HFMaxN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_HSMaxN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_VFMaxN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_VSMaxN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);

    T_RAIN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_DTREQ = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);

    if(m_DoEvapoTranspiration)
    {
        T_TEMP = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_WIND = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_VAPR = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_NDVI = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_RAD = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_EVAPOCUM = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CROPF = CreateModelTexture(CROPF);
    }

    T_QFX1 = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_QFX2 = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_QFY1 = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_QFY2 = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_QFCHX1 = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_QFCHX2 = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_QFCHIN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);

    T_QFOUT = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    T_QFOUTN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);

    if(m_DoInfiltration && !m_DoHydrology)
    {
            T_KSATTOPSIMPLE = CreateModelTexture(KSATTOPSIMPLE);
    }
    if(m_DoInfiltration || m_DoHydrology)
    {
            T_INFIL = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
            T_GW_WFN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
            T_THETAS = CreateModelTexture(THETAS);
    }
    if(m_DoHydrology)
    {

        T_SCANOPY = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_SSURFACE = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);

        //T_BUILDING = CreateModelTexture(BUILDING);
        //T_ROAD = CreateModelTexture(ROAD);

        T_GW_US = CreateModelTexture(GW_US);
        T_GW_S = CreateModelTexture(GW_S);
        T_THETAR = CreateModelTexture(THETAR);


        T_SD = CreateModelTexture(SD);
        T_KSAT_T = CreateModelTexture(KSAT_T);
        T_KSAT_B = CreateModelTexture(KSAT_B);
        T_SWR_A = CreateModelTexture(SWR_A);
        T_SWR_B = CreateModelTexture(SWR_B);
        T_SMAX_CANOPY = CreateModelTexture(SMAX_CANOPY);
        T_SMAX_SURFACE = CreateModelTexture(SMAX_SURFACE);
        T_SCANOPYN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_SSURFACEN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_GW_USN = CreateModelTexture(GW_US);
        T_GW_SN = CreateModelTexture(GW_S);

    }
    //we need these maps, since there is no seperate channel flow kernel
    //the flow kernel does both surface and channel, and therefore requires both sets of maps allways
    //if(m_DoChannel)
    {

        T_CHH = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHHN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHWIDTH = CreateModelTexture(CH_WIDTH);
        T_CHHEIGHT = CreateModelTexture(CH_HEIGHT);
        T_CHVX = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHVXN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHMASK = CreateModelTexture(CH_M);


    }
    if(m_DoGroundWater)
    {

        T_QGWOUT = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_QGWOUTN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    }
    if(m_DoHydrology && m_DoSlopeStability)
    {

        T_COHESION = CreateModelTexture(COHESION);
        T_IFA = CreateModelTexture(IFA);
        T_DENSITY_BOTTOM = CreateModelTexture(DENSITY_BOTTOM);
        //T_ROCKSIZE = CreateModelTexture(ROCKSIZE);
        T_SF = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);

        if(m_DoSeismic || m_DoImprovedIterative)
        {
            T_PGA = CreateModelTexture(PGA);
            T_SFFX = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
            T_SFFY = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
            T_SFFXN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
            T_SFFYN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        }

    }
    if(m_DoHydrology && m_DoSlopeStability)
    {
        T_SFH = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_SFC = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_SDN = CreateModelTexture(SD);
        T_DEMN = CreateModelTexture(DEMM);

    }else if ((m_DoHydrology && m_DoErosion) || (((m_DoHydrology && m_DoSlopeFailure) || m_DoInitialSolids) && m_DoEntrainment))
    {
        T_SFH = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_SFC = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_SDN = CreateModelTexture(SD);
        T_DEMN = CreateModelTexture(DEMM);
    }

    if(m_DoHydrology && m_DoSlopeStability &&m_DoSlopeFailure)
    {
        T_SFHCUM = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_SFHCUMN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    }

    if((m_DoHydrology && m_DoSlopeFailure) || m_DoInitialSolids)
    {
        if( m_DoInitialSolids)
        {
            T_HS = CreateModelTexture(INITIALHS);
            T_SDensity = CreateModelTexture(INITIALD);
            T_SIfa = CreateModelTexture(INITIALIFA);
            T_SRockSize = CreateModelTexture(INITIALRS);

        }else {
            T_HS = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
            T_SDensity = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
            T_SIfa = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
            T_SRockSize = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);

        }

        if(m_DoParticleSolids)
        {
            T_SCohesion = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
            T_SCohesionFrac = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),1.0);
            T_SCohesionFracOld = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),1.0);
            //T_SCohesionFFrac = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),1.0);
            //T_SCohesionFFracN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),1.0);

            T_SFLUIDH = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
            T_SFLUIDHADD = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        }
        T_VSX = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_VSY = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_HSN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_VSXN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_VSYN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);

        T_SDensityN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_SIfaN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_SRockSizeN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHHS = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHVS = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHHSN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHVSN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHSDensity = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHSIfa = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHSRockSize = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHSDensityN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHSIfaN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHSRockSizeN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);

        T_QSOUT = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_QSOUTN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);

    }else if(((m_DoHydrology && m_DoErosion) || (((m_DoHydrology && m_DoSlopeFailure) || m_DoInitialSolids) && m_DoEntrainment)))
    {
        T_HS = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_SDensity = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_SIfa = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_SRockSize = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_SDensityN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_SIfaN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_SRockSizeN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_VSX = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_VSY = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_HSN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_VSXN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_VSYN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);

        T_CHHS = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHVS = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHHSN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHVSN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHSDensity = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHSIfa = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHSRockSize = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHSDensityN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHSIfaN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHSRockSizeN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    }

    if(((m_DoHydrology && m_DoErosion) || (((m_DoHydrology && m_DoSlopeFailure) || m_DoInitialSolids) && m_DoEntrainment)))
    {
        T_HARDCOVER = CreateModelTexture(HARDCOVER);
        T_VEGCOVER = CreateModelTexture(VEGCOVER);
        T_VEGH = CreateModelTexture(VEGH);
        T_D50 = CreateModelTexture(D50);
        T_D90 = CreateModelTexture(D90);
        T_COH = CreateModelTexture(COH);
        T_CHCOH = CreateModelTexture(CHCOH);
        T_Y = CreateModelTexture(Y);
        T_MD = CreateModelTexture(MD);
        T_MDN = CreateModelTexture(MD);
        T_MIFA = CreateModelTexture(MIFA);
        T_MROCKSIZE = CreateModelTexture(MROCKSIZE);
        T_MDENSITY = CreateModelTexture(MDENSITY);

    }

    if(m_DoErosion)
    {
        T_SED = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHSED = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_DETTOT = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_DEPTOT = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_ENTRAINMENTTOT = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_SEDN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_CHSEDN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_DETTOTN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_DEPTOTN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_ENTRAINMENTTOTN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_QSEDOUT = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_QSEDOUTN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    }

    if((((m_DoHydrology && m_DoSlopeFailure) || (m_DoHydrology && m_DoErosion) || (m_DoHydrology && m_DoSlopeFailure) || m_DoInitialSolids) && m_DoEntrainment) || m_DoErosion)
    {
        T_ENTRAINMENT = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_ENTRAINMENTN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_ENTRAINMENTCH = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_ENTRAINMENTCHN = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    }

    //if(m_DoRigidWorld)
    {
        T_BlockingX = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_BlockingY = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_BlockingFX = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_BlockingFY = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_BlockingVelX = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_BlockingVelY = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_BlockingLDD = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
        T_HCorrect = CreateModelTexture(DEM->nrCols(),DEM->nrRows(),0.0);
    }

    //ui relevant maps
    TUI_H = m_OpenGLCLManager->ModelGetMCLGLTexture(DEM->nrCols(),DEM->nrRows(),0.0,true);

    if(!m_MemorySave)
    {

        TUI_UX = m_OpenGLCLManager->ModelGetMCLGLTexture(DEM->nrCols(),DEM->nrRows(),0.0,true);
        TUI_UY = m_OpenGLCLManager->ModelGetMCLGLTexture(DEM->nrCols(),DEM->nrRows(),0.0,true);
        TUI_HALL = m_OpenGLCLManager->ModelGetMCLGLTexture(DEM->nrCols(),DEM->nrRows(),0.0,true);
        TUI_VX = m_OpenGLCLManager->ModelGetMCLGLTexture(DEM->nrCols(),DEM->nrRows(),0.0,true);
        TUI_VY = m_OpenGLCLManager->ModelGetMCLGLTexture(DEM->nrCols(),DEM->nrRows(),0.0,true);
        TUI_UI = m_OpenGLCLManager->ModelGetMCLGLTexture(DEM->nrCols(),DEM->nrRows(),0.0,true);
        TUI_HILLSHADE = m_OpenGLCLManager->ModelGetMCLGLTexture(&(HILLSHADE->data),true);
        TUI_DEM = m_OpenGLCLManager->ModelGetMCLGLTexture(&(DEMM->data),true);
        TUI_HS = m_OpenGLCLManager->ModelGetMCLGLTexture(DEM->nrCols(),DEM->nrRows(),0.0,true);

    }
}



void LISEMModel::ModelRunDeleteData()
{

    LISEM_STATUS("Deleting all model related memory objects");

    if(m_RigidWorld != nullptr)
    {
        if(m_RigidWorld->AS_IsFromScript)
        {
            m_RigidWorld->AS_ReleaseRef();
            m_RigidWorld = nullptr;
        }else
        {
            m_RigidWorld->Destroy();
            delete m_RigidWorld;
            m_RigidWorld = nullptr;
        }
    }

    m_UIMapsSet = false;
    m_NParticlePadded = 0;
    m_NParticle = 0;

    //delete all maps

    DestroyMaps();

    //delete all opengl textures

    m_OpenGLCLManager->DeleteMGLCLModelObjects();


    for(int i = 0;i < m_ModelTextureList.size(); i++)
    {
        if(m_ModelTextureList.at(i)->m_TextureCPU != nullptr)
        {
            delete m_ModelTextureList.at(i)->m_TextureCPU;
        }

        delete m_ModelTextureList.at(i);
    }

    m_ModelTextureList.clear();

    //delete all particle buffers


    if(m_DataVel != NULL)
    {
        if(!m_DoCPU)
        {
            m_DataVel->DestroyBufferObject();
        }
        SAFE_DELETE(m_DataVel);
    }
    if(m_DataIndex != NULL)
    {
        if(!m_DoCPU)
        {
            m_DataIndex->DestroyBufferObject();
        }
        SAFE_DELETE(m_DataIndex);
    }
    if(m_DataIndexC != NULL)
    {
        if(!m_DoCPU)
        {
            m_DataIndexC->DestroyBufferObject();
        }
        SAFE_DELETE(m_DataIndexC);
    }
    if(m_DataUI != NULL)
    {

        m_DataUI->DestroyBufferObject();
        if(!m_DoCPU)
        {
        }
        SAFE_DELETE(m_DataUI);
    }
    if(m_DataPos != NULL)
    {

        m_DataPos->DestroyBufferObject();

        if(!m_DoCPU)
        {
        }
        SAFE_DELETE(m_DataPos);
    }
    if(m_DataStressTensor != NULL)
    {
        m_DataStressTensor->DestroyBufferObject();
        SAFE_DELETE(m_DataStressTensor);
    }
    if(m_DataDispTensor != NULL)
    {
        if(!m_DoCPU)
        {
            m_DataDispTensor->DestroyBufferObject();
        }
        SAFE_DELETE(m_DataDispTensor);
    }
    if(m_DataFluidProps != NULL)
    {
        if(!m_DoCPU)
        {
            m_DataFluidProps->DestroyBufferObject();
        }
        SAFE_DELETE(m_DataFluidProps);
    }
    if(m_DataProps != NULL)
    {
        if(!m_DoCPU)
        {
            m_DataProps->DestroyBufferObject();
        }
        SAFE_DELETE(m_DataProps);
    }
    if(m_DataVelN != NULL)
    {
        if(!m_DoCPU)
        {
            m_DataVelN->DestroyBufferObject();
        }
        SAFE_DELETE(m_DataVelN);
    }
    if(m_DataPosN != NULL)
    {
        if(!m_DoCPU)
        {
            m_DataPosN->DestroyBufferObject();
        }
        SAFE_DELETE(m_DataPosN);
    }
    if(m_DataStressTensorN != NULL)
    {
        if(!m_DoCPU)
        {
            m_DataStressTensorN->DestroyBufferObject();
        }
        SAFE_DELETE(m_DataStressTensorN);
    }
    if(m_DataPropsPhysical != NULL)
    {
        if(!m_DoCPU)
        {
            m_DataPropsPhysical->DestroyBufferObject();
        }
        SAFE_DELETE(m_DataPropsPhysical);
    }


    /*SAFE_DELETE(m_CLProgram_Particles);
    SAFE_DELETE(m_CLProgram_Particles2);
    SAFE_DELETE(m_CLProgram_ParticlesToMaps);
    SAFE_DELETE(m_CLProgram_ParticlesFromMaps);
    SAFE_DELETE(m_CLProgram_Flow);
    SAFE_DELETE(m_CLProgram_Flow2);
    SAFE_DELETE(m_CLProgram_Infiltration);
    SAFE_DELETE(m_CLProgram_Infiltration2);
    SAFE_DELETE(m_CLProgram_Hydrology);
    SAFE_DELETE(m_CLProgram_Hydrology2);
    SAFE_DELETE(m_CLProgram_Erosion);
    SAFE_DELETE(m_CLProgram_Erosion2);
    SAFE_DELETE(m_CLProgram_Rigid);
    SAFE_DELETE(m_CLProgram_Rigid2);
    SAFE_DELETE(m_CLProgram_GroundWaterFlow);
    SAFE_DELETE(m_CLProgram_GroundWaterFlow2);
    SAFE_DELETE(m_CLProgram_SlopeStability);
    SAFE_DELETE(m_CLProgram_SlopeStabilityE);
    SAFE_DELETE(m_CLProgram_SlopeFailure);
    SAFE_DELETE(m_CLProgram_SlopeFailureE);
    SAFE_DELETE(m_CLProgram_SlopeFailure2);
    SAFE_DELETE(m_CLProgram_SlopeFailureP2);
    SAFE_DELETE(m_CLProgram_SubSurfaceForce1);
    SAFE_DELETE(m_CLProgram_SubSurfaceForce2);
    SAFE_DELETE(m_CLProgram_FlowSolids);
    SAFE_DELETE(m_CLProgram_FlowSolids2);
    SAFE_DELETE(m_CLProgram_FlowSolidsParticle);
    SAFE_DELETE(m_CLProgram_FlowSolidsParticle2);
    SAFE_DELETE(m_CLProgram_Sort);
    SAFE_DELETE(m_CLProgram_StoreIndices);*/

    LISEM_STATUS("Finished deleting all model related memory objects");


}

QString LISEMModel::GetOptionString(QString name)
{
    for(int i = 0; i < m_Options.length(); i++)
    {
        QStringList split = m_Options.at(i).split("=");

        if(split.length() > 1)
        {
            if(split.at(0).compare(name.trimmed()) == 0)
            {
                    return split.at(1);
            }
        }
    }

    return "";

}

int LISEMModel::GetOptionInt(QString name)
{
    for(int i = 0; i < m_Options.length(); i++)
    {
        QStringList split = m_Options.at(i).split("=");

        if(split.length() > 1)
        {
            if(split.at(0).compare(name.trimmed()) == 0)
            {
                bool ok = true;
                double val = split.at(1).toDouble(&ok);
                if(ok)
                {

                    return val;
                }else
                {
                    LISEM_ERROR("Could not read value for parameter " + name + " : " + m_Options.at(i));
                    return 0;
                }
            }
        }
    }

    return 0;

}

double LISEMModel::GetOptionDouble(QString name)
{
    for(int i = 0; i < m_Options.length(); i++)
    {
        QStringList split = m_Options.at(i).split("=");

        if(split.length() > 1)
        {
            if(split.at(0).compare(name.trimmed()) == 0)
            {
                bool ok = true;
                double val = split.at(1).toDouble(&ok);
                if(ok)
                {

                    return val;
                }else
                {
                    return 0;
                }
            }
        }
    }

    return 0;

}


void LISEMModel::CreateParticleBuffers(int size_req, bool init)
{
    std::cout << "allocating particle buffers " << std::endl;

    int n_particles_init = size_req;
    unsigned int n = n_particles_init/1 - 1;
    unsigned int p2 = 0;

    size_t original_vertex_size = n_particles_init;
    do ++p2; while( (n >>= 0x1) != 0);
    size_t padded_size = 0x1 << p2;

    n_particles_init = (int)padded_size;

    std::cout << "create particle buffers, " << size_req << std::endl;

    if(m_NParticlePadded > 0)
    {
        LISEM_DEBUG("Resizing and re-allocating particle buffers");

        if(!(n_particles_init > m_NParticlePadded))
        {

            std::cout << "smaller " << std::endl;

            m_DataPos->InitData(2 * n_particles_init);
            m_DataVel->InitData(2 * n_particles_init);
            m_DataStressTensor->InitData(4* n_particles_init );
            m_DataDispTensor->InitData(4* n_particles_init );
            m_DataFluidProps->InitData(5* n_particles_init );
            m_DataProps->InitData(4* n_particles_init );


            m_DataPosN->InitData(2 * n_particles_init);
            m_DataVelN->InitData(2 * n_particles_init);
            m_DataStressTensorN->InitData(4* n_particles_init );
            m_DataPropsPhysical->InitData(4* n_particles_init );

            m_DataUI->InitData(2* n_particles_init);
            m_DataIndex->InitData(n_particles_init);
            m_DataIndexC->InitData(n_particles_init);


            m_NParticle = init? 0:size_req;
            m_NParticlePadded = n_particles_init;
        }else
        {


            std::cout << "larger " << std::endl;

            OpenGLCLDataBuffer<float> *tm_DataPos = new OpenGLCLDataBuffer<float>();
            OpenGLCLDataBuffer<float> *tm_DataVel = new OpenGLCLDataBuffer<float>();
            OpenGLCLDataBuffer<int> *tm_DataIndex = new OpenGLCLDataBuffer<int>();
            OpenGLCLDataBuffer<int> *tm_DataIndexC = new OpenGLCLDataBuffer<int>();
            OpenGLCLDataBuffer<float> *tm_DataUI = new OpenGLCLDataBuffer<float>();
            OpenGLCLDataBuffer<float> *tm_DataStressTensor = new OpenGLCLDataBuffer<float>();
            OpenGLCLDataBuffer<float> *tm_DataDispTensor = new OpenGLCLDataBuffer<float>();
            OpenGLCLDataBuffer<float> *tm_DataFluidProps = new OpenGLCLDataBuffer<float>();
            OpenGLCLDataBuffer<float> *tm_DataStressDTensor = new OpenGLCLDataBuffer<float>();
            OpenGLCLDataBuffer<float> *tm_DataPosN = new OpenGLCLDataBuffer<float>();
            OpenGLCLDataBuffer<float> *tm_DataVelN = new OpenGLCLDataBuffer<float>();
            OpenGLCLDataBuffer<float> *tm_DataStressTensorN = new OpenGLCLDataBuffer<float>();
            OpenGLCLDataBuffer<float> *tm_DataStressDTensorN = new OpenGLCLDataBuffer<float>();

            m_DataPos->InitData(2 * n_particles_init);
            m_DataVel->InitData(2 * n_particles_init);
            m_DataStressTensor->InitData(4* n_particles_init );
            m_DataDispTensor->InitData(4* n_particles_init );
            m_DataFluidProps->InitData(5* n_particles_init );
            m_DataProps->InitData(4* n_particles_init );

            m_DataPosN->InitData(2 * n_particles_init);
            m_DataVelN->InitData(2 * n_particles_init);
            m_DataStressTensorN->InitData(4* n_particles_init );
            m_DataPropsPhysical->InitData(4* n_particles_init );

            m_DataUI->InitData(2* n_particles_init);
            m_DataIndex->InitData(n_particles_init);
            m_DataIndexC->InitData(n_particles_init);


            tm_DataPos->InitData(m_DataPos);
            tm_DataVel->InitData(m_DataVel);
            tm_DataUI->InitData(m_DataUI);
            tm_DataStressTensor->InitData(m_DataStressTensor );
            tm_DataDispTensor->InitData(m_DataDispTensor );
            tm_DataFluidProps->InitData(m_DataFluidProps );
            tm_DataStressDTensor->InitData(m_DataProps );
            tm_DataPosN->InitData(m_DataPosN);
            tm_DataVelN->InitData(m_DataVelN);
            tm_DataStressTensorN->InitData(m_DataStressTensorN);
            tm_DataStressDTensorN->InitData(m_DataPropsPhysical);
            tm_DataIndex->InitData(m_DataIndex);
            tm_DataIndexC->InitData(m_DataIndexC);


            m_DataPos->DestroyBufferObject();

            m_DataUI->DestroyBufferObject();

            if(!m_DoCPU)
            {
                m_DataVel->DestroyBufferObject();
                m_DataIndex->DestroyBufferObject();
                m_DataIndexC->DestroyBufferObject();
                m_DataStressTensor->DestroyBufferObject();
                m_DataDispTensor->DestroyBufferObject();
                m_DataFluidProps->DestroyBufferObject();
                m_DataProps->DestroyBufferObject();
                m_DataVelN->DestroyBufferObject();
                m_DataPosN->DestroyBufferObject();
                m_DataStressTensorN->DestroyBufferObject();
                m_DataPropsPhysical->DestroyBufferObject();
            }

            SAFE_DELETE(m_DataVel);
            SAFE_DELETE(m_DataIndex);
            SAFE_DELETE(m_DataIndexC);
            SAFE_DELETE(m_DataUI);
            SAFE_DELETE(m_DataPos);
            SAFE_DELETE(m_DataStressTensor);
            SAFE_DELETE(m_DataDispTensor);
            SAFE_DELETE(m_DataFluidProps);
            SAFE_DELETE(m_DataFluidProps);
            SAFE_DELETE(m_DataProps);
            SAFE_DELETE(m_DataVelN);
            SAFE_DELETE(m_DataPosN);
            SAFE_DELETE(m_DataStressTensorN);
            SAFE_DELETE(m_DataPropsPhysical);

            m_DataVel =tm_DataVel;
            m_DataIndex=tm_DataIndex;
            m_DataIndexC=tm_DataIndexC;
            m_DataUI=tm_DataUI;
            m_DataPos=tm_DataPos;
            m_DataStressTensor=tm_DataStressTensor;
            m_DataDispTensor=tm_DataDispTensor;
            m_DataFluidProps=tm_DataFluidProps;
            m_DataProps=tm_DataStressDTensor;
            m_DataVelN=tm_DataVelN;
            m_DataPosN=tm_DataPosN;
            m_DataStressTensorN=tm_DataStressTensorN;
            m_DataPropsPhysical=tm_DataStressDTensorN;

            for(int i = m_NParticle; i < n_particles_init; i++)
            {
                m_DataIndex->m_Data[i] = i;
            }

            for(int i = m_NParticle; i < n_particles_init; i++)
            {
                m_DataStressTensor->m_Data[i*4] = 0.0;
                m_DataProps->m_Data[i*4] = 0.0;
                m_DataStressTensor->m_Data[i*4+1] = 0.0;
                m_DataProps->m_Data[i*4+1] = 0.0;
                m_DataStressTensor->m_Data[i*4+2] = 0.0;
                m_DataProps->m_Data[i*4+2] = 0.0;
                m_DataStressTensor->m_Data[i*4+3] = 0.0;
                m_DataProps->m_Data[i*4+3] = 1.0;

                m_DataDispTensor->m_Data[i*4] = 0.0;
                m_DataDispTensor->m_Data[i*4+1] = 0.0;
                m_DataDispTensor->m_Data[i*4+2] = 0.0;
                m_DataDispTensor->m_Data[i*4+3] = 0.0;


                m_DataFluidProps->m_Data[i*5] = 0.0;
                m_DataFluidProps->m_Data[i*5+1] = 0.0;
                m_DataFluidProps->m_Data[i*5+2] = 0.0;
                m_DataFluidProps->m_Data[i*5+3] = 0.0;
                m_DataFluidProps->m_Data[i*5+4] = 0.0;

                m_DataStressTensorN->m_Data[i*4] = 0.0;
                m_DataPropsPhysical->m_Data[i*4] = 0.0;
                m_DataStressTensorN->m_Data[i*4+1] = 0.0;
                m_DataPropsPhysical->m_Data[i*4+1] = 0.0;
                m_DataStressTensorN->m_Data[i*4+2] = 0.0;
                m_DataPropsPhysical->m_Data[i*4+2] = 0.0;
                m_DataStressTensorN->m_Data[i*4+3] = 0.0;
                m_DataPropsPhysical->m_Data[i*4+3] = 0.0;

                m_DataUI->m_Data[i*2] = i;
                m_DataVel->m_Data[i*2] = 0.0;//-50.0;
                m_DataVel->m_Data[i*2+1] = 0.0;
                m_DataPos->m_Data[i*2] =  i;
                m_DataPos->m_Data[i*2+1] =  i;
                m_DataVelN->m_Data[i*2] = 0.0;
                m_DataVelN->m_Data[i*2+1] = 0.0;
                m_DataPosN->m_Data[i*2] =  i;
                m_DataPosN->m_Data[i*2+1] =  i;
            }


            m_DataUI->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init);
            m_DataPos->CreateBufferObjectWithSecondVertexAttribute(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init,m_DataUI,2,2);


            if(!m_DoCPU)
            {
                m_DataVel->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init);
                m_DataIndex->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init,1,GL_INT,GL_DYNAMIC_DRAW,CL_MEM_READ_WRITE,false);
                m_DataIndexC->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init,1,GL_INT,GL_DYNAMIC_DRAW,CL_MEM_READ_WRITE,false);
                m_DataStressTensor->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init,4);
                m_DataDispTensor->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init,4);
                m_DataFluidProps->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init,5);
                m_DataProps->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init,4);
                m_DataVelN->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init);
                m_DataPosN->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init);
                m_DataStressTensorN->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init,4);
                m_DataPropsPhysical->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init,4);
            }

            //since some input related to the kernels has changed, reset them
            SetKernelInput();

            m_NParticle = init? 0:size_req;
            m_DataPos->SetDrawDim(m_NParticle);
            m_NParticlePadded = n_particles_init;
        }
    }else
    {

        std::cout << "new " << std::endl;

        LISEM_DEBUG("Allocating particle buffers");

        m_DataPos = new OpenGLCLDataBuffer<float>();
        m_DataVel = new OpenGLCLDataBuffer<float>();
        m_DataIndex = new OpenGLCLDataBuffer<int>();
        m_DataIndexC = new OpenGLCLDataBuffer<int>();
        m_DataUI = new OpenGLCLDataBuffer<float>();
        m_DataStressTensor = new OpenGLCLDataBuffer<float>();
        m_DataDispTensor = new OpenGLCLDataBuffer<float>();
        m_DataFluidProps = new OpenGLCLDataBuffer<float>();
        m_DataProps = new OpenGLCLDataBuffer<float>();
        m_DataPosN = new OpenGLCLDataBuffer<float>();
        m_DataVelN = new OpenGLCLDataBuffer<float>();
        m_DataStressTensorN = new OpenGLCLDataBuffer<float>();
        m_DataPropsPhysical = new OpenGLCLDataBuffer<float>();

        m_DataPos->InitData(2 * n_particles_init);
        m_DataVel->InitData(2 * n_particles_init);
        m_DataUI->InitData(2* n_particles_init);
        m_DataStressTensor->InitData(4* n_particles_init );
        m_DataDispTensor->InitData(4* n_particles_init );
        m_DataFluidProps->InitData(5* n_particles_init );
        m_DataProps->InitData(4* n_particles_init );

        m_DataPosN->InitData(2 * n_particles_init);
        m_DataVelN->InitData(2 * n_particles_init);
        m_DataStressTensorN->InitData(4* n_particles_init );
        m_DataPropsPhysical->InitData(4* n_particles_init );

        m_DataIndex->InitData(n_particles_init);
        m_DataIndexC->InitData(n_particles_init);

        for(int i = 0; i < n_particles_init; i++)
        {
            m_DataIndex->m_Data[i] = i;
        }

        for(int i = 0; i < n_particles_init; i++)
        {
            m_DataStressTensor->m_Data[i*4] = 0.0;
            m_DataProps->m_Data[i*4] = 0.0;
            m_DataStressTensor->m_Data[i*4+1] = 0.0;
            m_DataProps->m_Data[i*4+1] = 0.0;
            m_DataStressTensor->m_Data[i*4+2] = 0.0;
            m_DataProps->m_Data[i*4+2] = 0.0;
            m_DataStressTensor->m_Data[i*4+3] = 0.0;
            m_DataProps->m_Data[i*4+3] = 1.0;

            //some default values
            m_DataStressTensorN->m_Data[i*4] = 0.0;
            m_DataPropsPhysical->m_Data[i*4] = 0.3;
            m_DataStressTensorN->m_Data[i*4+1] = 0.0;
            m_DataPropsPhysical->m_Data[i*4+1] = 0.1;
            m_DataStressTensorN->m_Data[i*4+2] = 0.0;
            m_DataPropsPhysical->m_Data[i*4+2] = 2400;
            m_DataStressTensorN->m_Data[i*4+3] = 0.0;
            m_DataPropsPhysical->m_Data[i*4+3] = 50;

            m_DataDispTensor->m_Data[i*4] = 0.0;
            m_DataDispTensor->m_Data[i*4+1] = 0.0;
            m_DataDispTensor->m_Data[i*4+2] = 0.0;
            m_DataDispTensor->m_Data[i*4+3] = 0.0;

            m_DataFluidProps->m_Data[i*5] = 0.0;
            m_DataFluidProps->m_Data[i*5+1] = 0.0;
            m_DataFluidProps->m_Data[i*5+2] = 0.0;
            m_DataFluidProps->m_Data[i*5+3] = 0.0;
            m_DataFluidProps->m_Data[i*5+4] = 0.0;

            m_DataUI->m_Data[i*2] = i;
            m_DataVel->m_Data[i*2] = 0.0;
            m_DataVel->m_Data[i*2+1] = 0.0;
            m_DataPos->m_Data[i*2] =  i;
            m_DataPos->m_Data[i*2+1] =  i;
            m_DataVelN->m_Data[i*2] = 0.0;
            m_DataVelN->m_Data[i*2+1] = 0.0;
            m_DataPosN->m_Data[i*2] =  i;
            m_DataPosN->m_Data[i*2+1] =  i;
        }

        m_DataUI->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init);
         m_DataPos->CreateBufferObjectWithSecondVertexAttribute(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init,m_DataUI,2,2);

        if(!m_DoCPU)
        {
            m_DataVel->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init);
            m_DataIndex->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init,1,GL_INT,GL_DYNAMIC_DRAW,CL_MEM_READ_WRITE,false);
            m_DataIndexC->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init,1,GL_INT,GL_DYNAMIC_DRAW,CL_MEM_READ_WRITE,false);

            m_DataStressTensor->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init,4);
            m_DataDispTensor->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init,4);
            m_DataFluidProps->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init,5);
            m_DataProps->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init,4);
            m_DataVelN->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init);
            m_DataPosN->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init);
            m_DataStressTensorN->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init,4);
            m_DataPropsPhysical->CreateBufferObject(m_OpenGLCLManager->context,m_OpenGLCLManager->q,n_particles_init,4);
        }

        m_NParticle = init? 0:size_req;
        m_DataPos->SetDrawDim(m_NParticle);
        m_NParticlePadded = n_particles_init;
    }

    m_OpenGLCLManager->q.finish();

}


void LISEMModel::CreateParticleFromFailures()
{
    int particle_req = 0;
    int nparticle_old = m_NParticle;

        for(int r = 0; r < DEM->nrRows(); r++)
        {
            for (int c = 0; c < DEM->nrCols(); c++)
            {
                if(!pcr::isMV(DEM->data[r][c]) &&!pcr::isMV(SLOPEFAILURE->data[r][c]) )
                {
                    float hs = SLOPEFAILURE->data[r][c] * (1.0 -THETAS->Drc);
                    particle_req += std::floor((hs/ _dx)*factor_particle);
                }
            }
        }


    if(particle_req > 0)
    {
        LISEM_DEBUG("Adding slope failure as particles");

        CreateParticleBuffers( nparticle_old + particle_req,false);
        LISEM_DEBUG("particle required for failure" + QString::number(nparticle_old) + "  " + QString::number(particle_req));


        LISEM_DEBUG(QString("Set particles from initial solids n: %1 ").arg(particle_req));

        int i_total = 0;
        for(int r = 0; r < DEM->nrRows(); r++)
        {
            for (int c = 0; c < DEM->nrCols(); c++)
            {
                if(!pcr::isMV(DEM->data[r][c]) &&!pcr::isMV(SLOPEFAILURE->data[r][c]) )
                {
                    float hs = SLOPEFAILURE->data[r][c] * (1.0 -THETAS->Drc);
                    float hf = SLOPEFAILURE->data[r][c] * (THETAS->Drc);
                    int particle_req_here = std::floor((hs/ _dx)*factor_particle);
                    float volf_particle = hf * _dx * _dx/((float)(particle_req));

                    float x_c = r*_dx;
                    float y_c = c*_dx;

                    //we use a two-dimensional generalization of the golden ratio to place particles in a
                    //approximately evenly distributed way throughout the cell. This method looks pseudo-random
                    //and spaces rather evenly (as far as possible), and avoids pitfalls of having straight rows
                    //or columns of particles that have higher connectivity with each other than neighbor cells
                    //for more information see:

                    float nfit = sqrt((float) particle_req_here);
                    float spacing = _dx/(1.0f + ((float) particle_req_here)/nfit);
                    for(int i_cell = 0; i_cell < particle_req_here; i_cell++)
                    {
                        float movedfromlefttop = i_cell * spacing + spacing * ( 1.0f *(float)(std::floor(i_cell/nfit)));
                        float g = 1.32471795724474602596;
                        float a1 = 1.0/g;
                        float a2 = 1.0/(g*g);
                        float xreln = fmod((0.5+a1*((float)(i_cell))),1.0f) * _dx;
                        float yreln = fmod((0.5+a2*((float)(i_cell))),1.0f) * _dx;

                        float x_particle = x_c + xreln;//std::fmod(movedfromlefttop,_dx);
                        float y_particle = y_c + yreln;//spacing *(std::floor(movedfromlefttop/_dx));


                        int index = nparticle_old + i_total + i_cell;

                        m_DataStressTensor->m_Data[index*4] = 0.0;
                        m_DataProps->m_Data[index*4] = 0.0;
                        m_DataStressTensor->m_Data[index*4+1] = 0.0;
                        m_DataProps->m_Data[index*4+1] = 0.0;
                        m_DataStressTensor->m_Data[index*4+2] = 0.0;
                        m_DataProps->m_Data[index*4+2] = 0.0;
                        m_DataStressTensor->m_Data[index*4+3] = 0.0;
                        m_DataProps->m_Data[index*4+3] = 1.0;

                        m_DataUI->m_Data[index*2] = index;
                        m_DataVel->m_Data[index*2] = 0.0;
                        m_DataVel->m_Data[index*2+1] = 0.0;
                        m_DataPos->m_Data[index*2] =y_particle;
                        m_DataPos->m_Data[index*2+1] =x_particle;

                        m_DataDispTensor->m_Data[index*4] = 0.0;
                        m_DataDispTensor->m_Data[index*4+1] = 0.0;
                        m_DataDispTensor->m_Data[index*4+2] = 0.0;
                        m_DataDispTensor->m_Data[index*4+3] = 0.0;

                        m_DataFluidProps->m_Data[index*5] = volf_particle;
                        m_DataFluidProps->m_Data[index*5+1] = 0.0;
                        m_DataFluidProps->m_Data[index*5+2] = 0.0;
                        m_DataFluidProps->m_Data[index*5+3] = 0.0;
                        m_DataFluidProps->m_Data[index*5+4] = 0.0;

                        m_DataStressTensorN->m_Data[index*4] = 0.0;
                        m_DataPropsPhysical->m_Data[index*4] = IFA->Drc;
                        m_DataStressTensorN->m_Data[index*4+1] = 0.0;
                        m_DataPropsPhysical->m_Data[index*4+1] = ROCKSIZE->Drc;
                        m_DataStressTensorN->m_Data[index*4+2] = 0.0;
                        m_DataPropsPhysical->m_Data[index*4+2] = DENSITY_BOTTOM->Drc;
                        m_DataStressTensorN->m_Data[index*4+3] = 0.0;
                        m_DataPropsPhysical->m_Data[index*4+3] = COHESION->Drc;

                    }
                    i_total += particle_req_here;
                }
            }

        }

        LISEM_DEBUG(QString("Set particles from failures n: %1 ").arg(i_total));


        std::cout << "update particles from failure "<< std::endl;
        m_DataPos->UpdateBufferFromData(m_OpenGLCLManager->q);

        m_DataPos->SetDrawDim(m_NParticle);

        m_DataUI->UpdateBufferFromData(m_OpenGLCLManager->q);

        if(!m_DoCPU)
        {
            m_DataVel->UpdateBufferFromData(m_OpenGLCLManager->q);
            m_DataStressTensor->UpdateBufferFromData(m_OpenGLCLManager->q);
            m_DataDispTensor->UpdateBufferFromData(m_OpenGLCLManager->q);
            m_DataFluidProps->UpdateBufferFromData(m_OpenGLCLManager->q);
            m_DataProps->UpdateBufferFromData(m_OpenGLCLManager->q);
            m_DataStressTensorN->UpdateBufferFromData(m_OpenGLCLManager->q);
            m_DataPropsPhysical->UpdateBufferFromData(m_OpenGLCLManager->q);

            m_CLProgram_Particles->SetRunDims(m_NParticle);
            m_DataPos->SetDrawDim(m_NParticle);
            m_CLProgram_Particles2->SetRunDims(m_NParticle);
            m_CLProgram_ParticlesFromMaps->SetRunDims(m_NParticle);
        }


    }

    for(int r = 0; r < DEM->nrRows(); r++)
    {
        for (int c = 0; c < DEM->nrCols(); c++)
        {
            if(!pcr::isMV(DEM->data[r][c]) )
            {
                SLOPEFAILURE->data[r][c] = 0.0;
            }
        }
    }


}



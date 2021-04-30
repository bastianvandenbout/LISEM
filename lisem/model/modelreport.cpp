#include "model.h"

#include <QTime>
#include "error.h"
#include "iogdal.h"
#include "iogdalshape.h"
#include <iostream>

#include "geo/shapes/shapefile.h"

void LISEMModel::GetOutput(float t, float dt)
{

    //copy maps we really need for reporting, output and other calculations
    //this should be limited as much as possible since this is a relatively slow operation
    //most importantly, while we do this, the opencl environment must wait, and the opengl environment must wait.
    //to limit memory usage on the gpu, we only have the base maps and a ui-chosen map with a copied texture available for drawing while the model is running
    if(m_DoSlopeStability && m_DoHydrology && m_DoSlopeFailure )
    {

        //get slope failures during this timestep
        m_OpenGLCLManager->CopyTextureToMap(T_SFHCUMN,&(SLOPEFAILURE->data));

        if( m_DoParticleSolids)
        {
            //since we are simulating solids using particles, create the new particles
            CreateParticleFromFailures();
        }
    }

    if(m_DoRigidWorld)
    {
        FOR_ROW_COL(DEM)
        {
            m_BlockX->data[r][c] = m_RigidWorld->m_BlockX->data[r][c];
            m_BlockY->data[r][c] = m_RigidWorld->m_BlockY->data[r][c];
        }
    }

    m_OpenGLCLManager->CopyTextureToMap(T_H,&(HF->data));
    m_OpenGLCLManager->CopyTextureToMap(T_VX,&(VFX->data));
    m_OpenGLCLManager->CopyTextureToMap(T_VY,&(VFY->data));

    m_OpenGLCLManager->CopyTextureToMap(T_HFMax,&(HFMax->data));
    m_OpenGLCLManager->CopyTextureToMap(T_VFMax,&(VFMax->data));

    m_OpenGLCLManager->CopyTextureToMap(T_QFOUT,&(QFOUT->data));

    if(m_DoInfiltration || m_DoHydrology)
    {
        m_OpenGLCLManager->CopyTextureToMap(T_F_INFILACT,&(INFIL->data));
    }


    if(m_DoHydrology)
    {

        m_OpenGLCLManager->CopyTextureToMap(T_SSURFACE,&(SURFACESTORAGE->data));
        m_OpenGLCLManager->CopyTextureToMap(T_SCANOPY,&(CANOPYSTORAGE->data));

    }
    if(m_DoHydrology && m_DoGroundWater)
    {

        m_OpenGLCLManager->CopyTextureToMap(T_QGWOUT,&(GWOUT->data));

    }

    if(m_DoEvapoTranspiration)
    {


    }

    if(m_DoChannel)
    {
        m_OpenGLCLManager->CopyTextureToMap(T_CHH,&(CHHF->data));
        m_OpenGLCLManager->CopyTextureToMap(T_CHVX,&(CHVFX->data));

    }


    if(m_DoInitialSolids || (m_DoSlopeFailure && m_DoHydrology && m_DoSlopeStability))
    {

        m_OpenGLCLManager->CopyTextureToMap(T_QSOUT,&(QSOUT->data));


        m_OpenGLCLManager->CopyTextureToMap(T_HS,&(HS->data));
        m_OpenGLCLManager->CopyTextureToMap(T_VSX,&(VSX->data));
        m_OpenGLCLManager->CopyTextureToMap(T_VSY,&(VSY->data));

        m_OpenGLCLManager->CopyTextureToMap(T_HSMax,&(HSMax->data));
        m_OpenGLCLManager->CopyTextureToMap(T_VSMax,&(VSMax->data));

        if(m_DoChannel)
        {
            m_OpenGLCLManager->CopyTextureToMap(T_CHHS,&(CHHS->data));
            m_OpenGLCLManager->CopyTextureToMap(T_CHVS,&(CHVSX->data));
        }
    }

    bool do_solids = m_DoInitialSolids || (m_DoSlopeFailure && m_DoHydrology && m_DoSlopeStability);
    bool failure =m_DoSlopeStability && m_DoHydrology && m_DoSlopeFailure;

    if(m_DoSlopeStability && m_DoHydrology)
    {
        m_OpenGLCLManager->CopyTextureToMap(T_SF,&(SF->data));
        m_OpenGLCLManager->CopyTextureToMap(T_SFC,&(SFC->data));
    }

    if(m_DoParticleSolids && (m_DoInitialSolids || (m_DoHydrology &&m_DoSlopeStability)))
    {
        if(m_NParticle > 1)
        {

            if(m_DoCPU)
            {
                std::cout << "fill buffers during report" << std::endl;
                m_DataPos->UpdateBufferFromDataGL();
                m_DataUI->UpdateBufferFromDataGL();
            }

            if(!m_DoCPU)
            {
                m_DataPos->UpdateDataFromBuffer(m_OpenGLCLManager->q);
                m_DataUI->UpdateDataFromBuffer(m_OpenGLCLManager->q);
                m_DataVel->UpdateDataFromBuffer(m_OpenGLCLManager->q);
                m_DataStressTensor->UpdateDataFromBuffer(m_OpenGLCLManager->q);
                m_DataDispTensor->UpdateDataFromBuffer(m_OpenGLCLManager->q);
                m_DataFluidProps->UpdateDataFromBuffer(m_OpenGLCLManager->q);
                m_DataProps->UpdateDataFromBuffer(m_OpenGLCLManager->q);
                m_DataFluidProps->UpdateDataFromBuffer(m_OpenGLCLManager->q);
                m_DataStressTensorN->UpdateDataFromBuffer(m_OpenGLCLManager->q);
                m_DataPropsPhysical->UpdateDataFromBuffer(m_OpenGLCLManager->q);
            }
        }
    }



    FOR_ROW_COL_MV(DEM)
    {
        float momentum = 0.0;

        VELF->Drc = sqrt(VFX->Drc * VFX->Drc + VFY->Drc*VFY->Drc);
        if(do_solids)
        {
            VELS->Drc = sqrt(VSX->Drc * VSX->Drc + VSY->Drc*VSY->Drc);
        }
        if(failure)
        {
            SLOPEFAILURETOTAL->Drc = SLOPEFAILURETOTAL->Drc + SLOPEFAILURE->Drc;
        }
        if(m_DoSlopeStability && m_DoHydrology)
        {

            SFMIN->Drc = std::min(SFMIN->Drc,SF->Drc);
        }

        if(m_DoInitialSolids || (m_DoSlopeFailure && m_DoHydrology && m_DoSlopeStability))
        {
            SFrac->Drc = HS->Drc/(std::max(0.0001f,HF->Drc + HS->Drc));
            SFracMax->Drc = std::max(SFrac->Drc,SFracMax->Drc);
        }

        momentum += VELF->Drc * HF->Drc* _dx;
        FluxFTotal->Drc += VELF->Drc * HF->Drc * _dx*dt;
        if(m_DoChannel)
        {
            if((CH_M->data[r][c] > 0 && CH_M->data[r][c] < 10))
            {
                FluxFTotal->Drc += CHHF->Drc * std::fabs(CHVFX->Drc) *dt * CH_WIDTH->Drc;
                momentum += CHHF->Drc * std::fabs(CHVFX->Drc)* CH_WIDTH->Drc;
            }
        }
        if(m_DoInitialSolids || (m_DoSlopeFailure && m_DoHydrology && m_DoSlopeStability))
        {
            momentum += VELS->Drc * HS->Drc * (2000.0/1000.0)* _dx; //estimate density of solids as 2000 kg/m3 and water as 1000 kg/m3
            FluxSTotal->Drc += VELS->Drc * HS->Drc *_dx * dt;

            if(m_DoChannel)
            {
                if((CH_M->data[r][c] > 0 && CH_M->data[r][c] < 10))
                {
                    FluxSTotal->Drc += CHHS->Drc * std::fabs(CHVSX->Drc) *dt * CH_WIDTH->Drc;
                    momentum += CHHS->Drc * std::fabs(CHVSX->Drc) * (2000.0/1000.0)* CH_WIDTH->Drc;
                }
            }
        }

        if(momentum > PeakMomentum->Drc)
        {
            PeakMomentum->Drc = momentum;
            FluxFPeakMomentum->Drc = VELF->Drc * HF->Drc * _dx;
            if(m_DoChannel)
            {
                if((CH_M->data[r][c] > 0 && CH_M->data[r][c] < 10))
                {
                    FluxFPeakMomentum->Drc += CHHF->Drc * std::fabs(CHVFX->Drc) * CH_WIDTH->Drc;
                }
            }
            if(m_DoInitialSolids || (m_DoSlopeFailure && m_DoHydrology && m_DoSlopeStability))
            {
                FluxSPeakMomentum->Drc = VELS->Drc * HS->Drc *_dx;
                if(m_DoChannel)
                {
                    if((CH_M->data[r][c] > 0 && CH_M->data[r][c] < 10))
                    {
                        FluxSPeakMomentum->Drc += CHHS->Drc * std::fabs(CHVSX->Drc)* CH_WIDTH->Drc;
                    }
                }
            }
        }
    }


}


//this function is called from a seperate thread that is restarted after each timestep.
//the maps meant for output can be safely written in GetOutput(), and further only be used during this function
void LISEMModel::ReportOutputToDisk()
{

    QString ext = m_DoOutputTiff? ".tif" : ".map";
    QString format = m_DoOutputTiff? "GTIFF" : "PCRaster";

    FinishResult.Name_HFFINAL = QString("hffinal" + ext) ;
    FinishResult.Name_VELFFINAL = QString("vffinal" + ext) ;
    FinishResult.Name_HFMAX = QString("hfmax" + ext) ;
    FinishResult.Name_VFMAX = QString("vfmax" + ext) ;
    FinishResult.Name_INFIL = QString("infiltotal" + ext) ;
    FinishResult.Name_SURFACESTORAGE = QString("surfstor" + ext) ;
    FinishResult.Name_CANOPYSTORAGE = QString("canstor" + ext) ;
    FinishResult.Name_CHHFFINAL = QString("chhffinal" + ext) ;
    FinishResult.Name_CHVELFFINAL = QString("chvffinal" + ext) ;
    FinishResult.Name_HSFINAL = QString("hsfinal" + ext) ;
    FinishResult.Name_VELSFINAL = QString("vsfinal" + ext) ;
    FinishResult.Name_HSMAX = QString("hsmax" + ext) ;
    FinishResult.Name_VSMAX = QString("vsmax" + ext) ;
    FinishResult.Name_CHHSFINAL = QString("chhsfinal" + ext) ;
    FinishResult.Name_CHVELSFINAL = QString( "chvsfinal" + ext) ;
    FinishResult.Name_FAILURE = QString("slopefailure" + ext) ;
    FinishResult.Name_SF = QString("safetyfactor" + ext) ;
    FinishResult.Name_SFC = QString("safetyfactorc" + ext) ;
    FinishResult.Name_SFracMax = QString("sfracmax" + ext) ;

    writeRaster(*HF,m_Dir_Res + "hffinal" + ext,format);
    writeRaster(*VELF,m_Dir_Res + "vffinal" + ext,format);

    writeRaster(*HFMax,m_Dir_Res + "hfmax" + ext,format);
    writeRaster(*VFMax,m_Dir_Res + "vfmax" + ext,format);

    if(m_DoHydrology || m_DoInfiltration)
    {
        writeRaster(*INFILTOTAL,m_Dir_Res + "infiltotal" + ext,format);
    }
    if(m_DoHydrology)
    {
        writeRaster(*SURFACESTORAGE,m_Dir_Res + "surfstor" + ext,format);
        writeRaster(*CANOPYSTORAGE,m_Dir_Res + "canstor" + ext,format);

    }
    if(m_DoHydrology && m_DoGroundWater)
    {


    }

    if(m_DoEvapoTranspiration)
    {


    }

    if(m_DoChannel)
    {
        writeRaster(*CHHF,m_Dir_Res + "chhffinal" + ext,format);
        writeRaster(*CHVFX,m_Dir_Res + "chvffinal" + ext,format);

    }

    if(m_DoRigidWorld)
    {
        writeRaster(*m_BlockX,m_Dir_Res + "finalblockx" + ext,format);
        writeRaster(*m_BlockY,m_Dir_Res + "finalblocky" + ext,format);
    }

    writeRaster(*FluxFPeakMomentum,m_Dir_Res + "ffluxpeak" + ext,format);
    writeRaster(*FluxFTotal,m_Dir_Res + "ffluxtotal" + ext,format);

    if(m_DoInitialSolids || (m_DoSlopeFailure && m_DoHydrology && m_DoSlopeStability))
    {
        writeRaster(*FluxSPeakMomentum,m_Dir_Res + "sfluxpeak" + ext,format);
        writeRaster(*FluxSTotal,m_Dir_Res + "sfluxtotal" + ext,format);

        writeRaster(*HS,m_Dir_Res + "hsfinal" + ext,format);
        writeRaster(*VELS,m_Dir_Res + "vsfinal" + ext,format);

        writeRaster(*HSMax,m_Dir_Res + "hsmax" + ext,format);
        writeRaster(*VSMax,m_Dir_Res + "vsmax" + ext,format);

        writeRaster(*SFracMax,m_Dir_Res + "sfracmax" + ext,format);
        if(m_DoChannel)
        {
            writeRaster(*CHHS,m_Dir_Res + "chhsfinal" + ext,format);
            writeRaster(*CHVSX,m_Dir_Res + "chvsfinal" + ext,format);
        }



    }

    if(m_DoSlopeStability && m_DoHydrology )
    {
        writeRaster(*SFMIN,m_Dir_Res + "safetyfactor" + ext,format);
        writeRaster(*SFC,m_Dir_Res + "safetyfactorc" + ext,format);
    }


    if(m_DoSlopeStability && m_DoHydrology && m_DoSlopeFailure)
    {
        writeRaster(*SLOPEFAILURETOTAL,m_Dir_Res + "slopefailure" + ext,format);
    }

    if(m_DoOutputTimeseries)
    {
        if(m_Step % m_DoOutputTimeseriesEvery == 0)
        {
            if(m_DoOutputTimeseriesH)
            {
                WriteMapSeries(*HF,m_Dir_Res,"hf",m_Step,format);
            }
            if(m_DoInitialSolids || (m_DoSlopeFailure && m_DoHydrology && m_DoSlopeStability))
            {
                if(m_DoOutputTimeseriesHS)
                {
                    WriteMapSeries(*HS,m_Dir_Res,"hs",m_Step,format);
                }
                if(m_DoOutputTimeseriesVS)
                {
                    WriteMapSeries(*VELS,m_Dir_Res,"vs",m_Step,format);
                }
                if(m_DoOutputTimeseriesSFRAC)
                {
                    WriteMapSeries(*SFrac,m_Dir_Res,"sf",m_Step,format);
                }
            }
            if(m_DoOutputTimeseriesV)
            {
                WriteMapSeries(*VELF,m_Dir_Res,"vf",m_Step,format);
            }


            if(m_DoOutputTimeseriesHT)
            {
                FOR_ROW_COL_MV(DEM)
                {
                    HT->Drc = HS->Drc + HF->Drc;
                }

                WriteMapSeries(*HT,m_Dir_Res,"ht",m_Step,format);
            }
            if(m_DoOutputTimeseriesVT)
            {
                FOR_ROW_COL_MV(DEM)
                {
                    VT->Drc = HS->Drc * VELS->Drc + HF->Drc * VELF->Drc;
                }
                WriteMapSeries(*VT,m_Dir_Res,"vt",m_Step,format);
            }
        }
    }

    if(m_DoParticleSolids && (m_DoInitialSolids || (m_DoHydrology &&m_DoSlopeStability)))
    {
        if(m_NParticle > 1 && m_Step % m_DoOutputParticleTimeseriesEvery == 0)
        {

            ShapeFile * particlefile = new ShapeFile();
            ShapeLayer * particleLayer = new ShapeLayer();
            particleLayer->AddAttribute("POS_X",ATTRIBUTE_TYPE_FLOAT64);
            particleLayer->AddAttribute("POS_Y",ATTRIBUTE_TYPE_FLOAT64);
            particleLayer->AddAttribute("VEL_X",ATTRIBUTE_TYPE_FLOAT64);
            particleLayer->AddAttribute("VEL_Y",ATTRIBUTE_TYPE_FLOAT64);
            particleLayer->AddAttribute("VEL_NORM",ATTRIBUTE_TYPE_FLOAT64);
            particleLayer->AddAttribute("STRESS_I1",ATTRIBUTE_TYPE_FLOAT64);
            particleLayer->AddAttribute("STRESS_J2",ATTRIBUTE_TYPE_FLOAT64);
            particleLayer->AddAttribute("STRAIN_VOL",ATTRIBUTE_TYPE_FLOAT64);
            particleLayer->AddAttribute("FRAC_COH",ATTRIBUTE_TYPE_FLOAT64);

            particlefile->AddLayer(particleLayer);

            for(int i = 0; i < m_NParticle; i++)
            {
                 Feature * particle_f = new Feature();
                 LSMPoint * point = new LSMPoint();

                 point->SetPos(m_DataPos->m_Data[i*2+0],m_DataPos->m_Data[i*2+1]);

                 particle_f->AddShape(point);
                 int index_f = particleLayer->AddFeature(particle_f);
                 particleLayer->SetFeatureAttribute(index_f,"POS_X",DEM->west() + DEM->cellSizeX() * ((m_DataPos->m_Data[i*2+0])/_dx));
                 particleLayer->SetFeatureAttribute(index_f,"POS_Y",DEM->north() + DEM->cellSizeY() * ((m_DataPos->m_Data[i*2+1])/_dx));
                 particleLayer->SetFeatureAttribute(index_f,"VEL_X",m_DataVel->m_Data[i*2+0]);
                 particleLayer->SetFeatureAttribute(index_f,"VEL_Y",m_DataVel->m_Data[i*2+1]);
                 particleLayer->SetFeatureAttribute(index_f,"VEL_NORM",std::sqrt(m_DataVel->m_Data[i*2+0]*m_DataVel->m_Data[i*2+0] + m_DataVel->m_Data[i*2+1]*m_DataVel->m_Data[i*2+1]));
                 particleLayer->SetFeatureAttribute(index_f,"STRESS_I1",m_DataStressTensor->m_Data[i*4+0] + m_DataStressTensor->m_Data[i*4+3]);
                 particleLayer->SetFeatureAttribute(index_f,"STRESS_J2",0.5*(m_DataStressTensor->m_Data[i*4+0]*m_DataStressTensor->m_Data[i*4+0] + m_DataStressTensor->m_Data[i*4+1]*m_DataStressTensor->m_Data[i*4+2] +m_DataStressTensor->m_Data[i*4+2]*m_DataStressTensor->m_Data[i*4+1]+ m_DataStressTensor->m_Data[i*4+3]*m_DataStressTensor->m_Data[i*4+3]));
                 particleLayer->SetFeatureAttribute(index_f,"STRAIN_VOL",(std::abs(m_DataDispTensor->m_Data[i*4+0]) + std::abs(m_DataDispTensor->m_Data[i*4+1])+std::abs(m_DataDispTensor->m_Data[i*4+2])+std::abs(m_DataDispTensor->m_Data[i*4+3])));
                 particleLayer->SetFeatureAttribute(index_f,"FRAC_COH",0.5*(m_DataProps->m_Data[i*4+3]));

            }

            SaveVector(particlefile,m_Dir_Res+"particle" + QString::number(m_Step) + ".shp");

            particlefile->Destroy();
            delete particlefile;
        }
    }
}


void LISEMModel::ReportOutputToInterface(float t, float dt)
{


    if(m_SetOutputArrays == false)
    {
        m_InterfaceData.rain_total = 0;
        m_InterfaceData.infil_total = 0;

        m_InterfaceData.outflow_total = 0.0;
        m_InterfaceData.outflows_total = 0.0;


    }


    float area = 0;
    float rain = 0;
    float infil = 0;
    float flow = 0;
    float surfstor = 0;
    float canstor = 0;
    float chflow = 0;
    float outflow = 0;
    float failure = 0;
    float failures = 0;
    float sflow = 0;
    float outflows = 0;
    float gwout = 0.0;

    float dx2 = _dx * _dx;
    float n = 0;


    FOR_ROW_COL_MV(DEM)
    {
        n = n+1;
        area = area + dx2;
        rain += RAIN->Drc;
        flow += HF->Drc;

        outflow += QFOUT->Drc;
        if(m_DoHydrology || m_DoInfiltration)
        {

            infil += INFIL->Drc;
            INFILTOTAL->Drc +=INFIL->Drc;
        }
        if(m_DoHydrology)
        {

            surfstor += SURFACESTORAGE->Drc;
            canstor += CANOPYSTORAGE->Drc;
        }
        if(m_DoChannel)
        {
            if(!pcr::isMV(CH_M->Drc))
            {
                float flowch =  CHHF->Drc * CH_WIDTH->Drc * _dx;
                if(!std::isnan(flowch))
                {
                    chflow += flowch;
                }
            }
        }
        if(m_DoHydrology && m_DoSlopeStability && m_DoSlopeFailure)
        {
            failure += 0.5 * SLOPEFAILURETOTAL->Drc;
            failures += 0.5 * SLOPEFAILURETOTAL->Drc;
        }
        if(m_DoHydrology && m_DoGroundWater)
        {
            gwout += GWOUT->Drc;
        }
        if(m_DoInitialSolids || (m_DoHydrology && m_DoSlopeStability && m_DoSlopeFailure))
        {

            outflows += QSOUT->Drc;

            sflow += HS->Drc;
        }
    }

    //now set all the values in the MODELTOINTERFACE structure

    m_InterfaceData.area = area;
    m_InterfaceData.rain_total += 1000.0 * rain/n;
    m_InterfaceData.infil_total += 1000.0 * infil/n;
    m_InterfaceData.wh_total = 1000.0 * flow/n;
    m_InterfaceData.surfstor_total = 1000.0 * surfstor/area;
    m_InterfaceData.canstor_total = 1000.0 * canstor/area;
    m_InterfaceData.chwh_total = 1000.0 * chflow/area;
    m_InterfaceData.outflow_total += 1000.0 * outflow/area;
    m_InterfaceData.failure_total = 1000.0 * failure;
    m_InterfaceData.failures_total = 1000.0 * failures;
    m_InterfaceData.sh_total = 1000.0 * sflow/n;
    m_InterfaceData.outflows_total += 1000.0 * outflows/area;




    if(m_SetOutputArrays == false)
    {
        m_InterfaceData.rain_total = 0;

        m_InterfaceData.outflow_total = 0.0;
        m_InterfaceData.outflows_total = 0.0;

        m_InterfaceData.outlets.clear();
        m_InterfaceData.H.clear();
        m_InterfaceData.Q.clear();
        m_InterfaceData.V.clear();
        m_InterfaceData.rain.clear();


        m_InterfaceData.rain_total = 0;

        m_InterfaceData.outflow_total = 0.0;
        m_InterfaceData.outflows_total = 0.0;

        int outletn = 1;
        Outlets.clear();
        OutletsR.clear();
        OutletsC.clear();

        Outlets.append(0);
        OutletsR.append(-1);
        OutletsC.append(-1);


        FOR_ROW_COL_MV(DEM)
        {
            if(m_DoChannel)
            {
                if(CH_M->Drc == 5)
                {
                    Outlets.append(outletn);
                    OutletsR.append(r);
                    OutletsC.append(c);
                    outletn ++;
                }
            }

        }

        for(int i = 0; i < Outlets.length(); i++)
        {
            m_InterfaceData.outlets.append(Outlets.at(i));
            m_InterfaceData.H.append(new QList<float>());
            m_InterfaceData.Q.append(new QList<float>());
            m_InterfaceData.V.append(new QList<float>());

        }

    }

    for(int i = 0; i < Outlets.length(); i++)
    {

        int outletn = Outlets.at(i);
        if(outletn > 0)
        {
            int r = OutletsR.at(i);
            int c = OutletsC.at(i);

            m_InterfaceData.H.at(i)->append(CHHF->Drc + HF->Drc);
            m_InterfaceData.Q.at(i)->append((std::fabs(VELF->Drc) * HF->Drc + std::fabs(CHVFX->Drc) * CHHF->Drc) /std::max(0.01f,HF->Drc + CHHF->Drc));
            m_InterfaceData.V.at(i)->append(CHHF->Drc * CHVFX->Drc + HF->Drc * std::fabs(VELF->Drc));

        }else {
            m_InterfaceData.H.at(i)->append(0);
            m_InterfaceData.V.at(i)->append(std::fabs(0));
            m_InterfaceData.Q.at(i)->append(0);
        }
    }
    m_InterfaceData.rain.append(rain);


    //report total file



    //report hydrograph file
    QString hyname = m_Dir_Res + "hydro.csv";
    QString pnr;
    bool solids = m_DoInitialSolids || (m_DoHydrology && m_DoSlopeStability && m_DoSlopeFailure);
    if(m_SetOutputArrays == false)
    {
        QFile fout(hyname);
        if(fout.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&fout);
            out.setRealNumberPrecision(4);
            out.setRealNumberNotation(QTextStream::FixedNotation);

            out << "#Total flow and sed output file for all reporting points in map\n";
            out << "Time";
            out << ",P";
            for(int p = 0; p < Outlets.length(); p++)
            {
                pnr.setNum((int)Outlets.at(p));
                out << ",Q #" << pnr;
                out << ",chanWH #" << pnr;
                if (solids)
                {
                    out << ",Qs #" << pnr;
                }
            }
            out << "\n";
            out << "min";
            out << ",mm/h";
            for(int p = 0; p < Outlets.length(); p++)
                {
                pnr.setNum((int)Outlets.at(p));
                out << ",m3/s #" << pnr;
                out << ",m #" << pnr;
                if (solids)
                {
                    out << ",kg/s #" << pnr;
                }
            }
            out << "\n";
            fout.close();
        }

    }else {
        QFile fout(hyname);
        if(fout.open(QIODevice::Append | QIODevice::Text))
        {

            QTextStream out(&fout);
            out.setRealNumberPrecision(3);
            out.setRealNumberNotation(QTextStream::FixedNotation);
            out << t/60;
            int point = 0;
            for(int p = 0; p <  Outlets.length(); p++)
            {

                out << "," << rain;
                out << "," << m_InterfaceData.Q.at(p)->at(m_InterfaceData.Q.at(p)->length()-1) << "," << m_InterfaceData.H.at(p)->at(m_InterfaceData.Q.at(p)->length()-1);
                if (solids)
                {
                    out << "," << 0;
                }
            }
            out << "\n";
            fout.close();
        }

    }

    QString totname = m_Dir_Res + "tot.txt";
    QFile fp(totname);
    if (fp.open(QIODevice::WriteOnly | QIODevice::Text))
    {

        QTextStream out(&fp);
        out.setRealNumberPrecision(5);
        out.setFieldWidth(12);
        out.setRealNumberNotation(QTextStream::FixedNotation);
        out << "LISEM results at time (min):," << t/60.0 <<"\"\n";
        out << "Catchment area              (ha): " << m_InterfaceData.area/10000.0<< "\n";
        out << "Total rainfall              (mm): " << m_InterfaceData.rain_total << "\n";
        out << "Total surface flow          (mm): " << m_InterfaceData.wh_total << "\n";
        out << "Total infiltration          (mm): " << m_InterfaceData.infil_total << "\n";
        out << "Total canopy storage        (mm): " << m_InterfaceData.canstor_total << "\n";
        out << "Total surface storage       (mm): " << m_InterfaceData.surfstor_total << "\n";
        out << "Total channel flow          (mm): " << m_InterfaceData.chwh_total << "\n";
        out << "Total fluid outflow         (mm): " << m_InterfaceData.outflow_total << "\n";
        out << "Total failures (fluids)     (mm): " << m_InterfaceData.failure_total << "\n";
        out << "Total failures (solids)     (mm): " << m_InterfaceData.failures_total << "\n";
        out << "Total surface solid flow    (mm): " << m_InterfaceData.sh_total << "\n";
        out << "Total solid outflow         (mm): " << m_InterfaceData.outflows_total << "\n";

        fp.flush();
        fp.close();

    }




    if(m_SetOutputArrays == false)
    {
        m_SetOutputArrays = true;
    }



}



void LISEMModel::CopyToUI()
{

    //copy maps to an extra image which can be used for the opengl ui thread
    //the elevation model, H, V are basic components required for the drawing at any time
    //the UI map filled in the kernel based on user input.

    if(!m_MemorySave)
    {
        m_OpenGLCLManager->CopyTextureUsingShaders(T_DEM,TUI_DEM);
        m_OpenGLCLManager->CopyTextureUsingShaders(T_HALL,TUI_HALL);
    }
    m_OpenGLCLManager->CopyTextureUsingShaders(T_H,TUI_H);
    if(!m_MemorySave)
    {
        m_OpenGLCLManager->CopyTextureUsingShaders(T_VX,TUI_VX);
        m_OpenGLCLManager->CopyTextureUsingShaders(T_VY,TUI_VY);
        m_OpenGLCLManager->CopyTextureUsingShaders(T_VX,TUI_UX);
        m_OpenGLCLManager->CopyTextureUsingShaders(T_VY,TUI_UY);
        m_OpenGLCLManager->CopyTextureUsingShaders(T_UI,TUI_UI);
    }
    if(m_DoSlopeFailure || m_DoInitialSolids)
    {
        if(!m_MemorySave)
        {
            m_OpenGLCLManager->CopyTextureUsingShaders(T_HS,TUI_HS);
        }
    }

}

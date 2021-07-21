#include "model.h"

#include <QTime>
#include "error.h"

#include "modelcpucode.h"

#define SWAPMAPPTR(x,y) {ModelTexture * temp = x; x = y;y = temp;}

void LISEMModel::DoModelRun()
{

    LISEM_DEBUG("Do Model Run on thread");

    m_OpenGLCLManager->m_GLOutputUIMutex.lock();
    m_OpenGLCLManager->m_GLMutex.lock();
    glfwMakeContextCurrent(m_OpenGLCLManager->window);
    m_Initializing  = true;

    ModelRunCompileCode();

    ModelRunLoadData();

    ModelRunInitialize();

    m_Initializing  = false;
    //release context since we are done with it for now
    glfwMakeContextCurrent(NULL);
    m_OpenGLCLManager->m_GLMutex.unlock();
    m_OpenGLCLManager->m_GLOutputUIMutex.unlock();


    m_Loaded = true;

    double par_t = GetOptionDouble("Start Time");
    double par_tend = GetOptionDouble("End Time");
    double par_dt = GetOptionDouble("Timestep");
    double par_dt_flow = std::min(par_dt,GetOptionDouble("Timestep Flow"));
    double par_dt_flow_part = std::min(par_dt,GetOptionDouble("Timestep Particle Flow"));
    LISEM_DEBUG(QString("Do Model Run on thread %1   %2   %3   %4").arg(par_t).arg(par_tend).arg(par_dt).arg(par_dt_flow));

    float t = par_t * 60.0;
    float t_end = par_tend * 60.0;
    float dt = m_DoRigidWorld? std::min(0.02,par_dt):par_dt;
    float dt_flow = std::min((double)dt,par_dt_flow);
    float dt_flow_part = std::min((double)dt_flow,par_dt_flow_part);
    float dt_flow_last = dt_flow;
    int steps = std::floor((t_end-t)/(dt)) + 1;
    m_Step = 0;
    int uimap = 0;
    bool first = true;
    bool ParticleFirst = true;

    LISEM_STATUS("Start main program loop");

    while(t < t_end)
    {
        m_Time = t;
        m_DT = dt;

        if(first)
        {
            LISEM_DEBUG("start running kernels")
        }

        LISEM_DEBUG(QString("Timestep ") + QString::number(m_Step));



        //check if there is the request to stop or pause the model
        m_ModelStateMutex.lock();



        if(m_PauseRequested)
        {
            m_PauseRequested = false;
            m_Paused = true;
            PauseMutex.lock();
            m_Paused = true;
            m_ModelStateMutex.unlock();
            if(m_CallBack_ModelPaused_Set)
            {
               m_CallBack_ModelPaused();
            }
            PauseCondition.wait(&PauseMutex);
            m_ModelStateMutex.lock();
            m_Paused = false;
            PauseMutex.unlock();


        }


        if(m_StopRequested)
        {
            m_PauseRequested = false;
            m_StopRequested = false;
            m_ModelStateMutex.unlock();
            break;
        }

        m_ModelStateMutex.unlock();

        if(m_Loaded)
        {

               std::cout << "get rain values " << std::endl;
            //get time-input stuff and upload maps
                m_Rain.fillValues(t,dt, RAIN);

                FOR_ROW_COL_MV(DEM)
                {
                    RAIN->Drc=  RAIN->Drc* dt/(3600.0 *1000.0);
                }

                std::cout << "done with rain " << std::endl;

                if(m_DoEvapoTranspiration)
                {
                    m_Temp.fillValues(t,dt,TEMP);
                    m_Rad.fillValues(t,dt, RAD);
                    m_Wind.fillValues(t,dt, WIND);
                    m_Vapr.fillValues(t,dt, VAPR);
                    m_NDVI.fillValues(t,dt, NDVI);

                }
                m_OpenGLCLManager->m_GLOutputUIMutex.lock();
                m_OpenGLCLManager->m_GLMutex.lock();

                glfwMakeContextCurrent(m_OpenGLCLManager->window);

                m_OpenGLCLManager->CopyMapToTexture(T_RAIN,&(RAIN->data));

                if(m_DoEvapoTranspiration)
                {
                    m_OpenGLCLManager->CopyMapToTexture(T_TEMP,&(TEMP->data));
                    m_OpenGLCLManager->CopyMapToTexture(T_WIND,&(WIND->data));
                    m_OpenGLCLManager->CopyMapToTexture(T_VAPR,&(VAPR->data));
                    m_OpenGLCLManager->CopyMapToTexture(T_RAD,&(RAD->data));
                    m_OpenGLCLManager->CopyMapToTexture(T_NDVI,&(NDVI->data));
                }

                glfwMakeContextCurrent(NULL);
                m_OpenGLCLManager->m_GLMutex.unlock();
                m_OpenGLCLManager->m_GLOutputUIMutex.unlock();

                std::cout << "done with upload " << std::endl;



                if(m_DoInfiltration && !m_DoHydrology)
                {
                    m_CLProgram_Infiltration->PlaceArgument(3, (float)(dt));
                    m_CLProgram_Infiltration->PlaceArgument(16, uimap);
                    m_CLProgram_Infiltration->PlaceArgument(17, 0.0);

                    m_CLProgram_Infiltration->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);
                    m_CLProgram_Infiltration2->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);
                }

                if(m_DoHydrology)
                {
                    m_CLProgram_Hydrology->PlaceArgument(3, (float)(dt));
                    m_CLProgram_Hydrology->PlaceArgument(37, uimap);
                    m_CLProgram_Hydrology->PlaceArgument(38, 0.0);

                    //do hydrology
                    m_CLProgram_Hydrology->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);
                    m_CLProgram_Hydrology2->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);
                }

                if(m_DoHydrology && m_DoGroundWater)
                {
                    m_CLProgram_GroundWaterFlow->PlaceArgument(3, (float)(dt));
                    m_CLProgram_GroundWaterFlow2->PlaceArgument(3, (float)(dt));
                    m_CLProgram_GroundWaterFlow->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);
                    m_CLProgram_GroundWaterFlow2->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);


                }

                if(m_DoEvapoTranspiration)
                {
                    //run evapo kernel
                    m_CLProgram_Evapotranspiration->PlaceArgument(3, (float)(dt));
                    m_CLProgram_Evapotranspiration->PlaceArgument(37, uimap);
                    m_CLProgram_Evapotranspiration->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);
                    m_CLProgram_Evapotranspiration2->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);

                }


                //do flow

                float t_flow = 0;
                float dt_flow_act;
                float dt_flow_act_new;
                if(first)
                {
                    if(m_DoInitialSolids || m_DoInitialWater)
                    {
                        dt_flow_act = dt_flow;
                    }else
                    {
                        dt_flow_act = dt_flow;
                    }
                }else {
                    dt_flow_act = dt_flow_last;
                }


                bool breakall = false;

                while(t_flow < dt)
                {
                    dt_flow_act = std::min( dt - t_flow,std::max(dt_flow_act,dt_flow));
                    if(m_DoParticleSolids)
                    {
                        dt_flow_act = dt_flow_part;
                    }

                    //LISEM_DEBUG(QString("timestep flow ") + QString::number(t_flow) + " " + QString::number(dt) + " " + QString::number( dt_flow_act));


                    t_flow += dt_flow_act;

                    if(m_DoRigidWorld)
                    {
                        m_RigidWorld->SetInteractTwoPhaseFlow(HF,VFX,VFY,nullptr,nullptr,nullptr,nullptr);
                        m_RigidWorld->RunSingleStep(dt_flow_act,t + t_flow);

                        m_OpenGLCLManager->m_GLOutputUIMutex.lock();
                        m_OpenGLCLManager->m_GLMutex.lock();


                        glfwMakeContextCurrent(m_OpenGLCLManager->window);

                        m_OpenGLCLManager->m_GLMutex.unlock();
                        m_OpenGLCLManager->CopyMapToTexture(T_BlockingFX,&(m_RigidWorld->m_BlockFX->data));
                        m_OpenGLCLManager->CopyMapToTexture(T_BlockingFY,&(m_RigidWorld->m_BlockFY->data));
                        m_OpenGLCLManager->CopyMapToTexture(T_BlockingX,&(m_RigidWorld->m_BlockX->data));
                        m_OpenGLCLManager->CopyMapToTexture(T_BlockingY,&(m_RigidWorld->m_BlockY->data));
                        m_OpenGLCLManager->CopyMapToTexture(T_BlockingVelX,&(m_RigidWorld->m_BlockCaptureX->data));
                        m_OpenGLCLManager->CopyMapToTexture(T_BlockingVelY,&(m_RigidWorld->m_BlockCaptureY->data));

                        glfwMakeContextCurrent(NULL);
                        m_OpenGLCLManager->m_GLOutputUIMutex.unlock();

                        m_CLProgram_Rigid->PlaceArgument(3, (float)(dt_flow_act));
                        m_CLProgram_Rigid->PlaceArgument(11, uimap);
                        m_CLProgram_Rigid2->PlaceArgument(3, (float)(dt_flow_act));
                        m_CLProgram_Rigid2->PlaceArgument(11, uimap);

                        m_CLProgram_Rigid->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);

                        m_CLProgram_Rigid2->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);
                    }


                    if(!first)
                    {
                        if(m_DoParticleSolids && m_NParticle > 0)
                        {


                            float t_flow_part = 0;
                            int step_flow_part = 0;
                            bool step_flow_first = true;

                            while(t_flow_part < dt_flow_act)
                            {

                                //check if there is the request to stop or pause the model
                                m_ModelStateMutex.lock();



                                if(m_PauseRequested)
                                {
                                    m_PauseRequested = false;
                                    m_Paused = true;
                                    PauseMutex.lock();
                                    m_Paused = true;
                                    m_ModelStateMutex.unlock();
                                    if(m_CallBack_ModelPaused_Set)
                                    {
                                       m_CallBack_ModelPaused();
                                    }
                                    PauseCondition.wait(&PauseMutex);
                                    m_ModelStateMutex.lock();
                                    m_Paused = false;
                                    PauseMutex.unlock();


                                }


                                if(m_StopRequested)
                                {
                                    m_PauseRequested = false;
                                    m_StopRequested = false;
                                    m_ModelStateMutex.unlock();
                                    breakall = true;
                                    break;
                                }

                                m_ModelStateMutex.unlock();


                                    float dt_flow_part_act = std::max(0.00000001f,std::min(dt_flow_act-t_flow_part,dt_flow_part));

                                    //sort particles for easy acces later

                                    //LISEM_DEBUG(QString("timesteps ") + QString::number(t_flow_part) + " " + QString::number( dt_flow_part_act) + " " + QString::number( dt_flow_act));
                                    //LISEM_DEBUG(QString("nr of particles %1 %2").arg(m_NParticlePadded).arg(m_NParticle));

                                    {
                                        unsigned int stage, passOfStage, numStages, temp;
                                        stage = passOfStage = numStages = 0;

                                        size_t padded_size = m_NParticlePadded;

                                        for(temp = padded_size; temp > 1; temp >>= 1)
                                        {
                                            ++numStages;
                                        }

                                        m_CLProgram_Sort->SetRunDims(padded_size>> 1 );

                                        for(stage = 0; stage < numStages; ++stage)
                                        {

                                            // Every stage has stage + 1 passes
                                            for(passOfStage = 0; passOfStage < stage + 1;
                                                ++passOfStage)
                                            {
                                                    m_CLProgram_Sort->PlaceArgument(0,m_DataIndex);
                                                    m_CLProgram_Sort->PlaceArgument(1,m_DataIndexC);
                                                    m_CLProgram_Sort->PlaceArgument(2,(int)stage);
                                                    m_CLProgram_Sort->PlaceArgument(3,(int)passOfStage);
                                                    m_CLProgram_Sort->PlaceArgument(4,(int)padded_size);
                                                    m_CLProgram_Sort->PlaceArgument(5,(int)m_NParticle);

                                                    m_CLProgram_Sort->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);

                                            } //end of for passStage = 0:stage-1
                                        } //end of for stage = 0:numStage-1

                                        //get particle indices for each raster pixel

                                        m_CLProgram_StoreIndices->SetRunDims(m_NParticlePadded);
                                        m_CLProgram_StoreIndices->PlaceArgument(3,(int)m_NParticle);
                                        m_CLProgram_StoreIndices->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);
                                    }


                                    if(ParticleFirst){
                                        unsigned int stage, passOfStage, numStages, temp;
                                        stage = passOfStage = numStages = 0;

                                        size_t padded_size = m_NParticlePadded;

                                        for(temp = padded_size; temp > 1; temp >>= 1)
                                        {
                                            ++numStages;
                                        }


                                        m_CLProgram_Sort->SetRunDims(padded_size>> 1 );

                                        for(stage = 0; stage < numStages; ++stage)
                                        {

                                            // Every stage has stage + 1 passes
                                            for(passOfStage = 0; passOfStage < stage + 1;
                                                ++passOfStage)
                                            {
                                                    m_CLProgram_Sort->PlaceArgument(0,m_DataIndex);
                                                    m_CLProgram_Sort->PlaceArgument(1,m_DataIndexC);
                                                    m_CLProgram_Sort->PlaceArgument(2,(int)stage);
                                                    m_CLProgram_Sort->PlaceArgument(3,(int)passOfStage);
                                                    m_CLProgram_Sort->PlaceArgument(4,(int)padded_size);
                                                    m_CLProgram_Sort->PlaceArgument(5,(int)m_NParticle);

                                                    m_CLProgram_Sort->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);

                                            } //end of for passStage = 0:stage-1
                                        } //end of for stage = 0:numStage-1

                                        //get particle indices for each raster pixel

                                        m_CLProgram_StoreIndices->SetRunDims(m_NParticlePadded);
                                        m_CLProgram_StoreIndices->PlaceArgument(3,(int)m_NParticle);
                                        m_CLProgram_StoreIndices->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);
                                    }





                                    m_CLProgram_Particles->PlaceArgument(3, (float)(dt_flow_part_act));

                                    m_CLProgram_Particles->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);

                                    {
                                        unsigned int stage, passOfStage, numStages, temp;
                                        stage = passOfStage = numStages = 0;

                                        size_t padded_size = m_NParticlePadded;

                                        for(temp = padded_size; temp > 1; temp >>= 1)
                                        {
                                            ++numStages;
                                        }


                                        m_CLProgram_Sort->SetRunDims(padded_size>> 1 );

                                        for(stage = 0; stage < numStages; ++stage)
                                        {

                                            // Every stage has stage + 1 passes
                                            for(passOfStage = 0; passOfStage < stage + 1;
                                                ++passOfStage)
                                            {
                                                    m_CLProgram_Sort->PlaceArgument(0,m_DataIndex);
                                                    m_CLProgram_Sort->PlaceArgument(1,m_DataIndexC);
                                                    m_CLProgram_Sort->PlaceArgument(2,(int)stage);
                                                    m_CLProgram_Sort->PlaceArgument(3,(int)passOfStage);
                                                    m_CLProgram_Sort->PlaceArgument(4,(int)padded_size);
                                                    m_CLProgram_Sort->PlaceArgument(5,(int)m_NParticle);

                                                    m_CLProgram_Sort->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);

                                            } //end of for passStage = 0:stage-1
                                        } //end of for stage = 0:numStage-1

                                        //get particle indices for each raster pixel

                                        m_CLProgram_StoreIndices->SetRunDims(m_NParticlePadded);
                                        m_CLProgram_StoreIndices->PlaceArgument(3,(int)m_NParticle);
                                        m_CLProgram_StoreIndices->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);
                                    }



                                    m_CLProgram_Particles2->PlaceArgument(13, (float)(m_NParticle));
                                    m_CLProgram_Particles2->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);



                                ParticleFirst = false;

                                t_flow_part += dt_flow_part_act;
                                step_flow_part += 1;

                                step_flow_first = false;
                            }//end while(t_flow_part < dt_flow)


                            if(breakall)
                            {
                                break;
                            }
                            m_CLProgram_ParticlesToMaps->PlaceArgument(3, (float)(dt_flow_part));
                            m_CLProgram_ParticlesToMaps->PlaceArgument(12, (int)m_NParticle);
                            m_CLProgram_ParticlesToMaps->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);


                        }//end if(m_DoParticleSolids && m_NParticle > 0)

                    }//end if(!first)


                    if((m_DoSlopeFailure || m_DoInitialSolids))
                    {
                        if(!m_DoParticleSolids)
                        {

                            m_CLProgram_FlowSolids->PlaceArgument(47, (float)(dt));
                            m_CLProgram_FlowSolids->PlaceArgument(3, (float)(dt_flow_act));
                            if(!m_DoInfiltration && !m_DoHydrology)
                            {
                                 //m_CLProgram_FlowSolids->PlaceArgument(53, rain* dt_flow_act/dt);
                            }else
                            {
                                //m_CLProgram_FlowSolids->PlaceArgument(53, 0.0f);
                            }

                            m_CLProgram_FlowSolids->PlaceArgument(46, uimap);
                            m_CLProgram_FlowSolids->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);

                            if(((m_DoHydrology && m_DoErosion) || (((m_DoHydrology && m_DoSlopeFailure) || m_DoInitialSolids) && m_DoEntrainment)))
                            {

                                SWAPMAPPTR(T_H,T_HN);
                                SWAPMAPPTR(T_HS,T_HSN);
                                SWAPMAPPTR(T_VSX,T_VSXN);
                                SWAPMAPPTR(T_VSY,T_VSYN);
                                SWAPMAPPTR(T_VX,T_VXN);
                                SWAPMAPPTR(T_VY,T_VYN);
                                SWAPMAPPTR(T_SIfa,T_SIfaN);
                                SWAPMAPPTR(T_SRockSize,T_SRockSizeN);
                                SWAPMAPPTR(T_SDensity,T_SDensityN);

                                m_CLProgram_Erosion2->PlaceArgument(74, T_VXN);
                                m_CLProgram_Erosion2->PlaceArgument(75, T_VYN);
                                m_CLProgram_Erosion->PlaceArgument(87, T_VXN);
                                m_CLProgram_Erosion->PlaceArgument(88, T_VYN);
                                m_CLProgram_Erosion2->PlaceArgument(62, T_HSN);
                                m_CLProgram_Erosion2->PlaceArgument(63, T_VSXN);
                                m_CLProgram_Erosion2->PlaceArgument(64, T_VSYN);
                                m_CLProgram_Erosion2->PlaceArgument(65, T_SIfaN);
                                m_CLProgram_Erosion2->PlaceArgument(66, T_SRockSizeN);
                                m_CLProgram_Erosion2->PlaceArgument(67, T_SDensityN);
                                m_CLProgram_Erosion2->PlaceArgument(7, T_H);
                                m_CLProgram_Erosion2->PlaceArgument(8, T_VX);
                                m_CLProgram_Erosion2->PlaceArgument(9, T_VY);
                                m_CLProgram_Erosion2->PlaceArgument(10, T_HS);
                                m_CLProgram_Erosion2->PlaceArgument(11, T_VSX);
                                m_CLProgram_Erosion2->PlaceArgument(12, T_VSY);
                                m_CLProgram_Erosion2->PlaceArgument(13, T_SIfa);
                                m_CLProgram_Erosion2->PlaceArgument(14, T_SRockSize);
                                m_CLProgram_Erosion2->PlaceArgument(15, T_SDensity);
                                m_CLProgram_Erosion2->PlaceArgument(73, T_HN);
                                m_CLProgram_Erosion->PlaceArgument(86, T_HN);
                                m_CLProgram_Erosion->PlaceArgument(7, T_H);
                                m_CLProgram_Erosion->PlaceArgument(8, T_VX);
                                m_CLProgram_Erosion->PlaceArgument(9, T_VY);
                                m_CLProgram_Erosion->PlaceArgument(10, T_HS);
                                m_CLProgram_Erosion->PlaceArgument(11, T_VSX);
                                m_CLProgram_Erosion->PlaceArgument(12, T_VSY);
                                m_CLProgram_Erosion->PlaceArgument(13, T_SIfa);
                                m_CLProgram_Erosion->PlaceArgument(14, T_SRockSize);
                                m_CLProgram_Erosion->PlaceArgument(15, T_SDensity);
                                m_CLProgram_Erosion->PlaceArgument(62, T_HSN);
                                m_CLProgram_Erosion->PlaceArgument(63, T_VSXN);
                                m_CLProgram_Erosion->PlaceArgument(64, T_VSYN);
                                m_CLProgram_Erosion->PlaceArgument(65, T_SIfaN);
                                m_CLProgram_Erosion->PlaceArgument(66, T_SRockSizeN);
                                m_CLProgram_Erosion->PlaceArgument(67, T_SDensityN);

                                    m_CLProgram_Erosion->PlaceArgument(3, (float)(dt_flow_act));
                                    m_CLProgram_Erosion->PlaceArgument(60, uimap);
                                    m_CLProgram_Erosion->PlaceArgument(81, 0.0);
                                    m_CLProgram_Erosion2->PlaceArgument(3, (float)(dt_flow_act));

                                    m_CLProgram_Erosion->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);

                                    m_CLProgram_Erosion2->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);
                            }

                            m_CLProgram_FlowSolids2->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);


                        }else
                        {


                            m_CLProgram_FlowSolidsParticle->PlaceArgument(47, (float)(dt));
                            m_CLProgram_FlowSolidsParticle->PlaceArgument(3, (float)(dt_flow_act));

                            if(!m_DoInfiltration && !m_DoHydrology)
                            {
                                 //m_CLProgram_FlowSolidsParticle->PlaceArgument(53, rain * dt_flow_act/dt);
                            }else
                            {
                                //m_CLProgram_FlowSolidsParticle->PlaceArgument(53, 0.0f);
                            }

                            m_CLProgram_FlowSolidsParticle->PlaceArgument(46, uimap);
                            m_CLProgram_FlowSolidsParticle->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);

                            if(((m_DoHydrology && m_DoErosion) || (((m_DoHydrology && m_DoSlopeFailure) || m_DoInitialSolids) && m_DoEntrainment)))
                            {
                                SWAPMAPPTR(T_H,T_HN);
                                SWAPMAPPTR(T_HS,T_HSN);
                                SWAPMAPPTR(T_VSX,T_VSXN);
                                SWAPMAPPTR(T_VSY,T_VSYN);
                                SWAPMAPPTR(T_VX,T_VXN);
                                SWAPMAPPTR(T_VY,T_VYN);
                                SWAPMAPPTR(T_SIfa,T_SIfaN);
                                SWAPMAPPTR(T_SRockSize,T_SRockSizeN);
                                SWAPMAPPTR(T_SDensity,T_SDensityN);

                                m_CLProgram_Erosion2->PlaceArgument(74, T_VXN);
                                m_CLProgram_Erosion2->PlaceArgument(75, T_VYN);
                                m_CLProgram_Erosion->PlaceArgument(87, T_VXN);
                                m_CLProgram_Erosion->PlaceArgument(88, T_VYN);
                                m_CLProgram_Erosion2->PlaceArgument(62, T_HSN);
                                m_CLProgram_Erosion2->PlaceArgument(63, T_VSXN);
                                m_CLProgram_Erosion2->PlaceArgument(64, T_VSYN);
                                m_CLProgram_Erosion2->PlaceArgument(65, T_SIfaN);
                                m_CLProgram_Erosion2->PlaceArgument(66, T_SRockSizeN);
                                m_CLProgram_Erosion2->PlaceArgument(67, T_SDensityN);
                                m_CLProgram_Erosion2->PlaceArgument(7, T_H);
                                m_CLProgram_Erosion2->PlaceArgument(8, T_VX);
                                m_CLProgram_Erosion2->PlaceArgument(9, T_VY);
                                m_CLProgram_Erosion2->PlaceArgument(10, T_HS);
                                m_CLProgram_Erosion2->PlaceArgument(11, T_VSX);
                                m_CLProgram_Erosion2->PlaceArgument(12, T_VSY);
                                m_CLProgram_Erosion2->PlaceArgument(13, T_SIfa);
                                m_CLProgram_Erosion2->PlaceArgument(14, T_SRockSize);
                                m_CLProgram_Erosion2->PlaceArgument(15, T_SDensity);
                                m_CLProgram_Erosion2->PlaceArgument(73, T_HN);
                                m_CLProgram_Erosion->PlaceArgument(86, T_HN);
                                m_CLProgram_Erosion->PlaceArgument(7, T_H);
                                m_CLProgram_Erosion->PlaceArgument(8, T_VX);
                                m_CLProgram_Erosion->PlaceArgument(9, T_VY);
                                m_CLProgram_Erosion->PlaceArgument(10, T_HS);
                                m_CLProgram_Erosion->PlaceArgument(11, T_VSX);
                                m_CLProgram_Erosion->PlaceArgument(12, T_VSY);
                                m_CLProgram_Erosion->PlaceArgument(13, T_SIfa);
                                m_CLProgram_Erosion->PlaceArgument(14, T_SRockSize);
                                m_CLProgram_Erosion->PlaceArgument(15, T_SDensity);
                                m_CLProgram_Erosion->PlaceArgument(62, T_HSN);
                                m_CLProgram_Erosion->PlaceArgument(63, T_VSXN);
                                m_CLProgram_Erosion->PlaceArgument(64, T_VSYN);
                                m_CLProgram_Erosion->PlaceArgument(65, T_SIfaN);
                                m_CLProgram_Erosion->PlaceArgument(66, T_SRockSizeN);
                                m_CLProgram_Erosion->PlaceArgument(67, T_SDensityN);

                                    m_CLProgram_Erosion->PlaceArgument(3, (float)(dt_flow_act));
                                    m_CLProgram_Erosion2->PlaceArgument(3, (float)(dt_flow_act));
                                    m_CLProgram_Erosion->PlaceArgument(60, uimap);
                                    m_CLProgram_Erosion->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);
                                    m_CLProgram_Erosion2->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);
                            }


                            m_CLProgram_FlowSolidsParticle2->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);


                            m_CLProgram_ParticlesFromMaps->SetRunDims(m_NParticle);
                            if(m_NParticle > 0)
                            {
                                m_CLProgram_ParticlesFromMaps->PlaceArgument(3, (float)(0.5));
                                m_CLProgram_ParticlesFromMaps->PlaceArgument(10, (int)m_NParticle);
                                m_CLProgram_ParticlesFromMaps->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);
                            }

                        }
                    }else
                    {

                        m_CLProgram_Flow->PlaceArgument(25, (float)(dt));
                        m_CLProgram_Flow->PlaceArgument(3, (float)(dt_flow_act));
                        m_CLProgram_Flow->PlaceArgument(24, uimap);
                        if(!m_DoInfiltration && !m_DoHydrology)
                        {
                             //m_CLProgram_Flow->PlaceArgument(36, rain* dt_flow_act/dt);
                        }else
                        {
                            //m_CLProgram_Flow->PlaceArgument(36, 0.0f);
                        }
                        m_CLProgram_Flow->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);

                        if(((m_DoHydrology && m_DoErosion) || (((m_DoHydrology && m_DoSlopeFailure) || m_DoInitialSolids) && m_DoEntrainment)))
                        {
                            SWAPMAPPTR(T_H,T_HN);
                            SWAPMAPPTR(T_HS,T_HSN);
                            SWAPMAPPTR(T_VSX,T_VSXN);
                            SWAPMAPPTR(T_VSY,T_VSYN);
                            SWAPMAPPTR(T_VX,T_VXN);
                            SWAPMAPPTR(T_VY,T_VYN);
                            SWAPMAPPTR(T_SIfa,T_SIfaN);
                            SWAPMAPPTR(T_SRockSize,T_SRockSizeN);
                            SWAPMAPPTR(T_SDensity,T_SDensityN);

                            m_CLProgram_Erosion2->PlaceArgument(74, T_VXN);
                            m_CLProgram_Erosion2->PlaceArgument(75, T_VYN);
                            m_CLProgram_Erosion->PlaceArgument(87, T_VXN);
                            m_CLProgram_Erosion->PlaceArgument(88, T_VYN);
                            m_CLProgram_Erosion2->PlaceArgument(62, T_HSN);
                            m_CLProgram_Erosion2->PlaceArgument(63, T_VSXN);
                            m_CLProgram_Erosion2->PlaceArgument(64, T_VSYN);
                            m_CLProgram_Erosion2->PlaceArgument(65, T_SIfaN);
                            m_CLProgram_Erosion2->PlaceArgument(66, T_SRockSizeN);
                            m_CLProgram_Erosion2->PlaceArgument(67, T_SDensityN);
                            m_CLProgram_Erosion2->PlaceArgument(7, T_H);
                            m_CLProgram_Erosion2->PlaceArgument(8, T_VX);
                            m_CLProgram_Erosion2->PlaceArgument(9, T_VY);
                            m_CLProgram_Erosion2->PlaceArgument(10, T_HS);
                            m_CLProgram_Erosion2->PlaceArgument(11, T_VSX);
                            m_CLProgram_Erosion2->PlaceArgument(12, T_VSY);
                            m_CLProgram_Erosion2->PlaceArgument(13, T_SIfa);
                            m_CLProgram_Erosion2->PlaceArgument(14, T_SRockSize);
                            m_CLProgram_Erosion2->PlaceArgument(15, T_SDensity);
                            m_CLProgram_Erosion2->PlaceArgument(73, T_HN);
                            m_CLProgram_Erosion->PlaceArgument(86, T_HN);
                            m_CLProgram_Erosion->PlaceArgument(7, T_H);
                            m_CLProgram_Erosion->PlaceArgument(8, T_VX);
                            m_CLProgram_Erosion->PlaceArgument(9, T_VY);
                            m_CLProgram_Erosion->PlaceArgument(10, T_HS);
                            m_CLProgram_Erosion->PlaceArgument(11, T_VSX);
                            m_CLProgram_Erosion->PlaceArgument(12, T_VSY);
                            m_CLProgram_Erosion->PlaceArgument(13, T_SIfa);
                            m_CLProgram_Erosion->PlaceArgument(14, T_SRockSize);
                            m_CLProgram_Erosion->PlaceArgument(15, T_SDensity);
                            m_CLProgram_Erosion->PlaceArgument(62, T_HSN);
                            m_CLProgram_Erosion->PlaceArgument(63, T_VSXN);
                            m_CLProgram_Erosion->PlaceArgument(64, T_VSYN);
                            m_CLProgram_Erosion->PlaceArgument(65, T_SIfaN);
                            m_CLProgram_Erosion->PlaceArgument(66, T_SRockSizeN);
                            m_CLProgram_Erosion->PlaceArgument(67, T_SDensityN);

                                m_CLProgram_Erosion->PlaceArgument(3, (float)(dt_flow_act));
                                m_CLProgram_Erosion2->PlaceArgument(3, (float)(dt_flow_act));
                                m_CLProgram_Erosion->PlaceArgument(60, uimap);
                                m_CLProgram_Erosion->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);
                                m_CLProgram_Erosion2->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);
                        }


                        m_CLProgram_Flow2->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);

                    }

                    m_OpenGLCLManager->m_GLOutputUIMutex.lock();
                    m_OpenGLCLManager->m_GLMutex.lock();

                    glfwMakeContextCurrent(m_OpenGLCLManager->window);
                    m_OpenGLCLManager->CopyTextureToMap(T_DTREQ,&(DTREQ->data));

                    glfwMakeContextCurrent(NULL);
                    m_OpenGLCLManager->m_GLMutex.unlock();
                    m_OpenGLCLManager->m_GLOutputUIMutex.unlock();

                    //get minimum of dt map
                    float dt_req_min = 1e31;
                    FOR_ROW_COL_MV(DEM)
                    {
                        if(DTREQ->Drc > 0.0)
                        {
                            dt_req_min = std::min(dt_req_min,DTREQ->Drc);
                        }
                    }

                   // dt_flow_act = std::min((dt - t_flow) + 0.001f,std::max(dt_flow,std::min(dt_flow_last,dt_req_min)));
                    dt_flow_act_new =std::max(dt_flow,dt_req_min);
                    dt_flow_act = std::min((dt - t_flow) + 0.0000001f,std::max(dt_flow,dt_req_min));

                    SWAPMAPPTR(T_HFMax,T_HFMaxN);
                    SWAPMAPPTR(T_HSMax,T_HSMaxN);
                    SWAPMAPPTR(T_VFMax,T_VFMaxN);
                    SWAPMAPPTR(T_VSMax,T_VSMaxN);

                    SWAPMAPPTR(T_QFOUT,T_QFOUTN);
                    SWAPMAPPTR(T_QSOUT,T_QSOUTN);

                    m_CLProgram_Flow2->PlaceArgument(19, T_HFMax);
                    m_CLProgram_Flow2->PlaceArgument(20, T_VFMax);
                    m_CLProgram_Flow2->PlaceArgument(21, T_HFMaxN);
                    m_CLProgram_Flow2->PlaceArgument(22, T_VFMaxN);
                    m_CLProgram_FlowSolids2->PlaceArgument(41, T_HFMax);
                    m_CLProgram_FlowSolids2->PlaceArgument(42, T_VFMax);
                    m_CLProgram_FlowSolids2->PlaceArgument(43, T_HFMaxN);
                    m_CLProgram_FlowSolids2->PlaceArgument(44, T_VFMaxN);
                    m_CLProgram_FlowSolids2->PlaceArgument(45, T_HSMax);
                    m_CLProgram_FlowSolids2->PlaceArgument(46, T_VSMax);
                    m_CLProgram_FlowSolids2->PlaceArgument(47, T_HSMaxN);
                    m_CLProgram_FlowSolids2->PlaceArgument(48, T_VSMaxN);
                    if(m_DoParticleSolids)
                    {
                        m_CLProgram_FlowSolidsParticle2->PlaceArgument(41, T_HFMax);
                        m_CLProgram_FlowSolidsParticle2->PlaceArgument(42, T_VFMax);
                        m_CLProgram_FlowSolidsParticle2->PlaceArgument(43, T_HFMaxN);
                        m_CLProgram_FlowSolidsParticle2->PlaceArgument(44, T_VFMaxN);
                        m_CLProgram_FlowSolidsParticle2->PlaceArgument(45, T_HSMax);
                        m_CLProgram_FlowSolidsParticle2->PlaceArgument(46, T_VSMax);
                        m_CLProgram_FlowSolidsParticle2->PlaceArgument(47, T_HSMaxN);
                        m_CLProgram_FlowSolidsParticle2->PlaceArgument(48, T_VSMaxN);
                    }

                    m_CLProgram_Flow->PlaceArgument(26, T_QFOUT);
                    m_CLProgram_Flow->PlaceArgument(27, T_QFOUTN);
                    m_CLProgram_FlowSolids->PlaceArgument(48, T_QFOUT);
                    m_CLProgram_FlowSolids->PlaceArgument(49, T_QFOUTN);
                    m_CLProgram_FlowSolids->PlaceArgument(50, T_QSOUT);
                    m_CLProgram_FlowSolids->PlaceArgument(51, T_QSOUTN);
                    if(m_DoParticleSolids)
                    {
                        m_CLProgram_FlowSolidsParticle->PlaceArgument(48, T_QFOUT);
                        m_CLProgram_FlowSolidsParticle->PlaceArgument(49, T_QFOUTN);
                        m_CLProgram_FlowSolidsParticle->PlaceArgument(50, T_QSOUT);
                        m_CLProgram_FlowSolidsParticle->PlaceArgument(51, T_QSOUTN);

                    }

                    if((dt - t_flow) < 0.000001)
                    {
                        break;
                    }


                }//end while(t_flow < dt)

                if(breakall)
                {
                    break;
                }

            dt_flow_last = dt_flow_act_new;

            if(m_DoHydrology && m_DoSlopeStability && !m_DoSlopeFailure)
            {
                if(m_DoSeismic || m_DoImprovedIterative)
                {
                    m_CLProgram_SlopeStabilityE->PlaceArgument(21, first? 1:0);
                    m_CLProgram_SlopeStabilityE->PlaceArgument(23, uimap);

                    m_CLProgram_SlopeStabilityE->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);

                }else
                {
                    m_CLProgram_SlopeStability->PlaceArgument(18, first? 1:0);
                    m_CLProgram_SlopeStability->PlaceArgument(20, uimap);

                    m_CLProgram_SlopeStability->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);
                }
            }

            if(m_DoHydrology && m_DoSlopeStability && m_DoSlopeFailure)
            {
                double slopefailvol = 0.0;

                for(int i_sf = 0; i_sf < 25; i_sf++)
                {
                    if(m_DoSeismic || m_DoImprovedIterative)
                    {
                        m_CLProgram_SlopeStabilityE->PlaceArgument(21, first? 1:0);
                        m_CLProgram_SlopeStabilityE->PlaceArgument(23, uimap);

                        m_CLProgram_SlopeStabilityE->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);

                    }else
                    {
                        m_CLProgram_SlopeStability->PlaceArgument(18, first? 1:0);
                        m_CLProgram_SlopeStability->PlaceArgument(20, uimap);

                        m_CLProgram_SlopeStability->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);
                    }


                    if(m_DoSeismic || m_DoImprovedIterative)
                    {
                        m_CLProgram_SlopeFailureE->PlaceArgument(37, uimap);
                        m_CLProgram_SlopeFailureE->PlaceArgument(38, m_DoInitialStability?1:0);

                        m_CLProgram_SlopeFailureE->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);
                    }else
                    {
                        m_CLProgram_SlopeFailure->PlaceArgument(34, uimap);
                        m_CLProgram_SlopeFailure->PlaceArgument(35, m_DoInitialStability?1:0);

                        m_CLProgram_SlopeFailure->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);
                    }

                    if(!m_DoParticleSolids)
                    {

                        m_CLProgram_SlopeFailure2->PlaceArgument(27, i_sf == 0?1:0);
                        m_CLProgram_SlopeFailure2->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);

                        //swap so we dont have to run a seperate kernel to do it
                        ModelTexture * temp = T_SFHCUM;
                        T_SFHCUM =  T_SFHCUMN;
                        T_SFHCUMN = temp;

                        m_CLProgram_SlopeFailure2->PlaceArgument(25, T_SFHCUM);
                        m_CLProgram_SlopeFailure2->PlaceArgument(26, T_SFHCUMN);
                    }

                    if(m_DoParticleSolids)
                    {
                        m_CLProgram_SlopeFailureP2->PlaceArgument(27, i_sf == 0?1:0);
                        m_CLProgram_SlopeFailureP2->Run(m_OpenGLCLManager->context,m_OpenGLCLManager->q);

                        //swap so we dont have to run a seperate kernel to do it
                        ModelTexture * temp = T_SFHCUM;
                        T_SFHCUM =  T_SFHCUMN;
                        T_SFHCUMN = temp;

                        m_CLProgram_SlopeFailureP2->PlaceArgument(25, T_SFHCUM);
                        m_CLProgram_SlopeFailureP2->PlaceArgument(26, T_SFHCUMN);
                    }

                    m_OpenGLCLManager->m_GLOutputUIMutex.lock();
                    m_OpenGLCLManager->m_GLMutex.lock();
                    glfwMakeContextCurrent(m_OpenGLCLManager->window);

                    m_OpenGLCLManager->CopyTextureToMap(T_SFHCUM,&(SLOPEFAILURETEMP->data));

                    glfwMakeContextCurrent(NULL);
                    m_OpenGLCLManager->m_GLMutex.unlock();
                    m_OpenGLCLManager->m_GLOutputUIMutex.unlock();

                    double slopefailvolold = slopefailvol;

                    slopefailvol = 0.0;
                    FOR_ROW_COL_MV(SLOPEFAILURETEMP)
                    {
                        slopefailvol += SLOPEFAILURETEMP->data[r][c] * _dx*_dx;

                    }

                    std::cout << "slope fail " << slopefailvol << " " << m_DoInitialStability << std::endl;

                    double slopefailvol_i = slopefailvol - slopefailvolold;

                    //if no significant failure, exit loop
                    if(slopefailvol_i < _dx * _dx * 0.1)
                    {
                        break;
                    }
                }//end for loop
            }

        }else//end if(m_Loaded)
        {
            LISEM_ERROR("Model initialization error");
            break;
        }


        //update modeldata structure for interface
        m_ModelDataMutex.lock();
        uimap = m_UIMapNums.at(m_InterfaceData.m_CurrentUIMap);
        m_InterfaceData.t = t;
        m_InterfaceData.timesteps = steps;
        m_InterfaceData.step = m_Step;
        m_InterfaceData.dt = dt;

        if(first)
        {
            m_InterfaceData.m_mapnameschanged = true;
        }

        m_ModelDataMutex.unlock();

        //not check if we can output maps to the gl thread for drawing later
        m_OpenGLCLManager->m_GLOutputUIMutex.lock();
        m_OpenGLCLManager->m_GLMutex.lock();
        glfwMakeContextCurrent(m_OpenGLCLManager->window);

        m_uimap = uimap;
        m_uimin = m_InterfaceData.val_min;
        m_uimax = m_InterfaceData.val_max;

        //copy the maps using shaders to textures used in the UI
        //this ensures we can safely read those textures while we edit the actual ones
        CopyToUI();

        //make sure the reporting thread from the previous timestep is done
        if(m_ThreadReport.joinable())
        {
            m_ThreadReport.join();
        }

        //now there are a couple of maps we need for output reporting, we get these now
        GetOutput(t,dt);


        //calculate relevant info for the interface
        ReportOutputToInterface(t,dt);

        //now free the opengl context and mutexes
        glfwMakeContextCurrent(NULL);

        m_OpenGLCLManager->m_GLMutex.unlock();
        m_OpenGLCLManager->m_GLOutputUIMutex.unlock();

        //start new thread that writes all user-asked files to disk
        //doing this on a seperate thread is worth it since writing map files to disk
        //is slow and we can write quite a lot of data!
        //(>10 mb per timestep easily, >500 mb in worst cases)
        m_ThreadReport = std::thread((&LISEMModel::ReportOutputToDisk),this);

        //signal the drawing functions that we have set the maps at least once
        m_UIMapsSet = true;
        if(m_Step == 0)
        {
            if(m_CallBackTimeStepSet)
            {
                m_CallBackTimeStep(true,false);
            }
            m_RequiresUIReset = true;
        }

        //call the interface to update its values
        if(m_CallBack_ModelToInterface_Set)
        {
            m_CallBack_ModelToInterface();
        }

        if(m_CallBackTimeStepSet)
        {
            m_CallBackTimeStep(false, false);
        }

        //
        t = t + dt;
        m_Step = m_Step + 1;
        first = false;

    }//end while(t < t_end)

    if(m_CallBackTimeStepSet)
    {
        m_CallBackTimeStep(false,true);
    }

    if(m_ThreadReport.joinable())
    {
        m_ThreadReport.join();
    }

    LISEM_STATUS("Model Finished");


}


#include "model.h"
#include "modelcpucode.h"


void LISEMModel::ModelRunCompileCode()
{
    //get display shaders

    //compile codes


    m_DoCPU = GetOptionInt("Use CPU") == 1;

    LISEM_STATUS("compiling codes");

    if(m_DoCPU)
    {
        LISEM_STATUS("Using CPU, no kernels to compile for GPU");

        m_CLProgram_Flow=new OpenCLProgram();
        m_CLProgram_Flow->LoadFromCPUFunction([](std::vector<void*> args){

            flow(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),

                    MAPARG(args[5]),
                    MAPARG(args[6]),
                    MAPARG(args[7]),
                    MAPARG(args[8]),
                    MAPARG(args[9]),
                    MAPARG(args[10]),
                    MAPARG(args[11]),
                    MAPARG(args[12]),
                    MAPARG(args[13]),
                    MAPARG(args[14]),
                    MAPARG(args[15]),
                    MAPARG(args[16]),
                    MAPARG(args[17]),
                    MAPARG(args[18]),
                    MAPARG(args[19]),
                    MAPARG(args[20]),
                    MAPARG(args[21]),
                    MAPARG(args[22]),
                    MAPARG(args[23]),
                    MAPARG(args[24]),
                    INTARG(args[25]),
                    FLTARG(args[26]),
                    MAPARG(args[27]),
                    MAPARG(args[28]),
                    MAPARG(args[29]),
                    MAPARG(args[30]),
                    MAPARG(args[31]),
                    MAPARG(args[32]),
                    MAPARG(args[33]),
                    MAPARG(args[34]),
                    MAPARG(args[35]),
                    MAPARG(args[36]),
                    MAPARG(args[37]),
                    MAPARG(args[38]),
                    MAPARG(args[39]),
                    MAPARG(args[40]),
                    MAPARG(args[41]),
                    MAPARG(args[42]),
                    MAPARG(args[43]),
                    MAPARG(args[44]),
                    MAPARG(args[45]),
                    MAPARG(args[46])); return true;});

        m_CLProgram_Flow2=new OpenCLProgram();
        m_CLProgram_Flow2->LoadFromCPUFunction([](std::vector<void*> args){ flow2(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),

                    MAPARG(args[5]),
                    MAPARG(args[6]),
                    MAPARG(args[7]),
                    MAPARG(args[8]),
                    MAPARG(args[9]),
                    MAPARG(args[10]),
                    MAPARG(args[11]),
                    MAPARG(args[12]),
                    MAPARG(args[13]),
                    MAPARG(args[14]),
                    MAPARG(args[15]),
                    MAPARG(args[16]),
                    MAPARG(args[17]),
                    MAPARG(args[18]),
                    MAPARG(args[19]),
                    MAPARG(args[20]),
                    MAPARG(args[21]),
                    MAPARG(args[22]),
                    MAPARG(args[23])); return true;});



        m_CLProgram_FlowSolids=new OpenCLProgram();
        m_CLProgram_FlowSolids->LoadFromCPUFunction([](std::vector<void*> args){ flowsolid(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),

                    MAPARG(args[5]),
                    MAPARG(args[6]),
                    MAPARG(args[7]),
                    MAPARG(args[8]),
                    MAPARG(args[9]),
                    MAPARG(args[10]),
                    MAPARG(args[11]),
                    MAPARG(args[12]),
                    MAPARG(args[13]),
                    MAPARG(args[14]),
                    MAPARG(args[15]),
                    MAPARG(args[16]),
                    MAPARG(args[17]),
                    MAPARG(args[18]),
                    MAPARG(args[19]),
                    MAPARG(args[20]),
                    MAPARG(args[21]),
                    MAPARG(args[22]),
                    MAPARG(args[23]),
                    MAPARG(args[24]),
                    MAPARG(args[25]),
                    MAPARG(args[26]),
                    MAPARG(args[27]),
                    MAPARG(args[28]),
                    MAPARG(args[29]),
                    MAPARG(args[30]),
                    MAPARG(args[31]),
                    MAPARG(args[32]),
                    MAPARG(args[33]),
                    MAPARG(args[34]),
                    MAPARG(args[35]),
                    MAPARG(args[36]),
                    MAPARG(args[37]),
                    MAPARG(args[38]),
                    MAPARG(args[39]),
                    MAPARG(args[40]),
                    MAPARG(args[41]),
                    MAPARG(args[42]),
                    MAPARG(args[43]),
                    MAPARG(args[44]),
                    MAPARG(args[45]),
                    MAPARG(args[46]),
                    INTARG(args[47]),
                    FLTARG(args[48]),
                    MAPARG(args[49]),
                    MAPARG(args[50]),
                    MAPARG(args[51]),
                    MAPARG(args[52]),
                    MAPARG(args[53]),
                    MAPARG(args[54]),
                    MAPARG(args[55]),
                    FLTARG(args[56]),
                    MAPARG(args[57]),
                    MAPARG(args[58]),
                    MAPARG(args[59]),
                    MAPARG(args[60]),
                    MAPARG(args[61]),
                    MAPARG(args[62]),
                    MAPARG(args[63])
                    ); return true;});

        m_CLProgram_FlowSolids2=new OpenCLProgram();
        m_CLProgram_FlowSolids2->LoadFromCPUFunction([](std::vector<void*> args){ flowsolid2(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),

                    MAPARG(args[5]),
                    MAPARG(args[6]),
                    MAPARG(args[7]),
                    MAPARG(args[8]),
                    MAPARG(args[9]),
                    MAPARG(args[10]),
                    MAPARG(args[11]),
                    MAPARG(args[12]),
                    MAPARG(args[13]),
                    MAPARG(args[14]),
                    MAPARG(args[15]),
                    MAPARG(args[16]),
                    MAPARG(args[17]),
                    MAPARG(args[18]),
                    MAPARG(args[19]),
                    MAPARG(args[20]),
                    MAPARG(args[21]),
                    MAPARG(args[22]),
                    MAPARG(args[23]),
                    MAPARG(args[24]),
                    MAPARG(args[25]),
                    MAPARG(args[26]),
                    MAPARG(args[27]),
                    MAPARG(args[28]),
                    MAPARG(args[29]),
                    MAPARG(args[30]),
                    MAPARG(args[31]),
                    MAPARG(args[32]),
                    MAPARG(args[33]),
                    MAPARG(args[34]),
                    MAPARG(args[35]),
                    MAPARG(args[36]),
                    MAPARG(args[37]),
                    MAPARG(args[38]),
                    MAPARG(args[39]),
                    MAPARG(args[40]),
                    MAPARG(args[41]),
                    MAPARG(args[42]),
                    MAPARG(args[43]),
                    MAPARG(args[44]),
                    MAPARG(args[45]),
                    MAPARG(args[46]),
                    MAPARG(args[47]),
                    MAPARG(args[48]),
                    MAPARG(args[49])); return true;});


        m_CLProgram_Hydrology=new OpenCLProgram();
        m_CLProgram_Hydrology->LoadFromCPUFunction([](std::vector<void*> args){ hydrology(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),

                    MAPARG(args[5]),
                    MAPARG(args[6]),
                    MAPARG(args[7]),
                    MAPARG(args[8]),
                    MAPARG(args[9]),
                    MAPARG(args[10]),
                    MAPARG(args[11]),
                    MAPARG(args[12]),
                    MAPARG(args[13]),
                    MAPARG(args[14]),
                    MAPARG(args[15]),
                    MAPARG(args[16]),
                    MAPARG(args[17]),
                    MAPARG(args[18]),
                    MAPARG(args[19]),
                    MAPARG(args[20]),
                    MAPARG(args[21]),
                    MAPARG(args[22]),
                    MAPARG(args[23]),
                    MAPARG(args[24]),
                    MAPARG(args[25]),
                    MAPARG(args[26]),
                    MAPARG(args[27]),
                    MAPARG(args[28]),
                    MAPARG(args[29]),
                    MAPARG(args[30]),
                    MAPARG(args[31]),
                    MAPARG(args[32]),
                    MAPARG(args[33]),
                    MAPARG(args[34]),
                    MAPARG(args[35]),
                    MAPARG(args[36]),
                    MAPARG(args[37]),
                    INTARG(args[38]),
                    FLTARG(args[39])
                    ); return true;});

        m_CLProgram_Hydrology2=new OpenCLProgram();
        m_CLProgram_Hydrology2->LoadFromCPUFunction([](std::vector<void*> args){ hydrology2(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),

                    MAPARG(args[5]),
                    MAPARG(args[6]),
                    MAPARG(args[7]),
                    MAPARG(args[8]),
                    MAPARG(args[9]),
                    MAPARG(args[10]),
                    MAPARG(args[11]),
                    MAPARG(args[12]),
                    MAPARG(args[13]),
                    MAPARG(args[14]),
                    MAPARG(args[15]),
                    MAPARG(args[16]),
                    MAPARG(args[17]),
                    MAPARG(args[18]),
                    MAPARG(args[19]),
                    MAPARG(args[20])); return true;});


        m_CLProgram_Evapotranspiration=new OpenCLProgram();
        m_CLProgram_Evapotranspiration->LoadFromCPUFunction([](std::vector<void*> args){ evapotranspiration(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),

                    MAPARG(args[5]),
                    MAPARG(args[6]),
                    MAPARG(args[7]),
                    MAPARG(args[8]),
                    MAPARG(args[9]),
                    MAPARG(args[10]),
                    MAPARG(args[11]),
                    MAPARG(args[12]),
                    MAPARG(args[13]),
                    MAPARG(args[14]),
                    MAPARG(args[15]),
                    MAPARG(args[16]),
                    MAPARG(args[17]),
                    MAPARG(args[18]),
                    MAPARG(args[19]),
                    MAPARG(args[20]),
                    MAPARG(args[21]),
                    MAPARG(args[22]),
                    MAPARG(args[23]),
                    MAPARG(args[24]),
                    MAPARG(args[25]),
                    MAPARG(args[26]),
                    MAPARG(args[27]),
                    MAPARG(args[28]),
                    MAPARG(args[29]),
                    MAPARG(args[30]),
                    MAPARG(args[31]),
                    MAPARG(args[32]),
                    MAPARG(args[33]),
                    MAPARG(args[34]),
                    MAPARG(args[35]),
                    MAPARG(args[36]),
                    MAPARG(args[37]),
                    MAPARG(args[38]),
                    MAPARG(args[39]),
                    MAPARG(args[40]),
                    MAPARG(args[41]),
                    MAPARG(args[42]),
                    MAPARG(args[43]),
                    INTARG(args[43]),
                    FLTARG(args[44])
                    ); return true;});

        m_CLProgram_Evapotranspiration2=new OpenCLProgram();
        m_CLProgram_Evapotranspiration2->LoadFromCPUFunction([](std::vector<void*> args){ evapotranspiration2(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),

                    MAPARG(args[5]),
                    MAPARG(args[6]),
                    MAPARG(args[7]),
                    MAPARG(args[8]),
                    MAPARG(args[9]),
                    MAPARG(args[10]),
                    MAPARG(args[11]),
                    MAPARG(args[12]),
                    MAPARG(args[13]),
                    MAPARG(args[14]),
                    MAPARG(args[15]),
                    MAPARG(args[16]),
                    MAPARG(args[17]),
                    MAPARG(args[18]),
                    MAPARG(args[19]),
                    MAPARG(args[20])); return true;});

        m_CLProgram_Infiltration=new OpenCLProgram();
        m_CLProgram_Infiltration->LoadFromCPUFunction([](std::vector<void*> args){ InfilOnly(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),

                    MAPARG(args[5]),
                    MAPARG(args[6]),
                    MAPARG(args[7]),
                    MAPARG(args[8]),
                    MAPARG(args[9]),
                    MAPARG(args[10]),
                    MAPARG(args[11]),
                    MAPARG(args[12]),
                    MAPARG(args[13]),
                    MAPARG(args[14]),
                    MAPARG(args[15]),
                    MAPARG(args[16]),
                    INTARG(args[17]),
                    FLTARG(args[18])
                    ); return true;});

        m_CLProgram_Infiltration2=new OpenCLProgram();
        m_CLProgram_Infiltration2->LoadFromCPUFunction([](std::vector<void*> args){ InfilOnly2(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),

                    MAPARG(args[5]),
                    MAPARG(args[6]),
                    MAPARG(args[7]),
                    MAPARG(args[8]),
                    MAPARG(args[9]),
                    MAPARG(args[10])
                    ); return true;});




        m_CLProgram_FlowSolidsParticle=new OpenCLProgram();
        m_CLProgram_FlowSolidsParticle->LoadFromCPUFunction([](std::vector<void*> args){ flowsolidparticle(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),

                    MAPARG(args[5]),
                    MAPARG(args[6]),
                    MAPARG(args[7]),
                    MAPARG(args[8]),
                    MAPARG(args[9]),
                    MAPARG(args[10]),
                    MAPARG(args[11]),
                    MAPARG(args[12]),
                    MAPARG(args[13]),
                    MAPARG(args[14]),
                    MAPARG(args[15]),
                    MAPARG(args[16]),
                    MAPARG(args[17]),
                    MAPARG(args[18]),
                    MAPARG(args[19]),
                    MAPARG(args[20]),
                    MAPARG(args[21]),
                    MAPARG(args[22]),
                    MAPARG(args[23]),
                    MAPARG(args[24]),
                    MAPARG(args[25]),
                    MAPARG(args[26]),
                    MAPARG(args[27]),
                    MAPARG(args[28]),
                    MAPARG(args[29]),
                    MAPARG(args[30]),
                    MAPARG(args[31]),
                    MAPARG(args[32]),
                    MAPARG(args[33]),
                    MAPARG(args[34]),
                    MAPARG(args[35]),
                    MAPARG(args[36]),
                    MAPARG(args[37]),
                    MAPARG(args[38]),
                    MAPARG(args[39]),
                    MAPARG(args[40]),
                    MAPARG(args[41]),
                    MAPARG(args[42]),
                    MAPARG(args[43]),
                    MAPARG(args[44]),
                    MAPARG(args[45]),
                    MAPARG(args[46]),
                    INTARG(args[47]),
                    FLTARG(args[48]),
                    MAPARG(args[49]),
                    MAPARG(args[50]),
                    MAPARG(args[51]),
                    MAPARG(args[52]),
                    MAPARG(args[53]),
                    MAPARG(args[54]),
                    MAPARG(args[55]),
                    MAPARG(args[56]),
                    MAPARG(args[57]),
                    MAPARG(args[58]),
                    MAPARG(args[59]),
                    FLTARG(args[60]),
                    MAPARG(args[61]),
                    MAPARG(args[62]),
                    MAPARG(args[63]),
                    MAPARG(args[64]),
                    MAPARG(args[65]),
                    MAPARG(args[66]),
                    MAPARG(args[67])
                    ); return true;});

        m_CLProgram_FlowSolidsParticle2=new OpenCLProgram();
        m_CLProgram_FlowSolidsParticle2->LoadFromCPUFunction([](std::vector<void*> args){ flowsolidparticle2(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),

                    MAPARG(args[5]),
                    MAPARG(args[6]),
                    MAPARG(args[7]),
                    MAPARG(args[8]),
                    MAPARG(args[9]),
                    MAPARG(args[10]),
                    MAPARG(args[11]),
                    MAPARG(args[12]),
                    MAPARG(args[13]),
                    MAPARG(args[14]),
                    MAPARG(args[15]),
                    MAPARG(args[16]),
                    MAPARG(args[17]),
                    MAPARG(args[18]),
                    MAPARG(args[19]),
                    MAPARG(args[20]),
                    MAPARG(args[21]),
                    MAPARG(args[22]),
                    MAPARG(args[23]),
                    MAPARG(args[24]),
                    MAPARG(args[25]),
                    MAPARG(args[26]),
                    MAPARG(args[27]),
                    MAPARG(args[28]),
                    MAPARG(args[29]),
                    MAPARG(args[30]),
                    MAPARG(args[31]),
                    MAPARG(args[32]),
                    MAPARG(args[33]),
                    MAPARG(args[34]),
                    MAPARG(args[35]),
                    MAPARG(args[36]),
                    MAPARG(args[37]),
                    MAPARG(args[38]),
                    MAPARG(args[39]),
                    MAPARG(args[40]),
                    MAPARG(args[41]),
                    MAPARG(args[42]),
                    MAPARG(args[43]),
                    MAPARG(args[44]),
                    MAPARG(args[45]),
                    MAPARG(args[46]),
                    MAPARG(args[47]),
                    MAPARG(args[48]),
                    MAPARG(args[49]),
                    MAPARG(args[50])
                    ); return true;});





        m_CLProgram_Sort=new OpenCLProgram();
        m_CLProgram_Sort->LoadFromCPUFunction([](std::vector<void*> args){ _kbitonic_stl_sort(INTARG(args[0]),BUFFERINTARG(args[1]),BUFFERINTARG(args[2]),INTARG(args[3]),INTARG(args[4]),

                    INTARG(args[5]),INTARG(args[6])); return true;});



        m_CLProgram_StoreIndices=new OpenCLProgram();
        m_CLProgram_StoreIndices->LoadFromCPUFunction([](std::vector<void*> args){ _storeindices(INTARG(args[0]),BUFFERINTARG(args[1]),BUFFERINTARG(args[2]),MAPARG(args[3]),INTARG(args[4]),

                    INTARG(args[5]),INTARG(args[6])); return true;});



        m_CLProgram_Particles=new OpenCLProgram();
        m_CLProgram_Particles->LoadFromCPUFunction([](std::vector<void*> args){ _particledynamics(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),

                            BUFFERFLTARG(args[5]),
                            BUFFERFLTARG(args[6]),
                            BUFFERFLTARG(args[7]),
                            BUFFERFLTARG(args[8]),
                            BUFFERINTARG(args[9]),
                            BUFFERINTARG(args[10]),
                            BUFFERFLTARG(args[11]),
                            INTARG(args[12]),
                            MAPARG(args[13]),
                            MAPARG(args[14]),
                            BUFFERFLTARG(args[15]),
                            BUFFERFLTARG(args[16]),
                            BUFFERFLTARG(args[17]),
                            BUFFERFLTARG(args[18]),
                            BUFFERFLTARG(args[19]),
                            BUFFERFLTARG(args[20]),
                            MAPARG(args[21]),
                            FLTARG(args[22]),
                            FLTARG(args[23]),
                            FLTARG(args[24]),
                            FLTARG(args[25])
                    ); return true;});

        m_CLProgram_Particles2=new OpenCLProgram();
        m_CLProgram_Particles2->LoadFromCPUFunction([](std::vector<void*> args){ _particledynamics2(INTARG(args[0]),

                            BUFFERFLTARG(args[1]),
                            BUFFERFLTARG(args[2]),
                            BUFFERFLTARG(args[3]),
                            BUFFERFLTARG(args[4]),
                            BUFFERINTARG(args[5]),
                            BUFFERFLTARG(args[6]),
                            BUFFERFLTARG(args[7]),
                            BUFFERFLTARG(args[8]),
                            BUFFERFLTARG(args[9]),
                            MAPARG(args[10]),
                            FLTARG(args[11]),
                            INTARG(args[12]),
                            INTARG(args[13]),
                            INTARG(args[14]),
                            FLTARG(args[15])
                    ); return true;});


        m_CLProgram_ParticlesToMaps=new OpenCLProgram();
        m_CLProgram_ParticlesToMaps->LoadFromCPUFunction([](std::vector<void*> args){ _particletomaps(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),

                            MAPARG(args[5]),
                            MAPARG(args[6]),
                            BUFFERFLTARG(args[7]),
                            BUFFERFLTARG(args[8]),
                            BUFFERFLTARG(args[9]),
                            BUFFERFLTARG(args[10]),
                            BUFFERINTARG(args[11]),
                            BUFFERINTARG(args[12]),
                            INTARG(args[13]),
                            MAPARG(args[14]),
                            MAPARG(args[15]),
                            MAPARG(args[16]),
                            MAPARG(args[17]),
                            MAPARG(args[18]),
                            MAPARG(args[19]),
                            MAPARG(args[20]),
                            MAPARG(args[21]),
                            MAPARG(args[22]),
                            BUFFERFLTARG(args[23]),
                            MAPARG(args[24]),
                            MAPARG(args[25]),
                            FLTARG(args[26])
                    ); return true;});

        m_CLProgram_ParticlesFromMaps=new OpenCLProgram();
        m_CLProgram_ParticlesFromMaps->LoadFromCPUFunction([](std::vector<void*> args){ _particlefrommaps(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),


                            BUFFERFLTARG(args[5]),
                            BUFFERFLTARG(args[6]),
                            BUFFERFLTARG(args[7]),
                            BUFFERFLTARG(args[8]),
                            BUFFERINTARG(args[9]),
                            BUFFERINTARG(args[10]),
                            INTARG(args[11]),
                            MAPARG(args[12]),
                            MAPARG(args[13]),
                            MAPARG(args[14]),
                            MAPARG(args[15]),
                            MAPARG(args[16]),
                            MAPARG(args[17]),
                            MAPARG(args[18]),
                            MAPARG(args[19]),
                            MAPARG(args[20]),
                            BUFFERFLTARG(args[21]),
                            FLTARG(args[22])
                    ); return true;});





        m_CLProgram_GroundWaterFlow=new OpenCLProgram();
        m_CLProgram_GroundWaterFlow->LoadFromCPUFunction([](std::vector<void*> args){GroundWaterFlow(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),


                            MAPARG(args[5]),
                            MAPARG(args[6]),
                            MAPARG(args[7]),
                            MAPARG(args[8]),
                            MAPARG(args[9]),
                            MAPARG(args[10]),
                            MAPARG(args[11]),
                            MAPARG(args[12]),
                            MAPARG(args[13]),
                            MAPARG(args[14]),
                            MAPARG(args[15])
                    ); return true;});


        m_CLProgram_GroundWaterFlow2=new OpenCLProgram();
        m_CLProgram_GroundWaterFlow2->LoadFromCPUFunction([](std::vector<void*> args){ GroundWaterFlow2(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),


                            MAPARG(args[5]),
                            MAPARG(args[6]),
                            MAPARG(args[7]),
                            MAPARG(args[8]),
                            MAPARG(args[9])
                    ); return true;});


        m_CLProgram_Rigid=new OpenCLProgram();
        m_CLProgram_Rigid->LoadFromCPUFunction([](std::vector<void*> args){ rigid(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),


                            MAPARG(args[5]),
                            MAPARG(args[6]),
                            MAPARG(args[7]),
                            MAPARG(args[8]),
                            MAPARG(args[9]),
                            MAPARG(args[10]),
                            MAPARG(args[11]),
                            INTARG(args[12])
                    ); return true;});

        m_CLProgram_Rigid2=new OpenCLProgram();
        m_CLProgram_Rigid2->LoadFromCPUFunction([](std::vector<void*> args){ rigid2(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),


                            MAPARG(args[5]),
                            MAPARG(args[6]),
                            MAPARG(args[7]),
                            MAPARG(args[8]),
                            MAPARG(args[9]),
                            MAPARG(args[10]),
                            MAPARG(args[11]),
                            INTARG(args[12])
                    ); return true;});


        m_CLProgram_SlopeStability=new OpenCLProgram();
        m_CLProgram_SlopeStability->LoadFromCPUFunction([](std::vector<void*> args){ SlopeStability(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),


                            MAPARG(args[5]),
                            MAPARG(args[6]),
                            MAPARG(args[7]),
                            MAPARG(args[8]),
                            MAPARG(args[9]),
                            MAPARG(args[10]),
                            MAPARG(args[11]),
                            MAPARG(args[12]),
                            MAPARG(args[13]),
                            MAPARG(args[14]),
                            MAPARG(args[15]),
                            MAPARG(args[16]),
                            MAPARG(args[17]),
                            MAPARG(args[18]),
                            INTARG(args[19]),
                            MAPARG(args[20]),
                            INTARG(args[21]),
                            FLTARG(args[22])

                    ); return true;});

        m_CLProgram_SlopeStabilityE=new OpenCLProgram();
        m_CLProgram_SlopeStabilityE->LoadFromCPUFunction([](std::vector<void*> args){ SlopeStabilityExtended(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),


                            MAPARG(args[5]),
                            MAPARG(args[6]),
                            MAPARG(args[7]),
                            MAPARG(args[8]),
                            MAPARG(args[9]),
                            MAPARG(args[10]),
                            MAPARG(args[11]),
                            MAPARG(args[12]),
                            MAPARG(args[13]),
                            MAPARG(args[14]),
                            MAPARG(args[15]),
                            MAPARG(args[16]),
                            MAPARG(args[17]),
                            MAPARG(args[18]),
                            MAPARG(args[19]),
                            MAPARG(args[20]),
                            MAPARG(args[21]),
                            INTARG(args[22]),
                            MAPARG(args[23]),
                            INTARG(args[24]),
                            FLTARG(args[25])

                    ); return true;});


        m_CLProgram_SlopeFailure=new OpenCLProgram();
        m_CLProgram_SlopeFailure->LoadFromCPUFunction([](std::vector<void*> args){ SlopeFailure(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),


                            MAPARG(args[5]),
                            MAPARG(args[6]),
                            MAPARG(args[7]),
                            MAPARG(args[8]),
                            MAPARG(args[9]),
                            MAPARG(args[10]),
                            MAPARG(args[11]),
                            MAPARG(args[12]),
                            MAPARG(args[13]),
                            MAPARG(args[14]),
                            MAPARG(args[15]),
                            MAPARG(args[16]),
                            MAPARG(args[17]),
                            MAPARG(args[18]),
                            MAPARG(args[19]),
                            MAPARG(args[20]),
                            MAPARG(args[21]),
                            MAPARG(args[22]),
                            MAPARG(args[23]),
                            MAPARG(args[24]),
                            MAPARG(args[25]),
                            MAPARG(args[26]),
                            MAPARG(args[27]),
                            MAPARG(args[28]),
                            MAPARG(args[29]),
                            MAPARG(args[30]),
                            MAPARG(args[31]),
                            MAPARG(args[32]),
                            MAPARG(args[33]),
                            MAPARG(args[34]),
                            INTARG(args[35]),
                            INTARG(args[36])

                    ); return true;});


        m_CLProgram_SlopeFailureE=new OpenCLProgram();
        m_CLProgram_SlopeFailureE->LoadFromCPUFunction([](std::vector<void*> args){ SlopeFailureExtended(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),


                            MAPARG(args[5]),
                            MAPARG(args[6]),
                            MAPARG(args[7]),
                            MAPARG(args[8]),
                            MAPARG(args[9]),
                            MAPARG(args[10]),
                            MAPARG(args[11]),
                            MAPARG(args[12]),
                            MAPARG(args[13]),
                            MAPARG(args[14]),
                            MAPARG(args[15]),
                            MAPARG(args[16]),
                            MAPARG(args[17]),
                            MAPARG(args[18]),
                            MAPARG(args[19]),
                            MAPARG(args[20]),
                            MAPARG(args[21]),
                            MAPARG(args[22]),
                            MAPARG(args[23]),
                            MAPARG(args[24]),
                            MAPARG(args[25]),
                            MAPARG(args[26]),
                            MAPARG(args[27]),
                            MAPARG(args[28]),
                            MAPARG(args[29]),
                            MAPARG(args[30]),
                            MAPARG(args[31]),
                            MAPARG(args[32]),
                            MAPARG(args[33]),
                            MAPARG(args[34]),
                            MAPARG(args[35]),
                            MAPARG(args[36]),
                            MAPARG(args[37]),
                            INTARG(args[38]),
                            INTARG(args[39])

                    ); return true;});

        m_CLProgram_SlopeFailure2=new OpenCLProgram();
        m_CLProgram_SlopeFailure2->LoadFromCPUFunction([](std::vector<void*> args){ SlopeFailure2(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),


                            MAPARG(args[5]),
                            MAPARG(args[6]),
                            MAPARG(args[7]),
                            MAPARG(args[8]),
                            MAPARG(args[9]),
                            MAPARG(args[10]),
                            MAPARG(args[11]),
                            MAPARG(args[12]),
                            MAPARG(args[13]),
                            MAPARG(args[14]),
                            MAPARG(args[15]),
                            MAPARG(args[16]),
                            MAPARG(args[17]),
                            MAPARG(args[18]),
                            MAPARG(args[19]),
                            MAPARG(args[20]),
                            MAPARG(args[21]),
                            MAPARG(args[22]),
                            MAPARG(args[23]),
                            MAPARG(args[24]),
                            MAPARG(args[25]),
                            MAPARG(args[26]),
                            MAPARG(args[27]),
                            INTARG(args[28]),
                            MAPARG(args[29]),
                            MAPARG(args[30])

                    ); return true;});

        m_CLProgram_SlopeFailureP2=new OpenCLProgram();
        m_CLProgram_SlopeFailureP2->LoadFromCPUFunction([](std::vector<void*> args){ SlopeFailureP2(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),


                            MAPARG(args[5]),
                            MAPARG(args[6]),
                            MAPARG(args[7]),
                            MAPARG(args[8]),
                            MAPARG(args[9]),
                            MAPARG(args[10]),
                            MAPARG(args[11]),
                            MAPARG(args[12]),
                            MAPARG(args[13]),
                            MAPARG(args[14]),
                            MAPARG(args[15]),
                            MAPARG(args[16]),
                            MAPARG(args[17]),
                            MAPARG(args[18]),
                            MAPARG(args[19]),
                            MAPARG(args[20]),
                            MAPARG(args[21]),
                            MAPARG(args[22]),
                            MAPARG(args[23]),
                            MAPARG(args[24]),
                            MAPARG(args[25]),
                            MAPARG(args[26]),
                            MAPARG(args[27]),
                            INTARG(args[28]),
                            MAPARG(args[29]),
                            MAPARG(args[30])

                    ); return true;});




        m_CLProgram_Erosion=new OpenCLProgram();
        m_CLProgram_Erosion->LoadFromCPUFunction([](std::vector<void*> args){ sediment(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),


                            MAPARG(args[5]),
                            MAPARG(args[6]),
                            MAPARG(args[7]),
                            MAPARG(args[8]),
                            MAPARG(args[9]),
                            MAPARG(args[10]),
                            MAPARG(args[11]),
                            MAPARG(args[12]),
                            MAPARG(args[13]),
                            MAPARG(args[14]),
                            MAPARG(args[15]),
                            MAPARG(args[16]),
                            MAPARG(args[17]),
                            MAPARG(args[18]),
                            MAPARG(args[19]),
                            MAPARG(args[20]),
                            MAPARG(args[21]),
                            MAPARG(args[22]),
                            MAPARG(args[23]),
                            MAPARG(args[24]),
                            MAPARG(args[25]),
                            MAPARG(args[26]),
                            MAPARG(args[27]),
                            MAPARG(args[28]),
                            MAPARG(args[29]),
                            MAPARG(args[30]),
                            MAPARG(args[31]),
                            MAPARG(args[32]),
                            MAPARG(args[33]),
                            MAPARG(args[34]),
                            MAPARG(args[35]),
                            MAPARG(args[36]),
                            MAPARG(args[37]),
                            MAPARG(args[38]),
                            MAPARG(args[39]),
                            MAPARG(args[40]),
                            MAPARG(args[41]),
                            MAPARG(args[42]),
                            MAPARG(args[43]),
                            MAPARG(args[44]),
                            MAPARG(args[45]),
                            MAPARG(args[46]),
                            MAPARG(args[47]),
                            MAPARG(args[48]),
                            MAPARG(args[49]),
                            MAPARG(args[50]),
                            MAPARG(args[51]),
                            MAPARG(args[52]),
                            MAPARG(args[53]),
                            MAPARG(args[54]),
                            MAPARG(args[55]),
                            MAPARG(args[56]),
                            MAPARG(args[57]),
                            MAPARG(args[58]),
                            MAPARG(args[59]),
                            MAPARG(args[60]),
                            INTARG(args[61]),
                            INTARG(args[62]),
                            MAPARG(args[63]),
                            MAPARG(args[64]),
                            MAPARG(args[65]),
                            MAPARG(args[66]),
                            MAPARG(args[67]),
                            MAPARG(args[68]),
                            MAPARG(args[69]),
                            MAPARG(args[70]),
                            MAPARG(args[71]),
                            MAPARG(args[72]),
                            MAPARG(args[73]),
                            MAPARG(args[74]),
                            MAPARG(args[75]),
                            MAPARG(args[76]),
                            MAPARG(args[77]),
                            MAPARG(args[78]),
                            MAPARG(args[79]),
                            MAPARG(args[80]),
                            MAPARG(args[81]),
                            FLTARG(args[82]),
                            MAPARG(args[83]),
                            MAPARG(args[84]),
                            MAPARG(args[85]),
                            INTARG(args[86]),
                            MAPARG(args[87]),
                            MAPARG(args[88]),
                            MAPARG(args[89])


                    ); return true;});


        m_CLProgram_Erosion2=new OpenCLProgram();
        m_CLProgram_Erosion2->LoadFromCPUFunction([](std::vector<void*> args){ sediment2(INTARG(args[0]),INTARG(args[1]),INTARG(args[2]),FLTARG(args[3]),FLTARG(args[4]),


                            MAPARG(args[5]),
                            MAPARG(args[6]),
                            MAPARG(args[7]),
                            MAPARG(args[8]),
                            MAPARG(args[9]),
                            MAPARG(args[10]),
                            MAPARG(args[11]),
                            MAPARG(args[12]),
                            MAPARG(args[13]),
                            MAPARG(args[14]),
                            MAPARG(args[15]),
                            MAPARG(args[16]),
                            MAPARG(args[17]),
                            MAPARG(args[18]),
                            MAPARG(args[19]),
                            MAPARG(args[20]),
                            MAPARG(args[21]),
                            MAPARG(args[22]),
                            MAPARG(args[23]),
                            MAPARG(args[24]),
                            MAPARG(args[25]),
                            MAPARG(args[26]),
                            MAPARG(args[27]),
                            MAPARG(args[28]),
                            MAPARG(args[29]),
                            MAPARG(args[30]),
                            MAPARG(args[31]),
                            MAPARG(args[32]),
                            MAPARG(args[33]),
                            MAPARG(args[34]),
                            MAPARG(args[35]),
                            MAPARG(args[36]),
                            MAPARG(args[37]),
                            MAPARG(args[38]),
                            MAPARG(args[39]),
                            MAPARG(args[40]),
                            MAPARG(args[41]),
                            MAPARG(args[42]),
                            MAPARG(args[43]),
                            MAPARG(args[44]),
                            MAPARG(args[45]),
                            MAPARG(args[46]),
                            MAPARG(args[47]),
                            MAPARG(args[48]),
                            MAPARG(args[49]),
                            MAPARG(args[50]),
                            MAPARG(args[51]),
                            MAPARG(args[52]),
                            MAPARG(args[53]),
                            MAPARG(args[54]),
                            MAPARG(args[55]),
                            MAPARG(args[56]),
                            MAPARG(args[57]),
                            MAPARG(args[58]),
                            MAPARG(args[59]),
                            MAPARG(args[60]),
                            INTARG(args[61]),
                            INTARG(args[62]),
                            MAPARG(args[63]),
                            MAPARG(args[64]),
                            MAPARG(args[65]),
                            MAPARG(args[66]),
                            MAPARG(args[67]),
                            MAPARG(args[68]),
                            MAPARG(args[69]),
                            MAPARG(args[70]),
                            MAPARG(args[71]),
                            MAPARG(args[72]),
                            MAPARG(args[73]),
                            MAPARG(args[74]),
                            MAPARG(args[75]),
                            MAPARG(args[76])


                    ); return true;});

    }else
    {
        m_CLProgram_Flow= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_flow.cl", "flow");
        m_CLProgram_Flow2= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_flow.cl", "flow2");


        m_CLProgram_Infiltration= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_hydrology.cl", "InfilOnly");
        m_CLProgram_Infiltration2= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_hydrology.cl", "InfilOnly2");

        m_CLProgram_Hydrology= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_hydrology.cl", "hydrology");
        m_CLProgram_Hydrology2= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_hydrology.cl", "hydrology2");

        if(m_DoEvapoTranspiration)
        {
            m_CLProgram_Evapotranspiration= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_evapotranspiration.cl", "evapotranspiration");
            m_CLProgram_Evapotranspiration2= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_evapotranspiration.cl", "evapotranspiration2");
        }

        m_CLProgram_Erosion =m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_sediment.cl", "sediment");
        m_CLProgram_Erosion2 = m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_sediment.cl", "sediment2");

        m_CLProgram_Rigid =m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_rigid.cl", "rigid");
        m_CLProgram_Rigid2 = m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_rigid.cl", "rigid2");

        m_CLProgram_GroundWaterFlow= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_groundwater.cl", "GroundWaterFlow");
        m_CLProgram_GroundWaterFlow2= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_groundwater.cl", "GroundWaterFlow2");

        m_CLProgram_SlopeFailure= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_slopestability.cl", "SlopeFailure");
        m_CLProgram_SlopeFailureE= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_slopestability.cl", "SlopeFailureExtended");
        m_CLProgram_SlopeFailure2= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_slopestability.cl", "SlopeFailure2");
        m_CLProgram_SlopeFailureP2= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_slopestability.cl", "SlopeFailureP2");


        m_CLProgram_SlopeStability= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_slopestability.cl", "SlopeStability");
        m_CLProgram_SlopeStabilityE= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_slopestability.cl", "SlopeStabilityExtended");

        //m_CLProgram_SubSurfaceForce1= m_OpenGLCLManager->ModelGetMCLProgram(m_AssetDir +  "model_hydrology.cl", "GroundWaterFlow");
        //m_CLProgram_SubSurfaceForce2= m_OpenGLCLManager->ModelGetMCLProgram(m_AssetDir +  "model_hydrology.cl", "GroundWaterFlow");

        m_CLProgram_FlowSolids= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_flow_solids.cl", "flowsolid");
        m_CLProgram_FlowSolids2= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_flow_solids.cl", "flowsolid2");

        m_CLProgram_FlowSolidsParticle= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_flow_solidsparticle.cl", "flowsolidparticle");
        m_CLProgram_FlowSolidsParticle2= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "model_flow_solidsparticle.cl", "flowsolidparticle2");

        m_CLProgram_Sort= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "sort.cl", "_kbitonic_stl_sort");

        m_CLProgram_StoreIndices= m_OpenGLCLManager->ModelGetMCLProgram(m_KernelDir +  "indexstore.cl", "_storeindices");


        m_CLProgram_Particles = m_OpenGLCLManager->ModelGetMCLProgram( m_KernelDir + "particles.cl","_particledynamics");
        m_CLProgram_Particles2 = m_OpenGLCLManager->ModelGetMCLProgram( m_KernelDir + "particles.cl","_particledynamics2");

        m_CLProgram_ParticlesToMaps = m_OpenGLCLManager->ModelGetMCLProgram( m_KernelDir + "particles_mpm.cl","_particletomaps");
        m_CLProgram_ParticlesFromMaps = m_OpenGLCLManager->ModelGetMCLProgram( m_KernelDir + "particles_mpm.cl","_particlefrommaps");


    }


}



void LISEMModel::SetUIMaps()
{


    m_UIMapNames.append("default");
    m_UIMapNums.append(0);

    m_UIMapNames.append("Flow Height (m)");
    m_UIMapNums.append(1);
    m_UIMapNames.append("Flow Velocity (m/s)");
    m_UIMapNums.append(14);

    if(m_DoChannel)
    {
        m_UIMapNames.append("Channel Flow Height (m)");
        m_UIMapNums.append(5);

        m_UIMapNames.append("Channel Flow Velocity (m)");
        m_UIMapNums.append(15);
    }

    if(!m_DoHydrology && m_DoInfiltration)
    {
        m_UIMapNames.append("Wetting Front Depth (m)");
        m_UIMapNums.append(2);
        m_UIMapNames.append("Cumulative Infiltration (m)");
        m_UIMapNums.append(4);
    }
    if(m_DoHydrology)
    {

        m_UIMapNames.append("Wetting Front Depth (m)");
        m_UIMapNums.append(2);
        m_UIMapNames.append("Ground Water Table (m)");
        m_UIMapNums.append(3);
        m_UIMapNames.append("Cumulative Infiltration (m)");
        m_UIMapNums.append(4);

        if(m_DoErosion)
        {
            m_UIMapNames.append("Net Soil Loss (kg/m2)");
            m_UIMapNums.append(25);
            m_UIMapNames.append("Sediment Concentration (kg/m3)");
            m_UIMapNums.append(26);
            m_UIMapNames.append("Sediment Load (kg/m2)");
            m_UIMapNums.append(27);
        }

    }

    if(m_DoHydrology && m_DoSlopeStability)
    {
        m_UIMapNames.append("Factor Of Safety");
        m_UIMapNums.append(6);
    }

    if(m_DoHydrology && m_DoSlopeStability && m_DoSlopeFailure)
    {

        m_UIMapNames.append("Failure Height (m)");
        m_UIMapNums.append(7);
    }
    if(m_DoSlopeFailure || m_DoInitialSolids)
    {
        m_UIMapNames.append("Solid Flow Height (m)");
        m_UIMapNums.append(8);
        m_UIMapNames.append("Solid Flow Velocity (m/s)");
        m_UIMapNums.append(10);

        if(m_DoEntrainment)
        {
            m_UIMapNames.append("Entrainment (m)");
            m_UIMapNums.append(28);
        }

        if(m_DoChannel)
        {
            m_UIMapNames.append("Channel Solid Height (m)");
            m_UIMapNums.append(12);
            m_UIMapNames.append("Channel Solid Velocity (m/s)");
            m_UIMapNums.append(13);
        }

        m_UIMapNames.append("Internal Friction Angle (Radians)");
        m_UIMapNums.append(17);
        m_UIMapNames.append("Rocksize (m)");
        m_UIMapNums.append(18);
        m_UIMapNames.append("Density (kg/m3)");
        m_UIMapNums.append(19);
        m_UIMapNames.append("DragC. (-)");
        m_UIMapNums.append(40);
    }

    if(m_DoParticleSolids)
    {

        m_UIMapNames.append("Coh. Fraction (fraction)");
        m_UIMapNums.append(20);


    }

    if(m_DoRigidWorld)
    {

        m_UIMapNames.append("Rigid Body Flow Blocking (fraction)");
        m_UIMapNums.append(30);


    }


}
void LISEMModel::SetKernelInput()
{
    int dim0 = DEM->nrCols();
    int dim1 = DEM->nrRows();


    {
        m_CLProgram_Flow->SetRunDims((int) m_NCells,0,0);

        m_CLProgram_Flow->PlaceArgument(0, (int)dim0);
        m_CLProgram_Flow->PlaceArgument(1, (int)dim1);
        m_CLProgram_Flow->PlaceArgument(2, (float)(DEM->cellSize()));
        m_CLProgram_Flow->PlaceArgument(3, (float)(0.5));
        m_CLProgram_Flow->PlaceArgument(4, T_LOCR);
        m_CLProgram_Flow->PlaceArgument(5, T_LOCC);
        m_CLProgram_Flow->PlaceArgument(6, T_DEM);
        m_CLProgram_Flow->PlaceArgument(7, T_H);
        m_CLProgram_Flow->PlaceArgument(8, T_VX);
        m_CLProgram_Flow->PlaceArgument(9, T_VY);
        m_CLProgram_Flow->PlaceArgument(10, T_F_INFILPOT);
        m_CLProgram_Flow->PlaceArgument(11, T_F_INFILFLOWCUM);
        m_CLProgram_Flow->PlaceArgument(12, T_CHMASK);
        m_CLProgram_Flow->PlaceArgument(13, T_CHWIDTH);
        m_CLProgram_Flow->PlaceArgument(14, T_CHHEIGHT);
        m_CLProgram_Flow->PlaceArgument(15, T_CHH);
        m_CLProgram_Flow->PlaceArgument(16, T_CHVX);
        m_CLProgram_Flow->PlaceArgument(17, T_HN);
        m_CLProgram_Flow->PlaceArgument(18, T_VXN);
        m_CLProgram_Flow->PlaceArgument(19, T_VYN);
        m_CLProgram_Flow->PlaceArgument(20, T_F_INFILACT);
        m_CLProgram_Flow->PlaceArgument(21, T_CHHN);
        m_CLProgram_Flow->PlaceArgument(22, T_CHVXN);
        m_CLProgram_Flow->PlaceArgument(23, T_UI);
        m_CLProgram_Flow->PlaceArgument(24, 0);
        m_CLProgram_Flow->PlaceArgument(25, 0.0);
        m_CLProgram_Flow->PlaceArgument(26, T_QFOUT);
        m_CLProgram_Flow->PlaceArgument(27, T_QFOUTN);
        m_CLProgram_Flow->PlaceArgument(28, T_DTREQ);
        m_CLProgram_Flow->PlaceArgument(29, T_QFX1);
        m_CLProgram_Flow->PlaceArgument(30, T_QFX2);
        m_CLProgram_Flow->PlaceArgument(31, T_QFY1);
        m_CLProgram_Flow->PlaceArgument(32, T_QFY2);
        m_CLProgram_Flow->PlaceArgument(33, T_QFCHX1);
        m_CLProgram_Flow->PlaceArgument(34, T_QFCHX2);
        m_CLProgram_Flow->PlaceArgument(35, T_QFCHIN);
        m_CLProgram_Flow->PlaceArgument(36, T_RAIN);
        m_CLProgram_Flow->PlaceArgument(37, T_N);
        m_CLProgram_Flow->PlaceArgument(38, T_BlockingX);
        m_CLProgram_Flow->PlaceArgument(39, T_BlockingY);
        m_CLProgram_Flow->PlaceArgument(40, T_BlockingFX);
        m_CLProgram_Flow->PlaceArgument(41, T_BlockingFY);
        m_CLProgram_Flow->PlaceArgument(42, T_BlockingVelX);
        m_CLProgram_Flow->PlaceArgument(43, T_BlockingVelY);
        m_CLProgram_Flow->PlaceArgument(44, T_BlockingLDD);
        m_CLProgram_Flow->PlaceArgument(45, T_HCorrect);


        //m_CLProgram_Flow2->SetRunDims((int)dim0, (int)dim1);
        m_CLProgram_Flow2->SetRunDims((int) m_NCells,0,0);

        m_CLProgram_Flow2->PlaceArgument(0, (int)dim0);
        m_CLProgram_Flow2->PlaceArgument(1, (int)dim1);
        m_CLProgram_Flow2->PlaceArgument(2, (float)(DEM->cellSize()));
        m_CLProgram_Flow2->PlaceArgument(3, (float)(0.5));
        m_CLProgram_Flow2->PlaceArgument(4, T_LOCR);
        m_CLProgram_Flow2->PlaceArgument(5, T_LOCC);
        m_CLProgram_Flow2->PlaceArgument(6, T_DEM);
        m_CLProgram_Flow2->PlaceArgument(7, T_H);
        m_CLProgram_Flow2->PlaceArgument(8, T_VX);
        m_CLProgram_Flow2->PlaceArgument(9, T_VY);
        m_CLProgram_Flow2->PlaceArgument(10, T_F_INFILFLOWCUM);
        m_CLProgram_Flow2->PlaceArgument(11, T_CHH);
        m_CLProgram_Flow2->PlaceArgument(12, T_CHVX);
        m_CLProgram_Flow2->PlaceArgument(13, T_HN);
        m_CLProgram_Flow2->PlaceArgument(14, T_VXN);
        m_CLProgram_Flow2->PlaceArgument(15, T_VYN);
        m_CLProgram_Flow2->PlaceArgument(16, T_F_INFILACT);
        m_CLProgram_Flow2->PlaceArgument(17, T_CHHN);
        m_CLProgram_Flow2->PlaceArgument(18, T_CHVXN);
        m_CLProgram_Flow2->PlaceArgument(19, T_HFMax);
        m_CLProgram_Flow2->PlaceArgument(20, T_VFMax);
        m_CLProgram_Flow2->PlaceArgument(21, T_HFMaxN);
        m_CLProgram_Flow2->PlaceArgument(22, T_VFMaxN);

        if(m_DoRigidWorld)
        {
            m_CLProgram_Rigid->SetRunDims((int)m_NCells,0,0);

            m_CLProgram_Rigid->PlaceArgument(0, (int)dim0);
            m_CLProgram_Rigid->PlaceArgument(1, (int)dim1);
            m_CLProgram_Rigid->PlaceArgument(2, (float)(DEM->cellSize()));
            m_CLProgram_Rigid->PlaceArgument(3, (float)(0.5));
            m_CLProgram_Rigid->PlaceArgument(4, T_LOCR);
            m_CLProgram_Rigid->PlaceArgument(5, T_LOCC);
            m_CLProgram_Rigid->PlaceArgument(6, T_DEM);
            m_CLProgram_Rigid->PlaceArgument(7, T_H);
            m_CLProgram_Rigid->PlaceArgument(8, T_BlockingX);
            m_CLProgram_Rigid->PlaceArgument(9, T_BlockingY);
            m_CLProgram_Rigid->PlaceArgument(10, T_UI);
            m_CLProgram_Rigid->PlaceArgument(11, 0);

            m_CLProgram_Rigid2->SetRunDims((int)m_NCells,0,0);

            m_CLProgram_Rigid2->PlaceArgument(0, (int)dim0);
            m_CLProgram_Rigid2->PlaceArgument(1, (int)dim1);
            m_CLProgram_Rigid2->PlaceArgument(2, (float)(DEM->cellSize()));
            m_CLProgram_Rigid2->PlaceArgument(3, (float)(0.5));
            m_CLProgram_Rigid2->PlaceArgument(4, T_LOCR);
            m_CLProgram_Rigid2->PlaceArgument(5, T_LOCC);
            m_CLProgram_Rigid2->PlaceArgument(6, T_DEM);
            m_CLProgram_Rigid2->PlaceArgument(7, T_H);
            m_CLProgram_Rigid2->PlaceArgument(8, T_BlockingX);
            m_CLProgram_Rigid2->PlaceArgument(9, T_BlockingY);
            m_CLProgram_Rigid2->PlaceArgument(10, T_UI);
            m_CLProgram_Rigid2->PlaceArgument(11, 0);
        }
        if(m_DoInfiltration && !m_DoHydrology)
        {

            m_CLProgram_Infiltration->SetRunDims((int)m_NCells,0,0);

            m_CLProgram_Infiltration->PlaceArgument(0, (int)dim0);
            m_CLProgram_Infiltration->PlaceArgument(1, (int)dim1);
            m_CLProgram_Infiltration->PlaceArgument(2, (float)(DEM->cellSize()));
            m_CLProgram_Infiltration->PlaceArgument(3, (float)(0.5));
            m_CLProgram_Infiltration->PlaceArgument(4, T_LOCR);
            m_CLProgram_Infiltration->PlaceArgument(5, T_LOCC);
            m_CLProgram_Infiltration->PlaceArgument(6, T_DEM);
            m_CLProgram_Infiltration->PlaceArgument(7, T_H);
            m_CLProgram_Infiltration->PlaceArgument(8, T_KSATTOPSIMPLE);
            m_CLProgram_Infiltration->PlaceArgument(9, T_INFIL);
            m_CLProgram_Infiltration->PlaceArgument(10, T_F_INFILACT);
            m_CLProgram_Infiltration->PlaceArgument(11, T_HN);
            m_CLProgram_Infiltration->PlaceArgument(12, T_GW_WFN);
            m_CLProgram_Infiltration->PlaceArgument(13, T_F_INFILPOT);
            m_CLProgram_Infiltration->PlaceArgument(14, T_F_INFILFLOWCUM);
            m_CLProgram_Infiltration->PlaceArgument(15, T_UI);
            m_CLProgram_Infiltration->PlaceArgument(16, 0);

            m_CLProgram_Infiltration2->SetRunDims((int)m_NCells,0,0);

            m_CLProgram_Infiltration2->PlaceArgument(0, (int)dim0);
            m_CLProgram_Infiltration2->PlaceArgument(1, (int)dim1);
            m_CLProgram_Infiltration2->PlaceArgument(2, (float)(DEM->cellSize()));
            m_CLProgram_Infiltration2->PlaceArgument(3, (float)(0.5));
            m_CLProgram_Infiltration2->PlaceArgument(4, T_LOCR);
            m_CLProgram_Infiltration2->PlaceArgument(5, T_LOCC);
            m_CLProgram_Infiltration2->PlaceArgument(6, T_H);
            m_CLProgram_Infiltration2->PlaceArgument(7, T_INFIL);
            m_CLProgram_Infiltration2->PlaceArgument(8, T_HN);
            m_CLProgram_Infiltration2->PlaceArgument(9, T_GW_WFN);
        }

        if(m_DoHydrology)
        {
            //m_CLProgram_Hydrology->SetRunDims((int)dim0, (int)dim1);
            m_CLProgram_Hydrology->SetRunDims((int)m_NCells,0,0);

            m_CLProgram_Hydrology->PlaceArgument(0, (int)dim0);
            m_CLProgram_Hydrology->PlaceArgument(1, (int)dim1);
            m_CLProgram_Hydrology->PlaceArgument(2, (float)(DEM->cellSize()));
            m_CLProgram_Hydrology->PlaceArgument(3, (float)(0.5));
            m_CLProgram_Hydrology->PlaceArgument(4, T_LOCR);
            m_CLProgram_Hydrology->PlaceArgument(5, T_LOCC);
            m_CLProgram_Hydrology->PlaceArgument(6, T_DEM);
            m_CLProgram_Hydrology->PlaceArgument(7, T_H);
            m_CLProgram_Hydrology->PlaceArgument(8, T_SCANOPY);
            m_CLProgram_Hydrology->PlaceArgument(9, T_SSURFACE);
            m_CLProgram_Hydrology->PlaceArgument(10, T_INFIL);
            m_CLProgram_Hydrology->PlaceArgument(11, T_F_INFILACT);
            m_CLProgram_Hydrology->PlaceArgument(12, T_GW_US);
            m_CLProgram_Hydrology->PlaceArgument(13, T_GW_S);
            m_CLProgram_Hydrology->PlaceArgument(14, T_THETAS);
            m_CLProgram_Hydrology->PlaceArgument(15, T_THETAR);
            m_CLProgram_Hydrology->PlaceArgument(16, T_SD);
            m_CLProgram_Hydrology->PlaceArgument(17, T_KSAT_T);
            m_CLProgram_Hydrology->PlaceArgument(18, T_KSAT_B);
            m_CLProgram_Hydrology->PlaceArgument(19, T_SWR_A);
            m_CLProgram_Hydrology->PlaceArgument(20, T_SWR_B);
            m_CLProgram_Hydrology->PlaceArgument(21, T_SMAX_CANOPY);
            m_CLProgram_Hydrology->PlaceArgument(22, T_SMAX_SURFACE);
            m_CLProgram_Hydrology->PlaceArgument(23, T_CHMASK);
            m_CLProgram_Hydrology->PlaceArgument(24, T_CHWIDTH);
            m_CLProgram_Hydrology->PlaceArgument(25, T_CHHEIGHT);
            m_CLProgram_Hydrology->PlaceArgument(26, T_CHH);
            m_CLProgram_Hydrology->PlaceArgument(27, T_HN);
            m_CLProgram_Hydrology->PlaceArgument(28, T_SCANOPYN);
            m_CLProgram_Hydrology->PlaceArgument(29, T_SSURFACEN);
            m_CLProgram_Hydrology->PlaceArgument(30, T_F_INFILPOT);
            m_CLProgram_Hydrology->PlaceArgument(31, T_GW_WFN);
            m_CLProgram_Hydrology->PlaceArgument(32, T_GW_USN);
            m_CLProgram_Hydrology->PlaceArgument(33, T_GW_SN);
            m_CLProgram_Hydrology->PlaceArgument(34, T_F_INFILFLOWCUM);
            m_CLProgram_Hydrology->PlaceArgument(35, T_CHHN);
            m_CLProgram_Hydrology->PlaceArgument(36, T_UI);
            m_CLProgram_Hydrology->PlaceArgument(37, 0);
            m_CLProgram_Hydrology->PlaceArgument(38, (float) 0.0);

            //m_CLProgram_Hydrology2->SetRunDims((int)dim0, (int)dim1);
            m_CLProgram_Hydrology2->SetRunDims((int)m_NCells,0,0);

            m_CLProgram_Hydrology2->PlaceArgument(0, (int)dim0);
            m_CLProgram_Hydrology2->PlaceArgument(1, (int)dim1);
            m_CLProgram_Hydrology2->PlaceArgument(2, (float)(DEM->cellSize()));
            m_CLProgram_Hydrology2->PlaceArgument(3, (float)(0.5));
            m_CLProgram_Hydrology2->PlaceArgument(4, T_LOCR);
            m_CLProgram_Hydrology2->PlaceArgument(5, T_LOCC);
            m_CLProgram_Hydrology2->PlaceArgument(6, T_H);
            m_CLProgram_Hydrology2->PlaceArgument(7, T_SCANOPY);
            m_CLProgram_Hydrology2->PlaceArgument(8, T_SSURFACE);
            m_CLProgram_Hydrology2->PlaceArgument(9, T_INFIL);
            m_CLProgram_Hydrology2->PlaceArgument(10, T_GW_US);
            m_CLProgram_Hydrology2->PlaceArgument(11, T_GW_S);
            m_CLProgram_Hydrology2->PlaceArgument(12, T_CHH);
            m_CLProgram_Hydrology2->PlaceArgument(13, T_HN);
            m_CLProgram_Hydrology2->PlaceArgument(14, T_SCANOPYN);
            m_CLProgram_Hydrology2->PlaceArgument(15, T_SSURFACEN);
            m_CLProgram_Hydrology2->PlaceArgument(16, T_GW_WFN);
            m_CLProgram_Hydrology2->PlaceArgument(17, T_GW_USN);
            m_CLProgram_Hydrology2->PlaceArgument(18, T_GW_SN);
            m_CLProgram_Hydrology2->PlaceArgument(19, T_CHHN);

        }

        if(m_DoEvapoTranspiration)
        {
            //m_CLProgram_Hydrology->SetRunDims((int)dim0, (int)dim1);
            m_CLProgram_Evapotranspiration->SetRunDims((int)m_NCells,0,0);

            m_CLProgram_Evapotranspiration->PlaceArgument(0, (int)dim0);
            m_CLProgram_Evapotranspiration->PlaceArgument(1, (int)dim1);
            m_CLProgram_Evapotranspiration->PlaceArgument(2, (float)(DEM->cellSize()));
            m_CLProgram_Evapotranspiration->PlaceArgument(3, (float)(0.5));
            m_CLProgram_Evapotranspiration->PlaceArgument(4, T_LOCR);
            m_CLProgram_Evapotranspiration->PlaceArgument(5, T_LOCC);
            m_CLProgram_Evapotranspiration->PlaceArgument(6, T_DEM);
            m_CLProgram_Evapotranspiration->PlaceArgument(7, T_H);
            m_CLProgram_Evapotranspiration->PlaceArgument(8, T_SCANOPY);
            m_CLProgram_Evapotranspiration->PlaceArgument(9, T_SSURFACE);
            m_CLProgram_Evapotranspiration->PlaceArgument(10, T_INFIL);
            m_CLProgram_Evapotranspiration->PlaceArgument(11, T_F_INFILACT);
            m_CLProgram_Evapotranspiration->PlaceArgument(12, T_GW_US);
            m_CLProgram_Evapotranspiration->PlaceArgument(13, T_GW_S);
            m_CLProgram_Evapotranspiration->PlaceArgument(14, T_THETAS);
            m_CLProgram_Evapotranspiration->PlaceArgument(15, T_THETAR);
            m_CLProgram_Evapotranspiration->PlaceArgument(16, T_SD);
            m_CLProgram_Evapotranspiration->PlaceArgument(17, T_KSAT_T);
            m_CLProgram_Evapotranspiration->PlaceArgument(18, T_KSAT_B);
            m_CLProgram_Evapotranspiration->PlaceArgument(19, T_SWR_A);
            m_CLProgram_Evapotranspiration->PlaceArgument(20, T_SWR_B);
            m_CLProgram_Evapotranspiration->PlaceArgument(21, T_SMAX_CANOPY);
            m_CLProgram_Evapotranspiration->PlaceArgument(22, T_SMAX_SURFACE);
            m_CLProgram_Evapotranspiration->PlaceArgument(23, T_CHMASK);
            m_CLProgram_Evapotranspiration->PlaceArgument(24, T_CHWIDTH);
            m_CLProgram_Evapotranspiration->PlaceArgument(25, T_CHHEIGHT);
            m_CLProgram_Evapotranspiration->PlaceArgument(26, T_CHH);
            m_CLProgram_Evapotranspiration->PlaceArgument(27, T_HN);
            m_CLProgram_Evapotranspiration->PlaceArgument(28, T_SCANOPYN);
            m_CLProgram_Evapotranspiration->PlaceArgument(29, T_SSURFACEN);
            m_CLProgram_Evapotranspiration->PlaceArgument(30, T_F_INFILPOT);
            m_CLProgram_Evapotranspiration->PlaceArgument(31, T_GW_WFN);
            m_CLProgram_Evapotranspiration->PlaceArgument(32, T_GW_USN);
            m_CLProgram_Evapotranspiration->PlaceArgument(33, T_GW_SN);
            m_CLProgram_Evapotranspiration->PlaceArgument(34, T_F_INFILFLOWCUM);
            m_CLProgram_Evapotranspiration->PlaceArgument(35, T_CHHN);
            m_CLProgram_Evapotranspiration->PlaceArgument(36, T_UI);
            m_CLProgram_Evapotranspiration->PlaceArgument(37, T_TEMP);
            m_CLProgram_Evapotranspiration->PlaceArgument(38, T_WIND);
            m_CLProgram_Evapotranspiration->PlaceArgument(39, T_VAPR);
            m_CLProgram_Evapotranspiration->PlaceArgument(40, T_RAD);
            m_CLProgram_Evapotranspiration->PlaceArgument(41, T_NDVI);
            m_CLProgram_Evapotranspiration->PlaceArgument(42, T_CROPF);
            m_CLProgram_Evapotranspiration->PlaceArgument(43, 0);

            //m_CLProgram_Evapotranspiration2->SetRunDims((int)dim0, (int)dim1);
            m_CLProgram_Evapotranspiration2->SetRunDims((int)m_NCells,0,0);

            m_CLProgram_Evapotranspiration2->PlaceArgument(0, (int)dim0);
            m_CLProgram_Evapotranspiration2->PlaceArgument(1, (int)dim1);
            m_CLProgram_Evapotranspiration2->PlaceArgument(2, (float)(DEM->cellSize()));
            m_CLProgram_Evapotranspiration2->PlaceArgument(3, (float)(0.5));
            m_CLProgram_Evapotranspiration2->PlaceArgument(4, T_LOCR);
            m_CLProgram_Evapotranspiration2->PlaceArgument(5, T_LOCC);
            m_CLProgram_Evapotranspiration2->PlaceArgument(6, T_H);
            m_CLProgram_Evapotranspiration2->PlaceArgument(7, T_SCANOPY);
            m_CLProgram_Evapotranspiration2->PlaceArgument(8, T_SSURFACE);
            m_CLProgram_Evapotranspiration2->PlaceArgument(9, T_INFIL);
            m_CLProgram_Evapotranspiration2->PlaceArgument(10, T_GW_US);
            m_CLProgram_Evapotranspiration2->PlaceArgument(11, T_GW_S);
            m_CLProgram_Evapotranspiration2->PlaceArgument(12, T_CHH);
            m_CLProgram_Evapotranspiration2->PlaceArgument(13, T_HN);
            m_CLProgram_Evapotranspiration2->PlaceArgument(14, T_SCANOPYN);
            m_CLProgram_Evapotranspiration2->PlaceArgument(15, T_SSURFACEN);
            m_CLProgram_Evapotranspiration2->PlaceArgument(16, T_GW_WFN);
            m_CLProgram_Evapotranspiration2->PlaceArgument(17, T_GW_USN);
            m_CLProgram_Evapotranspiration2->PlaceArgument(18, T_GW_SN);
            m_CLProgram_Evapotranspiration2->PlaceArgument(19, T_CHHN);

        }

        if(m_DoHydrology && m_DoGroundWater)
        {
            m_CLProgram_GroundWaterFlow->SetRunDims((int)m_NCells,0,0);
            m_CLProgram_GroundWaterFlow->PlaceArgument(0, (int)dim0);
            m_CLProgram_GroundWaterFlow->PlaceArgument(1, (int)dim1);
            m_CLProgram_GroundWaterFlow->PlaceArgument(2, (float)(DEM->cellSize()));
            m_CLProgram_GroundWaterFlow->PlaceArgument(3, (float)(0.5));
            m_CLProgram_GroundWaterFlow->PlaceArgument(4, T_LOCR);
            m_CLProgram_GroundWaterFlow->PlaceArgument(5, T_LOCC);
            m_CLProgram_GroundWaterFlow->PlaceArgument(6, T_DEM);
            m_CLProgram_GroundWaterFlow->PlaceArgument(7, T_INFIL);
            m_CLProgram_GroundWaterFlow->PlaceArgument(8, T_GW_US);
            m_CLProgram_GroundWaterFlow->PlaceArgument(9, T_GW_S);
            m_CLProgram_GroundWaterFlow->PlaceArgument(10, T_THETAS);
            m_CLProgram_GroundWaterFlow->PlaceArgument(11, T_THETAR);
            m_CLProgram_GroundWaterFlow->PlaceArgument(12, T_SD);
            m_CLProgram_GroundWaterFlow->PlaceArgument(13, T_KSAT_B);
            m_CLProgram_GroundWaterFlow->PlaceArgument(14, T_GW_SN);

            m_CLProgram_GroundWaterFlow2->SetRunDims((int)m_NCells,0,0);
            m_CLProgram_GroundWaterFlow2->PlaceArgument(0, (int)dim0);
            m_CLProgram_GroundWaterFlow2->PlaceArgument(1, (int)dim1);
            m_CLProgram_GroundWaterFlow2->PlaceArgument(2, (float)(DEM->cellSize()));
            m_CLProgram_GroundWaterFlow2->PlaceArgument(3, (float)(0.5));
            m_CLProgram_GroundWaterFlow2->PlaceArgument(4, T_LOCR);
            m_CLProgram_GroundWaterFlow2->PlaceArgument(5, T_LOCC);
            m_CLProgram_GroundWaterFlow2->PlaceArgument(6, T_DEM);
            m_CLProgram_GroundWaterFlow2->PlaceArgument(7, T_GW_S);
            m_CLProgram_GroundWaterFlow2->PlaceArgument(8, T_GW_SN);
        }

        if(((m_DoHydrology && m_DoErosion) || (((m_DoHydrology && m_DoSlopeFailure) || m_DoInitialSolids) && m_DoEntrainment)))
        {

            m_CLProgram_Erosion->SetRunDims((int)m_NCells,0,0);
            m_CLProgram_Erosion->PlaceArgument(0, (int)dim0);
            m_CLProgram_Erosion->PlaceArgument(1, (int)dim1);
            m_CLProgram_Erosion->PlaceArgument(2, (float)(DEM->cellSize()));
            m_CLProgram_Erosion->PlaceArgument(3, (float)(0.5));
            m_CLProgram_Erosion->PlaceArgument(4, T_LOCR);
            m_CLProgram_Erosion->PlaceArgument(5, T_LOCC);
            m_CLProgram_Erosion->PlaceArgument(6, T_DEM);
            m_CLProgram_Erosion->PlaceArgument(7, T_H);
            m_CLProgram_Erosion->PlaceArgument(8, T_VX);
            m_CLProgram_Erosion->PlaceArgument(9, T_VY);
            m_CLProgram_Erosion->PlaceArgument(10, T_HS);
            m_CLProgram_Erosion->PlaceArgument(11, T_VSX);
            m_CLProgram_Erosion->PlaceArgument(12, T_VSY);
            m_CLProgram_Erosion->PlaceArgument(13, T_SIfa);
            m_CLProgram_Erosion->PlaceArgument(14, T_SRockSize);
            m_CLProgram_Erosion->PlaceArgument(15, T_SDensity);
            m_CLProgram_Erosion->PlaceArgument(16, T_CHH);
            m_CLProgram_Erosion->PlaceArgument(17, T_CHVX);
            m_CLProgram_Erosion->PlaceArgument(18, T_CHHS);
            m_CLProgram_Erosion->PlaceArgument(19, T_CHVS);
            m_CLProgram_Erosion->PlaceArgument(20, T_CHSIfa);
            m_CLProgram_Erosion->PlaceArgument(21, T_CHSRockSize);
            m_CLProgram_Erosion->PlaceArgument(22, T_CHSDensity);
            m_CLProgram_Erosion->PlaceArgument(23, T_CHMASK);
            m_CLProgram_Erosion->PlaceArgument(24, T_CHWIDTH);
            m_CLProgram_Erosion->PlaceArgument(25, T_CHHEIGHT);
            m_CLProgram_Erosion->PlaceArgument(26, T_D50);
            m_CLProgram_Erosion->PlaceArgument(27, T_D90);
            m_CLProgram_Erosion->PlaceArgument(28, T_COH);
            m_CLProgram_Erosion->PlaceArgument(29, T_CHCOH);
            m_CLProgram_Erosion->PlaceArgument(30, T_Y);
            m_CLProgram_Erosion->PlaceArgument(31, T_SD);
            m_CLProgram_Erosion->PlaceArgument(32, T_MD);
            m_CLProgram_Erosion->PlaceArgument(33, T_MIFA);
            m_CLProgram_Erosion->PlaceArgument(34, T_MROCKSIZE);
            m_CLProgram_Erosion->PlaceArgument(35, T_MDENSITY);
            m_CLProgram_Erosion->PlaceArgument(36, T_SED);
            m_CLProgram_Erosion->PlaceArgument(37, T_CHSED);
            m_CLProgram_Erosion->PlaceArgument(38, T_DETTOT);
            m_CLProgram_Erosion->PlaceArgument(39, T_DEPTOT);
            m_CLProgram_Erosion->PlaceArgument(40, T_INFIL);
            m_CLProgram_Erosion->PlaceArgument(41, T_GW_US);
            m_CLProgram_Erosion->PlaceArgument(42, T_GW_S);
            m_CLProgram_Erosion->PlaceArgument(43, T_ENTRAINMENTTOT);
            m_CLProgram_Erosion->PlaceArgument(44, T_GW_WFN);
            m_CLProgram_Erosion->PlaceArgument(45, T_GW_USN);
            m_CLProgram_Erosion->PlaceArgument(46, T_GW_SN);
            m_CLProgram_Erosion->PlaceArgument(47, T_SEDN);
            m_CLProgram_Erosion->PlaceArgument(48, T_CHSEDN);
            m_CLProgram_Erosion->PlaceArgument(49, T_DETTOTN);
            m_CLProgram_Erosion->PlaceArgument(50, T_DEPTOTN);
            m_CLProgram_Erosion->PlaceArgument(51, T_SDN);
            m_CLProgram_Erosion->PlaceArgument(52, T_MDN);
            m_CLProgram_Erosion->PlaceArgument(53, T_ENTRAINMENT);
            m_CLProgram_Erosion->PlaceArgument(54, T_ENTRAINMENTCH);
            m_CLProgram_Erosion->PlaceArgument(55, T_ENTRAINMENTTOTN);
            m_CLProgram_Erosion->PlaceArgument(56, T_DEMN);
            m_CLProgram_Erosion->PlaceArgument(57, T_QSEDOUT);
            m_CLProgram_Erosion->PlaceArgument(58, T_QSEDOUTN);
            m_CLProgram_Erosion->PlaceArgument(59, T_UI);
            m_CLProgram_Erosion->PlaceArgument(60, 0);
            m_CLProgram_Erosion->PlaceArgument(61, m_DoParticleSolids? 0:1);
            m_CLProgram_Erosion->PlaceArgument(62, T_HSN);
            m_CLProgram_Erosion->PlaceArgument(63, T_VSXN);
            m_CLProgram_Erosion->PlaceArgument(64, T_VSYN);
            m_CLProgram_Erosion->PlaceArgument(65, T_SIfaN);
            m_CLProgram_Erosion->PlaceArgument(66, T_SRockSizeN);
            m_CLProgram_Erosion->PlaceArgument(67, T_SDensityN);
            m_CLProgram_Erosion->PlaceArgument(68, T_CHHSN);
            m_CLProgram_Erosion->PlaceArgument(69, T_CHVSN);
            m_CLProgram_Erosion->PlaceArgument(70, T_CHSIfaN);
            m_CLProgram_Erosion->PlaceArgument(71, T_CHSRockSizeN);
            m_CLProgram_Erosion->PlaceArgument(72, T_CHSDensityN);
            m_CLProgram_Erosion->PlaceArgument(73, T_N);
            m_CLProgram_Erosion->PlaceArgument(74, T_QFX1);
            m_CLProgram_Erosion->PlaceArgument(75, T_QFX2);
            m_CLProgram_Erosion->PlaceArgument(76, T_QFY1);
            m_CLProgram_Erosion->PlaceArgument(77, T_QFY2);
            m_CLProgram_Erosion->PlaceArgument(78, T_QFCHX1);
            m_CLProgram_Erosion->PlaceArgument(79, T_QFCHX2);
            m_CLProgram_Erosion->PlaceArgument(80, T_QFCHIN);
            m_CLProgram_Erosion->PlaceArgument(81, 0.0f);
            m_CLProgram_Erosion->PlaceArgument(82, T_HARDCOVER);
            m_CLProgram_Erosion->PlaceArgument(83, T_VEGCOVER);
            m_CLProgram_Erosion->PlaceArgument(84, T_VEGH);
            m_CLProgram_Erosion->PlaceArgument(85, (m_DoHydrology && m_DoErosion)? 1:0);
            m_CLProgram_Erosion->PlaceArgument(86, T_HN);
            m_CLProgram_Erosion->PlaceArgument(87, T_VXN);
            m_CLProgram_Erosion->PlaceArgument(88, T_VYN);

            m_CLProgram_Erosion2->SetRunDims((int)m_NCells,0,0);
            m_CLProgram_Erosion2->PlaceArgument(0, (int)dim0);
            m_CLProgram_Erosion2->PlaceArgument(1, (int)dim1);
            m_CLProgram_Erosion2->PlaceArgument(2, (float)(DEM->cellSize()));
            m_CLProgram_Erosion2->PlaceArgument(3, (float)(0.5));
            m_CLProgram_Erosion2->PlaceArgument(4, T_LOCR);
            m_CLProgram_Erosion2->PlaceArgument(5, T_LOCC);
            m_CLProgram_Erosion2->PlaceArgument(6, T_DEM);
            m_CLProgram_Erosion2->PlaceArgument(7, T_H);
            m_CLProgram_Erosion2->PlaceArgument(8, T_VX);
            m_CLProgram_Erosion2->PlaceArgument(9, T_VY);
            m_CLProgram_Erosion2->PlaceArgument(10, T_HS);
            m_CLProgram_Erosion2->PlaceArgument(11, T_VSX);
            m_CLProgram_Erosion2->PlaceArgument(12, T_VSY);
            m_CLProgram_Erosion2->PlaceArgument(13, T_SIfa);
            m_CLProgram_Erosion2->PlaceArgument(14, T_SRockSize);
            m_CLProgram_Erosion2->PlaceArgument(15, T_SDensity);
            m_CLProgram_Erosion2->PlaceArgument(16, T_CHH);
            m_CLProgram_Erosion2->PlaceArgument(17, T_CHVX);
            m_CLProgram_Erosion2->PlaceArgument(18, T_CHHS);
            m_CLProgram_Erosion2->PlaceArgument(19, T_CHVS);
            m_CLProgram_Erosion2->PlaceArgument(20, T_CHSIfa);
            m_CLProgram_Erosion2->PlaceArgument(21, T_CHSRockSize);
            m_CLProgram_Erosion2->PlaceArgument(22, T_CHSDensity);
            m_CLProgram_Erosion2->PlaceArgument(23, T_CHMASK);
            m_CLProgram_Erosion2->PlaceArgument(24, T_CHWIDTH);
            m_CLProgram_Erosion2->PlaceArgument(25, T_CHHEIGHT);
            m_CLProgram_Erosion2->PlaceArgument(26, T_D50);
            m_CLProgram_Erosion2->PlaceArgument(27, T_D90);
            m_CLProgram_Erosion2->PlaceArgument(28, T_COH);
            m_CLProgram_Erosion2->PlaceArgument(29, T_CHCOH);
            m_CLProgram_Erosion2->PlaceArgument(30, T_Y);
            m_CLProgram_Erosion2->PlaceArgument(31, T_SD);
            m_CLProgram_Erosion2->PlaceArgument(32, T_MD);
            m_CLProgram_Erosion2->PlaceArgument(33, T_MIFA);
            m_CLProgram_Erosion2->PlaceArgument(34, T_MROCKSIZE);
            m_CLProgram_Erosion2->PlaceArgument(35, T_MDENSITY);
            m_CLProgram_Erosion2->PlaceArgument(36, T_SED);
            m_CLProgram_Erosion2->PlaceArgument(37, T_CHSED);
            m_CLProgram_Erosion2->PlaceArgument(38, T_DETTOT);
            m_CLProgram_Erosion2->PlaceArgument(39, T_DEPTOT);
            m_CLProgram_Erosion2->PlaceArgument(40, T_INFIL);
            m_CLProgram_Erosion2->PlaceArgument(41, T_GW_US);
            m_CLProgram_Erosion2->PlaceArgument(42, T_GW_S);
            m_CLProgram_Erosion2->PlaceArgument(43, T_ENTRAINMENTTOT);
            m_CLProgram_Erosion2->PlaceArgument(44, T_GW_WFN);
            m_CLProgram_Erosion2->PlaceArgument(45, T_GW_USN);
            m_CLProgram_Erosion2->PlaceArgument(46, T_GW_SN);
            m_CLProgram_Erosion2->PlaceArgument(47, T_SEDN);
            m_CLProgram_Erosion2->PlaceArgument(48, T_CHSEDN);
            m_CLProgram_Erosion2->PlaceArgument(49, T_DETTOTN);
            m_CLProgram_Erosion2->PlaceArgument(50, T_DEPTOTN);
            m_CLProgram_Erosion2->PlaceArgument(51, T_SDN);
            m_CLProgram_Erosion2->PlaceArgument(52, T_MDN);
            m_CLProgram_Erosion2->PlaceArgument(53, T_ENTRAINMENT);
            m_CLProgram_Erosion2->PlaceArgument(54, T_ENTRAINMENTCH);
            m_CLProgram_Erosion2->PlaceArgument(55, T_ENTRAINMENTTOTN);
            m_CLProgram_Erosion2->PlaceArgument(56, T_DEMN);
            m_CLProgram_Erosion2->PlaceArgument(57, T_QSEDOUT);
            m_CLProgram_Erosion2->PlaceArgument(58, T_QSEDOUTN);
            m_CLProgram_Erosion2->PlaceArgument(59, T_UI);
            m_CLProgram_Erosion2->PlaceArgument(60, 0);
            m_CLProgram_Erosion2->PlaceArgument(61, 0);
            m_CLProgram_Erosion2->PlaceArgument(62, T_HSN);
            m_CLProgram_Erosion2->PlaceArgument(63, T_VSXN);
            m_CLProgram_Erosion2->PlaceArgument(64, T_VSYN);
            m_CLProgram_Erosion2->PlaceArgument(65, T_SIfaN);
            m_CLProgram_Erosion2->PlaceArgument(66, T_SRockSizeN);
            m_CLProgram_Erosion2->PlaceArgument(67, T_SDensityN);
            m_CLProgram_Erosion2->PlaceArgument(68, T_CHHSN);
            m_CLProgram_Erosion2->PlaceArgument(69, T_CHVSN);
            m_CLProgram_Erosion2->PlaceArgument(70, T_CHSIfaN);
            m_CLProgram_Erosion2->PlaceArgument(71, T_CHSRockSizeN);
            m_CLProgram_Erosion2->PlaceArgument(72, T_CHSDensityN);
            m_CLProgram_Erosion2->PlaceArgument(73, T_HN);
            m_CLProgram_Erosion2->PlaceArgument(74, T_VXN);
            m_CLProgram_Erosion2->PlaceArgument(75, T_VYN);

        }


        if(m_DoHydrology && m_DoSlopeStability)
        {

            if(m_DoSeismic || m_DoImprovedIterative)
            {
                m_CLProgram_SlopeStabilityE->SetRunDims((int)m_NCells,0,0);

                m_CLProgram_SlopeStabilityE->PlaceArgument(0, (int)dim0);
                m_CLProgram_SlopeStabilityE->PlaceArgument(1, (int)dim1);
                m_CLProgram_SlopeStabilityE->PlaceArgument(2, (float)(DEM->cellSize()));
                m_CLProgram_SlopeStabilityE->PlaceArgument(3, (float)(0.5));
                m_CLProgram_SlopeStabilityE->PlaceArgument(4, T_LOCR);
                m_CLProgram_SlopeStabilityE->PlaceArgument(5, T_LOCC);
                m_CLProgram_SlopeStabilityE->PlaceArgument(6, T_DEM);
                m_CLProgram_SlopeStabilityE->PlaceArgument(7, T_COHESION);
                m_CLProgram_SlopeStabilityE->PlaceArgument(8, T_IFA);
                m_CLProgram_SlopeStabilityE->PlaceArgument(9, T_DENSITY_BOTTOM);
                m_CLProgram_SlopeStabilityE->PlaceArgument(10, T_INFIL);
                m_CLProgram_SlopeStabilityE->PlaceArgument(11, T_GW_US);
                m_CLProgram_SlopeStabilityE->PlaceArgument(12, T_GW_S);
                m_CLProgram_SlopeStabilityE->PlaceArgument(13, T_THETAS);
                m_CLProgram_SlopeStabilityE->PlaceArgument(14, T_THETAR);
                m_CLProgram_SlopeStabilityE->PlaceArgument(15, T_SD);
                m_CLProgram_SlopeStabilityE->PlaceArgument(16, T_PGA);
                m_CLProgram_SlopeStabilityE->PlaceArgument(17, T_SFFX);
                m_CLProgram_SlopeStabilityE->PlaceArgument(18, T_SFFY);
                m_CLProgram_SlopeStabilityE->PlaceArgument(19, T_SF);
                m_CLProgram_SlopeStabilityE->PlaceArgument(20, T_SFC);
                m_CLProgram_SlopeStabilityE->PlaceArgument(21, 0);
                m_CLProgram_SlopeStabilityE->PlaceArgument(22, T_UI);
                m_CLProgram_SlopeStabilityE->PlaceArgument(23, 0);
                m_CLProgram_SlopeStabilityE->PlaceArgument(24, (float)m_InitialStabMargin);

            }else
            {
                m_CLProgram_SlopeStability->SetRunDims((int)m_NCells,0,0);

                m_CLProgram_SlopeStability->PlaceArgument(0, (int)dim0);
                m_CLProgram_SlopeStability->PlaceArgument(1, (int)dim1);
                m_CLProgram_SlopeStability->PlaceArgument(2, (float)(DEM->cellSize()));
                m_CLProgram_SlopeStability->PlaceArgument(3, (float)(0.5));
                m_CLProgram_SlopeStability->PlaceArgument(4, T_LOCR);
                m_CLProgram_SlopeStability->PlaceArgument(5, T_LOCC);
                m_CLProgram_SlopeStability->PlaceArgument(6, T_DEM);
                m_CLProgram_SlopeStability->PlaceArgument(7, T_COHESION);
                m_CLProgram_SlopeStability->PlaceArgument(8, T_IFA);
                m_CLProgram_SlopeStability->PlaceArgument(9, T_DENSITY_BOTTOM);
                m_CLProgram_SlopeStability->PlaceArgument(10, T_INFIL);
                m_CLProgram_SlopeStability->PlaceArgument(11, T_GW_US);
                m_CLProgram_SlopeStability->PlaceArgument(12, T_GW_S);
                m_CLProgram_SlopeStability->PlaceArgument(13, T_THETAS);
                m_CLProgram_SlopeStability->PlaceArgument(14, T_THETAR);
                m_CLProgram_SlopeStability->PlaceArgument(15, T_SD);
                m_CLProgram_SlopeStability->PlaceArgument(16, T_SF);
                m_CLProgram_SlopeStability->PlaceArgument(17, T_SFC);
                m_CLProgram_SlopeStability->PlaceArgument(18, 0);
                m_CLProgram_SlopeStability->PlaceArgument(19, T_UI);
                m_CLProgram_SlopeStability->PlaceArgument(20, 0);
                m_CLProgram_SlopeStability->PlaceArgument(21, (float)m_InitialStabMargin);
            }
        }

        if(m_DoHydrology && m_DoSlopeStability && m_DoSlopeFailure)
        {
            if(m_DoSeismic || m_DoImprovedIterative)
            {
                m_CLProgram_SlopeFailureE->SetRunDims((int)m_NCells,0,0);

                m_CLProgram_SlopeFailureE->PlaceArgument(0, (int)dim0);
                m_CLProgram_SlopeFailureE->PlaceArgument(1, (int)dim1);
                m_CLProgram_SlopeFailureE->PlaceArgument(2, (float)(DEM->cellSize()));
                m_CLProgram_SlopeFailureE->PlaceArgument(3, (float)(0.5));
                m_CLProgram_SlopeFailureE->PlaceArgument(4, T_LOCR);
                m_CLProgram_SlopeFailureE->PlaceArgument(5, T_LOCC);
                m_CLProgram_SlopeFailureE->PlaceArgument(6, T_DEM);
                m_CLProgram_SlopeFailureE->PlaceArgument(7, T_COHESION);
                m_CLProgram_SlopeFailureE->PlaceArgument(8, T_IFA);
                m_CLProgram_SlopeFailureE->PlaceArgument(9, T_DENSITY_BOTTOM);
                m_CLProgram_SlopeFailureE->PlaceArgument(10, T_INFIL);
                m_CLProgram_SlopeFailureE->PlaceArgument(11, T_GW_US);
                m_CLProgram_SlopeFailureE->PlaceArgument(12, T_GW_S);
                m_CLProgram_SlopeFailureE->PlaceArgument(13, T_THETAS);
                m_CLProgram_SlopeFailureE->PlaceArgument(14, T_THETAR);
                m_CLProgram_SlopeFailureE->PlaceArgument(15, T_SD);
                m_CLProgram_SlopeFailureE->PlaceArgument(16, T_H);
                m_CLProgram_SlopeFailureE->PlaceArgument(17, T_HS);
                m_CLProgram_SlopeFailureE->PlaceArgument(18, T_SIfa);
                m_CLProgram_SlopeFailureE->PlaceArgument(19, T_SDensity);
                m_CLProgram_SlopeFailureE->PlaceArgument(20, T_SRockSize);
                m_CLProgram_SlopeFailureE->PlaceArgument(21, T_SFC);
                m_CLProgram_SlopeFailureE->PlaceArgument(22, T_PGA);
                m_CLProgram_SlopeFailureE->PlaceArgument(23, T_SFFX);
                m_CLProgram_SlopeFailureE->PlaceArgument(24, T_SFFY);
                m_CLProgram_SlopeFailureE->PlaceArgument(25, T_SFH);
                m_CLProgram_SlopeFailureE->PlaceArgument(26, T_SDN);
                m_CLProgram_SlopeFailureE->PlaceArgument(27, T_HN);
                m_CLProgram_SlopeFailureE->PlaceArgument(28, T_HSN);
                m_CLProgram_SlopeFailureE->PlaceArgument(29, T_SIfaN);
                m_CLProgram_SlopeFailureE->PlaceArgument(30, T_SDensityN);
                m_CLProgram_SlopeFailureE->PlaceArgument(31, T_SRockSizeN);
                m_CLProgram_SlopeFailureE->PlaceArgument(32, T_GW_WFN);
                m_CLProgram_SlopeFailureE->PlaceArgument(33, T_GW_USN);
                m_CLProgram_SlopeFailureE->PlaceArgument(34, T_GW_SN);
                m_CLProgram_SlopeFailureE->PlaceArgument(35, T_DEMN );
                m_CLProgram_SlopeFailureE->PlaceArgument(36, T_UI);
                m_CLProgram_SlopeFailureE->PlaceArgument(37, 0);
                m_CLProgram_SlopeFailureE->PlaceArgument(38, 0);
            }else
            {
                m_CLProgram_SlopeFailure->SetRunDims((int)m_NCells,0,0);

                m_CLProgram_SlopeFailure->PlaceArgument(0, (int)dim0);
                m_CLProgram_SlopeFailure->PlaceArgument(1, (int)dim1);
                m_CLProgram_SlopeFailure->PlaceArgument(2, (float)(DEM->cellSize()));
                m_CLProgram_SlopeFailure->PlaceArgument(3, (float)(0.5));
                m_CLProgram_SlopeFailure->PlaceArgument(4, T_LOCR);
                m_CLProgram_SlopeFailure->PlaceArgument(5, T_LOCC);
                m_CLProgram_SlopeFailure->PlaceArgument(6, T_DEM);
                m_CLProgram_SlopeFailure->PlaceArgument(7, T_COHESION);
                m_CLProgram_SlopeFailure->PlaceArgument(8, T_IFA);
                m_CLProgram_SlopeFailure->PlaceArgument(9, T_DENSITY_BOTTOM);
                m_CLProgram_SlopeFailure->PlaceArgument(10, T_INFIL);
                m_CLProgram_SlopeFailure->PlaceArgument(11, T_GW_US);
                m_CLProgram_SlopeFailure->PlaceArgument(12, T_GW_S);
                m_CLProgram_SlopeFailure->PlaceArgument(13, T_THETAS);
                m_CLProgram_SlopeFailure->PlaceArgument(14, T_THETAR);
                m_CLProgram_SlopeFailure->PlaceArgument(15, T_SD);
                m_CLProgram_SlopeFailure->PlaceArgument(16, T_H);
                m_CLProgram_SlopeFailure->PlaceArgument(17, T_HS);
                m_CLProgram_SlopeFailure->PlaceArgument(18, T_SIfa);
                m_CLProgram_SlopeFailure->PlaceArgument(19, T_SDensity);
                m_CLProgram_SlopeFailure->PlaceArgument(20, T_SRockSize);
                m_CLProgram_SlopeFailure->PlaceArgument(21, T_SFC);
                m_CLProgram_SlopeFailure->PlaceArgument(22, T_SFH);
                m_CLProgram_SlopeFailure->PlaceArgument(23, T_SDN);
                m_CLProgram_SlopeFailure->PlaceArgument(24, T_HN);
                m_CLProgram_SlopeFailure->PlaceArgument(25, T_HSN);
                m_CLProgram_SlopeFailure->PlaceArgument(26, T_SIfaN);
                m_CLProgram_SlopeFailure->PlaceArgument(27, T_SDensityN);
                m_CLProgram_SlopeFailure->PlaceArgument(28, T_SRockSizeN);
                m_CLProgram_SlopeFailure->PlaceArgument(29, T_GW_WFN);
                m_CLProgram_SlopeFailure->PlaceArgument(30, T_GW_USN);
                m_CLProgram_SlopeFailure->PlaceArgument(31, T_GW_SN);
                m_CLProgram_SlopeFailure->PlaceArgument(32, T_DEMN );
                m_CLProgram_SlopeFailure->PlaceArgument(33, T_UI);
                m_CLProgram_SlopeFailure->PlaceArgument(34, 0);
                m_CLProgram_SlopeFailure->PlaceArgument(35, 0);

            }


            if(!m_DoParticleSolids)
            {
                m_CLProgram_SlopeFailure2->SetRunDims((int)m_NCells,0,0);
                m_CLProgram_SlopeFailure2->PlaceArgument(0, (int)dim0);
                m_CLProgram_SlopeFailure2->PlaceArgument(1, (int)dim1);
                m_CLProgram_SlopeFailure2->PlaceArgument(2, (float)(DEM->cellSize()));
                m_CLProgram_SlopeFailure2->PlaceArgument(3, (float)(0.5));
                m_CLProgram_SlopeFailure2->PlaceArgument(4, T_LOCR);
                m_CLProgram_SlopeFailure2->PlaceArgument(5, T_LOCC);
                m_CLProgram_SlopeFailure2->PlaceArgument(6, T_SFH);
                m_CLProgram_SlopeFailure2->PlaceArgument(7, T_INFIL);
                m_CLProgram_SlopeFailure2->PlaceArgument(8, T_GW_US);
                m_CLProgram_SlopeFailure2->PlaceArgument(9, T_GW_S);
                m_CLProgram_SlopeFailure2->PlaceArgument(10, T_SD);
                m_CLProgram_SlopeFailure2->PlaceArgument(11, T_H);
                m_CLProgram_SlopeFailure2->PlaceArgument(12, T_HS);
                m_CLProgram_SlopeFailure2->PlaceArgument(13, T_SIfa);
                m_CLProgram_SlopeFailure2->PlaceArgument(14, T_SDensity);
                m_CLProgram_SlopeFailure2->PlaceArgument(15, T_SRockSize);
                m_CLProgram_SlopeFailure2->PlaceArgument(16, T_SDN);
                m_CLProgram_SlopeFailure2->PlaceArgument(17, T_HN);
                m_CLProgram_SlopeFailure2->PlaceArgument(18, T_HSN);
                m_CLProgram_SlopeFailure2->PlaceArgument(19, T_SIfaN);
                m_CLProgram_SlopeFailure2->PlaceArgument(20, T_SDensityN);
                m_CLProgram_SlopeFailure2->PlaceArgument(21, T_SRockSizeN);
                m_CLProgram_SlopeFailure2->PlaceArgument(22, T_GW_WFN);
                m_CLProgram_SlopeFailure2->PlaceArgument(23, T_GW_USN);
                m_CLProgram_SlopeFailure2->PlaceArgument(24, T_GW_SN);
                m_CLProgram_SlopeFailure2->PlaceArgument(25, T_SFHCUM);
                m_CLProgram_SlopeFailure2->PlaceArgument(26, T_SFHCUMN);
                m_CLProgram_SlopeFailure2->PlaceArgument(28, T_DEMN );
                m_CLProgram_SlopeFailure2->PlaceArgument(29, T_DEM );
            }

            if(m_DoParticleSolids)
            {
                m_CLProgram_SlopeFailureP2->SetRunDims((int)m_NCells,0,0);
                m_CLProgram_SlopeFailureP2->PlaceArgument(0, (int)dim0);
                m_CLProgram_SlopeFailureP2->PlaceArgument(1, (int)dim1);
                m_CLProgram_SlopeFailureP2->PlaceArgument(2, (float)(DEM->cellSize()));
                m_CLProgram_SlopeFailureP2->PlaceArgument(3, (float)(0.5));
                m_CLProgram_SlopeFailureP2->PlaceArgument(4, T_LOCR);
                m_CLProgram_SlopeFailureP2->PlaceArgument(5, T_LOCC);
                m_CLProgram_SlopeFailureP2->PlaceArgument(6, T_SFH);
                m_CLProgram_SlopeFailureP2->PlaceArgument(7, T_INFIL);
                m_CLProgram_SlopeFailureP2->PlaceArgument(8, T_GW_US);
                m_CLProgram_SlopeFailureP2->PlaceArgument(9, T_GW_S);
                m_CLProgram_SlopeFailureP2->PlaceArgument(10, T_SD);
                m_CLProgram_SlopeFailureP2->PlaceArgument(11, T_H);
                m_CLProgram_SlopeFailureP2->PlaceArgument(12, T_HS);
                m_CLProgram_SlopeFailureP2->PlaceArgument(13, T_SIfa);
                m_CLProgram_SlopeFailureP2->PlaceArgument(14, T_SDensity);
                m_CLProgram_SlopeFailureP2->PlaceArgument(15, T_SRockSize);
                m_CLProgram_SlopeFailureP2->PlaceArgument(16, T_SDN);
                m_CLProgram_SlopeFailureP2->PlaceArgument(17, T_HN);
                m_CLProgram_SlopeFailureP2->PlaceArgument(18, T_HSN);
                m_CLProgram_SlopeFailureP2->PlaceArgument(19, T_SIfaN);
                m_CLProgram_SlopeFailureP2->PlaceArgument(20, T_SDensityN);
                m_CLProgram_SlopeFailureP2->PlaceArgument(21, T_SRockSizeN);
                m_CLProgram_SlopeFailureP2->PlaceArgument(22, T_GW_WFN);
                m_CLProgram_SlopeFailureP2->PlaceArgument(23, T_GW_USN);
                m_CLProgram_SlopeFailureP2->PlaceArgument(24, T_GW_SN);
                m_CLProgram_SlopeFailureP2->PlaceArgument(25, T_SFHCUM);
                m_CLProgram_SlopeFailureP2->PlaceArgument(26, T_SFHCUMN);
                m_CLProgram_SlopeFailureP2->PlaceArgument(28, T_DEMN );
                m_CLProgram_SlopeFailureP2->PlaceArgument(29, T_DEM );
            }


        }
        if((m_DoSlopeFailure || m_DoInitialSolids) && !m_DoParticleSolids)
        {

            m_CLProgram_FlowSolids->SetRunDims((int) m_NCells,0,0);

            m_CLProgram_FlowSolids->PlaceArgument(0, (int)dim0);
            m_CLProgram_FlowSolids->PlaceArgument(1, (int)dim1);
            m_CLProgram_FlowSolids->PlaceArgument(2, (float)(DEM->cellSize()));
            m_CLProgram_FlowSolids->PlaceArgument(3, (float)(0.5));
            m_CLProgram_FlowSolids->PlaceArgument(4, T_LOCR);
            m_CLProgram_FlowSolids->PlaceArgument(5, T_LOCC);
            m_CLProgram_FlowSolids->PlaceArgument(6, T_DEM);
            m_CLProgram_FlowSolids->PlaceArgument(7, T_H);
            m_CLProgram_FlowSolids->PlaceArgument(8, T_VX);
            m_CLProgram_FlowSolids->PlaceArgument(9, T_VY);
            m_CLProgram_FlowSolids->PlaceArgument(10, T_HS);
            m_CLProgram_FlowSolids->PlaceArgument(11, T_VSX);
            m_CLProgram_FlowSolids->PlaceArgument(12, T_VSY);
            m_CLProgram_FlowSolids->PlaceArgument(13, T_SIfa);
            m_CLProgram_FlowSolids->PlaceArgument(14, T_SRockSize);
            m_CLProgram_FlowSolids->PlaceArgument(15, T_SDensity);
            m_CLProgram_FlowSolids->PlaceArgument(16, T_F_INFILPOT);
            m_CLProgram_FlowSolids->PlaceArgument(17, T_F_INFILFLOWCUM);
            m_CLProgram_FlowSolids->PlaceArgument(18, T_CHMASK);
            m_CLProgram_FlowSolids->PlaceArgument(19, T_CHWIDTH);
            m_CLProgram_FlowSolids->PlaceArgument(20, T_CHHEIGHT);
            m_CLProgram_FlowSolids->PlaceArgument(21, T_CHH);
            m_CLProgram_FlowSolids->PlaceArgument(22, T_CHVX);
            m_CLProgram_FlowSolids->PlaceArgument(23, T_CHHS);
            m_CLProgram_FlowSolids->PlaceArgument(24, T_CHVS);
            m_CLProgram_FlowSolids->PlaceArgument(25, T_CHSIfa);
            m_CLProgram_FlowSolids->PlaceArgument(26, T_CHSRockSize);
            m_CLProgram_FlowSolids->PlaceArgument(27, T_CHSDensity);
            m_CLProgram_FlowSolids->PlaceArgument(28, T_HN);
            m_CLProgram_FlowSolids->PlaceArgument(29, T_VXN);
            m_CLProgram_FlowSolids->PlaceArgument(30, T_VYN);
            m_CLProgram_FlowSolids->PlaceArgument(31, T_HSN);
            m_CLProgram_FlowSolids->PlaceArgument(32, T_VSXN);
            m_CLProgram_FlowSolids->PlaceArgument(33, T_VSYN);
            m_CLProgram_FlowSolids->PlaceArgument(34, T_SIfaN);
            m_CLProgram_FlowSolids->PlaceArgument(35, T_SRockSizeN);
            m_CLProgram_FlowSolids->PlaceArgument(36, T_SDensityN);
            m_CLProgram_FlowSolids->PlaceArgument(37, T_F_INFILACT);
            m_CLProgram_FlowSolids->PlaceArgument(38, T_CHHN);
            m_CLProgram_FlowSolids->PlaceArgument(39, T_CHVXN);
            m_CLProgram_FlowSolids->PlaceArgument(40, T_CHHSN);
            m_CLProgram_FlowSolids->PlaceArgument(41, T_CHVSN);
            m_CLProgram_FlowSolids->PlaceArgument(42, T_CHSIfaN);
            m_CLProgram_FlowSolids->PlaceArgument(43, T_CHSRockSizeN);
            m_CLProgram_FlowSolids->PlaceArgument(44, T_CHSDensityN);
            m_CLProgram_FlowSolids->PlaceArgument(45, T_UI);
            m_CLProgram_FlowSolids->PlaceArgument(46, 0);
            m_CLProgram_FlowSolids->PlaceArgument(47, 0.0);
            m_CLProgram_FlowSolids->PlaceArgument(48, T_QFOUT);
            m_CLProgram_FlowSolids->PlaceArgument(49, T_QFOUTN);
            m_CLProgram_FlowSolids->PlaceArgument(50, T_QSOUT);
            m_CLProgram_FlowSolids->PlaceArgument(51, T_QSOUTN);
            m_CLProgram_FlowSolids->PlaceArgument(52, T_DTREQ);
            m_CLProgram_FlowSolids->PlaceArgument(53, T_RAIN);
            m_CLProgram_FlowSolids->PlaceArgument(54, T_N);
            m_CLProgram_FlowSolids->PlaceArgument(55, (float)m_DragMult);
            m_CLProgram_FlowSolids->PlaceArgument(56, T_QFX1);
            m_CLProgram_FlowSolids->PlaceArgument(57, T_QFX2);
            m_CLProgram_FlowSolids->PlaceArgument(58, T_QFY1);
            m_CLProgram_FlowSolids->PlaceArgument(59, T_QFY2);
            m_CLProgram_FlowSolids->PlaceArgument(60, T_QFCHX1);
            m_CLProgram_FlowSolids->PlaceArgument(61, T_QFCHX2);
            m_CLProgram_FlowSolids->PlaceArgument(62, T_QFCHIN);

            //m_CLProgram_Flow2->SetRunDims((int)dim0, (int)dim1);
            m_CLProgram_FlowSolids2->SetRunDims((int) m_NCells,0,0);

            m_CLProgram_FlowSolids2->PlaceArgument(0, (int)dim0);
            m_CLProgram_FlowSolids2->PlaceArgument(1, (int)dim1);
            m_CLProgram_FlowSolids2->PlaceArgument(2, (float)(DEM->cellSize()));
            m_CLProgram_FlowSolids2->PlaceArgument(3, (float)(0.5));
            m_CLProgram_FlowSolids2->PlaceArgument(4, T_LOCR);
            m_CLProgram_FlowSolids2->PlaceArgument(5, T_LOCC);
            m_CLProgram_FlowSolids2->PlaceArgument(6, T_DEM);
            m_CLProgram_FlowSolids2->PlaceArgument(7, T_H);
            m_CLProgram_FlowSolids2->PlaceArgument(8, T_VX);
            m_CLProgram_FlowSolids2->PlaceArgument(9, T_VY);
            m_CLProgram_FlowSolids2->PlaceArgument(10, T_HS);
            m_CLProgram_FlowSolids2->PlaceArgument(11, T_VSX);
            m_CLProgram_FlowSolids2->PlaceArgument(12, T_VSY);
            m_CLProgram_FlowSolids2->PlaceArgument(13, T_SIfa);
            m_CLProgram_FlowSolids2->PlaceArgument(14, T_SRockSize);
            m_CLProgram_FlowSolids2->PlaceArgument(15, T_SDensity);
            m_CLProgram_FlowSolids2->PlaceArgument(16, T_F_INFILFLOWCUM);
            m_CLProgram_FlowSolids2->PlaceArgument(17, T_CHH);
            m_CLProgram_FlowSolids2->PlaceArgument(18, T_CHVX);
            m_CLProgram_FlowSolids2->PlaceArgument(19, T_CHHS);
            m_CLProgram_FlowSolids2->PlaceArgument(20, T_CHVS);
            m_CLProgram_FlowSolids2->PlaceArgument(21, T_CHSIfa);
            m_CLProgram_FlowSolids2->PlaceArgument(22, T_CHSRockSize);
            m_CLProgram_FlowSolids2->PlaceArgument(23, T_CHSDensity);
            m_CLProgram_FlowSolids2->PlaceArgument(24, T_HN);
            m_CLProgram_FlowSolids2->PlaceArgument(25, T_VXN);
            m_CLProgram_FlowSolids2->PlaceArgument(26, T_VYN);
            m_CLProgram_FlowSolids2->PlaceArgument(27, T_HSN);
            m_CLProgram_FlowSolids2->PlaceArgument(28, T_VSXN);
            m_CLProgram_FlowSolids2->PlaceArgument(29, T_VSYN);
            m_CLProgram_FlowSolids2->PlaceArgument(30, T_SIfaN);
            m_CLProgram_FlowSolids2->PlaceArgument(31, T_SRockSizeN);
            m_CLProgram_FlowSolids2->PlaceArgument(32, T_SDensityN);
            m_CLProgram_FlowSolids2->PlaceArgument(33, T_F_INFILACT);
            m_CLProgram_FlowSolids2->PlaceArgument(34, T_CHHN);
            m_CLProgram_FlowSolids2->PlaceArgument(35, T_CHVXN);
            m_CLProgram_FlowSolids2->PlaceArgument(36, T_CHHSN);
            m_CLProgram_FlowSolids2->PlaceArgument(37, T_CHVSN);
            m_CLProgram_FlowSolids2->PlaceArgument(38, T_CHSIfaN);
            m_CLProgram_FlowSolids2->PlaceArgument(39, T_CHSRockSizeN);
            m_CLProgram_FlowSolids2->PlaceArgument(40, T_CHSDensityN);
            m_CLProgram_FlowSolids2->PlaceArgument(41, T_HFMax);
            m_CLProgram_FlowSolids2->PlaceArgument(42, T_VFMax);
            m_CLProgram_FlowSolids2->PlaceArgument(43, T_HFMaxN);
            m_CLProgram_FlowSolids2->PlaceArgument(44, T_VFMaxN);
            m_CLProgram_FlowSolids2->PlaceArgument(45, T_HSMax);
            m_CLProgram_FlowSolids2->PlaceArgument(46, T_VSMax);
            m_CLProgram_FlowSolids2->PlaceArgument(47, T_HSMaxN);
            m_CLProgram_FlowSolids2->PlaceArgument(48, T_VSMaxN);
        }

        if((m_DoSlopeFailure || m_DoInitialSolids) && m_DoParticleSolids)
        {
            m_CLProgram_FlowSolidsParticle->SetRunDims((int) m_NCells,0,0);

            m_CLProgram_FlowSolidsParticle->PlaceArgument(0, (int)dim0);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(1, (int)dim1);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(2, (float)(DEM->cellSize()));
            m_CLProgram_FlowSolidsParticle->PlaceArgument(3, (float)(0.5));
            m_CLProgram_FlowSolidsParticle->PlaceArgument(4, T_LOCR);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(5, T_LOCC);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(6, T_DEM);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(7, T_H);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(8, T_VX);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(9, T_VY);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(10, T_HS);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(11, T_VSX);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(12, T_VSY);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(13, T_SIfa);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(14, T_SRockSize);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(15, T_SDensity);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(16, T_F_INFILPOT);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(17, T_F_INFILFLOWCUM);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(18, T_CHMASK);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(19, T_CHWIDTH);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(20, T_CHHEIGHT);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(21, T_CHH);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(22, T_CHVX);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(23, T_CHHS);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(24, T_CHVS);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(25, T_CHSIfa);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(26, T_CHSRockSize);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(27, T_CHSDensity);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(28, T_HN);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(29, T_VXN);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(30, T_VYN);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(31, T_HSN);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(32, T_VSXN);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(33, T_VSYN);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(34, T_SIfaN);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(35, T_SRockSizeN);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(36, T_SDensityN);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(37, T_F_INFILACT);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(38, T_CHHN);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(39, T_CHVXN);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(40, T_CHHSN);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(41, T_CHVSN);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(42, T_CHSIfaN);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(43, T_CHSRockSizeN);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(44, T_CHSDensityN);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(45, T_UI);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(46, 0);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(47, 0.0);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(48, T_QFOUT);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(49, T_QFOUTN);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(50, T_QSOUT);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(51, T_QSOUTN);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(52, T_DTREQ);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(53, T_RAIN);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(54, T_SCohesionFrac);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(55, T_SFLUIDH);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(56, T_SFLUIDHADD);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(57, T_HALL);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(58, T_N);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(59, (float)m_DragMult);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(60, T_QFX1);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(61, T_QFX2);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(62, T_QFY1);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(63, T_QFY2);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(64, T_QFCHX1);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(65, T_QFCHX2);
            m_CLProgram_FlowSolidsParticle->PlaceArgument(66, T_QFCHIN);

            //m_CLProgram_Flow2->SetRunDims((int)dim0, (int)dim1);
            m_CLProgram_FlowSolidsParticle2->SetRunDims((int) m_NCells,0,0);

            m_CLProgram_FlowSolidsParticle2->PlaceArgument(0, (int)dim0);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(1, (int)dim1);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(2, (float)(DEM->cellSize()));
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(3, (float)(0.5));
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(4, T_LOCR);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(5, T_LOCC);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(6, T_DEM);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(7, T_H);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(8, T_VX);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(9, T_VY);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(10, T_HS);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(11, T_VSX);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(12, T_VSY);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(13, T_SIfa);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(14, T_SRockSize);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(15, T_SDensity);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(16, T_F_INFILFLOWCUM);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(17, T_CHH);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(18, T_CHVX);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(19, T_CHHS);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(20, T_CHVS);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(21, T_CHSIfa);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(22, T_CHSRockSize);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(23, T_CHSDensity);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(24, T_HN);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(25, T_VXN);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(26, T_VYN);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(27, T_HSN);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(28, T_VSXN);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(29, T_VSYN);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(30, T_SIfaN);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(31, T_SRockSizeN);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(32, T_SDensityN);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(33, T_F_INFILACT);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(34, T_CHHN);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(35, T_CHVXN);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(36, T_CHHSN);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(37, T_CHVSN);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(38, T_CHSIfaN);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(39, T_CHSRockSizeN);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(40, T_CHSDensityN);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(41, T_HFMax);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(42, T_VFMax);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(43, T_HFMaxN);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(44, T_VFMaxN);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(45, T_HSMax);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(46, T_VSMax);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(47, T_HSMaxN);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(48, T_VSMaxN);
            m_CLProgram_FlowSolidsParticle2->PlaceArgument(49, T_SCohesionFrac);

            m_CLProgram_StoreIndices->PlaceArgument(0,m_DataIndex);
            m_CLProgram_StoreIndices->PlaceArgument(1,m_DataIndexC);
            m_CLProgram_StoreIndices->PlaceArgument(2,T_P_Index);
            m_CLProgram_StoreIndices->PlaceArgument(3,(int)m_NParticle);
            m_CLProgram_StoreIndices->PlaceArgument(4,(int)dim0);
            m_CLProgram_StoreIndices->PlaceArgument(5,(int)dim1);


            m_CLProgram_ParticlesToMaps->SetRunDims((int)m_NCells,0,0);
            m_CLProgram_ParticlesToMaps->PlaceArgument(0, (int)dim0);
            m_CLProgram_ParticlesToMaps->PlaceArgument(1, (int)dim1);
            m_CLProgram_ParticlesToMaps->PlaceArgument(2, (float)(DEM->cellSize()));
            m_CLProgram_ParticlesToMaps->PlaceArgument(3, (float)(0.5));
            m_CLProgram_ParticlesToMaps->PlaceArgument(4, T_LOCR);
            m_CLProgram_ParticlesToMaps->PlaceArgument(5, T_LOCC);
            m_CLProgram_ParticlesToMaps->PlaceArgument(6, m_DataPos);
            m_CLProgram_ParticlesToMaps->PlaceArgument(7, m_DataVel);
            m_CLProgram_ParticlesToMaps->PlaceArgument(8, m_DataProps);
            m_CLProgram_ParticlesToMaps->PlaceArgument(9, m_DataPropsPhysical);
            m_CLProgram_ParticlesToMaps->PlaceArgument(10, m_DataIndex);
            m_CLProgram_ParticlesToMaps->PlaceArgument(11, m_DataIndexC);
            m_CLProgram_ParticlesToMaps->PlaceArgument(12, (int)m_NParticle);
            m_CLProgram_ParticlesToMaps->PlaceArgument(13, T_P_Index);
            m_CLProgram_ParticlesToMaps->PlaceArgument(14, T_HS);
            m_CLProgram_ParticlesToMaps->PlaceArgument(15, T_VSX);
            m_CLProgram_ParticlesToMaps->PlaceArgument(16, T_VSY);
            m_CLProgram_ParticlesToMaps->PlaceArgument(17, T_SRockSize);
            m_CLProgram_ParticlesToMaps->PlaceArgument(18, T_SDensity);
            m_CLProgram_ParticlesToMaps->PlaceArgument(19, T_SIfa);
            m_CLProgram_ParticlesToMaps->PlaceArgument(20, T_SCohesion);
            m_CLProgram_ParticlesToMaps->PlaceArgument(21, T_SCohesionFrac);
            m_CLProgram_ParticlesToMaps->PlaceArgument(22, m_DataFluidProps);
            m_CLProgram_ParticlesToMaps->PlaceArgument(23, T_SFLUIDH);
            m_CLProgram_ParticlesToMaps->PlaceArgument(24, T_SFLUIDHADD);
            m_CLProgram_ParticlesToMaps->PlaceArgument(25, factor_particle);

            m_CLProgram_Particles->SetRunDims(m_NParticle);
            m_DataPos->SetDrawDim(m_NParticle);
            m_CLProgram_Particles->PlaceArgument(0, (int)dim0);
            m_CLProgram_Particles->PlaceArgument(1, (int)dim1);
            m_CLProgram_Particles->PlaceArgument(2, (float)(DEM->cellSize()));
            m_CLProgram_Particles->PlaceArgument(3, (float)(0.5));
            m_CLProgram_Particles->PlaceArgument(4, m_DataPos);
            m_CLProgram_Particles->PlaceArgument(5, m_DataVel);
            m_CLProgram_Particles->PlaceArgument(6, m_DataProps);
            m_CLProgram_Particles->PlaceArgument(7, m_DataStressTensor);
            m_CLProgram_Particles->PlaceArgument(8, m_DataIndex);
            m_CLProgram_Particles->PlaceArgument(9, m_DataIndexC);
            m_CLProgram_Particles->PlaceArgument(10, m_DataUI);
            m_CLProgram_Particles->PlaceArgument(11, (int)m_DataPos->m_dims[0]);
            m_CLProgram_Particles->PlaceArgument(12, T_DEM);
            m_CLProgram_Particles->PlaceArgument(13, T_P_Index);
            m_CLProgram_Particles->PlaceArgument(14, m_DataPosN);
            m_CLProgram_Particles->PlaceArgument(15, m_DataVelN);
            m_CLProgram_Particles->PlaceArgument(16, m_DataPropsPhysical);
            m_CLProgram_Particles->PlaceArgument(17, m_DataStressTensorN);
            m_CLProgram_Particles->PlaceArgument(18, m_DataDispTensor);
            m_CLProgram_Particles->PlaceArgument(19, m_DataFluidProps);
            m_CLProgram_Particles->PlaceArgument(20, T_N);
            m_CLProgram_Particles->PlaceArgument(21, (float)m_DragMult);
            m_CLProgram_Particles->PlaceArgument(22, (float)factor_particle);
            m_CLProgram_Particles->PlaceArgument(23, (float)m_ShearModulus);
            m_CLProgram_Particles->PlaceArgument(24, (float)m_ElasticModulus);


            m_CLProgram_Particles2->SetRunDims(m_NParticle);
            m_CLProgram_Particles2->PlaceArgument(0, m_DataPos);
            m_CLProgram_Particles2->PlaceArgument(1, m_DataVel);
            m_CLProgram_Particles2->PlaceArgument(2, m_DataProps);
            m_CLProgram_Particles2->PlaceArgument(3, m_DataStressTensor);
            m_CLProgram_Particles2->PlaceArgument(4, m_DataIndex);
            m_CLProgram_Particles2->PlaceArgument(5, m_DataPosN);
            m_CLProgram_Particles2->PlaceArgument(6, m_DataVelN);
            m_CLProgram_Particles2->PlaceArgument(7, m_DataPropsPhysical);
            m_CLProgram_Particles2->PlaceArgument(8, m_DataStressTensorN);
            m_CLProgram_Particles2->PlaceArgument(9, T_P_Index);
            m_CLProgram_Particles2->PlaceArgument(10, (float)(DEM->cellSize()));
            m_CLProgram_Particles2->PlaceArgument(11, (int)dim0);
            m_CLProgram_Particles2->PlaceArgument(12, (int)dim1);
            m_CLProgram_Particles2->PlaceArgument(13, (int)m_NParticle);
            m_CLProgram_Particles2->PlaceArgument(14, (float)factor_particle);

            m_CLProgram_ParticlesFromMaps->SetRunDims(m_NParticle);
            m_CLProgram_ParticlesFromMaps->PlaceArgument(0, (int)dim0);
            m_CLProgram_ParticlesFromMaps->PlaceArgument(1, (int)dim1);
            m_CLProgram_ParticlesFromMaps->PlaceArgument(2, (float)(DEM->cellSize()));
            m_CLProgram_ParticlesFromMaps->PlaceArgument(3, (float)(0.5));
            m_CLProgram_ParticlesFromMaps->PlaceArgument(4, m_DataPos);
            m_CLProgram_ParticlesFromMaps->PlaceArgument(5, m_DataVel);
            m_CLProgram_ParticlesFromMaps->PlaceArgument(6, m_DataStressTensor);
            m_CLProgram_ParticlesFromMaps->PlaceArgument(7, m_DataProps);
            m_CLProgram_ParticlesFromMaps->PlaceArgument(8, m_DataIndex);
            m_CLProgram_ParticlesFromMaps->PlaceArgument(9, m_DataIndexC);
            m_CLProgram_ParticlesFromMaps->PlaceArgument(10, (int)m_NParticle);
            m_CLProgram_ParticlesFromMaps->PlaceArgument(11, T_P_Index);
            m_CLProgram_ParticlesFromMaps->PlaceArgument(12, T_HS);
            m_CLProgram_ParticlesFromMaps->PlaceArgument(13, T_VSX);
            m_CLProgram_ParticlesFromMaps->PlaceArgument(14, T_VSY);
            m_CLProgram_ParticlesFromMaps->PlaceArgument(15, T_VSXN);
            m_CLProgram_ParticlesFromMaps->PlaceArgument(16, T_VSYN);
            m_CLProgram_ParticlesFromMaps->PlaceArgument(17, T_HN);
            m_CLProgram_ParticlesFromMaps->PlaceArgument(18, T_VXN);
            m_CLProgram_ParticlesFromMaps->PlaceArgument(19, T_VYN);
            m_CLProgram_ParticlesFromMaps->PlaceArgument(20, m_DataFluidProps);
            m_CLProgram_ParticlesFromMaps->PlaceArgument(21, factor_particle);
        }
    }

}

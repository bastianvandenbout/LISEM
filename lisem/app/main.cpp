
#include <iostream>
#include "qsslsocket.h"
#include "chrono/physics/ChLinkMotorRotationSpeed.h"
#include "chrono/physics/ChSystemNSC.h"
#include "chrono/solver/ChSolverAPGD.h"
#include "chrono/solver/ChSolverPSOR.h"
#include "chrono/geometry/ChTriangleMeshSoup.h"
#include "chrono/physics/ChBody.h"
#include "chrono/core/ChQuadrature.h"
#include "chrono/physics/ChSystemNSC.h"
#include "chrono/core/ChApiCE.h"
#include "chrono/core/ChMatrix.h"

chrono::ChSystemNSC m_system;


inline void DoChrono()
{

    {

        std::shared_ptr<chrono::ChSolverPSOR> m_Solver;
        m_Solver = chrono_types::make_shared<chrono::ChSolverPSOR>();
        m_Solver->SetMaxIterations(100);
        m_Solver->EnableWarmStart(true);

        {
            std::shared_ptr<chrono::ChSolverPSOR> m_Solver;
            m_Solver = chrono_types::make_shared<chrono::ChSolverPSOR>();
            m_Solver->SetMaxIterations(100);
            m_Solver->EnableWarmStart(true);

        }

        {

            //m_system.Set_G_acc(ChVector<double>(0.0,0.0,0.0));
            m_system.SetSolver(m_Solver);
            m_system.SetMaxPenetrationRecoverySpeed(1.0);
            //m_system.SetTimestepperType(ChTimestepper::Type::RUNGEKUTTA45);
            std::cout << " a"  << std::endl;
        }
        std::cout <<" b" << std::endl;
    }
    std::cout << " c" << std::endl;

    for(int i= 0; i < 10; i++){
        std::cout << "qssl "  << QSslSocket::sslLibraryBuildVersionString().toStdString() << std::endl;

        //SPHazard sphazard;

        //int errorcode = sphazard.execute(argc,argv);

        chrono::ChMatrixDynamic<> lcoef(10,10);
        lcoef.setZero();

    }

    std::cout << "done" << std::endl;

}

#include "lisem.h"

int main(int argc, char *argv[])
{


    DoChrono();
    std::cout << "qssl "  << QSslSocket::sslLibraryBuildVersionString().toStdString() << std::endl;

    SPHazard sphazard;

    int errorcode = sphazard.execute(argc,argv);



    return 0;
}


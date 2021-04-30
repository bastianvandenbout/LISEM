#define OPENMVG_USE_OPENMP

#include "openMVG/sfm/sfm_data.hpp"
#include "openMVG/sfm/sfm_data_io.hpp"
#include <omp.h>
#include "error.h"
#include "defines.h"

using namespace openMVG;
using namespace openMVG::sfm;
using namespace openMVG::cameras;
using namespace std;

LISEM_API bool exportToOpenMVS(
  const SfM_Data & sfm_data,
  const std::string & sOutFile,
  const std::string & sOutFiletemp,
        const std::string & sOutDir,
  const int iNumThreads = 0
  );

LISEM_API bool ExportToMVSText(std::string in, std::string out);

inline static void AS_MGStoVMS(const SfM_Data &sfm_data,std::string sSfM_Data_Filename, std::string sOutDir)
{
		

  if (!exportToOpenMVS(sfm_data, sOutDir + "/MVS/scenetemp.mvs",sOutDir + "/MVS/scenetemp.mvs", sOutDir+"/MVS/", 0))
  {
    LISEMS_ERROR("Could not convert output to VMS format");
     throw 1;


  }

  ExportToMVSText(sOutDir + "/MVS/scenetemp.mvs",sOutDir + "/MVS/scene.mvs");

  std::cout << "exported to mvs scenes" << std::endl;

}



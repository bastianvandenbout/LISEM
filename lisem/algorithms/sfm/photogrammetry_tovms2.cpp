#include "photogrammetry_tovms.h"

#include "MVS.h"
#include <string>
#include <omp.h>
#include "error.h"

bool ExportToMVSText(std::string in, std::string out)
{
    MVS::Scene scene(omp_get_num_threads());

    // load and estimate a dense point-cloud
    if (!scene.Load(MAKE_PATH_SAFE((in).c_str())))
    {

        LISEMS_ERROR("Could not load scene " + QString(MAKE_PATH_SAFE((in).c_str())));
        throw 1;
    }

    scene.Save(out,(ARCHIVE_TYPE)(0));

    return true;
}

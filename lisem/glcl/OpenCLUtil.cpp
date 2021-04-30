#include "OpenCLUtil.h"

#include <vector>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <QString>
#include "error.h"
#include "QList"

using namespace cl;


#if defined (__APPLE__) || defined(MACOSX)
const std::string CL_GL_SHARING_EXT = "cl_APPLE_gl_sharing";
#else
const std::string CL_GL_SHARING_EXT = "cl_khr_gl_sharing";
#endif


QString NVIDIA_PLATFORM = "NVIDIA";
QString AMD_PLATFORM = "AMD";
QString MESA_PLATFORM = "Clover";
QString INTEL_PLATFORM = "Intel";
QString APPLE_PLATFORM = "Apple";

QString PREFERRED_PLATFORM = "NVIDIA";
int PREFERRED_DEVICE = -1;

typedef std::vector<Platform>::iterator PlatformIter;

void SetPlatformPreference(const QString& s, int preferred_id)
{

    if(s.compare(NVIDIA_PLATFORM) == 0)
    {
        PREFERRED_PLATFORM = NVIDIA_PLATFORM;
    }else if(s.compare(AMD_PLATFORM) == 0)
    {
        PREFERRED_PLATFORM = AMD_PLATFORM;
    }else if(s.compare(MESA_PLATFORM) == 0)
    {
        PREFERRED_PLATFORM = MESA_PLATFORM;
    }else if(s.compare(INTEL_PLATFORM) == 0)
    {
        PREFERRED_PLATFORM = INTEL_PLATFORM;
    }else if(s.compare(APPLE_PLATFORM) == 0)
    {
        PREFERRED_PLATFORM = APPLE_PLATFORM;
    }else
    {
        LISEM_ERROR("Unknown platform, did not set preference!");
    }



}


void ListPlatforms()
{
    cl_int error;
    try {
        // Get available platforms
        std::vector<Platform> platforms;
        LISEM_DEBUG(" ");
        LISEM_DEBUG("List all Platforms and Devices");
        Platform::get(&platforms);

        for(PlatformIter it=platforms.begin(); it<platforms.end(); ++it) {
            QString platformname = QString(it->getInfo<CL_PLATFORM_NAME>().c_str());
            LISEM_DEBUG("Found Platform " + platformname);
            std::string temp = it->getInfo<CL_PLATFORM_NAME>();

            cl::Platform lPlatform = *it;

            std::vector<cl::Device> devices;
            lPlatform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
            int i = 0;
            // Get a list of devices on this platform
            for (unsigned d=0; d<devices.size(); ++d) {

                QString devicename = QString(devices[d].getInfo<CL_DEVICE_NAME>().c_str());
                int computeunits = devices[d].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
                int computeclock = devices[d].getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>();
                int computepars = devices[d].getInfo<CL_DEVICE_MAX_PARAMETER_SIZE>();
                int computetype = devices[d].getInfo<CL_DEVICE_TYPE>();
                QString clversion = QString(devices[d].getInfo<CL_DEVICE_VERSION>().c_str());
                unsigned long long computemem = devices[d].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>();

                const char *typestr;

                switch(computetype) {
                case CL_DEVICE_TYPE_CPU:
                    typestr = "CPU";
                    break;
                case CL_DEVICE_TYPE_ACCELERATOR:
                    typestr = "accelerator";
                    break;
                case CL_DEVICE_TYPE_GPU:
                    typestr = "GPU";
                    break;
                default:
                    typestr = "<unknown>";
                }
                if (checkExtnAvailability(devices[d],CL_GL_SHARING_EXT)) {

                    LISEM_DEBUG("Found Device " + devicename + " id: " + QString::number(i) + " computetype " + QString(typestr) + " clversion:" + clversion + " mem (mb):" + QString::number(computemem/1000000) +" compute units:" + QString::number(computeunits) +" clock (Mhz):" + QString::number(computeclock) + " gl/cl interop true");
                }else
                {
                    LISEM_DEBUG("Found Device " + devicename + " id: " + QString::number(i)  + " computetype " + QString(typestr) + " clversion:" + clversion + " mem (mb):" + QString::number(computemem/1000000) +" compute units:" + QString::number(computeunits) +" clock (Mhz):" + QString::number(computeclock) + " gl/cl interop false");
                }
                i++;
            }

        }
    } catch(Error err) {
        LISEM_ERROR(QString(err.what()) + "(" + QString(err.err()) + ")");
        error = err.err();
        LISEM_ERROR("Error during search for OpenCL platform");
        throw 1;
    }
    LISEM_DEBUG(" ");
    LISEM_DEBUG(" ");
}


Platform getPlatform(std::string pName, cl_int &error)
{
    LISEM_DEBUG(QString("Try to find Platform ") +QString(pName.c_str()));

    Platform ret_val;
    error = 0;
    try {
        // Get available platforms
        std::vector<Platform> platforms;
        Platform::get(&platforms);
        int found = -1;
        for(PlatformIter it=platforms.begin(); it<platforms.end(); ++it) {
            std::string temp = it->getInfo<CL_PLATFORM_NAME>();
            if (temp.find(pName)!=std::string::npos) {
                found = it - platforms.begin();

                break;
            }
        }
        if (found==-1) {
            // Going towards + numbers to avoid conflict with OpenCl error codes
            error = +1; // requested platform not found
        } else {
            ret_val = platforms[found];
        }
    } catch(Error err) {
        LISEM_ERROR(QString(err.what()) + "(" + QString(err.err()) + ")");
        error = err.err();
        LISEM_ERROR("Error during search for OpenCL platform");
        throw 1;
    }

    return ret_val;
}

#define FIND_PLATFORM(name)             \
    plat = getPlatform(name.toStdString(), errCode);  \
    if (errCode == CL_SUCCESS)          \
        return plat;

Platform getPlatform()
{
    cl_int errCode;
    Platform plat;

    FIND_PLATFORM(PREFERRED_PLATFORM)

    //-1 indicates no preferred platform
    FIND_PLATFORM(NVIDIA_PLATFORM)
    FIND_PLATFORM(INTEL_PLATFORM)
    FIND_PLATFORM(AMD_PLATFORM)
    FIND_PLATFORM(APPLE_PLATFORM)
    FIND_PLATFORM(MESA_PLATFORM)

    // If no platforms are found
    exit(252);
}

bool checkExtnAvailability(Device pDevice, std::string pName)
{
    bool ret_val = true;
    try {
        // find extensions required
        std::string exts = pDevice.getInfo<CL_DEVICE_EXTENSIONS>();
        std::stringstream ss(exts);
        std::string item;
        int found = -1;
        while (std::getline(ss,item,' ')) {
            if (item==pName) {
                found=1;
                break;
            }
        }
        if (found==1) {
            ret_val = true;
        } else {
            ret_val = false;
        }
    } catch (Error err) {
        std::cout << err.what() << "(" << err.err() << ")" << std::endl;
    }
    return ret_val;
}

Program getProgram(Context pContext, std::string file, cl_int &error)
{
    Program ret_val;
    error = 0;
    std::ifstream sourceFile(file.c_str());
    std::string sourceCode(
            std::istreambuf_iterator<char>(sourceFile),
            (std::istreambuf_iterator<char>()));
    try {
        Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length()+1));
        // Make program of the source code in the context
        ret_val = Program(pContext, source);
    } catch(Error err) {
        LISEM_ERROR(QString("cl program error ") + err.what() + "(" + err.err() + ")");
        error = err.err();
    }
    return ret_val;
}

Program getProgramString(Context pContext, std::string file, cl_int &error)
{
    Program ret_val;
    error = 0;
    std::istringstream sourceFile(file.c_str());
    std::string sourceCode(
            std::istreambuf_iterator<char>(sourceFile),
            (std::istreambuf_iterator<char>()));
    try {
        Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length()+1));
        // Make program of the source code in the context
        ret_val = Program(pContext, source);
    } catch(Error err) {
        LISEM_ERROR(QString("cl program error ") + err.what() + "(" + err.err() + ")");
        error = err.err();
    }
    return ret_val;
}

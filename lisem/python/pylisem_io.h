#ifndef PYLISEM_IO_H
#define PYLISEM_IO_H


#include "gdal.h"
#include "iogdal.h"
#include "iogdalshape.h"
#include "iopdal.h"
#include "ioassimp.h"
#include "lsmio.h"
#include "webgeoservice.h"
#include "downloadmanager.h"

inline AS_ByteArray * AS_DownloadToBA(QString url, int timeout, QString username, QString password)
{
    DownloadTask * task = new DownloadTask(url,"",username,password,timeout);

    task->shouldbesaved = false;
    GetDownloadManager()->AddDownloadAndWait(task);

    AS_ByteArray * ba = new AS_ByteArray();
    ba->data = task->m_Data;
    delete task->Download;
    delete task;
    return ba;
}

inline AS_ByteArray * AS_DownloadToBAWithPost(QString url, QString post, int timeout,QString username, QString password)
{
    DownloadTask * task = new DownloadTask(url,"",username,password,timeout);
    task->HasPost = true;
    task->PostData = post;
    task->shouldbesaved = false;
    GetDownloadManager()->AddDownloadAndWait(task);

    AS_ByteArray * ba = new AS_ByteArray();
    ba->data = task->m_Data;
    delete task->Download;
    delete task;
    return ba;
}



#include "pybind11/pybind11.h"
namespace py = pybind11;

inline void init_pylisem_io(py::module &m)
{

    m.def("SetWorkDir", &AS_SetASDefaultPath);
    m.def("GetWorkDir", &AS_GetASDefaultPath);
    m.def("LoadMap", &AS_LoadMapFromFile);
    m.def("LoadMapAbsPath", &AS_LoadMapFromFile);
    m.def("SaveMap", &AS_SaveMapToFile);
    m.def("SaveMapAbsPath", &AS_SaveMapToFileAbsolute);
    m.def("CopyFile", py::overload_cast<QString,QString,QString,QString>(CopyMap));
    m.def("CopyFile", py::overload_cast<QString,QString>(CopyMapByPath));
    m.def("LoadShapes", &AS_LoadVectorFromFile);
    m.def("LoadShapesAbsPath", &AS_LoadVectorFromFile);
    m.def("SaveShapes", &AS_SaveVectorToFile);
    m.def("SaveShapesAbsPath", &AS_SaveVectorToFileAbsolute);
    m.def("LoadTable", &AS_LoadTableFromFile);
    m.def("LoadTableAbsPath", &AS_LoadTableFromFile);
    m.def("SaveTable", &AS_SaveTableToFile);
    m.def("SaveTableAbsPath", &AS_SaveTableToFileAbsPath);
    m.def("LoadPointCloud",&AS_LoadPointCloudFromFile);
    m.def("LoadPointCloudAbsPath",&AS_LoadPointCloudFromFileAbsPath);
    m.def("LoadPointCloud",&AS_SavePointCloudToFile);
    m.def("LoadPointCloudAbsPath",&AS_SavePointCloudToFileAbsPath);
    m.def("LoadObjectCloud",&AS_LoadModelFromFile);
    m.def("LoadObjecttCloudAbsPath",&AS_LoadModelFromFileAbsPath);
    m.def("LoadObjectCloud",&AS_SaveModelToFile);
    m.def("LoadObjectCloudAbsPath",&AS_SaveModelToFileAbsPath);
    m.def("ExtractFile",&ExtractFileTo,py::arg("File"),py::arg("Destination"));
    m.def("GetFileDir", &AS_GetFileDir);
    m.def("GetFileName", &AS_GetFileName);
    m.def("GetFileExt", &AS_GetFileExt);
    m.def("LoadMapBandList",&LoadMapBandList);
    m.def("LoadMapBandList",&LoadMapBandListFromStrings);
    m.def("GetMapListNames",&GetMapBandListStrings,py::arg("name"),py::arg("count"),py::arg("start"),py::arg("between") = QString("_"));
    m.def("GetMapListName",&GetMapBandListString,py::arg("name"),py::arg("index"),py::arg("between") = QString("_"));
    m.def("GetFilesInDir",&AS_GetFilesInDir,py::arg("Dir"),py::arg("Filter"));
    m.def("GetItemsContaining",&AS_GetItemsContaining,py::arg("Items"),py::arg("Filter"), py::arg("AllowDouble") = false);


    py::class_<AS_ByteArray>(m,"ByteArray")
            .def(py::init<>());

    m.def("Download",&AS_DownloadToBA, py::arg("url"),py::arg("timeout") = 0,py::arg("username") =QString(),py::arg("password")= QString());
    m.def("DownloadWithPost",&AS_DownloadToBAWithPost, py::arg("url"),py::arg("Post"),py::arg("timeout") = 0,py::arg("username") =QString(),py::arg("password")= QString());

    m.def("BytesToText",&ToText, py::arg("ByteArray"));
    m.def("BytesToXML",&ToXML, py::arg("ByteArray"));
    m.def("BytesToMap",&ToMap, py::arg("ByteArray"),py::arg("Format"));
    m.def("OSMExtentToXMLPost",&AS_OSMToXMLPost, py::arg("WGS84Region"));


    /*
   sm->RegisterGlobalFunction("string ValuesToXMLPost(string xmlname, array<string> &in namelist, array<string> &in valuelist)", asFUNCTION(AS_ValueListToXMLPost),  asCALL_CDECL);


        sm->m_Engine->RegisterGlobalFunction("string OWSCapabilities(const string &in xml)", asFUNCTION( GetOWSCapabilitiesType),  asCALL_CDECL);



   */
}
#endif // PYLISEM_IO_H

#ifndef SCRIPTIO_H
#define SCRIPTIO_H

#include "scriptmanager.h"
#include "lsmio.h"
#include "scriptarrayhelpers.h"
#include "iohdf5.h"

inline static void RegisterScriptFunctionsIO(LSMScriptEngine * sm)
{
   sm->RegisterGlobalFunction("Table @LoadTable(const string &in s)", asFUNCTION( AS_LoadTableFromFile), asCALL_CDECL);
   sm->RegisterGlobalFunction("void SaveTable(Table &in sh, const string &in s)", asFUNCTION( AS_SaveTableToFile), asCALL_CDECL);
   sm->RegisterGlobalFunction("Table @LoadTable(const string &in s,const string &in s2)", asFUNCTION( AS_LoadTableFromFileSep), asCALL_CDECL);
   sm->RegisterGlobalFunction("void SaveTables(Table &in sh, const string &in s,const string &in s2)", asFUNCTION( AS_SaveTableToFileSep), asCALL_CDECL);
   sm->RegisterGlobalFunction("Table @SaveThisTable(const string &in s)", asFUNCTION( AS_SaveThisTableToFile),  asCALL_CDECL);
   sm->RegisterGlobalFunction("Shapes @LoadVectorAbsPath(const string &in s)", asFUNCTION( AS_LoadVectorFromFileAbsPath), asCALL_CDECL);
   sm->RegisterGlobalFunction("Shapes @LoadVector(const string &in s)", asFUNCTION( AS_LoadVectorFromFile), asCALL_CDECL);
   sm->RegisterGlobalFunction("void SaveVector(Shapes &in sh, const string &in s)", asFUNCTION( AS_SaveVectorToFile), asCALL_CDECL);
   sm->RegisterGlobalFunction("void SaveVectorAbsPath(Shapes &in sh, const string &in s)", asFUNCTION( AS_SaveVectorToFileAbsolute), asCALL_CDECL);
   sm->RegisterGlobalFunction("Shapes @SaveThisVector(const string &in s)", asFUNCTION( AS_SaveThisVectorToFile),  asCALL_CDECL);
   sm->RegisterGlobalFunction("Map @LoadMapAbsPath(const string &in s)", asFUNCTION( AS_LoadMapFromFileAbsPath), asCALL_CDECL);
   sm->RegisterGlobalFunction("Map @LoadMap(const string &in s)", asFUNCTION( AS_LoadMapFromFile), asCALL_CDECL);
   sm->RegisterGlobalFunction("void SaveMap(Map &in m, const string &in s)", asFUNCTION(AS_SaveMapToFile),  asCALL_CDECL);
   sm->RegisterGlobalFunction("void SaveMapAbsPath(Map &in m, const string &in s)", asFUNCTION(AS_SaveMapToFileAbsolute),  asCALL_CDECL);
   sm->RegisterGlobalFunction("void SaveMap(array<Map> &in m, const string &in s)", asFUNCTION(AS_SaveMapListToFile),  asCALL_CDECL);
   sm->RegisterGlobalFunction("void SaveMapAbsPath(array<Map> &in m, const string &in s)", asFUNCTION(AS_SaveMapListToFileAbsolute),  asCALL_CDECL);
   sm->RegisterGlobalFunction("Map &SaveThisMap(const string &in s)", asFUNCTION( AS_SaveThisMapToFile),  asCALL_CDECL);
   sm->RegisterGlobalFunction("void SetWorkDir(const string &in dir)", asFUNCTION( AS_SetASDefaultPath),  asCALL_CDECL);
   sm->RegisterGlobalFunction("void RestoreWorkDir()", asFUNCTION( AS_RestoreASDefaultPath),  asCALL_CDECL);
   sm->RegisterGlobalFunction("void CopyFile(string dir1, string file1, string dir2, string file2)", asFUNCTION( CopyMap),  asCALL_CDECL);
   sm->RegisterGlobalFunction("void CopyFile(string path1, string path2)", asFUNCTION( CopyMapByPath),  asCALL_CDECL);
   sm->RegisterGlobalFunction("string GetDir()", asFUNCTION( AS_GetASDefaultPath),  asCALL_CDECL);
   sm->RegisterGlobalFunction("string GetDirOrg()", asFUNCTION( AS_GetASDefaultPathOrg),  asCALL_CDECL);
   sm->RegisterGlobalFunction("string GetFileDir(string s)", asFUNCTION( AS_GetFileDir),  asCALL_CDECL);
   sm->RegisterGlobalFunction("string GetFileName(string s)", asFUNCTION( AS_GetFileName),  asCALL_CDECL);
   sm->RegisterGlobalFunction("string GetFileExt(string s)", asFUNCTION( AS_GetFileExt),  asCALL_CDECL);
   sm->RegisterGlobalSTDFunction("array<Map>@ LoadMapBandList(string s)", ( GetFuncConvert(LoadMapBandList)),  asCALL_CDECL);
   sm->RegisterGlobalSTDFunction("array<Map>@ LoadMapBandList(array<string> &in s)", ( GetFuncConvert(LoadMapBandListFromStrings)),  asCALL_CDECL);
   sm->RegisterGlobalSTDFunction("array<string>@ GetMapListNames(string & in name, int count, int start = 0, string between = \"_\")", ( GetFuncConvert(GetMapBandListStrings)),  asCALL_CDECL);
   sm->RegisterGlobalFunction("string GetMapListNames(string & in name, int index_file, string between = \"_\")", asFUNCTION( GetMapBandListString),  asCALL_CDECL);
   sm->RegisterGlobalSTDFunction("array<string>@ GetFilesInDir(string dir = \"\", string filter =  \"\")",(GetFuncConvert(AS_GetFilesInDir)),asCALL_CDECL);
   sm->RegisterGlobalSTDFunction("array<string>@ GetItemsContaining(array<string> &in items, array<string> &in filter, bool allow_copy=false)",(GetFuncConvert(AS_GetItemsContaining)),asCALL_CDECL);

   sm->RegisterGlobalFunction("PointCloud @LoadPointCloudAbsPath(const string &in s)", asFUNCTION( AS_LoadPointCloudFromFileAbsPath), asCALL_CDECL);
   sm->RegisterGlobalFunction("PointCloud @LoadPointCloud(const string &in s)", asFUNCTION( AS_LoadPointCloudFromFile), asCALL_CDECL);
   sm->RegisterGlobalFunction("void SavePointCloud(PointCloud &in sh, const string &in s)", asFUNCTION( AS_SavePointCloudToFile), asCALL_CDECL);
   sm->RegisterGlobalFunction("void SavePointCloudAbsPath(PointCloud &in sh, const string &in s)", asFUNCTION( AS_SavePointCloudToFileAbsPath), asCALL_CDECL);
   sm->RegisterGlobalFunction("PointCloud @SaveThisPointCloud(const string &in s)", asFUNCTION( AS_SaveThisPointCloudToFile),  asCALL_CDECL);

   sm->RegisterGlobalFunction("Field @LoadFieldAbsPath(const string &in s)", asFUNCTION( AS_LoadFieldFromFileAbsPath), asCALL_CDECL);
   sm->RegisterGlobalFunction("Field @LoadField(const string &in s)", asFUNCTION( AS_LoadFieldFromFile), asCALL_CDECL);
   sm->RegisterGlobalFunction("void SaveField(Field &in sh, const string &in s)", asFUNCTION( AS_SaveFieldToFile), asCALL_CDECL);
   sm->RegisterGlobalFunction("void SaveFieldAbsPath(Field &in sh, const string &in s)", asFUNCTION( AS_SaveFieldToFileAbsPath), asCALL_CDECL);
   sm->RegisterGlobalFunction("Field @SaveThisField(const string &in s)", asFUNCTION( AS_SaveThisFieldToFile),  asCALL_CDECL);

   sm->RegisterGlobalSTDFunction("array<string>@ NCDFGetVarNames(const string&in s)",GetFuncConvert(GetVarNamesFromNetCDF));
   sm->RegisterGlobalSTDFunction("array<string>@ NCDFGetDimNames(const string&in s)",GetFuncConvert(GetVarDimNames));
   sm->RegisterGlobalSTDFunction("array<string>@ NCDFGetDimSizes(const string&in s)",GetFuncConvert(GetVarDimSizes));

   sm->RegisterGlobalSTDFunction("void PrintHDF5(const string&in s)",GetFuncConvert(HDF5InfoPrint));
   sm->RegisterGlobalSTDFunction("Map @ReadHDF5Map(const string&in file, const string&in variable)",GetFuncConvert(ReadHDF5Map));


   sm->RegisterGlobalFunction("string ValuesToXMLPost(string xmlname, array<string> &in namelist, array<string> &in valuelist)", asFUNCTION(AS_ValueListToXMLPost),  asCALL_CDECL);
   sm->RegisterGlobalFunction("string OSMExtentToXMLPost(Region b)", asFUNCTION(AS_OSMToXMLPost),  asCALL_CDECL);

   sm->RegisterGlobalFunction("Object @LoadObjectAbsPath(const string &in s)", asFUNCTION( AS_LoadModelFromFileAbsPath), asCALL_CDECL);
   sm->RegisterGlobalFunction("Object @LoadObject(const string &in s)", asFUNCTION( AS_LoadModelFromFile), asCALL_CDECL);
   sm->RegisterGlobalFunction("void SaveObject(Object &in sh, const string &in s)", asFUNCTION( AS_SaveModelToFile), asCALL_CDECL);
   sm->RegisterGlobalFunction("void SaveObjectAbsPath(Object &in sh, const string &in s)", asFUNCTION( AS_SaveModelToFileAbsPath), asCALL_CDECL);
   sm->RegisterGlobalFunction("Object @SaveThisObject(const string &in s)", asFUNCTION( AS_SaveThisModelToFile),  asCALL_CDECL);

   sm->RegisterGlobalFunction("void ExtractFile(const string &in file, const string &in path = \"\")", asFUNCTION( ExtractFileTo),  asCALL_CDECL);

}



#endif // SCRIPTIO_H

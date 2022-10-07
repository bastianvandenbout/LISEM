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
   sm->RegisterGlobalFunction("void CopyFile(string file1, string file2)", asFUNCTION( CopyFile1),  asCALL_CDECL);
   sm->RegisterGlobalFunction("void CopyFile(string dir1, string file1, string dir2, string file2)", asFUNCTION( CopyFile3),  asCALL_CDECL);
   sm->RegisterGlobalFunction("void CopyDir(string path1, string path2)", asFUNCTION( CopyDir),  asCALL_CDECL);
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

   sm->RegisterGlobalFunction("Field @LoadFieldAbsPath(const string &in s, const string &in var = \"data\")", asFUNCTION( AS_LoadFieldFromFileAbsPath), asCALL_CDECL);
   sm->RegisterGlobalFunction("Field @LoadField(const string &in s, const string &in var = \"data\")", asFUNCTION( AS_LoadFieldFromFile), asCALL_CDECL);
   sm->RegisterGlobalFunction("void SaveField(Field &in sh, const string &in s)", asFUNCTION( AS_SaveFieldToFile), asCALL_CDECL);
   sm->RegisterGlobalFunction("void SaveFieldAbsPath(Field &in sh, const string &in s)", asFUNCTION( AS_SaveFieldToFileAbsPath), asCALL_CDECL);
   sm->RegisterGlobalFunction("Field @SaveThisField(const string &in s)", asFUNCTION( AS_SaveThisFieldToFile),  asCALL_CDECL);

   sm->RegisterGlobalSTDFunction("array<string>@ NCDFGetVarNames(string s)",GetFuncConvert(AS_GetNetCDFVars));
   sm->RegisterGlobalSTDFunction("array<string>@ NCDFGetDimNames(string s, string var)",GetFuncConvert(AS_GetNetCDFDims));
   sm->RegisterGlobalSTDFunction("array<int>@ NCDFGetDimSizes(string s, string var)",GetFuncConvert(AS_GetNetCDFDimSize));
   sm->RegisterGlobalSTDFunction("array<Map>@ NCDFGetVarData(string  s, string var)",GetFuncConvert(AS_GetNetCDFData));


   sm->RegisterGlobalSTDFunction("array<string>@ NCDFGetVarNamesAbsPath(string s)",GetFuncConvert(GetVarNamesFromNetCDF));
   sm->RegisterGlobalSTDFunction("array<string>@ NCDFGetDimNamesAbsPath(string s,string var)",GetFuncConvert(GetVarDimNames));
   sm->RegisterGlobalSTDFunction("array<int>@ NCDFGetDimSizesAbsPath(string s, string var)",GetFuncConvert(GetVarDimSizes));

   sm->RegisterGlobalSTDFunction("void PrintHDF5(string s)",GetFuncConvert(HDF5InfoPrint));
   sm->RegisterGlobalSTDFunction("Map @ReadHDF5Map(string file, string variable)",GetFuncConvert(ReadHDF5Map));


   sm->RegisterGlobalFunction("string ValuesToXMLPost(string xmlname, array<string> &in namelist, array<string> &in valuelist)", asFUNCTION(AS_ValueListToXMLPost),  asCALL_CDECL);
   sm->RegisterGlobalFunction("string OSMExtentToXMLPost(Region b)", asFUNCTION(AS_OSMToXMLPost),  asCALL_CDECL);

   sm->RegisterGlobalFunction("Object @LoadObjectAbsPath(const string &in s)", asFUNCTION( AS_LoadModelFromFileAbsPath), asCALL_CDECL);
   sm->RegisterGlobalFunction("Object @LoadObject(const string &in s)", asFUNCTION( AS_LoadModelFromFile), asCALL_CDECL);
   sm->RegisterGlobalFunction("void SaveObject(Object &in sh, const string &in s)", asFUNCTION( AS_SaveModelToFile), asCALL_CDECL);
   sm->RegisterGlobalFunction("void SaveObjectAbsPath(Object &in sh, const string &in s)", asFUNCTION( AS_SaveModelToFileAbsPath), asCALL_CDECL);
   sm->RegisterGlobalFunction("Object @SaveThisObject(const string &in s)", asFUNCTION( AS_SaveThisModelToFile),  asCALL_CDECL);

   sm->RegisterGlobalFunction("void ExtractFile(const string &in file, const string &in path = \"\")", asFUNCTION( ExtractFileTo),  asCALL_CDECL);

   sm->RegisterGlobalSTDFunction("string GetFileAsString(const string &in file)",GetFuncConvert(AS_GetFileString));
   sm->RegisterGlobalSTDFunction("array<string> @GetFileAsStringList(const string &in filename)",GetFuncConvert(AS_ReadFileAsTextListAbsPath));
   sm->RegisterGlobalSTDFunction("string GetFileAsStringAbsPath(const string &in file)",GetFuncConvert(AS_GetFileStringAbsPath));
   sm->RegisterGlobalSTDFunction("array<string> @GetFileAsStringListAbsPath(const string &in filename)",GetFuncConvert(AS_ReadFileAsTextList));

   sm->RegisterGlobalSTDFunction("void WriteToFile(const string &in file, const string &in text)",GetFuncConvert(AS_WriteStringToFile));
   sm->RegisterGlobalSTDFunction("void WriteToFile(const string &in file, const array<string> &in text)",GetFuncConvert(AS_WriteStringListToFile));
   sm->RegisterGlobalSTDFunction("void WriteToFileAbsPath(const string &in file, const string &in text)",GetFuncConvert(AS_WriteStringToFileAbsPath));
   sm->RegisterGlobalSTDFunction("void WriteToFileAbsPath(const string &in file, const array<string> &in text)",GetFuncConvert(AS_WriteStringListToFileAbsPath));

   sm->RegisterGlobalSTDFunction("bool DoesFileExist(const string &in file)",GetFuncConvert( AS_FileExists));
   sm->RegisterGlobalSTDFunction("void ChangeFileName(const string &in file, const string &in newname)",GetFuncConvert( AS_RenameFile));
   sm->RegisterGlobalSTDFunction("void ChangeDirName(const string &in file)",GetFuncConvert( AS_RenameDir));
   sm->RegisterGlobalSTDFunction("void DeleteFile(const string &in file)",GetFuncConvert( AS_DeleteFile));
   sm->RegisterGlobalSTDFunction("void MakeDirectory(const string &in dir, const string &in name)",GetFuncConvert( AS_CreateDir));
   sm->RegisterGlobalSTDFunction("void DeleteDirectory(const string &in dir,const string &in name)",GetFuncConvert( AS_DeleteDir));
   sm->RegisterGlobalSTDFunction("void AddToFile(const string &in file, const string &in text)",GetFuncConvert( AS_AddToFile));
   sm->RegisterGlobalSTDFunction("void AddLineToFile(const string &in file, const string &in text)",GetFuncConvert( AS_AddLineToFile));
   sm->RegisterGlobalSTDFunction("int GetFileSize(const string &in file)",GetFuncConvert( AS_GetFileSize));

   sm->RegisterGlobalSTDFunction("bool DoesFileExistAbsPath(const string &in file)",GetFuncConvert(FileExists));
   sm->RegisterGlobalSTDFunction("void ChangeFileNameAbsPath(const string &in file)",GetFuncConvert( AS_RenameFileAbsPath));
   sm->RegisterGlobalSTDFunction("void ChangeDirNameAbsPath(const string &in file)",GetFuncConvert( AS_RenameDirAbsPath));
   sm->RegisterGlobalSTDFunction("void DeleteFileAbsPath(const string &in file)",GetFuncConvert( AS_DeleteFileAbsPath));
   sm->RegisterGlobalSTDFunction("void MakeDirectoryAbsPath(const string &in dir, const string &in name)",GetFuncConvert( AS_CreateDirAbsPath));
   sm->RegisterGlobalSTDFunction("void DeleteDirectoryAbsPath(const string &in dir, const string &in name)",GetFuncConvert( AS_DeleteDirAbsPath));
   sm->RegisterGlobalSTDFunction("void AddToFileAbsPath(const string &in dir)",GetFuncConvert( AS_AddToFileAbsPath));
   sm->RegisterGlobalSTDFunction("void AddLineToFileAbsPath(const string &in dir)",GetFuncConvert( AS_AddLineToFileAbsPath));
   sm->RegisterGlobalSTDFunction("int GetFileSizeAbsPath(const string &in file)",GetFuncConvert( AS_GetFileSizeAbsPath));



   sm->RegisterGlobalSTDFunction("bool mkdir(const string &in dir)",GetFuncConvert( AS_mkdir));
   sm->RegisterGlobalSTDFunction("bool rmdir(const string &in dir)",GetFuncConvert( AS_rmdir));
   sm->RegisterGlobalSTDFunction("bool rm(const string &in file)",GetFuncConvert( AS_rm));
   sm->RegisterGlobalSTDFunction("bool cd(const string &in dir)",GetFuncConvert( AS_cd));
   sm->RegisterGlobalSTDFunction("bool pwd()",GetFuncConvert( AS_pwd));
   sm->RegisterGlobalSTDFunction("bool cp(const string &in file, const string &in filen)",GetFuncConvert( AS_cp));
   sm->RegisterGlobalSTDFunction("bool mv(const string &in file, const string &in filen)",GetFuncConvert( AS_mv));
   sm->RegisterGlobalSTDFunction("bool filesize(const string &in file)",GetFuncConvert( AS_filesize));
   sm->RegisterGlobalFunction("bool ls()", asFUNCTIONPR(AS_ls,(void),bool),  asCALL_CDECL);
   sm->RegisterGlobalFunction("bool ls(const string &in dir)", asFUNCTIONPR(AS_ls,(QString),bool),  asCALL_CDECL);
   sm->RegisterGlobalFunction("bool ls(const string &in dir,const string &in filter)", asFUNCTIONPR(AS_ls,(QString, QString),bool),  asCALL_CDECL);

}



#endif // SCRIPTIO_H

#ifndef LSMIO_H
#define LSMIO_H

#include "QString"
#include "QFile"
#include "QStringList"
#define LISEM_FOLDER_KERNELS "/kernels/"
#define LISEM_FOLDER_SHADERS "/shaders/"
#define LISEM_FOLDER_ASSETS "/assets/"

#include "geo/shapes/shapefile.h"
#include "geo/raster/map.h"
#include <QString>
#include <QDir>
#include "iogdalshape.h"
#include "iogdal.h"
#include "QFileInfo"
#include "matrixtable.h"
#include "iopdal.h"
#include "geo/pointcloud/pointcloud.h"
#include "qdatetime.h"
#include "models/3dmodel.h"
#include "ioassimp.h"
#include "archive.h"
#include "archive_entry.h"
#include "extensionprovider.h"
#include "geo/raster/field.h"
#include "site.h"
#include "ionetcdf.h"


#define LISEM_IO_ERROR_CANNOT_OPEN -1
#define LISEM_IO_SUCCESS 1

LISEM_API extern QString AS_DIR_Org;
LISEM_API extern QString AS_DIR;
LISEM_API extern std::function<void(QString)> AS_DIRCALLBACK;

inline void SetUIASPathCallBack(std::function<void(QString)> func)
{
    AS_DIRCALLBACK = func;

}

inline void UpdateUIASPath()
{
    std::cout << "UIASPATH update " << (AS_DIRCALLBACK?1:0) << std::endl;

    if(AS_DIRCALLBACK)
    {
        AS_DIRCALLBACK(AS_DIR);
    }
}


inline void SetASDefaultPath(QString path, bool org = true)
{
    AS_DIR = path;
    AS_DIR_Org = path;
}

inline void AS_SetASDefaultPath(QString path)
{
    SetASDefaultPath(QString(path),false);
}

inline void AS_RestoreASDefaultPath()
{
    SetASDefaultPath(AS_DIR_Org,false);
}

inline QString AS_GetASDefaultPathOrg()
{
    return AS_DIR_Org;
}

inline QString AS_GetASDefaultPath()
{
    return AS_DIR;
}

inline QString GetASDefaultPathOrg()
{
    return AS_DIR_Org;
}

inline QString GetASDefaultPath()
{
    return AS_DIR;
}


inline QStringList GetFileStringList(QString path)
{
    QStringList res;

    QFile fin(path);
    if (!fin.open(QFile::ReadOnly | QFile::Text)) {
        return  res;
    }

    while (!fin.atEnd())
    {
        QString S = fin.readLine().trimmed();
        res.append(S);
    }

    return res;
}

inline bool OverWriteFileFromStringList(QString path, QStringList l)
{
    // write data
    QFile fOut(path);
    if (fOut.open(QFile::WriteOnly | QFile::Text)) {
      QTextStream s(&fOut);
      for (int i = 0; i < l.size(); ++i)
        s << l.at(i) << '\n';
    } else {

        return false;
    }
    fOut.close();


    return true;

}

inline QString AS_GetFileStringAbsPath(QString path)
{
    QString res;

    QFile fin(path);
    if (!fin.open(QFile::ReadOnly | QFile::Text)) {
        return  res;
    }

    while (!fin.atEnd())
    {
        QString S = fin.readLine().trimmed();
        res.append(S);
    }

    return res;
}
inline QString AS_GetFileString(QString path)
{
    return AS_GetFileStringAbsPath(AS_DIR+path);
}

inline QString GetFileString(QString path)
{
    QString res;

    QFile fin(path);
    if (!fin.open(QFile::ReadOnly | QFile::Text)) {
        return  res;
    }

    while (!fin.atEnd())
    {
        QString S = fin.readLine().trimmed();
        res.append(S);
    }

    return res;
}

inline bool AS_FileExists(QString path)
{
    QFile f(AS_DIR + path);
    if(f.exists())
    {
        return true;
    }else
    {
        return false;
    }
}


inline bool FileExists(QString path)
{
    QFile f(path);
    if(f.exists())
    {
        return true;
    }else
    {
        return false;
    }
}

inline std::vector<QString> AS_GetNetCDFVars(QString file)
{
    return GetVarNamesFromNetCDF(AS_DIR + file);
}

inline std::vector<QString> AS_GetNetCDFDims(QString file, QString variable)
{
    return GetVarDimNames(AS_DIR + file,variable);
}

inline std::vector<int> AS_GetNetCDFDimSize(QString file, QString variable)
{
    return GetVarDimSizes(AS_DIR + file,variable);
}

/*inline std::vector<double> AS_GetNetCDFDimData(QString file, QString variable)
{

}*/


inline std::vector<cTMap *> AS_GetNetCDFData(QString file, QString variable)
{
    std::vector<Field *> res= ReadFieldList(AS_DIR +  file, variable, true);

    if(res.size()> 0)
    {
        return res.at(0)->GetMapList();
    }else
    {
        return {};
    }
}

inline void AS_AddToFileAbsPath(QString path, QString text)
{
    QFile f(path);
    if (f.open(QIODevice::WriteOnly | QIODevice::Append)) {

        f.write(text.toUtf8());
    }else
    {
        LISEMS_ERROR("Could not append to file " + path);
        throw 1;

    }

}

inline void AS_WriteStringToFileAbsPath(QString path, QString text)
{

    QFile fin(path);

    if (fin.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        QTextStream out(&fin);

            out << text;

        out << "";

        fin.close();

    }else
    {
        LISEMS_ERROR("Could not open file " + path);
        throw 1;
    }
}

inline void AS_WriteStringListToFileAbsPath(QString path, std::vector<QString> text)
{
    QFile fin(path);

    if (fin.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        QTextStream out(&fin);
        for(int i = 0; i < text.size(); i++)
        {
            out << text.at(i) << "\n";
        }
        out << "";

        fin.close();
    }else
    {
        LISEMS_ERROR("Could not open file " + path);
        throw 1;
    }


}

inline void AS_WriteStringToFile(QString path, QString text)
{

    std::cout << "write to file " << path.toStdString() << " " << text.toStdString() << std::endl;
    AS_WriteStringToFileAbsPath(AS_DIR + path, text);
}

inline void AS_WriteStringListToFile(QString path, std::vector<QString> text)
{

    AS_WriteStringListToFileAbsPath(AS_DIR + path, text);
}
inline int AS_GetFileSize(QString path)
{

    QFile f(AS_DIR + path);
    if(!f.exists())
    {
        LISEMS_ERROR("Could not check file " + path);
        throw 1;
    }

    return f.size();
}



inline int AS_GetFileSizeAbsPath(QString path)
{

    QFile f(path);
    if(!f.exists())
    {
        LISEMS_ERROR("Could not check file " + path);
        throw 1;
    }

    return f.size();
}


inline void AS_AddToFile(QString path, QString text)
{
    AS_AddToFileAbsPath(AS_DIR + path,text);
}

inline void AS_AddLineToFileAbsPath(QString path, QString text)
{
    QFile f(path);
    if (f.open(QIODevice::WriteOnly | QIODevice::Append)) {

        f.write(("/n"+ text).toUtf8());
    }else
    {
        LISEMS_ERROR("Could not append to file " + path);
        throw 1;

    }

}


inline void AS_AddLineToFile(QString path, QString text)
{
    AS_AddLineToFile(AS_DIR + path,"/n"+ text);
}


inline void AS_DeleteDir(QString path)
{
    if(path.isEmpty())
    {
        LISEMS_ERROR("Could not delete directory with no name");
        throw 1;
    }
    QDir d(AS_DIR+ path);
    if(!d.exists())
    {
        LISEMS_ERROR("Could not delete directory " + path);
        throw 1;
    }

    if(!d.removeRecursively())
    {
        LISEMS_ERROR("Could not delete directory" + path);
        throw 1;
    }
}


inline void AS_DeleteDirAbsPath(QString path)
{
    if(path.isEmpty())
    {
        LISEMS_ERROR("Could not delete directory with no name");
        throw 1;
    }

    QDir d(path);
    if(!d.exists())
    {
        LISEMS_ERROR("Could not delete directory " + path);
        throw 1;
    }

    if(!d.removeRecursively())
    {
        LISEMS_ERROR("Could not delete directory " + path);
        throw 1;
    }
}


inline void AS_CreateDir(QString dir, QString name)
{
    QDir d(AS_DIR + dir);
    d.mkdir(name);
    if(!d.mkdir(name))
    {
        LISEMS_ERROR("Could not create directory " + name);
        throw 1;
    }
}


inline void AS_CreateDirAbsPath(QString dir, QString name)
{
    QDir d(dir);
    d.mkdir(name);
    if(!d.mkdir(name))
   {
       LISEMS_ERROR("Could not create directory " + name);
       throw 1;
   }
}

inline void AS_DeleteFile(QString path)
{
    QFile f(AS_DIR + path);
    if(!f.exists())
    {
        LISEMS_ERROR("Could not delete file " + path);
        throw 1;
    }

    if(!f.remove())
    {
        LISEMS_ERROR("Could not delete file " + path);
        throw 1;
    }
}


inline void AS_DeleteFileAbsPath(QString path)
{
    QFile f(path);
    if(!f.exists())
    {
        LISEMS_ERROR("Could not delete file " + path);
        throw 1;
    }

    if(!f.remove())
    {
        LISEMS_ERROR("Could not delete file " + path);
        throw 1;
    }
}

inline void AS_RenameDir(QString path, QString newname)
{
    QDir d(AS_DIR + path);
    QString name = d.dirName();
    if(!d.exists())
    {
        LISEMS_ERROR("Could not remane dir " + AS_DIR +path);
        throw 1;

    }
    if(d.cdUp())
    {
        if(!d.rename(name,newname))
        {
            LISEMS_ERROR("Could not rename dir  " + path);

        }
    }else
    {
        {
            LISEMS_ERROR("Could not rename dir  " + path);

        }
    }

}

inline void AS_RenameDirAbsPath(QString path, QString newname)
{
    QDir d(path);
    QString name = d.dirName();
    if(!d.exists())
    {
        LISEMS_ERROR("Could not remane dir " + path);
        throw 1;

    }

    if(d.cdUp())
    {
        if(!d.rename(name,newname))
        {
            LISEMS_ERROR("Could not rename dir  " + path);

        }
    }else
    {
        {
            LISEMS_ERROR("Could not rename dir  " + path);

        }
    }

}


inline void AS_RenameFile(QString path, QString newname)
{
    QFile f(AS_DIR + path);
    if(!f.exists())
    {
        LISEMS_ERROR("Could not remane file " + AS_DIR +path);
        throw 1;

    }

    if(!f.rename(newname))
    {
        LISEMS_ERROR("Could not rename file " +AS_DIR + path);

    }
}

inline void AS_RenameFileAbsPath(QString path, QString newname)
{
    QFile f(path);
    if(!f.exists())
    {
        LISEMS_ERROR("Could not remane file " + path);
        throw 1;

    }

    if(!f.rename(newname))
    {
        LISEMS_ERROR("Could not rename file " + path);

    }
}

inline std::vector<QString> AS_ReadFileAsTextListAbsPath(QString path)
{
    std::vector<QString> sl;

    QFile fin(path);
    if (!fin.open(QFile::ReadOnly | QFile::Text)) {

       LISEMS_ERROR("Could not open file " + path);
       throw 1;
        return sl;
    }

    while (!fin.atEnd())
    {
        QString S;
        S = fin.readLine();


        sl.push_back(S);
    }

    return sl;

}

inline std::vector<QString> AS_ReadFileAsTextList(QString path)
{
    return AS_ReadFileAsTextList(AS_DIR+path);

}

inline QStringList ReadFileAsText(QString path, bool trim = true, int * error = nullptr)
{

    QStringList sl;

    QFile fin(path);
    if (!fin.open(QFile::ReadOnly | QFile::Text)) {

        if(!(error == nullptr))
        {
            *error = LISEM_IO_ERROR_CANNOT_OPEN;
        }
        return sl;
    }

    while (!fin.atEnd())
    {
        QString S;
        if(trim)
        {
            S = fin.readLine().trimmed();
        }else {
            S = fin.readLine();
            }

        sl.append(S);
    }

    if(!(error == nullptr))
    {
        *error = LISEM_IO_SUCCESS;
    }

    return sl;

}




inline static bool WriteMessagesToFile(QList<LeveledMessage> &list, QString path)
{
    QFile fin(path);

    if (fin.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        QTextStream out(&fin);
        out << "OpenLISEM Hazard error file " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
        for(int i = 0; i < list.length(); i++)
        {
            out << list.at(i).Message << "\n";
        }
        out << "";

        fin.close();
        return true;
    }
    return false;
}

static int
copy_data(struct archive *ar, struct archive *aw)
{
    int r;
    const void *buff;
    size_t size;
#if ARCHIVE_VERSION_NUMBER >= 3000000
    int64_t offset;
#else
    off_t offset;
#endif

    for (;;) {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF)
            return (ARCHIVE_OK);
        if (r != ARCHIVE_OK)
            return (r);
        r = archive_write_data_block(aw, buff, size, offset);
        if (r != ARCHIVE_OK) {
            return (r);
        }
    }
}

static int
extract(const char *filename,const char *target)
{
  struct archive *a;
  struct archive *ext;
  struct archive_entry *entry;
  int flags;
  int r;

  //Select which attributes we want to restore.
  flags = ARCHIVE_EXTRACT_TIME;
  flags |= ARCHIVE_EXTRACT_PERM;
  flags |= ARCHIVE_EXTRACT_ACL;
  flags |= ARCHIVE_EXTRACT_FFLAGS;

  a = archive_read_new();
  archive_read_support_filter_all(a);
  archive_read_support_format_all(a);
  ext = archive_write_disk_new();
  archive_write_disk_set_options(ext, flags);
  archive_write_disk_set_standard_lookup(ext);
  if ((r = archive_read_open_filename(a, filename, 16384)))
  {
    printf("File name %s\n",filename);
    return 1;
  }

  for (;;) {
    r = archive_read_next_header(a, &entry);
    printf("%s \n",archive_entry_pathname(entry));
    if (r == ARCHIVE_EOF)
      break;
    if (r < ARCHIVE_OK)
    {
      fprintf(stderr, "%s\n", archive_error_string(a));
      printf("Error1 : %s\n",archive_error_string(a));
    if (r < ARCHIVE_WARN)
      return 1;
    }

    const char* currentFile = archive_entry_pathname(entry);
    const std::string fullOutputPath = std::string(target) + currentFile;
    archive_entry_set_pathname(entry, fullOutputPath.c_str());

    r = archive_write_header(ext, entry);

    if (r < ARCHIVE_OK){
      fprintf(stderr, "%s\n", archive_error_string(ext));
      printf("Error2 : %s\n",archive_error_string(ext));
    }
    else if (archive_entry_size(entry) > 0) {

      r = copy_data(a, ext);

      if (r < ARCHIVE_OK){
        fprintf(stderr, "%s\n", archive_error_string(ext));
        printf("Error3 : %s\n",archive_error_string(ext));
      }
      if (r < ARCHIVE_WARN)
        return 1;
    }

    r = archive_write_finish_entry(ext);

    if (r < ARCHIVE_OK){
      fprintf(stderr, "%s\n", archive_error_string(ext));
      printf("Error4 : %s\n",archive_error_string(ext));
    }
    if (r < ARCHIVE_WARN)
      return 1;
  }
  archive_read_close(a);
  archive_read_free(a);
  archive_write_close(ext);
  archive_write_free(ext);
  return 0;
}















inline static void ExtractFileTo(QString name, QString target)
{

    if(extract((AS_DIR + name).toStdString().c_str(),(AS_DIR + target).toStdString().c_str()))
    {
        LISEMS_ERROR("Could not extract file");
        throw 1;
    }


}






inline MatrixTable * AS_LoadTableFromFileSep(const QString &path, const QString &sep)
{
    MatrixTable * t = new MatrixTable();

    t->LoadAs(AS_DIR + QString(path),QString(sep),false);
    return t;
}


inline MatrixTable * AS_LoadTableFromFile(const QString &path)
{
    MatrixTable * t = new MatrixTable();

    t->LoadAsAutoFormat(AS_DIR + QString(path));
    return t;
}

inline void AS_SaveTableToFileSep(MatrixTable * table,const QString &path, const QString &sep)
{
    table->SaveAs(AS_DIR + QString(path),QString(sep));
}

inline void AS_SaveTableToFileAbsPath(MatrixTable * table,const QString &path)
{

    table->SaveAsAutoFormat( QString(path));
}

inline void AS_SaveTableToFile(MatrixTable * table,const QString &path)
{

    table->SaveAsAutoFormat(AS_DIR + QString(path));
}

inline MatrixTable * AS_SaveThisTableToFile(const QString &path)
{

    MatrixTable * _M = new MatrixTable();
    _M->FileName = path;
    _M->writeonassign = true;

    std::function<void(MatrixTable *,QString)> writecall;
    writecall = [](MatrixTable * m, QString p){AS_SaveTableToFile(m,p);};
    _M->writefunc = writecall;

    std::function<MatrixTable *(QString)> readcall;
    readcall = [](QString p){return AS_LoadTableFromFile(p);};
    _M->readfunc = readcall;
    return _M;
}


inline ShapeFile * AS_LoadVectorFromFile(const QString &path)
{
    QString qpath = QString(AS_DIR + path);
    return LoadVector(qpath);
}
inline ShapeFile * AS_LoadVectorFromFileAbsPath(const QString &path)
{
    QString qpath = QString( path);
    return LoadVector(qpath);
}




inline void CopyDir(QString path1, QString path2)
{
    QString src = AS_DIR + path1;
    QString dst = AS_DIR + path2;
    QDir dir(src);
        if (! dir.exists())
            return;

        foreach (QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            QString dst_path = dst + QDir::separator() + d;
            dir.mkpath(dst_path);
            CopyDir(src+ QDir::separator() + d, dst_path);
        }

        foreach (QString f, dir.entryList(QDir::Files)) {
            QFile::copy(src + QDir::separator() + f, dst + QDir::separator() + f);
        }

}
inline void CopyFile3(QString dir, QString file, QString dir2, QString file2)
{
    if(!(dir.endsWith("/") || dir.endsWith("\\")))
    {
        dir = dir + "/";
    }
    if(!(dir2.endsWith("/") || dir2.endsWith("\\")))
    {
        dir2 = dir2 + "/";
    }
    QFileInfo f1(AS_DIR + QString(dir) + QString(file));
    QFileInfo f2(AS_DIR + QString(dir2)  + QString(file2));

    if(!QDir(AS_DIR +QString(dir2)).exists())
    {
        if(!QDir(AS_DIR + QString(dir2)).mkdir(AS_DIR + QString(dir2)))
        {
            LISEMS_DEBUG("Could not create target directory for copy: from " + QString(dir) + QString(file) + "  to: " + QString(dir2)  + QString(file2))
        }
    }
    if(f2.exists())
    {
        QFile(AS_DIR + QString(dir2)  + QString(file2)).remove();
    }
    if(!QFile::copy(AS_DIR + QString(dir)  + QString(file),QString(dir2) + "/" + QString(file2)))
    {
            LISEMS_DEBUG("Could not copy: from " + AS_DIR +  QString(dir)  + QString(file) + "  to: " + AS_DIR + QString(dir2)  + QString(file2))
    }

}

inline void CopyFile1(QString file,  QString file2)
{
    CopyFile3("",file,"",file2);
}


inline void AS_SaveMapToFileAbsolute(cTMap *m, const QString &path)
{

    QString qp = QString(path);

    QString format = "GEOTIFF";


    if(path.length() > 3)
    {
        //try to automatically guess format based on extension
        // .map -> PCRaster
        // .tif -> GEOTIFF
        // .asc -> ASCII Grid

        //else use .tif format


       format = GetGDALDriverForRasterFile(path);


    }

    writeRaster(*m,qp,format);


}

inline void AS_SaveMapToExistingFileBand(cTMap * m, QString path, int band)
{
    QString qp = QString(path);

    QString format = "GEOTIFF";


    if(path.length() > 3)
    {
        //try to automatically guess format based on extension
        // .map -> PCRaster
        // .tif -> GEOTIFF
        // .asc -> ASCII Grid

        //else use .tif format



        format = GetGDALDriverForRasterFile(path);

        if(!GetGDALDriverForRasterFileBandCountRSupport(path,band))
        {
            LISEMS_ERROR("Band index " + QString::number(band) + " is not supported for this format");
            throw 1;
        }
    }

    writeRaster(*m,qp,format,band);

}


inline void AS_SaveMapToFile(cTMap *m, const QString &in_path)
{

    QString path = QString(AS_DIR +in_path).trimmed();

    AS_SaveMapToFileAbsolute(m,path);

}

inline void AS_SaveMapQListToFileAbsolute(QList<cTMap *> bands, const QString &path)
{

    QString qp = QString(path);

    QString format = "GEOTIFF";


    if(path.length() > 3)
    {
        //try to automatically guess format based on extension
        // .map -> PCRaster
        // .tif -> GEOTIFF
        // .asc -> ASCII Grid

        //else use .tif format


        format = GetGDALDriverForRasterFile(path);
    }

    writeRaster(bands,qp,format);
}
inline void AS_SaveMapListToFileAbsolute(std::vector<cTMap*> maps, QString &path)
{


    QList<cTMap*> bands;
    for(int i = 0; i < maps.size(); i++)
    {
        bands.push_back(maps.at(i));
    }


    AS_SaveMapQListToFileAbsolute(bands,path);

}


inline void AS_SaveMapListToFile(std::vector<cTMap*> maps, const QString &in_path)
{

    QString path = QString(AS_DIR +in_path).trimmed();

    AS_SaveMapListToFileAbsolute(maps,path);

}


inline void AS_SaveVectorToFileAbsolute(ShapeFile *m, const QString &path)
{
    QString qp = QString(path);

    QString format = "ESRI Shapefile";


    if(path.length() > 3)
    {
        //try to automatically guess format based on extension
        // .shp -> ESRI Shapefile
        // .kml -> KML

        //else use .shp format


        format = GetGDALDriverForShapeFile(path);

    }

    SaveVector(m,qp,format);

}

inline void AS_SaveVectorToFile(ShapeFile *m, const QString &path)
{
    AS_SaveVectorToFileAbsolute(m,QString(AS_DIR +path));
}

inline ShapeFile * AS_SaveThisVectorToFile(const QString &path)
{
    ShapeFile * _M = new ShapeFile();
    _M->AS_FileName = path;
    _M->AS_writeonassign = true;

    std::function<void(ShapeFile *,QString)> writecall;
    writecall = [](ShapeFile * m, QString p){AS_SaveVectorToFile(m,p);};
    _M->AS_writefunc = writecall;

    std::function<ShapeFile *(QString)> readcall;
    readcall = [](QString p){return AS_LoadVectorFromFile(p);};
    _M->AS_readfunc = readcall;




    return _M;
}


inline QString AS_GetFileDir(QString full_path)
{
    return QFileInfo(QString(full_path)).absolutePath();
}

inline QString AS_GetFileName(QString full_path)
{
    return QFileInfo(QString(full_path)).baseName();
}

inline QString AS_GetFileExt(QString full_path)
{
return ("." + QFileInfo(QString(full_path)).completeSuffix());
}


inline void AS_SaveMapQListToFile(QList<cTMap *> m,QString p) //relative path
{
    AS_SaveMapQListToFileAbsolute(m,AS_DIR + p);
}
inline void AS_SaveMapToBandFile(cTMap *m, QString p,int i) //relative path
{
    AS_SaveMapToExistingFileBand(m,AS_DIR + p,i);
}

inline cTMap * AS_LoadMapFromFileBand(QString p,int i) //relative path
{
     return new cTMap(readRaster(AS_DIR + p,i));

}
inline QList<cTMap *> AS_LoadMapQListFromFile(QString p) //full path
{
    return readRasterList(p);
}

inline cTMap * AS_LoadMapFromAbsFileBand(QString p,int i)//full path
{
    return new cTMap(readRaster(p,i));
}



inline cTMap * AS_LoadMapFromFileAbsPath(const QString &path)
{
    asIScriptContext *ctx = asGetActiveContext();

    try
    {
        QString qpath = QString(path);
        cTMap * _M = new cTMap(readRaster(qpath));
        _M->AS_TotalNBands = readRasterProps(qpath).bands;
        _M->AS_IsBandLoadable = true;
        _M->AS_BandFilePath = path;



        std::function<void(QList<cTMap *>,QString)> writecall3;
        writecall3 = [](QList<cTMap * >m, QString p){AS_SaveMapQListToFile(m,p);};
        _M->AS_writefunclist = writecall3;

        std::function<void(cTMap *,QString, int)> writecall2;
        writecall2 = [](cTMap * m, QString p, int i){AS_SaveMapToBandFile(m,p,i);};
        _M->AS_writebandfunc = writecall2;

        std::function<cTMap *(QString,int )> readcall2;
        readcall2 = [](QString p, int i){return AS_LoadMapFromFileBand(p,i);};
        _M->AS_readbandfunc = readcall2;

        std::function<QList<cTMap *>(QString)> readcall3;
        readcall3 = [](QString p){return AS_LoadMapQListFromFile(p);};
        _M->AS_readallfunc = readcall3;


        std::function<cTMap *(QString,int)> readcall4;
        readcall4 = [](QString p,int i){return AS_LoadMapFromAbsFileBand(p,i);};
        _M->AS_readbandabsfunc = readcall4;

        return _M;
    }catch(...)
    {
        LISEMS_ERROR("Could not read map from: "+ path);
        throw 1;
    }



}


inline cTMap * AS_LoadMapFromFile(const QString &path)
{

    asIScriptContext *ctx = asGetActiveContext();

    try
    {

        QString qpath = QString(AS_DIR + path);
        cTMap * _M = new cTMap(readRaster(qpath));
        _M->AS_TotalNBands = readRasterProps(qpath).bands;
        _M->AS_IsBandLoadable = true;
        _M->AS_BandFilePath = AS_DIR + path;

        std::function<void(QList<cTMap *>,QString)> writecall3;
        writecall3 = [](QList<cTMap * >m, QString p){AS_SaveMapQListToFile(m,p);};
        _M->AS_writefunclist = writecall3;

        std::function<void(cTMap *,QString, int)> writecall2;
        writecall2 = [](cTMap * m, QString p, int i){AS_SaveMapToBandFile(m,p,i);};
        _M->AS_writebandfunc = writecall2;

        std::function<cTMap *(QString,int )> readcall2;
        readcall2 = [](QString p, int i){return AS_LoadMapFromFileBand(p,i);};
        _M->AS_readbandfunc = readcall2;

        std::function<QList<cTMap *>(QString)> readcall3;
        readcall3 = [](QString p){return AS_LoadMapQListFromFile(p);};
        _M->AS_readallfunc = readcall3;


        std::function<cTMap *(QString,int)> readcall4;
        readcall4 = [](QString p,int i){return AS_LoadMapFromAbsFileBand(p,i);};
        _M->AS_readbandabsfunc = readcall4;


        return _M;

    }catch(...)
    {
        LISEMS_ERROR("Could not read map from: "+ path);
        throw 1;
    }


}




inline cTMap * AS_SaveThisMapToFile(const QString &path)
{
    cTMap * _M = new cTMap();
    _M->AS_FileName = path;
    _M->AS_writeonassign = true;
    _M->AS_refcount = 0;

    _M->AS_TotalNBands = readRasterProps(AS_DIR + path).bands;
    _M->AS_IsBandLoadable    = true;
    _M->AS_BandFilePath      = AS_DIR + path;

    std::function<void(cTMap *,QString)> writecall;
    writecall = [](cTMap * m, QString p){AS_SaveMapToFile(m,p);};
    _M->AS_writefunc = writecall;


    std::function<void(QList<cTMap *>,QString)> writecall3;
    writecall3 = [](QList<cTMap * >m, QString p){AS_SaveMapQListToFile(m,p);};
    _M->AS_writefunclist = writecall3;

    std::function<void(cTMap *,QString, int)> writecall2;
    writecall2 = [](cTMap * m, QString p, int i){AS_SaveMapToBandFile(m,p,i);};
    _M->AS_writebandfunc = writecall2;

    std::function<cTMap *(QString)> readcall;
    readcall = [](QString p){return AS_LoadMapFromFile(p);};
    _M->AS_readfunc = readcall;

    std::function<cTMap *(QString,int )> readcall2;
    readcall2 = [](QString p, int i){return AS_LoadMapFromFileBand(p,i);};
    _M->AS_readbandfunc = readcall2;

    std::function<QList<cTMap *>(QString)> readcall3;
    readcall3 = [](QString p){return AS_LoadMapQListFromFile(p);};
    _M->AS_readallfunc = readcall3;


    std::function<cTMap *(QString,int)> readcall4;
    readcall4 = [](QString p,int i){return AS_LoadMapFromAbsFileBand(p,i);};
    _M->AS_readbandabsfunc = readcall4;

    return _M;
}


inline static int GetMapBandCount(const QString &name)
{
    return readRasterBandCount(AS_DIR + name);

}
inline static cTMap * LoadMapBand(const QString &name, int band)
{
    cTMap* bandd = new cTMap(readRaster(AS_DIR + name,band));

    return bandd;
}

inline static std::vector<cTMap *> LoadMapBandList(const QString &name)
{
    QList<cTMap*> bands = readRasterList(AS_DIR + name);


    std::vector<cTMap *> array;
    for(int i = 0; i < bands.length(); i++)
    {
        array.push_back(bands.at(i));
    }

    return array;
}


inline static std::vector<cTMap *> LoadMapBandListFromStrings(std::vector<QString> names)
{

    std::vector<cTMap *> array;

    for(int i = 0; i < names.size(); i++)
    {
        QString name = names.at(i);
        cTMap* band = new cTMap(readRaster(AS_DIR + name));
        if(band == nullptr)
        {

            LISEMS_ERROR("Could not load map " + name);
            throw 1;
        }
        array.push_back(band);
    }

    return array;
}

inline static std::vector<QString> AS_GetItemsContaining(std::vector<QString> items, std::vector<QString> select, bool allow_double = false)
{

    std::vector<QString> res;

    for(int i = 0; i < select.size() ; i++)
    {

        for(int j=0; j < items.size(); j++)
        {

            if(items.at(j).contains(select.at(i)))
            {
                if(allow_double)
                {
                    res.push_back(items.at(j));
                }else
                {
                    bool found = false;
                    for(int k = 0; k < res.size(); k++)
                    {
                        if(res.at(k) == items.at(j))
                        {
                            found = true;
                            break;

                        }
                    }

                    if(!found)
                    {
                        res.push_back(items.at(j));
                    }

                }
            }
        }
    }
    return res;
}


inline static std::vector<QString> AS_GetFilesInDir(QString dir, QString filter)
{
    QDir d(AS_DIR + dir);

    if(filter.isEmpty())
    {
        filter = "*.*";
    }
    QString rel = d.relativeFilePath(dir);
    QStringList files = d.entryList({filter});

    std::vector<QString> array;

    for(int i = 0; i < files.length(); i++)
    {
        array.push_back(QString(rel +"/" + files.at(i)));
    }


    return array;
}

inline static QString GetMapBandListString(const QString & name, int count, QString between)
{


    int indexdot =name.lastIndexOf(".");
    if(indexdot < 0)
    {
        indexdot = name.length();
    }
    QString firstpart = name.left(indexdot);
    QString lastpart = name.right(name.length() - indexdot);


    QString ret =QString(firstpart + between + QString::number(count)+lastpart);
    return ret;
}
inline static std::vector<QString> GetMapBandListStrings(const QString & name, int count, int start, QString between)
{
    std::vector<QString> array;

    int indexdot =name.lastIndexOf(".");
    if(indexdot < 0)
    {
        indexdot = name.length();
    }
    QString firstpart = name.left(indexdot);
    QString lastpart = name.right(name.length() - indexdot);

    for(int i = 0; i < count; i++)
    {
        array.push_back(QString(firstpart + between + QString::number(start+i)+lastpart));
    }

    return array;
}


inline static PointCloud * AS_LoadPointCloudFromFileAbsPath(const QString & name)
{
    PointCloud * ret = ReadPointCloud(name);
    if(ret == nullptr)
    {
        LISEMS_ERROR("Could not read Point Cloud from file: " + name);
        LISEM_ERROR("Could not read Point Cloud from file: " + name);
        throw 1;
    }else {
        return ret;
    }
}

inline static PointCloud * AS_LoadPointCloudFromFile(const QString & name)
{
    return AS_LoadPointCloudFromFileAbsPath(AS_DIR + name);
}

inline static void AS_SavePointCloudToFileAbsPath(PointCloud * pc, const QString & name)
{

    //call writing function for point cloud
    WritePointCloud(pc,name);

}
inline static void AS_SavePointCloudToFile(PointCloud * pc,const QString & name)
{
    AS_SavePointCloudToFileAbsPath(pc,AS_DIR + name);
}

inline static PointCloud * AS_SaveThisPointCloudToFile(const QString & path)
{
    PointCloud * _M = new PointCloud();
    _M->AS_FileName = path;
    _M->AS_writeonassign = true;

    std::function<void(PointCloud *,QString)> writecall;
    writecall = [](PointCloud * m, QString p){AS_SavePointCloudToFile(m,p);};
    _M->AS_writefunc = writecall;

    std::function<PointCloud *(QString)> readcall;
    readcall = [](QString p){return AS_LoadPointCloudFromFile(p);};
    _M->AS_readfunc = readcall;
    return _M;
}


#include "ionetcdf.h"



inline static Field * AS_LoadFieldFromFileAbsPath(const QString & name, QString variable = "")
{
    std::vector<Field *> ret = ReadFieldList(name,variable, true);
    if(ret.size() == 0)
    {
        LISEMS_ERROR("Could not read Field from file: " + name);
        LISEM_ERROR("Could not read Field from file: " + name);
        throw 1;
    }else {
        return ret[0];
    }
}

inline static Field * AS_LoadFieldFromFile(const QString & name, QString variable = "")
{
    return AS_LoadFieldFromFileAbsPath(AS_DIR + name);
}

inline static void AS_SaveFieldToFileAbsPath(Field * pc, const QString & name)
{

    //call writing function for point cloud
    WriteFieldList({pc},{0.0},name);

}
inline static void AS_SaveFieldToFile(Field * pc,const QString & name)
{
    std::cout << "save field to file " << std::endl;
    AS_SaveFieldToFileAbsPath(pc,AS_DIR + name);
}

inline static Field * AS_SaveThisFieldToFile(const QString & path)
{
    Field * _M = new Field();
    _M->AS_FileName = path;
    _M->AS_writeonassign = true;

    std::function<void(Field *,QString)> writecall;
    writecall = [](Field * m, QString p){AS_SaveFieldToFile(m,p);};
    _M->AS_writefunc = writecall;

    std::function<Field *(QString)> readcall;
    readcall = [](QString p){return AS_LoadFieldFromFile(p);};
    _M->AS_readfunc = readcall;
    return _M;
}


class ModelGeometry;


LISEM_API ModelGeometry * AS_LoadModelFromFileAbsPath(const QString & name);
LISEM_API ModelGeometry * AS_LoadModelFromFile(const QString & name);
LISEM_API void AS_SaveModelToFileAbsPath(ModelGeometry * pc, const QString & name);
LISEM_API void AS_SaveModelToFile(ModelGeometry * pc,const QString & name);
LISEM_API ModelGeometry * AS_SaveThisModelToFile(const QString & path);


inline static QStringList GetSupportedFileTypeExtensionsFilter()
{
    return GetAllExtensionsFilter();
}

inline static QStringList GetSupportedFileTypeExtensions()
{
    return GetAllExtensions();

}


inline static QString AS_ValueListToXMLPost(QString name, std::vector<QString> sl1, std::vector<QString> sl2)
{

    if(sl1.size() != sl2.size())
    {
        LISEMS_ERROR("length of names and value lists is not equal");
        throw 1;
    }

    QString doc=  "<"+ name + ">";

    for(int i = 0; i < sl1.size(); i++)
    {
        doc+= "    <" + sl1.at(i) + "  value=" + sl2.at(i) + ">";
    }

    doc +=  "<\\"+ name + ">";
    return doc;

}
inline static QString AS_OSMToXMLPost(BoundingBox b)
{
    QString xmlData = "<osm-script timeout=\"10\">";
    xmlData += "<union into=\"_\">";
    xmlData += "<bbox-query e=\"";
    xmlData += QString::number(b.GetMaxX());
    xmlData += "\" n=\"";
    xmlData += QString::number(b.GetMaxY());
    xmlData +="\" s=\"";
    xmlData += QString::number(b.GetMinY());
    xmlData += "\" w=\"";
    xmlData += QString::number(b.GetMinX());
    xmlData +="\"/>";
    xmlData += "<recurse type=\"up\"/><recurse type=\"down\"/>";
    xmlData += "</union><print limit=\"\" mode=\"meta\" order=\"id\"/>";
    xmlData += "</osm-script>";

    QString data = xmlData;

    std::cout << "return OSM data: " << data.toStdString() << std::endl;

    return data;
}


//linux style commands for easy navigation
inline bool AS_mkdir(QString dir)
{
    QDir d = QDir(AS_DIR);
    return d.mkdir(dir);
}
inline bool AS_rmdir(QString dir)
{
    QDir d = QDir(AS_DIR);
    return d.rmdir(dir);
}
inline bool AS_rm(QString file)
{
    return QFile::remove(file);
}
inline bool AS_cd(QString dir)
{

    QDir d = QDir(AS_DIR);
    bool done=d.cd(dir);
    if(done )
    {

        AS_DIR_Org = d.absolutePath();
        AS_DIR = d.absolutePath();
        UpdateUIASPath();


        return true;
    }else
    {
        LISEM_STATUS("Could not open directory: " + dir);
        return false;
    }
}
inline QString AS_pwd()
{
    LISEMS_STATUS("wd: " + AS_DIR);
    return AS_DIR;
}
inline bool AS_cp(QString file, QString filen)
{
    return QFile::copy(AS_DIR + file,AS_DIR+ filen);
}
inline bool AS_mv(QString file, QString filen)
{
    return QFile::rename(file,filen);
}
inline bool AS_grep(QString file, QString text)
{
    //TODO
    //open all files in dir and search for text
    //print all results
    //this can take very very long if used/implemented unwisely
    return false;
}
inline uint64_t AS_filesize(QString file)
{
    QFile f = QFile(file);
    if(f.exists())
    {
        return f.size();
    }else
    {
        return 0;
    }

}
inline bool AS_ls()
{
    QDir d(AS_DIR);

    if(!d.exists())
    {
        return false;
    }
        QString filter = "*.*";
    QStringList files = d.entryList({filter});

    for(int i =0; i < files.length(); i++)
    {
        LISEMS_STATUS(files[i]);
    }
    return true;
}

inline bool AS_ls(QString dir)
{
    QDir d(AS_DIR + dir);

    if(!d.exists())
    {
        return false;
    }
    QString filter = "*.*";
    QString rel = d.relativeFilePath(dir);
    QStringList files = d.entryList({filter});

    for(int i =0; i < files.length(); i++)
    {
        LISEMS_STATUS(files[i]);
    }
    return true;
}
inline bool AS_ls(QString dir, QString filter)
{
    QDir d(AS_DIR + dir);
    if(!d.exists())
    {
        return false;
    }
    if(filter.isEmpty())
    {
        filter = "*.*";
    }
    QString rel = d.relativeFilePath(dir);
    QStringList files = d.entryList({filter});

    for(int i =0; i < files.length(); i++)
    {
        LISEMS_STATUS(files[i]);
    }
    return true;
}

#endif // LSMIO_H

#include "lsmio.h"


QString AS_DIR_Org;
QString AS_DIR;



ModelGeometry * AS_LoadModelFromFileAbsPath(const QString & name)
{
    ModelGeometry * ret = Read3DModel(name);
    if(ret == nullptr)
    {
        LISEMS_ERROR("Could not read Object from file: " + name);
        LISEM_ERROR("Could not read Object from file: " + name);
        throw 1;
    }else {
        return ret;
    }
}

ModelGeometry * AS_LoadModelFromFile(const QString & name)
{
    return AS_LoadModelFromFileAbsPath(AS_DIR + name);
}

void AS_SaveModelToFileAbsPath(ModelGeometry * pc, const QString & name)
{

    //call writing function for point cloud
    Write3DModel(pc,name);

}
void AS_SaveModelToFile(ModelGeometry * pc,const QString & name)
{
    AS_SaveModelToFileAbsPath(pc,AS_DIR + name);
}

ModelGeometry * AS_SaveThisModelToFile(const QString & path)
{
    ModelGeometry * _M = new ModelGeometry();
    _M->AS_FileName = path;
    _M->AS_writeonassign = true;

    std::function<void(ModelGeometry *,QString)> writecall;
    writecall = [](ModelGeometry * m, QString p){AS_SaveModelToFile(m,p);};
    _M->AS_writefunc = writecall;

    std::function<ModelGeometry *(QString)> readcall;
    readcall = [](QString p){return AS_LoadModelFromFile(p);};
    _M->AS_readfunc = readcall;
    return _M;
}



#ifndef OPENGLCLDATAMANAGER_H
#define OPENGLCLDATAMANAGER_H




#include "defines.h"
#include "openglclmanager.h"
#include "lsmio.h"
#include "site.h"

LISEM_API extern OpenGLProgram * GLProgram_uimap;
LISEM_API extern OpenGLProgram * GLProgram_uiduomap;
LISEM_API extern OpenGLProgram * GLProgram_uimapshape;
LISEM_API extern OpenGLProgram * GLProgram_uimultimap;
LISEM_API extern OpenGLProgram * GLProgram_uivectorl;
LISEM_API extern OpenGLProgram * GLProgram_uivectorp;
LISEM_API extern OpenGLProgram * GLProgram_uipointsimple;
LISEM_API extern OpenGLProgram * GLProgram_particles;
LISEM_API extern OpenGLProgram * GLProgram_pointcloud;
LISEM_API extern OpenGLProgram * GLProgram_pointcloud3d;
LISEM_API extern OpenGLProgram * GLProgram_uiterrain;
LISEM_API extern OpenGLProgram * GLProgram_uiterrainlayer;
LISEM_API extern OpenGLProgram * GLProgram_uiocean;
LISEM_API extern OpenGLProgram * GLProgram_uiobject;
LISEM_API extern OpenGLProgram * GLProgram_uiobjectinstanced;

LISEM_API extern QString KernelDir;
LISEM_API extern QString AssetDir;

inline static void InitUIShaders(OpenGLCLManager * m)
{
    KernelDir = GetSite() + LISEM_FOLDER_SHADERS;
    AssetDir = GetSite() + LISEM_FOLDER_ASSETS;


    GLProgram_uimap = m->GetMGLProgram(KernelDir+ "UIMapDraw.vert", KernelDir+ "UIMapDraw.frag");
    GLProgram_uiobject = m->GetMGLProgram(KernelDir+ "UIObjectDraw.vert", KernelDir+ "UIObjectDraw.frag");
    GLProgram_uiobjectinstanced = m->GetMGLProgram(KernelDir+ "UIObjectDrawInstanced.vert", KernelDir+ "UIObjectDrawInstanced.frag");
    GLProgram_uiduomap = m->GetMGLProgram(KernelDir+ "UIDuoMapDraw.vert", KernelDir+ "UIDuoMapDraw.frag");
    GLProgram_uimapshape = m->GetMGLProgram(KernelDir+ "UIMapDrawShape.vert", KernelDir+ "UIMapDrawShape.frag");
    GLProgram_uimultimap = m->GetMGLProgram(KernelDir+ "UIMultiMapDraw.vert", KernelDir+ "UIMultiMapDraw.frag");
    GLProgram_uivectorl = m->GetMGLProgram(KernelDir+ "UIGeoLineDraw.vert", KernelDir+ "UIGeoLineDraw.frag");
    GLProgram_uivectorp = m->GetMGLProgram(KernelDir+ "UIGeoPolygonDraw.vert", KernelDir+ "UIGeoPolygonDraw.frag");
    GLProgram_particles = m->GetMGLProgram(KernelDir + "partsim.vert", KernelDir + "partsim.frag");
    GLProgram_pointcloud = m->GetMGLProgram(KernelDir + "UIGeoPointCloud.vert", KernelDir + "UIGeoPointCloud.frag");
    GLProgram_pointcloud3d = m->GetMGLProgram(KernelDir + "UIGeo3DPointCloud.vert", KernelDir + "UIGeo3DPointCloud.frag");
    GLProgram_uipointsimple = m->GetMGLProgram(KernelDir+ "UIGeoSimplePointDraw.vert", KernelDir+ "UIGeoSimplePointDraw.frag");
    GLProgram_uiterrain = m->GetMGLProgram(KernelDir + "terrain_vs.glsl", KernelDir + "terrain_fs.glsl", KernelDir + "terrain_gs.glsl", KernelDir + "terrain_tc.glsl", KernelDir + "terrain_te.glsl");
    GLProgram_uiterrainlayer = m->GetMGLProgram(KernelDir + "terrain_vs_layer.glsl", KernelDir + "terrain_fs_layer.glsl", KernelDir + "terrain_gs_layer.glsl", KernelDir + "terrain_tc_layer.glsl", KernelDir + "terrain_te_layer.glsl");
    GLProgram_uiocean = m->GetMGLProgram(KernelDir + "terrain_vs_ocean.glsl", KernelDir + "terrain_fs_ocean.glsl", KernelDir + "terrain_gs_ocean.glsl", KernelDir + "terrain_tc_ocean.glsl", KernelDir + "terrain_te_ocean.glsl");

}

inline static void DestroyUIShaders(OpenGLCLManager * m)
{
    KernelDir = GetSite() + LISEM_FOLDER_SHADERS;


    SAFE_DELETE(GLProgram_uimap);
    SAFE_DELETE(GLProgram_uiobject);
    SAFE_DELETE(GLProgram_uiobjectinstanced);
    SAFE_DELETE(GLProgram_uiduomap);
    SAFE_DELETE(GLProgram_uimapshape);
    SAFE_DELETE(GLProgram_uimultimap);
    SAFE_DELETE(GLProgram_uivectorl);
    SAFE_DELETE(GLProgram_uivectorp);
    SAFE_DELETE(GLProgram_particles);
    SAFE_DELETE(GLProgram_pointcloud);
    SAFE_DELETE(GLProgram_pointcloud3d);
    SAFE_DELETE(GLProgram_uipointsimple);
    SAFE_DELETE(GLProgram_uiterrain);
    SAFE_DELETE(GLProgram_uiterrainlayer);
    SAFE_DELETE(GLProgram_uiocean);

}



#endif // OPENGLCLDATAMANAGER_H

#pragma once

#include "linear/lsm_vector3.h"
#include "linear/lsm_vector2.h"


#include "linear/lsm_matrix3x3.h"
#include "linear/lsm_matrix4x4.h"
#include "boundingbox.h"
#include "vertex.h"
#include "defines.h"

//#include "scriptarray.h"
//#include "shape.h"

#define f32 float
#define f64 float

class Texture;

typedef struct Texture
{
    QString m_Path;
    QString m_Type;
    int MapU;
    int MapV;
    int Operator;
    float Strength;


} Texture;

class ModelMaterial
{
public:

    LSMVector4 Color_Diffuse = LSMVector4(1.0,0.0,0.0,1.0);
    LSMVector4 Color_Ambient = LSMVector4(0.0,1.0,0.0,1.0);
    LSMVector4 Color_Specular = LSMVector4(0.0,0.0,1.0,1.0);
    LSMVector4 Color_Shininess = LSMVector4(0.0,0.0,0.0,1.0);
    LSMVector4 Color_Emissive = LSMVector4(0.0,0.0,0.0,1.0);
    LSMVector4 Color_Transparent = LSMVector4(0.0,0.0,0.0,1.0);
    LSMVector4 Color_Reflective = LSMVector4(0.0,0.0,0.0,1.0);
    float Shininess = 0.0;
    float BumpScaling = 1.0;
    float Reftractivity = 0.0;
    float Opacity = 1.0;
    float TransparancyFactor = 0.0;
    float ShininessStrength = 1.0;

    std::vector<Texture> diffuseMaps;
    std::vector<Texture> specularMaps;
    std::vector<Texture> ambientMaps;
    std::vector<Texture> normalMaps;
    std::vector<Texture> heightMaps;
    std::vector<Texture> emissiveMaps;
    std::vector<Texture> shininessMaps;
    std::vector<Texture> opacityMaps;
    std::vector<Texture> displacementMaps;
    std::vector<Texture> lightMaps;
    std::vector<Texture> reflectionMaps;


    std::vector<Texture> textures;


    inline void CopyTo(std::vector<Texture> & source,std::vector<Texture> & target)
    {
        target.clear();
        for(int i = 0; i < source.size(); i++)
        {
            Texture t = Texture();
            t.m_Path = source.at(i).m_Path;
            t.m_Type = source.at(i).m_Type;
            t.MapU = source.at(i).MapU;
            t.MapV = source.at(i).MapV;
            t.Operator = source.at(i).Operator;
            t.Strength = source.at(i).Strength;

            target.push_back(t);
        }

    }

    inline ModelMaterial Copy()
    {

        ModelMaterial ret;
        CopyTo( diffuseMaps, (ret.diffuseMaps));
        CopyTo( specularMaps, (ret.specularMaps));
        CopyTo( ambientMaps, (ret.ambientMaps));
        CopyTo( normalMaps, (ret.normalMaps));
        CopyTo( heightMaps, (ret.heightMaps));
        CopyTo( emissiveMaps, (ret.emissiveMaps));
        CopyTo( shininessMaps, (ret.shininessMaps));
        CopyTo( opacityMaps, (ret.opacityMaps));
        CopyTo( displacementMaps, (ret.displacementMaps));
        CopyTo( lightMaps, (ret.lightMaps));
        CopyTo( reflectionMaps, (ret.reflectionMaps));

        ret.Color_Diffuse = Color_Diffuse;
        ret.Color_Ambient = Color_Ambient;
        ret.Color_Specular = Color_Specular;
        ret.Color_Shininess = Color_Shininess;
        ret.Color_Emissive = Color_Emissive;
        ret.Color_Transparent = Color_Transparent;
        ret.Color_Reflective = Color_Reflective;
        ret.Shininess = Shininess;
        ret.BumpScaling = BumpScaling ;
        ret.Reftractivity = Reftractivity ;
        ret.Opacity = Opacity;
        ret.TransparancyFactor = TransparancyFactor;
        ret.ShininessStrength = ShininessStrength;

        return ret;
    }

};

typedef struct Face{

    int i_1 = 0;
    int i_2 = 0;
    int i_3 = 0;
} Face;

class LISEM_API LSMMesh
{
public:
    // mesh data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    LSMMatrix4x4 * m_InstanceMatrices = nullptr;

    BoundingBox3D m_BoundingBox;
    bool m_IsSetup = false;
    bool m_IsInstanced = false;
    bool m_IsConvex = false;

    ModelMaterial m_Material;
    int m_MaterialIndex = -1;

    unsigned int VAO, VBO, EBO, InstanceMBO;
    int m_NumInstances = 0;
    inline LSMMesh()
    {

    }


    BoundingBox3D GetBoundingBox(bool calc= false);
    BoundingBox3D GetAndCalcBoundingBox();

     LSMMesh GetCopy();
     LSMMesh(std::vector<Vertex> in_vertices, std::vector<unsigned int> in_indices, ModelMaterial m = ModelMaterial(), int mindex = -1);

     int GetVertexCount();

     int GetFaceCount();

     Vertex GetVertex(int i);
     Face GetFace(int i);
     void SetMaterial(ModelMaterial m);
     float RayCast(LSMVector3 O, LSMVector3 Dir);

     void CalculateNormalsAndTangents();
     void CalcIsConvex();
     void SetIsConvex(bool conv);
     void CalculateNormals();
     void CalculateTangents();
     void SetAsEllipsoidTruncated(f32 radiusH, f32 radiusV, f32 Ylow, f32 Yhigh, f32 offset, unsigned int polyCountX, unsigned int polyCountY);

     void SetAsCube(LSMVector3 size = LSMVector3(1.0,1.0,1.0), LSMVector3 center = LSMVector3(0.0,0.0,0.0));


     void SetAsCone(float radius_top, float radius_low, float length, unsigned int tesselation);
     void Rotate(LSMVector3 axis, double angle);

     bool IsSetup();

    int GetNumInstances();

    //check for convexity by determining all face planes, and checking if all other points are behind this plane
    bool int_Calc_IsConvex(float threshold = 0.001);
    double CalculateSurface();
    double CalculateProjectedSurface(LSMVector3 shape_loc,LSMVector2 Velocity,BoundingBox extent, float h, float dhdx, float dhdy, float z, float dzdx, float dzdy, bool do_concave_detailed = false);
    double CalculateVolume(double density = 1.0, LSMVector3 *center = nullptr);
    double CalculateVolumeWithinSurfaceFlowBox(LSMVector3 shape_loc, BoundingBox extent, float h, float dhdx, float dhdy, float z, float dzdx, float dzdy, LSMVector3 * center = nullptr);


    //Angelscript related functions
    public:

        QString        AS_FileName          = "";
        bool           AS_writeonassign     = false;
        std::function<void(LSMMesh *,QString)> AS_writefunc;
        std::function<LSMMesh *(QString)> AS_readfunc;
        bool           AS_Created           = false;
        int            AS_refcount          = 1;
        void           AS_AddRef            ();
        void           AS_ReleaseRef        ();
        LSMMesh*          AS_Assign            (LSMMesh*);



        /*int AS_GetTriangeCount          ();
        AS_LSMShape* AS_GetTriangle         (int index);
        void AS_AddTriangle               (AS_LSMShape * );
        void AS_RemoveTriangle            (int index);
        int AS_GetTextureCount             ();
        CScriptArray * AS_GetTexture (int index);*/


};


//Angelscript related functionality

inline void LSMMesh::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void LSMMesh::AS_ReleaseRef()
{
    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {
        //Destroy();
        delete this;
    }

}

inline LSMMesh* LSMMesh::AS_Assign(LSMMesh* sh)
{
    //Destroy();


    return this;
}


inline static LSMMesh * AS_MeshFactory()
{
    return new LSMMesh();
}

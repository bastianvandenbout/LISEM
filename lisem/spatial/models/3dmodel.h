#pragma once

#include "defines.h"
#include "lsmmesh.h"
#include "linear/lsm_vector3.h"
#include "linear/lsm_vector2.h"
#include "linear/lsm_vector4.h"
#include "linear/lsm_matrix4x4.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include "error.h"
#include "QFileInfo"
#include <assimp/pbrmaterial.h>
#include "QDir"
#include "boundingbox.h"
//#include "scriptarray.h"




class LSMMesh;

typedef struct ModelRayCastResult
{
    LSMMesh * m_Mesh = nullptr;
    LSMVector3 m_Position;

} ModelRayCastResult;

class LISEM_API ModelGeometry
{

    BoundingBox3D m_BoundingBox;
    std::vector<Texture> textures_loaded;

    std::vector<ModelMaterial> materials;
    QString dir = "";
    QString path = "";
public:

    std::vector<LSMMesh> meshes;

    inline ModelGeometry()
    {

    }

    //copy constructor
    inline ModelGeometry(const ModelGeometry &m)
    {
        m_BoundingBox = m.m_BoundingBox;
        meshes = m.meshes;
        dir = m.dir;
        path = m.path;
        textures_loaded = m.textures_loaded;

        for(int i = 0; i < m.textures_loaded.size(); i++)
        {
            textures_loaded.push_back(m.textures_loaded.at(i));
        }
        textures_loaded = m.textures_loaded;


    }
    inline ModelGeometry * GetCopy()
    {
        ModelGeometry * geom = new ModelGeometry();
        geom->dir = dir;
        geom->path = path;
        geom->textures_loaded = textures_loaded;
        geom->m_BoundingBox = m_BoundingBox;

        for(int i = 0; i < meshes.size(); i++)
        {
            geom->meshes.push_back(meshes.at(i).GetCopy());
        }

        return geom;

    }

    void AddMesh(LSMMesh m)
    {
        meshes.push_back(m);
    }

    void loadModel(QString path)
    {
        Assimp::Importer import;

        const aiScene *scene = import.ReadFile(path.toStdString().c_str(), aiProcess_CalcTangentSpace| aiProcess_GenNormals|aiProcess_JoinIdenticalVertices| aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_FlipUVs | aiProcess_SortByPType);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            LISEM_ERROR("Assimp model import error: " + path + import.GetErrorString());
            return;
        }

        QFileInfo f(path);
        dir = f.dir().path();
        path= path;

        processMaterials(scene);

        processNode(scene->mRootNode, scene);


    }

    inline BoundingBox3D GetBoundingBox(bool calc= false)
    {
        BoundingBox3D bf;
        for(int i = 0; i < meshes.size(); i++)
        {
            BoundingBox3D b = meshes.at(i).GetBoundingBox(calc);
            if(i == 0)
            {
                bf= b;
            }else
            {
                bf.Merge(b);
            }

        }
        m_BoundingBox = bf;
        std::cout << "get bounding box "<< bf.GetMinX() << " " << bf.GetMaxX() << " " << bf.GetMinZ() << "  " << bf.GetMaxZ() <<  std::endl;
        return m_BoundingBox;
    }

    inline BoundingBox3D GetAndCalcBoundingBox()
    {

        return GetBoundingBox(true);

    }

    ModelRayCastResult RayCast(LSMVector3 O, LSMVector3 Dir, LSMMatrix4x4 ObjtoWorld, LSMMatrix4x4 WorldtoObj)
    {
        LSMVector3 O_o = WorldtoObj * O;
        LSMVector3 O_opd = WorldtoObj * (O+Dir);
        LSMVector3 O_D = (O_opd - O_o).Normalize();

        //raycast in local coordinates against all meshes

        bool found = false;
        double dist_min = 0.0;
        LSMMesh * mesh = nullptr;
        for(int i = 0; i < meshes.size(); i++)
        {
            float dist = meshes.at(i).RayCast(O_o,O_D);
            if(std::isfinite(dist))
            {
                if(found == false)
                {
                    found = true;
                    mesh = &meshes.at(i);
                    dist_min = dist;
                }else if(dist <dist_min)
                {
                    mesh = &meshes.at(i);
                    dist_min = dist;
                }
            }
        }


        ModelRayCastResult res;
        res.m_Mesh = mesh;
        res.m_Position = ObjtoWorld * (O_o+O_D * dist_min);
        return res;
    }

    inline bool IsPointInside(LSMVector3 O)
    {
        //extension of algorithm for polygon
        //we do a ray-triangle intersection from the point in any direction and count the number of intersections
        //if the mesh is closed, an even number of intersections means outside, and an uneven number means inside the object.
        //if we use this for rasterization of 3d object spaces, our ray-tracing better be fast!!
        for(int i = 0; i < meshes.size(); i++)
        {
            bool in = meshes.at(i).IsPointInside(O);
            if(in)
            {
                return true;
            }
        }
        return false;

    }


    void processNode(aiNode *node, const aiScene *scene)
    {
        // process all the node's meshes (if any)
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // then do the same for each of its children
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    void processMaterials(const aiScene *scene)
    {
        for(int i= 0; i < scene->mNumMaterials; i++)
        {
            // process materials
            aiMaterial* material = scene->mMaterials[i];

            // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
            // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
            // Same applies to other texture as the following list summarizes:
            // diffuse: texture_diffuseN
            // specular: texture_specularN
            // normal: texture_normalN

            ModelMaterial m;

            aiColor3D color(0.f, 0.f, 0.f);
            float shininess;

            material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            m.Color_Diffuse = LSMVector4(color.r, color.b, color.g,1.0);

            material->Get(AI_MATKEY_COLOR_AMBIENT, color);
            m.Color_Ambient = LSMVector4(color.r, color.b, color.g,1.0);

            material->Get(AI_MATKEY_COLOR_SPECULAR, color);
            m.Color_Specular = LSMVector4(color.r, color.b, color.g,1.0);

            material->Get(AI_MATKEY_SHININESS, shininess);
            m.Shininess = shininess;

            // 1. diffuse maps
            m.diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            m.textures.insert(m.textures.end(), m.diffuseMaps.begin(), m.diffuseMaps.end());
            // 2. specular maps
            m.specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            m.textures.insert(m.textures.end(), m.specularMaps.begin(), m.specularMaps.end());
            //
            m.ambientMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ambient");
            m.textures.insert(m.textures.end(), m.ambientMaps.begin(), m.ambientMaps.end());

            // 3. normal maps
            m.normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
            m.textures.insert(m.textures.end(), m.normalMaps.begin(), m.normalMaps.end());
            // 4. height maps
            m.heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
            m.textures.insert(m.textures.end(), m.heightMaps.begin(), m.heightMaps.end());
            // 4. Emissive maps
            m.emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emissive");
            m.textures.insert(m.textures.end(), m.emissiveMaps.begin(), m.emissiveMaps.end());
            // 4. Shininess maps
            m.shininessMaps = loadMaterialTextures(material, aiTextureType_SHININESS, "texture_shininess");
            m.textures.insert(m.textures.end(), m.shininessMaps.begin(), m.shininessMaps.end());
            // 4. Opacity maps
            m.opacityMaps = loadMaterialTextures(material, aiTextureType_OPACITY, "texture_opacity");
            m.textures.insert(m.textures.end(), m.opacityMaps.begin(), m.opacityMaps.end());
            // 4. Displacement maps
            m.displacementMaps = loadMaterialTextures(material, aiTextureType_DISPLACEMENT, "texture_displacement");
            m.textures.insert(m.textures.end(), m.displacementMaps.begin(), m.displacementMaps.end());
            // 4. Light maps
            m.lightMaps = loadMaterialTextures(material, aiTextureType_LIGHTMAP, "texture_lightmap");
            m.textures.insert(m.textures.end(), m.lightMaps.begin(), m.lightMaps.end());
            // 4. Reflection maps
            m.reflectionMaps = loadMaterialTextures(material, aiTextureType_REFLECTION, "texture_reflection");
            m.textures.insert(m.textures.end(), m.reflectionMaps.begin(), m.reflectionMaps.end());

            materials.push_back(m);
        }

    }

    LSMMesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
        // data to fill
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // walk through each of the mesh's vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            LSMVector3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.setPosition(vector);
            // normals
            if(mesh->mNormals)
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.setNormal(vector);
            }
            LSMVector3 color;
            if(mesh->mColors[0])
            {
                color.x = mesh->mColors[0][i].r;
                color.y = mesh->mColors[0][i].g;
                color.z = mesh->mColors[0][i].b;
                vertex.setColor(color);
            }

            // texture coordinates
            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                LSMVector2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.setUV(vec);
            }
            else
            {
                vertex.setUV(LSMVector2(0.0f, 0.0f));
            }
            // tangent
            if(mesh->mTangents)
            {
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.setTangent(vector);
            }

            // bitangent
            if(mesh->mBitangents)
            {
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.setBiTangent(vector);
            }

            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for(unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);

            }
        }

        ModelMaterial m;
        int index = mesh->mMaterialIndex;
        if(index < materials.size())
        {
            m = materials.at(index);
        }

        // return a mesh object created from the extracted mesh data
        return LSMMesh(vertices, indices, m, mesh->mMaterialIndex);
    }


    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, QString typeName)
    {
        std::vector<Texture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            aiTextureOp op = aiTextureOp_Multiply;
            mat->Get(AI_MATKEY_TEXOP(type,i),op);
            float strength = 1.0;
            mat->Get(AI_MATKEY_TEXBLEND(type,i),strength);
            int mapu = aiTextureMapMode_Wrap ;
            int mapv = aiTextureMapMode_Wrap ;
            mat->Get(AI_MATKEY_MAPPINGMODE_U(type,i),mapu);
            mat->Get(AI_MATKEY_MAPPINGMODE_U(type,i),mapv);

            QString fullpath = dir + "/" + str.C_Str();

            bool skip = false;
            /*for(unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if(textures_loaded[j]->GetPath() == fullpath)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }*/
            if(!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture = Texture();
                texture.m_Path=fullpath;
                texture.m_Type=typeName;
                texture.Operator =op;
                texture.MapU = mapu;
                texture.MapV = mapv;
                texture.Strength = strength;

                textures.push_back(texture);
                textures_loaded.push_back(texture); // add to loaded textures
            }
        }
        return textures;
    }


    inline QList<LSMMesh *> GetMeshes()
    {

        QList<LSMMesh *> retmeshes;
        for(int i = 0; i < meshes.size(); i++)
        {
            retmeshes.append(&(meshes.data()[i]));
        }
        return retmeshes;

    }

    inline LSMMesh * GetMesh(int i)
    {
        if(i > -1 && i < meshes.size())
        {
            return (&(meshes.at(i)));
        }else
        {
            return nullptr;
        }
    }

    inline ModelMaterial GetMaterial(int i)
    {
        if(i > -1 && i < materials.size())
        {
            return materials.at(i);
        }else
        {
            return ModelMaterial();
        }
    }

    inline int GetVertexCount()
    {
        int count = 0;
        for(int i = 0; i < meshes.size();i++)
        {
            count += meshes.at(i).GetVertexCount();
        }
        return count;
    }
    inline int GetMeshCount()
    {

        return meshes.size();
    }

    inline int GetMaterialCount()
    {
        return materials.size();
    }

    //Angelscript related functions
    public:

        QString    AS_FileName          = "";
        bool           AS_writeonassign     = false;
        std::function<void(ModelGeometry *,QString)> AS_writefunc;
        std::function<ModelGeometry *(QString)> AS_readfunc;
        bool           AS_Created           = false;
        int            AS_refcount          = 1;
        void           AS_AddRef            ();
        void           AS_ReleaseRef        ();
        ModelGeometry*     AS_Assign            (ModelGeometry*);



        /*Mesh * AS_GetMesh            (int index);
        int AS_GetMeshCount                ();
        int AS_GetTextureCount             ();
        CScriptArray * AS_GetTexture (int index);*/


};

inline void ModelGeometry::AS_AddRef()
{
    AS_refcount = AS_refcount + 1;

}

inline void ModelGeometry::AS_ReleaseRef()
{
    AS_refcount = AS_refcount - 1;
    if(AS_refcount == 0)
    {
        //Destroy();
        delete this;
    }

}

inline ModelGeometry* ModelGeometry::AS_Assign(ModelGeometry* sh)
{

    std::cout << "assign" << std::endl;

    for(int i = 0; i < sh->meshes.size() ; i++)
    {
        std::cout <<  i << "  " << &(sh->meshes.data()[i]) << std::endl;
    }
    if(AS_writeonassign)
    {
        AS_writefunc(sh,AS_FileName);
    }else
    {

        m_BoundingBox = sh->m_BoundingBox;
        meshes = sh->meshes;
        dir = sh->dir;
        path = sh->path;
        textures_loaded =sh->textures_loaded;

        for(int i = 0; i <sh->textures_loaded.size(); i++)
        {
            textures_loaded.push_back(sh->textures_loaded.at(i));
        }
        textures_loaded = sh->textures_loaded;

    }
    return this;
}


inline static ModelGeometry * AS_ModelGeometryFactory()
{
    return new ModelGeometry();
}


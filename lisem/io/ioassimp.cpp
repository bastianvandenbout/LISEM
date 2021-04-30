#include "ioassimp.h"
#include "assimp/scene.h"
#include "assimp/Exporter.hpp"
ModelGeometry * Read3DModel(QString path)
{
    ModelGeometry * model = new ModelGeometry();
    model->loadModel(path);

    return model;

}

void PutMapStackToMaterial(aiMaterial * m,std::vector<Texture * > * list, aiTextureType type)
{
    for(int j = 0; j< list->size();j++)
    {
        aiString path = aiString(list->at(j)->m_Path.toStdString());
        m->AddProperty(&path,AI_MATKEY_TEXTURE(type,j));
        m->AddProperty<int>(&(list->at(j)->MapU),1, AI_MATKEY_MAPPINGMODE_U( type, j));
        m->AddProperty<int>(&(list->at(j)->MapV),1, AI_MATKEY_MAPPINGMODE_V( type, j));
        m->AddProperty<float>(&(list->at(j)->Strength), 1,AI_MATKEY_TEXBLEND( type, j));
        m->AddProperty<int>(&(list->at(j)->Operator),1, AI_MATKEY_TEXOP(  type, j));
    }
}

void PutMapStackToMaterial(aiMaterial * m,std::vector<Texture> * list, aiTextureType type)
{
    for(int j = 0; j< list->size();j++)
    {
        aiString path = aiString(list->at(j).m_Path.toStdString());
        m->AddProperty(&path,AI_MATKEY_TEXTURE(type,j));
        m->AddProperty<int>(&(list->at(j).MapU),1, AI_MATKEY_MAPPINGMODE_U( type, j));
        m->AddProperty<int>(&(list->at(j).MapV),1, AI_MATKEY_MAPPINGMODE_V( type, j));
        m->AddProperty<float>(&(list->at(j).Strength), 1,AI_MATKEY_TEXBLEND( type, j));
        m->AddProperty<int>(&(list->at(j).Operator),1, AI_MATKEY_TEXOP(  type, j));
    }
}

void Write3DModel(ModelGeometry * model,QString path, QString format)
{
    aiScene scene = aiScene();
    scene.mRootNode = new aiNode();

    scene.mMeshes = new aiMesh*[ model->GetMeshCount() ];
    scene.mMeshes[ 0 ] = nullptr;
    scene.mNumMeshes = model->GetMeshCount();

    scene.mMaterials = new aiMaterial*[ model->GetMaterialCount() ];
    scene.mMaterials[ 0 ] = nullptr;
    scene.mNumMaterials = model->GetMaterialCount();

    for(int i = 0; i < model->GetMaterialCount(); i++)
    {

        ModelMaterial m = model->GetMaterial(i);
        scene.mMaterials[ i ] = new aiMaterial();

        aiColor3D color(0.f, 0.f, 0.f);
        float shininess = m.Shininess;
        color = aiColor3D(m.Color_Diffuse.x,m.Color_Diffuse.y,m.Color_Diffuse.z);
        scene.mMaterials[ i ]->AddProperty<aiColor3D>(&color, 1, AI_MATKEY_COLOR_DIFFUSE);
        color = aiColor3D(m.Color_Ambient.x,m.Color_Ambient.y,m.Color_Ambient.z);
        scene.mMaterials[ i ]->AddProperty<aiColor3D>(&color, 1, AI_MATKEY_COLOR_AMBIENT);
        color = aiColor3D(m.Color_Specular.x,m.Color_Specular.y,m.Color_Specular.z);
        scene.mMaterials[ i ]->AddProperty<aiColor3D>(&color, 1, AI_MATKEY_COLOR_SPECULAR);
        scene.mMaterials[ i ]->AddProperty<float>(&shininess, 1, AI_MATKEY_SHININESS);

        PutMapStackToMaterial(scene.mMaterials[ i ],&(m.diffuseMaps),aiTextureType_DIFFUSE);
        PutMapStackToMaterial(scene.mMaterials[ i ],&(m.specularMaps),aiTextureType_SPECULAR);
        PutMapStackToMaterial(scene.mMaterials[ i ],&(m.ambientMaps),aiTextureType_AMBIENT);
        PutMapStackToMaterial(scene.mMaterials[ i ],&(m.normalMaps),aiTextureType_NORMALS);
        PutMapStackToMaterial(scene.mMaterials[ i ],&(m.heightMaps),aiTextureType_HEIGHT);
        PutMapStackToMaterial(scene.mMaterials[ i ],&(m.emissiveMaps),aiTextureType_EMISSIVE);
        PutMapStackToMaterial(scene.mMaterials[ i ],&(m.shininessMaps),aiTextureType_SHININESS);
        PutMapStackToMaterial(scene.mMaterials[ i ],&(m.opacityMaps),aiTextureType_OPACITY);
        PutMapStackToMaterial(scene.mMaterials[ i ],&(m.displacementMaps),aiTextureType_DISPLACEMENT);
        PutMapStackToMaterial(scene.mMaterials[ i ],&(m.lightMaps),aiTextureType_LIGHTMAP);
        PutMapStackToMaterial(scene.mMaterials[ i ],&(m.reflectionMaps), aiTextureType_REFLECTION);
    }


    scene.mRootNode->mMeshes = new unsigned int[ model->GetMeshCount() ];

    scene.mRootNode->mNumMeshes =  model->GetMeshCount();

    for(int i = 0; i < model->GetMeshCount(); i++)
    {
        scene.mRootNode->mMeshes[ i ] = i;
        LSMMesh * m = model->GetMesh(i);

        scene.mMeshes[ i ] = new aiMesh();
        scene.mMeshes[ i ]->mMaterialIndex = m->m_MaterialIndex;

        aiMesh* pMesh = scene.mMeshes[ i ];

        pMesh->mVertices = new aiVector3D[ m->GetVertexCount() ];
        pMesh->mNormals = new aiVector3D[  m->GetVertexCount() ];
        pMesh->mNumVertices =  m->GetVertexCount();
        pMesh->mTangents = new aiVector3D[ m->GetVertexCount() ];
        pMesh->mBitangents = new aiVector3D[ m->GetVertexCount() ];
        pMesh->mColors[0] = new aiColor4D[ m->GetVertexCount() ];

        pMesh->mTextureCoords[ 0 ] = new aiVector3D[  m->GetVertexCount() ];
        pMesh->mNumUVComponents[ 0 ] =  m->GetVertexCount();
        pMesh->mFaces = new aiFace[ m->GetFaceCount() ];
        pMesh->mNumFaces = m->GetFaceCount();


        for (int j = 0; j < m->GetVertexCount(); j++)
        {
            Vertex v = m->GetVertex(j);
            pMesh->mVertices[ j] = aiVector3D( v.m_position.x, v.m_position.y,v.m_position.z );
            pMesh->mNormals[ j ] = aiVector3D( v.m_normal.x,  v.m_normal.y,  v.m_normal.z );
            pMesh->mTextureCoords[0][ j ] = aiVector3D( v.m_UV.x, v.m_UV.y, 0 );
            pMesh->mTangents[j] = aiVector3D( v.m_tangent.x,  v.m_tangent.y,  v.m_tangent.z );
            pMesh->mBitangents[j] = aiVector3D( v.m_bitangent.x,  v.m_bitangent.y,  v.m_bitangent.z );
            pMesh->mColors[0][j] = aiColor4D( v.m_color.x,  v.m_color.y,  v.m_color.z,1.0 );

            j++;
        }

        for(int j = 0; j < m->GetFaceCount(); j++)
        {

            Face f = m->GetFace(j);

            aiFace &face = pMesh->mFaces[i];
            face.mIndices = new unsigned int[3];
            face.mNumIndices = 3;

            face.mIndices[0] = f.i_1;
            face.mIndices[1] = f.i_2;
            face.mIndices[2] = f.i_3;

        }


    }


    aiExportScene(&scene,format.toStdString().c_str(),path.toStdString().c_str(),0);

}

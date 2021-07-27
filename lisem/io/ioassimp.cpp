#include "ioassimp.h"
#include "assimp/scene.h"
#include "assimp/Exporter.hpp"
#include "QMutex"
#include "error.h"
#include <assimp/IOSystem.hpp>
#include <assimp/Logger.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/DefaultLogger.hpp>

// Example stream
class myStream :
        public Assimp::LogStream
{
public:
        // Constructor
        myStream()
        {
                // empty
        }

        // Destructor
        ~myStream()
        {
                // empty
        }
        // Write womethink using your own functionality
        void write(const char* message)
        {
               LISEMS_DEBUG(QString(message));
        }
};


bool logger_set = false;
QMutex logger_set_mutex;

inline static void SetAssimpLogger()
{
    logger_set_mutex.lock();
    if(!logger_set)
    {

        // Select the kinds of messages you want to receive on this log stream
        const unsigned int severity = Assimp::Logger::ErrorSeverity::Debugging|Assimp::Logger::ErrorSeverity::Info|Assimp::Logger::ErrorSeverity::Err|Assimp::Logger::ErrorSeverity::Warn;
        // Attaching it to the default logger
        Assimp::DefaultLogger::get()->attachStream( new myStream(), severity );
    }
    logger_set_mutex.unlock();
}
ModelGeometry * Read3DModel(QString path)
{
    SetAssimpLogger();

    std::cout << "read 3d model " << path.toStdString() << std::endl;
    ModelGeometry * model = new ModelGeometry();
    model->loadModel(path);

    //for(int i = 0; i < model->meshes.size() ; i++)
    {
        //std::cout <<  i << "  " << &(model->meshes.data()[i]) << std::endl;
    }

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
    SetAssimpLogger();

    std::cout << "write 3d model " << path.toStdString() << std::endl;

    aiScene scene = aiScene();

    scene.mFlags = 0;
    scene.mRootNode = NULL;
    scene.mNumMeshes = 0;
    scene.mMeshes = NULL;
    scene.mNumMaterials = 0;
    scene.mMaterials = NULL;
    scene.mNumAnimations = 0;
    scene.mAnimations = NULL;
    scene.mNumTextures = 0;
    scene.mTextures = NULL;
    scene.mNumLights = 0;
    scene.mLights = NULL;
    scene.mNumCameras = 0;
    scene.mCameras = NULL;
    scene.mPrivate = NULL;



    scene.mRootNode = new aiNode();


    std::cout << 1 << std::endl;
    scene.mMeshes = new aiMesh*[ model->GetMeshCount() ];
    scene.mMeshes[ 0 ] = nullptr;
    scene.mNumMeshes = model->GetMeshCount();

    std::cout << 1 << std::endl;

    scene.mMaterials = new aiMaterial*[ model->GetMaterialCount() ];
    scene.mMaterials[ 0 ] = nullptr;
    scene.mNumMaterials = model->GetMaterialCount();


    std::cout << 1 << std::endl;

    for(int i = 0; i < model->GetMaterialCount(); i++)
    {


        std::cout << 2 << std::endl;
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


    std::cout << 3 << std::endl;

    scene.mRootNode->mMeshes = new unsigned int[ model->GetMeshCount() ];

    scene.mRootNode->mNumMeshes =  model->GetMeshCount();

    std::cout << "3A " << model->GetMeshCount() << std::endl;
    scene.mMeshes =  new aiMesh*[  model->GetMeshCount() ];

    scene.mNumTextures = 0;
    std::cout << 4 << std::endl;
    for(int i = 0; i < model->GetMeshCount(); i++)
    {
        std::cout << 5 << std::endl;

        scene.mRootNode->mMeshes[ i ] = i;
        LSMMesh * m = model->GetMesh(i);

        std::cout << "5A " << scene.mMeshes<< " "  << m << std::endl;
        scene.mMeshes[ i ] = new aiMesh();
        std::cout << "5A2" << std::endl;
        scene.mMeshes[ i ]->mMaterialIndex = m->m_MaterialIndex;

        std::cout << "5B" << std::endl;
        aiMesh* pMesh = scene.mMeshes[ i ];
        pMesh->mPrimitiveTypes = aiPrimitiveType_TRIANGLE;

        std::cout << "5C" << std::endl;
        pMesh->mVertices = new aiVector3D[ m->GetVertexCount() ];
        pMesh->mNormals = new aiVector3D[  m->GetVertexCount() ];
        pMesh->mNumVertices =  m->GetVertexCount();
        pMesh->mTangents = new aiVector3D[ m->GetVertexCount() ];
        pMesh->mBitangents = new aiVector3D[ m->GetVertexCount() ];
        pMesh->mColors[0] = new aiColor4D[ m->GetVertexCount() ];

        std::cout << "5D" << std::endl;
        pMesh->mTextureCoords[ 0 ] = new aiVector3D[  m->GetVertexCount() ];
        pMesh->mNumUVComponents[ 0 ] =  m->GetVertexCount();
        pMesh->mFaces = new aiFace[ m->GetFaceCount() ];
        pMesh->mNumFaces = m->GetFaceCount();


        std::cout << 6 << std::endl;

        for (int j = 0; j < m->GetVertexCount(); j++)
        {
            Vertex v = m->GetVertex(j);
            pMesh->mVertices[ j] = aiVector3D( v.m_position.x, v.m_position.y,v.m_position.z );
            pMesh->mNormals[ j ] = aiVector3D( v.m_normal.x,  v.m_normal.y, v.m_normal.z );
            pMesh->mTextureCoords[0][ j ] = aiVector3D( v.m_UV.x, v.m_UV.y, 0 );
            pMesh->mTangents[j] = aiVector3D( v.m_tangent.x,  v.m_tangent.y,  v.m_tangent.z );
            pMesh->mBitangents[j] = aiVector3D( v.m_bitangent.x,  v.m_bitangent.y,  v.m_bitangent.z );
            pMesh->mColors[0][j] = aiColor4D( v.m_color.x,  v.m_color.y,  v.m_color.z,1.0 );

            j++;
        }


        std::cout << 7 << std::endl;

        for(int j = 0; j < m->GetFaceCount(); j++)
        {

            Face f = m->GetFace(j);

            pMesh->mFaces[j].mIndices = new unsigned int[3];
            pMesh->mFaces[j].mNumIndices = 3;

            std::cout << "write " <<  (unsigned int) f.i_1 << " " << (unsigned int)  f.i_2 << " " << (unsigned int)  f.i_3 << std::endl;
            pMesh->mFaces[j].mIndices[0] = (unsigned int) f.i_1;
            pMesh->mFaces[j].mIndices[1] = (unsigned int) f.i_2;
            pMesh->mFaces[j].mIndices[2] = (unsigned int) f.i_3;

        }
    }


    std::cout << 8 << std::endl;

    aiReturn returncode = aiExportScene(&scene,format.toStdString().c_str(),path.toStdString().c_str(),aiProcess_CalcTangentSpace| aiProcess_GenNormals|aiProcess_JoinIdenticalVertices| aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_FlipUVs | aiProcess_SortByPType);

    if(returncode != aiReturn_SUCCESS)
    {
        std::cout << "error during writing " << returncode << std::endl;
        LISEMS_ERROR("Error when writing model " + path + " " + QString::number(returncode));
    }

    std::cout << 9 << std::endl;

    for(int i = 0; i < scene.mNumMeshes; i++)
    {
        aiMesh* pMesh = scene.mMeshes[ i ];
        //for(int j = 0; j < pMesh->mNumFaces; j++)
        //{
       //     std::cout << pMesh->mFaces[j].mIndices[0] << " "<< pMesh->mFaces[j].mIndices[1]  << " " << pMesh->mFaces[j].mIndices[2] << std::endl;

        //}

    }
}

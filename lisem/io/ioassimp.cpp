#include "ioassimp.h"
#include "assimp/scene.h"
#include "assimp/Exporter.hpp"
#include "QMutex"
#include "error.h"
#include <assimp/IOSystem.hpp>
#include <assimp/Logger.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/mesh.h>
#include <assimp/scene.h>

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
            std::cout << "ASSIMP Message : " << message << std::endl;
            LISEMS_DEBUG("ASSIMP Message : " + QString(message));
        }
};


bool logger_set = false;
QMutex logger_set_mutex;

inline static void SetAssimpLogger()
{
    logger_set_mutex.lock();
    if(!logger_set)
    {
        Assimp::DefaultLogger::create("",Assimp::Logger::VERBOSE);
        // Now I am ready for logging my stuff
        Assimp::DefaultLogger::get()->info("Test of assimp custom logger");

        std::cout << "SET ASSIMP LOGGER " << std::endl;

        // Select the kinds of messages you want to receive on this log stream
        const unsigned int severity = Assimp::Logger::ErrorSeverity::Debugging|Assimp::Logger::ErrorSeverity::Info|Assimp::Logger::ErrorSeverity::Err|Assimp::Logger::ErrorSeverity::Warn;
        // Attaching it to the default logger
        Assimp::DefaultLogger::get()->attachStream( new myStream(), severity );
        logger_set = true;
    }
    logger_set_mutex.unlock();
}
ModelGeometry * Read3DModel(QString path)
{
    SetAssimpLogger();

    ModelGeometry * model = new ModelGeometry();
    model->loadModel(path);

    /*LSMMesh * m = model->GetMesh(0);
    for(int i = 0; i < m->GetVertexCount(); i++)
    {
        Vertex vertex = m->GetVertex(i);
       std::cout << "position " << vertex.m_position.x << " " << vertex.m_position.y << " " << vertex.m_position.z << std::endl;
    }
    //test
    Assimp::Importer import;

    const aiScene *scene = import.ReadFile(path.toStdString().c_str(), aiProcess_CalcTangentSpace| aiProcess_GenNormals|aiProcess_JoinIdenticalVertices| aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_FlipUVs | aiProcess_SortByPType);

    QString path2 = path;
    path2 = path.insert(path.size() - 4,'2');

    Assimp::Exporter exporter;

    aiScene *scene2;
    aiCopyScene(scene,&scene2);

    PrintAiScene(scene2);

    aiReturn returncode = exporter.Export(scene2,"obj",path2.toStdString(),0u);

    std::cout << "done writing " << returncode << std::endl;
    //delete scene;*/


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


void ModelToAssimp(ModelGeometry * model,aiScene* scene_in)
{
    aiScene &scene = *scene_in;

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

    scene.mMetaData = new aiMetadata();


    scene.mRootNode = new aiNode();

    scene.mMeshes = new aiMesh*[ model->GetMeshCount() ];
    scene.mNumMeshes = model->GetMeshCount();

    if(model->GetMaterialCount() == 0)
    {
        scene.mMaterials = new aiMaterial*[ model->GetMaterialCount() ];
        scene.mNumMaterials = 1;
        scene.mMaterials[0] = new aiMaterial();

        aiColor3D color(0.f, 0.f, 0.f);
        float shininess = 0.5;
        color = aiColor3D(0.95,0.95,0.95);
        scene.mMaterials[0]->AddProperty<aiColor3D>(&color, 1, AI_MATKEY_COLOR_DIFFUSE);
        color = aiColor3D(0.95,0.8,0.7);
        scene.mMaterials[0]->AddProperty<aiColor3D>(&color, 1, AI_MATKEY_COLOR_AMBIENT);
        color = aiColor3D(0.7,0.8,0.95);
        scene.mMaterials[0]->AddProperty<aiColor3D>(&color, 1, AI_MATKEY_COLOR_SPECULAR);
        scene.mMaterials[0]->AddProperty<float>(&shininess, 1, AI_MATKEY_SHININESS);


    }else
    {
        scene.mMaterials = new aiMaterial*[ model->GetMaterialCount() ];
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
    }

    scene.mRootNode->mMeshes = new unsigned int[ model->GetMeshCount() ];

    scene.mRootNode->mNumMeshes =  model->GetMeshCount();

    scene.mNumTextures = 0;
    for(int i = 0; i < model->GetMeshCount(); i++)
    {

        scene.mRootNode->mMeshes[ i ] = i;
        LSMMesh * m = model->GetMesh(i);

        scene.mMeshes[ i ] = new aiMesh();
        if(m->m_MaterialIndex < 0)
        {
            scene.mMeshes[ i ]->mMaterialIndex = 0;
        }else
        {
            scene.mMeshes[ i ]->mMaterialIndex = m->m_MaterialIndex;
        }

        aiMesh* pMesh = scene.mMeshes[ i ];
        pMesh->mPrimitiveTypes = aiPrimitiveType_TRIANGLE;

        pMesh->mVertices = new aiVector3D[ m->GetVertexCount() ];
        pMesh->mNormals = new aiVector3D[  m->GetVertexCount() ];
        pMesh->mNumVertices =  (unsigned int) m->GetVertexCount();
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
            pMesh->mNormals[ j ] = aiVector3D( v.m_normal.x,  v.m_normal.y, v.m_normal.z );
            pMesh->mTextureCoords[0][ j ] = aiVector3D( v.m_UV.x, v.m_UV.y, 0 );
            pMesh->mTangents[j] = aiVector3D( v.m_tangent.x,  v.m_tangent.y,  v.m_tangent.z );
            pMesh->mBitangents[j] = aiVector3D( v.m_bitangent.x,  v.m_bitangent.y,  v.m_bitangent.z );
            pMesh->mColors[0][j] = aiColor4D( v.m_color.x,  v.m_color.y,  v.m_color.z,1.0 );

        }

        for(int j = 0; j < m->GetFaceCount(); j++)
        {

            Face f = m->GetFace(j);

            pMesh->mFaces[j].mIndices = new unsigned int[3];
            pMesh->mFaces[j].mNumIndices = 3;

            pMesh->mFaces[j].mIndices[0] = (unsigned int) f.i_1;
            pMesh->mFaces[j].mIndices[1] = (unsigned int) f.i_2;
            pMesh->mFaces[j].mIndices[2] = (unsigned int) f.i_3;

        }
    }

    scene.mFlags = 0;

    aiNode * nod = scene_in->mRootNode;
    nod->mName = aiString("RootNode");

}

void PrintAiScene(aiScene* scene_in)
{

    std::cout << "=======PRINT SCENE========" <<std::endl;

    std::cout << "flag          : " << scene_in->mFlags << std::endl;
    std::cout << "private       : " << scene_in->mPrivate << std::endl;
    std::cout << "Nummeshes     : " << scene_in->mNumMeshes << std::endl;
    std::cout << "NumMaterials  : " << scene_in->mNumMaterials << std::endl;
    std::cout << "NumLights     : " << scene_in->mNumLights << std::endl;
    std::cout << "NumAnimations : " << scene_in->mNumAnimations << std::endl;
    std::cout << "NumCameras    : " << scene_in->mNumCameras << std::endl;
    std::cout << "meshes        : " << scene_in->mNumMeshes << std::endl;

    for(int i = 0; i < scene_in->mNumMeshes; i++)
    {
        aiMesh * m = scene_in->mMeshes[i];
        std::cout << "===MESH=== : "  << i << std::endl;
        std::cout << "primite type: " << m->mPrimitiveTypes << std::endl;
        std::cout << "NumVertices : " << m->mNumVertices << std::endl;
        std::cout << "NumAnimeshes: " << m->mNumAnimMeshes << std::endl;
        std::cout << "NumBones    : " << m->mNumBones << std::endl;
        std::cout << "NumFaces    : " << m->mNumFaces << std::endl;
        std::cout << "NumUVComp   : " << m->mNumUVComponents << std::endl;
        std::cout << "has " << m->HasFaces() << "  " <<  m->HasNormals() <<"  " <<  m->HasPositions() << "  " << m->HasTangentsAndBitangents() <<"  " << m->HasTextureCoords(0) << "  " << m->HasVertexColors(0) << std::endl;

        std::cout << "==vertices==" << std::endl;
        for(int j = 0; j < m->mNumVertices; j++)
        {
            std::cout << m->mVertices[j].x << " " << m->mVertices[j].y <<  " " <<m->mVertices[j].z << std::endl;
        }

        std::cout << "==faces==" << std::endl;
        for(int j = 0; j < m->mNumFaces; j++)
        {
            if(m->mFaces[j].mNumIndices != 3)
            {
                std::cout << "NUMBER OF INDICES IN FACE IS NOT 3" << std::endl;
            }else
            {
                std::cout << m->mFaces[j].mNumIndices << " " << m->mFaces[j].mIndices[0] <<  " " << m->mFaces[j].mIndices[1] << " " << m->mFaces[j].mIndices[2] <<std::endl;
            }
        }

    }

    std::cout << "===nodes==="<< std::endl;
    aiNode * n = scene_in->mRootNode;
    std::cout << "num meshes " << n->mNumMeshes << std::endl;
    std::cout << "num childs " << n->mNumChildren << std::endl;

    for(int i =  0; i < n->mNumMeshes ; i++)
    {
        std::cout << "mesh " << i << " " << n->mMeshes[i] << std::endl;
    }

    std::cout << "=======PRINT SCENE DONE ==" <<std::endl;
}

void Write3DModel(ModelGeometry * model,QString path, QString format)
{
    SetAssimpLogger();

    std::cout << "write 3d model " << path.toStdString() << " " << model->GetMeshCount() << " " << model->GetVertexCount() << std::endl;

    aiScene scene = aiScene();

    ModelToAssimp(model,&scene);

    Assimp::Exporter exporter;

    PrintAiScene(&scene);


    aiReturn returncode = exporter.Export(&scene,format.toStdString().c_str(),path.toStdString().c_str(),aiProcess_CalcTangentSpace| aiProcess_GenNormals|aiProcess_JoinIdenticalVertices| aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_FlipUVs | aiProcess_SortByPType); //

    if(returncode != aiReturn_SUCCESS)
    {
        std::cout << "error during writing " << returncode << + " " << exporter.GetErrorString() << std::endl;
        LISEMS_ERROR("Error when writing model " + path + " " + QString::number(returncode) + " " + QString(exporter.GetErrorString()));
    }

}

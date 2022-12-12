#pragma once

#include "models/3dmodel.h"
#include "openglclmanager.h"

class gl3dMesh
{
    public:
    unsigned int VAO, VBO, EBO, InstanceMBO,InstanceMBOP,InstanceMBON;
    int m_NumInstances = 0;
    ModelMaterial m_Material;
    int m_MaterialIndex = -1;
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    LSMMatrix4x4 * m_InstanceNMatrices = nullptr;
    LSMMatrix4x4 * m_InstanceMatrices = nullptr;
    LSMMatrix4x4 * m_InstanceMatricesP = nullptr;
    bool m_IsSetup = false;
    bool m_IsInstanced = false;

    inline gl3dMesh(OpenGLCLManager* m, LSMMesh * o, int n_instance = 0)
    {
        vertices = o->vertices;
        indices = o->indices;
        m_Material = o->m_Material;
        m_MaterialIndex = o->m_MaterialIndex;




        m_IsSetup = true;
        m_IsInstanced = false;

        //setup of mesh


        glad_glGenVertexArrays(1, &VAO);
        glad_glGenBuffers(1, &VBO);
        glad_glGenBuffers(1, &EBO);

        glad_glBindVertexArray(VAO);
        glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);


        glad_glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                     &indices[0], GL_STATIC_DRAW);

        // vertex positions
        glad_glEnableVertexAttribArray(0);
        glad_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)Vertex::positionOffset());
        // vertex normals
        glad_glEnableVertexAttribArray(1);
        glad_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)Vertex::uvOffset());
        // vertex texture coords
        glad_glEnableVertexAttribArray(2);
        glad_glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)Vertex::normalOffset());
        // vertex texture coords
        glad_glEnableVertexAttribArray(3);
        glad_glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)Vertex::tangentOffset());
        // vertex texture coords
        glad_glEnableVertexAttribArray(4);
        glad_glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)Vertex::bitangentOffset());
        // vertex texture coords
        glad_glEnableVertexAttribArray(5);
        glad_glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)Vertex::colorOffset());

        glad_glBindVertexArray(0);


        //setup of textures

        if(n_instance > 0)
        {
            if(m_IsSetup)
            {
                //if we already have the correct number
                if(m_IsInstanced)
                {
                    if(m_NumInstances == n_instance)
                    {
                        return;
                    }
                }

                //otherwise delete first, then reupload
                if(m_IsInstanced)
                {
                    glad_glDeleteBuffers(1,&InstanceMBO);
                    delete[] m_InstanceMatrices;
                    glad_glDeleteBuffers(1,&InstanceMBOP);
                    delete[] m_InstanceMatricesP;
                    glad_glDeleteBuffers(1,&InstanceMBON);
                    delete[] m_InstanceNMatrices;
                }
                m_NumInstances = n_instance;

                m_InstanceNMatrices = new LSMMatrix4x4[n_instance];
                m_InstanceMatrices = new LSMMatrix4x4[n_instance];
                m_InstanceMatricesP = new LSMMatrix4x4[n_instance];

                // vertex buffer object
                glad_glGenBuffers(1, &InstanceMBO);
                glad_glBindBuffer(GL_ARRAY_BUFFER, InstanceMBO);
                glad_glBufferData(GL_ARRAY_BUFFER, n_instance * sizeof(LSMMatrix4x4), &m_InstanceMatrices[0], GL_STATIC_DRAW);

                // vertex buffer object
                glad_glGenBuffers(1, &InstanceMBOP);
                glad_glBindBuffer(GL_ARRAY_BUFFER, InstanceMBOP);
                glad_glBufferData(GL_ARRAY_BUFFER, n_instance * sizeof(LSMMatrix4x4), &m_InstanceMatricesP[0], GL_STATIC_DRAW);

                // vertex buffer object
                glad_glGenBuffers(1, &InstanceMBON);
                glad_glBindBuffer(GL_ARRAY_BUFFER, InstanceMBON);
                glad_glBufferData(GL_ARRAY_BUFFER, n_instance * sizeof(LSMMatrix4x4), &m_InstanceNMatrices[0], GL_STATIC_DRAW);

                m_IsInstanced = true;

                //setup of mesh

                glad_glBindVertexArray(VAO);


                std::size_t vec4Size = sizeof(LSMVector4);
                glad_glEnableVertexAttribArray(6);
                glad_glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
                glad_glEnableVertexAttribArray(7);
                glad_glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
                glad_glEnableVertexAttribArray(8);
                glad_glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
                glad_glEnableVertexAttribArray(9);
                glad_glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

                glad_glEnableVertexAttribArray(10);
                glad_glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
                glad_glEnableVertexAttribArray(11);
                glad_glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
                glad_glEnableVertexAttribArray(12);
                glad_glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
                glad_glEnableVertexAttribArray(13);
                glad_glVertexAttribPointer(13, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

                glad_glEnableVertexAttribArray(14);
                glad_glVertexAttribPointer(14, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
                glad_glEnableVertexAttribArray(15);
                glad_glVertexAttribPointer(15, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
                glad_glEnableVertexAttribArray(16);
                glad_glVertexAttribPointer(16, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
                glad_glEnableVertexAttribArray(17);
                glad_glVertexAttribPointer(17, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

                glad_glVertexAttribDivisor(6, 1);
                glad_glVertexAttribDivisor(7, 1);
                glad_glVertexAttribDivisor(8, 1);
                glad_glVertexAttribDivisor(9, 1);

                glad_glVertexAttribDivisor(10, 1);
                glad_glVertexAttribDivisor(11, 1);
                glad_glVertexAttribDivisor(12, 1);
                glad_glVertexAttribDivisor(13, 1);

                glad_glVertexAttribDivisor(14, 1);
                glad_glVertexAttribDivisor(15, 1);
                glad_glVertexAttribDivisor(16, 1);
                glad_glVertexAttribDivisor(17, 1);


                //setup of textures

                glad_glBindVertexArray(0);
                std::cout<<"!!!!!!!!!!!!!!!!!!!! create instanced" << std::endl;

            }
        }
    }

    inline bool UpdateInstanceMatrices(OpenGLCLManager * m)
    {
        if(m_IsInstanced)
        {
            glad_glBindBuffer(GL_ARRAY_BUFFER, InstanceMBO);
            glad_glBufferSubData(GL_ARRAY_BUFFER, 0,m_NumInstances* sizeof(LSMMatrix4x4), &m_InstanceMatrices[0]);
            glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
            glad_glBindBuffer(GL_ARRAY_BUFFER, InstanceMBOP);
            glad_glBufferSubData(GL_ARRAY_BUFFER, 0,m_NumInstances* sizeof(LSMMatrix4x4), &m_InstanceMatricesP[0]);
            glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
            glad_glBindBuffer(GL_ARRAY_BUFFER, InstanceMBON);
            glad_glBufferSubData(GL_ARRAY_BUFFER, 0,m_NumInstances* sizeof(LSMMatrix4x4), &m_InstanceNMatrices[0]);
            glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
            return true;
        }
        return false;

    }
    inline void SetInstanceNMatrix(int n,const LSMMatrix4x4 & objrotm)
    {
        if(n < m_NumInstances && n > -1)
        {
            m_InstanceNMatrices[n] = objrotm;
        }

    }

    inline void SetInstanceMatrix(int n,const LSMMatrix4x4 & objrotm)
    {
        if(n < m_NumInstances && n > -1)
        {
            m_InstanceMatrices[n] = objrotm;
        }

    }

    inline void SetInstanceProps(int n,const LSMMatrix4x4 & objrotm)
    {
        if(n < m_NumInstances && n > -1)
        {
            m_InstanceMatricesP[n] = objrotm;
        }

    }


    inline void Destroy(OpenGLCLManager* m)
    {



    }

};


class gl3dObject
{

    std::vector<gl3dMesh*> m_Mesh;
public:

   inline gl3dObject(OpenGLCLManager* m, ModelGeometry * o, int n_instance = 0)
   {
       QList<LSMMesh*> meshes = o->GetMeshes();

       for(int i = 0; i < meshes.size(); i++)
       {
           m_Mesh.push_back(new gl3dMesh(m,meshes.at(i),n_instance));
       }

   }

   inline std::vector<gl3dMesh*> GetMeshes()
   {
        return m_Mesh;
   }

   inline void UpdateInstanceMatrices(OpenGLCLManager* m)
   {
       for(int i = 0; i < m_Mesh.size(); i++)
       {
           m_Mesh.at(i)->UpdateInstanceMatrices(m);
       }


   }
   inline void SetInstanceMatrix(int j,LSMMatrix4x4 objrotm)
   {
       for(int i = 0; i < m_Mesh.size(); i++)
       {
           m_Mesh.at(i)->SetInstanceMatrix(j,objrotm);
       }

   }
   inline void SetInstanceNMatrix(int j,LSMMatrix4x4 objrotm)
   {
       for(int i = 0; i < m_Mesh.size(); i++)
       {
           m_Mesh.at(i)->SetInstanceNMatrix(j,objrotm);
       }

   }

   inline void SetInstanceProps(int j,LSMMatrix4x4 objprops)
   {
       for(int i = 0; i < m_Mesh.size(); i++)
       {
           m_Mesh.at(i)->SetInstanceProps(j,objprops);
       }

   }

   inline void Destroy(OpenGLCLManager* m)
   {



   }


};

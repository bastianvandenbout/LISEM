#ifndef GL3DGEOMETRY_H
#define GL3DGEOMETRY_H

#include "vertex.h"
#include "linear/lsm_vector3.h"
#include "glad/glad.h"
#include "iostream"

//Colored triangle
static const Vertex test_vertexes[] = {
  Vertex( LSMVector3( 0.0f,  1.0f, 0.0f), LSMVector3(1.0f, 0.0f, 0.0f) ),
  Vertex( LSMVector3( 0.0f, 0.0f, 1.0f), LSMVector3(0.0f, 1.0f, 0.0f) ),
  Vertex( LSMVector3( 0.0f, 1.0f, 1.0f), LSMVector3(0.0f, 0.0f, 1.0f) )
};
static const GLuint test_indices[] = {0,1,2};


static const Vertex vertices_SKYBOX[36] =
{
    Vertex( LSMVector3(-1.0f, -1.0f, -1.0f)),
    Vertex( LSMVector3(-1.0f,  1.0f, -1.0f)),
    Vertex( LSMVector3( 1.0f, -1.0f, -1.0f)),
    Vertex( LSMVector3( 1.0f,  1.0f, -1.0f)),
    Vertex( LSMVector3( 1.0f, -1.0f, -1.0f)),
    Vertex( LSMVector3(-1.0f,  1.0f, -1.0f)),

    Vertex( LSMVector3(-1.0f, -1.0f, -1.0f)),
    Vertex( LSMVector3(-1.0f, -1.0f,  1.0f)),
    Vertex( LSMVector3(-1.0f,  1.0f, -1.0f)),
    Vertex( LSMVector3(-1.0f,  1.0f,  1.0f)),
    Vertex( LSMVector3(-1.0f,  1.0f, -1.0f)),
    Vertex( LSMVector3(-1.0f, -1.0f,  1.0f)),

    Vertex( LSMVector3( 1.0f, -1.0f,  1.0f)),
    Vertex( LSMVector3( 1.0f, -1.0f, -1.0f)),
    Vertex( LSMVector3( 1.0f,  1.0f,  1.0f)),
    Vertex( LSMVector3( 1.0f,  1.0f, -1.0f)),
    Vertex( LSMVector3( 1.0f,  1.0f,  1.0f)),
    Vertex( LSMVector3( 1.0f, -1.0f, -1.0f)),

    Vertex( LSMVector3(-1.0f,  1.0f,  1.0f)),
    Vertex( LSMVector3(-1.0f, -1.0f,  1.0f)),
    Vertex( LSMVector3( 1.0f,  1.0f,  1.0f)),
    Vertex( LSMVector3( 1.0f, -1.0f,  1.0f)),
    Vertex( LSMVector3( 1.0f,  1.0f,  1.0f)),
    Vertex( LSMVector3(-1.0f, -1.0f,  1.0f)),

    Vertex( LSMVector3( 1.0f,  1.0f, -1.0f)),
    Vertex( LSMVector3(-1.0f,  1.0f, -1.0f)),
    Vertex( LSMVector3( 1.0f,  1.0f,  1.0f)),
    Vertex( LSMVector3(-1.0f,  1.0f,  1.0f)),
    Vertex( LSMVector3( 1.0f,  1.0f,  1.0f)),
    Vertex( LSMVector3(-1.0f,  1.0f, -1.0f)),

    Vertex( LSMVector3(-1.0f, -1.0f,  1.0f)),
    Vertex( LSMVector3(-1.0f, -1.0f, -1.0f)),
    Vertex( LSMVector3( 1.0f, -1.0f, -1.0f)),
    Vertex( LSMVector3(-1.0f, -1.0f,  1.0f)),
    Vertex( LSMVector3( 1.0f, -1.0f, -1.0f)),
    Vertex( LSMVector3( 1.0f, -1.0f,  1.0f))
};

static const GLuint indices_SKYBOX[36] = {
    0,			1,			2,			3,
    4,			5,			6,			7,
    8,			9,			10,			11,
    12,			13,			14,			15,
    16,			17,			18,			19,
    20,			21,			22,			23,
    24,			25,			26,			27,
    28,			29,			30,			31,
    32,			33,			34,			35
};



class GL3DGeometry
{
    private:


    std::vector<Vertex> m_VertexData;
    std::vector<GLuint> m_Indices;

public:

    int m_IndexLength = 0;
        GLuint m_vbo;
        GLuint m_ibo;
        GLuint m_vao;

    public:

    inline void CreateFromUnitBox()
    {
        CreateFrom(vertices_SKYBOX,36,indices_SKYBOX,36);
    }

    inline void CreateFromRegularUnitGrid(int x)
    {
        int n_total = x*x*6;

        m_VertexData = std::vector<Vertex>(n_total);
        m_Indices = std::vector<GLuint>(n_total);

        int icount = n_total;
        int vcount = n_total;

        int n = 0;
        for(int r = 0; r < x; r = r + 1){
        for (int c = 0; c < x; c= c + 1){

            float m = 1.0f;
            float cs = 2.0/(double(x));
            float c2 = -1.0f + cs * c;
            float r2 = -1.0f + cs * r;
            float c2m = -1.0f + cs * (c+m);
            float r2m = -1.0f + cs * (r+m);

            m_VertexData[n*6 + 0] = Vertex(LSMVector3((double)c2,-10.0f,(double)r2));
            m_VertexData[n*6 + 1] = Vertex(LSMVector3((double)c2m,-10.0f,(double)r2));
            m_VertexData[n*6 + 2] = Vertex(LSMVector3((double)c2,-10.0f,(double)r2m));

            m_VertexData[n*6 + 3] = Vertex(LSMVector3((double)c2m,-10.0f,r2m));
            m_VertexData[n*6 + 4] = Vertex(LSMVector3((double)c2,-10.0f,(double)r2m));
            m_VertexData[n*6 + 5] = Vertex(LSMVector3((double)c2m,-10.0f,(double)r2));

            m_Indices[n*6 + 0] = (GLuint)(n*6 + 0);
            m_Indices[n*6 + 1] = (GLuint)(n*6 + 1);
            m_Indices[n*6 + 2] = (GLuint)(n*6 + 2);
            m_Indices[n*6 + 3] = (GLuint)(n*6 + 3);
            m_Indices[n*6 + 4] = (GLuint)(n*6 + 4);
            m_Indices[n*6 + 5] = (GLuint)(n*6 + 5);

            n = n + 1;
        }}

        m_IndexLength = icount;

        glad_glGenBuffers(1,&m_vbo);
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
        glad_glBufferData(GL_ARRAY_BUFFER,vcount*sizeof(Vertex),m_VertexData.data(),GL_STATIC_DRAW);
        glad_glBindBuffer(GL_ARRAY_BUFFER,0);

        glad_glGenBuffers(1,&m_ibo);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ibo);
        glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER,icount*sizeof(GLuint),m_Indices.data(),GL_STATIC_DRAW);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

        // bind vao
        glad_glGenVertexArrays(1,&m_vao);
        glad_glBindVertexArray(m_vao);
        // attach vbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
        glad_glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,m_position));
        glad_glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,m_UV));
        glad_glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,m_normal));
        glad_glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,m_tangent));
        glad_glVertexAttribPointer(4,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,m_bitangent));
        glad_glVertexAttribPointer(5,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,m_color));
        glad_glEnableVertexAttribArray(0);
        glad_glEnableVertexAttribArray(1);
        glad_glEnableVertexAttribArray(2);
        glad_glEnableVertexAttribArray(3);
        glad_glEnableVertexAttribArray(4);
        glad_glEnableVertexAttribArray(5);
        // attach ibo
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ibo);
        glad_glBindVertexArray(0);

        glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    inline void CreateFromInfiniteCenteredGrid(int x)
    {
        int n_total = x*x*6;

        m_VertexData = std::vector<Vertex>(n_total);
        m_Indices = std::vector<GLuint>(n_total);

        int icount = n_total;
        int vcount = n_total;

        int n = 0;
        for(int r = 0; r < x; r = r + 1){
        for (int c = 0; c < x; c= c + 1){

            float m = 1.0f;
            float cs = 2.0/(double(x));
            float c2 = -1.0f + cs * c;
            float r2 = -1.0f + cs * r;
            float c2m = -1.0f + cs * (c+m);
            float r2m = -1.0f + cs * (r+m);

            if(c == 0)
            {
                c2 = -1000.0;
            }
            if(c == x-1)
            {
                c2m = +1000.0;
            }

            if(r == 0)
            {
                r2 = -1000.0;
            }
            if(r == x-1)
            {
                r2m = + 1000.0;
            }

            m_VertexData[n*6 + 0] = Vertex(LSMVector3((double)c2,-10.0f,(double)r2));
            m_VertexData[n*6 + 1] = Vertex(LSMVector3((double)c2m,-10.0f,(double)r2));
            m_VertexData[n*6 + 2] = Vertex(LSMVector3((double)c2,-10.0f,(double)r2m));

            m_VertexData[n*6 + 3] = Vertex(LSMVector3((double)c2m,-10.0f,r2m));
            m_VertexData[n*6 + 4] = Vertex(LSMVector3((double)c2,-10.0f,(double)r2m));
            m_VertexData[n*6 + 5] = Vertex(LSMVector3((double)c2m,-10.0f,(double)r2));

            m_Indices[n*6 + 0] = (GLuint)(n*6 + 0);
            m_Indices[n*6 + 1] = (GLuint)(n*6 + 1);
            m_Indices[n*6 + 2] = (GLuint)(n*6 + 2);
            m_Indices[n*6 + 3] = (GLuint)(n*6 + 3);
            m_Indices[n*6 + 4] = (GLuint)(n*6 + 4);
            m_Indices[n*6 + 5] = (GLuint)(n*6 + 5);

            n = n + 1;
        }}

        m_IndexLength = icount;

        glad_glGenBuffers(1,&m_vbo);
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
        glad_glBufferData(GL_ARRAY_BUFFER,vcount*sizeof(Vertex),m_VertexData.data(),GL_STATIC_DRAW);
        glad_glBindBuffer(GL_ARRAY_BUFFER,0);

        glad_glGenBuffers(1,&m_ibo);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ibo);
        glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER,icount*sizeof(GLuint),m_Indices.data(),GL_STATIC_DRAW);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

        // bind vao
        glad_glGenVertexArrays(1,&m_vao);
        glad_glBindVertexArray(m_vao);
        // attach vbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
        glad_glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,m_position));
        glad_glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,m_UV));
        glad_glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,m_normal));
        glad_glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,m_tangent));
        glad_glVertexAttribPointer(4,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,m_bitangent));
        glad_glVertexAttribPointer(5,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,m_color));
        glad_glEnableVertexAttribArray(0);
        glad_glEnableVertexAttribArray(1);
        glad_glEnableVertexAttribArray(2);
        glad_glEnableVertexAttribArray(3);
        glad_glEnableVertexAttribArray(4);
        glad_glEnableVertexAttribArray(5);
        // attach ibo
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ibo);
        glad_glBindVertexArray(0);

        glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    inline void CreateFrom(const Vertex * v, int vcount, const GLuint * i, int icount)
    {
        m_IndexLength = icount;
        m_VertexData = std::vector<Vertex>(vcount);
        m_Indices = std::vector<GLuint>(icount);
        std::copy(&v[0], &v[vcount-1], back_inserter(m_VertexData));
        std::copy(&i[0], &i[icount-1], back_inserter(m_Indices));


        glad_glGenBuffers(1,&m_vbo);
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
        glad_glBufferData(GL_ARRAY_BUFFER,vcount*sizeof(Vertex),v,GL_STATIC_DRAW);
        glad_glBindBuffer(GL_ARRAY_BUFFER,0);

        glad_glGenBuffers(1,&m_ibo);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ibo);
        glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER,icount*sizeof(GLuint),i,GL_STATIC_DRAW);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

        // bind vao
        glad_glGenVertexArrays(1,&m_vao);
        glad_glBindVertexArray(m_vao);
        // attach vbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
        glad_glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,m_position));
        glad_glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,m_UV));
        glad_glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,m_normal));
        glad_glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,m_tangent));
        glad_glVertexAttribPointer(4,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,m_bitangent));
        glad_glVertexAttribPointer(5,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,m_color));
        glad_glEnableVertexAttribArray(0);
        glad_glEnableVertexAttribArray(1);
        glad_glEnableVertexAttribArray(2);
        glad_glEnableVertexAttribArray(3);
        glad_glEnableVertexAttribArray(4);
        glad_glEnableVertexAttribArray(5);
        // attach ibo
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ibo);
        glad_glBindVertexArray(0);

        glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    inline void Destroy()
    {
        glad_glDeleteVertexArrays(1,&m_vao);
        glad_glDeleteBuffers(1,&m_ibo);
        glad_glDeleteBuffers(1,&m_vbo);

    }
};





#endif // GL3DGEOMETRY_H

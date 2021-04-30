#ifndef OPENGLGEOMETRY_H
#define OPENGLGEOMETRY_H

#include "defines.h"
#ifdef OS_WIN
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif

#ifdef OS_LNX
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#endif



#include <glad/glad.h>
#include "OpenCLUtil.h"
#include "cl.hpp"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "OpenGLUtil.h"
#include "math.h"
#include <QList>
#include <QString>

#include "iostream"

static const float quad_vertices[12] =
{
    -1.0f,-1.0f, 0.0,
     1.0f,-1.0f, 0.0,
     1.0f, 1.0f, 0.0,
    -1.0f, 1.0f, 0.0
};

static const float quad_texcords[8] =
{
    0.0, 0.0,
    1.0, 0.0,
    1.0, 1.0,
    0.0, 1.0
};

static const unsigned int quad_indices[6] = {0,2,1,0,3,2};


class LISEM_API OpenGLGeometry
{
public:
    std::vector<float> m_vertexdata;
    std::vector<float> m_colordata;
    std::vector<float> m_texcoorddata;
    std::vector<float> m_vertexorgdata;
    std::vector<float> m_vertexorg2data;
    std::vector<float> m_attributedata;
    std::vector<float> m_normaldata;
    std::vector<int> m_indices;
    std::vector<float> m_numberattributes;
    std::vector<int> m_numberindex;

    GLuint m_vao = 0; //array object describing the buffers
    GLuint m_vbo = 0; //vertex
    GLuint m_vbocolor = 0; //vertex color
    GLuint m_vboorg = 0; //origin of the line
    GLuint m_vboorg2 = 0; //origin of the shape
    GLuint m_tbo = 0; //texture buffer object
    GLuint m_ibo = 0; //index buffer object (for rendering)

    int m_indexlength;

    bool m_hasattribute;
    GLuint m_nbo; //index buffer object (for shape recognition)
    GLuint m_abo; //attribute buffer object


public:
    OpenGLGeometry();

    inline int CreateUnitQuad()
    {
        m_vertexdata = std::vector<float>(12);
        m_texcoorddata = std::vector<float>(8);
        m_indices = std::vector<int>(6);

        std::copy(&quad_vertices[0], &quad_vertices[12], back_inserter(m_vertexdata));
        std::copy(&quad_texcords[0], &quad_texcords[12], back_inserter(m_texcoorddata));
        std::copy(&quad_indices[0], &quad_indices[12], back_inserter(m_indices));

        m_vbo  = createBuffer(12,quad_vertices,GL_STATIC_DRAW);
        m_tbo  = createBuffer(8,quad_texcords,GL_STATIC_DRAW);
        glad_glGenBuffers(1,&m_ibo);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(uint)*6,quad_indices,GL_STATIC_DRAW);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        // bind vao
        glad_glGenVertexArrays(1,&m_vao);
        glad_glBindVertexArray(m_vao);
        // attach vbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
        glad_glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
        glad_glEnableVertexAttribArray(0);
        // attach tbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_tbo);
        glad_glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,NULL);
        glad_glEnableVertexAttribArray(1);
        // attach ibo
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ibo);
        glad_glBindVertexArray(0);

        m_indexlength = 6;

        return 0;
    }

    inline int CreateFromPoints(float * x, float *y, int length, bool with_attribute, float*a = nullptr, int*n = nullptr)
    {
        int numelements = length;
        m_hasattribute = with_attribute;
        m_vertexdata = std::vector<float>(numelements *3);
        m_numberattributes = std::vector<float>(numelements * 2);
        m_numberindex = std::vector<int>(numelements * 2);
        m_indices = std::vector<int>(numelements);

        for(int i = 0; i < numelements; i +=1)
        {
            m_vertexdata[i * 3 + 0] =x[i];
            m_vertexdata[i * 3 + 1] =y[i];
            m_vertexdata[i * 3 + 2] =0.0f;
            m_indices[i] = i;
        }

        m_vbo  = createBuffer(numelements * 3,m_vertexdata.data(),GL_STATIC_DRAW);
        glad_glGenBuffers(1,&m_ibo);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER,numelements * sizeof(uint)*1,m_indices.data(),GL_STATIC_DRAW);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        // bind vao
        glad_glGenVertexArrays(1,&m_vao);
        glad_glBindVertexArray(m_vao);
        // attach vbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
        glad_glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
        glad_glEnableVertexAttribArray(0);
        // attach ibo
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ibo);
        glad_glBindVertexArray(0);

        m_indexlength = numelements;

        return 0;
    }

    inline int CreateFrom3DPointsWithColor(float * x, float *y, float * z, float * r, float * g, float * b, int length, float*a = nullptr, int*n = nullptr)
    {
        int numelements = length;
        m_hasattribute = false;
        m_vertexdata = std::vector<float>(numelements *3);
        m_colordata = std::vector<float>(numelements * 3);
        m_numberattributes = std::vector<float>(numelements * 2);
        m_numberindex = std::vector<int>(numelements * 2);
        m_indices = std::vector<int>(numelements);

        for(int i = 0; i < numelements; i +=1)
        {
            m_vertexdata[i * 3 + 0] =x[i];
            m_vertexdata[i * 3 + 1] =y[i];
            m_vertexdata[i * 3 + 2] =z[i];
            m_colordata[i * 3 + 0] =r[i];
            m_colordata[i * 3 + 1] =g[i];
            m_colordata[i * 3 + 2] =b[i];
            m_indices[i] = i;
        }

        m_vbo  = createBuffer(numelements * 3,m_vertexdata.data(),GL_STATIC_DRAW);
        m_vbocolor  = createBuffer(numelements * 3,m_colordata.data(),GL_STATIC_DRAW);
        glad_glGenBuffers(1,&m_ibo);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER,numelements * sizeof(uint)*1,m_indices.data(),GL_STATIC_DRAW);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        // bind vao
        glad_glGenVertexArrays(1,&m_vao);
        glad_glBindVertexArray(m_vao);
        // attach vbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
        glad_glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
        glad_glEnableVertexAttribArray(0);
        // attach color vbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vbocolor);
        glad_glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,NULL);
        glad_glEnableVertexAttribArray(1);
        // attach ibo
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ibo);
        glad_glBindVertexArray(0);

        m_indexlength = numelements;

        return 0;
    }

    inline int CreateFromTriangles(float * x, float *y, int length, bool with_attribute, float*a = nullptr, int*n = nullptr)
    {
        int numelements = length/3;
        m_hasattribute = with_attribute;
        m_vertexdata = std::vector<float>(numelements * 9);

        m_vertexorg2data = std::vector<float>(numelements * 9);

        m_texcoorddata = std::vector<float>(numelements * 6);
        m_numberattributes = std::vector<float>(numelements * 3);
        m_numberindex = std::vector<int>(numelements * 3);
        m_indices = std::vector<int>(numelements * 3);

        for(int i = 0; i < numelements; i +=1)
        {
            m_vertexdata[i * 9 + 0] =x[i*3];
            m_vertexdata[i * 9 + 1] =y[i*3];
            m_vertexdata[i * 9 + 2] =0.0f;
            m_vertexdata[i * 9 + 3] =x[i*3+1];
            m_vertexdata[i * 9 + 4] =y[i*3+1];
            m_vertexdata[i * 9 + 5] =0.0f;
            m_vertexdata[i * 9 + 6] =x[i*3+2];
            m_vertexdata[i * 9 + 7] =y[i*3+2];
            m_vertexdata[i * 9 + 8] =0.0f;

            m_texcoorddata[i * 6 + 0] = quad_texcords[0];
            m_texcoorddata[i * 6 + 1] = quad_texcords[1];
            m_texcoorddata[i * 6 + 2] = quad_texcords[2];
            m_texcoorddata[i * 6 + 3] = quad_texcords[3];
            m_texcoorddata[i * 6 + 4] = quad_texcords[4];
            m_texcoorddata[i * 6 + 5] = quad_texcords[5];

            if(with_attribute)
            {
                m_numberattributes[i * 3 + 0] = a[i * 3+ 0];
                m_numberattributes[i * 3 + 1] = a[i * 3+ 1];
                m_numberattributes[i * 3 + 2] = a[i * 3+ 2];
                m_numberindex[i * 3+ 0] = n[i * 3+ 0];
                m_numberindex[i * 3+ 1] = n[i * 3+ 1];
                m_numberindex[i * 3+ 2] = n[i * 3+ 2];
            }

            m_indices[i * 3 + 0] = i * 3 + 0;
            m_indices[i * 3 + 1] = i * 3 + 1;
            m_indices[i * 3 + 2] = i * 3 + 2;

        }

        m_vbo  = createBuffer(numelements * 9,m_vertexdata.data(),GL_STATIC_DRAW);
        m_tbo  = createBuffer(numelements * 6,m_texcoorddata.data(),GL_STATIC_DRAW);
        m_vboorg2  = createBuffer(numelements * 9,m_vertexorg2data.data(),GL_STATIC_DRAW);
        glad_glGenBuffers(1,&m_ibo);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER,numelements * sizeof(uint)*3,m_indices.data(),GL_STATIC_DRAW);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        // bind vao
        glad_glGenVertexArrays(1,&m_vao);
        glad_glBindVertexArray(m_vao);
        // attach vbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
        glad_glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
        glad_glEnableVertexAttribArray(0);
        // attach tbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_tbo);
        glad_glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,NULL);
        glad_glEnableVertexAttribArray(1);
        // attach vbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vboorg2);
        glad_glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,NULL);
        glad_glEnableVertexAttribArray(2);
        // attach ibo
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ibo);


        if(m_hasattribute)
        {
            m_nbo  = createBuffer(numelements * 3,m_numberindex.data(),GL_STATIC_DRAW);
            m_abo  = createBuffer(numelements * 3,m_numberattributes.data(),GL_STATIC_DRAW);
            glad_glBindBuffer(GL_ARRAY_BUFFER,m_nbo);
            glad_glVertexAttribIPointer(3,1,GL_INT,0,NULL);
            glad_glEnableVertexAttribArray(3);

            glad_glBindBuffer(GL_ARRAY_BUFFER,m_abo);
            glad_glVertexAttribPointer(4,1,GL_FLOAT,GL_FALSE,0,NULL);
            glad_glEnableVertexAttribArray(4);
        }

        glad_glBindVertexArray(0);
        m_indexlength = numelements * 3;

        return 0;
    }

    inline int CreateFromLines(float *x, float * y, int * connect, int length, int width, bool with_attribute, float*a = nullptr, int*n = nullptr)
    {
        int n_connect = 0;
        bool has_connect = (connect != nullptr);
        m_hasattribute = with_attribute;
        if(has_connect)
        {
            for(int i = 0; i < length/2; i++)
            {
                if(connect[i] > 0)
                {
                    n_connect += 1;
                }
            }
        }

        int numelements = length/2;

        m_vertexdata = std::vector<float>(numelements * 12 + n_connect * 6);
        m_vertexorgdata = std::vector<float>(numelements * 12 + n_connect * 6);
        m_texcoorddata = std::vector<float>(numelements * 8+ n_connect * 4);
        m_numberattributes = std::vector<float>(numelements * 4+ n_connect * 2);
        m_numberindex = std::vector<int>(numelements * 4+ n_connect * 2);
        m_indices = std::vector<int>(numelements * 6+ n_connect * 3);

        float dx_last = 0.0f;
        float dy_last = 0.0f;
        int connected = 0;
        float lcum = 0.0f;
        bool last_connected = false;
        for(int i = 0; i < numelements; i +=1)
        {
            float dx = x[i*2] - x[i*2+1];
            float dy = y[i*2] - y[i*2+1];

            float l = sqrt(dx*dx + dy*dy);
            dx = dx/l;
            dy = dy/l;
            //if(relativesize)
            {
                width = 1.0;
            }

            {
                float dxe = 0.5f * width *(-dy);
                float dye = 0.5f * width *(dx);

                m_vertexdata[i * 12 + connected * 6+ 0] =x[i*2] - dxe;
                m_vertexdata[i * 12 + connected * 6+ 1] =y[i*2] - dye;
                m_vertexdata[i * 12 + connected * 6+ 2] =lcum;
                m_vertexdata[i * 12 + connected * 6+ 3] =x[i*2];// + dxe;
                m_vertexdata[i * 12 + connected * 6+ 4] =y[i*2];// + dye;
                m_vertexdata[i * 12 + connected * 6+ 5] =lcum;
                m_vertexdata[i * 12 + connected * 6+ 6] =x[i*2+1] - dxe;
                m_vertexdata[i * 12 + connected * 6+ 7] =y[i*2+1] - dye;
                m_vertexdata[i * 12 + connected * 6+ 8] =lcum+l;
                m_vertexdata[i * 12 + connected * 6+ 9] =x[i*2+1];// + dxe;
                m_vertexdata[i * 12 + connected * 6+ 10] =y[i*2+1];// + dye;
                m_vertexdata[i * 12 + connected * 6+ 11] =lcum+l;

                m_vertexorgdata[i * 12 + connected * 6+ 0] =x[i*2];
                m_vertexorgdata[i * 12 + connected * 6+ 1] =y[i*2];
                m_vertexorgdata[i * 12 + connected * 6+ 2] =0.0f;
                m_vertexorgdata[i * 12 + connected * 6+ 3] =x[i*2];// + dxe;
                m_vertexorgdata[i * 12 + connected * 6+ 4] =y[i*2];// + dye;
                m_vertexorgdata[i * 12 + connected * 6+ 5] =0.0f;
                m_vertexorgdata[i * 12 + connected * 6+ 6] =x[i*2+1];
                m_vertexorgdata[i * 12 + connected * 6+ 7] =y[i*2+1];
                m_vertexorgdata[i * 12 + connected * 6+ 8] =0.0f;
                m_vertexorgdata[i * 12 + connected * 6+ 9] =x[i*2+1];// + dxe;
                m_vertexorgdata[i * 12 + connected * 6+ 10] =y[i*2+1];// + dye;
                m_vertexorgdata[i * 12 + connected * 6+ 11] =0.0f;

                m_texcoorddata[i * 8 + connected * 4+ 0] = quad_texcords[0];
                m_texcoorddata[i * 8 + connected * 4+ 1] = quad_texcords[1];
                m_texcoorddata[i * 8 + connected * 4+ 2] = quad_texcords[2];
                m_texcoorddata[i * 8 + connected * 4+ 3] = quad_texcords[3];
                m_texcoorddata[i * 8 + connected * 4+ 4] = quad_texcords[4];
                m_texcoorddata[i * 8 + connected * 4+ 5] = quad_texcords[5];
                m_texcoorddata[i * 8 + connected * 4+ 6] = quad_texcords[6];
                m_texcoorddata[i * 8 + connected * 4+ 7] = quad_texcords[7];

                if(with_attribute)
                {
                    m_numberattributes[i * 4 + connected * 2+ 0] = a[i*2];
                    m_numberattributes[i * 4 + connected * 2+ 1] = a[i*2];
                    m_numberattributes[i * 4 + connected * 2+ 2] = a[i*2+1];
                    m_numberattributes[i * 4 + connected * 2+ 3] = a[i*2+1];
                    m_numberindex[i * 4 + connected * 2+ 0] = n[i*2];
                    m_numberindex[i * 4 + connected * 2+ 1] = n[i*2];
                    m_numberindex[i * 4 + connected * 2+ 2] = n[i*2+1];
                    m_numberindex[i * 4 + connected * 2+ 3] = n[i*2+1];
                }

                m_indices[i * 6 + connected * 3+ 0] = i * 4 + connected* 2+ 0;
                m_indices[i * 6 + connected * 3+ 1] = i * 4 + connected* 2+ 1;
                m_indices[i * 6 + connected * 3+ 2] = i * 4 + connected* 2+ 2;
                m_indices[i * 6 + connected * 3+ 3] = i * 4 + connected* 2+ 2;
                m_indices[i * 6 + connected * 3+ 4] = i * 4 + connected* 2+ 3;
                m_indices[i * 6 + connected * 3+ 5] = i * 4 + connected* 2+ 1;


                if(has_connect && i > 0  && connect[i] > 0)
                {
                    bool turnleft = (dx * dy_last - dy * dx_last) > 0.0f;
                    int i_temp = i+1;
                    m_vertexdata[i_temp * 12 + connected * 6+ 0] =x[(i-1)*2+1];
                    m_vertexdata[i_temp * 12 + connected * 6+ 1] =y[(i-1)*2+1];
                    m_vertexdata[i_temp * 12 + connected * 6+ 2] =lcum;
                    m_vertexdata[i_temp * 12 + connected * 6+ 3] =x[i*2];
                    m_vertexdata[i_temp * 12 + connected * 6+ 4] =y[i*2];
                    m_vertexdata[i_temp * 12 + connected * 6+ 5] =lcum;

                    m_vertexorgdata[i_temp * 12 + connected * 6+ 0] =x[(i-1)*2+1];
                    m_vertexorgdata[i_temp * 12 + connected * 6+ 1] =y[(i-1)*2+1];
                    m_vertexorgdata[i_temp * 12 + connected * 6+ 2] =0.0f;
                    m_vertexorgdata[i_temp * 12 + connected * 6+ 3] =x[i*2];
                    m_vertexorgdata[i_temp * 12 + connected * 6+ 4] =y[i*2];
                    m_vertexorgdata[i_temp * 12 + connected * 6+ 5] =0.0f;

                    m_texcoorddata[i_temp * 8 + connected * 4+ 0] = quad_texcords[2];
                    m_texcoorddata[i_temp * 8 + connected * 4+ 1] = quad_texcords[3];
                    m_texcoorddata[i_temp * 8 + connected * 4+ 2] = quad_texcords[2];
                    m_texcoorddata[i_temp * 8 + connected * 4+ 3] = quad_texcords[3];

                    if(with_attribute)
                    {
                        m_numberattributes[i_temp * 4 + connected * 2+ 0] = a[(i-1)*2+1];
                        m_numberattributes[i_temp * 4 + connected * 2+ 1] = a[i*2];
                        m_numberindex[i_temp * 4 + connected * 2+ 0] = n[(i-1)*2+1];
                        m_numberindex[i_temp * 4 + connected * 2+ 1] = n[i*2];
                    }

                    if(turnleft)
                    {
                        m_indices[i_temp * 6 + connected * 3+ 0] = i_temp * 4 + connected* 2+ 1;
                        m_indices[i_temp * 6 + connected * 3+ 1] = m_indices[i * 6 + connected * 3+ 0];
                        m_indices[i_temp * 6 + connected * 3+ 2] = m_indices[(i-1) * 6 + (connected-(last_connected?1:0)) * 3+ 0];
                    }else {
                        m_indices[i_temp * 6 + connected * 3+ 0] = i_temp * 4 + connected* 2+ 1;
                        m_indices[i_temp * 6 + connected * 3+ 1] = m_indices[i * 6 + connected * 3+ 1];
                        m_indices[i_temp * 6 + connected * 3+ 2] = m_indices[(i-1) * 6 + (connected-(last_connected?1:0)) * 3+ 1];
                    }
                    last_connected = true;
                    connected += 1;
                }
                last_connected = false;
            }
            dx_last = dx;
            dy_last = dy;
            lcum = lcum + l;
        }

        m_vbo  = createBuffer(numelements * 12 + n_connect * 6,m_vertexdata.data(),GL_STATIC_DRAW);
        m_vboorg  = createBuffer(numelements * 12 + n_connect * 6,m_vertexorgdata.data(),GL_STATIC_DRAW);
        m_tbo  = createBuffer(numelements * 8 + n_connect * 4,m_texcoorddata.data(),GL_STATIC_DRAW);

        glad_glGenBuffers(1,&m_ibo);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER,numelements * sizeof(uint)*6 + n_connect * sizeof(uint)*3,m_indices.data(),GL_STATIC_DRAW);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        // bind vao
        glad_glGenVertexArrays(1,&m_vao);
        glad_glBindVertexArray(m_vao);
        // attach vbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
        glad_glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
        glad_glEnableVertexAttribArray(0);
        // attach tbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_tbo);
        glad_glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,NULL);
        glad_glEnableVertexAttribArray(1);
        // attach vbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vboorg);
        glad_glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,NULL);
        glad_glEnableVertexAttribArray(2);
        // attach ibo
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ibo);

        if(m_hasattribute)
        {
            m_nbo  = createBuffer(numelements * 4 + n_connect * 2,m_numberindex.data(),GL_STATIC_DRAW);
            m_abo  = createBuffer(numelements * 4 + n_connect * 2,m_numberattributes.data(),GL_STATIC_DRAW);
            glad_glBindBuffer(GL_ARRAY_BUFFER,m_nbo);
            glad_glVertexAttribIPointer(4,1,GL_INT,0,NULL);
            glad_glEnableVertexAttribArray(4);

            glad_glBindBuffer(GL_ARRAY_BUFFER,m_abo);
            glad_glVertexAttribPointer(5,1,GL_FLOAT,GL_FALSE,0,NULL);
            glad_glEnableVertexAttribArray(5);
        }

        glad_glBindVertexArray(0);


        m_indexlength = numelements * 6 +n_connect * 3;

        return 0;
    }


    inline int CreateFromTrianglesWithOrigin(float * x, float *y, float * xr, float *yr, int length, bool with_attribute, float*a = nullptr, int*n = nullptr )
    {
        int numelements = length/3;

        m_vertexdata = std::vector<float>(numelements * 9);
        m_vertexorg2data = std::vector<float>(numelements * 9);
        m_texcoorddata = std::vector<float>(numelements * 6);
        m_numberattributes = std::vector<float>(numelements * 3);
        m_numberindex = std::vector<int>(numelements * 3);
        m_indices = std::vector<int>(numelements * 3);
        m_hasattribute = with_attribute;
        for(int i = 0; i < numelements; i +=1)
        {
            m_vertexdata[i * 9 + 0] =x[i*3];
            m_vertexdata[i * 9 + 1] =y[i*3];
            m_vertexdata[i * 9 + 2] =0.0f;
            m_vertexdata[i * 9 + 3] =x[i*3+1];
            m_vertexdata[i * 9 + 4] =y[i*3+1];
            m_vertexdata[i * 9 + 5] =0.0f;
            m_vertexdata[i * 9 + 6] =x[i*3+2];
            m_vertexdata[i * 9 + 7] =y[i*3+2];
            m_vertexdata[i * 9 + 8] =0.0f;

            m_vertexorg2data[i * 9 + 0] =xr[i*3];
            m_vertexorg2data[i * 9 + 1] =yr[i*3];
            m_vertexorg2data[i * 9 + 2] =0.0f;
            m_vertexorg2data[i * 9 + 3] =xr[i*3+1];
            m_vertexorg2data[i * 9 + 4] =yr[i*3+1];
            m_vertexorg2data[i * 9 + 5] =0.0f;
            m_vertexorg2data[i * 9 + 6] =xr[i*3+2];
            m_vertexorg2data[i * 9 + 7] =yr[i*3+2];
            m_vertexorg2data[i * 9 + 8] =0.0f;

            m_texcoorddata[i * 6 + 0] = quad_texcords[0];
            m_texcoorddata[i * 6 + 1] = quad_texcords[1];
            m_texcoorddata[i * 6 + 2] = quad_texcords[2];
            m_texcoorddata[i * 6 + 3] = quad_texcords[3];
            m_texcoorddata[i * 6 + 4] = quad_texcords[4];
            m_texcoorddata[i * 6 + 5] = quad_texcords[5];

            if(with_attribute)
            {
                m_numberattributes[i * 3 + 0] = a[i * 3+ 0];
                m_numberattributes[i * 3 + 1] = a[i * 3+ 1];
                m_numberattributes[i * 3 + 2] = a[i * 3+ 2];
                m_numberindex[i * 3+ 0] = n[i * 3+ 0];
                m_numberindex[i * 3+ 1] = n[i * 3+ 1];
                m_numberindex[i * 3+ 2] = n[i * 3+ 2];
            }

            m_indices[i * 3 + 0] = i * 3 + 0;
            m_indices[i * 3 + 1] = i * 3 + 1;
            m_indices[i * 3 + 2] = i * 3 + 2;


        }

        m_vbo  = createBuffer(numelements * 9,m_vertexdata.data(),GL_STATIC_DRAW);
        m_vboorg2  = createBuffer(numelements * 9,m_vertexorg2data.data(),GL_STATIC_DRAW);
        m_tbo  = createBuffer(numelements * 6,m_texcoorddata.data(),GL_STATIC_DRAW);
        glad_glGenBuffers(1,&m_ibo);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER,numelements * sizeof(uint)*3,m_indices.data(),GL_STATIC_DRAW);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        // bind vao
        glad_glGenVertexArrays(1,&m_vao);
        glad_glBindVertexArray(m_vao);
        // attach vbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
        glad_glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
        glad_glEnableVertexAttribArray(0);
        // attach tbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_tbo);
        glad_glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,NULL);
        glad_glEnableVertexAttribArray(1);
        // attach vbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vboorg2);
        glad_glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,NULL);
        glad_glEnableVertexAttribArray(2);
        // attach ibo
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ibo);


        if(m_hasattribute)
        {
            m_nbo  = createBuffer(numelements * 3,m_numberindex.data(),GL_STATIC_DRAW);
            m_abo  = createBuffer(numelements * 3,m_numberattributes.data(),GL_STATIC_DRAW);
            glad_glBindBuffer(GL_ARRAY_BUFFER,m_nbo);
            glad_glVertexAttribIPointer(3,1,GL_INT,0,NULL);
            glad_glEnableVertexAttribArray(4);

            glad_glBindBuffer(GL_ARRAY_BUFFER,m_abo);
            glad_glVertexAttribPointer(4,1,GL_FLOAT,GL_FALSE,0,NULL);
            glad_glEnableVertexAttribArray(5);
        }

        glad_glBindVertexArray(0);

        m_indexlength = numelements * 3;

        return 0;

    }

    inline int CreateFromLinesWithOrigin(float *x, float * y, int * connect,float *xr, float * yr, int length, int width, bool with_attribute, float*a = nullptr, int*n = nullptr)
    {

        int n_connect = 0;
        bool has_connect = (connect != nullptr);
        m_hasattribute = with_attribute;
        if(has_connect)
        {
            for(int i = 0; i < length/2; i++)
            {
                if(connect[i] > 0)
                {
                    n_connect += 1;
                }
            }
        }

        int numelements = length/2;

        m_vertexdata = std::vector<float>(numelements * 12 + n_connect * 6);
        m_vertexorgdata = std::vector<float>(numelements * 12 + n_connect * 6);
        m_vertexorg2data = std::vector<float>(numelements * 12 + n_connect * 6);
        m_texcoorddata = std::vector<float>(numelements * 8+ n_connect * 4);
        m_numberattributes = std::vector<float>(numelements * 4+ n_connect * 2);
        m_numberindex = std::vector<int>(numelements * 4+ n_connect * 2);
        m_indices = std::vector<int>(numelements * 6+ n_connect * 3);

        float dx_last = 0.0f;
        float dy_last = 0.0f;
        int connected = 0;
        float lcum = 0.0f;
        bool last_connected = false;
        for(int i = 0; i < numelements; i +=1)
        {
            float dx = x[i*2] - x[i*2+1];
            float dy = y[i*2] - y[i*2+1];

            float l = sqrt(dx*dx + dy*dy);
            dx = dx/l;
            dy = dy/l;
            //if(relativesize)
            {
                width = 1.0;
            }

            {
                float dxe = 0.5f * width *(-dy);
                float dye = 0.5f * width *(dx);

                m_vertexdata[i * 12 + connected * 6+ 0] =x[i*2] - dxe;
                m_vertexdata[i * 12 + connected * 6+ 1] =y[i*2] - dye;
                m_vertexdata[i * 12 + connected * 6+ 2] =lcum;
                m_vertexdata[i * 12 + connected * 6+ 3] =x[i*2];// + dxe;
                m_vertexdata[i * 12 + connected * 6+ 4] =y[i*2];// + dye;
                m_vertexdata[i * 12 + connected * 6+ 5] =lcum;
                m_vertexdata[i * 12 + connected * 6+ 6] =x[i*2+1] - dxe;
                m_vertexdata[i * 12 + connected * 6+ 7] =y[i*2+1] - dye;
                m_vertexdata[i * 12 + connected * 6+ 8] =lcum+l;
                m_vertexdata[i * 12 + connected * 6+ 9] =x[i*2+1];// + dxe;
                m_vertexdata[i * 12 + connected * 6+ 10] =y[i*2+1];// + dye;
                m_vertexdata[i * 12 + connected * 6+ 11] =lcum+l;

                m_vertexorgdata[i * 12 + connected * 6+ 0] =x[i*2];
                m_vertexorgdata[i * 12 + connected * 6+ 1] =y[i*2];
                m_vertexorgdata[i * 12 + connected * 6+ 2] =0.0f;
                m_vertexorgdata[i * 12 + connected * 6+ 3] =x[i*2];// + dxe;
                m_vertexorgdata[i * 12 + connected * 6+ 4] =y[i*2];// + dye;
                m_vertexorgdata[i * 12 + connected * 6+ 5] =0.0f;
                m_vertexorgdata[i * 12 + connected * 6+ 6] =x[i*2+1];
                m_vertexorgdata[i * 12 + connected * 6+ 7] =y[i*2+1];
                m_vertexorgdata[i * 12 + connected * 6+ 8] =0.0f;
                m_vertexorgdata[i * 12 + connected * 6+ 9] =x[i*2+1];// + dxe;
                m_vertexorgdata[i * 12 + connected * 6+ 10] =y[i*2+1];// + dye;
                m_vertexorgdata[i * 12 + connected * 6+ 11] =0.0f;

                m_vertexorg2data[i * 12 + connected * 6+ 0] =xr[i*2];
                m_vertexorg2data[i * 12 + connected * 6+ 1] =yr[i*2];
                m_vertexorg2data[i * 12 + connected * 6+ 2] =0.0f;
                m_vertexorg2data[i * 12 + connected * 6+ 3] =xr[i*2];// + dxe;
                m_vertexorg2data[i * 12 + connected * 6+ 4] =yr[i*2];// + dye;
                m_vertexorg2data[i * 12 + connected * 6+ 5] =0.0f;
                m_vertexorg2data[i * 12 + connected * 6+ 6] =xr[i*2+1];
                m_vertexorg2data[i * 12 + connected * 6+ 7] =yr[i*2+1];
                m_vertexorg2data[i * 12 + connected * 6+ 8] =0.0f;
                m_vertexorg2data[i * 12 + connected * 6+ 9] =xr[i*2+1];// + dxe;
                m_vertexorg2data[i * 12 + connected * 6+ 10] =yr[i*2+1];// + dye;
                m_vertexorg2data[i * 12 + connected * 6+ 11] =0.0f;

                m_texcoorddata[i * 8 + connected * 4+ 0] = quad_texcords[0];
                m_texcoorddata[i * 8 + connected * 4+ 1] = quad_texcords[1];
                m_texcoorddata[i * 8 + connected * 4+ 2] = quad_texcords[2];
                m_texcoorddata[i * 8 + connected * 4+ 3] = quad_texcords[3];
                m_texcoorddata[i * 8 + connected * 4+ 4] = quad_texcords[4];
                m_texcoorddata[i * 8 + connected * 4+ 5] = quad_texcords[5];
                m_texcoorddata[i * 8 + connected * 4+ 6] = quad_texcords[6];
                m_texcoorddata[i * 8 + connected * 4+ 7] = quad_texcords[7];

                if(with_attribute)
                {
                    m_numberattributes[i * 4 + connected * 2+ 0] = a[i*2];
                    m_numberattributes[i * 4 + connected * 2+ 1] = a[i*2];
                    m_numberattributes[i * 4 + connected * 2+ 2] = a[i*2+1];
                    m_numberattributes[i * 4 + connected * 2+ 3] = a[i*2+1];
                    m_numberindex[i * 4 + connected * 2+ 0] = n[i*2];
                    m_numberindex[i * 4 + connected * 2+ 1] = n[i*2];
                    m_numberindex[i * 4 + connected * 2+ 2] = n[i*2+1];
                    m_numberindex[i * 4 + connected * 2+ 3] = n[i*2+1];
                }


                m_indices[i * 6 + connected * 3+ 0] = i * 4 + connected* 2+ 0;
                m_indices[i * 6 + connected * 3+ 1] = i * 4 + connected* 2+ 1;
                m_indices[i * 6 + connected * 3+ 2] = i * 4 + connected* 2+ 2;
                m_indices[i * 6 + connected * 3+ 3] = i * 4 + connected* 2+ 2;
                m_indices[i * 6 + connected * 3+ 4] = i * 4 + connected* 2+ 3;
                m_indices[i * 6 + connected * 3+ 5] = i * 4 + connected* 2+ 1;


                if(has_connect && i > 0  && connect[i] > 0)
                {
                    bool turnleft = (dx * dy_last - dy * dx_last) > 0.0f;
                    int i_temp = i+1;
                    m_vertexdata[i_temp * 12 + connected * 6+ 0] =x[(i-1)*2+1];
                    m_vertexdata[i_temp * 12 + connected * 6+ 1] =y[(i-1)*2+1];
                    m_vertexdata[i_temp * 12 + connected * 6+ 2] =lcum;
                    m_vertexdata[i_temp * 12 + connected * 6+ 3] =x[i*2];
                    m_vertexdata[i_temp * 12 + connected * 6+ 4] =y[i*2];
                    m_vertexdata[i_temp * 12 + connected * 6+ 5] =lcum;

                    m_vertexorgdata[i_temp * 12 + connected * 6+ 0] =x[(i-1)*2+1];
                    m_vertexorgdata[i_temp * 12 + connected * 6+ 1] =y[(i-1)*2+1];
                    m_vertexorgdata[i_temp * 12 + connected * 6+ 2] =0.0f;
                    m_vertexorgdata[i_temp * 12 + connected * 6+ 3] =x[i*2];
                    m_vertexorgdata[i_temp * 12 + connected * 6+ 4] =y[i*2];
                    m_vertexorgdata[i_temp * 12 + connected * 6+ 5] =0.0f;

                    m_vertexorg2data[i_temp * 12 + connected * 6+ 0] =xr[(i-1)*2+1];
                    m_vertexorg2data[i_temp * 12 + connected * 6+ 1] =yr[(i-1)*2+1];
                    m_vertexorg2data[i_temp * 12 + connected * 6+ 2] =0.0f;
                    m_vertexorg2data[i_temp * 12 + connected * 6+ 3] =xr[i*2];
                    m_vertexorg2data[i_temp * 12 + connected * 6+ 4] =yr[i*2];
                    m_vertexorg2data[i_temp * 12 + connected * 6+ 5] =0.0f;

                    m_texcoorddata[i_temp * 8 + connected * 4+ 0] = quad_texcords[2];
                    m_texcoorddata[i_temp * 8 + connected * 4+ 1] = quad_texcords[3];
                    m_texcoorddata[i_temp * 8 + connected * 4+ 2] = quad_texcords[2];
                    m_texcoorddata[i_temp * 8 + connected * 4+ 3] = quad_texcords[3];

                    if(with_attribute)
                    {
                        m_numberattributes[i_temp * 4 + connected * 2+ 0] = a[(i-1)*2+1];
                        m_numberattributes[i_temp * 4 + connected * 2+ 1] = a[i*2];
                        m_numberindex[i_temp * 4 + connected * 2+ 0] = n[(i-1)*2+1];
                        m_numberindex[i_temp * 4 + connected * 2+ 1] = n[i*2];
                    }

                    if(turnleft)
                    {
                        m_indices[i_temp * 6 + connected * 3+ 0] = i_temp * 4 + connected* 2+ 1;
                        m_indices[i_temp * 6 + connected * 3+ 1] = m_indices[i * 6 + connected * 3+ 0];
                        m_indices[i_temp * 6 + connected * 3+ 2] = m_indices[(i-1) * 6 + (connected-(last_connected?1:0)) * 3+ 0];
                    }else {
                        m_indices[i_temp * 6 + connected * 3+ 0] = i_temp * 4 + connected* 2+ 1;
                        m_indices[i_temp * 6 + connected * 3+ 1] = m_indices[i * 6 + connected * 3+ 1];
                        m_indices[i_temp * 6 + connected * 3+ 2] = m_indices[(i-1) * 6 + (connected-(last_connected?1:0)) * 3+ 1];
                    }
                    last_connected = true;
                    connected += 1;
                }
                last_connected = false;
            }
            dx_last = dx;
            dy_last = dy;
            lcum = lcum + l;
        }

        m_vbo  = createBuffer(numelements * 12 + n_connect * 6,m_vertexdata.data(),GL_STATIC_DRAW);
        m_vboorg  = createBuffer(numelements * 12 + n_connect * 6,m_vertexorgdata.data(),GL_STATIC_DRAW);
        m_vboorg2  = createBuffer(numelements * 12 + n_connect * 6,m_vertexorg2data.data(),GL_STATIC_DRAW);
        m_tbo  = createBuffer(numelements * 8 + n_connect * 4,m_texcoorddata.data(),GL_STATIC_DRAW);
        glad_glGenBuffers(1,&m_ibo);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER,numelements * sizeof(uint)*6 + n_connect * sizeof(uint)*3,m_indices.data(),GL_STATIC_DRAW);
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        // bind vao
        glad_glGenVertexArrays(1,&m_vao);
        glad_glBindVertexArray(m_vao);
        // attach vbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
        glad_glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
        glad_glEnableVertexAttribArray(0);
        // attach tbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_tbo);
        glad_glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,NULL);
        glad_glEnableVertexAttribArray(1);
        // attach vbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vboorg);
        glad_glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,NULL);
        glad_glEnableVertexAttribArray(2);
        // attach vbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vboorg2);
        glad_glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,0,NULL);
        glad_glEnableVertexAttribArray(3);
        // attach ibo
        glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ibo);


        if(m_hasattribute)
        {
            m_nbo  = createBuffer(numelements * 4 + n_connect * 2,m_numberindex.data(),GL_STATIC_DRAW);
            m_abo  = createBuffer(numelements * 4 + n_connect * 2,m_numberattributes.data(),GL_STATIC_DRAW);
            glad_glBindBuffer(GL_ARRAY_BUFFER,m_nbo);
            glad_glVertexAttribIPointer(4,1,GL_INT,0,NULL);
            glad_glEnableVertexAttribArray(4);

            glad_glBindBuffer(GL_ARRAY_BUFFER,m_abo);
            glad_glVertexAttribPointer(5,1,GL_FLOAT,GL_FALSE,0,NULL);
            glad_glEnableVertexAttribArray(5);
        }

        glad_glBindVertexArray(0);

        m_indexlength = numelements * 6 +n_connect * 3;

        return 0;
    }

    inline int FillFromPoints(float * x, float *y, int length, bool with_attribute, float*a = nullptr, int*n = nullptr)
    {
        int numelements = length;

        for(int i = 0; i < numelements; i +=1)
        {
            m_vertexdata[i * 3 + 0] =x[i];
            m_vertexdata[i * 3 + 1] =y[i];
            m_vertexdata[i * 3 + 2] =0.0f;
        }

        glad_glBufferSubData(m_vbo,0,sizeof(float) * numelements * 3,m_vertexdata.data());

        m_indexlength = numelements * 1;

        return 0;
    }

    inline int FillFromPoints3DWithColor(float * x, float *y, float *z, float * r, float *g, float * b, int length)
    {
        int numelements = length;

        for(int i = 0; i < numelements; i +=1)
        {
            m_vertexdata[i * 3 + 0] =x[i];
            m_vertexdata[i * 3 + 1] =y[i];
            m_vertexdata[i * 3 + 2] =z[i];

            m_colordata[i * 3 + 0] =r[i];
            m_colordata[i * 3 + 1] =g[i];
            m_colordata[i * 3 + 2] =b[i];
        }

        glad_glBufferSubData(m_vbo,0,sizeof(float) * numelements * 3,m_vertexdata.data());
        glad_glBufferSubData(m_vbocolor,0,sizeof(float) * numelements * 3,m_colordata.data());

        m_indexlength = numelements * 1;

        return 0;
    }


    inline int FillFromTriangles(float * x, float *y, int length, bool with_attribute, float*a = nullptr, int*n = nullptr)
    {
        int numelements = length/3;

        std::cout << "update " << with_attribute << "  " << m_hasattribute << std::endl;

        if(with_attribute && m_hasattribute)
        {
            std::cout << "Update with attribute" << std::endl;
        }

        for(int i = 0; i < numelements-1; i +=1)
        {
            m_vertexdata[i * 9 + 0] =x[i*3];
            m_vertexdata[i * 9 + 1] =y[i*3];
            m_vertexdata[i * 9 + 2] =0.0f;
            m_vertexdata[i * 9 + 3] =x[i*3+1];
            m_vertexdata[i * 9 + 4] =y[i*3+1];
            m_vertexdata[i * 9 + 5] =0.0f;
            m_vertexdata[i * 9 + 6] =x[i*3+2];
            m_vertexdata[i * 9 + 7] =y[i*3+2];
            m_vertexdata[i * 9 + 8] =0.0f;

            m_texcoorddata[i * 6 + 0] = quad_texcords[0];
            m_texcoorddata[i * 6 + 1] = quad_texcords[1];
            m_texcoorddata[i * 6 + 2] = quad_texcords[2];
            m_texcoorddata[i * 6 + 3] = quad_texcords[3];
            m_texcoorddata[i * 6 + 4] = quad_texcords[4];
            m_texcoorddata[i * 6 + 5] = quad_texcords[5];

            if(with_attribute && m_hasattribute)
            {
                std::cout << "Update with attribute " << n[i * 3+ 0] << " " <<  a[i * 3+ 0] <<  std::endl;
                m_numberattributes[i * 3 + 0] = a[i * 3+ 0];
                m_numberattributes[i * 3 + 1] = a[i * 3+ 1];
                m_numberattributes[i * 3 + 2] = a[i * 3+ 2];
                m_numberindex[i * 3+ 0] = n[i * 3+ 0];
                m_numberindex[i * 3+ 1] = n[i * 3+ 1];
                m_numberindex[i * 3+ 2] = n[i * 3+ 2];
            }

            m_indices[i * 3 + 0] = i * 3 + 0;
            m_indices[i * 3 + 1] = i * 3 + 1;
            m_indices[i * 3 + 2] = i * 3 + 2;
        }

        glad_glBufferSubData(m_vbo,0,sizeof(float) * numelements * 9,m_vertexdata.data());

        if(with_attribute && m_hasattribute)
        {
            glad_glBufferSubData(m_nbo,0,sizeof(int) * numelements * 3,m_numberindex.data());
            glad_glBufferSubData(m_abo,0,sizeof(float) * numelements * 3,m_numberattributes.data());

        }

        m_indexlength = numelements * 3;

        return 0;
    }

    inline int FillFromLines(float *x, float * y, int * connect, int length, int width, bool with_attribute, float*a = nullptr, int*n = nullptr)
    {
        int n_connect = 0;
        bool has_connect = (connect != nullptr);

        if(has_connect)
        {
            for(int i = 0; i < length/2; i++)
            {
                if(connect[i] > 0)
                {
                    n_connect += 1;
                }
            }
        }

        int numelements = length/2;

        float dx_last = 0.0f;
        float dy_last = 0.0f;
        int connected = 0;
        float lcum = 0.0;
        bool last_connected = false;
        for(int i = 0; i < numelements-1; i +=1)
        {
            int i_index = i + connected;
            float dx = x[i*2] - x[i*2+1];
            float dy = y[i*2] - y[i*2+1];

            float l = sqrt(dx*dx + dy*dy);
            dx = dx/l;
            dy = dy/l;
            //if(relativesize)
            {
                width = 1.0;
            }
            if(l > 0.000001f)
            {
                float dxe = 0.5f * width *(-dy);
                float dye = 0.5f * width *(dx);

                m_vertexdata[i * 12 + connected * 6+ 0] =x[i*2] - dxe;
                m_vertexdata[i * 12 + connected * 6+ 1] =y[i*2] - dye;
                m_vertexdata[i * 12 + connected * 6+ 2] =lcum;
                m_vertexdata[i * 12 + connected * 6+ 3] =x[i*2];// + dxe;
                m_vertexdata[i * 12 + connected * 6+ 4] =y[i*2];// + dye;
                m_vertexdata[i * 12 + connected * 6+ 5] =lcum;
                m_vertexdata[i * 12 + connected * 6+ 6] =x[i*2+1] - dxe;
                m_vertexdata[i * 12 + connected * 6+ 7] =y[i*2+1] - dye;
                m_vertexdata[i * 12 + connected * 6+ 8] =lcum+l;
                m_vertexdata[i * 12 + connected * 6+ 9] =x[i*2+1] ;//+ dxe;
                m_vertexdata[i * 12 + connected * 6+ 10] =y[i*2+1];// + dye;
                m_vertexdata[i * 12 + connected * 6+ 11] =lcum+l;

                m_vertexorgdata[i * 12 + connected * 6+ 0] =x[i*2];
                m_vertexorgdata[i * 12 + connected * 6+ 1] =y[i*2];
                m_vertexorgdata[i * 12 + connected * 6+ 2] =0.0f;
                m_vertexorgdata[i * 12 + connected * 6+ 3] =x[i*2];// + dxe;
                m_vertexorgdata[i * 12 + connected * 6+ 4] =y[i*2];// + dye;
                m_vertexorgdata[i * 12 + connected * 6+ 5] =0.0f;
                m_vertexorgdata[i * 12 + connected * 6+ 6] =x[i*2+1];
                m_vertexorgdata[i * 12 + connected * 6+ 7] =y[i*2+1];
                m_vertexorgdata[i * 12 + connected * 6+ 8] =0.0f;
                m_vertexorgdata[i * 12 + connected * 6+ 9] =x[i*2+1];// + dxe;
                m_vertexorgdata[i * 12 + connected * 6+ 10] =y[i*2+1];// + dye;
                m_vertexorgdata[i * 12 + connected * 6+ 11] =0.0f;

                m_texcoorddata[i * 8 + connected * 4+ 0] = quad_texcords[0];
                m_texcoorddata[i * 8 + connected * 4+ 1] = quad_texcords[1];
                m_texcoorddata[i * 8 + connected * 4+ 2] = quad_texcords[2];
                m_texcoorddata[i * 8 + connected * 4+ 3] = quad_texcords[3];
                m_texcoorddata[i * 8 + connected * 4+ 4] = quad_texcords[4];
                m_texcoorddata[i * 8 + connected * 4+ 5] = quad_texcords[5];
                m_texcoorddata[i * 8 + connected * 4+ 6] = quad_texcords[6];
                m_texcoorddata[i * 8 + connected * 4+ 7] = quad_texcords[7];

                if(with_attribute && m_hasattribute)
                {
                    m_numberattributes[i * 4 + connected * 2+ 0] = a[i*2];
                    m_numberattributes[i * 4 + connected * 2+ 1] = a[i*2];
                    m_numberattributes[i * 4 + connected * 2+ 2] = a[i*2+1];
                    m_numberattributes[i * 4 + connected * 2+ 3] = a[i*2+1];
                    m_numberindex[i * 4 + connected * 2+ 0] = n[i*2];
                    m_numberindex[i * 4 + connected * 2+ 1] = n[i*2];
                    m_numberindex[i * 4 + connected * 2+ 2] = n[i*2+1];
                    m_numberindex[i * 4 + connected * 2+ 3] = n[i*2+1];
                }


                m_indices[i * 6 + connected * 3+ 0] = i * 4 + connected* 2+ 0;
                m_indices[i * 6 + connected * 3+ 1] = i * 4 + connected* 2+ 1;
                m_indices[i * 6 + connected * 3+ 2] = i * 4 + connected* 2+ 2;
                m_indices[i * 6 + connected * 3+ 3] = i * 4 + connected* 2+ 2;
                m_indices[i * 6 + connected * 3+ 4] = i * 4 + connected* 2+ 3;
                m_indices[i * 6 + connected * 3+ 5] = i * 4 + connected* 2+ 1;

                if(has_connect && i > 0 && connect[i] > 0)
                {
                    bool turnleft = (dx * dy_last - dy * dx_last) > 0.0f;
                    int i_temp = i+1;
                    m_vertexdata[i_temp * 12 + connected * 6+ 0] =x[(i-1)*2+2];
                    m_vertexdata[i_temp * 12 + connected * 6+ 1] =y[(i-1)*2+2];
                    m_vertexdata[i_temp * 12 + connected * 6+ 2] =lcum;
                    m_vertexdata[i_temp * 12 + connected * 6+ 3] =x[i*2+1];
                    m_vertexdata[i_temp * 12 + connected * 6+ 4] =y[i*2+1];
                    m_vertexdata[i_temp * 12 + connected * 6+ 5] =lcum;

                    m_vertexorgdata[i_temp * 12 + connected * 6+ 0] =x[(i-1)*2+1];
                    m_vertexorgdata[i_temp * 12 + connected * 6+ 1] =y[(i-1)*2+1];
                    m_vertexorgdata[i_temp * 12 + connected * 6+ 2] =0.0f;
                    m_vertexorgdata[i_temp * 12 + connected * 6+ 3] =x[i*2];
                    m_vertexorgdata[i_temp * 12 + connected * 6+ 4] =y[i*2];
                    m_vertexorgdata[i_temp * 12 + connected * 6+ 5] =0.0f;

                    m_texcoorddata[i_temp * 8 + connected * 4+ 0] = quad_texcords[2];
                    m_texcoorddata[i_temp * 8 + connected * 4+ 1] = quad_texcords[3];
                    m_texcoorddata[i_temp * 8 + connected * 4+ 2] = quad_texcords[2];
                    m_texcoorddata[i_temp * 8 + connected * 4+ 3] = quad_texcords[3];

                    if(with_attribute && m_hasattribute)
                    {
                        m_numberattributes[i_temp * 4 + connected * 2+ 0] = a[(i-1)*2+1];
                        m_numberattributes[i_temp * 4 + connected * 2+ 1] = a[i*2];
                        m_numberindex[i_temp * 4 + connected * 2+ 0] = n[(i-1)*2+1];
                        m_numberindex[i_temp * 4 + connected * 2+ 1] = n[i*2];
                    }

                    if(turnleft)
                    {
                        m_indices[i_temp * 6 + connected * 3+ 0] = i_temp * 4 + connected* 2+ 1;
                        m_indices[i_temp * 6 + connected * 3+ 1] = m_indices[i * 6 + connected * 3+ 0];
                        m_indices[i_temp * 6 + connected * 3+ 2] = m_indices[(i-1) * 6 + (connected-(last_connected?1:0)) * 3+ 0];
                    }else {
                        m_indices[i_temp * 6 + connected * 3+ 0] = i_temp * 4 + connected* 2+ 1;
                        m_indices[i_temp * 6 + connected * 3+ 1] = m_indices[i * 6 + connected * 3+ 1];
                        m_indices[i_temp * 6 + connected * 3+ 2] = m_indices[(i-1) * 6 + (connected-(last_connected?1:0)) * 3+ 1];
                    }
                    last_connected = true;
                    connected += 1;
                }
                last_connected = false;

            }
            dx_last = dx;
            dy_last = dy;
            lcum = lcum+l;
        }

        if(with_attribute && m_hasattribute)
        {
            glad_glBufferSubData(m_nbo,0,sizeof(int) * numelements * 4 + n_connect * 2,m_numberindex.data());
            glad_glBufferSubData(m_abo,0,sizeof(float) * numelements * 4 + n_connect * 2,m_numberattributes.data());

        }

        glad_glBufferSubData(m_vbo,0,sizeof(float) * numelements * 12 + n_connect * 6,m_vertexdata.data());
        glad_glBufferSubData(m_vboorg,0,sizeof(float) * numelements * 12 + n_connect * 6,m_vertexorgdata.data());


        m_indexlength = numelements * 6 + n_connect * 3;

        return 0;
    }



    inline int FillFromTrianglesWithOrigin(float * x, float *y, float * xr, float * yr,int length, bool with_attribute, float*a = nullptr, int*n = nullptr)
    {
        int numelements = length/3;

        for(int i = 0; i < numelements-1; i +=1)
        {
            m_vertexdata[i * 9 + 0] =x[i*3];
            m_vertexdata[i * 9 + 1] =y[i*3];
            m_vertexdata[i * 9 + 2] =0.0f;
            m_vertexdata[i * 9 + 3] =x[i*3+1];
            m_vertexdata[i * 9 + 4] =y[i*3+1];
            m_vertexdata[i * 9 + 5] =0.0f;
            m_vertexdata[i * 9 + 6] =x[i*3+2];
            m_vertexdata[i * 9 + 7] =y[i*3+2];
            m_vertexdata[i * 9 + 8] =0.0f;

            m_vertexorg2data[i * 9 + 0] =xr[i*3];
            m_vertexorg2data[i * 9 + 1] =yr[i*3];
            m_vertexorg2data[i * 9 + 2] =0.0f;
            m_vertexorg2data[i * 9 + 3] =xr[i*3+1];
            m_vertexorg2data[i * 9 + 4] =yr[i*3+1];
            m_vertexorg2data[i * 9 + 5] =0.0f;
            m_vertexorg2data[i * 9 + 6] =xr[i*3+2];
            m_vertexorg2data[i * 9 + 7] =yr[i*3+2];
            m_vertexorg2data[i * 9 + 8] =0.0f;

            m_texcoorddata[i * 6 + 0] = quad_texcords[0];
            m_texcoorddata[i * 6 + 1] = quad_texcords[1];
            m_texcoorddata[i * 6 + 2] = quad_texcords[2];
            m_texcoorddata[i * 6 + 3] = quad_texcords[3];
            m_texcoorddata[i * 6 + 4] = quad_texcords[4];
            m_texcoorddata[i * 6 + 5] = quad_texcords[5];

            if(with_attribute && m_hasattribute)
            {
                m_numberattributes[i * 3 + 0] = a[i * 3+ 0];
                m_numberattributes[i * 3 + 1] = a[i * 3+ 1];
                m_numberattributes[i * 3 + 2] = a[i * 3+ 2];
                m_numberindex[i * 3+ 0] = n[i * 3+ 0];
                m_numberindex[i * 3+ 1] = n[i * 3+ 1];
                m_numberindex[i * 3+ 2] = n[i * 3+ 2];
            }



            m_indices[i * 3 + 0] = i * 3 + 0;
            m_indices[i * 3 + 1] = i * 3 + 1;
            m_indices[i * 3 + 2] = i * 3 + 2;
        }

        glad_glBufferSubData(m_vbo,0,sizeof(float) * numelements * 9,m_vertexdata.data());
        glad_glBufferSubData(m_vboorg2,0,sizeof(float) * numelements * 9,m_vertexorg2data.data());

        if(with_attribute && m_hasattribute)
        {
            glad_glBufferSubData(m_nbo,0,sizeof(int) * numelements * 3,m_numberindex.data());
            glad_glBufferSubData(m_abo,0,sizeof(float) * numelements * 3,m_numberattributes.data());

        }

        m_indexlength = numelements * 3;

        return 0;
    }

    inline int FillFromLinesWithOrigin(float *x, float * y, int * connect,float *xr, float * yr, int length, int width, bool with_attribute, float*a = nullptr, int*n = nullptr)
    {
        int n_connect = 0;
        bool has_connect = (connect != nullptr);

        if(has_connect)
        {
            for(int i = 0; i < length/2; i++)
            {
                if(connect[i] > 0)
                {
                    n_connect += 1;
                }
            }
        }

        int numelements = length/2;

        float dx_last = 0.0f;
        float dy_last = 0.0f;
        int connected = 0;
        float lcum = 0.0;
        bool last_connected = false;
        for(int i = 0; i < numelements-1; i +=1)
        {
            int i_index = i + connected;
            float dx = x[i*2] - x[i*2+1];
            float dy = y[i*2] - y[i*2+1];

            float l = sqrt(dx*dx + dy*dy);
            dx = dx/l;
            dy = dy/l;
            //if(relativesize)
            {
                width = 1.0;
            }
            if(l > 0.000001f)
            {
                float dxe = 0.5f * width *(-dy);
                float dye = 0.5f * width *(dx);

                m_vertexdata[i * 12 + connected * 6+ 0] =x[i*2] - dxe;
                m_vertexdata[i * 12 + connected * 6+ 1] =y[i*2] - dye;
                m_vertexdata[i * 12 + connected * 6+ 2] =lcum;
                m_vertexdata[i * 12 + connected * 6+ 3] =x[i*2];// + dxe;
                m_vertexdata[i * 12 + connected * 6+ 4] =y[i*2];// + dye;
                m_vertexdata[i * 12 + connected * 6+ 5] =lcum;
                m_vertexdata[i * 12 + connected * 6+ 6] =x[i*2+1] - dxe;
                m_vertexdata[i * 12 + connected * 6+ 7] =y[i*2+1] - dye;
                m_vertexdata[i * 12 + connected * 6+ 8] =lcum+l;
                m_vertexdata[i * 12 + connected * 6+ 9] =x[i*2+1] ;//+ dxe;
                m_vertexdata[i * 12 + connected * 6+ 10] =y[i*2+1];// + dye;
                m_vertexdata[i * 12 + connected * 6+ 11] =lcum+l;

                m_vertexorgdata[i * 12 + connected * 6+ 0] =x[i*2];
                m_vertexorgdata[i * 12 + connected * 6+ 1] =y[i*2];
                m_vertexorgdata[i * 12 + connected * 6+ 2] =0.0f;
                m_vertexorgdata[i * 12 + connected * 6+ 3] =x[i*2];// + dxe;
                m_vertexorgdata[i * 12 + connected * 6+ 4] =y[i*2];// + dye;
                m_vertexorgdata[i * 12 + connected * 6+ 5] =0.0f;
                m_vertexorgdata[i * 12 + connected * 6+ 6] =x[i*2+1];
                m_vertexorgdata[i * 12 + connected * 6+ 7] =y[i*2+1];
                m_vertexorgdata[i * 12 + connected * 6+ 8] =0.0f;
                m_vertexorgdata[i * 12 + connected * 6+ 9] =x[i*2+1];// + dxe;
                m_vertexorgdata[i * 12 + connected * 6+ 10] =y[i*2+1];// + dye;
                m_vertexorgdata[i * 12 + connected * 6+ 11] =0.0f;

                m_vertexorg2data[i * 12 + connected * 6+ 0] =xr[i*2];
                m_vertexorg2data[i * 12 + connected * 6+ 1] =yr[i*2];
                m_vertexorg2data[i * 12 + connected * 6+ 2] =0.0f;
                m_vertexorg2data[i * 12 + connected * 6+ 3] =xr[i*2];// + dxe;
                m_vertexorg2data[i * 12 + connected * 6+ 4] =yr[i*2];// + dye;
                m_vertexorg2data[i * 12 + connected * 6+ 5] =0.0f;
                m_vertexorg2data[i * 12 + connected * 6+ 6] =xr[i*2+1];
                m_vertexorg2data[i * 12 + connected * 6+ 7] =yr[i*2+1];
                m_vertexorg2data[i * 12 + connected * 6+ 8] =0.0f;
                m_vertexorg2data[i * 12 + connected * 6+ 9] =xr[i*2+1];// + dxe;
                m_vertexorg2data[i * 12 + connected * 6+ 10] =yr[i*2+1];// + dye;
                m_vertexorg2data[i * 12 + connected * 6+ 11] =0.0f;

                m_texcoorddata[i * 8 + connected * 4+ 0] = quad_texcords[0];
                m_texcoorddata[i * 8 + connected * 4+ 1] = quad_texcords[1];
                m_texcoorddata[i * 8 + connected * 4+ 2] = quad_texcords[2];
                m_texcoorddata[i * 8 + connected * 4+ 3] = quad_texcords[3];
                m_texcoorddata[i * 8 + connected * 4+ 4] = quad_texcords[4];
                m_texcoorddata[i * 8 + connected * 4+ 5] = quad_texcords[5];
                m_texcoorddata[i * 8 + connected * 4+ 6] = quad_texcords[6];
                m_texcoorddata[i * 8 + connected * 4+ 7] = quad_texcords[7];

                if(with_attribute && m_hasattribute)
                {
                    m_numberattributes[i * 4 + connected * 2+ 0] = a[i*2];
                    m_numberattributes[i * 4 + connected * 2+ 1] = a[i*2];
                    m_numberattributes[i * 4 + connected * 2+ 2] = a[i*2+1];
                    m_numberattributes[i * 4 + connected * 2+ 3] = a[i*2+1];
                    m_numberindex[i * 4 + connected * 2+ 0] = n[i*2];
                    m_numberindex[i * 4 + connected * 2+ 1] = n[i*2];
                    m_numberindex[i * 4 + connected * 2+ 2] = n[i*2+1];
                    m_numberindex[i * 4 + connected * 2+ 3] = n[i*2+1];
                }



                m_indices[i * 6 + connected * 3+ 0] = i * 4 + connected* 2+ 0;
                m_indices[i * 6 + connected * 3+ 1] = i * 4 + connected* 2+ 1;
                m_indices[i * 6 + connected * 3+ 2] = i * 4 + connected* 2+ 2;
                m_indices[i * 6 + connected * 3+ 3] = i * 4 + connected* 2+ 2;
                m_indices[i * 6 + connected * 3+ 4] = i * 4 + connected* 2+ 3;
                m_indices[i * 6 + connected * 3+ 5] = i * 4 + connected* 2+ 1;

                if(has_connect && i > 0 && connect[i] > 0)
                {
                    bool turnleft = (dx * dy_last - dy * dx_last) > 0.0f;
                    int i_temp = i+1;
                    m_vertexdata[i_temp * 12 + connected * 6+ 0] =x[(i-1)*2+2];
                    m_vertexdata[i_temp * 12 + connected * 6+ 1] =y[(i-1)*2+2];
                    m_vertexdata[i_temp * 12 + connected * 6+ 2] =lcum;
                    m_vertexdata[i_temp * 12 + connected * 6+ 3] =x[i*2+1];
                    m_vertexdata[i_temp * 12 + connected * 6+ 4] =y[i*2+1];
                    m_vertexdata[i_temp * 12 + connected * 6+ 5] =lcum;

                    m_vertexorgdata[i_temp * 12 + connected * 6+ 0] =x[(i-1)*2+1];
                    m_vertexorgdata[i_temp * 12 + connected * 6+ 1] =y[(i-1)*2+1];
                    m_vertexorgdata[i_temp * 12 + connected * 6+ 2] =0.0f;
                    m_vertexorgdata[i_temp * 12 + connected * 6+ 3] =x[i*2];
                    m_vertexorgdata[i_temp * 12 + connected * 6+ 4] =y[i*2];
                    m_vertexorgdata[i_temp * 12 + connected * 6+ 5] =0.0f;

                    m_vertexorg2data[i_temp * 12 + connected * 6+ 0] =xr[(i-1)*2+1];
                    m_vertexorg2data[i_temp * 12 + connected * 6+ 1] =yr[(i-1)*2+1];
                    m_vertexorg2data[i_temp * 12 + connected * 6+ 2] =0.0f;
                    m_vertexorg2data[i_temp * 12 + connected * 6+ 3] =xr[i*2];
                    m_vertexorg2data[i_temp * 12 + connected * 6+ 4] =yr[i*2];
                    m_vertexorg2data[i_temp * 12 + connected * 6+ 5] =0.0f;

                    m_texcoorddata[i_temp * 8 + connected * 4+ 0] = quad_texcords[2];
                    m_texcoorddata[i_temp * 8 + connected * 4+ 1] = quad_texcords[3];
                    m_texcoorddata[i_temp * 8 + connected * 4+ 2] = quad_texcords[2];
                    m_texcoorddata[i_temp * 8 + connected * 4+ 3] = quad_texcords[3];

                    if(with_attribute && m_hasattribute)
                    {
                        m_numberattributes[i_temp * 4 + connected * 2+ 0] = a[(i-1)*2+1];
                        m_numberattributes[i_temp * 4 + connected * 2+ 1] = a[i*2];
                        m_numberindex[i_temp * 4 + connected * 2+ 0] = n[(i-1)*2+1];
                        m_numberindex[i_temp * 4 + connected * 2+ 1] = n[i*2];
                    }

                    if(turnleft)
                    {
                        m_indices[i_temp * 6 + connected * 3+ 0] = i_temp * 4 + connected* 2+ 1;
                        m_indices[i_temp * 6 + connected * 3+ 1] = m_indices[i * 6 + connected * 3+ 0];
                        m_indices[i_temp * 6 + connected * 3+ 2] = m_indices[(i-1) * 6 + (connected-(last_connected?1:0)) * 3+ 0];
                    }else {
                        m_indices[i_temp * 6 + connected * 3+ 0] = i_temp * 4 + connected* 2+ 1;
                        m_indices[i_temp * 6 + connected * 3+ 1] = m_indices[i * 6 + connected * 3+ 1];
                        m_indices[i_temp * 6 + connected * 3+ 2] = m_indices[(i-1) * 6 + (connected-(last_connected?1:0)) * 3+ 1];
                    }
                    last_connected = true;
                    connected += 1;
                }
                last_connected = false;

            }
            dx_last = dx;
            dy_last = dy;
            lcum = lcum+l;
        }


        glad_glBufferSubData(m_vbo,0,sizeof(float) * numelements * 12 + n_connect * 6,m_vertexdata.data());
        glad_glBufferSubData(m_vboorg,0,sizeof(float) * numelements * 12 + n_connect * 6,m_vertexorgdata.data());
        glad_glBufferSubData(m_vboorg2,0,sizeof(float) * numelements * 12 + n_connect * 6,m_vertexorg2data.data());

        if(with_attribute && m_hasattribute)
        {
            glad_glBufferSubData(m_nbo,0,sizeof(float) * numelements * 4 + n_connect * 2,m_numberindex.data());
            glad_glBufferSubData(m_abo,0,sizeof(float) * numelements * 4 + n_connect * 2,m_numberattributes.data());

        }

        m_indexlength = numelements * 6 + n_connect * 3;

        return 0;
    }

    inline int CreateFromData(float * vertices, int length_v, float * texcoords, int length_t, int * indices, int length_i)
    {




        return 0;

    }

    inline void Destroy()
    {

        if(m_vbo > 0)
        {
            glad_glDeleteBuffers(1,&m_vbo);
        }
        if(m_tbo > 0)
        {
            glad_glDeleteBuffers(1,&m_tbo);
        }
        if(m_ibo > 0)
        {
            glad_glDeleteBuffers(1,&m_ibo);
        }
        if(m_vboorg > 0)
        {
            glad_glDeleteBuffers(1,&m_vboorg);
        }
        if(m_vboorg2 > 0)
        {
            glad_glDeleteBuffers(1,&m_vboorg2);
        }
        if(m_vao > 0)
        {
            glad_glDeleteVertexArrays(1,&m_vao);
        }

    }


};

#endif // OPENGLGEOMETRY_H

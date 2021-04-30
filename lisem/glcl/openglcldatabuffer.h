#ifndef OPENGLCLDATABUFFER_H
#define OPENGLCLDATABUFFER_H

#ifdef OS_WIN
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif

#include <vector>

#include <glad/glad.h>
#include "OpenCLUtil.h"
#include "cl.hpp"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "OpenGLUtil.h"
#include "iostream"
#include <atomic>

class OpenGLCLBuffer
{
public:
    cl::Buffer m_buffercl;
    GLuint m_vao;
    GLuint m_vbo;
    cl::BufferGL m_buffergl;
    cl::size_t<3> m_dims;
    ::size_t m_size;
    bool m_clonly = false;
    OpenGLCLBuffer * m_second = 0;

    inline OpenGLCLBuffer()
    {

    }
};


template<typename A>
class OpenGLCLDataBuffer : public OpenGLCLBuffer
{
public:
    int m_Length = 0;
    int m_stride = 0;
    std::vector<A> m_Data;
    bool m_created = false;
    bool m_bound = false;


    std::atomic<int> m_DrawDim;

public:

    inline OpenGLCLDataBuffer()
    {
        m_Length = 0;
        m_stride = 0;
        m_DrawDim.store(0);

    }

    inline int GetDrawDim()
    {
        return std::min((int)(m_dims[0]),m_DrawDim.load());
    }

    inline void SetDrawDim(int x)
    {
        m_DrawDim.store(x);
    }


    inline int InitData(int length)
    {
        if(m_Length != 0)
        {
            m_Length = length;
            m_Data.resize(length,0.0);
            m_size = sizeof(A) * m_Length;
            if(m_stride > 0)
            {
                m_dims[0] = m_Length/m_stride;
                m_dims[1] = 1;
                m_dims[2] = 1;
            }
        }else
        {
            m_Length = length;
            m_Data = std::vector<A>(length);
            m_size = sizeof(A) * m_Length;

        }
        return 0;
    }

    inline int InitData(OpenGLCLDataBuffer<A> *copy)
    {
        m_Data = std::move(copy->m_Data);
        m_Length = copy->m_Length;
        m_size = copy->m_size;
        m_dims[0] = copy->m_dims[0];
        m_dims[1] = copy->m_dims[1];
        m_dims[2] = copy->m_dims[2];
        return 0;
    }


    inline int UpdateDataFromBuffer(cl::CommandQueue &q)
    {
        cl::Event ev;
        std::vector<cl::Memory> objs;
        objs.clear();
        objs.push_back(this->m_buffergl);
        cl_int res = q.enqueueAcquireGLObjects(&objs,NULL,&ev);
        ev.wait();
        res = q.enqueueReadBuffer(m_buffercl, CL_TRUE, 0, sizeof(A)*m_Length, m_Data.data());
        res = q.enqueueReleaseGLObjects(&objs);
        ev.wait();
        q.finish();
        return 0;
    }

    inline int UpdateBufferFromData(cl::CommandQueue &q)
    {
        cl::Event ev;
        std::vector<cl::Memory> objs;
        objs.clear();
        objs.push_back(this->m_buffergl);
        cl_int res = q.enqueueAcquireGLObjects(&objs,NULL,&ev);
                ev.wait();
        res = q.enqueueWriteBuffer(m_buffercl, CL_TRUE, 0, sizeof(A)*m_Length, m_Data.data());
        q.enqueueReleaseGLObjects(&objs);
        ev.wait();
        q.finish();
        return 0;
    }

    inline int UpdateBufferFromDataGL()
    {
        std::cout << "update buffer from GL " << m_vao << std::endl;
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
        glad_glBufferData(GL_ARRAY_BUFFER,m_Length*sizeof(int),m_Data.data(), GL_DYNAMIC_DRAW);
        glad_glBindBuffer(GL_ARRAY_BUFFER,0);
        return 0;
    }

    inline int CreateBufferObjectCL(cl::Context &c,cl::CommandQueue &q, int length, int stride = 2, cl_mem_flags mem_flags= CL_MEM_READ_WRITE, bool bind = true)
    {
        m_created = true;
        m_clonly = true;

        cl_int err = 0;
        m_buffercl = cl::Buffer(c, mem_flags, sizeof(A) * m_Length,m_Data.data(),&err);
        q.enqueueWriteBuffer(m_buffercl, CL_TRUE, 0, sizeof(A)*m_Length, m_Data.data());
        m_dims[0] = m_Length/stride;
        m_dims[1] = 1;
        m_dims[2] = 1;

        m_stride = stride;

        return 0;
    }



    inline int CreateBufferObject(cl::Context &c,cl::CommandQueue &q, int length, int stride = 2, GLenum datatype = GL_FLOAT, GLenum usage = GL_DYNAMIC_DRAW, cl_mem_flags mem_flags= CL_MEM_READ_WRITE, bool bind = true)
    {
        m_created = true;

        m_vbo = createBuffer(m_Length, m_Data.data(),usage);
        m_buffergl = cl::BufferGL(c, mem_flags, m_vbo, NULL);

        // bind vao
        if(bind)
        {
            m_bound = true;
            glad_glGenVertexArrays(1,&m_vao);
            glad_glBindVertexArray(m_vao);


            // attach vbo
            glad_glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
            glad_glVertexAttribPointer(0,stride,datatype,GL_FALSE,0,NULL);
            glad_glEnableVertexAttribArray(0);

            glad_glBindVertexArray(0);
        }

        // create opencl input and output buffer
        m_buffercl = cl::Buffer(c, CL_MEM_READ_WRITE, sizeof(A)*m_Length);
        q.enqueueWriteBuffer(m_buffercl, CL_TRUE, 0, sizeof(A)*m_Length, m_Data.data());
        m_dims[0] = m_Length/stride;
        m_dims[1] = 1;
        m_dims[2] = 1;

        m_stride = stride;

        return 0;
    }

    inline int CreateBufferObjectWithSecondVertexAttribute(cl::Context &c,cl::CommandQueue &q, int length, OpenGLCLDataBuffer<float> * in_second,int stride = 2, int stride2 = 2, GLenum datatype = GL_FLOAT,GLenum usage = GL_DYNAMIC_DRAW, cl_mem_flags mem_flags= CL_MEM_READ_WRITE)
    {
        m_created = true;
        m_vbo = createBuffer(m_Length, m_Data.data(),usage);
        m_buffergl = cl::BufferGL(c, mem_flags, m_vbo, NULL);

        m_second =in_second;

        m_bound = true;

        // bind vao
        glad_glGenVertexArrays(1,&m_vao);
        glad_glBindVertexArray(m_vao);
        // attach vbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
        glad_glVertexAttribPointer(0,2,datatype,GL_FALSE,0,NULL);
        glad_glEnableVertexAttribArray(0);


        // attach second vbo
        glad_glBindBuffer(GL_ARRAY_BUFFER,m_second->m_vbo);
        glad_glVertexAttribPointer(1,stride2,GL_FLOAT,GL_FALSE,0,NULL);
        glad_glEnableVertexAttribArray(1);

        glad_glBindVertexArray(0);

        // create opencl input and output buffer
        m_buffercl = cl::Buffer(c, CL_MEM_READ_WRITE, sizeof(A)*m_Length);
        q.enqueueWriteBuffer(m_buffercl, CL_TRUE, 0, sizeof(A)*m_Length, m_Data.data());
        m_dims[0] = m_Length/2;
        m_dims[1] = 1;
        m_dims[2] = 1;
        m_stride = stride;
        return 0;
    }

    inline int DestroyBufferObject()
    {
        if(m_created)
        {
            if(m_bound)
            {
                glad_glDeleteBuffers(1,&m_vbo);
                glad_glDeleteVertexArrays(1,&m_vao);
                m_created = false;

                if(m_second != NULL)
                {
                    glad_glDeleteBuffers(1,&(m_second->m_vbo));
                    m_second = NULL;
                }
            }
        }
        return 0;
    }

    inline int DestroyBufferData()
    {

        if(m_Data.size() > 0)
        {
            m_Data.clear();
        }

         return 0;
    }
};



#endif // OPENGLCLDATABUFFER_H

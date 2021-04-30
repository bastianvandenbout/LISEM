#ifndef OPENGLPROGRAM_H
#define OPENGLPROGRAM_H

#include "defines.h"
#ifdef OS_WIN
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif
#include <glad/glad.h>
#include "OpenCLUtil.h"
#include "cl.hpp"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "OpenGLUtil.h"

#include <QList>
#include <QString>
#include <iostream>

class LISEM_API OpenGLProgram
{
public:
    QString m_vshaderpath;
    QString m_fshaderpath;
    QString m_gshaderpath;
    QString m_tcshaderpath;
    QString m_teshaderpath;

    GLuint m_program = 0;

public:
    OpenGLProgram();

    inline int LoadProgram(QString vshaderpath, QString fshaderpath)
    {

        return LoadProgram(vshaderpath.toStdString().c_str(),fshaderpath.toStdString().c_str());

        return 0;
    }
    inline int LoadProgram(const char* vshaderpath, const char* fshaderpath)
    {
        m_vshaderpath = QString(vshaderpath);
        m_fshaderpath = QString(fshaderpath);

        m_program = initShaders(vshaderpath,fshaderpath);

        return 0;
    }

    inline int LoadProgramFromStrings(const char* vshader, const char* fshader)
    {
        m_program = initShadersFromStrings(vshader,fshader);

        return 0;
    }

    inline int LoadProgram(QString vshaderpath, QString fshaderpath,QString gshaderpath,QString tcshaderpath,QString teshaderpath)
    {

        return LoadProgram(vshaderpath.toStdString().c_str(),fshaderpath.toStdString().c_str(), gshaderpath.toStdString().c_str(),tcshaderpath.toStdString().c_str(),teshaderpath.toStdString().c_str());

    }
    inline int LoadProgram(const char* vshaderpath, const char* fshaderpath,const char* gshaderpath,const char* tcshaderpath,const char* teshaderpath)
    {

        m_vshaderpath = QString(vshaderpath);
        m_fshaderpath = QString(fshaderpath);
        m_gshaderpath = QString(gshaderpath);
        m_teshaderpath = QString(tcshaderpath);
        m_tcshaderpath = QString(teshaderpath);

        m_program = initShaders(vshaderpath,fshaderpath,gshaderpath,tcshaderpath,teshaderpath);

        return 0;
    }

    inline void Destroy()
    {

    }


};

#endif // OPENGLPROGRAM_H

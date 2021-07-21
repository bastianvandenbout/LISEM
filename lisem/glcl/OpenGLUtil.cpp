#include "OpenGLUtil.h"

#include <iostream>
#include <fstream>
#include "error.h"
#include "string"
#include "errno.h"

#include "lsmio.h"

char* loadFile(const char *fname, GLint &fSize)
{
    {

        QString s = GetFileString(fname);
        LISEM_DEBUG(s);
    }

    QString fnamec = QString(fname);
    fnamec.replace("/",QDir::separator());
    std::ifstream file (fnamec.toStdString(),std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        unsigned int size = (unsigned int)file.tellg();
        fSize = size;
        char *memblock = new char [size];
        file.seekg (0, std::ios::beg);
        file.read (memblock, size);
        file.close();
        LISEM_DEBUG(QString("file ") + fnamec + " loaded");
        return memblock;
    }

    LISEM_ERROR(QString("Unable to open file ") +fnamec+  "  "  + QString(strerror(errno)));
    throw 5;
    return NULL;
}

void printShaderInfoLog(GLint shader)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    if (infoLogLen > 1)
    {
        infoLog = new GLchar[infoLogLen];
        glGetShaderInfoLog(shader,infoLogLen, &charsWritten, infoLog);
        LISEM_ERROR(QString("Error log for compilation of shader"));
        LISEM_ERROR(QString(infoLog));
        std::cout << "InfoLog:" << std::endl << infoLog << std::endl;
        delete [] infoLog;
    }
}

void printLinkInfoLog(GLint prog)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);

    if (infoLogLen > 1)
    {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        glGetProgramInfoLog(prog,infoLogLen, &charsWritten, infoLog);
        std::cout << "InfoLog:" << std::endl << infoLog << std::endl;
        delete [] infoLog;
    }
}

shaders_t loadShaders(const char * vert_path, const char * frag_path) {
    GLuint f, v,g,tc,te;

    char *vs,*fs,gs,tcs,tes;

    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);
    // load shaders & get length of each
    GLint vlen;
    GLint flen;
    vs = loadFile(vert_path,vlen);
    fs = loadFile(frag_path,flen);

    const char * vv = vs;
    const char * ff = fs;

    glShaderSource(v, 1, &vv,&vlen);
    glShaderSource(f, 1, &ff,&flen);

    GLint compiled;

    glCompileShader(v);
    glGetShaderiv(v, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        LISEM_ERROR("Vertex shader not compiled.");

        printShaderInfoLog(v);
        throw 4;
    }

    glCompileShader(f);
    glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        LISEM_ERROR("Fragment shader not compiled.");
        printShaderInfoLog(f);

        throw 2;
    }
    shaders_t out; out.vertex = v; out.fragment = f;

    delete [] vs; // dont forget to free allocated memory
    delete [] fs; // we allocated this in the loadFile function...

    return out;
}


shaders_t loadShadersFromStrings(const char * vert_path, const char * frag_path) {
    GLuint f, v,g,tc,te;


    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);
    // load shaders & get length of each
    GLint vlen = strlen(vert_path);
    GLint flen = strlen(frag_path);

    QString qvs = QString(vert_path);
    QString qfs = QString(frag_path);

    glShaderSource(v, 1, &vert_path,&vlen);
    glShaderSource(f, 1, &frag_path,&flen);

    GLint compiled;

    glCompileShader(v);
    glGetShaderiv(v, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        LISEM_ERROR("Vertex shader not compiled.");

        printShaderInfoLog(v);
        throw 4;
    }

    glCompileShader(f);
    glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        LISEM_ERROR("Fragment shader not compiled.");
        printShaderInfoLog(f);

        throw 2;
    }
    shaders_t out; out.vertex = v; out.fragment = f;

    return out;
}


shaders_t loadShaders(const char * vert_path, const char * frag_path, const char * geom_path, const char * tc_path, const char * te_path) {
    GLuint f, v,g,tc,te;

    char *vs,*fs,*gs,*tcs,*tes;

    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);
    g = glCreateShader(GL_GEOMETRY_SHADER);
    tc = glCreateShader(GL_TESS_CONTROL_SHADER);
    te = glCreateShader(GL_TESS_EVALUATION_SHADER);

    // load shaders & get length of each
    GLint vlen;
    GLint flen;
    GLint glen;
    GLint tclen;
    GLint telen;
    vs = loadFile(vert_path,vlen);
    fs = loadFile(frag_path,flen);
    gs = loadFile(geom_path,glen);
    tcs = loadFile(tc_path,tclen);
    tes = loadFile(te_path,telen);

    const char * vv = vs;
    const char * ff = fs;
    const char * gg = gs;
    const char * tctc = tcs;
    const char * tete = tes;

    glShaderSource(v, 1, &vv,&vlen);
    glShaderSource(f, 1, &ff,&flen);
    glShaderSource(g, 1, &gg,&glen);
    glShaderSource(tc, 1, &tctc,&tclen);
    glShaderSource(te, 1, &tete,&telen);

    GLint compiled;

    glCompileShader(v);
    glGetShaderiv(v, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        LISEM_ERROR("Vertex shader not compiled.");

        printShaderInfoLog(v);
        throw 4;
    }

    glCompileShader(f);
    glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        LISEM_ERROR("Fragment shader not compiled.");
        printShaderInfoLog(f);

        throw 2;
    }

    glCompileShader(g);
    glGetShaderiv(g, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        LISEM_ERROR("Geometry shader not compiled.");
        printShaderInfoLog(g);

        throw 2;
    }

    glCompileShader(tc);
    glGetShaderiv(tc, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        LISEM_ERROR("Tesselation Control shader not compiled.");
        printShaderInfoLog(tc);

        throw 2;
    }

    glCompileShader(te);
    glGetShaderiv(te, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        LISEM_ERROR("Tesselation Evaulation shader not compiled.");
        printShaderInfoLog(te);

        throw 2;
    }
    shaders_t out; out.vertex = v; out.fragment = f; out.geometry = g; out.tc = tc; out.te = te;

    delete [] vs; // dont forget to free allocated memory
    delete [] fs; // we allocated this in the loadFile function...
    delete [] gs;
    delete [] tcs;
    delete [] tes;

    return out;
}
void attachAndLinkProgramtess( GLuint program, shaders_t shaders) {
    glAttachShader(program, shaders.vertex);
    glAttachShader(program, shaders.tc);
    glAttachShader(program, shaders.te);
    glAttachShader(program, shaders.geometry);
    glAttachShader(program, shaders.fragment);

    glLinkProgram(program);
    GLint linked;
    glGetProgramiv(program,GL_LINK_STATUS, &linked);
    if (!linked)
    {
        LISEM_ERROR("Program did not link.");

        printLinkInfoLog(program);
        throw 3;
    }
}

void attachAndLinkProgram( GLuint program, shaders_t shaders) {
    glAttachShader(program, shaders.vertex);
    glAttachShader(program, shaders.fragment);

    glLinkProgram(program);
    GLint linked;
    glGetProgramiv(program,GL_LINK_STATUS, &linked);
    if (!linked)
    {
        LISEM_ERROR("Program did not link.");

        printLinkInfoLog(program);
        throw 3;
    }
}

GLuint initShadersFromStrings(const char* vshaderpath, const char* fshaderpath)
{
    shaders_t shaders = loadShadersFromStrings(vshaderpath, fshaderpath);
    GLuint shader_program = glCreateProgram();
    attachAndLinkProgram(shader_program, shaders);
    return shader_program;
}

GLuint initShaders(const char* vshaderpath, const char* fshaderpath)
{
    shaders_t shaders = loadShaders(vshaderpath, fshaderpath);
    GLuint shader_program = glCreateProgram();
    attachAndLinkProgram(shader_program, shaders);
    return shader_program;
}

GLuint initShaders(const char* vshaderpath, const char* fshaderpath, const char* gshaderpath, const char* tcshaderpath, const char* teshaderpath)
{
    shaders_t shaders = loadShaders(vshaderpath, fshaderpath,gshaderpath,tcshaderpath,teshaderpath);

    GLuint shader_program = glCreateProgram();
    attachAndLinkProgramtess(shader_program, shaders);
    return shader_program;
}

GLuint createTexture2D(int width, int height, void* data,int component, int datatype, int format)
{
    GLuint ret_val = 0;
    glGenTextures(1,&ret_val);
    glBindTexture(GL_TEXTURE_2D,ret_val);
    glTexImage2D(GL_TEXTURE_2D,0,format,width,height,0,component,datatype,data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,0);
    return ret_val;
}

GLuint createTexture2DLinear(int width, int height, void* data,int component, int datatype, int format)
{
    GLuint ret_val = 0;
    glGenTextures(1,&ret_val);
    glBindTexture(GL_TEXTURE_2D,ret_val);
    glTexImage2D(GL_TEXTURE_2D,0,format,width,height,0,component,datatype,data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,0);
    return ret_val;
}

GLuint createBuffer(int size, const int* data, GLenum usage)
{
    GLuint ret_val = 0;
    glGenBuffers(1,&ret_val);
    glBindBuffer(GL_ARRAY_BUFFER,ret_val);
    glBufferData(GL_ARRAY_BUFFER,size*sizeof(int),data,usage);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    return ret_val;
}


GLuint createBuffer(int size, const float* data, GLenum usage)
{
    GLuint ret_val = 0;
    glGenBuffers(1,&ret_val);
    glBindBuffer(GL_ARRAY_BUFFER,ret_val);
    glBufferData(GL_ARRAY_BUFFER,size*sizeof(float),data,usage);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    return ret_val;
}

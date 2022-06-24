#ifndef __OPENGL_UTIL_H__
#define __OPENGL_UTIL_H__

#include "defines.h"
#include <glad/glad.h>

typedef struct {
    GLuint vertex = 0;
    GLuint fragment = 0;
    GLuint geometry = 0;
    GLuint tc = 0;
    GLuint te = 0;
} shaders_t;



LISEM_API char* loadFile(const char *fname, GLint &fSize);

// printShaderInfoLog
// From OpenGL Shading Language 3rd Edition, p215-216
// Display (hopefully) useful error messages if shader fails to compile
LISEM_API void printShaderInfoLog(GLint shader);

LISEM_API void printLinkInfoLog(GLint prog);

LISEM_API shaders_t loadShaderscfs(const char * vert_path, const char * frag_path);
LISEM_API shaders_t loadShaders(const char * vert_path, const char * frag_path);
LISEM_API shaders_t loadShadersFromStrings(const char * vert_path, const char * frag_path);

LISEM_API void attachAndLinkProgram( GLuint program, shaders_t shaders);

LISEM_API GLuint initShadersFromStrings(const char* vshaderpath, const char* fshaderpath);
LISEM_API GLuint initShaders(const char* vshaderpath, const char* fshaderpath);
LISEM_API GLuint initShaderscfs(const char* vshaderpath, const char* fshaderpath, std::vector<int> &el, std::vector<QString> &em);
LISEM_API GLuint initShaders(const char* vshaderpath, const char* fshaderpath,const char* gshaderpath,const char* tcshaderpath,const char* teshaderpath);

LISEM_API GLuint createTexture2D(int width, int height, void* data=0,int component = GL_RGBA, int datatype = GL_FLOAT, int format =GL_RGBA);
LISEM_API GLuint createTexture2DLinear(int width, int height, void* data=0,int component = GL_RGBA, int datatype = GL_FLOAT, int format =GL_RGBA);

LISEM_API GLuint createBuffer(int size, const float* data, GLenum usage);
LISEM_API GLuint createBuffer(int size, const int* data, GLenum usage);


#endif//__OPENGL_UTIL_H__

#pragma once

#ifdef OS_LNX
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX

#include "defines.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

LISEM_API extern void* lisem_glfwGetGLXContext(GLFWwindow*window);
LISEM_API extern void*lisem_glfwGetX11Display();

#endif



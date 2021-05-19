#include "lisem_glfw3native.h" 


#ifdef OS_LNX

void * lisem_glfwGetGLXContext(GLFWwindow*window)
{
	return glfwGetGLXContext(window);
}
void *lisem_glfwGetX11Display()
{
	return glfwGetX11Display();
}
#endif


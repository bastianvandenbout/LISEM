#include "opengl3dpainter.h"

#include "opengl3dobject.h"

void OpenGL3DPainter::LoadShaders()
{


    m_GLProgram_DrawObject = new OpenGLProgram();
    m_GLProgram_DrawObject->LoadProgram(m_AssetDir+ "UI3DObject.vert", m_AssetDir+ "UI3DObject.frag");




}

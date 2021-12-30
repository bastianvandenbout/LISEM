#include "opengl3dpainter.h"

#include "opengl3dobject.h"

void OpenGL3DPainter::LoadShaders()
{


    m_GLProgram_DrawObject = new OpenGLProgram();
    m_GLProgram_DrawObject->LoadProgram(m_ShaderDir+ "UI3DObject.vert", m_ShaderDir+ "UI3DObject.frag");

    m_GLProgram_DrawTexturedPolygon3D = new OpenGLProgram();
    m_GLProgram_DrawTexturedPolygon3D->LoadProgram(m_ShaderDir+ "UI3DPolyDraw.vert", m_ShaderDir+ "UI3DPolyDraw.frag");


    glad_glGenVertexArrays(1, &VAO);
    glad_glGenBuffers(1, &VBO);
    glad_glBindVertexArray(VAO);
    glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glad_glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glad_glEnableVertexAttribArray(0);
    glad_glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
    glad_glBindVertexArray(0);

    glad_glGenVertexArrays(1, &VAO3D);
    glad_glGenBuffers(1, &VBO3D);
    glad_glGenBuffers(1, &TBO3D);
    glad_glGenBuffers(1, &NBO3D);
    glad_glBindVertexArray(VAO3D);

    glad_glBindBuffer(GL_ARRAY_BUFFER, VBO3D);
    glad_glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 3, NULL, GL_DYNAMIC_DRAW);
    glad_glEnableVertexAttribArray(0);
    glad_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glad_glBindBuffer(GL_ARRAY_BUFFER, TBO3D);
    glad_glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 2, NULL, GL_DYNAMIC_DRAW);
    glad_glEnableVertexAttribArray(1);
    glad_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    glad_glBindBuffer(GL_ARRAY_BUFFER, NBO3D);
    glad_glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 3, NULL, GL_DYNAMIC_DRAW);
    glad_glEnableVertexAttribArray(2);
    glad_glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);


    glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
    glad_glBindVertexArray(0);


}



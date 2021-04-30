#include "openglshapepainter.h"


void OpenGLShapePainter::DrawPolygonGradient(float * x, float *y, int length,LSMColorGradient * grad, LSMVector2 dir)
{

    length = 3*(length / 3);
    if(!(length > 3))
    {
        return;
    }

    glad_glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTarget);

    glad_glDisable(GL_DEPTH_TEST);
    // bind shader
    glad_glUseProgram(m_GLProgram_DrawPolygonGradient->m_program);

    LSMMatrix4x4 orthom;
    orthom.SetOrtho(0,m_RenderTargetWidth,0.0,m_RenderTargetHeight,0,1.0);
    float *ortho = orthom.GetMatrixData();
    float imatrix[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};

    int mat_loc = glad_glGetUniformLocation(m_GLProgram_DrawPolygonGradient->m_program,"projection");
    glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,ortho);

    glad_glUniform1f(glad_glGetUniformLocation(m_GLProgram_DrawPolygonGradient->m_program, "alpha"), 1.0);
    glad_glUniform1i(glad_glGetUniformLocation(m_GLProgram_DrawPolygonGradient->m_program, "is_double"), 0);


    for(int i = 0; i <LISEM_GRADIENT_NCOLORS; i++)
    {
        QString is = QString::number(i);
        int colorstop_i_loc = glad_glGetUniformLocation(m_GLProgram_DrawPolygonGradient->m_program,QString("colorstop_"+ is).toStdString().c_str());
        int colorstop_ci_loc = glad_glGetUniformLocation(m_GLProgram_DrawPolygonGradient->m_program,QString("colorstop_c"+ is).toStdString().c_str());

        if(i < grad->m_Gradient_Stops.length())
        {
            glad_glUniform1f(colorstop_i_loc,grad->m_Gradient_Stops.at(i));
            ColorF c = grad->m_Gradient_Colors.at(i);
            glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
        }else {
            glad_glUniform1f(colorstop_i_loc,1e30f);
            glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
        }
    }


    glad_glActiveTexture(GL_TEXTURE0);
    glad_glBindVertexArray(VAO);

    float x_mindot = x[0];
    float y_mindot = y[0];
    float mindot = 1e30f;
    float maxdot = -1e30f;

    for(int i = 0; i < length; i++)
    {
        float dot = LSMVector2(x[i],y[i]).dot(dir);
        if(dot < mindot)
        {
            mindot = dot;
            x_mindot = x[i];
            y_mindot = y[i];
        }
        if(dot > maxdot)
        {
            maxdot = dot;
        }
    }

    for(int i = 0; i < length; i = i+6)
    {
        // Render glyph texture over quad
        glad_glBindTexture(GL_TEXTURE_2D, 0);

        if( i + 3 <  length)
        {
            // Update VBO for each character
            GLfloat vertices[6][4] = {
                { x[i],    y[i],        std::fabs(LSMVector2(x[i]-x_mindot,y[i]-y_mindot).dot(dir)/(maxdot-mindot)), 0.0 },
                { x[i+1],  y[i+1],      std::fabs(LSMVector2(x[i+1]-x_mindot,y[i+1]-y_mindot).dot(dir)/(maxdot-mindot)), 1.0 },
                { x[i+2],  y[i+2],      std::fabs(LSMVector2(x[i+2]-x_mindot,y[i+2]-y_mindot).dot(dir)/(maxdot-mindot)), 1.0 },

                { x[i+3],  y[i+3],      std::fabs(LSMVector2(x[i+3]-x_mindot,y[i+3]-y_mindot).dot(dir)/(maxdot-mindot)), 0.0 },
                { x[i+4],  y[i+4],      std::fabs(LSMVector2(x[i+4]-x_mindot,y[i+4]-y_mindot).dot(dir)/(maxdot-mindot)), 1.0 },
                { x[i+5],  y[i+5],      std::fabs(LSMVector2(x[i+5]-x_mindot,y[i+5]-y_mindot).dot(dir)/(maxdot-mindot)), 0.0 }
              };

           // Update content of VBO memory
           glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
           glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
           glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
        }else {
            // Update VBO for each character
            GLfloat vertices[6][4] = {
                { x[i],    y[i],        std::fabs(LSMVector2(x[i]-x_mindot,y[i]-y_mindot).dot(dir)/(maxdot-mindot)), 0.0 },
                { x[i+1],  y[i+1],      std::fabs(LSMVector2(x[i+1]-x_mindot,y[i+1]-y_mindot).dot(dir)/(maxdot-mindot)), 1.0 },
                { x[i+2],  y[i+2],      std::fabs(LSMVector2(x[i+2]-x_mindot,y[i+2]-y_mindot).dot(dir)/(maxdot-mindot)), 1.0 },

                { 0.0f,  0.0f,      0.0, 0.0 },
                { 0.0f,  0.0f,      1.0, 1.0 },
                { 0.0f,  0.0f,      1.0, 0.0 }
              };

           // Update content of VBO memory
           glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
           glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
           glad_glBindBuffer(GL_ARRAY_BUFFER, 0);

        }

        // Render quad
        if( i + 3 <  length)
        {
            glad_glDrawArrays(GL_TRIANGLES, 0, 6);
        }else {
            glad_glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    }
    glad_glBindVertexArray(0);
    glad_glBindTexture(GL_TEXTURE_2D, 0);

    delete[] ortho;
}

void OpenGLShapePainter::DrawPolygonDoubleGradient(float * x, float *y, int length,LSMColorGradient * grad,LSMColorGradient * grad2, LSMVector2 dir)
{

    length = 3*(length / 3);
    if(!(length > 3))
    {
        return;
    }

    glad_glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTarget);

    glad_glDisable(GL_DEPTH_TEST);
    // bind shader
    glad_glUseProgram(m_GLProgram_DrawPolygonGradient->m_program);

    LSMMatrix4x4 orthom;
    orthom.SetOrtho(0,m_RenderTargetWidth,0.0,m_RenderTargetHeight,0,1.0);
    float *ortho = orthom.GetMatrixData();
    float imatrix[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f};

    int mat_loc = glad_glGetUniformLocation(m_GLProgram_DrawPolygonGradient->m_program,"projection");
    glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,ortho);

    glad_glUniform1f(glad_glGetUniformLocation(m_GLProgram_DrawPolygonGradient->m_program, "alpha"), 1.0);
    glad_glUniform1i(glad_glGetUniformLocation(m_GLProgram_DrawPolygonGradient->m_program, "is_double"), 1);

    for(int i = 0; i <LISEM_GRADIENT_NCOLORS; i++)
    {
        QString is = QString::number(i);
        int colorstop_i_loc = glad_glGetUniformLocation(m_GLProgram_DrawPolygonGradient->m_program,QString("colorstop_"+ is).toStdString().c_str());
        int colorstop_ci_loc = glad_glGetUniformLocation(m_GLProgram_DrawPolygonGradient->m_program,QString("colorstop_c"+ is).toStdString().c_str());

        if(i < grad->m_Gradient_Stops.length())
        {
            glad_glUniform1f(colorstop_i_loc,grad->m_Gradient_Stops.at(i));
            ColorF c = grad->m_Gradient_Colors.at(i);
            glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
        }else {
            glad_glUniform1f(colorstop_i_loc,1e30f);
            glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
        }
    }

    for(int i = 0; i <LISEM_GRADIENT_NCOLORS; i++)
    {
        QString is = QString::number(i);
        int colorstop_i_loc = glad_glGetUniformLocation(m_GLProgram_DrawPolygonGradient->m_program,QString("colorstop2_"+ is).toStdString().c_str());
        int colorstop_ci_loc = glad_glGetUniformLocation(m_GLProgram_DrawPolygonGradient->m_program,QString("colorstop2_c"+ is).toStdString().c_str());

        if(i < grad2->m_Gradient_Stops.length())
        {
            glad_glUniform1f(colorstop_i_loc,grad2->m_Gradient_Stops.at(i));
            ColorF c = grad2->m_Gradient_Colors.at(i);
            glad_glUniform4f(colorstop_ci_loc,c.r,c.g,c.b,c.a);
        }else {
            glad_glUniform1f(colorstop_i_loc,1e30f);
            glad_glUniform4f(colorstop_ci_loc,1.0,1.0,1.0,1.0);
        }
    }


    glad_glActiveTexture(GL_TEXTURE0);
    glad_glBindVertexArray(VAO);

    float x_mindot = x[0];
    float y_mindot = y[0];
    float mindot = 1e30f;
    float maxdot = -1e30f;

    float x_mindot2 = x[0];
    float y_mindot2 = y[0];
    float mindot2 = 1e30f;
    float maxdot2 = -1e30f;

    LSMVector2 dirperp = LSMVector2(-dir.y,dir.x);
    for(int i = 0; i < length; i++)
    {
        float dot = LSMVector2(x[i],y[i]).dot(dir);
        if(dot < mindot)
        {
            mindot = dot;
            x_mindot = x[i];
            y_mindot = y[i];
        }
        if(dot > maxdot)
        {
            maxdot = dot;
        }
    }

    for(int i = 0; i < length; i++)
    {
        float dot2 = LSMVector2(x[i],y[i]).dot(dirperp);
        if(dot2 < mindot2)
        {
            mindot2 = dot2;
            x_mindot2 = x[i];
            y_mindot2 = y[i];
        }
        if(dot2 > maxdot2)
        {
            maxdot2 = dot2;
        }
    }

    for(int i = 0; i < length; i = i+6)
    {
        // Render glyph texture over quad
        glad_glBindTexture(GL_TEXTURE_2D, 0);

        if( i + 3 <  length)
        {
            // Update VBO for each character
            GLfloat vertices[6][4] = {
                { x[i],    y[i],        std::fabs(LSMVector2(x[i]-x_mindot,y[i]-y_mindot).dot(dir)/(maxdot-mindot)),std::fabs(LSMVector2(x[i]-x_mindot2,y[i]-y_mindot2).dot(dirperp)/(maxdot2-mindot2)) },
                { x[i+1],  y[i+1],      std::fabs(LSMVector2(x[i+1]-x_mindot,y[i+1]-y_mindot).dot(dir)/(maxdot-mindot)),std::fabs(LSMVector2(x[i+1]-x_mindot2,y[i+1]-y_mindot2).dot(dirperp)/(maxdot2-mindot2))},
                { x[i+2],  y[i+2],      std::fabs(LSMVector2(x[i+2]-x_mindot,y[i+2]-y_mindot).dot(dir)/(maxdot-mindot)),std::fabs(LSMVector2(x[i+2]-x_mindot2,y[i+2]-y_mindot2).dot(dirperp)/(maxdot2-mindot2)) },

                { x[i+3],  y[i+3],      std::fabs(LSMVector2(x[i+3]-x_mindot,y[i+3]-y_mindot).dot(dir)/(maxdot-mindot)),std::fabs(LSMVector2(x[i+3]-x_mindot2,y[i+3]-y_mindot2).dot(dirperp)/(maxdot2-mindot2)) },
                { x[i+4],  y[i+4],      std::fabs(LSMVector2(x[i+4]-x_mindot,y[i+4]-y_mindot).dot(dir)/(maxdot-mindot)),std::fabs(LSMVector2(x[i+4]-x_mindot2,y[i+4]-y_mindot2).dot(dirperp)/(maxdot2-mindot2)) },
                { x[i+5],  y[i+5],      std::fabs(LSMVector2(x[i+5]-x_mindot,y[i+5]-y_mindot).dot(dir)/(maxdot-mindot)),std::fabs(LSMVector2(x[i+5]-x_mindot2,y[i+5]-y_mindot2).dot(dirperp)/(maxdot2-mindot2)) }
              };

           // Update content of VBO memory
           glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
           glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
           glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
        }else {
            // Update VBO for each character
            GLfloat vertices[6][4] = {
                { x[i],    y[i],        std::fabs(LSMVector2(x[i]-x_mindot,y[i]-y_mindot).dot(dir)/(maxdot-mindot)), 0.0 },
                { x[i+1],  y[i+1],      std::fabs(LSMVector2(x[i+1]-x_mindot,y[i+1]-y_mindot).dot(dir)/(maxdot-mindot)), 1.0 },
                { x[i+2],  y[i+2],      std::fabs(LSMVector2(x[i+2]-x_mindot,y[i+2]-y_mindot).dot(dir)/(maxdot-mindot)), 1.0 },

                { 0.0f,  0.0f,      0.0, 0.0 },
                { 0.0f,  0.0f,      1.0, 1.0 },
                { 0.0f,  0.0f,      1.0, 0.0 }
              };

           // Update content of VBO memory
           glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
           glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
           glad_glBindBuffer(GL_ARRAY_BUFFER, 0);

        }

        // Render quad
        if( i + 3 <  length)
        {
            glad_glDrawArrays(GL_TRIANGLES, 0, 6);
        }else {
            glad_glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    }
    glad_glBindVertexArray(0);
    glad_glBindTexture(GL_TEXTURE_2D, 0);

    delete[] ortho;
}



void OpenGLShapePainter::DrawSquare(float xlt, float ylt, float width, float height,LSMVector4 color)
{
    float * x = new float[6];
    float * y = new float[6];

    x[0] = xlt;
    y[0] = ylt + height;
    x[1] = xlt;
    y[1] = ylt;
    x[2] = xlt + width;
    y[2] = ylt;

    x[3] = xlt + width;
    y[3] = ylt;
    x[4] = xlt + width;
    y[4] = ylt + height;
    x[5] = xlt;
    y[5] = ylt + height;

    DrawPolygon(x,y,6,color);

    delete[] x;
    delete[] y;
}

void OpenGLShapePainter::DrawSquareGradient(float xlt, float ylt, float width, float height, LSMColorGradient * grad, LSMVector2 dir)
{
    float * x = new float[6];
    float * y = new float[6];

    x[0] = xlt;
    y[0] = ylt + height;
    x[1] = xlt;
    y[1] = ylt;
    x[2] = xlt + width;
    y[2] = ylt;

    x[3] = xlt + width;
    y[3] = ylt;
    x[4] = xlt + width;
    y[4] = ylt + height;
    x[5] = xlt;
    y[5] = ylt + height;

    DrawPolygonGradient(x,y,6,grad,dir);

    delete[] x;
    delete[] y;

}

void OpenGLShapePainter::DrawSquareDoubleGradient(float xlt, float ylt, float width, float height, LSMColorGradient * grad,LSMColorGradient * grad2, LSMVector2 dir)
{
    float * x = new float[6];
    float * y = new float[6];

    x[0] = xlt;
    y[0] = ylt + height;
    x[1] = xlt;
    y[1] = ylt;
    x[2] = xlt + width;
    y[2] = ylt;

    x[3] = xlt + width;
    y[3] = ylt;
    x[4] = xlt + width;
    y[4] = ylt + height;
    x[5] = xlt;
    y[5] = ylt + height;

    DrawPolygonDoubleGradient(x,y,6,grad,grad2,dir);

    delete[] x;
    delete[] y;

}

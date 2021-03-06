#ifndef OPENGLTEXTPAINTER_H
#define OPENGLTEXTPAINTER_H
#ifdef OS_WIN
//#define GLFW_EXPOSE_NATIVE_WIN32
//#define GLFW_EXPOSE_NATIVE_WGL
#endif
#include <ft2build.h>
#include FT_FREETYPE_H
#include "defines.h"
#include <glad/glad.h>
#include "OpenCLUtil.h"
#include "cl.hpp"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <map>
#include "linear/lsm_vector2.h"
#include <QList>
#include <iostream>
#include <QApplication>
#include "error.h"
#include "openglprogram.h"
#include "linear/lsm_vector3.h"
#include "linear/lsm_vector4.h"
#include "linear/lsm_matrix4x4.h"
#include "site.h"
#include "font/glyph_foundry.hpp"

struct Character {
    GLuint     TextureID;  // ID handle of the glyph texture
    LSMVector2 Size;       // Size of glyph
    LSMVector2 Bearing;    // Offset from baseline to left/top of glyph
    GLuint     Advance;    // Offset to advance to next glyph
    float      size_org;
    int        buffer;
};

struct Font {

    QString FontName;
    QString FontSize;
    bool is_bold;
    bool is_italic;
    FT_Face * face = NULL;
    int size = 12;
    std::map<GLchar, Character> Glyphs_Rendered;


};


class LISEM_API OpenGLTextPainter
{
public:

    QList<Font*> m_Fonts;
    std::map<GLchar, Character> Glyphs_Rendered;

    FT_Library ft;

    Font*m_MainFont;

    QList<Font *> m_MainFonts;

    QString m_FontDir;
    QString m_ShaderDir;
    QString m_AssetDir;
    OpenGLProgram * m_GLProgram_DrawText;
    OpenGLProgram * m_GLProgram_DrawTexturedPolygon3D;

    GLuint VAO, VBO, VAO3D, VBO3D,TBO3D,NBO3D;

    inline OpenGLTextPainter()
    {
        //load the freetype font libary. We will use this to pre-render glyphs that we draw with opengl.
        //we have our own copy of the font for portability and customizability for users

        LISEM_DEBUG("Loading Freetype Font Library and Fonts");

        if (FT_Init_FreeType(&ft))
        {
            LISEM_ERROR("ERROR::FREETYPE: Could not init FreeType Library");
            throw 1;
        }

        m_FontDir = GetSite()+"/assets/";
        m_AssetDir = GetSite()+"/kernels/";
        m_ShaderDir = GetSite()+"/shaders/";


    }


    GLuint m_RenderTarget;
    int m_RenderTargetWidth;
    int m_RenderTargetHeight;

    inline void UpdateRenderTargetProperties(GLuint render_target, int width, int height)
    {
        m_RenderTarget = render_target;
        m_RenderTargetWidth = width;
        m_RenderTargetHeight = height;
    }



    QList<FT_Face> m_Faces;
    QList<Font *> m_Fonts_Rendered;

    inline void LoadDefaultFontsAndShaders()
    {
        m_MainFont = LoadFont(m_FontDir + "Roboto-Regular.ttf", 48);
        //Font * f_9 = LoadFont(m_FontDir + "Roboto-Regular.ttf", 9);
        //Font * f_12 = LoadFont(m_FontDir + "Roboto-Regular.ttf", 12);
        //Font * f_24 = LoadFont(m_FontDir + "Roboto-Regular.ttf", 24);
        m_Fonts.append(m_MainFont);

        //m_MainFonts.append(f_9);
        //m_MainFonts.append(f_12);
        //m_MainFonts.append(f_24);
        m_MainFonts.append(m_MainFont);

        m_GLProgram_DrawText = new OpenGLProgram();
        m_GLProgram_DrawText->LoadProgram(m_ShaderDir+ "UITextDraw.vert", m_ShaderDir+ "UITextDraw.frag");

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

    inline Font * LoadFont(QString file, int size = 12, bool bold = false, bool italic = false)
    {
        LISEM_DEBUG("Loading Font: " + file);

        Font * ret = new Font();

        ret->size= size;
        FT_Face face;
        if (FT_New_Face(ft, QString(file).toStdString().c_str(), 0, &face))
        {
            LISEM_ERROR("ERROR::FREETYPE: Failed to load font");
            throw 1;
        }

        std::map<GLchar, Character> Characters;

        glad_glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

        FT_Set_Pixel_Sizes(face, size, size);

        for (GLubyte c = 0; c < 128; c++)
        {
            int err = FT_Load_Char(face, c, FT_LOAD_RENDER);
            // Load character glyph
            if (err)
            {
                LISEM_ERROR("ERROR::FREETYTPE: Failed to load Glyph, code: " + QString::number(err));
                continue;
            }

            sdf_glyph_foundry::glyph_info glyph;

            glyph.glyph_index = FT_Get_Char_Index(face,c);
            int buffer = 8;
            sdf_glyph_foundry::RenderSDF(glyph, size, buffer, 0.25, face);



            // Generate texture
            GLuint texture;
            glad_glGenTextures(1, &texture);
            glad_glBindTexture(GL_TEXTURE_2D, texture);
           /*glad_glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );*/


            glad_glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                glyph.width + buffer * 2,
                glyph.height + buffer * 2,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                glyph.bitmap.c_str()
            );
            // Set texture options
            glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            //std::cout << "store character " <<c << " "<< face->glyph->bitmap.width << " "<< face->glyph->bitmap.rows << " "<<  face->glyph->advance.x << std::endl;
            // Now store character for later use
            Character character = {
                texture,
                LSMVector2(glyph.width+ buffer * 2, glyph.height+ buffer * 2),
                LSMVector2(glyph.left, glyph.top),
                static_cast<GLuint>(face->glyph->advance.x),
                size,
                buffer
            };

            ret->Glyphs_Rendered.insert(std::pair<GLchar, Character>(c, character));
        }

        return ret;
    }

    inline Font * GetFont(QString Name, bool bold, bool italic)
    {

        return nullptr;
    }

    inline void DrawStringShadow(QString in_text, Font * f, float pos_x, float pos_y, LSMVector4 color = LSMVector4(1.0,1.0,1.0,1.0), float size = 12, float size_incr = 2)
    {
        glad_glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTarget);


        Font * useFont = nullptr;
        if(f == nullptr)
        {
            /*float minsizedif = 1e31;
            bool found = false;
            for(int i = 0; i < m_MainFonts.size(); i++)
            {
                float sizedif = std::fabs((m_MainFonts.at(i)->size - size));
                if(sizedif < minsizedif)
                {
                    found = true;
                    useFont = m_MainFonts.at(i);
                    minsizedif = sizedif;
                }
            }*/

            //if(!found)
            {
                useFont = m_MainFont;
            }
        }else
        {
            useFont = f;
        }

        float x = pos_x;
        float y = pos_y;

        glad_glDisable(GL_DEPTH_TEST);
        // bind shader
        glad_glUseProgram(m_GLProgram_DrawText->m_program);

        LSMMatrix4x4 orthom;
        orthom.SetOrtho(0,m_RenderTargetWidth,0.0,m_RenderTargetHeight,0,1.0);
        float *ortho = orthom.GetMatrixData();
        float imatrix[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        int mat_loc = glad_glGetUniformLocation(m_GLProgram_DrawText->m_program,"projection");
        glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,ortho);

        glad_glUniform4f(glad_glGetUniformLocation(m_GLProgram_DrawText->m_program, "textColor"), color.x, color.y, color.z,color.w);
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glBindVertexArray(VAO);

        std::string text = in_text.toStdString();

        // Iterate through all characters
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = useFont->Glyphs_Rendered[*c];

            float scale = size/ch.size_org;

            GLfloat xpos = x + ch.Bearing.x * scale + ch.buffer + 0.25 * ch.Size.x * (size_incr/ch.size_org);
            GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale + 0.5 * ch.buffer + 0.25 * ch.Size.y * (size_incr/ch.size_org);

            GLfloat w = ch.Size.x * scale * (size_incr + size)/(size);
            GLfloat h = ch.Size.y * scale * (size_incr + size)/(size);

            // Update VBO for each character
            GLfloat vertices[6][4] = {
                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos,     ypos,       0.0, 1.0 },
                { xpos + w, ypos,       1.0, 1.0 },

                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos + w, ypos,       1.0, 1.0 },
                { xpos + w, ypos + h,   1.0, 0.0 }
            };

            // Render glyph texture over quad
            glad_glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // Update content of VBO memory
            glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
            // Render quad
            glad_glDrawArrays(GL_TRIANGLES, 0, 6);
            // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glad_glBindVertexArray(0);
        glad_glBindTexture(GL_TEXTURE_2D, 0);

        delete[] ortho;

    }
    inline void DrawString(QString in_text, Font * f, float pos_x, float pos_y, LSMVector4 color = LSMVector4(1.0,1.0,1.0,1.0), float size = 12)
    {
        glad_glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTarget);


        Font * useFont = nullptr;
        if(f == nullptr)
        {
            /*float minsizedif = 1e31;
            bool found = false;
            for(int i = 0; i < m_MainFonts.size(); i++)
            {
                float sizedif = std::fabs((m_MainFonts.at(i)->size - size));
                if(sizedif < minsizedif)
                {
                    found = true;
                    useFont = m_MainFonts.at(i);
                    minsizedif = sizedif;
                }
            }*/

            //if(!found)
            {
                useFont = m_MainFont;
            }
        }else
        {
            useFont = f;
        }

        float x = pos_x;
        float y = pos_y;

        glad_glDisable(GL_DEPTH_TEST);
        // bind shader
        glad_glUseProgram(m_GLProgram_DrawText->m_program);

        LSMMatrix4x4 orthom;
        orthom.SetOrtho(0,m_RenderTargetWidth,0.0,m_RenderTargetHeight,0,1.0);
        float *ortho = orthom.GetMatrixData();
        float imatrix[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        int mat_loc = glad_glGetUniformLocation(m_GLProgram_DrawText->m_program,"projection");
        glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,ortho);

        glad_glUniform4f(glad_glGetUniformLocation(m_GLProgram_DrawText->m_program, "textColor"), color.x, color.y, color.z,color.w);
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glBindVertexArray(VAO);

        std::string text = in_text.toStdString();

        // Iterate through all characters
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = useFont->Glyphs_Rendered[*c];

            float scale = size/ch.size_org;

            GLfloat xpos = x + ch.Bearing.x * scale + ch.buffer;
            GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale + 0.5 * ch.buffer;

            GLfloat w = ch.Size.x * scale;
            GLfloat h = ch.Size.y * scale;

            // Update VBO for each character
            GLfloat vertices[6][4] = {
                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos,     ypos,       0.0, 1.0 },
                { xpos + w, ypos,       1.0, 1.0 },

                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos + w, ypos,       1.0, 1.0 },
                { xpos + w, ypos + h,   1.0, 0.0 }
            };

            // Render glyph texture over quad
            glad_glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // Update content of VBO memory
            glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
            // Render quad
            glad_glDrawArrays(GL_TRIANGLES, 0, 6);
            // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glad_glBindVertexArray(0);
        glad_glBindTexture(GL_TEXTURE_2D, 0);

        delete[] ortho;
    }

    inline float DrawStringWidth(QString in_text, Font * f, float pos_x, float pos_y, LSMVector4 color = LSMVector4(1.0,1.0,1.0,1.0), float size = 12)
    {
        glad_glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTarget);


        Font * useFont = nullptr;
        if(f == nullptr)
        {
            /*float minsizedif = 1e31;
            bool found = false;
            for(int i = 0; i < m_MainFonts.size(); i++)
            {
                float sizedif = std::fabs((m_MainFonts.at(i)->size - size));
                if(sizedif < minsizedif)
                {
                    found = true;
                    useFont = m_MainFonts.at(i);
                    minsizedif = sizedif;
                }
            }*/

            //if(!found)
            {
                useFont = m_MainFont;
            }
        }else
        {
            useFont = f;
        }

        float x_orig = pos_x;
        float x = pos_x;
        float y = pos_y;

        //glad_glDisable(GL_DEPTH_TEST);
        // bind shader
        //glad_glUseProgram(m_GLProgram_DrawText->m_program);

        LSMMatrix4x4 orthom;
        orthom.SetOrtho(0,m_RenderTargetWidth,0.0,m_RenderTargetHeight,0,1.0);
        float *ortho = orthom.GetMatrixData();
        float imatrix[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};

        //int mat_loc = glad_glGetUniformLocation(m_GLProgram_DrawText->m_program,"projection");
        //glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,ortho);

        //glad_glUniform4f(glad_glGetUniformLocation(m_GLProgram_DrawText->m_program, "textColor"), color.x, color.y, color.z,color.w);
        //glad_glActiveTexture(GL_TEXTURE0);
        //glad_glBindVertexArray(VAO);

        std::string text = in_text.toStdString();

        // Iterate through all characters
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = useFont->Glyphs_Rendered[*c];

            float scale = size/ch.size_org;

            GLfloat xpos = x + ch.Bearing.x * scale + ch.buffer;
            GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale + 0.5 * ch.buffer;

            GLfloat w = ch.Size.x * scale;
            GLfloat h = ch.Size.y * scale;

            // Update VBO for each character
            GLfloat vertices[6][4] = {
                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos,     ypos,       0.0, 1.0 },
                { xpos + w, ypos,       1.0, 1.0 },

                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos + w, ypos,       1.0, 1.0 },
                { xpos + w, ypos + h,   1.0, 0.0 }
            };

            // Render glyph texture over quad
            //glad_glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // Update content of VBO memory
            //glad_glBindBuffer(GL_ARRAY_BUFFER, VBO);
            //glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            //glad_glBindBuffer(GL_ARRAY_BUFFER, 0);
            // Render quad
            //glad_glDrawArrays(GL_TRIANGLES, 0, 6);
            // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
        }
        //glad_glBindVertexArray(0);
        //glad_glBindTexture(GL_TEXTURE_2D, 0);

        delete[] ortho;

        //
        return x - x_orig;
    }


    inline void DrawText3D(QString in_text, Font * f,LSMVector3 topleft, LSMVector3 normal, LSMVector4 color, float size,LSMVector3 CameraPosition, LSMMatrix4x4 cameramatrix, bool test_depth)
    {

        glad_glDepthMask(GL_FALSE);
        if(test_depth)
        {
            glad_glEnable(GL_DEPTH_TEST);
        }else
        {
            glad_glDisable(GL_DEPTH_TEST);
        }

        //glad_glBindFramebuffer(GL_FRAMEBUFFER, m_RenderTarget);
        //glad_glEnable(GL_DEPTH_TEST);
        // bind shader
        glad_glUseProgram(m_GLProgram_DrawTexturedPolygon3D->m_program);
        glad_glActiveTexture(GL_TEXTURE0);
        glad_glBindVertexArray(VAO3D);


        LSMVector3 updir = LSMVector3(0.0,1.0,0.0);
        LSMVector3 rightdir = -LSMVector3::CrossProduct(normal,updir).Normalize();


        Font * useFont = nullptr;
        if(f == nullptr)
        {
            /*float minsizedif = 1e31;
            bool found = false;
            for(int i = 0; i < m_MainFonts.size(); i++)
            {
                float sizedif = std::fabs((m_MainFonts.at(i)->size - size));
                if(sizedif < minsizedif)
                {
                    found = true;
                    useFont = m_MainFonts.at(i);
                    minsizedif = sizedif;
                }
            }*/

            //if(!found)
            {
                useFont = m_MainFont;
            }
        }else
        {
            useFont = f;
        }

        float x = 0.0;
        float y = 0.0;

        std::string text = in_text.toStdString();

        // Iterate through all characters
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Character ch = useFont->Glyphs_Rendered[*c];

            float scale = size/ch.size_org;

            GLfloat xpos = x + ch.Bearing.x * scale + ch.buffer;
            GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale + 0.5 * ch.buffer;

            GLfloat w = ch.Size.x * scale;
            GLfloat h = ch.Size.y * scale;

            LSMVector3 pos = topleft + xpos * rightdir + ypos * updir;
            LSMVector3 pos1 = topleft + xpos * rightdir + (ypos+h) * updir;
            LSMVector3 pos2 = topleft + (xpos+w) * rightdir + ypos * updir;
            LSMVector3 pos3 = topleft + (xpos+w) * rightdir + (ypos+h) * updir;

            GLfloat vertices[6][3] = {
                { pos1.x,pos1.y,pos1.z},
                { pos.x,pos.y,pos.z},
                { pos2.x,pos2.y,pos2.z},

                { pos1.x,pos1.y,pos1.z},
                { pos2.x,pos2.y,pos2.z},
                { pos3.x,pos3.y,pos3.z},

            };

            GLfloat uv[6][2] = {

                { 0.0, 0.0},
                { 0.0, 1.0 },
                { 1.0, 1.0 },
                { 0.0, 0.0},
                { 1.0, 1.0 },
                { 1.0, 0.0 }

            };

            GLfloat normals[6][3] = {
                { normal.x,normal.y,normal.z},
                { normal.x,normal.y,normal.z},
                { normal.x,normal.y,normal.z},

                { normal.x,normal.y,normal.z},
                { normal.x,normal.y,normal.z},
                { normal.x,normal.y,normal.z},

            };



            // Update content of VBO memory
            glad_glBindBuffer(GL_ARRAY_BUFFER, VBO3D);
            glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glad_glBindBuffer(GL_ARRAY_BUFFER, 0);

            glad_glBindBuffer(GL_ARRAY_BUFFER, TBO3D);
            glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(uv), uv);
            glad_glBindBuffer(GL_ARRAY_BUFFER, 0);

            glad_glBindBuffer(GL_ARRAY_BUFFER, NBO3D);
            glad_glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(normals), normals);
            glad_glBindBuffer(GL_ARRAY_BUFFER, 0);


            int mat_loc = glad_glGetUniformLocation(m_GLProgram_DrawTexturedPolygon3D->m_program,"projection");
            glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,cameramatrix.GetMatrixDataPtr());

            glad_glUniform4f(glad_glGetUniformLocation(m_GLProgram_DrawTexturedPolygon3D->m_program, "inColor"), color.x, color.y, color.z, color.w);
            glad_glActiveTexture(GL_TEXTURE0);
            glad_glBindVertexArray(VAO3D);

            glad_glUniform1i(glad_glGetUniformLocation(m_GLProgram_DrawTexturedPolygon3D->m_program,"TextureIsText"),1);
            glad_glUniform1i(glad_glGetUniformLocation(m_GLProgram_DrawTexturedPolygon3D->m_program,"SampleTexture"),1);
            glad_glUniform1i(glad_glGetUniformLocation(m_GLProgram_DrawTexturedPolygon3D->m_program,"tex1"),0);
            glad_glActiveTexture(GL_TEXTURE0);
            glad_glBindTexture(GL_TEXTURE_2D,ch.TextureID);

            glad_glCullFace(GL_FRONT_AND_BACK);

            // Render quad
            glad_glDrawArrays(GL_TRIANGLES, 0, 6);

            // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glad_glBindVertexArray(0);
        glad_glBindTexture(GL_TEXTURE_2D, 0);

        glad_glEnable(GL_DEPTH_TEST);

        glad_glDepthMask(GL_TRUE);
    }


    inline void DrawText3DBillboard(QString text, LSMVector3 topleft, LSMVector4 Color, float size)
    {



    }

    inline void Destroy()
    {
        for(int i = 0; i < m_Fonts.length(); i++)
        {
            Font * f = m_Fonts.at(i);
            if(f != nullptr)
            {
                FT_Face * fa = (f->face);
                if(fa != nullptr)
                {
                    FT_Done_Face(*fa);
                }
                f->Glyphs_Rendered.clear();
                SAFE_DELETE(f);
            }
        }
        m_Fonts.clear();
        m_MainFont = nullptr;
        FT_Done_FreeType(ft);
    }


};

#endif // OPENGLTEXTPAINTER_H

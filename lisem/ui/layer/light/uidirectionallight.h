#pragma once


#include "styles/sphstyle.h"
#include "openglclmanager.h"
#include "glplot/worldgltransforms.h"
#include "layer/geowindowstate.h"
#include "matrixtable.h"
#include "QMutex"
#include "layer/uilayereditor.h"
#include "layer/layerparameters.h"
#include "geo/shapes/shapefile.h"
#include "boundingbox.h"
#include "linear/lsm_vector3.h"
#include "layer/uilight.h"


class UIDirectionalLight :public UILight
{

protected:

    std::vector<OpenGLCLMSAARenderTarget *> m_ShadowMaps;

    OpenGLProgram * m_Program = nullptr;

public:

    inline UIDirectionalLight() : UILight()
    {

        m_Active = true;
        m_Universal = true;
        m_RequiresShadowMaps = true;
        m_DoShadowMapping = true;
        m_Intensity = 1.0;
        m_Color = LSMVector3(1.0,0.8,0.3);


    }

    virtual inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {
        int size_x = m_ShadowMapSizeX;
        int size_y = m_ShadowMapSizeY;
        std::cout <<"prepare shadowmaps " <<s.m_2D3DRenderTargetScales.size() << std::endl;
        for(int i = 0; i < s.m_2D3DRenderTargetScales.size(); i++)
        {
            OpenGLCLMSAARenderTarget * ShadowMap = new OpenGLCLMSAARenderTarget();
            ShadowMap->CreateR32(size_x,size_y);
            size_x = size_x/2;
            size_y = size_y/2;
            m_ShadowMaps.push_back(ShadowMap);

        }


        m_Program = m->GetMGLProgram(KernelDir + "PostProcess.vert", KernelDir + "PostDirLight.frag");


        m_IsPrepared = true;

    }

    virtual inline bool IsPrepared() override
    {
        return m_IsPrepared;
    }

    virtual inline void OnDrawShadowMaps(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm, std::vector<UILayer *> shadowcasters) override
    {
        //if(m_DoShadowMapping)
        if(m_IsPrepared)
        {

            GL3DCamera * c = s.Camera3D;

            BoundingBox bb_prev;
            int size_x = m_ShadowMapSizeX;
            int size_y = m_ShadowMapSizeY;
            for(int i = 0; i < s.m_2D3DRenderTargetScales.size(); i++)
            {


                BoundingBox bb = s.m_2D3DRenderTargetBoundingBox.at(i);

                //get perspective/view matrix
                LSMMatrix4x4 mview;
                //looking from solar direction
                mview.SetLookAt(LSMVector3(0,s.Camera3D->GetPosition().y,0) + 1.0 *s.SunDir,LSMVector3(0,s.Camera3D->GetPosition().y,0),LSMVector3(0.0,1.0,0.0));
                LSMMatrix4x4 mortho;
                mortho.SetOrtho(-0.5 * bb.GetSizeX(),0.5 * bb.GetSizeX(),-0.5 * bb.GetSizeY(),0.5 * bb.GetSizeY(),-std::max(bb.GetSizeX(),bb.GetSizeY()) * 1.0,std::max(bb.GetSizeX(),bb.GetSizeY()) * 1.0);

                mortho = mortho * mview;

                m_ShadowMaps.at(i)->ClearAll0();

                for(int j = 0; j < shadowcasters.size(); j++)
                {
                    if(shadowcasters.at(j)->IsDraw() && shadowcasters.at(j)->IsShadowCaster())
                    {
                        shadowcasters.at(j)->OnDraw3DShadowDepth(m,s,tm,m_ShadowMaps.at(i),mortho,bb,i);
                    }
                }

                size_x = size_x/2;
                size_y = size_y/2;
            }
        }

    }


    const float pi = 3.14159265359;
    const float invPi = 1.0 / pi;

    const float multiScatterPhase = 0.1;
    const float density = 0.7;

    const float anisotropicIntensity = 0.0; //Higher numbers result in more anisotropic scattering

    const LSMVector3 skyColor = LSMVector3(0.39, 0.57, 1.0) * (1.0 + anisotropicIntensity); //Make sure one of the conponents is never 0.0

    #define smooth(x) x*x*(3.0-2.0*x)
    #define zenithDensity(x) density / std::pow(std::max(x - zenithOffset, 0.35e-2f), 0.75f)

   inline LSMVector3 getSkyAbsorption(LSMVector3 x, float y){

           LSMVector3 absorption = x * -y;
                 absorption = LSMVector3(std::exp2(absorption.x),std::exp2(absorption.y),std::exp2(absorption.z)) * 2.0;

            return absorption;
    }

    inline float getSunPoint(LSMVector3 p, LSMVector3 lp){
            return LSMVector3::smoothstep(0.03, 0.026, LSMVector3::VDistance(p, lp)) * 50.0;
    }

    inline float getRayleigMultiplier(LSMVector3 p, LSMVector3 lp){
            return 1.0 + pow(1.0 - clamp(LSMVector3::VDistance(p, lp), 0.0f, 1.0f), 2.0f) * pi * 0.5;
    }

    inline float getMie(LSMVector3 p, LSMVector3 lp){
            float disk = clamp(1.0 - pow(LSMVector3::VDistance(p, lp), 0.1), 0.0, 1.0);
            return disk*disk*(3.0 - 2.0 * disk) * 2.0 * pi;
    }

    inline LSMVector3 getAtmosphericScattering(LSMVector3 p, LSMVector3 lp){


            const float zenithOffset = 0.0;

            float zenith = zenithDensity(p.y);
            float sunPointDistMult =  clamp(std::fabs((std::max(lp.y + multiScatterPhase - zenithOffset, 0.0f))), 0.0f, 1.0f);

            float rayleighMult = getRayleigMultiplier(p, lp);

            LSMVector3 absorption = getSkyAbsorption(skyColor, zenith);
            LSMVector3 sunAbsorption = getSkyAbsorption(skyColor, zenithDensity(lp.y + multiScatterPhase));
            LSMVector3 sky = skyColor * zenith * rayleighMult;
            LSMVector3 sun = getSunPoint(p, lp) * absorption;
            LSMVector3 mie = getMie(p, lp) * sunAbsorption;

            LSMVector3 totalSky = LSMVector3::mix(LSMVector3(sky.x * absorption.x,sky.y * absorption.y,sky.z * absorption.z), sky / (sky + 0.5f), sunPointDistMult);
             totalSky = totalSky +  sun + mie;
                 totalSky.x *= sunAbsorption.x * 0.5 + 0.5 * (sunAbsorption.length());
                 totalSky.y *= sunAbsorption.y * 0.5 + 0.5 * (sunAbsorption.length());
                 totalSky.z *= sunAbsorption.z * 0.5 + 0.5 * (sunAbsorption.length());

            return totalSky;
    }

    inline LSMVector3 jodieReinhardTonemap(LSMVector3 c){
        float l =c.dot(LSMVector3(0.2126, 0.7152, 0.0722));
        LSMVector3 tc = c / (c + 1.0);

        return LSMVector3::mix(c / (l + 1.0), tc, tc);
    }


    inline LSMVector3 GetFinalSkyColor(LSMVector3 uv, LSMVector3 iSunDir)
    {

        LSMVector3 color = getAtmosphericScattering(uv,iSunDir.Normalize()) * pi;
        color = jodieReinhardTonemap(color);
        color.x = pow(color.x, 2.2f); //Back to linear
        color.y = pow(color.y, 2.2f); //Back to linear
        color.z = pow(color.z, 2.2f); //Back to linear

        return color;
    }

    inline LSMVector3 GetBaseColor(LSMVector3 iSunDir)
    {
        return GetFinalSkyColor(LSMVector3(0.0,-1.0,0.0),iSunDir);
    }

    inline LSMVector3 GetSunColor(LSMVector3 iSunDir)
    {
        return GetFinalSkyColor(iSunDir,iSunDir);
    }
    inline LSMVector3 GetSkyColor(LSMVector3 iSunDir)
    {
        //which direction to sample? try to take the bet of four quadrant upward directions
        std::vector<LSMVector3> dirs = {LSMVector3(1.0,1.0,1.0),LSMVector3(1.0,1.0,-1.0),LSMVector3(-1.0,1.0,1.0),LSMVector3(-1.0,1.0,-1.0)};
        std::vector<LSMVector3> colors;
        std::vector<float> dist;
        LSMVector3 color = LSMVector3(0.0,0.0,0.0);
        double min = 1e30;
        double i_best = 0;
        for(int i  = 0; i < dirs.size(); i++)
        {

            min =dirs[i].dot(LSMVector3(0.0,1.0,0.0));
            LSMVector3 colori = GetFinalSkyColor(dirs[i],iSunDir);
            colors.push_back(colori);
            dist.push_back(1.0-std::max(0.0,dirs[i].dot(LSMVector3(0.0,1.0,0.0))));

            color = color + (1.0-std::max(0.0,dirs[i].dot(LSMVector3(0.0,1.0,0.0)))) * colori;
            if(dirs[i].dot(LSMVector3(0.0,1.0,0.0)) < min)
            {
                i_best = i;
            }
        }

        return color;
    }


    virtual inline void OnRenderLightBuffer(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm, OpenGLCLMSAARenderTarget * target, int bufferindex) override
    {
        //the render target is a rgb32 texture that contains the lighted material color

        float matrix[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f};


        //if(m_DoShadowMapping)
        if(m_IsPrepared){


            BoundingBox bb_prev;
            int size_x = m_ShadowMapSizeX;
            int size_y = m_ShadowMapSizeY;
            LSMMatrix4x4 mortho;

            int i = 0;
            {
                BoundingBox bb = s.m_2D3DRenderTargetBoundingBox.at(i);

                //get perspective/view matrix
                LSMMatrix4x4 mview;
                //looking from solar direction
                mview.SetLookAt(LSMVector3(0,s.Camera3D->GetPosition().y,0)+1.0 *s.SunDir,LSMVector3(0,s.Camera3D->GetPosition().y,0),LSMVector3(0.0,1.0,0.0));

                mortho.SetOrtho(-0.5 * bb.GetSizeX(),0.5 * bb.GetSizeX(),-0.5 * bb.GetSizeY(),0.5 * bb.GetSizeY(),-std::max(bb.GetSizeX(),bb.GetSizeY()) * 1.0,std::max(bb.GetSizeX(),bb.GetSizeY()) * 1.0);


                mortho = mortho * mview;
            }
            LSMMatrix4x4 mortho1;

            i = 1;
            {
                BoundingBox bb = s.m_2D3DRenderTargetBoundingBox.at(i);

                //get perspective/view matrix
                LSMMatrix4x4 mview;
                //looking from solar direction
                mview.SetLookAt(LSMVector3(0,s.Camera3D->GetPosition().y,0)+1.0 *s.SunDir,LSMVector3(0,s.Camera3D->GetPosition().y,0),LSMVector3(0.0,1.0,0.0));

                mortho1.SetOrtho(-0.5 * bb.GetSizeX(),0.5 * bb.GetSizeX(),-0.5 * bb.GetSizeY(),0.5 * bb.GetSizeY(),-std::max(bb.GetSizeX(),bb.GetSizeY()) * 1.0,std::max(bb.GetSizeX(),bb.GetSizeY()) * 1.0);


                mortho1 = mortho1 * mview;
            }


            glad_glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target->GetFrameBuffer());
            GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0 + 6};
            glad_glDrawBuffers(1, DrawBuffers);

            glad_glViewport(0,0,target->GetWidth(),target->GetHeight());//m_OpenGLCLManager->GL_GLOBAL.Width,m_OpenGLCLManager->GL_GLOBAL.Height);

            glad_glDisable(GL_DEPTH_TEST);
            // bind shader
            glad_glUseProgram(m_Program->m_program);
            // get uniform locations


            glad_glUniformMatrix4fv(glad_glGetUniformLocation(m_Program->m_program,"CMatrix"),1,GL_FALSE,s.Camera3D->GetProjectionMatrix().GetMatrixDataPtr());
            glad_glUniformMatrix4fv(glad_glGetUniformLocation(m_Program->m_program,"CMatrixShadowMap0"),1,GL_FALSE,mortho.GetMatrixDataPtr());
            glad_glUniformMatrix4fv(glad_glGetUniformLocation(m_Program->m_program,"CMatrixShadowMap1"),1,GL_FALSE,mortho1.GetMatrixDataPtr());
            glad_glUniformMatrix4fv(glad_glGetUniformLocation(m_Program->m_program,"CMatrixNoTranslation"),1,GL_FALSE,s.Camera3D->GetProjectionMatrixNoTranslation().GetMatrixDataPtr());
            //glad_glUniformMatrix4fv(glad_glGetUniformLocation(m_Program->m_program,"CMatrixWorldToView"),1,GL_FALSE,s.Camera3D->);
            glad_glUniform3f(glad_glGetUniformLocation(m_Program->m_program,"iCameraPos"),s.GL_FrameBuffer3DWindow.at(0).GetCenterX(),s.Camera3D->GetPosition().y,s.GL_FrameBuffer3DWindow.at(0).GetCenterY());
            glad_glUniform3f(glad_glGetUniformLocation(m_Program->m_program,"iCameraDir"),s.Camera3D->GetViewDir().x,s.Camera3D->GetViewDir().y,s.Camera3D->GetViewDir().z);
            glad_glUniform3f(glad_glGetUniformLocation(m_Program->m_program,"iSolarDir"),s.SunDir.x,s.SunDir.y,s.SunDir.z);
            glad_glUniform1f(glad_glGetUniformLocation(m_Program->m_program,"iTime"),s.m_time);
            glad_glUniform2f(glad_glGetUniformLocation(m_Program->m_program,"iViewPortSize"),s.GL_PostProcessBuffer2->GetWidth(),s.GL_PostProcessBuffer2->GetHeight());
            glad_glUniform2f(glad_glGetUniformLocation(m_Program->m_program,"iResolution"),s.GL_PostProcessBuffer2->GetWidth(),s.GL_PostProcessBuffer2->GetHeight());


            glad_glUniform1i(glad_glGetUniformLocation(m_Program->m_program,"DrawLights"),s.draw_lighting);
            glad_glUniform1i(glad_glGetUniformLocation(m_Program->m_program,"DrawShadows"),s.draw_shadows?1:0);

            LSMVector3 s_color = GetSunColor(s.SunDir);
            LSMVector3 sky_color = GetSkyColor(s.SunDir);
            LSMVector3 base_color = GetBaseColor(s.SunDir);

            glad_glUniform3f(glad_glGetUniformLocation(m_Program->m_program,"iSolarColor"),s_color.x,s_color.y,s_color.z);
            glad_glUniform3f(glad_glGetUniformLocation(m_Program->m_program,"iSkyColor"),sky_color.x,sky_color.y,sky_color.z);
            glad_glUniform3f(glad_glGetUniformLocation(m_Program->m_program,"iBaseColor"),base_color.x,base_color.y,base_color.z);

            // bind texture
            glad_glActiveTexture(GL_TEXTURE0);
            glad_glUniform1i(glad_glGetUniformLocation(m_Program->m_program,"PosX"),0);
            glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(1));

            glad_glActiveTexture(GL_TEXTURE1);
            glad_glUniform1i(glad_glGetUniformLocation(m_Program->m_program,"PosY"),1);
            glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(2));

            glad_glActiveTexture(GL_TEXTURE2);
            glad_glUniform1i(glad_glGetUniformLocation(m_Program->m_program,"PosZ"),2);
            glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(3));

            glad_glActiveTexture(GL_TEXTURE5);
            glad_glUniform1i(glad_glGetUniformLocation(m_Program->m_program,"Normal"),5);
            glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(4));


            glad_glActiveTexture(GL_TEXTURE6);
            glad_glUniform1i(glad_glGetUniformLocation(m_Program->m_program,"Props"),6);
            glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(5));


            glad_glActiveTexture(GL_TEXTURE7);
            glad_glUniform1i(glad_glGetUniformLocation(m_Program->m_program,"Velocity"),7);
            glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(7));

            glad_glActiveTexture(GL_TEXTURE3);
            glad_glUniform1i(glad_glGetUniformLocation(m_Program->m_program,"ShadowMap0"),3);
            glad_glBindTexture(GL_TEXTURE_2D,m_ShadowMaps.at(0)->GetTexture(0));

            glad_glActiveTexture(GL_TEXTURE4);
            glad_glUniform1i(glad_glGetUniformLocation(m_Program->m_program,"ShadowMap1"),4);
            glad_glBindTexture(GL_TEXTURE_2D,m_ShadowMaps.at(1)->GetTexture(0));

            /*glad_glActiveTexture(GL_TEXTURE4);
            glad_glUniform1i(glad_glGetUniformLocation(m_Program->m_program,"Normal"),4);
            glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(4));

            glad_glActiveTexture(GL_TEXTURE5);
            glad_glUniform1i(glad_glGetUniformLocation(m_Program->m_program,"Props"),5);
            glad_glBindTexture(GL_TEXTURE_2D,s.GL_3DFrameBuffer->GetTexture(5));*/

            int mat_loc = glad_glGetUniformLocation(m_Program->m_program,"matrix");

            // set project matrix
            glad_glUniformMatrix4fv(mat_loc,1,GL_FALSE,matrix);

            // now render stuff
            glad_glBindVertexArray(m->m_Quad->m_vao);
            glad_glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
            glad_glBindVertexArray(0);

        }//else
        {



        }

    }

};

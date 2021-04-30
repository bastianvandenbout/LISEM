#ifndef UIRIGIDWORLDLAYER_H
#define UIRIGIDWORLDLAYER_H

#include "layer/geo/uigeolayer.h"
#include "rigidphysics/rigidworld.h"
#include "rigidphysics/rigidobject.h"
#include "openglgeometry.h"
#include "models/3dmodel.h"
#include "geometry/triangleintersect.h"
#include "opengl3dobject.h"
#include "gl/openglcldatamanager.h"
class UIRigidObjectVisualizer
{

public:

    RigidPhysicsObject * m_RigidObject = nullptr;
    ModelGeometry * m_Model = nullptr;
    gl3dObject * m_Actor = nullptr;

    QString m_Family;
    bool m_ShouldBeRemoved = false;

    LSMVector3 m_Position = LSMVector3(0.0,0.0,0.0);
    LSMVector3 m_Scale = LSMVector3(1.0,1.0,1.0);
    LSMMatrix4x4 m_RotationMatrix; //defeault is identity matrix

    inline UIRigidObjectVisualizer(RigidPhysicsObject * o)
    {
        m_RigidObject = o;
        QString fam = o->GetFamily();
        if(!fam.isEmpty())
        {
            m_Family = fam;
            m_Scale = m_RigidObject->GetFamilyScale();

        }

        m_Model = o->GetModel();
    }


};


class UIRigidWorldLayer : public UIGeoLayer
{

    RigidPhysicsWorld * m_RigidWorld = nullptr;
    QList<UIRigidObjectVisualizer*> m_Visualizers;
    QMap<QString,QList<UIRigidObjectVisualizer*>> m_Families;
    QMap<QString,ModelGeometry *> m_FamilyModels;
    QMap<QString,ModelGeometry *> m_FamilyGLModels;

public:

    inline UIRigidWorldLayer() : UIGeoLayer()
    {

    }


    inline UIRigidWorldLayer(RigidPhysicsWorld * world,QString name,QString file, bool is_model = false) : UIGeoLayer()
    {
        m_IsModel = is_model;
        Initialize(world,name,file,!is_model);

    }

    inline void Initialize(RigidPhysicsWorld * world, QString name, QString file, bool allow_edit = false)
    {
        m_Exists =true;
        m_Name = name;
        m_File = file;

        m_RigidWorld = world;

    }

    inline ~UIRigidWorldLayer()
    {

    }

    inline void SaveLayer(QJsonObject * obj) override
    {

    }

    inline void RestoreLayer(QJsonObject * obj)
    {
        QString path;
        QString name;

    }

    inline QString layertypeName()
    {
        return "RigidWorldLayer";
    }

    inline void OnDraw(OpenGLCLManager * m, GeoWindowState state) override
    {
        //first synchronize existing objects and their locations/rotations/velocities
        SyncAssets();


    }

    inline void SyncAssets()
    {

        //we have to keep track of the objects and locations within the simulation

        if(m_RigidWorld)
        {


            if(m_RigidWorld->HasChanges())
            {
                m_RigidWorld->LockMutex();

                QList<RigidPhysicsObject * > objects = m_RigidWorld->GetObjectList();

                for(int i = m_Visualizers.length()-1; i>-1; i--)
                {
                    UIRigidObjectVisualizer* vis = m_Visualizers.at(i);
                    if(vis->m_ShouldBeRemoved)
                    {
                        //remove from list
                        m_Visualizers.removeAt(i);

                        //check for families
                        if(!vis->m_Family.isEmpty())
                        {
                            if(!m_Families.contains(vis->m_Family))
                            {
                                m_Families[vis->m_Family].removeAll(vis);
                            }
                        }

                        //destroy resources






                    }
                }

                for(int i = 0; i < objects.length(); i++)
                {
                    if(!objects.at(i)->IsTerrain())
                    {
                        RigidPhysicsObject * obj = objects.at(i);
                        UIRigidObjectVisualizer* vis;
                        if(obj->GetCustomPtr() == nullptr)
                        {
                            //add new object
                            vis = new UIRigidObjectVisualizer(obj);
                            m_Visualizers.append(vis);

                            if(!vis->m_Family.isEmpty())
                            {
                                if(!m_Families.contains(vis->m_Family))
                                {

                                    m_Families[vis->m_Family] = QList<UIRigidObjectVisualizer*>();
                                }

                                m_Families[vis->m_Family].append(vis);
                            }

                            obj->SetCustomPtr(vis);
                            std::function<void(void)> f = [vis](){vis->m_ShouldBeRemoved = true;};
                            obj->SetRemoveCall(f);

                        }else
                        {
                            vis = (UIRigidObjectVisualizer*)(obj->GetCustomPtr());
                        }


                        //update object properties
                        vis->m_Position = obj->GetPosition();
                        vis->m_RotationMatrix = obj->GetRotationMatrix();


                    }
                }

                //remove any unused visualizers, and do cleanup of memory



                m_RigidWorld->UnLockMutex();

                m_RigidWorld->SetChanges(false);
            }


        }

    }

    inline void ClearAssets()
    {




    }


    inline void OnDraw3DGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {
        //actually draw 3d object representing rigid body physics world

        LSMVector2 offset = m_RigidWorld->GetSimpleGeoOrigin();

        QMap<QString,bool> familiesdone;
        QMap<QString, QList<UIRigidObjectVisualizer*>>::iterator it;
        QString output;
        for (it = m_Families.begin(); it != m_Families.end(); ++it) {
            // Format output here.
            familiesdone[it.key()] = false;
        }

        for(int i = 0; i < m_Visualizers.size(); i++)
        {
            UIRigidObjectVisualizer* vis = m_Visualizers.at(i);

            bool fam_found = false;
            //check if part of family
            if(!vis->m_Family.isEmpty())
            {

                if(m_Families.contains(vis->m_Family) )
                {
                    if((familiesdone[vis->m_Family] == false))
                    {
                        familiesdone[vis->m_Family] = true;

                        ModelGeometry* geom;
                        //check if this family has custom model
                        if(m_FamilyModels.contains(vis->m_Family))
                        {

                            //draw this model
                            geom = m_FamilyModels[vis->m_Family];
                        }else
                        {
                            //draw a simple model
                            geom = vis->m_Model->GetCopy();
                            m_FamilyModels[vis->m_Family] = geom;
                        }


                        //get all the different objects that are part of this family

                        const QList<UIRigidObjectVisualizer*> * objvislist = &(m_Families[vis->m_Family]);


                        LSMVector3 camerapos = s.Camera3D->GetPosition();

                        vis->m_Actor = new gl3dObject(m,geom,objvislist->size());

                        for(int j = 0; j < objvislist->size(); j++)
                        {
                            UIRigidObjectVisualizer* vis2 = objvislist->at(j);

                            LSMVector3 objpos = vis2->m_Position;
                            LSMMatrix4x4 objrotm = vis2->m_RotationMatrix;
                            LSMMatrix4x4 objtrans;
                            objtrans.Translate(LSMVector3(objpos.x + offset.x- camerapos.x,objpos.y,objpos.z + offset.y - camerapos.z));
                            objrotm = objtrans*objrotm;

                            vis->m_Actor->SetInstanceMatrix(j,objrotm);

                        }

                        vis->m_Actor->UpdateInstanceMatrices(m);

                        std::vector<gl3dMesh *> meshes = vis->m_Actor->GetMeshes();

                        for(int i = 0; i < meshes.size(); i++)
                        {
                            gl3dMesh * mesh = meshes.at(i);


                            s.GL_3DFrameBuffer->SetAsTarget();

                            glad_glDepthMask(GL_TRUE);
                            glad_glEnable(GL_DEPTH_TEST);

                            //set shader uniform values
                            OpenGLProgram * program = GLProgram_uiobjectinstanced;

                            // bind shader
                            glad_glUseProgram(program->m_program);

                            glad_glUniformMatrix4fv(glad_glGetUniformLocation(program->m_program,"CMatrix"),1,GL_FALSE,s.Camera3D->GetProjectionMatrixNoTranslationXZ().GetMatrixDataPtr());
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"SResolutionX"),s.scr_pixwidth);
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"SResolutionY"),s.scr_pixheight);
                            glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"iTime"),s.m_time);
                            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"isclouds"),0);
                            glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"iSunDir"),s.SunDir.x,s.SunDir.y,s.SunDir.z);

                            glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"CameraPosition"),s.GL_FrameBuffer3DWindow.at(0).GetCenterX(),s.Camera3D->GetPosition().y,s.GL_FrameBuffer3DWindow.at(0).GetCenterY());


                            /*glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iChannel0"),0);
                            glad_glActiveTexture(GL_TEXTURE0);
                            glad_glBindTexture(GL_TEXTURE_2D,m_TextureH->m_texgl);

                            glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iChannel1"),1);
                            glad_glActiveTexture(GL_TEXTURE1);
                            glad_glBindTexture(GL_TEXTURE_2D,m_TextureN->m_texgl);*/


                            // now render stuff
                            glad_glBindVertexArray(mesh->VAO);
                            glad_glDrawElementsInstanced(GL_TRIANGLES,mesh->indices.size(),GL_UNSIGNED_INT,0,objvislist->size());
                            glad_glBindVertexArray(0);

                        }

                    }


                    fam_found = true;

                }
            }


            if(!fam_found)
            {
                //draw the normal geometry

                if(vis->m_Model != nullptr)
                {
                    vis->m_Actor = new gl3dObject(m,vis->m_Model);

                    std::vector<gl3dMesh *> meshes = vis->m_Actor->GetMeshes();


                    for(int i = 0; i < meshes.size(); i++)
                    {

                        gl3dMesh * mesh = meshes.at(i);

                        LSMVector3 camerapos = s.Camera3D->GetPosition();
                        LSMVector3 objpos = vis->m_Position;
                        LSMMatrix4x4 objrotm = vis->m_RotationMatrix;


                        LSMMatrix4x4 objtrans;
                        objtrans.Translate(LSMVector3(objpos.x + offset.x- camerapos.x,objpos.y,objpos.z + offset.y - camerapos.z));
                        objrotm = objtrans*objrotm;

                        s.GL_3DFrameBuffer->SetAsTarget();

                        glad_glDepthMask(GL_TRUE);
                        glad_glEnable(GL_DEPTH_TEST);

                        //set shader uniform values
                        OpenGLProgram * program = GLProgram_uiobject;


                        // bind shader
                        glad_glUseProgram(program->m_program);

                        glad_glUniformMatrix4fv(glad_glGetUniformLocation(program->m_program,"OMatrix"),1,GL_FALSE,(float*)(&objrotm));//.GetMatrixDataPtr());

                        glad_glUniformMatrix4fv(glad_glGetUniformLocation(program->m_program,"CMatrix"),1,GL_FALSE,s.Camera3D->GetProjectionMatrixNoTranslationXZ().GetMatrixDataPtr());
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"SResolutionX"),s.scr_pixwidth);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"SResolutionY"),s.scr_pixheight);
                        glad_glUniform1f(glad_glGetUniformLocation(program->m_program,"iTime"),s.m_time);
                        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"isclouds"),0);
                        glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"iSunDir"),s.SunDir.x,s.SunDir.y,s.SunDir.z);
                        glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"ObjTranslate"),objpos.x + offset.x- camerapos.x,objpos.y,objpos.z + offset.y - camerapos.z);

                        glad_glUniform3f(glad_glGetUniformLocation(program->m_program,"CameraPosition"),s.GL_FrameBuffer3DWindow.at(0).GetCenterX(),s.Camera3D->GetPosition().y,s.GL_FrameBuffer3DWindow.at(0).GetCenterY());


                        /*glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iChannel0"),0);
                        glad_glActiveTexture(GL_TEXTURE0);
                        glad_glBindTexture(GL_TEXTURE_2D,m_TextureH->m_texgl);

                        glad_glUniform1i(glad_glGetUniformLocation(program->m_program,"iChannel1"),1);
                        glad_glActiveTexture(GL_TEXTURE1);
                        glad_glBindTexture(GL_TEXTURE_2D,m_TextureN->m_texgl);*/


                        // now render stuff
                        glad_glBindVertexArray(mesh->VAO);
                        glad_glDrawElements(GL_TRIANGLES,mesh->indices.size(),GL_UNSIGNED_INT,0);
                        glad_glBindVertexArray(0);


                        //debug display of submerged triangles and forces

                        /*BoundingBox3D extent = vis->m_RigidObject->GetAABB();
                         double csx = this->m_RigidWorld->m_DEM->cellSizeX();
                         double csy = this->m_RigidWorld->m_DEM->cellSizeY();
                         int rmin = extent.GetMinZ()/csy;
                         int cmin = extent.GetMinX()/csx;
                         int rmax = extent.GetMaxZ()/csy;
                         int cmax = extent.GetMaxX()/csx;

                         if(rmin > rmax)
                         {
                             int temp = rmax;
                             rmax = rmin;
                             rmin = temp;
                         }
                         if(cmin > cmax)
                         {
                             int temp = cmax;
                             cmax = cmin;
                             cmin = temp;
                         }
                         rmin = std::max(rmin-1,0);
                         cmin = std::max(cmin-1,0);
                         rmax = std::min(rmax+1,this->m_RigidWorld->m_DEM->nrRows()-1);
                         cmax = std::min(cmax+1,this->m_RigidWorld->m_DEM->nrCols()-1);

                         double area = 0.0;
                         double vol_total = 0.0;
                         LSMVector3 com_total;
                         LSMVector3 FFricTotal;

                         for(int r = rmin; r < rmax+1; r++)
                         {
                             for(int c = cmin; c < cmax+1; c++)
                             {

                                 //get top elevation of flow surface
                                 float h = m_RigidWorld->m_DEM->data[r][c];
                                 float hf = ((m_RigidWorld->m_HF == nullptr)? 0.0:m_RigidWorld->m_HF->data[r][c]);
                                 float hs = ((m_RigidWorld->m_HS == nullptr)? 0.0:m_RigidWorld->m_HS->data[r][c]);
                                 float dhdx = cTMap::DER_X(m_RigidWorld->m_DEM,r,c)/csx;
                                 float dhdy = cTMap::DER_Y(m_RigidWorld->m_DEM,r,c)/csy;
                                 float dhfdx = cTMap::DER_X(m_RigidWorld->m_HF,r,c)/csx;
                                 float dhfdy = cTMap::DER_Y(m_RigidWorld->m_HF,r,c)/csy;
                                 float dhsdx = cTMap::DER_X(m_RigidWorld->m_HS,r,c)/csx;
                                 float dhsdy = cTMap::DER_Y(m_RigidWorld->m_HS,r,c)/csy;
                                 float surfacetop = h+ hf +hs
                                         + 0.5 * std::fabs(csy) * dhdy
                                         +  0.5 * std::fabs(csx) * dhdx
                                         + 0.5 * std::fabs(csy) * dhsdy
                                         +  0.5 * std::fabs(csx) * dhsdx
                                         + 0.5 * std::fabs(csy) * dhfdy
                                         +  0.5 * std::fabs(csx) * dhfdx;

                                int triangleCount = mesh->indices.size()/3;
                                for (unsigned long i = 0; i <  triangleCount; i++)
                                {
                                    LSMVector3 relpos_A = mesh->vertices[mesh->indices[i * 3 + 0]].position();
                                    LSMVector3 relpos_B = mesh->vertices[mesh->indices[i * 3 + 1]].position();
                                    LSMVector3 relpos_C = mesh->vertices[mesh->indices[i * 3 + 2]].position();

                                    LSMMatrix4x4 objrotm2 = vis->m_RotationMatrix;

                                    //we have to convert this to global world space
                                    LSMVector3 Atmp = objrotm2 *relpos_A + objpos;
                                    LSMVector3 Btmp = objrotm2 *relpos_B + objpos;
                                    LSMVector3 Ctmp = objrotm2 *relpos_C + objpos;

                                    //get velocity and angular velocity
                                    ChVector<double> rot = vis->m_RigidObject->m_chBody->GetWvel_par();
                                    ChVector<double> vel = vis->m_RigidObject->m_chBody->GetPos_dt();

                                    LSMVector3 axis = LSMVector3(rot[0],rot[1],rot[2]);
                                    LSMVector3 velo = LSMVector3(vel[0],vel[1],vel[2]);

                                    LSMVector3 FLoc = (Atmp + Btmp + Ctmp)*(1.0/3.0);

                                    LSMVector3 vel_FLoc = velo + LSMVector3::CrossProduct(axis,FLoc - objpos);//*DistancePointLine(mesh->vertices[mesh->indices[i * 3 + 2]].position(),LSMVector3(0.0,0.0,0.0),axis);
                                    LSMVector3 vel_FLoc_NoRot = velo;//*DistancePointLine(mesh->vertices[mesh->indices[i * 3 + 2]].position(),LSMVector3(0.0,0.0,0.0),axis);


                                    LSMVector3 Normal = LSMVector3::CrossProduct(Btmp-Atmp,Ctmp-Atmp).Normalize();


                                    LSMVector3 vel_difav = vel_FLoc * 1.0+LSMVector3(1.0,0.0,1.0)*-1.0;
                                    LSMVector3 vel_tangent = LSMVector3::CrossProduct(Normal,LSMVector3::CrossProduct(vel_difav,Normal)).Normalize() * (-vel_difav.length());//

                                    double vol;
                                    double area_proj;
                                    double proj_area_vel;
                                    LSMVector3 com;
                                    LSMVector3 hydro_cof;
                                    double hydro_total;
                                    BoundingBox extenthor((float)(c) * csx, (float)(c+1) * csx, (float)(r) * csy, (float)(r+1) * csy) ;
                                    std::vector<LSMVector3> points;
                                    double area_this =TriangleWaterFlowIntersect(Btmp,Atmp,Ctmp,extenthor,hf,dhfdx,dhfdy,h,dhdx,dhdy,vol,area_proj,com,LSMVector3(10.0,0.0,10.0),proj_area_vel, hydro_cof,hydro_total,points);//);

                                    LSMVector3 DrawCenter = FLoc  + LSMVector3(offset.x,0.0,offset.y) - LSMVector3(camerapos.x,0.0,camerapos.z);


                                    if(area_this > 0.0001 )
                                    {
                                        LSMVector3 F = vel_tangent * 1000.0 * vel_tangent.length() * area_this * 1.0/250.0;
                                        m->m_ShapePainter->DrawLine3D(DrawCenter,DrawCenter + F ,LSMVector3(0.0,camerapos.y,0.0),0.1,LSMVector4(0.0,0.0,1.0,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false);

                                        m->m_ShapePainter->DrawLine3D(DrawCenter,DrawCenter + Normal * 3.0,LSMVector3(0.0,camerapos.y,0.0),0.1,LSMVector4(0.0,1.0,0.0,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false);
                                        m->m_ShapePainter->DrawLine3D(DrawCenter,DrawCenter + vel_FLoc ,LSMVector3(0.0,camerapos.y,0.0),0.1,LSMVector4(1.0,0.0,0.0,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false);

                                    }

                                    ChVector<double> rot3 = vis->m_RigidObject->m_chBody->GetWvel_par() * 10.0;
                                    m->m_ShapePainter->DrawLine3D(objpos + LSMVector3(offset.x,0.0,offset.y) - LSMVector3(camerapos.x,0.0,camerapos.z),objpos + LSMVector3(rot3.x(),rot3.y(),rot3.z()) + LSMVector3(offset.x,0.0,offset.y) - LSMVector3(camerapos.x,0.0,camerapos.z),LSMVector3(0.0,camerapos.y,0.0),0.05,LSMVector4(1.0,1.0,0.0,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false);
                                    ChVector<double> rot4 = vis->m_RigidObject->m_chBody->Dir_Body2World(vis->m_RigidObject->m_chBody->Get_accumulated_torque()) /250.0;
                                    m->m_ShapePainter->DrawLine3D(objpos + LSMVector3(offset.x,0.0,offset.y) - LSMVector3(camerapos.x,0.0,camerapos.z),objpos + LSMVector3(rot4.x(),rot4.y(),rot4.z()) + LSMVector3(offset.x,0.0,offset.y) - LSMVector3(camerapos.x,0.0,camerapos.z),LSMVector3(0.0,camerapos.y,0.0),0.05,LSMVector4(0.0,1.0,1.0,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false);



                                    if(points.size() > 0)
                                    {
                                        LSMVector3 pref = points[0];
                                        for(int l = 0; l < points.size()-1; l++)
                                        {
                                            LSMVector3 v0 = pref;
                                            LSMVector3 v1 = points.at(l);
                                            LSMVector3 v2 = points.at(l+1);

                                            m->m_ShapePainter->DrawLine3D(v0 + LSMVector3(offset.x,0.0,offset.y) - LSMVector3(camerapos.x,0.0,camerapos.z),v1 + LSMVector3(offset.x,0.0,offset.y) - LSMVector3(camerapos.x,0.0,camerapos.z),LSMVector3(0.0,camerapos.y,0.0),0.05,LSMVector4(1.0,1.0,1.0,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false);
                                            m->m_ShapePainter->DrawLine3D(v1 + LSMVector3(offset.x,0.0,offset.y) - LSMVector3(camerapos.x,0.0,camerapos.z),v2 + LSMVector3(offset.x,0.0,offset.y) - LSMVector3(camerapos.x,0.0,camerapos.z),LSMVector3(0.0,camerapos.y,0.0),0.05,LSMVector4(1.0,1.0,1.0,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false);
                                            m->m_ShapePainter->DrawLine3D(v2 + LSMVector3(offset.x,0.0,offset.y) - LSMVector3(camerapos.x,0.0,camerapos.z),v0 + LSMVector3(offset.x,0.0,offset.y) - LSMVector3(camerapos.x,0.0,camerapos.z),LSMVector3(0.0,camerapos.y,0.0),0.05,LSMVector4(1.0,1.0,1.0,1.0),s.Camera3D->GetProjectionMatrixNoTranslationXZ(),false);

                                        }
                                    }


                                }
                             }
                         }*/


                    }
                }
            }
        }
    }

    inline void OnDrawGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {

        for(int i = m_Visualizers.length()-1; i>-1; i--)
        {
            UIRigidObjectVisualizer* vis = m_Visualizers.at(i);
            if(!(vis->m_ShouldBeRemoved))
            {
                LSMVector3 position = vis->m_Position;
                LSMVector2 offset = m_RigidWorld->GetSimpleGeoOrigin();

                float CursorX = s.scr_width * (offset.x + position.x - s.tlx)/s.width;
                float CursorY = (s.scr_height * (offset.y + position.z - s.tly)/s.height) ;

                float width_black = std::max(1.0,2.0 * s.ui_scale2d3d);
                float size = std::max(1.0,5.0 *s.ui_scale2d3d);

                //draw black dot on cursor location
                m->m_ShapePainter->DrawLine(CursorX-size,CursorY,CursorX+size,CursorY,width_black,LSMVector4(0.0,0.0,0.0,1.0));
                m->m_ShapePainter->DrawLine(CursorX,CursorY-size,CursorX,CursorY+size,width_black,LSMVector4(0.0,0.0,0.0,1.0));


            }
        }

    }

    inline void OnPrepare(OpenGLCLManager * m,GeoWindowState s) override
    {
        m_IsPrepared = true;
    }

    inline void OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm) override
    {

    }

    inline void OnDestroy(OpenGLCLManager * m) override
    {
        ClearAssets();
    }

    inline void UpdatePositionInfo(OpenGLCLManager * m) override
    {


    }

    inline float MinimumLegendHeight(OpenGLCLManager * m, GeoWindowState state) override
    {
        return 0.0;
    }

    inline float MaximumLegendHeight(OpenGLCLManager * m, GeoWindowState s) override
    {
        return 0.0;
    }

    inline virtual void OnDrawLegend(OpenGLCLManager * m, GeoWindowState state, float posy_start = 0.0, float posy_end = 0.0)
    {

    }

    inline virtual LayerProbeResult Probe(LSMVector2 Pos, GeoProjection proj, double tolerence)
    {
        return LayerProbeResult();
    }

    inline virtual LayerProbeResult Probe3D(LSMVector2 Pos, GeoProjection proj, double tolerence)
    {
        return LayerProbeResult();
    }
};




#endif // UIRIGIDWORLDLAYER_H

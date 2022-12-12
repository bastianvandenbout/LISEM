#include "uirigidworldeditor.h"

UIRigidWorldLayerEditor::UIRigidWorldLayerEditor(UILayer * rwl) : UILayerEditor(rwl)
{
    m_RigidWorldLayer =dynamic_cast<UIRigidWorldLayer*>( rwl);

    AddParameter(UI_PARAMETER_TYPE_GROUP,"Rigid World Tool","","");
    AddParameter(UI_PARAMETER_TYPE_RADIO,"Type","Select|DrawSelect|Add|AddConstraint|Fragment|PropertySet","Select");
    AddParameter(UI_PARAMETER_TYPE_RADIO,"Shape","Box|Ellipsoid|Cilinder|Object|Terrain","Box");
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Shape Size X","Horizontal extent of the added object","10.0",0,1000000.0);
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Shape Size Y","Vertical extent of the added object","10.0",0,1000000.0);
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Shape Size Z","Depth extent of the added object","10.0",0,1000000.0);
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Add Distance","Distance from the camera used to determine where to add a new object","0",0.0,1.0);
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Density","The density of the object","1",-1e31,1e31);
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Friction","The frictional coefficient for the objects surface","1",-1e31,1e31);
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Constraint Force","Force at which the rigid constraint breaks","1",-1e31,1e31);
    AddParameter(UI_PARAMETER_TYPE_FLOAT,"Fragment Size","The fragment size to create from the selected objects","1",-1e31,1e31);
}


void UIRigidWorldLayerEditor::OnPrepare(OpenGLCLManager *m)
{


}
void UIRigidWorldLayerEditor::OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
{

}

void UIRigidWorldLayerEditor::OnMouse3DPressed(int button, int pressed, GeoWindowState s,LSMVector2 ppos, LSMVector3 Pos, LSMVector3 Dir)
{
    m_ScriptMutex.lock();
    QString edit_action_string = GetParameterValue("Type");

    std::cout << "on mouse 3d pressed " << std::endl;
    if(edit_action_string == "Select")
    {
        Dir = Dir.Normalize();

        LayerProbeResult res = LayerProbeResult();
        //raytrace our world, and check if we get an intersection

        m_RigidWorldLayer->m_RigidWorld->LockMutex();
        RigidPhysicsObject * objf = nullptr;
        double dist_min = 0.0;
        bool found = false;
        ModelRayCastResult castf;
        LSMMatrix4x4 modelmtif;

        QList<RigidPhysicsObject *> objl = m_RigidWorldLayer->m_RigidWorld->GetObjectList();
        for(int i = 0; i < objl.size(); i++)
        {
            RigidPhysicsObject * robj = objl.at(i);

            LSMVector3 Position = robj->GetPosition() + LSMVector3(m_RigidWorldLayer->m_RigidWorld->GetSimpleGeoOrigin().x,0.0,m_RigidWorldLayer->m_RigidWorld->GetSimpleGeoOrigin().y);
            LSMVector3 Rotation = robj->GetRotation();

            ModelGeometry * model = robj->GetModel();
            LSMVector3 objpos = LSMVector3(0.0,0.0,0.0) + Position;
            LSMMatrix4x4 objrotm = LSMMatrix4x4();
            objrotm.SetRotation3(Rotation.x,Rotation.y,Rotation.z);
            LSMMatrix4x4 scalem = LSMMatrix4x4();
            scalem.SetScaling(1.0,1.0,1.0);

            LSMMatrix4x4 modelmti = objrotm*scalem;
            modelmti.Invert();
            modelmti.Transpose();

            LSMMatrix4x4 objtrans;
            objtrans.Translate(LSMVector3(objpos.x,objpos.y,objpos.z));
            objrotm = objtrans*objrotm*scalem;

            LSMMatrix4x4 objw =objrotm;
            objw.Invert();
            LSMMatrix4x4 objtoworld = objrotm;
            LSMMatrix4x4 worldtoobj = objw;

            ModelRayCastResult cast = model->RayCast(Pos,Dir,objtoworld,worldtoobj);
            double dist= (Pos-cast.m_Position).length();


            if(cast.m_Mesh != nullptr)
            {
                if(!found)
                {
                    dist_min = dist;
                    objf = robj;
                    found =true;
                    castf = cast;
                    modelmtif = modelmti;
                }else if(dist < dist_min)
                {
                    dist_min = dist;
                    objf = robj;
                    castf = cast;
                    modelmtif = modelmti;
                }

            }

        }

        if(found)
        {
            std::cout << "object found for selection " << objf << std::endl;
            LSMVector3 posf = objf->GetPosition()  + LSMVector3(m_RigidWorldLayer->m_RigidWorld->GetSimpleGeoOrigin().x,0.0,m_RigidWorldLayer->m_RigidWorld->GetSimpleGeoOrigin().y);
            LSMVector3 velf = objf->GetVelocity();

            if(button == GLFW_MOUSE_BUTTON_1)
            {
                m_RigidWorldLayer->m_RigidWorld->SetObjectHighlight(objf,LSMVector4(1.0,0.3,0.2,0.5),true);
            }else if(button == GLFW_MOUSE_BUTTON_2)
            {
                m_RigidWorldLayer->m_RigidWorld->SetObjectHighlight(objf,LSMVector4(0.0,0.0,0.0,0.0),true);

            }


        }

        m_RigidWorldLayer->m_RigidWorld->UnLockMutex();
    }



    m_ScriptMutex.unlock();


}
void UIRigidWorldLayerEditor::OnMousePressed(int button, int pressed)
{

    QString edit_action_string = GetParameterValue("Type");

    //are we in selection mode?

    //are we in 3d? check for raytrainc

    //are we in 2d? check proximity of object center

    //add to selection,
    //set highlighting

}

void UIRigidWorldLayerEditor::OnDraw(OpenGLCLManager * m,GeoWindowState s)
{

    m_ScriptMutex.lock();

    QString edit_action_string = GetParameterValue("Type");
    QString edit_shapetype_string = GetParameterValue("Shape");


    //GeoCoordTransformer * transformer = GeoCoordTransformer::GetCoordTransformer(s.projection,m_RasterLayer->GetProjection());

    //LSMVector2 MousePosLoc = transformer->Transform(LSMVector2(state.MouseGeoPosX,state.MouseGeoPosZ));




    m_ScriptMutex.unlock();
    //only called once per frame
    //use this to do the instructed edits

}
void UIRigidWorldLayerEditor::OnDrawGeo(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
{
    //do the actual drawing process
    //all we need it a 2D screen overlay for the DrawSelect
    //otherwise we can use the gizmo and object highlighting stuff we already have from the engine




}

void UIRigidWorldLayerEditor::OnDrawPostProcess(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
{


}


#include "uirigidworldeditor.h"

UIRigidWorldLayerEditor::UIRigidWorldLayerEditor(UILayer * rwl) : UILayerEditor(rwl)
{
    m_RigidWorldLayer =dynamic_cast<UIRigidWorldLayer*>( rwl);

    AddParameter(UI_PARAMETER_TYPE_GROUP,"Rigid World Tool","","");
    AddParameter(UI_PARAMETER_TYPE_RADIO,"Type","Select|DrawSelect|Add|AddConstraint|Fragment|PropertySet","Selection");
    AddParameter(UI_PARAMETER_TYPE_RADIO,"Shape","Box|Ellipsoid|Object|Terrain","Box");
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
void UIRigidWorldLayerEditor::OnParameterChanged(QString name, QString value,std::function<void(QString,QString)> fset, std::function<void(QString,QString)> foptions)
{

}
void UIRigidWorldLayerEditor::OnDraw(OpenGLCLManager * m,GeoWindowState s)
{

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


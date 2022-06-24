
#include "layer/editors/uishadereditor.h"

UIShaderLayerEditor::UIShaderLayerEditor(UILayer * rl) : UILayerEditor(rl)
{

    m_ShaderLayer =dynamic_cast<UIShaderLayer*>( rl);


    AddParameter(UI_PARAMETER_TYPE_SHADER,"SHADER","Shader Editor","");


}

void UIShaderLayerEditor::OnUpdate(std::vector<QString> shaders,std::vector<std::vector<QString>> files,std::vector<std::vector<cTMap *>> maps)
{

    m_ShaderLayer->UpdateShader(shaders);
    m_ShaderLayer->UpdateImages(files,maps);


}
void UIShaderLayerEditor::OnRun()
{


}
void UIShaderLayerEditor::OnPause()
{


}
void UIShaderLayerEditor::OnCoordinates(BoundingBox b, bool abs, bool is_2d,GeoProjection p)
{


}

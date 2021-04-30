#include "layer/uilayereditor.h"


UILayerEditor::UILayerEditor(UILayer * ml)
{
    if(ml->IsGeo())
    {
        m_IsGeo= true;
        m_GLayer = (UIGeoLayer*) ml;

    }

    m_Layer = ml;

    m_Exists = true;

    if(m_Transformer != nullptr)
    {
        delete m_Transformer;
    }
    if(m_Transformerinv != nullptr)
    {
        delete m_Transformerinv;
    }
    m_Transformer = GeoCoordTransformer::GetCoordTransformerGeneric();
    m_Transformerinv = GeoCoordTransformer::GetCoordTransformerGeneric();
}

void UILayerEditor::OnCRSChanged(OpenGLCLManager * m, GeoWindowState s, WorldGLTransformManager * tm)
{
    if(m_Transformer != nullptr)
    {
        delete m_Transformer;
    }
    if(m_Transformerinv != nullptr)
    {
        delete m_Transformerinv;
    }
    m_Transformer = GeoCoordTransformer::GetCoordTransformer(s.projection,m_GLayer->GetProjection());
    m_Transformerinv = GeoCoordTransformer::GetCoordTransformer(m_GLayer->GetProjection(),s.projection);

}

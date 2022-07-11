#include "model.h"
#include <thread>
#include <QThread>
#include "iogdal.h"


void LISEMModel::OnDraw()
{



    m_ModelDataMutex.lock();
    m_uimin = m_InterfaceData.val_min;
    m_uimax = m_InterfaceData.val_max;
    m_ModelDataMutex.unlock();

    if(m_UIMapsSet)
    {
        m_OpenGLCLManager->CopyTextureToMap(TUI_H,&(MUIH->data));

        if(!m_MemorySave)
        {
            m_OpenGLCLManager->CopyTextureToMap(TUI_UI,&(MUI->data));
            m_OpenGLCLManager->CopyTextureToMap(TUI_UX,&(MUIUX->data));
            m_OpenGLCLManager->CopyTextureToMap(TUI_UY,&(MUIUY->data));
            m_OpenGLCLManager->CopyTextureToMap(TUI_HALL,&(MUIHALL->data));
            m_OpenGLCLManager->CopyTextureToMap(TUI_HS,&(MUIHS->data));
        }


    }


}

void LISEMModel::DrawPlotDataToBuffer(GLuint framebuffer)
{


}

void LISEMModel::DrawPlotInterfaceToBuffer(GLuint framebuffer)
{


}

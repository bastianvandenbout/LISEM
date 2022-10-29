#include "uirigidworldlayer.h"
#include "layer/editors/uirigidworldeditor.h"


UILayerEditor* UIRigidWorldLayer::GetEditor()
{
    std::cout << "get editor rigid " << std::endl;
    return new UIRigidWorldLayerEditor(this);

}

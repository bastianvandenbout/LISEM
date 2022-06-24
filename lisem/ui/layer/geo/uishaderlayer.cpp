#include "layer/uilayer.h"
#include "uishaderlayer.h"

UILayerEditor* UIShaderLayer::GetEditor()
{
    std::cout << "get editor " << std::endl;
    return new UIShaderLayerEditor(this);

}

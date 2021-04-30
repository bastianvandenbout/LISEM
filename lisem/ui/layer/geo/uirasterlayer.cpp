#include "uirasterlayer.h"

UILayerEditor* UIRasterLayer::GetEditor()
{
    return new UIRasterLayerEditor(this);

}

#include "uirasterstreamlayer.h"

UILayerEditor* UIStreamRasterLayer::GetEditor()
{
    std::cout << "get editor " << std::endl;
    return new UIRasterLayerEditor(this);

}

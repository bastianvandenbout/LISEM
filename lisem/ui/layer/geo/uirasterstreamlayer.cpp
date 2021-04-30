#include "uirasterstreamlayer.h"

UILayerEditor* UIStreamRasterLayer::GetEditor()
{
    return new UIRasterLayerEditor(this);

}

#include "uivectorlayer.h"

inline UILayerEditor* UIVectorLayer::GetEditor()
{
    return new UIVectorLayerEditor(this);

}


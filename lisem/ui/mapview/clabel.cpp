#include "clabel.h"
#include "model.h" 
#include "layerwidget.h"
#include <iostream>

void CLabel::mousePressEvent(QMouseEvent* event)
{
    /*LayerWidget *s = (LayerWidget*)(parent());
    if (s != nullptr) {
        //move mouse button press forward
        std::cout <<" mouse clicked to drag"<< std::endl;
        s->mousePressEvent(event);
    } else {
        QLabel::mousePressEvent(event);
    }*/

    event->ignore();

    emit clicked(m_arg);
    emit clicked();
}

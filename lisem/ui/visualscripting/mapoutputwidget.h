#ifndef MAPOUTPUTWIDGET_H
#define MAPOUTPUTWIDGET_H


#include <QLabel>
#include "geo/raster/map.h"



class MapOutputPreviewWidget : public QLabel
{

    Q_OBJECT;

public:
    inline MapOutputPreviewWidget() : QLabel()
    {

    }
    inline ~MapOutputPreviewWidget()
    {

    }


public:

    inline void SetIsLoading(bool load)
    {

    }

    inline void SetMapData(cTMap * m)
    {

    }


};



#endif // MAPOUTPUTWIDGET_H

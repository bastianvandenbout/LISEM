#ifndef PYLISEM_UI_H
#define PYLISEM_UI_H

#include "pylisem.h"
#include "pybind11/pybind11.h"
#include "pybind11/functional.h"
#include <pybind11/numpy.h>
#include <functional>
//#include "extensions/modelscripting.h"
#include <exception>
namespace py = pybind11;



inline void SetChannelUpdateCallback(int channel, std::function<void(bool,BoundingBox)> f)
{


    d->m_ChannelCallbacks[channel] = f;



}

inline void ClearChannelUpdateCallback(int channel)
{

    if(d->m_ChannelCallbacks.find(channel) != d->m_ChannelCallbacks.end())
    {
        d->m_ChannelCallbacks.erase(channel);
    }


}


inline void CallChannelUpdateCallback(int channel, bool focus = false, BoundingBox b = BoundingBox(0.0,0.0,0.0,0.0))
{

    if(d->m_ChannelCallbacks.find(channel) != d->m_ChannelCallbacks.end())
    {
        d->m_ChannelCallbacks[channel](focus,b);
    }


}



inline py::array_t<uint8_t> GetLISEMPlot(int sizex, int sizey, double ulx, double sizevx,  double uly,double sizevy, int channel)
{
    GeoWindowState viewstate = d->m_WorldWindow->ExternalDrawGeoWindowState(sizex,sizey,BoundingBox(ulx,ulx + sizevx,uly,uly + sizevy),true,true,true,1.1);

    viewstate.ChannelFilter = true;
    viewstate.Channel = channel;
    viewstate.draw_legends = true;
    viewstate.draw_cursor = false;
    uchar * image = d->m_WorldWindow->DoExternalDrawToUChar(viewstate);

    int ncols = sizex;
    int nrows = sizey;

    // Create a Python object that will free the allocated
    // memory when destroyed:
    py::capsule free_when_done(image, [](void *f) {
        uint8_t *foo = reinterpret_cast<uint8_t *>(f);
        delete[] foo;
    });

    return py::array_t<uint8_t>(
        {nrows,ncols,3}, // shape
        {ncols*1*3, 1*3,1}, // C-style contiguous strides for double
        image, // the data pointer
        free_when_done); // numpy array references this parent
}


inline py::array_t<uint8_t> GetLISEMPlot3D(int sizex, int sizey, LSMVector3 Position, LSMVector3 ViewDir, LSMVector3 Up, float fov, int channel)
{
    GeoWindowState viewstate = d->m_WorldWindow->ExternalDrawGeoWindowState(sizex,sizey,BoundingBox(Position.x, 1.0, Position.y, 1.0),true,true,true,1.1);

    viewstate.is_3d = true;
    viewstate.Camera3D->SetPosition(Position.x,Position.y, Position.z);
    viewstate.Camera3D->SetViewDir(ViewDir.x, ViewDir.y,ViewDir.z);

    viewstate.ChannelFilter = true;
    viewstate.Channel = channel;
    uchar * image = d->m_WorldWindow->DoExternalDrawToUChar(viewstate);

    int ncols = sizex;
    int nrows = sizey;

    // Create a Python object that will free the allocated
    // memory when destroyed:
    py::capsule free_when_done(image, [](void *f) {
        uint8_t *foo = reinterpret_cast<uint8_t *>(f);
        delete[] foo;
    });

    return py::array_t<uint8_t>(
        {nrows,ncols,3}, // shape
        {ncols*1*3, 1*3,1}, // C-style contiguous strides for double
        image, // the data pointer
        free_when_done); // numpy array references this parent
}



class LISEM_API LISEM_UILAYER_ID
{
public:
    int uid = -1;


    inline int GetUID()
    {
        return uid;
    }
};


inline UILayer * GetLayerFromUIID(LISEM_UILAYER_ID layer)
{
    for(int i = 0; i < d->m_WorldWindow->GetUILayerCount(); i++)
    {
        UILayer *l = d->m_WorldWindow->GetUILayer(i);
        if(l->GetUID() == layer.uid)
        {
            return l;
        }
    }

    return nullptr;

}



inline LISEM_UILAYER_ID AddLayer(cTMap * mi, int channel)
{

    cTMap*m = mi->GetCopy();
    UILayer * l = d->m_WorldWindow->GetUILayerFromMap(m,"py_layer",false);

    l->SetDrawLegend(true);
    l->SetChannel(channel);
    d->m_WorldWindow->AddUILayer(l,true,false);
    LISEM_UILAYER_ID uid;
    uid.uid = l->GetUID();

    CallChannelUpdateCallback(channel, true,((UIGeoLayer *)(l))->GetBoundingBox());
    return uid;
}

inline LISEM_UILAYER_ID AddLayer(ShapeFile * mi, int channel)
{

    ShapeFile * f = mi->GetCopy();
    UILayer * l = d->m_WorldWindow->GetUILayerFromShapeFile(f,"py_layer",false);

    l->SetDrawLegend(true);
    l->SetChannel(channel);
    d->m_WorldWindow->AddUILayer(l,true,false);
    LISEM_UILAYER_ID uid;
    uid.uid = l->GetUID();

    CallChannelUpdateCallback(channel, true,((UIGeoLayer *)(l))->GetBoundingBox());
    return uid;
}



inline void ReplaceLayer(LISEM_UILAYER_ID layer, cTMap * m)
{


    UILayer * l = GetLayerFromUIID(layer);

    if(l != nullptr)
    {
        if(l->layertypeName() == "RasterStreamLayer")
        {
            UIStreamRasterLayer * lrs =  (UIStreamRasterLayer *) l;
            if(lrs->IsLayerDirectReplaceable({{m}}))
            {
                d->m_WorldWindow->m_UILayerMutex.lock();
                std::cout << "do direct replace " << std::endl;
                lrs->DirectReplace({{m->GetCopy()}});
                d->m_WorldWindow->m_UILayerMutex.unlock();
                return;
            }
        }

        RasterDataProvider * rdp = new RasterDataProvider(QList<QList<cTMap*>>({{m->GetCopy()}}),false,true);


        if(rdp->Exists())
        {
            QString name =l->GetName();
            UIStreamRasterLayer * ret = new UIStreamRasterLayer(rdp,name,m->AS_FileName.size() == 0? false : true,m->AS_FileName,!false);
            if(rdp->GetBandCount() > 1)
            {
                ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_MULTIRASTER),true);
            }else
            {
                ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_RASTERUI),true);
            }

            d->m_WorldWindow->ReplaceUILayer(l,ret);

            CallChannelUpdateCallback(l->GetChannel());

        }else
        {

            LISEM_WARNING("Could not create raster layer to replace layer");
        }
    }
}




inline void LookAtLayer(LISEM_UILAYER_ID id)
{

    for(int i = 0; i < d->m_WorldWindow->GetUILayerCount(); i++)
    {
        UILayer *l = d->m_WorldWindow->GetUILayer(i);
        if(l->GetUID() == id.uid && l->IsGeo())
        {
            d->m_WorldWindow->LookAt((UIGeoLayer*)l);
        }
    }


}

inline QString ValueProbe(double x, double y)
{


    return QString();

}

inline void SetUISize(double size)
{

}

inline void Clear(int Channel)
{
    for(int i = d->m_WorldWindow->GetUILayerCount() - 1; i > -1; i--)
    {
        UILayer *l = d->m_WorldWindow->GetUILayer(i);
        if(l->GetChannel() == Channel)
        {
            d->m_WorldWindow->RemoveUILayer(l);
        }
    }

    CallChannelUpdateCallback(Channel);

}


inline void RequestStartAndWaitPyVis(QList<QString> options, RigidPhysicsWorld * world, int channel, std::function<void(void)> f)
{

    d->m_ModelIsRunningMutex.lock();
    if(d->m_ModelIsRunning)
    {

        d->m_ModelIsRunningMutex.unlock();
        py::print("Model is already running");
        return;


    }


    std::cout << "Request start and wait "<< std::endl;
    LISEMModel * m = GetGLobalModel();
    if(m != nullptr)
    {

        std::cout << "Global model found" << std::endl;
        m->FinishMutex.lock();
        m->m_ModelStateMutex.lock();
        m->m_Options = QList<QString>(options);
        m->m_RigidWorld = world;
        m->m_StartRequested = true;
        m->m_ModelStateMutex.unlock();

        d->m_ModelIsRunning = true;
        std::cout << "requested start" << std::endl;
        m->OnStep();


        std::cout << "test1" << std::endl;
        f();
        std::cout << "test2" << std::endl;

         m->FinishMutex.unlock();
        m->SetCallBackTimeStep([d,m,f,channel](int step, int step_max,bool start, bool stop){

            if(start)
            {
                std::cout << "add ui layer "<<std::endl;
                BoundingBox b = d->m_WorldWindow->AddNativeUILayers(channel,d->m_Model);
                CallChannelUpdateCallback(channel, true,b);

                std::cout << "done add ui layer "<<std::endl;

            }else if(stop)
            {
                d->m_WorldWindow->RemoveNativeUILayers(channel);

                d->m_ModelIsRunningMutex.lock();

                d->m_ModelIsRunning = false;

                d->m_ModelIsRunningMutex.unlock();

            }else
            {
                std::cout << "call back()"<<std::endl;

                //py::gil_scoped_acquire acquire;

                try
                {

                    f();
                }catch(std::exception e)
                {
                   std::cout << "error std " << e.what() << std::endl;
                }


                //py::gil_scoped_release release;

                std::cout << "call back done()"<<std::endl;

            }





        });


    }

    d->m_ModelIsRunningMutex.unlock();


}

inline void RequestStartAndWaitPyVis(QString file, QString add_options, RigidPhysicsWorld * world, int channel,std::function<void(void)> func)
{

    ParameterManager* m_ParameterManager = new ParameterManager();
    m_ParameterManager->InitParameters();

        QFileInfo f(file);
        if(f.exists())
        {
            return RequestStartAndWaitPyVis(m_ParameterManager->GetParameterListFromFile(file,add_options),world,channel,func);
        }else {

            LISEM_ERROR("Run file could not be found " + file);
        }


        m_ParameterManager->Destroy();
        SAFE_DELETE(m_ParameterManager);

}

inline void StartModelFromRunFilePyVis(QString file,QString options, int channel, std::function<void(void)> f)
{
    LISEMS_STATUS("Requesting model simulation with runfile : " + QString(file));
    return RequestStartAndWaitPyVis( QString(file),options, nullptr, channel,f);
}

inline void SetLayerColorGradient(LISEM_UILAYER_ID layer, QString gradient)
{
    UILayer * l = GetLayerFromUIID(layer);
    if(l != nullptr)
    {

        LSMColorGradient grad = GetDefaultGradientFromName(gradient);
        l->GetStyleRef()->m_ColorGradientb1 = grad;
        CallChannelUpdateCallback(l->GetChannel(),false,BoundingBox(0.0,0.0,0.0,0.0));


    }
}

inline void SetLayerColor(LISEM_UILAYER_ID layer, LSMVector4 gradient)
{
    UILayer * l = GetLayerFromUIID(layer);
    if(l != nullptr)
    {

        CallChannelUpdateCallback(l->GetChannel(),false,BoundingBox(0.0,0.0,0.0,0.0));


    }

}

inline void SetLayerFillStyle(LISEM_UILAYER_ID layer, QString gradient)
{
    UILayer * l = GetLayerFromUIID(layer);
    if(l != nullptr)
    {

        CallChannelUpdateCallback(l->GetChannel(),false,BoundingBox(0.0,0.0,0.0,0.0));


    }


}
inline void SetLayerLineStyle(LISEM_UILAYER_ID layer, QString gradient)
{

    UILayer * l = GetLayerFromUIID(layer);
    if(l != nullptr)
    {

        CallChannelUpdateCallback(l->GetChannel(),false,BoundingBox(0.0,0.0,0.0,0.0));


    }

}

inline void SetLayerHillshade(LISEM_UILAYER_ID layer, bool x)
{
    UILayer * l = GetLayerFromUIID(layer);
    if(l != nullptr)
    {
        l->SetDrawAsHillShade(x);
        CallChannelUpdateCallback(l->GetChannel(),false,BoundingBox(0.0,0.0,0.0,0.0));


    }


}
inline void SetLayerElevation(LISEM_UILAYER_ID layer, bool x)
{
    UILayer * l = GetLayerFromUIID(layer);
    if(l != nullptr)
    {
        l->SetDrawAsDEM(true);
        CallChannelUpdateCallback(l->GetChannel(),false,BoundingBox(0.0,0.0,0.0,0.0));
    }


}

inline void SetLayerLegend(LISEM_UILAYER_ID layer, bool x)
{
    UILayer * l = GetLayerFromUIID(layer);
    if(l != nullptr)
    {
        l->SetDrawLegend(x);
        CallChannelUpdateCallback(l->GetChannel(),false,BoundingBox(0.0,0.0,0.0,0.0));


    }


}

inline BoundingBox GetTotalBoundingBox(int channel)
{
    BoundingBox b;
    bool set = false;
    for(int i = 0; i < d->m_WorldWindow->GetUILayerCount(); i++)
    {
        UILayer *l = d->m_WorldWindow->GetUILayer(i);
        if(l->GetChannel() == channel && l->IsGeo())
        {
            if(set)
            {
                b.MergeWith( ((UIGeoLayer*)(l))->GetBoundingBox());
            }else
            {
                set =true;
                b = ((UIGeoLayer*)(l))->GetBoundingBox();
            }
        }
    }

    return b;


}

inline void init_pylisem_ui(py::module &m)
{


    m.def("GeoPlot",&GetLISEMPlot,py::arg("Width"),py::arg("Height"), py::arg("geo_ulx"),py::arg("geo_sizex"),py::arg("geo_uly"),py::arg("geo_sizey"), py::arg("Channel") = 0);
    m.def("GeoPlot3D",&GetLISEMPlot3D,py::arg("Width"),py::arg("Height"), py::arg("Position"),py::arg("ViewDir"),py::arg("Up"),py::arg("FoV"), py::arg("Channel") = 0);

    py::class_<LISEM_UILAYER_ID>(m,"Layer");

    //adding layers
    m.def("AddLayer",py::overload_cast<cTMap*,int>(&AddLayer),py::arg("Map"), py::arg("Channel") = 0);
    m.def("AddLayer",py::overload_cast<ShapeFile*,int>(&AddLayer),py::arg("Shapes"), py::arg("Channel") = 0);

    //removing layers
    m.def("ReplaceLayer",&ReplaceLayer, py::arg("Layer"),py::arg("Data"));

    m.def("ClearLayers",&Clear, py::arg("Channel") = 0);

    //query layers

    m.def("RegisterChannelUpdateCallback",&SetChannelUpdateCallback,py::arg("Channel"),py::arg("CallBack"));
    m.def("ClearChannelUpdateCallback",&ClearChannelUpdateCallback,py::arg("Channel"));

    //re-sorting layers

    //styling layers
    m.def("SetLayerColorGradient",&SetLayerColorGradient,py::arg("Layer"),py::arg("GradientName"));
    m.def("SetLayerHillshade",&SetLayerHillshade,py::arg("Layer"),py::arg("Hillshade"));
    m.def("SetLayerElevation",&SetLayerElevation,py::arg("Layer"),py::arg("Elevation"));
    m.def("SetLayerLegend",&SetLayerLegend,py::arg("Layer"),py::arg("Legend"));

    //view setup
    m.def("GetLayersRegion",&GetTotalBoundingBox, py::arg("Channel"));

    //Query view

    //clear command


    //visualized model
    m.def("RunModelVis",&StartModelFromRunFilePyVis,py::arg("runfile"),py::arg("options"), py::arg("Channel"), py::arg("Callback"));


}

#endif // PYLISEM_UI_H

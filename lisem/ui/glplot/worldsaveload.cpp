#include "worldwindow.h"

#include "layer/3d/gl3docean.h"
#include "layer/3d/gl3dskybox.h"
#include "layer/3d/gl3dterrain.h"
#include "layer/geo/uirasterstreamlayer.h"
#include "layer/geo/uivectorlayer.h"
#include "layer/geo/uipointcloudlayer.h"
#include "layer/geo/uiwebtilelayer.h"
#include "layer/3d/uicloudlayer.h"

#include "QJsonArray"
#include "QJsonValue"

void WorldWindow::RegisterLayerTypes()
{

    m_LayerRegistry = new UILayerTypeRegistry();

    m_LayerRegistry->registerModel<UIStreamRasterLayer>();
    m_LayerRegistry->registerModel<UIVectorLayer>();
    m_LayerRegistry->registerModel<UIPointCloudLayer>();
    m_LayerRegistry->registerModel<UIWebTileLayer>();
    m_LayerRegistry->registerModel<UIOceanLayer>();
    m_LayerRegistry->registerModel<UISkyBoxLayer>();
    m_LayerRegistry->registerModel<UICloudLayer>();

}

void WorldWindow::SaveTo(QJsonObject * obj)
{

      QJsonArray layerJsonArray;

      m_UILayerMutex.lock();

      for(int i = 0; i < m_UILayerList.length(); i++)
      {
          if(m_UILayerList.at(i)->IsLayerSaveAble())
          {
              //the layer is saveable
              UILayer * l =m_UILayerList.at(i);
              QString l_typename = l->layertypeName();

              if(m_LayerRegistry->isRegistered(l_typename))
              {
                  //the layer type is registered

                  QJsonObject lobj;
                  l->SaveLayer(&lobj);
                  lobj["TypeName"] = l_typename;
                  layerJsonArray.append(lobj);
              }
          }
      }

      (*obj)["layers"] = layerJsonArray;

      m_UILayerMutex.unlock();


}

void WorldWindow::LoadFrom(QJsonObject * obj, bool delete_old)
{

    m_UILayerMutex.lock();

    bool first = true;

   QList<UILayer * > layers;

   QJsonValue layersval = (*obj)["layers"];
   if(layersval.isArray() &&!layersval.isUndefined())
   {
        QJsonArray layerJsonArray = layersval.toArray();

        std::cout << "loading layer files " << layerJsonArray.size() << std::endl;

        for(int i = 0; i < layerJsonArray.size(); i++)
        {

            QJsonValue layerval = layerJsonArray.at(i);
            if(layerval.isObject() && !layerval.isUndefined())
            {


                QJsonObject obj_layer = layerval.toObject();
                QJsonValue nameval = obj_layer["TypeName"];
                std::cout << "layer exists: " << i << " " << nameval.isUndefined() << " " << nameval.isString() <<  std::endl;
                if(!nameval.isUndefined())
                {
                    QString l_typename = nameval.toString();

                    std::cout << "layer type name: " << l_typename.toStdString() << std::endl;

                    if(m_LayerRegistry->isRegistered(l_typename))
                    {
                        std::cout << "layer type is registered " << l_typename.toStdString() << std::endl;

                        if(first)
                        {
                            if(delete_old)
                            {
                                for(int i = m_UILayerList.length()-1; i > -1; i--)
                                {
                                    if(m_UILayerList.at(i)->IsUser() == true)
                                    {
                                        m_UILayerList.at(i)->SetRemove();
                                    }
                                }
                            }

                            m_UILayersChanged = true;


                            first = false;
                        }

                        UILayer * l = m_LayerRegistry->create(l_typename);
                        l->RestoreLayer(&obj_layer);

                        if(l->Exists())
                        {
                            layers.append(l);
                        }
                    }
                }
            }
        }
   }

   m_UILayerMutex.unlock();

   for(int i = 0; i < layers.length(); i++)
   {
        AddUILayer(layers.at(i),false);
   }

   {
       emit OnMapsChanged();
   }

   //if there were no layers
   if(first == true)
   {

   }
}


















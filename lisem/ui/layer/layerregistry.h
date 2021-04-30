#pragma once

#include "layer/uilayer.h"
#include "functional"
#include "map"
#include <set>
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>

class UILayerTypeRegistry
{


private:

  std::map<QString, std::function<UILayer*()>>_registeredItemCreators;

public:

    UILayerTypeRegistry()  = default;
    ~UILayerTypeRegistry() = default;


    UILayerTypeRegistry(UILayerTypeRegistry const &) = delete;
    UILayerTypeRegistry(UILayerTypeRegistry &&)      = default;

public:
    template<typename UILayerType>
    void registerModel(std::function<UILayer*()> creator)
    {
      registerModelImpl<UILayerType>(std::move(creator));
    }

    template<typename UILayerType>
    void registerModel()
    {
      std::function<UILayer*()> creator = [](){ return new UILayerType(); };
      registerModelImpl<UILayerType>(std::move(creator));
    }

    inline UILayer* create(QString const &modelName)
    {
      auto it = _registeredItemCreators.find(modelName);

      if (it != _registeredItemCreators.end())
      {
        return it->second();
      }

      return nullptr;
    }


    inline bool isRegistered(QString name)
    {
        return _registeredItemCreators.count(name) > 0;
    }
  private:
  // If the registered ModelType class has the static member method
  //
  //      static Qstring Name();
  //
  // use it. Otherwise use the non-static method:
  //
  //       virtual QString name() const;

  template <typename T, typename = void>
  struct HasStaticMethodName
      : std::false_type
  {};

  template <typename T>
  struct HasStaticMethodName<T,
          typename std::enable_if<std::is_same<decltype(T::LayerTypeName()), QString>::value>::type>
      : std::true_type
  {};

  template<typename UILayerType>
  typename std::enable_if< HasStaticMethodName<UILayerType>::value>::type
  registerModelImpl(std::function<UILayer*()> creator)
  {
    const QString name = UILayerType::LayerTypeName();
    if (_registeredItemCreators.count(name) == 0)
    {
      _registeredItemCreators[name] = std::move(creator);
    }
  }

  template<typename UILayerType>
  typename std::enable_if< !HasStaticMethodName<UILayerType>::value>::type
  registerModelImpl(std::function<UILayer*()> creator)
  {
    const QString name = creator()->layertypeName();
    if (_registeredItemCreators.count(name) == 0)
    {
      _registeredItemCreators[name] = std::move(creator);
    }
  }
};

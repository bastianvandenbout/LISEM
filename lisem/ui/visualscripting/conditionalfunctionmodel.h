#pragma once

#include "scriptmanager.h"
#include "mapdatatype.h"
#include "QComboBox"
#include "functionalnodemodel.h"

class ConditionalFunctionModel : public FunctionalNodeModel
{

    Q_OBJECT;

private:


public:

    QList<QString> m_Types;
    int m_IndexType = 0;


    std::function<void(ConditionalFunctionModel *)> m_FuncReset;

    inline ConditionalFunctionModel(QList<QString> types, std::function<void(ConditionalFunctionModel *)> f,  int index_default = 3)
    {
        m_IndexType = index_default;
        m_FuncReset = f;
        m_ComboBox = new QComboBox();
        m_ComboBox->insertItems(0,types);
        connect(m_ComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(OnTypeChanged(int)));
        m_Types = types;
    }
    inline ~ConditionalFunctionModel() {}


public:

  QString
  caption() const override
  { return QString("If,Else"); }


  bool
  captionVisible() const override
  { return true; }

  bool
  portCaptionVisible(QtNodes::PortType, QtNodes::PortIndex ) const override
  { return true; }

  QString
  portCaption(QtNodes::PortType portType,QtNodes::PortIndex portIndex) const override
  {
    switch (portType)
    {
      case QtNodes::PortType::In:
            {
                if(portIndex == 0)
                {
                    return "Condition (bool)";
                }else if(portIndex == 1)
                {
                    return "If true (" + m_Types.at(m_IndexType) + ")";
                }else
                {
                    return "If false (" + m_Types.at(m_IndexType) + ")";
                }
            }

      case QtNodes::PortType::Out:
            {
                return "Out " + m_Types.at(m_IndexType);

            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return "If,Else";
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 3;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {
      switch (portType)
      {
        case QtNodes::PortType::In:
              {
                  if(portIndex == 0)
                  {
                      return GetDataFromTypeName("bool");
                  }else if(portIndex == 1)
                  {
                      return GetDataFromTypeName(m_Types.at(m_IndexType));
                  }else
                  {
                      return GetDataFromTypeName(m_Types.at(m_IndexType));
                  }

                  return GetDataFromTypeName(m_Types.at(m_IndexType));
              }

        case QtNodes::PortType::Out:
              {
                  return GetDataFromTypeName(m_Types.at(m_IndexType));
              }


        default:
          break;
      }

      return MapData().type();
  }

  inline std::shared_ptr<QtNodes::NodeData> outData(QtNodes::PortIndex port) override
  {
        return std::static_pointer_cast<QtNodes::NodeData>(_result);
  }

  inline void setInData(std::shared_ptr<QtNodes::NodeData> data, QtNodes::PortIndex portIndex) override
  {
      auto Data =
        std::dynamic_pointer_cast<MapData>(data);

      if (portIndex == 0)
      {
        _number1 = Data;
      }
      else
      {
        _number2 = Data;
      }

      compute();

  }

  inline QWidget * embeddedWidget() override
  {
      return m_ComboBox;
  }

  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["type"] = m_IndexType;

    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {

      QJsonValue vcount = p["count"];

      if(!vcount.isUndefined())
      {
            m_IndexType = vcount.toInt();
            m_ComboBox->setCurrentIndex(m_IndexType);
      }


  }



protected:

  //bool
  //eventFilter(QObject *object, QEvent *event) override;

protected:

  inline void compute()
  {

  }

  std::weak_ptr<MapData> _number1;
  std::weak_ptr<MapData> _number2;

  std::shared_ptr<MapData> _result;

    QComboBox * m_ComboBox = nullptr;

public slots:

  inline void OnTypeChanged(int x)
  {

       m_IndexType = x;
       m_FuncReset(this);

        std::cout << "variable type changed " << std::endl;
  }

};

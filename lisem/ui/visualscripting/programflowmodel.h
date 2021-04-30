#pragma once
#include "functionalnodemodel.h"
#include "scriptmanager.h"
#include "mapdatatype.h"

#include "QGroupBox"
#include "QComboBox"
#include "QToolButton"
#include "QVBoxLayout"
#include "widgets/labeledwidget.h"
#include "QLabel"

class WhileFunctionModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:

    QList<QString> m_Types;
    QList<int> m_IndexTypes;
    int m_Count = 0;

    std::function<void(WhileFunctionModel *)> m_FuncReset;


public:

    inline WhileFunctionModel(QList<QString> types, std::function<void(WhileFunctionModel *)> f,int count_default, QList<int> intypes)
    {

        m_Types =types;
        m_IndexTypes = intypes;
        m_Count = std::max(1,count_default);
        for(int i = m_IndexTypes.length(); i < count_default+1; i++)
        {
            m_IndexTypes.append(3);
        }

        for(int i = m_IndexTypes.length(); i > count_default-1; i--)
        {
            m_IndexTypes.removeAt(i);
        }

        m_FuncReset = f;
        m_Count = count_default;

        m_Widget = new QGroupBox();
        QVBoxLayout * l = new QVBoxLayout();
        m_Widget->setLayout(l);

        for(int i = 0; i < m_Count; i++)
        {
            QComboBox * m_ComboBox = new QComboBox();
            m_ComboBox->insertItems(0,types);
            m_ComboBox->setCurrentIndex(m_IndexTypes.at(i));
            l->addWidget(new QWidgetHDuo(new QLabel("Type "+QString::number(i)),m_ComboBox));

            connect(m_ComboBox,qOverload<int>(&QComboBox::currentIndexChanged),[i,this](const int x){m_IndexTypes[i] = x; m_FuncReset(this);});


        }

        QToolButton * ButtonPlus = new QToolButton();
        QToolButton * ButtonMinus = new QToolButton();
        ButtonPlus->setText("+");
        ButtonMinus->setText("-");
        l->addWidget(new QWidgetHDuo(ButtonPlus,ButtonMinus));

        connect(ButtonMinus,&QToolButton::pressed,[this](){this->m_Count = std::max(1,this->m_Count-1); m_FuncReset(this);});
        connect(ButtonPlus,&QToolButton::pressed,[this](){this->m_Count = std::max(1,this->m_Count+1); m_FuncReset(this);});


    }
    inline ~WhileFunctionModel() {}


public:

  QString
  caption() const override
  { return QString("While"); }


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

        if(portIndex == 2 * m_Count)
        {
            return "Condition (bool)";
        }

                if(portIndex % 2 == 0)
                {
                    return "In (" + m_Types.at(m_IndexTypes.at(portIndex/2)) + ")";
                }else {
                    return "Loop (" + m_Types.at(m_IndexTypes.at(portIndex/2)) + ")";

                }

                return "In " + QString::number(portIndex);
            }

      case QtNodes::PortType::Out:
            {
                if(portIndex % 2 == 0)
                {
                    return "Out (" + m_Types.at(m_IndexTypes.at(portIndex/2)) + ")";
                }else {
                    return "Loop (" + m_Types.at(m_IndexTypes.at(portIndex/2)) + ")";

                }


            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return "While";
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 2 * m_Count + 1;
      }
      else
      {
          return 2 * m_Count;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {
      switch (portType)
      {
        case QtNodes::PortType::In:
              {
                  if(portIndex == 2 * m_Count)
                  {
                      return GetDataFromTypeName("bool");
                  }else {
                        return GetDataFromTypeName(m_Types.at(m_IndexTypes.at(portIndex/2)));
                    }

              }

        case QtNodes::PortType::Out:
              {
                  return GetDataFromTypeName(m_Types.at(m_IndexTypes.at(portIndex/2)));
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
      return m_Widget;
  }

  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["count"] = m_Count;

    for(int i = 0; i < m_Count; i++)
    {
        modelJson["type_"+QString::number(i)] = QString::number(m_IndexTypes.at(i));

    }

    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue vcount = p["count"];

      if (!vcount.isUndefined())
      {
        int val = vcount.toInt(0);

        m_Count = val;

        m_IndexTypes.clear();
        for(int i = 0; i < m_Count; i++)
        {
            QJsonValue vitem = p["type_"+QString::number(i)];
            int item = 0;
            if (!vitem.isUndefined())
            {
                item = vitem.toInt(0);
            }
            m_IndexTypes.append(item);
        }

        std::cout << m_Count << " reset node " << std::endl;

        //m_FuncReset(this);


        QVBoxLayout * l = (QVBoxLayout*)m_Widget->layout();
        //m_Widget->setLayout(l);
        QLayoutItem *wItem = l->takeAt(0);
        while (wItem != 0)
        {
            delete wItem;
            wItem = l->takeAt(0);
        }


        for(int i = 0; i < m_Count; i++)
        {
            QComboBox * m_ComboBox = new QComboBox();
            m_ComboBox->insertItems(0,m_Types);
            m_ComboBox->setCurrentIndex(m_IndexTypes.at(i));
            l->addWidget(new QWidgetHDuo(new QLabel("Type "+QString::number(i)),m_ComboBox));

            connect(m_ComboBox,qOverload<int>(&QComboBox::currentIndexChanged),[i,this](const int x){m_IndexTypes[i] = x; m_FuncReset(this);});


        }

        QToolButton * ButtonPlus = new QToolButton();
        QToolButton * ButtonMinus = new QToolButton();
        ButtonPlus->setText("+");
        ButtonMinus->setText("-");
        l->addWidget(new QWidgetHDuo(ButtonPlus,ButtonMinus));

        connect(ButtonMinus,&QToolButton::pressed,[this](){this->m_Count = std::max(1,this->m_Count-1); m_FuncReset(this);});
        connect(ButtonPlus,&QToolButton::pressed,[this](){this->m_Count = std::max(1,this->m_Count+1); m_FuncReset(this);});


        l->update();
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


  QGroupBox * m_Widget;
};


#ifndef NUMBERSOURCEMODEL_H
#define NUMBERSOURCEMODEL_H

#include "functionalnodemodel.h"
#include "scriptmanager.h"
#include "mapdatatype.h"
#include "QLineEdit"
#include "QDoubleSpinBox"
#include "QSpinBox"
#include "QCheckBox"
#include "tools/toolparameterwidget.h"
#include "rect-selection.h"

#include "widgets/imageviewer.h"
#include "geo/raster/map.h"
#include "resourcemanager.h"
#include "site.h"

class StringSourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline StringSourceModel()
    {
        m_ValueEdit = new QLineEdit();


        m_ReturnData.isvalid = true;
        m_ReturnData.type = "string";
        m_ReturnData.xm = std::make_shared<QString>("");

        connect(m_ValueEdit,&QLineEdit::textChanged,this,&StringSourceModel::OnValueChanged);
    }
    inline ~StringSourceModel() {}


public:

  QString
  caption() const override
  { return QString("String Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (String)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return QString("String Source");
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {
      return GetDataFromTypeName("string");
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
      return m_ValueEdit;
  }

  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, LSMScriptEngine * e)
  {
      QList<QString> l;

      l.append("string " + outputs.at(0) + "=" + "\"" + m_ValueEdit->text() + "\";");
      return l;
  }

  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] = m_ValueEdit->text();


    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        QString str = v.toString();

        m_ValueEdit->setText(str);
      }

  }


  bool m_ReturnError = false;
  PreviewArgument m_ReturnData;

  inline virtual bool IsReturner() override
  {
      return true;
  }

  inline virtual void SetPreviewData(PreviewArgument data) override
  {
      if(data.isvalid)
      {
          SetExternalDataChangeFlag(true);
      }

      std::cout << "set preview data for map source"<< std::endl;
      if(data.isvalid)
      {
            m_ReturnData = data;
            //m_Label->setText(data.xb? "true":"false");
      }else
      {
            m_ReturnData = PreviewArgument();
            //m_ReturnError = true;
      }
  }

  inline virtual bool IsInternalDataForPreviewValid()
  {
      return true;

  }

  inline virtual void ClearPreview() override
  {
      //m_Label->setText("No preview available!");
      //m_ReturnError = false;
      m_ReturnData = PreviewArgument();
  }


  inline virtual void ClearError()
  {
      m_ReturnError = false;
      SetExternalDataChangeFlag(true);
  }

  inline virtual bool IsPreviewError() override
  {
      return m_ReturnError;
  }

  inline virtual bool IsPreviewAvailable() override
  {
      return m_ReturnData.isvalid;
  }

  inline virtual PreviewArgument GetPreviewData() override
  {
      if(m_ReturnData.isvalid)
      {
           return m_ReturnData;
      }
      return PreviewArgument();
  }


public slots:

  inline void OnValueChanged(QString x)
  {

      m_ReturnData.isvalid = true;
      m_ReturnData.type = "string";
      m_ReturnData.xm = std::make_shared<QString>(x);

      emit OnInternalDataChanged();

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



  QLineEdit * m_ValueEdit = nullptr;



};

class DoubleSourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline DoubleSourceModel()
    {
        m_ValueEdit = new QDoubleSpinBox();

        m_ReturnData.isvalid = true;
        m_ReturnData.type = "double";
        m_ReturnData.xd = 0.0;

        connect(m_ValueEdit,qOverload<double>(&QDoubleSpinBox::valueChanged),this,&DoubleSourceModel::OnValueChanged);

    }
    inline ~DoubleSourceModel() {}


public:

  QString
  caption() const override
  { return QString("Number Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (Number)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return QString("Number Source");
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {


    return GetDataFromTypeName("double");
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
      return m_ValueEdit;
  }


  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] =m_ValueEdit->value();


    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        int str = v.toInt(0);

        m_ValueEdit->setValue(str);
      }

  }


  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, LSMScriptEngine * e)
  {
      QList<QString> l;

      l.append("double " + outputs.at(0) + "=" + "" + QString::number(m_ValueEdit->value()) + ";");
      return l;
  }



  bool m_ReturnError = false;
  PreviewArgument m_ReturnData;

  inline virtual bool IsReturner() override
  {
      return true;
  }

  inline virtual void SetPreviewData(PreviewArgument data) override
  {
      if(data.isvalid)
      {
          SetExternalDataChangeFlag(true);
      }

      std::cout << "set preview data for map source"<< std::endl;
      if(data.isvalid)
      {
            m_ReturnData = data;
            //m_Label->setText(data.xb? "true":"false");
      }else
      {
            m_ReturnData = PreviewArgument();
            //m_ReturnError = true;
      }
  }

  inline virtual bool IsInternalDataForPreviewValid()
  {
      return true;

  }

  inline virtual void ClearPreview() override
  {
      //m_Label->setText("No preview available!");
      //m_ReturnError = false;
      m_ReturnData = PreviewArgument();
  }


  inline virtual void ClearError()
  {
      m_ReturnError = false;
      SetExternalDataChangeFlag(true);
  }

  inline virtual bool IsPreviewError() override
  {
      return m_ReturnError;
  }

  inline virtual bool IsPreviewAvailable() override
  {
      return m_ReturnData.isvalid;
  }

  inline virtual PreviewArgument GetPreviewData() override
  {
      if(m_ReturnData.isvalid)
      {
           return m_ReturnData;
      }
      return PreviewArgument();
  }


public slots:

  inline void OnValueChanged(double x)
  {

      m_ReturnData.isvalid = true;
      m_ReturnData.type = "double";
      m_ReturnData.xd = x;

      emit OnInternalDataChanged();

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


  QDoubleSpinBox * m_ValueEdit = nullptr;



};


class IntSourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline IntSourceModel()
    {
        m_ValueEdit = new QSpinBox();


        m_ReturnData.isvalid = true;
        m_ReturnData.type = "int";
        m_ReturnData.xi = 0;

        connect(m_ValueEdit,qOverload<int>(&QSpinBox::valueChanged),this,&IntSourceModel::OnValueChanged);
    }
    inline ~IntSourceModel() {}


public:

  QString
  caption() const override
  { return QString("Integer Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (Integer)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {
    return QString("Integer Source");

  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {


   return GetDataFromTypeName("int");
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
      return m_ValueEdit;
  }

  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] =m_ValueEdit->value();


    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        int str = v.toInt(0);

        m_ValueEdit->setValue(str);
      }

  }


  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, LSMScriptEngine * e)
  {
      QList<QString> l;

      l.append("int " + outputs.at(0) + "=" + "" + QString::number(m_ValueEdit->value()) + ";");
      return l;
  }



  bool m_ReturnError = false;
  PreviewArgument m_ReturnData;

  inline virtual bool IsReturner() override
  {
      return true;
  }

  inline virtual void SetPreviewData(PreviewArgument data) override
  {
      if(data.isvalid)
      {
          SetExternalDataChangeFlag(true);
      }

      std::cout << "set preview data for map source"<< std::endl;
      if(data.isvalid)
      {
            m_ReturnData = data;
            //m_Label->setText(data.xb? "true":"false");
      }else
      {
            m_ReturnData = PreviewArgument();
            //m_ReturnError = true;
      }
  }

  inline virtual bool IsInternalDataForPreviewValid()
  {
      return true;

  }

  inline virtual void ClearPreview() override
  {
      //m_Label->setText("No preview available!");
      //m_ReturnError = false;
      m_ReturnData = PreviewArgument();
  }


  inline virtual void ClearError()
  {
      m_ReturnError = false;
      SetExternalDataChangeFlag(true);
  }

  inline virtual bool IsPreviewError() override
  {
      return m_ReturnError;
  }

  inline virtual bool IsPreviewAvailable() override
  {
      return m_ReturnData.isvalid;
  }

  inline virtual PreviewArgument GetPreviewData() override
  {
      if(m_ReturnData.isvalid)
      {
           return m_ReturnData;
      }
      return PreviewArgument();
  }


public slots:

  inline void OnValueChanged(int x)
  {

      m_ReturnData.isvalid = true;
      m_ReturnData.type = "int";
      m_ReturnData.xi = x;

      emit OnInternalDataChanged();

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


  QSpinBox * m_ValueEdit = nullptr;


};


class BoolSourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline BoolSourceModel()
    {
        m_ValueEdit = new QCheckBox();
        m_ValueEdit->setChecked(false);
        m_ReturnData.isvalid = true;
        m_ReturnData.type = "bool";
        m_ReturnData.xb = false;

        connect(m_ValueEdit,&QCheckBox::clicked,this,&BoolSourceModel::OnValueChanged);
    }
    inline ~BoolSourceModel() {}


public:

  QString
  caption() const override
  { return QString("Boolean Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (Boolean)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return QString("Boolean Source");
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {
        return GetDataFromTypeName("bool");
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
      return m_ValueEdit;
  }

  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] =m_ValueEdit->isChecked()?1:0;

    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        int str = v.toInt(0);

        m_ValueEdit->setChecked(str > 0);
      }

  }


  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, LSMScriptEngine * e)
  {
      QList<QString> l;

      l.append("bool " + outputs.at(0) + "=" + "" + QString(m_ValueEdit->isChecked()? "true": "false") + ";");
      return l;
  }

  bool m_ReturnError = false;
  PreviewArgument m_ReturnData;

  inline virtual bool IsReturner() override
  {
      return true;
  }

  inline virtual void SetPreviewData(PreviewArgument data) override
  {
      if(data.isvalid)
      {
          SetExternalDataChangeFlag(true);
      }

      std::cout << "set preview data for map source"<< std::endl;
      if(data.isvalid)
      {
            m_ReturnData = data;
            //m_Label->setText(data.xb? "true":"false");
      }else
      {
            m_ReturnData = PreviewArgument();
            m_ReturnError = true;
      }
  }

  inline virtual bool IsInternalDataForPreviewValid()
  {
      return true;

  }

  inline virtual void ClearPreview() override
  {
      //m_Label->setText("No preview available!");
      //m_ReturnError = false;
      //m_ReturnData = PreviewArgument();
  }

  inline virtual void ClearError()
  {
      m_ReturnError = false;
      SetExternalDataChangeFlag(true);
  }

  inline virtual bool IsPreviewError() override
  {
      return m_ReturnError;
  }

  inline virtual bool IsPreviewAvailable() override
  {
      return m_ReturnData.isvalid;
  }

  inline virtual PreviewArgument GetPreviewData() override
  {
      if(m_ReturnData.isvalid)
      {
           return m_ReturnData;
      }
      return PreviewArgument();
  }

public slots:

  inline void OnValueChanged(bool x)
  {
      m_ReturnData.isvalid = true;
      m_ReturnData.type = "bool";
      m_ReturnData.xb = x;

      emit OnInternalDataChanged();
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


  QCheckBox * m_ValueEdit = nullptr;


public slots:



};



class MapSourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:

    QGroupBox * m_Widget;
    pal::ImageViewer * m_ImageViewer;
    std::shared_ptr<cTMap> m_Map = nullptr;
    bool error = false;
public:

    inline MapSourceModel()
    {
        m_Widget = new QGroupBox();
        m_Widget->setStyleSheet("background-color: rgba(255,255,255,0)");
        QVBoxLayout * l = new QVBoxLayout();
        m_Widget->setLayout(l);

        QString Dir = GetSite();

        m_ValueEdit = new MapSelectWidget();
        m_ValueEdit->setStyleSheet("background-color: rgba(255,255,255,185)");
        l->addWidget(m_ValueEdit);

        connect(m_ValueEdit, &MapSelectWidget::OnValueChanged,this,MapSourceModel::OnValueChanged);

        m_ImageViewer = new pal::ImageViewer();
        m_ImageViewer->setToolBarMode(pal::ImageViewer::ToolBarMode::AutoHidden);
        m_ImageViewer->setStyleSheet("background-color: rgba(255,255,255,185)");
        m_ImageViewer->setMaximumSize(500,500);

        auto sel = new QToolButton();
                sel->setToolTip(tr("Selects a rectangle area in the image"));
                sel->setIcon(QIcon(Dir + LISEM_FOLDER_ASSETS + "image-selection.png"));
                sel->setCheckable(true);

        // selection tool
        auto selecter = new pal::SelectionItem(m_ImageViewer->pixmapItem());
        selecter->setVisible(false);

        // cropping only allowed when an image is visible
       auto updater = [=] {
           const bool ok = !m_ImageViewer->image().isNull();
           if (ok)
               selecter->resetSelection();
           else
               sel->setChecked(false);
           sel->setEnabled(ok);
       };

       connect(sel, &QToolButton::toggled, selecter, &pal::SelectionItem::setVisible);
       connect(m_ImageViewer, &pal::ImageViewer::imageChanged, selecter, updater);
       updater();
       m_ImageViewer->addTool(sel);

       //set an empty image with gray values initially

       QImage i(100,100,QImage::Format_RGB888);
       i.fill(qRgba(25,25,25,255.0));

       m_ImageViewer->setImage(i);

       l->addWidget(m_ImageViewer);


    }
    inline ~MapSourceModel() {}
public slots:
  inline void OnValueChanged()
  {
      emit OnInternalDataChanged();
  }

public:

  QString
  caption() const override
  { return QString("Map Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (Map)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return QString("Map Source");
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {
        return GetDataFromTypeName("Map");
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

    modelJson["value"] =m_ValueEdit->GetValueAsString();


    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        QString str = v.toString();

        m_ValueEdit->SetValue(str);
      }

  }


  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, LSMScriptEngine * e)
  {
      QList<QString> l;

      l.append("Map " + outputs.at(0) + "=" + "" + m_ValueEdit->GetValueAsString() + ";");
      return l;
  }

  inline virtual bool IsReturner() override
  {
      return true;
  }

  inline virtual void SetPreviewData(PreviewArgument data) override
  {
      if(data.isvalid)
      {
          SetExternalDataChangeFlag(true);
      }
      std::cout << "set preview data for map source"<< std::endl;
      if(data.type == "Map" && data.xm != nullptr)
      {
          std::cout << "ok"<< std::endl;

          m_Map = std::static_pointer_cast<cTMap>(data.xm);

          QImage i(m_Map->nrCols(),m_Map->nrRows(),QImage::Format_RGB888);

          RasterBandStats rs = m_Map->GetRasterBandStats(false);

          FOR_ROW_COL(m_Map)
          {
              float val = (m_Map->data[r][c] - rs.min)/(std::max(1e-20,rs.max-rs.min));
              i.setPixel(c,r,qRgba(255.0 * val,255.0*val,255.0*val,255.0));
          }
          m_ImageViewer->SetToolTipSingleBandMinMax(rs.min,rs.max);
          m_ImageViewer->setImage(i);
      }else
      {
          m_ImageViewer->SetToolTipDefault();
          QImage i(100,100,QImage::Format_RGB888);
          i.fill(qRgba(25,25,25,255.0));

          m_ImageViewer->setImage(i);

            error = true;
      }
  }

  inline virtual bool IsInternalDataForPreviewValid()
  {
      return !m_ValueEdit->GetValueAsString().isEmpty();

  }

  inline virtual void ClearError()
  {
      error = false;
      SetExternalDataChangeFlag(true);
  }

  inline virtual void ClearPreview() override
  {

      m_ImageViewer->SetToolTipDefault();
      QImage i(100,100,QImage::Format_RGB888);
      i.fill(qRgba(25,25,25,255.0));

      m_ImageViewer->setImage(i);

      if(m_Map != nullptr)
      {
          m_Map = nullptr;

      }
      error = false;
  }

  inline virtual void SetPreviewIsLoading() override
  {
      QIcon mLoadIcon =*(GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_RELOAD),-1));
      QImage i = mLoadIcon.pixmap(QSize(255, 255)).toImage();
      m_ImageViewer->setImage(i);


  }

  inline virtual bool IsPreviewError() override
  {

      return error;
  }

  inline virtual bool IsPreviewAvailable() override
  {
      std::cout << "check if map source has map " << (m_Map != nullptr) << std::endl;
      return m_Map != nullptr;
  }

  inline virtual PreviewArgument GetPreviewData() override
  {

      std::cout << "get preview data from source model " << std::endl;
      if(m_Map != nullptr)
      {
          PreviewArgument p;
          p.isvalid = true;
          p.type = "Map";
          p.xm = m_Map;

          std::cout << "reutrn map " << p.xm.get() << std::endl;
          return p;
      }
      return PreviewArgument();
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

  MapSelectWidget * m_ValueEdit = nullptr;


public slots:



};


class ShapesSourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline ShapesSourceModel()
    {
        m_ValueEdit = new ShapeFileSelectWidget();
    }
    inline ~ShapesSourceModel() {}


public:

  QString
  caption() const override
  { return QString("Shapes Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (Shapes)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return QString("Shapes Source");
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {
    return GetDataFromTypeName("Shapes");
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
      return m_ValueEdit;
  }

  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] = m_ValueEdit->GetValueAsString();

    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        QString str = v.toString();

        m_ValueEdit->SetValue(str);
      }

  }


  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, LSMScriptEngine * e)
  {
      QList<QString> l;

      l.append("Shapes " + outputs.at(0) + "=" + "" + QString(m_ValueEdit->GetValueAsString()) + ";");
      return l;
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


  ShapeFileSelectWidget * m_ValueEdit = nullptr;


public slots:



};


class TableSourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline TableSourceModel()
    {
        m_ValueEdit = new TableSelectWidget();
    }
    inline ~TableSourceModel() {}


public:

  QString
  caption() const override
  { return QString("Table Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (Table)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return QString("Table Source");
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {
        return GetDataFromTypeName("Table");
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
      return m_ValueEdit;
  }

  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] = m_ValueEdit->GetValueAsString();

    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        QString str = v.toString();

        m_ValueEdit->SetValue(str);
      }

  }


  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, LSMScriptEngine * e)
  {
      QList<QString> l;

      l.append("Table " + outputs.at(0) + "=" + "" + QString(m_ValueEdit->GetValueAsString()) + ";");
      return l;
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

  TableSelectWidget * m_ValueEdit = nullptr;


public slots:



};


class GeoProjectionSourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline GeoProjectionSourceModel()
    {
        m_ValueEdit = new GeoProjectionSelectWidget();
    }
    inline ~GeoProjectionSourceModel() {}


public:

  QString
  caption() const override
  { return QString("GeoProjection Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (GeoProjection)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return QString("GeoProjection Source");
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {
    return GetDataFromTypeName("GeoProjection");
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
      return m_ValueEdit;
  }

  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] = m_ValueEdit->GetValueAsString();

    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        QString str = v.toString();

        m_ValueEdit->SetValue(str);
      }

  }


  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, LSMScriptEngine * e)
  {
      QList<QString> l;

      l.append("Shapes " + outputs.at(0) + "=" + "" + QString(m_ValueEdit->GetValueAsString()) + ";");
      return l;
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

  GeoProjectionSelectWidget * m_ValueEdit = nullptr;


public slots:



};

















class PointCloudSourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline PointCloudSourceModel()
    {
        m_ValueEdit = new PointCloudSelectWidget();
    }
    inline ~PointCloudSourceModel() {}


public:

  QString
  caption() const override
  { return QString("PointCloud Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (PointCloud)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return QString("PointCloud Source");
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {
    return GetDataFromTypeName("PointCloud");
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
      return m_ValueEdit;
  }

  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] = m_ValueEdit->GetValueAsString();

    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        QString str = v.toString();

        m_ValueEdit->SetValue(str);
      }

  }


  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, LSMScriptEngine * e)
  {
      QList<QString> l;

      l.append("PointCloud " + outputs.at(0) + "=" + "" + QString(m_ValueEdit->GetValueAsString()) + ";");
      return l;
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

  PointCloudSelectWidget * m_ValueEdit = nullptr;


public slots:



};


class MapArraySourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline MapArraySourceModel()
    {
        m_ValueEdit = new MapArraySelectWidget();
    }
    inline ~MapArraySourceModel() {}


public:

  QString
  caption() const override
  { return QString("MapArray Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (MapArray)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return QString("MapArray Source");
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {
        return GetDataFromTypeName("MapArray");
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
      return m_ValueEdit;
  }


  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] = m_ValueEdit->GetValueAsString();

    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        QString str = v.toString();

        m_ValueEdit->SetValue(str);
      }

  }


  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, LSMScriptEngine * e)
  {
      QList<QString> l;

      l.append("MapArray " + outputs.at(0) + "=" + "" + QString(m_ValueEdit->GetValueAsString()) + ";");
      return l;
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

  MapArraySelectWidget * m_ValueEdit = nullptr;


public slots:



};


class BoundingBoxSourceModel : public FunctionalNodeModel
{

    Q_OBJECT;

public:


public:

    inline BoundingBoxSourceModel()
    {
        m_ValueEdit = new BoundingBoxSelectWidget();
    }
    inline ~BoundingBoxSourceModel() {}


public:

  QString
  caption() const override
  { return QString("BoundingBox Source"); }


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

            }

      case QtNodes::PortType::Out:
            {
                return "Value (BoundingBox)";
            }


      default:
        break;
    }
    return QString();
  }

  QString
  name() const override
  {

      return QString("BoundingBox Source");
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          return 0;
      }
      else
      {
          return 1;
      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {
    return GetDataFromTypeName("BoundingBox");
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
      return m_ValueEdit;
  }

  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    modelJson["value"] = m_ValueEdit->GetValueAsString();

    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
      QJsonValue v = p["value"];

      if (!v.isUndefined())
      {
        QString str = v.toString();

        m_ValueEdit->SetValue(str);
      }

  }


  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, LSMScriptEngine * e)
  {
      QList<QString> l;

      l.append("BoundingBox " + outputs.at(0) + "=" + "" + QString(m_ValueEdit->GetValueAsString()) + ";");
      return l;
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

  BoundingBoxSelectWidget * m_ValueEdit = nullptr;


public slots:



};



#endif // NUMBERSOURCEMODEL_H

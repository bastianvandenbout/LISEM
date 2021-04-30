#pragma once

#include "functionalnodemodel.h"
#include "scriptmanager.h"
#include "mapdatatype.h"
#include "functionalnodemodel.h"
#include "QGroupBox"
#include "QCheckBox"
#include "QVBoxLayout"
#include "widgets/imageviewer.h"
#include "QLabel"
#include "lsmio.h"
#include "rect-selection.h"
#include "QToolButton"
#include "extensionprovider.h"
#include "iogdal.h"
#include "resourcemanager.h"
#include "site.h"

class MapFunctionModel : public FunctionalNodeModel
{

    Q_OBJECT;

private:

    ScriptFunctionInfo m_Function;
    ScriptManager *m_ScriptManager;

    QGroupBox * m_Widget;
    QToolButton  * m_SaveButton;
    QCheckBox * m_CheckBox;
    pal::ImageViewer * m_ImageViewer;
    QLabel * m_Label;

    std::shared_ptr<cTMap> m_Map = nullptr;

    bool m_IsMapReturner = false;
    bool m_IsReturner = false;
    bool m_IsReturnError = false;
    PreviewArgument m_ReturnData;
public:

    inline MapFunctionModel(ScriptFunctionInfo func, ScriptManager * sm)
    {
        m_ScriptManager = sm;
        m_Function = func;

        if(m_Function.is_member || IsReturner())
        {

            m_Widget = new QGroupBox();
            QVBoxLayout * l = new QVBoxLayout();
            m_Widget->setLayout(l);
            m_Widget->setStyleSheet("background-color: rgba(255,255,255,0)");
            if(m_Function.is_member)
            {
                m_CheckBox = new QCheckBox();
                m_CheckBox->setStyleSheet("background-color: rgba(255,255,255,185)");
                m_CheckBox->setText("Duplicate Subject");
                m_CheckBox->setChecked(true);
                l->addWidget(m_CheckBox);
            }
            if(IsReturner())
            {
                m_IsReturner = true;

                int tid = m_Function.Function->GetReturnTypeId();
                QString type = m_ScriptManager->m_Engine->GetTypeName(tid);

                if(type == "Map")
                {
                    QString Dir = GetSite();

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
                   m_SaveButton= new QToolButton();
                   m_SaveButton->setText("Save map as..");

                   connect(m_SaveButton,&QToolButton::pressed,[this](){

                       QList<QString> exts =GetMapExtensions();

                       QString extss;
                       for(int i = 0; i < exts.length(); i++)
                       {
                           extss += QString("*") + exts.at(i);
                           if(i != exts.length())
                           {
                               extss += ";";
                           }
                       }

                       exts += ";*.*";
                       QString path = QFileDialog::getOpenFileName(m_SaveButton,QString("Select a save file"),
                                                                   "",extss);

                       if(!path.isEmpty())
                       {

                           //now ask layer to do the saving!

                           bool success = true;

                           QString format;
                           if(path.length() > 3)
                           {
                               format = GetGDALDriverForRasterFile(path);
                                writeRaster(*m_Map,path,format);

                           if(!success)
                           {
                               QMessageBox msgBox;
                               msgBox.setText("An error occured when saving file to: " + path);
                               msgBox.exec();
                           }
                       }
                       }

                       ;});


                   m_SaveButton->setEnabled(false);
                   l->addWidget(m_SaveButton);
                    l->addWidget(m_ImageViewer);

                    m_IsMapReturner = true;

                }else
                {
                    m_Label = new QLabel();
                    m_Label->setText("No preview available!");
                    l->addWidget(m_Label);

                }
            }

        }


    }
    inline ~MapFunctionModel() {}


public:


  QString
  caption() const override
  {
      if(m_Function.is_basetypeoperator)
      {
          QString op = m_ScriptManager->m_Engine->GetOperatorFromMemberFunctionName(m_Function.basetypeoperatorname);
          QString opname = m_ScriptManager->m_Engine->GetOperatorNameFromMemberFunctionName(m_Function.basetypeoperatorname);

          if(m_Function.basetypeoperatorparametertype.isEmpty())
          {
                return op + " " + m_Function.ObjectName + " (" + opname + ")";
          }else {
                return m_Function.ObjectName + " " +  op + " " +m_Function.basetypeoperatorparametertype + " (" + opname + ")";
            }

      }else {
          QString parameters;
          parameters += "(";

          int paramcount = m_Function.Function->GetParamCount();

          for(int i = 0; i < paramcount; i++)
          {
              int id = 0;
              m_Function.Function->GetParam(i,&id);
              parameters += m_ScriptManager->m_Engine->GetTypeName(id);
              if(i != paramcount -1)
              {
                  parameters += ",";
              }
          }
          parameters += ")";

          if(m_Function.is_constructor)
          {
              return QString(m_Function.ObjectName);
          }else if(m_Function.is_member)
          {
              QString op;
              QString opname;
              bool opinv;

              if(QString(m_Function.Function->GetName()).compare("eq") == 0)
              {
                  op = "==";
                  opname = "equal to";
                  opinv = false;

              }else if(QString(m_Function.Function->GetName()).compare("eq_r") == 0)
              {
                  op = "==";
                  opname = "equal to";
                  opinv = true;

              }else if(QString(m_Function.Function->GetName()).compare("neq") == 0)
              {
                  op = "!=";
                  opname = "not equal to";
                  opinv = false;

              }else if(QString(m_Function.Function->GetName()).compare("neq_r") == 0)
              {
                    op = "!=";
                    opname = "not equal to";
                    opinv = true;

              }else {

                  op = m_ScriptManager->m_Engine->GetOperatorFromMemberFunctionName(m_Function.Function->GetName());
                  opname = m_ScriptManager->m_Engine->GetOperatorNameFromMemberFunctionName(m_Function.Function->GetName());
                  opinv = m_ScriptManager->m_Engine->GetOperatorIsReverseFromMemberFunctionName(m_Function.Function->GetName());
              }

              if(!op.isEmpty())
              {

                 return opname;
              }else {
                  return m_Function.ObjectName;
              }
          }else {
              return QString(m_Function.Function->GetName());
         }

      }

  }


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
                if(m_Function.is_basetypeoperator)
                {

                    if(portIndex == 0)
                    {
                        return "Input 1 (" + m_Function.ObjectName + ")";

                    }else {
                        return "Input 2 (" + m_Function.basetypeoperatorparametertype + ")";

                    }
                }else {
                    if(m_Function.is_member)
                    {
                        if(portIndex == 0)
                        {
                            QString name;
                            QString type = m_Function.ObjectName;

                            QString out  = name.isEmpty()? QString("Input ") + QString::number(portIndex) + " (" + type + ")": name + " (" + type + ")";
                            return out;
                        }else {
                            portIndex = portIndex -1;

                            QString name;
                            if(portIndex < m_Function.Parameters.length())
                            {
                                name =m_Function.Parameters.at(portIndex);
                            }
                            int tid = 0;
                            m_Function.Function->GetParam(portIndex, &tid);
                            QString type = m_ScriptManager->m_Engine->GetTypeName(tid);

                            QString out  = name.isEmpty()? QString("Input ") + QString::number(portIndex+1) + " (" + type + ")": name + " (" + type + ")";
                            return out;
                        }
                    }else {
                        QString name;
                        if(portIndex < m_Function.Parameters.length())
                        {
                            name =m_Function.Parameters.at(portIndex);
                        }
                        int tid = 0;
                        m_Function.Function->GetParam(portIndex, &tid);
                        QString type = m_ScriptManager->m_Engine->GetTypeName(tid);

                        QString out  = name.isEmpty()? QString("Input ") + QString::number(portIndex) + " (" + type + ")": name + " (" + type + ")";
                        return out;

                    }
                }
            }

      case QtNodes::PortType::Out:
            {
                if(m_Function.is_basetypeoperator)
                {
                    return m_Function.basetypeoperatorreturntype;

                }else
                {
                    if(m_Function.is_valueobjectconstructor)
                    {
                        return QString("Result (") + m_Function.ObjectName + ")";

                    }else {
                        if(m_Function.is_member)
                        {
                            if(portIndex == 0)
                            {
                                QString name;
                                QString type = m_Function.ObjectName;

                                QString out  = name.isEmpty()? QString("Subject ") + " (" + type + ")": name + " (" + type + ")";
                                return out;
                            }else {
                                portIndex = portIndex -1;
                                int tid = m_Function.Function->GetReturnTypeId();
                                QString type = m_ScriptManager->m_Engine->GetTypeName(tid);


                                return QString("Result (") + type + ")";
                            }
                        }else {

                            int tid = m_Function.Function->GetReturnTypeId();
                            QString type = m_ScriptManager->m_Engine->GetTypeName(tid);


                            return QString("Result (") + type + ")";

                        }

                    }

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
      if(m_Function.is_basetypeoperator)
      {
          QString op = m_ScriptManager->m_Engine->GetOperatorFromMemberFunctionName(m_Function.basetypeoperatorname);
          QString opname = m_ScriptManager->m_Engine->GetOperatorNameFromMemberFunctionName(m_Function.basetypeoperatorname);

          if(m_Function.basetypeoperatorparametertype.isEmpty())
          {
                return op + " " + m_Function.ObjectName + " (" + opname + ")";
          }else {
                return m_Function.ObjectName + " " +  op + " " +m_Function.basetypeoperatorparametertype +  " (" + opname + ")";
            }

      }else {
          QString parameters;
          parameters += "(";

          int paramcount = m_Function.Function->GetParamCount();

          for(int i = 0; i < paramcount; i++)
          {
              int id = 0;
              m_Function.Function->GetParam(i,&id);
              parameters += m_ScriptManager->m_Engine->GetTypeName(id);
              if(i != paramcount -1)
              {
                  parameters += ",";
              }
          }
          parameters += ")";

          if(m_Function.is_constructor)
          {
              return QString(m_Function.ObjectName)+"::"+ parameters;
          }else if(m_Function.is_member)
          {
              QString op;
              QString opname;
              bool opinv;

              if(QString(m_Function.Function->GetName()).compare("eq") == 0)
              {
                  op = "==";
                  opname = "equal to";
                  opinv = false;

              }else if(QString(m_Function.Function->GetName()).compare("eq_r") == 0)
              {
                  op = "==";
                  opname = "equal to";
                  opinv = true;

              }else if(QString(m_Function.Function->GetName()).compare("neq") == 0)
              {
                  op = "!=";
                  opname = "not equal to";
                  opinv = false;

              }else if(QString(m_Function.Function->GetName()).compare("neq_r") == 0)
              {
                    op = "!=";
                    opname = "not equal to";
                    opinv = true;

              }else {

                  op = m_ScriptManager->m_Engine->GetOperatorFromMemberFunctionName(m_Function.Function->GetName());
                  opname = m_ScriptManager->m_Engine->GetOperatorNameFromMemberFunctionName(m_Function.Function->GetName());
                  opinv = m_ScriptManager->m_Engine->GetOperatorIsReverseFromMemberFunctionName(m_Function.Function->GetName());
              }

              if(!op.isEmpty())
              {

                  QString object2name;
                  if(paramcount > 0)
                  {
                      int id = 0;
                      m_Function.Function->GetParam(0,&id);
                      object2name = m_ScriptManager->m_Engine->GetTypeName(id);
                  }

                  if(opinv)
                  {
                      return m_Function.ObjectName + " " + op + " " + object2name + "(" + opname + ")";
                  }else {
                      return object2name + " " + op + " " +m_Function.ObjectName +  "(" + opname + ")";
                    }
              }else {
                  return m_Function.ObjectName + QString("::") + QString(m_Function.Function->GetName()) + parameters;
              }
          }else {
              return QString(m_Function.Function->GetName())+ parameters;
         }
      }
  }


  inline unsigned int nPorts(QtNodes::PortType portType) const override
  {
      unsigned int result = 0;

      if (portType == QtNodes::PortType::In)
      {
          if(m_Function.is_basetypeoperator)
          {
               if(m_Function.basetypeoperatorparametertype.isEmpty())
               {
                   return 1;
               }else {
                   return 2;
               }

          }else
          {
              if(!(m_Function.is_member))
              {
                  result = m_Function.Function->GetParamCount();
              }else {
                result = m_Function.Function->GetParamCount() + 1;
              }
          }
      }
      else
      {
          if(m_Function.is_basetypeoperator)
          {
            return 1;

          }else
          {
              if(m_Function.is_valueobjectconstructor)
              {
                  return 1;

              }else {

                  if(m_Function.is_member)
                  {
                      int typeId =m_Function.Function->GetReturnTypeId();
                      if(typeId == asTYPEID_VOID)
                      {
                          return 1;
                      }else {
                          return 2;
                      }
                  }else {
                      int typeId =m_Function.Function->GetReturnTypeId();
                      if(typeId == asTYPEID_VOID)
                      {
                          return 0;
                      }else {
                          return 1;
                      }

                  }


              }
          }

      }
      return result;
  }

  inline QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override
  {

      switch (portType)
      {
        case QtNodes::PortType::In:
              {
                  if(m_Function.is_basetypeoperator)
                  {
                      if(portIndex == 0)
                      {
                            return GetDataFromTypeName(m_Function.ObjectName);
                      }else {
                            return GetDataFromTypeName(m_Function.basetypeoperatorparametertype);
                        }

                  }else {

                      if(m_Function.is_member)
                      {
                          if(portIndex == 0)
                          {
                              return GetDataFromTypeName(m_ScriptManager->m_Engine->GetUITypeName( m_Function.ObjectType->GetTypeId()));
                          }else
                          {
                              QString name;
                              if(portIndex-1 < m_Function.Parameters.length())
                              {
                                  name =m_Function.Parameters.at(portIndex-1);
                              }
                              int tid = 0;
                              m_Function.Function->GetParam(portIndex-1, &tid);
                              return GetDataFromTypeName(m_ScriptManager->m_Engine->GetUITypeName(tid));
                          }


                      }else if(m_Function.is_constructor)
                      {
                          QString name;
                          if(portIndex < m_Function.Parameters.length())
                          {
                              name =m_Function.Parameters.at(portIndex);
                          }
                          int tid = 0;
                          m_Function.Function->GetParam(portIndex, &tid);
                          return GetDataFromTypeName(m_ScriptManager->m_Engine->GetUITypeName(tid));

                      }else
                      {
                          QString name;
                          if(portIndex < m_Function.Parameters.length())
                          {
                              name =m_Function.Parameters.at(portIndex);
                          }
                          int tid = 0;
                          m_Function.Function->GetParam(portIndex, &tid);
                          return GetDataFromTypeName(m_ScriptManager->m_Engine->GetUITypeName(tid));
                      }
                  }

              }

        case QtNodes::PortType::Out:
              {
                  if(m_Function.is_basetypeoperator)
                  {
                      return GetDataFromTypeName(m_Function.basetypeoperatorreturntype);
                  }else {
                      if(m_Function.is_valueobjectconstructor)
                      {
                          return GetDataFromTypeName(m_Function.ObjectName);
                       }else {

                          if(m_Function.is_member)
                          {
                              if(portIndex == 0)
                              {
                                    return GetDataFromTypeName(m_ScriptManager->m_Engine->GetUITypeName( m_Function.ObjectType->GetTypeId()));

                              }else {
                                  int typeId = m_Function.Function->GetReturnTypeId();
                                  return GetDataFromTypeName(m_ScriptManager->m_Engine->GetUITypeName(typeId));

                              }

                          }else {
                              int typeId = m_Function.Function->GetReturnTypeId();
                              return GetDataFromTypeName(m_ScriptManager->m_Engine->GetUITypeName(typeId));
                          }

                    }
                  }
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
      if(!(m_Function.is_member || IsReturner()))
      {
        return nullptr;
      }
      return m_Widget;
  }

  inline virtual bool FirstOutputIsFirstInput()
  {
      if(m_Function.is_member)
      {
          return !m_CheckBox->isChecked();
      }

      return false;
  }

  inline virtual QList<QString> GetCodeLine(std::vector<QString> inputs,std::vector<QString> outputs, LSMScriptEngine * e) override
  {
      QList<QString> l;

      if(m_Function.is_basetypeoperator)
      {

          bool error = false;
          if(!((outputs.size() ==  1) ))
          {
              std::cout << "output parameters do not match parameter count A " << std::endl;
              error = true;
          }

          if(!((inputs.size() == 1 && m_Function.basetypeoperatorparametertype.isEmpty() )|| ((inputs.size() == 2 && !m_Function.basetypeoperatorparametertype.isEmpty() ))))
          {
              std::cout << "input parameters do not match parameter count B" << std::endl;
              error = true;
          }

          if(!error)
          {
              if(m_Function.basetypeoperatorparametertype.isEmpty())
              {
                  QString op = m_ScriptManager->m_Engine->GetOperatorFromMemberFunctionName(m_Function.basetypeoperatorname);

                  QString line;
                  line = m_Function.basetypeoperatorreturntype + " " + outputs.at(0) + " = " + op +  inputs.at(0)+ ";";

                  l.append(line);
              }else {


                  QString op = m_ScriptManager->m_Engine->GetOperatorFromMemberFunctionName(m_Function.basetypeoperatorname);

                  QString line;
                  line = m_Function.basetypeoperatorreturntype + " " + outputs.at(0) + " = " + inputs.at(0)  + op + inputs.at(1) + ";";

                  l.append(line);

                }
          }
      }else {



          //check consistencty
          //returning an empty stringlist indicates an error occured
          bool error = false;

          int r_typeId =m_Function.Function->GetReturnTypeId();
          if(!m_Function.is_valueobjectconstructor)
          {
              int add = 0;
              int typeId =m_Function.Function->GetReturnTypeId();
              if(typeId == asTYPEID_VOID)
              {
                  add = 0;
              }else {
                  add = 1;
              }

              if(m_Function.is_member)
              {

                  if(outputs.size() != 1 + add)
                  {


                      std::cout << "output parameters do not match parameter count E" << std::endl;
                      error = true;
                  }
              }else {
                  if(outputs.size() != 0 + add)
                  {
                      std::cout << "output parameters do not match parameter count F" << std::endl;
                      error = true;
                  }

              }
          }else {
              if(outputs.size() != 1)
              {
                  std::cout << "output parameters do not match parameter count D" << std::endl;
                  error = true;
              }

            }
          int paramcount = m_Function.Function->GetParamCount() ;

          if(!((paramcount+ (m_Function.is_member?  1:0)) == inputs.size()))
          {
              std::cout << "input parameters do not match parameter count " << inputs.size() << "  " << paramcount << " " << paramcount+ (m_Function.is_member?  1:0) << std::endl;
              error = true;
          }

          if(!error)
          {
              if(m_Function.is_member)
              {
                  QString op = m_ScriptManager->m_Engine->GetOperatorFromMemberFunctionName(m_Function.Function->GetName());
                  QString opname = m_ScriptManager->m_Engine->GetOperatorNameFromMemberFunctionName(m_Function.Function->GetName());
                  bool opinv = m_ScriptManager->m_Engine->GetOperatorIsReverseFromMemberFunctionName(m_Function.Function->GetName());

                  if(!op.isEmpty())
                  {

                      std::cout << "operator code " << paramcount << " " << inputs.size() << std::endl;
                      //use the operator notation
                      //Unary operator (such as negate: -a)
                      if(paramcount == 0 && inputs.size() == 1)
                      {
                          QString line;
                          if(!(r_typeId == asTYPEID_VOID) && outputs.size() > 0)
                          {
                                line += e->GetTypeName(r_typeId) + " " + outputs.at(0) + " = ";
                          }

                          //use object name as function name (since it is a constructor)
                          line += op + inputs.at(0);
                          line += ";";

                          l.append(line);
                      }else if(inputs.size() == 2){ //binary operator (addition: a + b)

                          QString line;
                          if(!(r_typeId == asTYPEID_VOID) && outputs.size() > 0)
                          {
                                line += e->GetTypeName(r_typeId) + " " + outputs.at(0) + " = ";
                          }

                          QString obj1 = inputs.at(0);
                          QString obj2 = inputs.at(1);

                          //use object name as function name (since it is a constructor)
                          line += opinv? obj1 + op + obj2: obj2 + op + obj1;
                          line += ";";

                          l.append(line);
                      }

                  }else {

                      //use normal member function notation
                      //make the code
                      QString line;
                      QString line2;
                      if(!(r_typeId == asTYPEID_VOID) && outputs.size() > 0 && inputs.size() > 0)
                      {
                          QString outputobjname;
                          if(!m_Function.is_valueobjectconstructor)
                          {
                              outputobjname = e->GetTypeName(r_typeId);
                          }else {
                              outputobjname = m_Function.ObjectName;
                          }
                            if(m_CheckBox->isChecked())
                            {
                                line += outputobjname + " " + outputs.at(0) + " = " +  inputs.at(0) + ";";
                                l.append(line);
                            }

                            line2 += e->GetTypeName(r_typeId) + " " + outputs.at(1) + " = " + outputs.at(0) + "." + QString(m_Function.Function->GetName()) + "(";

                            for(int i = 0; i < inputs.size()-1; i++)
                            {
                                line2 += inputs.at(i+1);
                                if(i != inputs.size()-2)
                                {
                                    line2 += ",";
                                }
                            }
                            line2 += ");";

                            l.append(line2);

                      }

                    }
              }else if(m_Function.is_constructor)
              {
                  QString outputobjname;
                  if(!m_Function.is_valueobjectconstructor)
                  {
                      outputobjname = e->GetTypeName(r_typeId);
                  }else {
                      outputobjname = m_Function.ObjectName;
                  }

                  //use constructor notation
                  QString line;
                  if(!(r_typeId == asTYPEID_VOID) && outputs.size() > 0)
                  {
                        line += outputobjname + " " + outputs.at(0) + " = ";
                  }

                  //use object name as function name (since it is a constructor)
                  line += QString(m_Function.ObjectName) + "(";

                  for(int i = 0; i < paramcount; i++)
                  {
                      line += inputs.at(i);
                      if(i != paramcount-1)
                      {
                          line += ",";
                      }
                  }
                  line += ");";

                  l.append(line);

              }else {
                  //make the code
                  QString line;
                  if(!(r_typeId == asTYPEID_VOID) && outputs.size() > 0)
                  {
                        line += e->GetTypeName(r_typeId) + " " + outputs.at(0) + " = ";
                  }

                  line += QString(m_Function.Function->GetName()) + "(";

                  for(int i = 0; i < paramcount; i++)
                  {
                      line += inputs.at(i);
                      if(i != paramcount-1)
                      {
                          line += ",";
                      }
                  }
                  line += ");";

                  l.append(line);
              }

          }



      }
      return l;
  }

  inline QJsonObject
  save() const
  {
    QJsonObject modelJson = QtNodes::NodeDataModel::save();

    if(m_Function.is_member)
    {
        modelJson["duplicate"] = m_CheckBox->isChecked();
    }else
    {
        modelJson["duplicate"] = false;
    }

    return modelJson;
  }

  inline void
  restore(QJsonObject const &p)
  {
       if(m_Function.is_member)
       {
           QJsonValue checked = p["duplicate"];
            if(checked.isUndefined() || checked.toBool(false))
            {
                m_CheckBox->setChecked(false);
            }else {
                m_CheckBox->setChecked(true);
            }
       }


  }


  inline virtual bool IsReturner() override
  {
      QString rtype = dataType(QtNodes::PortType::Out,0).name;
      int nout = nPorts(QtNodes::PortType::Out);

      if(nout == 1)
      {
        if(rtype == "int" || rtype == "float" || rtype == "double" || rtype == "bool" || rtype == "string" || rtype == "Map")
        {
            bool ok = true;
            int nin = nPorts(QtNodes::PortType::In);
            for(int i = 0; i < nin; i++)
            {
                QString itype = dataType(QtNodes::PortType::Out,0).name;
                if(!((itype == "int" || itype == "float" || itype == "double" || itype == "bool" || itype == "string" || itype == "Map")))
                {
                    return false;
                }
            }

            if(ok)
            {
                return true;
            }
        }
      }
      return false;
  }

  inline virtual void ClearPreview() override
  {
      std::cout << "Clear node " << name().toStdString() << std::endl;

      QString rtype = dataType(QtNodes::PortType::Out,0).name;

      if(rtype == "int")
      {
          m_Label->setText("No preview available");
      }else if(rtype == "float")
      {
          m_Label->setText("No preview available");
      }else if(rtype == "double")
      {
          m_Label->setText("No preview available");
      }else if(rtype == "bool")
      {
          m_Label->setText("No preview available");
      }else if(rtype == "string")
      {
          m_Label->setText("No preview available");
      }else if(rtype == "Map")
      {
          if(m_Map != nullptr)
          {
              m_Map = nullptr;

          }
          m_IsReturnError = false;

          QImage i(100,100,QImage::Format_RGB888);
          i.fill(qRgba(25,25,25,255.0));

          m_ImageViewer->SetToolTipDefault();
          m_ImageViewer->setImage(i);
          m_SaveButton->setEnabled(false);

      }

      PreviewArgument p;
      p.isvalid = false;
      m_ReturnData = p;


   return;
  }

  inline virtual void SetPreviewData(PreviewArgument data) override
  {
      if(data.isvalid)
      {
          SetExternalDataChangeFlag(true);
      }

      QString rtype = dataType(QtNodes::PortType::Out,0).name;

      std::cout << " set preview data for map function " << name().toStdString() << " " <<  rtype.toStdString() <<  data.isvalid <<  std::endl;


      m_ReturnData = data;

      if(rtype == "int")
      {
        m_Label->setText(QString::number(m_ReturnData.xi));

      }else if(rtype == "float")
      {
        m_Label->setText(QString::number(m_ReturnData.xd));
      }else if(rtype == "double")
      {
        m_Label->setText(QString::number(m_ReturnData.xd));
      }else if(rtype == "bool")
      {
          m_Label->setText(m_ReturnData.xb?"True":"False");

      }else if(rtype == "string")
      {
          m_Label->setText(*((QString*)std::static_pointer_cast<QString>(m_ReturnData.xm).get()));
      }else if(rtype == "Map")
      {
          if(data.type == "Map" && data.type != nullptr)
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
              m_SaveButton->setEnabled(true);

          }else
          {
                m_IsReturnError = true;
          }
      }
      return;
  }

  inline virtual void SetPreviewIsLoading() override
  {

      QString rtype = dataType(QtNodes::PortType::Out,0).name;

      if(rtype == "Map")
      {
          QIcon mLoadIcon =*(GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_RELOAD),-1));
          QImage i = mLoadIcon.pixmap(QSize(255, 255)).toImage();
          m_ImageViewer->setImage(i);
      }


  }




  inline virtual PreviewArgument GetPreviewData() override
  {
      QString rtype = dataType(QtNodes::PortType::Out,0).name;

      std::cout << "get preview data from function model " << std::endl;
      if(rtype == "int")
      {

          return m_ReturnData;
      }else if(rtype == "float")
      {

          return m_ReturnData;
      }else if(rtype == "double")
      {

          return m_ReturnData;
      }else if(rtype == "bool")
      {

          return m_ReturnData;
      }else if(rtype == "string")
      {

          return m_ReturnData;
      }else if(rtype == "Map")
      {
            std::cout << "reutrn map " << m_ReturnData.xm.get() << std::endl;
          return m_ReturnData;
      }


      return PreviewArgument();
  }


  inline virtual bool IsInternalDataForPreviewValid() override
  {
      return true;

  }

  inline virtual void ClearError()
  {
      m_IsReturnError = false;
      SetExternalDataChangeFlag(true);
  }


  inline virtual bool IsPreviewError() override
  {
      return m_IsReturnError;
  }

  inline virtual bool IsPreviewAvailable() override
  {
      return m_ReturnData.isvalid;
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

};


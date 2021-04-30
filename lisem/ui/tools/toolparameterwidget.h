#ifndef TOOLPARAMETERWIDGET_H
#define TOOLPARAMETERWIDGET_H

#include "QWidget"
#include "lsmscriptengine.h"
#include "QHBoxLayout"
#include "QLineEdit"
#include "QToolButton"
#include "QSpinBox"
#include "QDoubleSpinBox"
#include "widgets/switchbutton.h"
#include "QFileDialog"
#include "iogdal.h"
#include "lsmio.h"
#include "qmessagebox.h"
#include "widgets/labeledwidget.h"
#include "QKeyEvent"
#include "QClipboard"
#include "widgets/crsselectionwidget.h"
#include "site.h"

class ParameterSelectionWidget : public QWidget
{
Q_OBJECT;

public:

    virtual QString GetValueAsString()
    {

        return "";
    }

signals:

    void OnValueChanged();
};



class BoolSelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:

    bool m_Value = false;

    SwitchButton * m_SwitchButton;

    inline BoolSelectWidget(QString DefaultValue)
    {

        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_SwitchButton = new SwitchButton();
        m_Layout->addWidget(m_SwitchButton);

        connect(m_SwitchButton,&SwitchButton::valueChanged,this, &BoolSelectWidget::ValueChanged);
    }
    inline void SetValue(bool x)
    {

    }
    inline bool GetValue()
    {

        return "";
    }

    inline QString GetValueAsString()
    {
        return m_Value? "true":"false";
    }
public slots:

    void ValueChanged(bool)
    {
        emit OnValueChanged();
    }
};

class StringSelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:

    QString m_Value;

    QLineEdit * m_LineEdit;

    inline StringSelectWidget(QString DefaultValue)
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_LineEdit = new QLineEdit();
        m_Layout->addWidget(m_LineEdit);

        connect(m_LineEdit,&QLineEdit::textChanged,this,&StringSelectWidget::TextChanged);
    }
    inline void SetValue(QString x)
    {

    }
    inline QString GetValue()
    {

        return "";
    }

    inline QString GetValueAsString()
    {
        return m_LineEdit->text();
    }
public slots:

    void TextChanged(QString)
    {
        emit OnValueChanged();
    }
};

class DoubleSelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:

    bool m_Value = false;

    QDoubleSpinBox * m_DoubleSpinBox;

    inline DoubleSelectWidget(QString DefaultValue)
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_DoubleSpinBox = new QDoubleSpinBox();
        m_Layout->addWidget(m_DoubleSpinBox);

        connect(m_DoubleSpinBox, SIGNAL(valueChanged(double)),this,SLOT(ValueChanged(double)));
    }
    inline void SetValue(QString x)
    {

    }
    inline QString GetValue()
    {

        return "";
    }

    inline QString GetValueAsString()
    {
        return QString::number(m_DoubleSpinBox->value());
    }
public slots:

    void ValueChanged(double)
    {
        emit OnValueChanged();
    }

};

class FloatSelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:

    bool m_Value = false;

    QDoubleSpinBox * m_DoubleSpinBox;

    inline FloatSelectWidget(QString DefaultValue)
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_DoubleSpinBox = new QDoubleSpinBox();
        m_Layout->addWidget(m_DoubleSpinBox);


        connect(m_DoubleSpinBox, SIGNAL(valueChanged(double)),this,SLOT(ValueChanged(double)));
         }
    inline void SetValue(QString x)
    {

    }
    inline QString GetValue()
    {

        return "";
    }

    inline QString GetValueAsString()
    {
        return QString::number(m_DoubleSpinBox->value());
    }

public slots:

    void ValueChanged(double)
    {
        emit OnValueChanged();
    }

};

class Int8SelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:

    bool m_Value = false;

    QSpinBox * m_SpinBox;
    inline Int8SelectWidget(QString DefaultValue)
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_SpinBox = new QSpinBox();
        m_Layout->addWidget(m_SpinBox);

        connect(m_SpinBox, SIGNAL(valueChanged(int)),this,SLOT(ValueChanged(int)));
    }
    inline void SetValue(QString x)
    {

    }
    inline QString GetValue()
    {

        return "";
    }

    inline QString GetValueAsString()
    {
        return QString::number(m_SpinBox->value());
    }

public slots:

    void ValueChanged(int)
    {
        emit OnValueChanged();
    }
};

class Int16SelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:

    bool m_Value = false;

    QSpinBox * m_SpinBox;
    inline Int16SelectWidget(QString DefaultValue)
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_SpinBox = new QSpinBox();
        m_Layout->addWidget(m_SpinBox);

        connect(m_SpinBox, SIGNAL(valueChanged(int)),this,SLOT(ValueChanged(int)));
    }
    inline void SetValue(QString x)
    {

    }
    inline QString GetValue()
    {

        return "";
    }

    inline QString GetValueAsString()
    {
        return QString::number(m_SpinBox->value());
    }

public slots:

    void ValueChanged(int)
    {
        emit OnValueChanged();
    }
};
class Int32SelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:

    bool m_Value = false;

    QSpinBox * m_SpinBox;
    inline Int32SelectWidget(QString DefaultValue)
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_SpinBox = new QSpinBox();
        m_Layout->addWidget(m_SpinBox);
        connect(m_SpinBox, SIGNAL(valueChanged(int)),this,SLOT(ValueChanged(int)));
    }
    inline void SetValue(QString x)
    {

    }
    inline QString GetValue()
    {

        return "";
    }

    inline QString GetValueAsString()
    {
        return QString::number(m_SpinBox->value());
    }

public slots:

    void ValueChanged(int)
    {
        emit OnValueChanged();
    }
};
class Int64SelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:

    bool m_Value = false;

    QSpinBox * m_SpinBox;
    inline Int64SelectWidget(QString DefaultValue)
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_SpinBox = new QSpinBox();
        m_Layout->addWidget(m_SpinBox);
        connect(m_SpinBox, SIGNAL(valueChanged(int)),this,SLOT(ValueChanged(int)));
    }
    inline void SetValue(QString x)
    {

    }
    inline QString GetValue()
    {

        return "";
    }

    inline QString GetValueAsString()
    {
        return QString::number(m_SpinBox->value());
    }

public slots:

    void ValueChanged(int)
    {
        emit OnValueChanged();
    }
};

class UInt8SelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:

    bool m_Value = false;

    QSpinBox * m_SpinBox;
    inline UInt8SelectWidget(QString DefaultValue)
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_SpinBox = new QSpinBox();
        m_Layout->addWidget(m_SpinBox);
        connect(m_SpinBox, SIGNAL(valueChanged(int)),this,SLOT(ValueChanged(int)));
    }
    inline void SetValue(QString x)
    {

    }
    inline QString GetValue()
    {

        return "";
    }

    inline QString GetValueAsString()
    {
        return QString::number(m_SpinBox->value());
    }

public slots:

    void ValueChanged(int)
    {
        emit OnValueChanged();
    }
};

class UInt16SelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:

    bool m_Value = false;

    QSpinBox * m_SpinBox;
    inline UInt16SelectWidget(QString DefaultValue)
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_SpinBox = new QSpinBox();
        m_Layout->addWidget(m_SpinBox);
        connect(m_SpinBox, SIGNAL(valueChanged(int)),this,SLOT(ValueChanged(int)));
    }
    inline void SetValue(QString x)
    {


    }
    inline QString GetValue()
    {

        return "";
    }

    inline QString GetValueAsString()
    {
        return QString::number(m_SpinBox->value());
    }

public slots:

    void ValueChanged(int)
    {
        emit OnValueChanged();
    }
};
class UInt32SelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:

    bool m_Value = false;

    QSpinBox * m_SpinBox;
    inline UInt32SelectWidget(QString DefaultValue)
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_SpinBox = new QSpinBox();
        m_Layout->addWidget(m_SpinBox);
        connect(m_SpinBox, SIGNAL(valueChanged(int)),this,SLOT(ValueChanged(int)));
    }
    inline void SetValue(QString x)
    {

    }
    inline QString GetValue()
    {

        return "";
    }

    inline QString GetValueAsString()
    {
        return QString::number(m_SpinBox->value());
    }

public slots:

    void ValueChanged(int)
    {
        emit OnValueChanged();
    }
};
class UInt64SelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:

    bool m_Value = false;

    QSpinBox * m_SpinBox;
    inline UInt64SelectWidget(QString DefaultValue)
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_SpinBox = new QSpinBox();
        m_Layout->addWidget(m_SpinBox);
        connect(m_SpinBox, SIGNAL(valueChanged(int)),this,SLOT(ValueChanged(int)));
    }
    inline void SetValue(QString x)
    {

    }
    inline QString GetValue()
    {

        return "";
    }

    inline QString GetValueAsString()
    {
        return QString::number(m_SpinBox->value());
    }

public slots:

    void ValueChanged(int)
    {
        emit OnValueChanged();
    }
};



class GeoProjectionSelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:

    bool m_Value = false;

    QLineEdit * m_LineEdit;
    QToolButton * m_ProjectionButton;
    inline GeoProjectionSelectWidget(QString DefaultValue = "")
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_LineEdit = new QLineEdit();
        m_LineEdit->setText("Generic");

        m_ProjectionButton = new QToolButton();


        QString m_Dir = GetSite();


        QIcon iconCRS;
        iconCRS.addFile((m_Dir + LISEM_FOLDER_ASSETS + "globe.png"), QSize(), QIcon::Normal, QIcon::Off);


        QToolButton *CRSButton = new QToolButton();
        CRSButton->setIcon(iconCRS);
        CRSButton->setIconSize(QSize(22,22));
        CRSButton->resize(22,22);
        CRSButton->setEnabled(true);

        m_Layout->addWidget(new QWidgetHDuo(m_LineEdit,CRSButton));


        connect(CRSButton,SIGNAL(clicked(bool)), this, SLOT(OnSelectCRS()));


    }
    inline void SetValue(QString x)
    {
        m_LineEdit->setText(x);
    }
    inline QString GetValue()
    {

        return m_LineEdit->text();
    }

    inline QString GetValueAsString()
    {
         return m_LineEdit->text();
    }
public slots:

    void OnSelectCRS()
    {
        CRSSelectWidget * select = new CRSSelectWidget();
        select->SetCurrentCRS(GeoProjection::FromString(m_LineEdit->text()));
        int res = select->exec();

        if(res == QDialog::Accepted){
               GeoProjection p = select->GetCurrentProjection();
               m_LineEdit->setText(p.GetWKT());

        }

        delete select;
    }
};

class ShapeFileSelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:

    bool m_IsFile = false;
    bool m_IsConstant = false;
    bool m_IsLayer = false;


    QString dir = "";

    QLineEdit * m_LineEdit;
    QToolButton * m_LoadFromFile;
    QToolButton * m_LoadFromViewLayer;

    inline ShapeFileSelectWidget(QString DefaultValue = "")
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_LineEdit = new QLineEdit();
        m_Layout->addWidget(m_LineEdit);
        m_LoadFromFile = new QToolButton();
        m_LoadFromViewLayer = new QToolButton();

        QString m_Dir = GetSite();
        QIcon *icon_open = new QIcon();
        icon_open->addFile((m_Dir + LISEM_FOLDER_ASSETS + "fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);
        m_LoadFromFile->setIcon(*icon_open);

        m_Layout->addWidget(m_LoadFromFile);
        m_Layout->addWidget(m_LoadFromViewLayer);

        connect(m_LoadFromFile,&QToolButton::clicked,this,&ShapeFileSelectWidget::LoadFileClicked);
        connect(m_LoadFromFile,&QToolButton::clicked,this,&ShapeFileSelectWidget::LoadViewClicked);
    }
    inline void SetValue(QString x)
    {

    }
    inline QString GetValue()
    {

        return "";
    }

    inline QString GetValueAsString()
    {
        return m_LineEdit->text();
    }
public slots:
    void LoadFileClicked(bool)
    {

        QString path = QFileDialog::getOpenFileName(this,	QString("Select the shapefile:"),
                                                    dir,QString("*.shp ;;*.*"));
        // open file dialog


        if (!path.isEmpty())// && QFileInfo(path).exists())
        {
           /*if(!rasterCanBeOpenedForReading(path)) {
              QMessageBox::critical(this, "SPHazard",
                 QString("File \"%1\" is not a supported raster map.")
                     .arg(path));
              return;
           }*/

           QString f = QFileInfo(path).fileName();
           QString p = QFileInfo(path).dir().path();
           dir = p;

           m_LineEdit->setText("LoadVectorAbsPath(\"" + path + "\")");

           OnValueChanged();
        }
    }

    void LoadViewClicked(bool)
    {

    }


};

class ShapeFileSaveSelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:

    bool m_IsFile = false;
    bool m_IsConstant = false;
    bool m_IsLayer = false;


    QString dir = "";

    QLineEdit * m_LineEdit;
    QToolButton * m_LoadFromFile;
    QToolButton * m_LoadFromViewLayer;

    inline ShapeFileSaveSelectWidget(QString DefaultValue = "")
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_LineEdit = new QLineEdit();
        m_Layout->addWidget(m_LineEdit);
        m_LoadFromFile = new QToolButton();
        m_LoadFromViewLayer = new QToolButton();

        QString m_Dir = GetSite();
        QIcon *icon_open = new QIcon();
        icon_open->addFile((m_Dir + LISEM_FOLDER_ASSETS + "fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);
        m_LoadFromFile->setIcon(*icon_open);

        m_Layout->addWidget(m_LoadFromFile);

        connect(m_LoadFromFile,&QToolButton::clicked,this,&ShapeFileSaveSelectWidget::LoadFileClicked);
    }
    inline void SetValue(QString x)
    {

    }
    inline QString GetValue()
    {

        return "";
    }

    inline QString GetValueAsString()
    {
        return m_LineEdit->text();
    }
public slots:
    void LoadFileClicked(bool)
    {

        QString path = QFileDialog::getOpenFileName(this,	QString("Select the shapefile:"),
                                                    dir,QString("*.shp;;*.*"));
        // open file dialog


        if (!path.isEmpty())// && QFileInfo(path).exists())
        {

           QString f = QFileInfo(path).fileName();
           QString p = QFileInfo(path).dir().path();
           dir = p;

           m_LineEdit->setText("" + path + "");

           OnValueChanged();
        }
    }

    void LoadViewClicked(bool)
    {

    }


};

class MapSelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:


    bool m_IsFile = false;
    bool m_IsConstant = false;
    bool m_IsLayer = false;

    QString dir = "";

    QLineEdit * m_LineEdit;

    QToolButton * m_LoadFromFile;
    QToolButton * m_LoadFromViewLayer;
    QToolButton * m_LoadFromConstant;
    inline MapSelectWidget(QString DefaultValue= "")
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_LineEdit = new QLineEdit();
        m_LineEdit->setText("");
        m_Layout->addWidget(m_LineEdit);
        m_LoadFromFile = new QToolButton();
        m_LoadFromViewLayer = new QToolButton();
        m_LoadFromConstant = new QToolButton();

        QString m_Dir = GetSite();
        QIcon *icon_open = new QIcon();
        icon_open->addFile((m_Dir + LISEM_FOLDER_ASSETS + "fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);
        QIcon *icon_new = new QIcon();
        icon_new->addFile((m_Dir + LISEM_FOLDER_ASSETS + "display.png"), QSize(), QIcon::Normal, QIcon::Off);

        QIcon *icon_mapnew = new QIcon();
        icon_new->addFile((m_Dir + LISEM_FOLDER_ASSETS + "rasternew.png"), QSize(), QIcon::Normal, QIcon::Off);

        m_LoadFromFile->setIcon(*icon_open);
        m_LoadFromConstant->setIcon(*icon_new);
        m_LoadFromViewLayer->setIcon(*icon_mapnew);

        m_Layout->addWidget(m_LoadFromFile);
        m_Layout->addWidget(m_LoadFromViewLayer);
        m_Layout->addWidget(m_LoadFromConstant);

        connect(m_LoadFromFile,&QToolButton::clicked,this,&MapSelectWidget::LoadFileClicked);
        connect(m_LoadFromViewLayer,&QToolButton::clicked,this,&MapSelectWidget::LoadViewClicked);
        connect(m_LoadFromConstant,&QToolButton::clicked,this,&MapSelectWidget::LoadConstantClicked);
    }
    inline void SetValue(QString x)
    {

        m_LineEdit->setText(x);
    }
    inline QString GetValue()
    {

        return "";
    }

    inline QString GetValueAsString()
    {
        return m_LineEdit->text();
    }

public slots:
    void LoadFileClicked(bool)
    {

        QString path = QFileDialog::getOpenFileName(this,	QString("Select the map:"),
                                                    dir,QString("*.map *.tif *.csf;;*.*"));
        // open file dialog


        if (!path.isEmpty())// && QFileInfo(path).exists())
        {
           if(!rasterCanBeOpenedForReading(path)) {
              QMessageBox::critical(this, "SPHazard",
                 QString("File \"%1\" is not a supported raster map.")
                     .arg(path));
              return;
           }

           QString f = QFileInfo(path).fileName();
           QString p = QFileInfo(path).dir().path();
           dir = p;

           m_LineEdit->setText("LoadMapAbsPath(\"" + path + "\")");

           OnValueChanged();
        }
    }

    void LoadViewClicked(bool);
    void LoadConstantClicked(bool);
};

class MapSaveSelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:


    bool m_IsFile = false;
    bool m_IsConstant = false;
    bool m_IsLayer = false;

    QString dir = "";

    QLineEdit * m_LineEdit;

    QToolButton * m_LoadFromFile;
    inline MapSaveSelectWidget(QString DefaultValue= "")
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_LineEdit = new QLineEdit();
        m_LineEdit->setText("");
        m_Layout->addWidget(m_LineEdit);
        m_LoadFromFile = new QToolButton();

        QString m_Dir = GetSite();
        QIcon *icon_open = new QIcon();
        icon_open->addFile((m_Dir + LISEM_FOLDER_ASSETS + "fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);
        QIcon *icon_new = new QIcon();
        icon_new->addFile((m_Dir + LISEM_FOLDER_ASSETS + "new.png"), QSize(), QIcon::Normal, QIcon::Off);

        m_LoadFromFile->setIcon(*icon_open);

        m_Layout->addWidget(m_LoadFromFile);

        connect(m_LoadFromFile,&QToolButton::clicked,this,&MapSaveSelectWidget::LoadFileClicked);

    }
    inline void SetValue(QString x)
    {
        m_LineEdit->setText(x);
    }
    inline QString GetValue()
    {
        return "";
    }

    inline QString GetValueAsString()
    {
        return m_LineEdit->text();
    }

public slots:
    void LoadFileClicked(bool)
    {

        QString path = QFileDialog::getSaveFileName(this,	QString("Select the map:"),
                                                    dir,QString("*.map *.tif *.csf;;*.*"));
        // open file dialog


        if (!path.isEmpty())// && QFileInfo(path).exists())
        {


           QString f = QFileInfo(path).fileName();
           QString p = QFileInfo(path).dir().path();
           dir = p;

           m_LineEdit->setText("" + path + "");

           OnValueChanged();
        }
    }
};
class TableSelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:

    bool m_Value = false;

    QToolButton * m_LoadFromFile;
    QToolButton * m_LoadFromConstant;
    QLineEdit * m_LineEdit;
    inline TableSelectWidget(QString DefaultValue = "")
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_LineEdit = new QLineEdit();
        m_Layout->addWidget(m_LineEdit);
    }
    inline void SetValue(QString x)
    {

    }
    inline QString GetValue()
    {

        return "";
    }

    inline QString GetValueAsString()
    {
        return "Table";
    }


};

class PointCloudSelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:

    bool m_Value = false;

    QToolButton * m_LoadFromFile;
    QToolButton * m_LoadFromConstant;
    QLineEdit * m_LineEdit;
    inline PointCloudSelectWidget(QString DefaultValue = "")
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_LineEdit = new QLineEdit();
        m_Layout->addWidget(m_LineEdit);
    }
    inline void SetValue(QString x)
    {

    }
    inline QString GetValue()
    {

        return "";
    }

    inline QString GetValueAsString()
    {
        return "PointCloud";
    }


};

class MapArraySelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:

    bool m_Value = false;

    QToolButton * m_LoadFromFile;
    QToolButton * m_LoadFromConstant;
    QLineEdit * m_LineEdit;
    inline MapArraySelectWidget(QString DefaultValue = "")
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_LineEdit = new QLineEdit();
        m_Layout->addWidget(m_LineEdit);
    }
    inline void SetValue(QString x)
    {

    }
    inline QString GetValue()
    {

        return "";
    }

    inline QString GetValueAsString()
    {
        return "MapArray";
    }


};

class SpinBox : public QDoubleSpinBox
{
    Q_OBJECT;

public:
    SpinBox(QWidget *parent = 0) : QDoubleSpinBox(parent) { }

protected:
    // reimplement keyPressEvent
    void keyPressEvent(QKeyEvent *event)
    {
        if(event->matches(QKeySequence::Paste))
        {
            QClipboard *clipboard = QApplication::clipboard();
            QString originalText = clipboard->text();

            bool ok = true;
            originalText.toDouble(&ok);
            if(!ok)
            {
                std::cout << "emit text " << originalText.toStdString()<<std::endl;
                emit OnTextPasted(originalText);
                return;
            }

        }
        QDoubleSpinBox::keyPressEvent(event);
    }

signals:

    void OnTextPasted(QString text);
};

class BoundingBoxSelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:

    bool m_Value = false;

    QToolButton * m_LoadFromFile;
    QToolButton * m_LoadFromConstant;
    SpinBox * m_MinX;
    SpinBox * m_MaxX;
    SpinBox * m_MinY;
    SpinBox * m_MaxY;

    inline BoundingBoxSelectWidget(QString DefaultValue = "")
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_MinX = new SpinBox();
        m_MaxX = new SpinBox();
        m_MinY = new SpinBox();
        m_MaxY = new SpinBox();

        m_MinX->setRange(-1e10,1e10);
        m_MaxX->setRange(-1e10,1e10);
        m_MinY->setRange(-1e10,1e10);
        m_MaxY->setRange(-1e10,1e10);

        m_Layout->addWidget(new  QWidgetHDuo(new QLabel("Min. X:"),m_MinX));
        m_Layout->addWidget(new  QWidgetHDuo(new QLabel("Max. X:"),m_MaxX));
        m_Layout->addWidget(new  QWidgetHDuo(new QLabel("Min. Y:"),m_MinY));
        m_Layout->addWidget(new  QWidgetHDuo(new QLabel("Max. Y:"),m_MaxY));

        //connect

        connect(m_MinX,&SpinBox::OnTextPasted,[this](QString text){

            if(text.startsWith("BoundingBox("))
            {

                std::cout << "received " << std::endl;

                text.remove(0,12);
                if(text.endsWith(")"))
                {
                        text.remove(text.length()-1,1);
                }
                QStringList l = text.split(",");
                std::cout << "length  " << l.length() << " " << l.at(3).toStdString()<< std::endl;
                if(l.length() == 4)
                {
                    bool ok = true;
                    double x1 = l.at(0).toDouble(&ok);
                    if(!ok){return;}
                    double x2 = l.at(1).toDouble(&ok);
                    if(!ok){return;}
                    double x3 = l.at(2).toDouble(&ok);
                    if(!ok){return;}
                    double x4 = l.at(3).toDouble(&ok);
                    if(!ok){return;}

                    std::cout << "set values" << std::endl;
                    m_MinX->setValue(x1);
                    m_MaxX->setValue(x2);
                    m_MinY->setValue(x3);
                    m_MaxY->setValue(x4);
                }
            }

            if(text.startsWith("Point("))
            {
                text.remove(0,6);
                if(text.endsWith(")"))
                {
                        text.remove(text.length()-1,1);
                }
                QStringList l = text.split(",");
                if(l.length() == 2)
                {
                    bool ok = true;;
                    double x1 = l.at(0).toDouble(&ok);
                    if(!ok){return;}
                    double x2 = l.at(1).toDouble(&ok);
                    if(!ok){return;}

                    m_MinX->setValue(x1);
                    m_MinY->setValue(x2);
                }
            }
        });
        connect(m_MaxX,&SpinBox::OnTextPasted,[this](QString text){
            if(text.startsWith("BoundingBox("))
            {
                text.remove(0,12);
                if(text.endsWith(")"))
                {
                        text.remove(text.length()-1,1);
                }
                QStringList l = text.split(",");
                if(l.length() == 4)
                {
                    bool ok = true;
                    double x1 = l.at(0).toDouble(&ok);
                    if(!ok){return;}
                    double x2 = l.at(1).toDouble(&ok);
                    if(!ok){return;}
                    double x3 = l.at(2).toDouble(&ok);
                    if(!ok){return;}
                    double x4 = l.at(3).toDouble(&ok);
                    if(!ok){return;}

                    m_MinX->setValue(x1);
                    m_MaxX->setValue(x2);
                    m_MinY->setValue(x3);
                    m_MaxY->setValue(x4);
                }
            }

            if(text.startsWith("Point("))
            {
                text.remove(0,6);
                if(text.endsWith(")"))
                {
                        text.remove(text.length()-1,1);
                }
                QStringList l = text.split(",");
                if(l.length() == 2)
                {
                    bool ok = true;;
                    double x1 = l.at(0).toDouble(&ok);
                    if(!ok){return;}
                    double x2 = l.at(1).toDouble(&ok);
                    if(!ok){return;}

                    m_MaxX->setValue(x1);
                    m_MaxY->setValue(x2);
                }
            }

        });
        connect(m_MinY,&SpinBox::OnTextPasted,[this](QString text){
            if(text.startsWith("BoundingBox("))
            {
                text.remove(0,12);
                if(text.endsWith(")"))
                {
                        text.remove(text.length()-1,1);
                }
                QStringList l = text.split(",");
                if(l.length() == 4)
                {
                    bool ok = true;
                    double x1 = l.at(0).toDouble(&ok);
                    if(!ok){return;}
                    double x2 = l.at(1).toDouble(&ok);
                    if(!ok){return;}
                    double x3 = l.at(2).toDouble(&ok);
                    if(!ok){return;}
                    double x4 = l.at(3).toDouble(&ok);
                    if(!ok){return;}

                    m_MinX->setValue(x1);
                    m_MaxX->setValue(x2);
                    m_MinY->setValue(x3);
                    m_MaxY->setValue(x4);
                }
            }

            if(text.startsWith("Point("))
            {
                text.remove(0,6);
                if(text.endsWith(")"))
                {
                        text.remove(text.length()-1,1);
                }
                QStringList l = text.split(",");
                if(l.length() == 2)
                {
                    bool ok = true;;
                    double x1 = l.at(0).toDouble(&ok);
                    if(!ok){return;}
                    double x2 = l.at(1).toDouble(&ok);
                    if(!ok){return;}

                    m_MinX->setValue(x1);
                    m_MinY->setValue(x2);
                }
            }

        });
        connect(m_MaxY,&SpinBox::OnTextPasted,[this](QString text){
            if(text.startsWith("BoundingBox("))
            {
                text.remove(0,12);
                if(text.endsWith(")"))
                {
                        text.remove(text.length()-1,1);
                }
                QStringList l = text.split(",");
                if(l.length() == 4)
                {
                    bool ok = true;
                    double x1 = l.at(0).toDouble(&ok);
                    if(!ok){return;}
                    double x2 = l.at(1).toDouble(&ok);
                    if(!ok){return;}
                    double x3 = l.at(2).toDouble(&ok);
                    if(!ok){return;}
                    double x4 = l.at(3).toDouble(&ok);
                    if(!ok){return;}

                    m_MinX->setValue(x1);
                    m_MaxX->setValue(x2);
                    m_MinY->setValue(x3);
                    m_MaxY->setValue(x4);
                }
            }

            if(text.startsWith("Point("))
            {
                text.remove(0,6);
                if(text.endsWith(")"))
                {
                        text.remove(text.length()-1,1);
                }
                QStringList l = text.split(",");
                if(l.length() == 2)
                {
                    bool ok = true;;
                    double x1 = l.at(0).toDouble(&ok);
                    if(!ok){return;}
                    double x2 = l.at(1).toDouble(&ok);
                    if(!ok){return;}

                    m_MaxX->setValue(x1);
                    m_MaxY->setValue(x2);
                }
            }

        });

    }
    inline void SetValue(QString text)
    {
        if(text.startsWith("BoundingBox("))
        {
            text.remove(0,12);
            if(text.endsWith(")"))
            {
                    text.remove(text.length()-1,1);
            }
            QStringList l = text.split(",");
            if(l.length() == 4)
            {
                bool ok = true;
                double x1 = l.at(0).toDouble(&ok);
                if(!ok){return;}
                double x2 = l.at(1).toDouble(&ok);
                if(!ok){return;}
                double x3 = l.at(2).toDouble(&ok);
                if(!ok){return;}
                double x4 = l.at(3).toDouble(&ok);
                if(!ok){return;}

                m_MinX->setValue(x1);
                m_MaxX->setValue(x2);
                m_MinY->setValue(x3);
                m_MaxY->setValue(x4);
            }
        }

    }

    inline BoundingBox GetBoundingBox()
    {
        return BoundingBox(m_MinX->value(),m_MaxX->value(),m_MinY->value(),m_MaxY->value());
    }
    inline QString GetValue()
    {

        return "BoundingBox(" + QString::number(m_MinX->value()) + "," +QString::number(m_MaxX->value())  + ","+QString::number(m_MinY->value())  + "," +QString::number(m_MaxY->value()) + ")";
    }

    inline QString GetValueAsString()
    {
        return "BoundingBox(" + QString::number(m_MinX->value()) + "," +QString::number(m_MaxX->value())  + ","+QString::number(m_MinY->value())  + "," +QString::number(m_MaxY->value()) + ")";
    }


};

class PointSelectWidget : public ParameterSelectionWidget
{
    Q_OBJECT;
public:

    bool m_Value = false;

    QToolButton * m_LoadFromFile;
    QToolButton * m_LoadFromConstant;
    SpinBox * m_MinX;
    SpinBox * m_MinY;

    inline PointSelectWidget(QString DefaultValue = "")
    {
        QHBoxLayout * m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);
        m_MinX = new SpinBox();
        m_MinY = new SpinBox();

        m_MinX->setRange(-1e10,1e10);
        m_MinY->setRange(-1e10,1e10);

        m_Layout->addWidget(new  QWidgetHDuo(new QLabel("X:"),m_MinX));
        m_Layout->addWidget(new  QWidgetHDuo(new QLabel("Y:"),m_MinY));

        //connect

        connect(m_MinX,&SpinBox::OnTextPasted,[this](QString text){

            if(text.startsWith("BoundingBox("))
            {

                std::cout << "received " << std::endl;

                text.remove(0,12);
                if(text.endsWith(")"))
                {
                        text.remove(text.length()-1,1);
                }
                QStringList l = text.split(",");
                std::cout << "length  " << l.length() << " " << l.at(3).toStdString()<< std::endl;
                if(l.length() == 4)
                {
                    bool ok = true;
                    double x1 = l.at(0).toDouble(&ok);
                    if(!ok){return;}
                    double x2 = l.at(1).toDouble(&ok);
                    if(!ok){return;}
                    double x3 = l.at(2).toDouble(&ok);
                    if(!ok){return;}
                    double x4 = l.at(3).toDouble(&ok);
                    if(!ok){return;}

                    std::cout << "set values" << std::endl;
                    m_MinX->setValue(x1);
                    m_MinY->setValue(x3);
                }
            }

            if(text.startsWith("Point("))
            {
                text.remove(0,6);
                if(text.endsWith(")"))
                {
                        text.remove(text.length()-1,1);
                }
                QStringList l = text.split(",");
                if(l.length() == 2)
                {
                    bool ok = true;;
                    double x1 = l.at(0).toDouble(&ok);
                    if(!ok){return;}
                    double x2 = l.at(1).toDouble(&ok);
                    if(!ok){return;}

                    m_MinX->setValue(x1);
                    m_MinY->setValue(x2);
                }
            }
        });
        connect(m_MinY,&SpinBox::OnTextPasted,[this](QString text){
            if(text.startsWith("BoundingBox("))
            {
                text.remove(0,12);
                if(text.endsWith(")"))
                {
                        text.remove(text.length()-1,1);
                }
                QStringList l = text.split(",");
                if(l.length() == 4)
                {
                    bool ok = true;
                    double x1 = l.at(0).toDouble(&ok);
                    if(!ok){return;}
                    double x2 = l.at(1).toDouble(&ok);
                    if(!ok){return;}
                    double x3 = l.at(2).toDouble(&ok);
                    if(!ok){return;}
                    double x4 = l.at(3).toDouble(&ok);
                    if(!ok){return;}

                    m_MinX->setValue(x1);
                    m_MinY->setValue(x3);
                }
            }

            if(text.startsWith("Point("))
            {
                text.remove(0,6);
                if(text.endsWith(")"))
                {
                        text.remove(text.length()-1,1);
                }
                QStringList l = text.split(",");
                if(l.length() == 2)
                {
                    bool ok = true;;
                    double x1 = l.at(0).toDouble(&ok);
                    if(!ok){return;}
                    double x2 = l.at(1).toDouble(&ok);
                    if(!ok){return;}

                    m_MinX->setValue(x1);
                    m_MinY->setValue(x2);
                }
            }

        });


    }
    inline void SetValue(QString x)
    {

    }
    inline LSMVector2 GetPoint()
    {
        return LSMVector2(m_MinX->value(),m_MinY->value());
    }
    inline QString GetValue()
    {

        return "Point(" + QString::number(m_MinX->value())  + ","+QString::number(m_MinY->value())  + ")";
    }

    inline QString GetValueAsString()
    {
        return "Point(" + QString::number(m_MinX->value()) +  ","+QString::number(m_MinY->value()) + ")";
    }


};

static inline ParameterSelectionWidget * GetSelectionWidgetFromType(LSMScriptEngine * sm, int as_type,QString DefaultValue = "")
{
    asITypeInfo * ti = sm->GetTypeInfoById(as_type);
    if(ti != nullptr)
    {

       QString name = QString(ti->GetName());

       if(name.compare("Map") == 0)
       {
            return new MapSelectWidget(DefaultValue);
       }else  if(name.compare("Shapes") == 0)
       {
            return new ShapeFileSelectWidget(DefaultValue);
       }else  if(name.compare("GeoProjection") == 0)
       {
            return new GeoProjectionSelectWidget(DefaultValue);
       }else  if(name.compare("Table") == 0)
       {
            return new TableSelectWidget(DefaultValue);
       }else  if(name.compare("string") == 0)
       {
           return new StringSelectWidget(DefaultValue);

       }else  if(name.compare("PointCloud") == 0)
       {
           return new PointCloudSelectWidget(DefaultValue);

       }else  if(name.compare("MapArray") == 0)
       {
           return new MapArraySelectWidget(DefaultValue);

       }else
       {
           return nullptr;
       }

    }else
    {
        int in_typeid = as_type;
        if(in_typeid == asTYPEID_VOID)
        {
            return nullptr;
        }
        //The type id for void.

        if(in_typeid == asTYPEID_BOOL)
        {
            return new BoolSelectWidget(DefaultValue);
        }
        //The type id for bool.

        if(in_typeid == asTYPEID_INT8)
        {
            return new Int8SelectWidget(DefaultValue);
        }
        //The type id for int8.

        if(in_typeid == asTYPEID_INT16)
        {
            return new Int16SelectWidget(DefaultValue);
        }
        //The type id for int16.

        if(in_typeid == asTYPEID_INT32)
        {
            return new Int32SelectWidget(DefaultValue);
        }
        //The type id for int.

        if(in_typeid == asTYPEID_INT64)
        {
            return new Int64SelectWidget(DefaultValue);
        }
        //The type id for int64.

        if(in_typeid == asTYPEID_UINT8)
        {
            return new UInt8SelectWidget(DefaultValue);
        }
        //The type id for uint8.

        if(in_typeid == asTYPEID_UINT16)
        {
            return new UInt16SelectWidget(DefaultValue);
        }
        //The type id for uint16.

        if(in_typeid == asTYPEID_UINT32)
        {
            return new UInt32SelectWidget(DefaultValue);
        }
        //The type id for uint.

        if(in_typeid == asTYPEID_UINT64)
        {
            return new UInt64SelectWidget(DefaultValue);
        }
        //The type id for uint64.

        if(in_typeid == asTYPEID_FLOAT)
        {
            return new FloatSelectWidget(DefaultValue);
        }
        //The type id for float.

        if(in_typeid == asTYPEID_DOUBLE)
        {
            return new DoubleSelectWidget(DefaultValue);
        }
        //The type id for double.

        return nullptr;


    }

}

#endif // TOOLPARAMETERWIDGET_H

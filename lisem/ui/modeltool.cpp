#include "modeltool.h"
#include "site.h"

void ModelTool::start()
{
    UpdateParameters();
    m_Model->RequestStart(m_ParameterManager->GetParameterList());

    m_ProgressBar->resetFormat();

    //indicate loading
    m_ProgressBar->setMinimum(0);
    m_ProgressBar->setMaximum(0);
    m_ProgressBar->setValue(0);

    //startAct->blockSignals(true);
    //startAct->setChecked(true);
    //startAct->blockSignals(false);
}


void ModelTool::pause()
{
    m_Model->RequestPause();
}

void ModelTool::stop()
{
    m_Model->RequestStop();
}

void ModelTool::OnModelPaused()
{
    QTimer::singleShot(0,this,SLOT(ModelPaused()));

}

void ModelTool::ModelPaused()
{
    m_ProgressBar->setTextVisible(true);
    m_ProgressBar->setFormat(" Paused at %p% ");
}

void ModelTool::OnModelStopped(bool has_error)
{

    ModelTool * w = this;
    QTimer::singleShot(0,this, [w,has_error] () {w->ModelStopped(has_error); });

}

void ModelTool::ModelStopped(bool has_error)
{

    m_ProgressBar->setTextVisible(true);
    m_ProgressBar->setMinimum(0);
    m_ProgressBar->setMaximum(1);
    m_ProgressBar->setValue(0);
    if(has_error)
    {
        m_ProgressBar->setFormat(" Error %p% ");


        QMessageBox messageBox;
        messageBox.critical(0,"Error","A critical error has occured! \nSee console for details.");
        messageBox.setFixedSize(500,200);

        messageBox.show();

    }else
    {
        m_ProgressBar->setFormat(" Finished %p% ");
    }

}


void ModelTool::newFile()
{
    m_ParameterManager->SetAllDefault();
    this->UpdateInterface();
    m_DropDown->insertItem(0,QString(""));
}

void ModelTool::open()
{
    ImportRunfile("");
}

void ModelTool::save()
{
    ExportRunfile(false);
}

void ModelTool::saveas()
{
    ExportRunfile(true);
}

void ModelTool::ImportRunfile(QString name)
{

    LISEM_STATUS("import runfile " + name);

    QString path;


    if(name.length() == 0)
    {
        QString openDir;
        QString currentDir = GetSite()+"/";

        if (m_DropDown->count() > 0)
        {
            openDir = QFileInfo(m_DropDown->currentText()).dir().absolutePath();
        }
        else
        {
            openDir = currentDir;
        }

        path = QFileDialog::getOpenFileName(this,
                                            QString("Select run file"),
                                            openDir,
                                            QString("*.run"));
    }else
    {
        path = name;
    }

    if (path.isEmpty())
    {

        return;
    }

    int ret = m_ParameterManager->LoadFromRunFile(path);

    if(ret != 0)
    {
        return;
    }

    UpdateInterface();

    bool exst = false;
    int nr = 0;
    for (int i = 0; i < m_DropDown->count(); i++)
    {
        if (m_DropDown->itemText(i).compare(path) == 0)
        {
            exst = true;
            nr = i;
        }
    }
    m_DropDown->blockSignals(true);
    if (!exst)
    {
        m_DropDown->insertItem(0,path);
        m_DropDown->setCurrentIndex(0);
    }else
    {
        m_DropDown->removeItem(nr);
        m_DropDown->insertItem(0,path);
        m_DropDown->setCurrentIndex(0);
    }
    m_DropDown->blockSignals(false);

    ExportRunfileList();

}

void ModelTool::ExportRunfile(bool saveas)
{
    bool current_exists = false;
    QString fileName;
    if(!saveas)
    {
        if(m_DropDown->count() > 0)
        {
            QString text = m_DropDown->currentText();
            if(text.length() > 0)
            {
                fileName = text;
                current_exists =true;
            }

        }
    }


    if(!current_exists)
    {
        QString selectedFilter;
        fileName = QFileDialog::getSaveFileName(this,
                                                        QString("Give a new runfile name"),
                                                        QString("sph.run"),
                                                        QString("Text Files (*.run);;All Files (*)"),
                                                        &selectedFilter);
    }

    if (!fileName.isEmpty())
    {

        UpdateParameters();
        int res =m_ParameterManager->SaveToRunFile(fileName);

        if(res != 0)
        {
            LISEM_ERROR("could not write to file: " + fileName);
        }
    }


    bool exst = false;
    int nr = 0;
    for (int i = 0; i < m_DropDown->count(); i++)
    {
        if (m_DropDown->itemText(i).compare(fileName) == 0)
        {
            exst = true;
            nr = i;
        }
    }

    m_DropDown->blockSignals(true);
    if (!exst)
    {
        m_DropDown->insertItem(0,fileName);
        m_DropDown->setCurrentIndex(0);
    }else
    {
        m_DropDown->removeItem(nr);
        m_DropDown->insertItem(0,fileName);
        m_DropDown->setCurrentIndex(0);

    }
    m_DropDown->blockSignals(false);

    ExportRunfileList();
}

void ModelTool::ExportRunfileList()
{

    QFile fin(m_Path_runfiles);
    if (fin.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        QTextStream out(&fin);
        out << QString("[Runfile List]\n");
        for(int i = 0; i < m_DropDown->count(); i++)
        {

            out << m_DropDown->itemText(i) << "\n";
        }
    }
    fin.close();

}

void ModelTool::OnRunFileDeleteFromList()
{

    m_DropDown->blockSignals(true);
    if(m_DropDown->count() > 0)
    {
        m_DropDown->removeItem(0);
    }
    if(m_DropDown->count() > 0)
    {
        m_DropDown->setCurrentIndex(0);
    }
    m_DropDown->blockSignals(false);

    if(m_DropDown->count() > 0)
    {
        ImportRunfile(m_DropDown->itemText(0));
    }

}
void ModelTool::OnRunfileIndexChanged(int i)
{
    ImportRunfile(m_DropDown->itemText(i));
}



void ModelTool::SignalFunction_UI(QString action)
{


}


void ModelTool::UpdateInterfaceFromModelData()
{

    m_Model->m_ModelDataMutex.lock();
    m_MapViewTool->m_map_combo->blockSignals(true);

    if(m_ModelData->m_mapnameschanged==true)
    {
        m_MapViewTool->m_map_combo->clear();

        for(int i =0; i < m_ModelData->m_UIMapNames.length();i++)
        {
            m_MapViewTool->m_map_combo->addItem(m_ModelData->m_UIMapNames.at(i));
        }
        m_MapViewTool->m_map_combo->setCurrentIndex(0);

        m_ModelData->m_mapnameschanged = false;
    }

    m_ProgressBar->setRange(0,m_ModelData->timesteps);
    m_ProgressBar->setValue(m_ModelData->step);
    m_MapViewTool->m_map_combo->blockSignals(false);

    m_ModelStatTool->OnModelDataUpdate();

    m_Model->m_ModelDataMutex.unlock();



}




void ModelTool::CreateParameterWidgets()
{


    QTabWidget * m_TabModel = m_TabWidget;

    QList<QVBoxLayout *> pastparents;
    QVBoxLayout * currentparent = m_Layout;
    pastparents.append(m_Layout);
    int currentlevel = -1;

    TabWidgetLayouts.clear();

    LISEM_DEBUG("Using parameter list to automatically generate widgets")

    for(int i = 0; i < m_ParameterManager->m_Parameters.length(); i++)
    {

        SPHParameter p = m_ParameterManager->m_Parameters.at(i);

        if(p.m_level > -1)
        {


            ParameterWidget pw;
            pw.m_SpinBox = 0;
            pw.m_CheckBox = 0;
            pw.m_DSpinBox = 0;
            pw.m_LineEdit = 0;
            pw.m_RadioButtons.clear();
            pw.m_ParameterIndex = i;
            pw.m_ParameterName = p.m_Name;


            int pwindex = m_ParameterWidgetList.length();


            bool is_grouplabel = false;
            if(p.m_level > currentlevel)
            {
                is_grouplabel = true;
                if(p.m_Type == LISEM_PARAMETER_LABEL)
                {
                    if(p.m_level == 0)
                    {
                        QWidget *tab = new QWidget();

                        QScrollArea *tab_scrollArea = new QScrollArea;
                        tab_scrollArea->setWidgetResizable(true);
                        tab_scrollArea->setWidget(tab);
                        tab_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
                        tab_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

                        QVBoxLayout *gblayout = new QVBoxLayout;
                        gblayout->addWidget(tab_scrollArea);

                        QVBoxLayout *gblayout2 = new QVBoxLayout;
                        tab->setLayout(gblayout2);
                        gblayout2->addWidget(tab);

                        m_TabModel->addTab(tab_scrollArea,p.m_Name);

                        TabWidgetLayouts.append(gblayout2);

                        pastparents.append(currentparent);
                        currentparent = gblayout2;
                    }else
                    {
                        QGroupBox * gb = new QGroupBox(p.m_Name);
                        gb->setFont(QFont( "Helvetica", 15 ));
                        QVBoxLayout *gblayout = new QVBoxLayout;
                        gb->setLayout(gblayout);
                        gblayout->setMargin(0);
                        gblayout->setSpacing(0);
                        currentparent->addWidget(gb);
                        pastparents.append(currentparent);
                        currentparent = gblayout;
                    }

                }

            }else if(p.m_level < currentlevel)
            {
                is_grouplabel = true;
                for(int j = currentlevel; j > std::max(0,p.m_level); j--)
                {
                    if(pastparents.length() > 0)
                    {
                        currentparent = pastparents.at(pastparents.length()-1);
                        pastparents.removeLast();
                    }
                }

                QGroupBox * gb = new QGroupBox(p.m_Name);
                gb->setFont(QFont( "Helvetica", 15 ));
                gb->setToolTip(p.m_Description);
                QVBoxLayout *gblayout = new QVBoxLayout;
                gb->setLayout(gblayout);
                currentparent->addWidget(gb);
                pastparents.append(currentparent);
                currentparent = gblayout;
            }

            currentlevel = p.m_level;

            if(p.m_Type == LISEM_PARAMETER_LABEL && !is_grouplabel)
            {
                QLabel *l = new QLabel();
                l->setText(p.m_Name);
                l->setFont(QFont( "Helvetica", 10 ));
                currentparent->addWidget(l);

            }else if(p.m_Type == LISEM_PARAMETER_DIR)
            {
                QWidget * hw = new QWidget();
                QHBoxLayout *templayout = new QHBoxLayout(hw);
                hw->setLayout(templayout);
                templayout->setSpacing(0);
                templayout->setMargin(0);

                QLineEdit *l = new QLineEdit();
                l->setText(p.m_Value);
                l->setFont(QFont( "Helvetica", 10 ));
                templayout->addWidget(l);

                QIcon icon;
                icon.addFile((m_Dir + LISEM_FOLDER_ASSETS + "fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);

                QToolButton *addButton = new QToolButton(this);
                addButton->setIcon(icon);
                addButton->setIconSize(QSize(22,22));
                addButton->resize(22,22);
                addButton->setEnabled(true);



                connect(addButton,SIGNAL(pressed()),m_SignalMapper_Dir,SLOT(map()));
                m_SignalMapper_Dir->setMapping(addButton,pwindex);

                QString buttonStyle = "QPushButton{border:none;background-color:rgba(255, 255, 255,100);}";
                addButton->setStyleSheet(buttonStyle);
                templayout->addWidget(addButton);
                templayout->setSpacing(0);
                templayout->setMargin(0);
                currentparent->addWidget(hw);

                pw.m_LineEdit = l;

            }else if(p.m_Type == LISEM_PARAMETER_FILE)
            {
                QWidget * hw = new QWidget();
                QHBoxLayout *templayout = new QHBoxLayout(hw);
                hw->setLayout(templayout);
                templayout->setSpacing(0);
                templayout->setMargin(0);

                QLabel * lab = new QLabel(p.m_Name);
                lab->setFont(QFont( "Helvetica", 10 ));
                lab->setMinimumSize(120,22);
                lab->setMaximumSize(120,22);
                lab->setToolTip(p.m_Description);
                templayout->addWidget(lab);


                QLineEdit *l = new QLineEdit();
                l->setText(p.m_Value);
                l->setFont(QFont( "Helvetica", 10 ));
                templayout->addWidget(l);

                if(p.m_hasmult)
                {
                    QDoubleSpinBox * dsb = new QDoubleSpinBox();
                    dsb->setValue(p.m_mult);
                    dsb->setFont(QFont( "Helvetica", 10 ));
                    templayout->addWidget(dsb);
                    pw.m_DSpinBox = dsb;
                }


                QIcon icon;
                icon.addFile((m_Dir + LISEM_FOLDER_ASSETS + "fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);

                QToolButton *addButton = new QToolButton(this);
                addButton->setIcon(icon);
                addButton->setIconSize(QSize(22,22));
                addButton->resize(22,22);
                addButton->setEnabled(true);


                connect(addButton,SIGNAL(pressed()),m_SignalMapper_File,SLOT(map()));
                m_SignalMapper_File->setMapping(addButton,pwindex);

                QString buttonStyle = "QPushButton{border:none;background-color:rgba(255, 255, 255,100);}";
                addButton->setStyleSheet(buttonStyle);
                templayout->addWidget(addButton);
                templayout->setSpacing(0);
                templayout->setMargin(0);
                currentparent->addWidget(hw);

                pw.m_LineEdit = l;

            }else if(p.m_Type == LISEM_PARAMETER_BOOL)
            {

                QWidget * hw = new QWidget();
                QHBoxLayout *templayout = new QHBoxLayout(hw);
                hw->setLayout(templayout);
                templayout->setSpacing(0);
                templayout->setMargin(0);

                QCheckBox *l = new QCheckBox(p.m_Name);
                l->setFont(QFont( "Helvetica", 10 ));
                l->setToolTip(p.m_Description);


                connect(l, &QCheckBox::clicked,[this, pwindex](bool val){


                    this->UpdateParameters();

                    bool changed = false;

                    for(int i =0; i< m_ParameterWidgetList.length(); i++)
                    {
                        ParameterWidget pw = m_ParameterWidgetList.at(i);
                        bool active = m_ParameterManager->GetParameterShouldBeActive(pw.m_ParameterIndex);

                        if(pw.m_SpinBox != 0)
                        {
                            if(active != pw.m_SpinBox->isEnabled())
                            {
                                pw.m_SpinBox->setEnabled(active);
                            }
                        }else if(pw.m_LineEdit != 0)
                        {
                            if(active != pw.m_LineEdit->isEnabled())
                            {
                                pw.m_LineEdit->setEnabled(active);
                            }
                        }else if(pw.m_CheckBox != 0)
                        {
                            if(active != pw.m_CheckBox->isEnabled())
                            {
                                pw.m_CheckBox->setEnabled(active);
                                if(pw.m_CheckBox->isChecked())
                                {

                                    pw.m_CheckBox->setChecked(false);
                                     changed = true;
                                }
                            }
                        }else if(pw.m_RadioButtons.length() > 0)
                        {
                            for(int k = 0; k < pw.m_RadioButtons.length(); k++)
                            {
                                if(active != pw.m_RadioButtons.at(k)->isEnabled())
                                {
                                    pw.m_RadioButtons.at(k)->setEnabled(active);
                                     changed = true;
                                }
                            }

                        }

                        if(pw.m_DSpinBox != 0)
                        {
                            if(active != pw.m_DSpinBox->isEnabled())
                            {
                                pw.m_DSpinBox->setEnabled(active);
                            }
                        }

                        changed = changed && m_ParameterManager->DoParameterActivity(pw.m_ParameterIndex);
                    }

                    if(changed)
                    {
                        this->UpdateInterface();
                    }
                    ;});


                templayout->addWidget(l);



                currentparent->addWidget(hw);

                pw.m_CheckBox = l;

            }
            if(p.m_Type == LISEM_PARAMETER_MAP)
            {
                QWidget * hw = new QWidget();
                QHBoxLayout *templayout = new QHBoxLayout(hw);
                hw->setLayout(templayout);
                templayout->setSpacing(1);
                templayout->setMargin(0);


                QLabel * lab = new QLabel(p.m_Name);
                lab->setFont(QFont( "Helvetica", 10 ));
                lab->setMinimumSize(120,22);
                lab->setMaximumSize(120,22);
                lab->setToolTip(p.m_Description);
                templayout->addWidget(lab);

                QLineEdit *l = new QLineEdit();
                l->setText(p.m_Value);
                l->setFont(QFont( "Helvetica", 10 ));
                templayout->addWidget(l);


                if(p.m_hasmult)
                {
                    QDoubleSpinBox * dsb = new QDoubleSpinBox();
                    dsb->setValue(p.m_mult);
                    dsb->setFont(QFont( "Helvetica", 10 ));
                    templayout->addWidget(dsb);
                    pw.m_DSpinBox = dsb;
                }

                QIcon icon;
                icon.addFile((m_Dir + LISEM_FOLDER_ASSETS + "fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);

                QToolButton *addButton = new QToolButton(this);
                addButton->setIcon(icon);
                addButton->setIconSize(QSize(22,22));
                addButton->resize(22,22);
                addButton->setEnabled(true);

                connect(addButton,SIGNAL(pressed()),m_SignalMapper_Map,SLOT(map()));
                m_SignalMapper_Map->setMapping(addButton,pwindex);

                QString buttonStyle = "QPushButton{border:none;background-color:rgba(255, 255, 255,100);}";
                addButton->setStyleSheet(buttonStyle);
                addButton->setMinimumSize(22,22);
                addButton->setMaximumSize(22,22);
                templayout->addWidget(addButton);

                currentparent->addWidget(hw);

                pw.m_LineEdit = l;

            }else if(p.m_Type == LISEM_PARAMETER_FLOAT)
            {


                QWidget * hw = new QWidget();
                QHBoxLayout *templayout = new QHBoxLayout(hw);
                hw->setLayout(templayout);
                templayout->setSpacing(0);
                templayout->setMargin(0);

                if(p.m_minrange == p.m_maxrange)
                {
                    QLabel *la = new QLabel(p.m_Name);
                    la->setFont(QFont( "Helvetica", 10 ));
                    QLineEdit *l = new QLineEdit();
                    l->setText(p.m_Value);
                    l->setFont(QFont( "Helvetica", 10 ));
                    l->setToolTip(p.m_Description);
                    templayout->addWidget(la);
                    templayout->addWidget(l);
                    pw.m_LineEdit = l;
                }else
                {
                    QLabel *la = new QLabel(p.m_Name);
                    la->setFont(QFont( "Helvetica", 10 ));
                    QDoubleSpinBox *l = new QDoubleSpinBox();
                    l->setValue(p.m_Value.toFloat());
                    l->setFont(QFont( "Helvetica", 10 ));
                    l->setToolTip(p.m_Description);
                    templayout->addWidget(la);
                    templayout->addWidget(l);
                    pw.m_DSpinBox = l;
                }
                currentparent->addWidget(hw);

            }else if(p.m_Type == LISEM_PARAMETER_INT)
            {

                QWidget * hw = new QWidget();
                QHBoxLayout *templayout = new QHBoxLayout(hw);
                hw->setLayout(templayout);
                templayout->setSpacing(0);
                templayout->setMargin(0);

                if(p.m_minrange == p.m_maxrange)
                {
                    QLabel *la = new QLabel(p.m_Name);
                    la->setFont(QFont( "Helvetica", 10 ));
                    QLineEdit *l = new QLineEdit();
                    l->setText(p.m_Value);
                    l->setFont(QFont( "Helvetica", 10 ));
                    l->setToolTip(p.m_Description);
                    templayout->addWidget(la);
                    templayout->addWidget(l);
                    pw.m_LineEdit = l;
                }else
                {
                    QLabel *la = new QLabel(p.m_Name);
                    la->setFont(QFont( "Helvetica", 10 ));
                    QSpinBox *l = new QSpinBox();
                    l->setValue(p.m_Value.toFloat());
                    l->setFont(QFont( "Helvetica", 10 ));
                    l->setToolTip(p.m_Description);
                    templayout->addWidget(la);
                    templayout->addWidget(l);
                    pw.m_SpinBox = l;
                }
                currentparent->addWidget(hw);
            }else if(p.m_Type == LISEM_PARAMETER_MULTICHECK)
            {

                QWidget * hw = new QWidget();
                QHBoxLayout *templayout = new QHBoxLayout(hw);
                hw->setLayout(templayout);
                templayout->setSpacing(0);
                templayout->setMargin(0);

                currentparent->addWidget(hw);
            }

            m_ParameterWidgetList.append(pw);


        }else
        {
            if(p.m_level < currentlevel)
            {
                for(int j = currentlevel; j > std::max(0,p.m_level); j--)
                {
                    if(pastparents.length() > 0)
                    {
                        currentparent = pastparents.at(pastparents.length()-1);
                        pastparents.removeLast();
                    }
                }
            }
            currentlevel = -1;

        }
    }

    for(int i = 0; i < TabWidgetLayouts.length(); i++)
    {
        QVBoxLayout * l = TabWidgetLayouts.at(i);
        l->addStretch();
    }
}

void ModelTool::UpdateParameters()
{
    for(int i =0; i< m_ParameterWidgetList.length(); i++)
    {
        ParameterWidget pw = m_ParameterWidgetList.at(i);
        bool found = false;
        if(pw.m_SpinBox != 0)
        {
            found = true;
            m_ParameterManager->SetParameterValue(pw.m_ParameterIndex,pw.m_SpinBox->value());
        }else if(pw.m_LineEdit != 0)
        {
            found = true;
            m_ParameterManager->SetParameterValue(pw.m_ParameterIndex,pw.m_LineEdit->text());
        }else if(pw.m_CheckBox != 0)
        {
            found = true;
            m_ParameterManager->SetParameterValue(pw.m_ParameterIndex,pw.m_CheckBox->isChecked());
        }else if(pw.m_RadioButtons.length() > 0)
        {
            found = true;
        }

        if(pw.m_DSpinBox != 0)
        {
            if(found)
            {
                m_ParameterManager->SetParameterMultValue(pw.m_ParameterIndex,pw.m_DSpinBox->value());
            }else
            {
                m_ParameterManager->SetParameterValue(pw.m_ParameterIndex,pw.m_DSpinBox->value());
            }
        }
    }
}

void ModelTool::UpdateInterface()
{
    for(int i =0; i< m_ParameterWidgetList.length(); i++)
    {
        ParameterWidget pw = m_ParameterWidgetList.at(i);
        {
            bool found = false;
            if(pw.m_SpinBox != 0)
            {
                found = true;
                pw.m_SpinBox->setValue(m_ParameterManager->GetParameterValueInt(pw.m_ParameterIndex));
            }else if(pw.m_LineEdit != 0)
            {
                found = true;
                pw.m_LineEdit->setText(m_ParameterManager->GetParameterValueString(pw.m_ParameterIndex));

            }else if(pw.m_CheckBox != 0)
            {
                found = true;
                bool val = m_ParameterManager->GetParameterValueBool(pw.m_ParameterIndex);
                if(pw.m_CheckBox->isChecked() != val)
                {

                    pw.m_CheckBox->setChecked(val);
                }
            }else if(pw.m_RadioButtons.length() > 0)
            {
                found = true;
            }

            if(pw.m_DSpinBox != 0)
            {
                if(found)
                {
                    pw.m_DSpinBox->setValue(m_ParameterManager->GetParameterMultValueDouble(pw.m_ParameterIndex));
                }else
                {
                    pw.m_DSpinBox->setValue(m_ParameterManager->GetParameterValueDouble(pw.m_ParameterIndex));
                }
            }
        }
    }
}

void ModelTool::SignalFunction_Bool(int index)
{


}

void ModelTool::SignalFunction_Map(int index)
{

    ParameterWidget pw = m_ParameterWidgetList.at(index);
    SPHParameter p = m_ParameterManager->m_Parameters.at(pw.m_ParameterIndex);

    QString dir = m_ParameterManager->GetParameterValueString("Map Directory");

    if(dir.isEmpty())
    {
        QString currentDir = GetSite()+"/";

        if (m_DropDown->count() > 0)
        {
            dir = QFileInfo(m_DropDown->currentText()).dir().absolutePath();
        }
        else
        {
            dir = currentDir;
        }
    }
    QString path = QFileDialog::getOpenFileName(this,	QString("Select the map: %1;").arg(pw.m_ParameterName),
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

       pw.m_LineEdit->setText(f);

    }else
    {
        /*if(!rasterCanBeOpenedForReading(path)) {
           QMessageBox::critical(this, "SPHazard",
              QString("Empty file name: %1.")
                  .arg(path));
           return;
        }*/

    }
}

void ModelTool::SignalFunction_Dir(int index)
{
    ParameterWidget pw = m_ParameterWidgetList.at(index);
    SPHParameter p = m_ParameterManager->m_Parameters.at(pw.m_ParameterIndex);

    QString dir;
    QString currentDir = GetSite()+"/";

    if (m_DropDown->count() > 0)
    {
        dir = QFileInfo(m_DropDown->currentText()).dir().absolutePath();
    }
    else
    {
        dir = currentDir;
    }

    QString path = QFileDialog::getExistingDirectory(this, QString("Select a directory: ") + p.m_Name,
                                                             dir,
                                                             QFileDialog::ShowDirsOnly
                                                             | QFileDialog::DontResolveSymlinks);

    // open file dialog

    if (!path.isEmpty())// && QFileInfo(path).exists())
    {

        pw.m_LineEdit->setText(QDir::cleanPath(path) + "/");
    }else
    {
        /*if(!rasterCanBeOpenedForReading(path)) {
           QMessageBox::critical(this, "SPHazard",
              QString("Empty file name: %1.")
                  .arg(path));
           return;
        }*/

    }

}

void ModelTool::SignalFunction_File(int index)
{
    ParameterWidget pw = m_ParameterWidgetList.at(index);
    SPHParameter p = m_ParameterManager->m_Parameters.at(pw.m_ParameterIndex);



    QString dir;
    QString currentDir = GetSite()+"/";

    if (m_DropDown->count() > 0)
    {
        dir = QFileInfo(m_DropDown->currentText()).dir().absolutePath();
    }
    else
    {
        dir = currentDir;
    }

    QString path = QFileDialog::getOpenFileName(this,	QString("Select the file: %1;").arg(pw.m_ParameterName),
                                                dir,QString("*.*"));
    // open file dialog


    if (!path.isEmpty())// && QFileInfo(path).exists())
    {
       QString f = QFileInfo(path).fileName();
       QString p = QFileInfo(path).dir().path();

       pw.m_LineEdit->setText(f);

    }else
    {
        /*if(!rasterCanBeOpenedForReading(path)) {
           QMessageBox::critical(this, "SPHazard",
              QString("Empty file name: %1.")
                  .arg(path));
           return;
        }*/

    }

}

void ModelTool::SignalFunction_Float(int index)
{


}

void ModelTool::SignalFunction_Int(int index)
{


}

void ModelTool::SignalFunction_MultiCheck(int index)
{


}


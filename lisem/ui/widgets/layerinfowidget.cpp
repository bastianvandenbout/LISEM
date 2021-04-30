#include "widgets/layerinfowidget.h"

#include "tableplot.h"
#include "qinputdialog.h"

void LayerInfoWidget::plot()
{

    std::sort(m_SelectedColumnsList.begin(),m_SelectedColumnsList.end());
    if(m_SelectedColumnsList.length() > 0)
    {

        MatrixTable * plott = model->getList()->GetCopyOfColumns(m_SelectedColumnsList);
        TablePlotter * plotter = new TablePlotter(plott);
        plotter->show();
    }else
    {
        MatrixTable * plott = model->getList()->Copy();
        TablePlotter * plotter = new TablePlotter(plott);
        plotter->show();
    }

}

void LayerInfoWidget::tableresize()
{
    bool ok;
    int sizex = QInputDialog::getInt(this, tr("Table Resize"),
                                           tr("Columns (horizontal size)"),10,
                                           0,2147483647,1, &ok);

    if(!ok)
    {
        return;
    }
    int sizey = QInputDialog::getInt(this, tr("Table Resize"),
                                           tr("Rows (Vertical size)"),10,
                                           0,2147483647,1, &ok);

    if(!ok)
    {
        return;
    }



    MatrixTable *t = model->getList();
    t->SetSize(sizey,sizex);
    model->SetAllData(t);
    m_Table->update();
    OnDataChanged();


}


void LayerInfoWidget::changetitle()
{


    std::sort(m_SelectedColumnsList.begin(),m_SelectedColumnsList.end());

    if(m_SelectedColumnsList.size() > 0)
    {
        int column = m_SelectedColumnsList.at(0);

        QString currenttitle = "";
        {
            MatrixTable *t = model->getList();
            currenttitle = t->GetColumnTitleQ(column);
        }
        bool ok;
        QString text = QInputDialog::getText(0, "OpenLISEM Hazard",
                                                 "Title of Column:", QLineEdit::Normal,
                                                 currenttitle, &ok);

        if(ok)
        {
            MatrixTable *t = model->getList();
            t->SetColumnTitle(column,text);
            model->SetAllData(t);
            m_Table->update();
            OnDataChanged();
        }

    }

}


void LayerInfoWidget::changedatatype()
{
    std::sort(m_SelectedColumnsList.begin(),m_SelectedColumnsList.end());

    if(m_SelectedColumnsList.size() > 0)
    {
        int column = m_SelectedColumnsList.at(0);
        int currenttype = 0;
        {
            MatrixTable *t = model->getList();
            currenttype = t->GetColumnType(column);
        }

        QStringList soundList;
        soundList <<"Unknown"<<"Integer(32bit)" << "Integer(64bit)" <<"Float(64bit)" <<  "Boolean" <<  "String";

        bool ok = false;
        QString result = QInputDialog::getItem(this,"OpenLISEM Hazard","Get Column Type",soundList,currenttype,false,&ok);

        if(ok)
        {
            int type_out = soundList.indexOf(result);
            if(type_out > -1)
            {

                MatrixTable *t = model->getList();
                t->SetColumnType(column,type_out);
                model->SetAllData(t);
                m_Table->update();
                OnDataChanged();

            }
        }
    }

}


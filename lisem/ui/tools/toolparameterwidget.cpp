#include "model.h"
#include "widgets/mapcreatedialog.h"
#include "toolparameterwidget.h"

#include "mapview/mapviewtool.h"


void MapSelectWidget::LoadConstantClicked(bool)
{

    MapCreateWidget * creator = new MapCreateWidget();
    int res = creator->exec();

    if(res == QDialog::Accepted){
           MapConstructInfo i = creator->GetMapInfo();

           m_LineEdit->setText(creator->GetValueAsCodeString(i));
           OnValueChanged();
    }

    delete creator;
}

void MapSelectWidget::LoadViewClicked(bool)
{
    QList<QString> maps;
    QList<int> ids;


    GetCurrentMapLayerNamesAndID(maps,ids);


    if(maps.length() == 0)
    {
        QMessageBox::information(this,"Info","No maps available to select");
    }else
    {
        QList<QString> mapsandids;
        for(int i = 0; i < maps.size(); i++)
        {
            mapsandids.append(maps.at(i) + " (" + QString::number(ids.at(i)) + ")");
        }

        bool ok = true;

        QString select = QInputDialog::getItem(this,"Select Map","Select map from current scene",mapsandids,0,true,&ok);

        int index = mapsandids.indexOf(select);
        if(ok && index > -1)
        {
            m_LineEdit->setText("GetViewLayerMap(GetUILayer(\""+ maps.at(index)+ "\")");
            OnValueChanged();
        }

    }


}

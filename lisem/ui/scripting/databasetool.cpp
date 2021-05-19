#include "model.h"
#include "databasetool.h"
#include "QInputDialog"
void DatabaseTool::OnNewFolder()
{
    if(m_BrowseTree->hasFocus())
    {
        QModelIndex index = m_BrowseTree->currentIndex();
        QString file = model->filePath(index);

        QString name = QInputDialog::getText(this,"New Directory","Directory name");
        QDir dir(file);
        bool maked = dir.mkdir(name);
        if(maked)
        {

        }else
        {
            QMessageBox::information(this,"Error","Could not create directory!");
        }

    }

}

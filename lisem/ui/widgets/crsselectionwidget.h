#ifndef CRSSELECTIONWIDGET_H
#define CRSSELECTIONWIDGET_H

#include "QWidget"
#include "QLineEdit"
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include "QLabel"
#include "QDialog"
#include "layer/geo/uigeolayer.h"
#include "QPushButton"
#include "QTreeView"
#include "QToolButton"
#include "QSortFilterProxyModel"
#include "treemodel.h"
#include "matrixtable.h"
#include "geo/geoobject.h"
#include "QFile"
#include "site.h"

#define LISEM_MAX_FREQ_CRS 10

extern MatrixTable LISEM_CRSTable;

inline static void LoadCRSTable(QString dir)
{
    LISEM_CRSTable.LoadFromFileCSV(dir + "/" + "pcs.csv");
}


class CRSSelectWidget : public QDialog
{
    Q_OBJECT;

    QGroupBox * m_MainWidget;
    QGridLayout * m_MainLayout;

    QLineEdit *m_SearchEdit;
    QLabel *m_Selected;
    QLabel *m_SelectedDescription;
    QTreeView *m_TreeViewAll;
    QTreeView *m_TreeViewFrequent;
    QWidget * m_MenuBar;
    QHBoxLayout *m_MenuLayout;
    QToolButton *m_SelectButton;
    QToolButton *m_SelectGenericButton;
    QToolButton *m_CRSToProjectButton;
    QSortFilterProxyModel *m_proxyModel;
    TreeModel *m_DataModel;
    TreeModel *m_DataModelFreq;
    GeoProjection m_CurrentProjection;
    int m_CurrentProjectionEPSG;
    bool m_currentset = false;
    int m_currentmodel = 0;
    QModelIndex m_currentindex;
    QModelIndex m_currentindex2;

    QString m_PathFreq;
    QList<int> m_CRSFreqList;

    //these are some frequently used global CRS that we always add to the frequent list (WGS 72, WGS 84 etc..)
    QList<int> m_CRSFreqGenList = {4326,4322,4760};

public:
    inline CRSSelectWidget() : QDialog()
    {
        QVBoxLayout *mv = new QVBoxLayout();
        this->setLayout(mv);

        m_PathFreq = GetSite() + "/crsfeq" + ".ini";

        m_MainWidget = new QGroupBox("CRS Selection");
        m_MainLayout = new QGridLayout();
        m_MainWidget->setLayout(m_MainLayout);

        m_Selected = new QLabel("Current: ");
        m_SelectedDescription = new QLabel("Description");
        m_SelectedDescription->setWordWrap(true);
        m_MenuBar = new QWidget();
        m_MenuLayout =new QHBoxLayout();

        m_TreeViewAll = new QTreeView();
        m_TreeViewFrequent = new QTreeView();

        LoadFreqCRSList();

        QString datamodel;
        for(int i = 2 ; i < LISEM_CRSTable.GetNumberOfRows(); i++)
        {
            datamodel.append(LISEM_CRSTable.GetValueStringQ(i,1) + " \t "  + LISEM_CRSTable.GetValueStringQ(i,0) + "\n");
        }

        QString datamodelfreq;
        for(int i = 0 ; i < m_CRSFreqGenList.length(); i++)
        {
            int epsg = m_CRSFreqGenList.at(i);
            GeoProjection gp = GeoProjection::GetFromEPSG(epsg);
            if(gp.IsValid())
            {
                datamodelfreq.append(gp.GetName() + " \t "  + QString::number(epsg) + "\n");
            }
        }

        for(int i = 0 ; i < m_CRSFreqList.length(); i++)
        {
            int epsg = m_CRSFreqList.at(i);
            GeoProjection gp = GeoProjection::GetFromEPSG(epsg);
            if(gp.IsValid())
            {
                datamodelfreq.append(gp.GetName() + " \t "  + QString::number(epsg) + "\n");
            }
        }

        m_DataModel = new TreeModel(datamodel, "CRS Name", "EPSG Code");
        m_proxyModel = new QSortFilterProxyModel();
        m_proxyModel->setSourceModel(m_DataModel);
        m_TreeViewAll->setModel(m_proxyModel);

        m_DataModelFreq = new TreeModel(datamodelfreq, "CRS Name", "EPSG Code");
        m_TreeViewFrequent->setModel(m_DataModelFreq);
        m_CRSToProjectButton = new QToolButton();
        m_CRSToProjectButton->setText("Set as project CRS");

        m_SelectButton = new QToolButton();
        m_SelectButton->setText("Select");

        m_SelectGenericButton = new QToolButton();
        m_SelectGenericButton->setText("Set as Generic");

        m_SearchEdit = new QLineEdit();

        m_MenuBar->setLayout(m_MenuLayout);
        m_MenuLayout->addWidget(m_SelectButton);
        m_MenuLayout->addSpacing(10);
        m_MenuLayout->addWidget(m_SelectGenericButton);
        m_MenuLayout->addSpacing(10);
        //m_MenuLayout->addWidget(m_CRSToProjectButton);
        m_MainLayout->addWidget(m_SearchEdit);
        m_MainLayout->addWidget(m_TreeViewAll);
        m_MainLayout->addWidget(new QLabel("Recently Used"));
        m_MainLayout->addWidget(m_TreeViewFrequent);
        m_MainLayout->addWidget(m_MenuBar);
        m_MainLayout->addWidget(m_Selected);
        m_MainLayout->addWidget(m_SelectedDescription);


        mv->addWidget(m_MainWidget);


        connect(m_TreeViewAll,SIGNAL(clicked(const QModelIndex &)),this, SLOT(OnCRSModelClicked(const QModelIndex &)));
        connect(m_TreeViewFrequent,SIGNAL(clicked(const QModelIndex &)),this, SLOT(OnCRSModelFreqClicked(const QModelIndex &)));

        connect(m_TreeViewAll,SIGNAL(doubleClicked(const QModelIndex &)),this, SLOT(OnCRSModelDClicked(const QModelIndex &)));
        connect(m_TreeViewFrequent,SIGNAL(doubleClicked(const QModelIndex &)),this, SLOT(OnCRSModelFreqDClicked(const QModelIndex &)));

        connect(m_SelectButton,SIGNAL(pressed()),this,SLOT(OnSelect()));
        connect(m_SelectGenericButton,SIGNAL(pressed()),this,SLOT(OnGeneric()));
        connect(m_CRSToProjectButton,SIGNAL(pressed()),this,SLOT(OnProjectAs()));

        connect(m_SearchEdit, SIGNAL(textChanged(QString)),this,SLOT(OnSearchChanged(QString)));

        QWidget * buttons = new QWidget();
        QHBoxLayout *buttonsl = new QHBoxLayout();
        buttons->setLayout(buttonsl);

        QPushButton * okb = new QPushButton("Ok");
        QPushButton * cancelb = new QPushButton("Cancel");

        connect(okb, SIGNAL(clicked(bool)),this,SLOT(onOkClicked()));
        connect(cancelb, SIGNAL(clicked(bool)),this,SLOT(onCancelClicked()));

        buttonsl->addWidget(okb);
        buttonsl->addWidget(cancelb);

        mv->addWidget(buttons);

    }

    inline GeoProjection GetCurrentProjection()
    {
        return m_CurrentProjection;
    }
    inline void AddCurrentProjectionToFreq(int epsg)
    {
        //int epsg =m_CurrentProjection.GetEPSG();

        if(epsg > 0 && !m_CRSFreqList.contains(epsg))
        {
            m_CRSFreqList.prepend(epsg);
        }
    }

    inline void UpdateFreqCRSList()
    {
        QFile fin(m_PathFreq);
        if (fin.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        {
            QTextStream out(&fin);
            out << QString("[Frequent CRS]\n");
            for(int i = 0; i < std::min(m_CRSFreqList.length(),LISEM_MAX_FREQ_CRS); i++)
            {
                out << m_CRSFreqList.at(i) << "\n";
            }
        }
        fin.close();
    }

    inline void LoadFreqCRSList()
    {
        QFile fin(m_PathFreq);
        if (!fin.open(QFile::ReadOnly | QFile::Text)) {

            if (fin.open(QIODevice::WriteOnly | QIODevice::Truncate |QIODevice::Text))
            {
                QTextStream out(&fin);
                out << QString("[Frequent CRS]\n");
            }
            fin.close();
        }else
        {
            while (!fin.atEnd())
            {
                QString S = fin.readLine().trimmed();
                if(!S.startsWith("["))
                {
                    bool ok = true;

                    int epsg = S.toInt(&ok);
                    if(ok)
                    {
                        m_CRSFreqList.append(epsg);
                    }
                }
            }
        }

        UpdateFreqCRSList();
    }

    inline void SetCurrentCRS(GeoProjection p, int epsg = -99999)
    {
        if(epsg == -99999)
        {
            epsg = p.GetEPSG();
        }
        m_CurrentProjection = p;
        m_Selected->setText("Current: " + QString(p.GetName()));
        m_SelectedDescription->setText("Description: " + p.GetWKT());

        if(p.IsValid() && !p.IsGeneric())
        {
            AddCurrentProjectionToFreq(epsg);
            UpdateFreqCRSList();
        }

    }

    inline void SetCurrentCRSFromCurrentIndex()
    {
        int r = m_currentindex.row();
        QModelIndex i = m_proxyModel->index(r,1);
        QVariant data = m_proxyModel->data(i,Qt::DisplayRole);
        bool ok = true;
        int code = data.toString().toInt(&ok);
        if(ok)
        {
            GeoProjection gp = GeoProjection::GetFromEPSG(code);

            if(gp.IsValid() && !gp.IsGeneric())
            {
                return SetCurrentCRS(gp,code);
            }
        }

        LISEM_ERROR("Could not parse EPSG code: " +data.toString());
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Could not parse EPSG code: " +data.toString());
        messageBox.setFixedSize(500,200);

        messageBox.show();
    }

    inline void SetCurrentCRSFreqFromCurrentIndex()
    {
        int r = m_currentindex2.row();
        QModelIndex i = m_DataModelFreq->index(r,1);
        QVariant data = m_DataModelFreq->data(i,Qt::DisplayRole);
        bool ok = true;
        int code = data.toString().toInt(&ok);
        if(ok)
        {
            GeoProjection gp = GeoProjection::GetFromEPSG(code);

            if(gp.IsValid() && !gp.IsGeneric())
            {
                return SetCurrentCRS(gp,code);
            }
        }

        LISEM_ERROR("Could not parse EPSG code: " +data.toString());
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Could not parse EPSG code: " +data.toString());
        messageBox.setFixedSize(500,200);

        messageBox.show();
    }


public slots:

    inline void OnSearchChanged(QString t)
    {
        if(!t.isEmpty())
        {
            m_proxyModel->setFilterKeyColumn(0);
            m_proxyModel->setFilterRegExp(QRegExp(t, Qt::CaseInsensitive, QRegExp::FixedString));
            m_proxyModel->sort(1, Qt::AscendingOrder);
        }else {
            m_proxyModel->setFilterRegExp("");
            m_proxyModel->sort(1, Qt::AscendingOrder);
        }
    }
    inline void OnSelect()
    {
        if(m_currentset)
        {
            if(m_currentmodel == 0)
            {

                SetCurrentCRSFromCurrentIndex();
            }else
            {
                SetCurrentCRSFreqFromCurrentIndex();
            }
        }
    }
    inline void OnGeneric()
    {
        GeoProjection p;
        p.SetGeneric();
        SetCurrentCRS(p);
    }

    inline void OnProjectAs()
    {

    }

    inline void onOkClicked()
    {
        emit accept();
    }
    inline void onCancelClicked()
    {
        emit reject();
    }

    inline void OnCRSModelClicked(const QModelIndex & index)
    {
        m_currentset = true;
        m_currentmodel = 0;
        m_currentindex = index;
    }

    inline void OnCRSModelDClicked(const QModelIndex & index)
    {
        m_currentset = true;
        m_currentmodel = 0;
        m_currentindex = index;
        SetCurrentCRSFromCurrentIndex();

    }

    inline void OnCRSModelFreqClicked(const QModelIndex & index)
    {
        m_currentset = true;
        m_currentmodel = 1;
        m_currentindex2 = index;
    }


    inline void OnCRSModelFreqDClicked(const QModelIndex & index)
    {
        m_currentset = true;
        m_currentmodel = 1;
        m_currentindex2 = index;
        SetCurrentCRSFreqFromCurrentIndex();

    }


};
#endif // CRSSELECTIONWIDGET_H

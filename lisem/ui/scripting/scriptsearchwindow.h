#ifndef SCRIPTSEARCHWINDOW_H
#define SCRIPTSEARCHWINDOW_H

#include "qtablewidget.h"
#include "widgets/tablemodel.h"
#include "qsortfilterproxymodel.h"
#include "QLineEdit"
#include "QLabel"
#include "QToolButton"
#include "QTableView"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include "resourcemanager.h"
#include "QHeaderView"

class ScriptSearchWindow : public QWidget
{
    Q_OBJECT

    MatrixTable * mt;
    TableModel *model;
    QSortFilterProxyModel * proxyModel;
    QTableView * m_Table;

    QToolButton * m_ButtonNext;
    QToolButton * m_ButtonPrev;
    QLabel * m_LabelCount;
    QLineEdit * m_ReplaceEdit;
    QToolButton * m_Replace;
    QLabel * m_Label;
    QToolButton * m_Close;

public:
    inline ScriptSearchWindow() :QWidget()
    {

        mt = new MatrixTable();
        model = new TableModel(mt);
        m_Table = new QTableView();

        proxyModel = new QSortFilterProxyModel();
        proxyModel->setSourceModel( model );
        m_Table->setModel( proxyModel );

        m_Table->horizontalHeader()->setSectionsMovable(true);
        m_Table->verticalHeader()->setSectionsMovable(true);


        m_Label = new QLabel();
        m_ButtonNext = new QToolButton();
        m_ButtonPrev = new QToolButton();
        m_LabelCount = new QLabel();
        m_ReplaceEdit = new QLineEdit();
        m_Replace = new QToolButton();
        m_Close = new QToolButton();

        m_Label->setText("no search");


        m_Replace->setText("Search and Replace");
        m_ButtonNext->setIcon(*GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_ADD)));
        m_ButtonPrev->setIcon(*GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_REMOVE)));

        m_Close->setIcon(*GetResourceManager()->GetIcon(GetResourceManager()->GetDefaultIconName(LISEM_ICON_DELETE)));

        //create widgets and set empty data

        QWidget * bar = new QWidget();
        QHBoxLayout * barl = new QHBoxLayout();
        barl->addWidget(m_Label);
        barl->addWidget(m_ButtonPrev);
        barl->addWidget(m_ButtonNext);
        barl->addWidget(m_LabelCount);
        barl->addWidget(m_ReplaceEdit);
        barl->addWidget(m_Replace);
        barl->addWidget(m_Close);
        bar->setLayout(barl);

        QVBoxLayout *lv = new QVBoxLayout();
        this->setLayout(lv);
        lv->addWidget(bar);
        lv->addWidget(m_Table);

        connect(m_Table, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onTableClicked(const QModelIndex &)));        connect(m_Table, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onTableClicked(const QModelIndex &)));
        connect(m_Table, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onTableDClicked(const QModelIndex &)));        connect(m_Table, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onTableClicked(const QModelIndex &)));
    }

    inline void Clear()
    {
        mt->Empty();
        delete mt;
        mt = new MatrixTable();
        model->SetAllData(mt);
        //proxyModel = new QSortFilterProxyModel();
        //proxyModel->setSourceModel( model );
        m_Table->setModel( proxyModel );


    }

    inline void SetTitle(QString title)
    {
        m_Label->setText(title);
    }
    inline void SetData(MatrixTable * T)
    {
        mt->Empty();
        delete mt;
        mt = T->Copy();

        model->SetAllData(mt);
        //proxyModel = new QSortFilterProxyModel();
        //proxyModel->setSourceModel( model );
        m_Table->setModel( proxyModel );


    }

    bool m_HasCallBackClicked = false;
    std::function<void(int,int,QString,int)> m_func; //arguments are item row, item column, text in line, type of click

    inline void SetCallBackItemClicked(std::function<void(int,int,QString,int)> func)
    {
        m_HasCallBackClicked = true;
        m_func = func;
    }


public slots:


    inline void onTableClicked(const QModelIndex &index)
    {
        if (index.isValid()) {
            QString cellText = index.data().toString();
            std::cout << "table clicked: " << cellText.toStdString() << std::endl;

            if(m_HasCallBackClicked)
            {
                m_func(index.row(),index.column(),cellText,1);
            }
        }
    }
    inline void onTableDClicked(const QModelIndex &index)
    {
        if (index.isValid()) {
            QString cellText = index.data().toString();
            std::cout << "table dclicked: " << cellText.toStdString() << std::endl;

            if(m_HasCallBackClicked)
            {
                m_func(index.row(),index.column(),cellText,2);
            }
        }
    }



};




#endif // SCRIPTSEARCHWINDOW_H

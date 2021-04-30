 #ifndef TABLEMODEL_H
 #define TABLEMODEL_H

#include "defines.h"
 #include <QAbstractTableModel>
 #include <QPair>
 #include <QList>
#include "matrixtable.h"

 class LISEM_API TableModel : public QAbstractTableModel
 {
     Q_OBJECT

 public:
     TableModel(QObject *parent=0);
     TableModel(MatrixTable * listofPairs, QObject *parent=0);

     inline void SetAllData(MatrixTable * table)
     {
        beginResetModel();

        if(!(listOfPairs == table))
        {
            listOfPairs->CopyFrom(table);
        }


        endResetModel();

     }

     int rowCount(const QModelIndex &parent) const;
     int columnCount(const QModelIndex &parent) const;
     QVariant data(const QModelIndex &index, int role) const;
     QVariant headerData(int section, Qt::Orientation orientation, int role) const;
     Qt::ItemFlags flags(const QModelIndex &index) const;
     bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
     bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
     bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
     MatrixTable *getList();

 private:
     MatrixTable *listOfPairs;
 };

 #endif

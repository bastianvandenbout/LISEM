

 #include "tablemodel.h"

 TableModel::TableModel(QObject *parent)
     : QAbstractTableModel(parent)
 {
 }

 TableModel::TableModel(MatrixTable *pairs, QObject *parent)
     : QAbstractTableModel(parent)
 {
     listOfPairs=pairs;
 }

 int TableModel::rowCount(const QModelIndex &parent) const
 {
     return listOfPairs->GetNumberOfRows();
 }

 int TableModel::columnCount(const QModelIndex &parent) const
 {
     return listOfPairs->GetNumberOfCols();
 }

 QVariant TableModel::data(const QModelIndex &index, int role) const
 {
     if (!index.isValid())
     {
         return QVariant();
     }

     if (index.row() >= listOfPairs->GetNumberOfRows() || index.row() < 0)
     {
         return QVariant();
     }

     if (role == Qt::DisplayRole) {
         if (index.column() >= listOfPairs->GetNumberOfCols() || index.column() < 0)
         {
             return QVariant();
         }

         if(listOfPairs->IsColumnCalcDouble(index.column()))
         {
             bool ok = false;
            double number =listOfPairs->GetValueStringQ(index.row(),index.column()).toDouble(&ok);
            if(ok)
            {
                return QVariant(number);
            }else {
                return listOfPairs->GetValueStringQ(index.row(),index.column());
            }
         }else if (listOfPairs->IsColumnCalcInt(index.column()))
         {
             bool ok = false;
            int number =listOfPairs->GetValueStringQ(index.row(),index.column()).toInt(&ok);
            if(ok)
            {
                return QVariant(number);
            }else {
                return listOfPairs->GetValueStringQ(index.row(),index.column());
            }
         }
         return listOfPairs->GetValueStringQ(index.row(),index.column());
     }else if(role == Qt::EditRole)
     {
         if (index.column() >= listOfPairs->GetNumberOfCols() || index.column() < 0)
         {
             return QVariant();
         }

         if(listOfPairs->IsColumnCalcDouble(index.column()))
         {
             bool ok = false;
            double number =listOfPairs->GetValueStringQ(index.row(),index.column()).toDouble(&ok);
            if(ok)
            {
                return QVariant(number);
            }else {
                return listOfPairs->GetValueStringQ(index.row(),index.column());
            }
         }else if (listOfPairs->IsColumnCalcInt(index.column()))
         {
             bool ok = false;
            int number =listOfPairs->GetValueStringQ(index.row(),index.column()).toInt(&ok);
            if(ok)
            {
                return QVariant(number);
            }else {
                return listOfPairs->GetValueStringQ(index.row(),index.column());
            }
         }
         return listOfPairs->GetValueStringQ(index.row(),index.column());
     }
     return QVariant();
 }

 QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
 {
     if (role != Qt::DisplayRole)
         return QVariant();

     if (orientation == Qt::Horizontal) {
         QString title = listOfPairs->GetColumnTitle(section);
         if(title.isEmpty())
         {
             return "<" +QString::number(section)+">";
         }else {
             return title;
             }
     }
     if (orientation == Qt::Vertical) {
         QString title = listOfPairs->GetRowTitle(section);
         if(title.isEmpty())
         {
             return "<" +QString::number(section)+">";
         }else {
            return title;
            }
     }
     return QVariant();
 }

 bool TableModel::insertRows(int position, int rows, const QModelIndex &index)
 {
     Q_UNUSED(index);
     beginInsertRows(QModelIndex(), position, position+rows-1);

     for (int row=0; row < rows; row++) {
         listOfPairs->AddRow(position);
     }

     endInsertRows();
     return true;
 }

 bool TableModel::removeRows(int position, int rows, const QModelIndex &index)
 {
     Q_UNUSED(index);
     beginRemoveRows(QModelIndex(), position, position+rows-1);

     for (int row=0; row < rows; ++row) {
         listOfPairs->RemoveRow(position);
     }

     endRemoveRows();
     return true;
 }

 bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
 {
         if (index.isValid() && role == Qt::EditRole) {
                 int row = index.row();
                 int column = index.column();

                 listOfPairs->SetValue(row,column,value.toString());
                 emit(dataChanged(index, index));

                return true;
         }

         return false;
 }

 Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
 {
     if (!index.isValid())
         return Qt::ItemIsEnabled;

     return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
 }

MatrixTable *TableModel::getList()
 {
     return listOfPairs;
 }

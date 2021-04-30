#include "stackdisplay.h"
#include "widgets/TreeItem.h"

#include <QtWidgets>
#include "QString"

#include "widgets/TreeItem.h"


SDTreeItem::SDTreeItem(const QVector<QVariant> &data,  void * var , std::vector<std::pair<QString,std::function<void(void*)>>> actions, SDTreeItem *parent)
    : itemData(data), m_Variable(var),m_Actions(actions),
    parentItem(parent)
{




}


SDTreeItem::~SDTreeItem()
{
    qDeleteAll(childItems);
}


SDTreeItem *SDTreeItem::child(int number)
{
    if (number < 0 || number >= childItems.size())
        return nullptr;
    return childItems.at(number);
}

int SDTreeItem::childCount() const
{
    return childItems.count();
}

int SDTreeItem::childNumber() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<SDTreeItem*>(this));
    return 0;
}

int SDTreeItem::columnCount() const
{
    return itemData.count();
}


QVariant SDTreeItem::data(int column) const
{
    if (column < 0 || column >= itemData.size())
        return QVariant();
    return itemData.at(column);
}

bool SDTreeItem::insertChildren(SDTreeItem*item)
{
        childItems.push_back(item);
        item->parentItem = this;

    return true;
}


SDTreeItem *SDTreeItem::parent()
{
    return parentItem;
}

bool SDTreeItem::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= itemData.size())
        return false;

    itemData[column] = value;
    return true;
}


SDTreeModel::SDTreeModel( QObject *parent)
    : QAbstractItemModel(parent)
{


}



void SDTreeModel::SetRoot(SDTreeItem * Root)
{
    beginResetModel();

    rootItem = Root;

    endResetModel();

}

SDTreeModel::~SDTreeModel()
{
    delete rootItem;
}

int SDTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return rootItem->columnCount();
}

QVariant SDTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    SDTreeItem *item = getItem(index);

    return item->data(index.column());
}


Qt::ItemFlags SDTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

SDTreeItem *SDTreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        SDTreeItem *item = static_cast<SDTreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}

QVariant SDTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex SDTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    SDTreeItem *parentItem = getItem(parent);
    if (!parentItem)
        return QModelIndex();


    SDTreeItem *childItem = parentItem->child(row);

    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex SDTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    SDTreeItem *childItem = getItem(index);
    SDTreeItem *parentItem = childItem ? childItem->parent() : nullptr;

    if (parentItem == rootItem || !parentItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}


int SDTreeModel::rowCount(const QModelIndex &parent) const
{
    const SDTreeItem *parentItem = getItem(parent);

    return parentItem ? parentItem->childCount() : 0;
}

bool SDTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    SDTreeItem *item = getItem(index);
    bool result = item->setData(index.column(), value);

    if (result)
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});

    return result;
}

bool SDTreeModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    const bool result = rootItem->setData(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}

void SDTreeModel::setupModelData(const QStringList &lines, SDTreeItem *parent)
{

}

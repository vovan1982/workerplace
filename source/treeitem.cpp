#include <QStringList>
#include "headers/treeitem.h"

TreeItem::TreeItem(const QVector<QVariant> &data, TreeItem *parent)
{
    parentItem = parent;
    itemData = data;
}

TreeItem::~TreeItem()
{
    qDeleteAll(children);
}

TreeItem *TreeItem::child(int number)
{
    return children.value(number);
}

QList<TreeItem*> TreeItem::child()
{
    return children;
}

int TreeItem::childCount() const
{
    return children.count();
}

int TreeItem::childNumber() const
{
    if (parentItem)
        return parentItem->children.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

int TreeItem::columnCount() const
{
    return itemData.count();
}

QVariant TreeItem::data(int column) const
{
    return itemData.value(column);
}

QVector<QVariant> TreeItem::data()
{
    return itemData;
}

bool TreeItem::insertChildren(int position, int count, int columns)
{
    if (position < 0 || position > children.size())
        return false;

    for (int row = 0; row < count; ++row) {
        QVector<QVariant> data(columns);
        TreeItem *item = new TreeItem(data, this);
        children.insert(position, item);
    }

    return true;
}

bool TreeItem::insertChildren(int position, TreeItem* item)
{
    if (position < 0 || position > children.size())
        return false;
    children.insert(position, item);
    return true;
}

void TreeItem::setChildren(const QList<TreeItem*> &c)
{
    children = c;
}

bool TreeItem::insertColumns(int position, int columns)
{
    if (position < 0 || position > itemData.size())
        return false;

    for (int column = 0; column < columns; ++column)
        itemData.insert(position, QVariant());

    foreach (TreeItem *child, children)
        child->insertColumns(position, columns);

    return true;
}

TreeItem *TreeItem::parent()
{
    return parentItem;
}

void TreeItem::setParent(TreeItem *newParent)
{
    parentItem = newParent;
}

bool TreeItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > children.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete children.takeAt(position);

    return true;
}


bool TreeItem::removeColumns(int position, int columns)
{
    if (position < 0 || position + columns > itemData.size())
        return false;

    for (int column = 0; column < columns; ++column)
        itemData.remove(position);

    foreach (TreeItem *child, children)
        child->removeColumns(position, columns);

    return true;
}


bool TreeItem::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= itemData.size())
        return false;

    itemData[column] = value;
    return true;
}

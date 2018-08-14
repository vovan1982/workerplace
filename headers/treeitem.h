#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <QVector>
#include <QtDebug>

class TreeItem
{
public:
    TreeItem(const QVector<QVariant> &data, TreeItem *parent = 0);
    ~TreeItem();
    TreeItem *child(int number);
    QList<TreeItem*> child();
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    QVector<QVariant> data();
    bool insertChildren(int position, int count, int columns);
    bool insertChildren(int position, TreeItem* item);
    bool insertColumns(int position, int columns);
    TreeItem *parent();
    void setParent(TreeItem *newParent);
    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);
    int childNumber() const;
    bool setData(int column, const QVariant &value);
    void setChildren(const QList<TreeItem*> &c);
private:
    QList<TreeItem*> children;
    QVector<QVariant> itemData;
    TreeItem *parentItem;
};

#endif

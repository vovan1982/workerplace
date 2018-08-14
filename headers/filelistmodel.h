#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QStringList>

class FileListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    FileListModel(QObject *parent = 0);
    QModelIndex index ( int row, int column = 0, const QModelIndex & parent = QModelIndex() ) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QModelIndex findItem(const QString &name);
    void setHideFileName(bool hide);

signals:
    void numberPopulated(int number);

public slots:
    void setDirPath(const QString &path);

protected:
    bool canFetchMore(const QModelIndex &parent) const;
    void fetchMore(const QModelIndex &parent);

private:
    bool hideFileName;
    QStringList fileList;
    int fileCount;
    QString dirPatch;
    QModelIndex indexFromItemData(const QString &file);
};

#endif

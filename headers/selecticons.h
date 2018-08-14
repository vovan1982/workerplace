#ifndef SELECTICONS_H
#define SELECTICONS_H

#include "ui_selecticons.h"

class FileListModel;

class SelectIcons : public QDialog, private Ui::SelectIcons
{
    Q_OBJECT

public:
    explicit SelectIcons(QWidget *parent = 0, const QString &dirIcon = "");
private:
    FileListModel *model;
signals:
    void iconSelected(const QString &iconFile);
protected:
    void changeEvent(QEvent *e);
private slots:
    void on_selectButton_clicked();
};

#endif // SELECTICONS_H

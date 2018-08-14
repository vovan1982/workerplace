#ifndef JOURNALHISTORYMOVED_H
#define JOURNALHISTORYMOVED_H

#include "ui_journalhistorymoved.h"

class QStandardItemModel;

class JournalHistoryMoved : public QWidget, private Ui::JournalHistoryMoved
{
    Q_OBJECT
    
public:
    explicit JournalHistoryMoved(QWidget *parent = 0, int itemCod = 0, int itemType = 0, int wpCod = 0);
private:
    QString curFilter;
    QStandardItemModel *historyModel;
    void initModel(const QString &filter = "");
    void populateCBox(const QString &idName, const QString &tableName,
                      const QString &filter, const QString &nullStr, QComboBox *cBox);
signals:
    void closeJhmWin();
protected:
    void changeEvent(QEvent *e);
private slots:
    void updateHistoryModel();
    void updateHistoryModel(const QString &filter);
    void on_buttonClose_clicked();
    void on_buttonSetFilter_clicked();
    void on_buttonClearFilter_clicked();
};

#endif // JOURNALHISTORYMOVED_H

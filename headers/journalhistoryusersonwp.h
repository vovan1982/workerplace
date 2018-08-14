#ifndef JOURNALHISTORYUSERSONWP_H
#define JOURNALHISTORYUSERSONWP_H

#include "ui_journalhistoryusersonwp.h"

class QStandardItemModel;

class JournalHistoryUsersOnWP : public QWidget, private Ui::JournalHistoryUsersOnWP
{
    Q_OBJECT
    
public:
    explicit JournalHistoryUsersOnWP(QWidget *parent = 0, int codUser = 0, int codWP = 0);
private:
    QString curFilter;
    QStandardItemModel *historyModel;
    void initModel(const QString &filter = "");
    void populateCBox(const QString &idName, const QString &tableName,
                      const QString &filter, const QString &nullStr, QComboBox *cBox);
signals:
    void closeJhuWin();
protected:
    void changeEvent(QEvent *e);
private slots:
    void updateHistoryModel();
    void setFilterUser(const QString &userName, int userId);
    void setFilterWP(int wpId, const QString &wpName);
    void on_userFilter_runButtonClicked();
    void on_userFilter_clearButtonClicked();
    void on_wpFilter_runButtonClicked();
    void on_wpFilter_clearButtonClicked();
    void on_buttonClose_clicked();
//    void on_organizationFilter_currentIndexChanged();
};

#endif // JOURNALHISTORYUSERSONWP_H

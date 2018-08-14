#ifndef ADDEDITNETWORKINTERFACE_H
#define ADDEDITNETWORKINTERFACE_H

#include <QtWidgets>
#include "ui_addeditnetworkinterface.h"

class InterfaceModel;

class AddEditNetworkInterface : public QDialog, private Ui::AddEditNetworkInterface
{
    Q_OBJECT
    
public:
    explicit AddEditNetworkInterface(QWidget *parent = 0, int networkDataId = 0, InterfaceModel *im = 0, bool editMode = false,
                                     const QModelIndex &index = QModelIndex());
private:
    int m_networkDataId;
    InterfaceModel *m_im;
    bool m_editMode;
    QModelIndex m_index;
    int autoIP, autoDns, autoWins;
    void setDefaultEditData();
    bool dataIsChanged();
    bool compareAuto(QCheckBox *autoCheck, int dbData);
protected:
    void changeEvent(QEvent *e);
private slots:
    void on_checkAutoIP_clicked(bool checked);
    void on_checkAutoDNS_clicked(bool checked);
    void on_checkAutoWINS_clicked(bool checked);
    void on_buttonSave_clicked();
    void on_buttonClose_clicked();
    void on_name_textEdited(const QString &arg1);
    void on_ip_textEdited();
    void on_maska_textEdited();
    void on_gate_textEdited();
    void on_dns_textEdited();
    void on_wins_textEdited();
    void on_buttonRevert_clicked();
};

#endif // ADDEDITNETWORKINTERFACE_H
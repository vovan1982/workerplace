#include <QtSql>
#include <QMessageBox>
#include "headers/addeditnetworkinterface.h"

AddEditNetworkInterface::AddEditNetworkInterface(QWidget *parent, int deviceId, QSqlTableModel *im, bool editMode, const QModelIndex &index) :
    QDialog(parent),
    m_deviceId(deviceId),
    m_im(im),
    m_editMode(editMode),
    m_index(index)
{
    setupUi(this);
    if(!editMode){
        autoIP = 1;
        autoDns = 1;
        autoWins = 1;
        buttonRevert->setVisible(false);
    }else
        setDefaultEditData();
}

void AddEditNetworkInterface::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void AddEditNetworkInterface::setDefaultEditData()
{
    name->setText(m_im->data(m_im->index(m_index.row(),2)).toString());
    note->setPlainText(m_im->data(m_im->index(m_index.row(),8)).toString());
    if(m_im->data(m_im->index(m_index.row(),9)).toInt() == 1){
        checkAutoIP->setChecked(true);
        on_checkAutoIP_clicked(true);
    }else{
        checkAutoIP->setChecked(false);
        on_checkAutoIP_clicked(false);
        ip->setText(m_im->data(m_im->index(m_index.row(),3)).toString());
        maska->setText(m_im->data(m_im->index(m_index.row(),4)).toString());
        gate->setText(m_im->data(m_im->index(m_index.row(),5)).toString());
        dns->setText(m_im->data(m_im->index(m_index.row(),6)).toString());
    }
    if(m_im->data(m_im->index(m_index.row(),9)).toInt() == 1 && m_im->data(m_im->index(m_index.row(),10)).toInt() == 0){
        checkAutoDNS->setChecked(false);
        on_checkAutoDNS_clicked(false);
        dns->setText(m_im->data(m_im->index(m_index.row(),6)).toString());
    }else if(m_im->data(m_im->index(m_index.row(),9)).toInt() == 1 && m_im->data(m_im->index(m_index.row(),10)).toInt() == 1){
        checkAutoDNS->setChecked(true);
        on_checkAutoDNS_clicked(true);
    }
    if(m_im->data(m_im->index(m_index.row(),11)).toInt() == 1){
        checkAutoWINS->setChecked(true);
        on_checkAutoWINS_clicked(true);
    }else{
        checkAutoWINS->setChecked(false);
        on_checkAutoWINS_clicked(false);
        wins->setText(m_im->data(m_im->index(m_index.row(),7)).toString());
    }
}

bool AddEditNetworkInterface::compareAuto(QCheckBox *autoCheck, int dbData)
{
    if(autoCheck->isChecked() && dbData == 1){
        return true;
    }else if(!autoCheck->isChecked() && dbData == 0){
        return true;
    }else
        return false;
}

bool AddEditNetworkInterface::dataIsChanged()
{
    if(name->text() != m_im->data(m_im->index(m_index.row(),2)).toString() || !compareAuto(checkAutoIP,m_im->data(m_im->index(m_index.row(),9)).toInt()) ||
            !compareAuto(checkAutoDNS,m_im->data(m_im->index(m_index.row(),10)).toInt()) ||
            !compareAuto(checkAutoWINS,m_im->data(m_im->index(m_index.row(),11)).toInt()) ||
            note->toPlainText() != m_im->data(m_im->index(m_index.row(),8)).toString())
        return true;
    if(m_im->data(m_im->index(m_index.row(),9)).toInt() == 0){
        if(m_im->data(m_im->index(m_index.row(),3)).toString() != ip->text() || m_im->data(m_im->index(m_index.row(),4)).toString() != maska->text() ||
                m_im->data(m_im->index(m_index.row(),5)).toString() != gate->text())
            return true;
    }
    if(m_im->data(m_im->index(m_index.row(),10)).toInt() == 0){
        if(m_im->data(m_im->index(m_index.row(),6)).toString() != dns->text())
            return true;
    }
    if(m_im->data(m_im->index(m_index.row(),11)).toInt() == 0){
        if(m_im->data(m_im->index(m_index.row(),7)).toString() != wins->text())
            return true;
    }
    return false;
}

void AddEditNetworkInterface::on_checkAutoIP_clicked(bool checked)
{
    if(!checked){
        ip->setEnabled(true);
        ip->clear();
        maska->setEnabled(true);
        maska->clear();
        gate->setEnabled(true);
        gate->clear();
        checkAutoDNS->setEnabled(false);
        if(checkAutoDNS->isChecked()){
            checkAutoDNS->setChecked(false);
            on_checkAutoDNS_clicked(false);
        }
    }else{
        ip->setEnabled(false);
        ip->setText(tr("Автоматически"));
        maska->setEnabled(false);
        maska->setText(tr("Автоматически"));
        gate->setEnabled(false);
        gate->setText(tr("Автоматически"));
        checkAutoDNS->setEnabled(true);
    }
    if(m_editMode){
        if(dataIsChanged()){
            buttonRevert->setEnabled(true);
            if(!name->text().isNull() && !name->text().isEmpty())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}

void AddEditNetworkInterface::on_checkAutoDNS_clicked(bool checked)
{
    if(!checked){
        dns->setEnabled(true);
        dns->clear();
    }else{
        dns->setEnabled(false);
        dns->setText(tr("Автоматически"));
    }
    if(m_editMode){
        if(dataIsChanged()){
            buttonRevert->setEnabled(true);
            if(!name->text().isNull() && !name->text().isEmpty())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}

void AddEditNetworkInterface::on_checkAutoWINS_clicked(bool checked)
{
    if(!checked){
        wins->setEnabled(true);
        wins->clear();
    }else{
        wins->setEnabled(false);
        wins->setText(tr("Автоматически"));
    }
    if(m_editMode){
        if(dataIsChanged()){
            buttonRevert->setEnabled(true);
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}

void AddEditNetworkInterface::on_buttonSave_clicked()
{
    if(!m_editMode){
        if(checkAutoIP->isChecked()){
            if(checkAutoDNS->isChecked()){
                if(checkAutoWINS->isChecked()){
                    m_im->insertRow(m_im->rowCount());
                    if(m_deviceId != -1)
                        m_im->setData(m_im->index(m_im->rowCount()-1,1),m_deviceId);
                    m_im->setData(m_im->index(m_im->rowCount()-1,2),name->text());
                    m_im->setData(m_im->index(m_im->rowCount()-1,3),tr("Авто"));
                    m_im->setData(m_im->index(m_im->rowCount()-1,4),tr("Авто"));
                    m_im->setData(m_im->index(m_im->rowCount()-1,5),tr("Авто"));
                    m_im->setData(m_im->index(m_im->rowCount()-1,6),tr("Авто"));
                    m_im->setData(m_im->index(m_im->rowCount()-1,7),tr("Авто"));
                    m_im->setData(m_im->index(m_im->rowCount()-1,8),note->toPlainText());
                    m_im->setData(m_im->index(m_im->rowCount()-1,9),1);
                    m_im->setData(m_im->index(m_im->rowCount()-1,10),1);
                    m_im->setData(m_im->index(m_im->rowCount()-1,11),1);
                }else{
                    m_im->insertRow(m_im->rowCount());
                    if(m_deviceId != -1)
                        m_im->setData(m_im->index(m_im->rowCount()-1,1),m_deviceId);
                    m_im->setData(m_im->index(m_im->rowCount()-1,2),name->text());
                    m_im->setData(m_im->index(m_im->rowCount()-1,3),tr("Авто"));
                    m_im->setData(m_im->index(m_im->rowCount()-1,4),tr("Авто"));
                    m_im->setData(m_im->index(m_im->rowCount()-1,5),tr("Авто"));
                    m_im->setData(m_im->index(m_im->rowCount()-1,6),tr("Авто"));
                    m_im->setData(m_im->index(m_im->rowCount()-1,7),wins->text());
                    m_im->setData(m_im->index(m_im->rowCount()-1,8),note->toPlainText());
                    m_im->setData(m_im->index(m_im->rowCount()-1,9),1);
                    m_im->setData(m_im->index(m_im->rowCount()-1,10),1);
                    m_im->setData(m_im->index(m_im->rowCount()-1,11),0);
                }
            }else{
                if(checkAutoWINS->isChecked()){
                    m_im->insertRow(m_im->rowCount());
                    if(m_deviceId != -1)
                        m_im->setData(m_im->index(m_im->rowCount()-1,1),m_deviceId);
                    m_im->setData(m_im->index(m_im->rowCount()-1,2),name->text());
                    m_im->setData(m_im->index(m_im->rowCount()-1,3),tr("Авто"));
                    m_im->setData(m_im->index(m_im->rowCount()-1,4),tr("Авто"));
                    m_im->setData(m_im->index(m_im->rowCount()-1,5),tr("Авто"));
                    m_im->setData(m_im->index(m_im->rowCount()-1,6),dns->text());
                    m_im->setData(m_im->index(m_im->rowCount()-1,7),tr("Авто"));
                    m_im->setData(m_im->index(m_im->rowCount()-1,8),note->toPlainText());
                    m_im->setData(m_im->index(m_im->rowCount()-1,9),1);
                    m_im->setData(m_im->index(m_im->rowCount()-1,10),0);
                    m_im->setData(m_im->index(m_im->rowCount()-1,11),1);
                }else{
                    m_im->insertRow(m_im->rowCount());
                    if(m_deviceId != -1)
                        m_im->setData(m_im->index(m_im->rowCount()-1,1),m_deviceId);
                    m_im->setData(m_im->index(m_im->rowCount()-1,2),name->text());
                    m_im->setData(m_im->index(m_im->rowCount()-1,3),tr("Авто"));
                    m_im->setData(m_im->index(m_im->rowCount()-1,4),tr("Авто"));
                    m_im->setData(m_im->index(m_im->rowCount()-1,5),tr("Авто"));
                    m_im->setData(m_im->index(m_im->rowCount()-1,6),dns->text());
                    m_im->setData(m_im->index(m_im->rowCount()-1,7),wins->text());
                    m_im->setData(m_im->index(m_im->rowCount()-1,8),note->toPlainText());
                    m_im->setData(m_im->index(m_im->rowCount()-1,9),1);
                    m_im->setData(m_im->index(m_im->rowCount()-1,10),0);
                    m_im->setData(m_im->index(m_im->rowCount()-1,11),0);
                }
            }
        }else{
            if(checkAutoWINS->isChecked()){
                m_im->insertRow(m_im->rowCount());
                if(m_deviceId != -1)
                    m_im->setData(m_im->index(m_im->rowCount()-1,1),m_deviceId);
                m_im->setData(m_im->index(m_im->rowCount()-1,2),name->text());
                m_im->setData(m_im->index(m_im->rowCount()-1,3),ip->text());
                m_im->setData(m_im->index(m_im->rowCount()-1,4),maska->text());
                m_im->setData(m_im->index(m_im->rowCount()-1,5),gate->text());
                m_im->setData(m_im->index(m_im->rowCount()-1,6),dns->text());
                m_im->setData(m_im->index(m_im->rowCount()-1,7),tr("Авто"));
                m_im->setData(m_im->index(m_im->rowCount()-1,8),note->toPlainText());
                m_im->setData(m_im->index(m_im->rowCount()-1,9),0);
                m_im->setData(m_im->index(m_im->rowCount()-1,10),0);
                m_im->setData(m_im->index(m_im->rowCount()-1,11),1);
            }else{
                m_im->insertRow(m_im->rowCount());
                if(m_deviceId != -1)
                    m_im->setData(m_im->index(m_im->rowCount()-1,1),m_deviceId);
                m_im->setData(m_im->index(m_im->rowCount()-1,2),name->text());
                m_im->setData(m_im->index(m_im->rowCount()-1,3),ip->text());
                m_im->setData(m_im->index(m_im->rowCount()-1,4),maska->text());
                m_im->setData(m_im->index(m_im->rowCount()-1,5),gate->text());
                m_im->setData(m_im->index(m_im->rowCount()-1,6),dns->text());
                m_im->setData(m_im->index(m_im->rowCount()-1,7),wins->text());
                m_im->setData(m_im->index(m_im->rowCount()-1,8),note->toPlainText());
                m_im->setData(m_im->index(m_im->rowCount()-1,9),0);
                m_im->setData(m_im->index(m_im->rowCount()-1,10),0);
                m_im->setData(m_im->index(m_im->rowCount()-1,11),0);
            }
        }
    }else{
        if(checkAutoIP->isChecked()){
            if(checkAutoDNS->isChecked()){
                if(checkAutoWINS->isChecked()){
                    if(m_deviceId != -1)
                        m_im->setData(m_im->index(m_im->rowCount()-1,1),m_deviceId);
                    m_im->setData(m_im->index(m_index.row(),2),name->text());
                    m_im->setData(m_im->index(m_index.row(),3),tr("Авто"));
                    m_im->setData(m_im->index(m_index.row(),4),tr("Авто"));
                    m_im->setData(m_im->index(m_index.row(),5),tr("Авто"));
                    m_im->setData(m_im->index(m_index.row(),6),tr("Авто"));
                    m_im->setData(m_im->index(m_index.row(),7),tr("Авто"));
                    m_im->setData(m_im->index(m_index.row(),8),note->toPlainText());
                    m_im->setData(m_im->index(m_index.row(),9),1);
                    m_im->setData(m_im->index(m_index.row(),10),1);
                    m_im->setData(m_im->index(m_index.row(),11),1);
                }else{
                    if(m_deviceId != -1)
                        m_im->setData(m_im->index(m_im->rowCount()-1,1),m_deviceId);
                    m_im->setData(m_im->index(m_index.row(),2),name->text());
                    m_im->setData(m_im->index(m_index.row(),3),tr("Авто"));
                    m_im->setData(m_im->index(m_index.row(),4),tr("Авто"));
                    m_im->setData(m_im->index(m_index.row(),5),tr("Авто"));
                    m_im->setData(m_im->index(m_index.row(),6),tr("Авто"));
                    m_im->setData(m_im->index(m_index.row(),7),wins->text());
                    m_im->setData(m_im->index(m_index.row(),8),note->toPlainText());
                    m_im->setData(m_im->index(m_index.row(),9),1);
                    m_im->setData(m_im->index(m_index.row(),10),1);
                    m_im->setData(m_im->index(m_index.row(),11),0);
                }
            }else{
                if(checkAutoWINS->isChecked()){
                    if(m_deviceId != -1)
                        m_im->setData(m_im->index(m_im->rowCount()-1,1),m_deviceId);
                    m_im->setData(m_im->index(m_index.row(),2),name->text());
                    m_im->setData(m_im->index(m_index.row(),3),tr("Авто"));
                    m_im->setData(m_im->index(m_index.row(),4),tr("Авто"));
                    m_im->setData(m_im->index(m_index.row(),5),tr("Авто"));
                    m_im->setData(m_im->index(m_index.row(),6),dns->text());
                    m_im->setData(m_im->index(m_index.row(),7),tr("Авто"));
                    m_im->setData(m_im->index(m_index.row(),8),note->toPlainText());
                    m_im->setData(m_im->index(m_index.row(),9),1);
                    m_im->setData(m_im->index(m_index.row(),10),0);
                    m_im->setData(m_im->index(m_index.row(),11),1);
                }else{
                    if(m_deviceId != -1)
                        m_im->setData(m_im->index(m_im->rowCount()-1,1),m_deviceId);
                    m_im->setData(m_im->index(m_index.row(),2),name->text());
                    m_im->setData(m_im->index(m_index.row(),3),tr("Авто"));
                    m_im->setData(m_im->index(m_index.row(),4),tr("Авто"));
                    m_im->setData(m_im->index(m_index.row(),5),tr("Авто"));
                    m_im->setData(m_im->index(m_index.row(),6),dns->text());
                    m_im->setData(m_im->index(m_index.row(),7),wins->text());
                    m_im->setData(m_im->index(m_index.row(),8),note->toPlainText());
                    m_im->setData(m_im->index(m_index.row(),9),1);
                    m_im->setData(m_im->index(m_index.row(),10),0);
                    m_im->setData(m_im->index(m_index.row(),11),0);
                }
            }
        }else{
            if(checkAutoWINS->isChecked()){
                if(m_deviceId != -1)
                    m_im->setData(m_im->index(m_im->rowCount()-1,1),m_deviceId);
                m_im->setData(m_im->index(m_index.row(),2),name->text());
                m_im->setData(m_im->index(m_index.row(),3),ip->text());
                m_im->setData(m_im->index(m_index.row(),4),maska->text());
                m_im->setData(m_im->index(m_index.row(),5),gate->text());
                m_im->setData(m_im->index(m_index.row(),6),dns->text());
                m_im->setData(m_im->index(m_index.row(),7),tr("Авто"));
                m_im->setData(m_im->index(m_index.row(),8),note->toPlainText());
                m_im->setData(m_im->index(m_index.row(),9),0);
                m_im->setData(m_im->index(m_index.row(),10),0);
                m_im->setData(m_im->index(m_index.row(),11),1);
            }else{
                if(m_deviceId != -1)
                    m_im->setData(m_im->index(m_im->rowCount()-1,1),m_deviceId);
                m_im->setData(m_im->index(m_index.row(),2),name->text());
                m_im->setData(m_im->index(m_index.row(),3),ip->text());
                m_im->setData(m_im->index(m_index.row(),4),maska->text());
                m_im->setData(m_im->index(m_index.row(),5),gate->text());
                m_im->setData(m_im->index(m_index.row(),6),dns->text());
                m_im->setData(m_im->index(m_index.row(),7),wins->text());
                m_im->setData(m_im->index(m_index.row(),8),note->toPlainText());
                m_im->setData(m_im->index(m_index.row(),9),0);
                m_im->setData(m_im->index(m_index.row(),10),0);
                m_im->setData(m_im->index(m_index.row(),11),0);
            }
        }
    }
    accept();
}

void AddEditNetworkInterface::on_buttonClose_clicked()
{
    reject();
}

void AddEditNetworkInterface::on_name_textEdited(const QString &arg1)
{
    if(!m_editMode){
        if(!arg1.isNull() && !arg1.isEmpty())
            buttonSave->setEnabled(true);
        else
            buttonSave->setEnabled(false);
    }else{
        if(dataIsChanged()){
            buttonRevert->setEnabled(true);
            if(!arg1.isNull() && !arg1.isEmpty())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}

void AddEditNetworkInterface::on_ip_textEdited()
{
    if(m_editMode){
        if(dataIsChanged()){
            buttonRevert->setEnabled(true);
            if(!name->text().isNull() && !name->text().isEmpty())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}

void AddEditNetworkInterface::on_maska_textEdited()
{
    if(m_editMode){
        if(dataIsChanged()){
            buttonRevert->setEnabled(true);
            if(!name->text().isNull() && !name->text().isEmpty())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}

void AddEditNetworkInterface::on_gate_textEdited()
{
    if(m_editMode){
        if(dataIsChanged()){
            buttonRevert->setEnabled(true);
            if(!name->text().isNull() && !name->text().isEmpty())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}

void AddEditNetworkInterface::on_dns_textEdited()
{
    if(m_editMode){
        if(dataIsChanged()){
            buttonRevert->setEnabled(true);
            if(!name->text().isNull() && !name->text().isEmpty())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}

void AddEditNetworkInterface::on_wins_textEdited()
{
    if(m_editMode){
        if(dataIsChanged()){
            buttonRevert->setEnabled(true);
            if(!name->text().isNull() && !name->text().isEmpty())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}

void AddEditNetworkInterface::on_buttonRevert_clicked()
{
    setDefaultEditData();
}

void AddEditNetworkInterface::on_note_cursorPositionChanged()
{
    if(m_editMode){
        if(dataIsChanged()){
            buttonRevert->setEnabled(true);
            if(!name->text().isNull() && !name->text().isEmpty())
                buttonSave->setEnabled(true);
            else
                buttonSave->setEnabled(false);
        }else{
            buttonRevert->setEnabled(false);
            buttonSave->setEnabled(false);
        }
    }
}

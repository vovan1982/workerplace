#include <QtSql>
#include <QMessageBox>
#include "headers/addeditnetworkinterface.h"
#include "headers/interfacemodel.h"

AddEditNetworkInterface::AddEditNetworkInterface(QWidget *parent, int networkDataId, InterfaceModel *im, bool editMode, const QModelIndex &index) :
    QDialog(parent),
    m_networkDataId(networkDataId),
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
    if(m_im->data(m_im->index(m_index.row(),8)).toInt() == 1){
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
    if(m_im->data(m_im->index(m_index.row(),8)).toInt() == 1 && m_im->data(m_im->index(m_index.row(),9)).toInt() == 0){
        checkAutoDNS->setChecked(false);
        on_checkAutoDNS_clicked(false);
        dns->setText(m_im->data(m_im->index(m_index.row(),6)).toString());
    }else if(m_im->data(m_im->index(m_index.row(),8)).toInt() == 1 && m_im->data(m_im->index(m_index.row(),9)).toInt() == 1){
        checkAutoDNS->setChecked(true);
        on_checkAutoDNS_clicked(true);
    }
    if(m_im->data(m_im->index(m_index.row(),10)).toInt() == 1){
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
    if(name->text() != m_im->data(m_im->index(m_index.row(),2)).toString() || !compareAuto(checkAutoIP,m_im->data(m_im->index(m_index.row(),8)).toInt()) ||
            !compareAuto(checkAutoDNS,m_im->data(m_im->index(m_index.row(),9)).toInt()) ||
            !compareAuto(checkAutoWINS,m_im->data(m_im->index(m_index.row(),10)).toInt()))
        return true;
    if(m_im->data(m_im->index(m_index.row(),8)).toInt() == 0){
        if(m_im->data(m_im->index(m_index.row(),3)).toString() != ip->text() || m_im->data(m_im->index(m_index.row(),4)).toString() != maska->text() ||
                m_im->data(m_im->index(m_index.row(),5)).toString() != gate->text())
            return true;
    }
    if(m_im->data(m_im->index(m_index.row(),9)).toInt() == 0){
        if(m_im->data(m_im->index(m_index.row(),6)).toString() != dns->text())
            return true;
    }
    if(m_im->data(m_im->index(m_index.row(),10)).toInt() == 0){
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
            buttonSave->setEnabled(true);
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
            buttonSave->setEnabled(true);
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
            buttonSave->setEnabled(true);
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
                    m_im->insertRecord(m_networkDataId,name->text());
                    if (m_im->lastError().type() != QSqlError::NoError)
                    {
                        QMessageBox::warning(this, tr("Ошибка"),
                                             tr("Не удалось сохранить данные интерфейса:\n %1")
                                                 .arg(m_im->lastError().text()),
                                                 tr("Закрыть"));
                        return;
                    }
                }else{
                    m_im->insertRecord(m_networkDataId,name->text(),"","","","",wins->text(),1,1,0);
                    if (m_im->lastError().type() != QSqlError::NoError)
                    {
                        QMessageBox::warning(this, tr("Ошибка"),
                                             tr("Не удалось сохранить данные интерфейса:\n %1")
                                                 .arg(m_im->lastError().text()),
                                                 tr("Закрыть"));
                        return;
                    }
                }
            }else{
                if(checkAutoWINS->isChecked()){
                    m_im->insertRecord(m_networkDataId,name->text(),"","","",dns->text(),"",1,0,1);
                    if (m_im->lastError().type() != QSqlError::NoError)
                    {
                        QMessageBox::warning(this, tr("Ошибка"),
                                             tr("Не удалось сохранить данные интерфейса:\n %1")
                                                 .arg(m_im->lastError().text()),
                                                 tr("Закрыть"));
                        return;
                    }
                }else{
                    m_im->insertRecord(m_networkDataId,name->text(),"","","",dns->text(),wins->text(),1,0,0);
                    if (m_im->lastError().type() != QSqlError::NoError)
                    {
                        QMessageBox::warning(this, tr("Ошибка"),
                                             tr("Не удалось сохранить данные интерфейса:\n %1")
                                                 .arg(m_im->lastError().text()),
                                                 tr("Закрыть"));
                        return;
                    }
                }
            }
        }else{
            if(checkAutoWINS->isChecked()){
                m_im->insertRecord(m_networkDataId,name->text(),ip->text(),maska->text(),gate->text(),dns->text(),"",0,0,1);
                if (m_im->lastError().type() != QSqlError::NoError)
                {
                    QMessageBox::warning(this, tr("Ошибка"),
                                         tr("Не удалось сохранить данные интерфейса:\n %1")
                                             .arg(m_im->lastError().text()),
                                             tr("Закрыть"));
                    return;
                }
            }else{
                m_im->insertRecord(m_networkDataId,name->text(),ip->text(),maska->text(),gate->text(),dns->text(),wins->text(),0,0,0);
                if (m_im->lastError().type() != QSqlError::NoError)
                {
                    QMessageBox::warning(this, tr("Ошибка"),
                                         tr("Не удалось сохранить данные интерфейса:\n %1")
                                             .arg(m_im->lastError().text()),
                                             tr("Закрыть"));
                    return;
                }
            }
        }
    }else{
        int interfaceId = m_im->data(m_im->index(m_index.row(),0)).toInt();
        if(checkAutoIP->isChecked()){
            if(checkAutoDNS->isChecked()){
                if(checkAutoWINS->isChecked()){
                    m_im->updateRecord(interfaceId,0,name->text());
                    if (m_im->lastError().type() != QSqlError::NoError)
                    {
                        QMessageBox::warning(this, tr("Ошибка"),
                                             tr("Не удалось сохранить данные интерфейса:\n %1")
                                                 .arg(m_im->lastError().text()),
                                                 tr("Закрыть"));
                        return;
                    }
                }else{
                    m_im->updateRecord(interfaceId,0,name->text(),"","","","",wins->text(),1,1,0);
                    if (m_im->lastError().type() != QSqlError::NoError)
                    {
                        QMessageBox::warning(this, tr("Ошибка"),
                                             tr("Не удалось сохранить данные интерфейса:\n %1")
                                                 .arg(m_im->lastError().text()),
                                                 tr("Закрыть"));
                        return;
                    }
                }
            }else{
                if(checkAutoWINS->isChecked()){
                    m_im->updateRecord(interfaceId,0,name->text(),"","","",dns->text(),"",1,0,1);
                    if (m_im->lastError().type() != QSqlError::NoError)
                    {
                        QMessageBox::warning(this, tr("Ошибка"),
                                             tr("Не удалось сохранить данные интерфейса:\n %1")
                                                 .arg(m_im->lastError().text()),
                                                 tr("Закрыть"));
                        return;
                    }
                }else{
                    m_im->updateRecord(interfaceId,0,name->text(),"","","",dns->text(),wins->text(),1,0,0);
                    if (m_im->lastError().type() != QSqlError::NoError)
                    {
                        QMessageBox::warning(this, tr("Ошибка"),
                                             tr("Не удалось сохранить данные интерфейса:\n %1")
                                                 .arg(m_im->lastError().text()),
                                                 tr("Закрыть"));
                        return;
                    }
                }
            }
        }else{
            if(checkAutoWINS->isChecked()){
                m_im->updateRecord(interfaceId,0,name->text(),ip->text(),maska->text(),gate->text(),dns->text(),"",0,0,1);
                if (m_im->lastError().type() != QSqlError::NoError)
                {
                    QMessageBox::warning(this, tr("Ошибка"),
                                         tr("Не удалось сохранить данные интерфейса:\n %1")
                                             .arg(m_im->lastError().text()),
                                             tr("Закрыть"));
                    return;
                }
            }else{
                m_im->updateRecord(interfaceId,0,name->text(),ip->text(),maska->text(),gate->text(),dns->text(),wins->text(),0,0,0);
                if (m_im->lastError().type() != QSqlError::NoError)
                {
                    QMessageBox::warning(this, tr("Ошибка"),
                                         tr("Не удалось сохранить данные интерфейса:\n %1")
                                             .arg(m_im->lastError().text()),
                                             tr("Закрыть"));
                    return;
                }
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
            buttonSave->setEnabled(true);
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
            buttonSave->setEnabled(true);
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
            buttonSave->setEnabled(true);
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
            buttonSave->setEnabled(true);
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
            buttonSave->setEnabled(true);
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

#include <QtSql>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QFileDialog>
#include "headers/userimport.h"
#include "headers/edittable.h"
#include "headers/userreportparser.h"
#include "headers/userimportcomparisonorganization.h"

UserImport::UserImport(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    fileForImport->setVisibleClearButtron(false);
    populateCBox("CodLoginType","logintype","",tr("<Выберите тип логина>"),loginType);
    comparisonOrganization = QMap<QString,QVariant>();
}

void UserImport::changeEvent(QEvent *e)
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

void UserImport::populateCBox(const QString &idName, const QString &tableName,
                            const QString &filter, const QString &nullStr, QComboBox *cBox)
{
    cBox->clear();
    cBox->addItem(nullStr);
    QSqlQuery query;
    if(filter.isNull() || filter.isEmpty())
        query.exec(QString("SELECT %2,name FROM %1;").arg(tableName).arg(idName));
    else
        query.exec(QString("SELECT %2,name FROM %1 WHERE %3;").arg(tableName).arg(idName).arg(filter));
    if (query.lastError().type() != QSqlError::NoError)
    {
        QMessageBox::information(this, tr("Ошибка"),
                                 tr("Не удалось заполнить комбобокс %2:\n %1")
                                 .arg(query.lastError().text())
                                 .arg(cBox->objectName()),
                                 tr("Закрыть"));
        return;
    }
    cBox->setMaxCount(query.size()+1);
    while(query.next())
        cBox->addItem(query.value(1).toString(),query.value(0).toInt());
}

bool UserImport::acceptImport()
{
    if(!fileForImport->text().isNull() && !fileForImport->text().isEmpty() && loginType->currentIndex() > 0 && !comparisonOrganization.isEmpty())
        return true;
    else
        return false;
}

void UserImport::setComparisonOrganization(const QMap<QString,QVariant> &comparisons)
{
    comparisonOrganization = comparisons;
    buttonComparisonOrganization->setIcon(QIcon(":/16x16/apply/ico/agt_action_success_16x16.png"));
    buttonImport->setEnabled(acceptImport());
}

void UserImport::on_buttonClose_clicked()
{
    QWidget *parent = this->parentWidget();
    while (parent) {
        QMdiSubWindow *subWindow = qobject_cast<QMdiSubWindow *>(parent);
        if (subWindow) {
            subWindow->close();
            break;
        }
        parent = parent->parentWidget();
    }
}

void UserImport::on_fileForImport_runButtonClicked()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("Выберите файл"),
                                              QString(), tr("XML files (*.xml);;All Files (*)"));
    if (fn.isEmpty()) return;
    if(!fn.isNull() && !fn.isEmpty()){
        fileForImport->setText(fn);
        importFileStatus->setPixmap(QPixmap(":/16x16/apply/ico/agt_action_success_16x16.png"));
        loginType->setEnabled(true);
        buttonLoginTypeEdit->setEnabled(true);
        buttonComparisonOrganization->setEnabled(true);
        buttonComparisonOrganization->setIcon(QIcon(":/16x16/cancelDel/ico/button_cancel_16x16 (1).png"));
        comparisonOrganization.clear();
    }
    buttonImport->setEnabled(acceptImport());
}

void UserImport::on_buttonLoginTypeEdit_clicked()
{
    CeditTable edittable(this,QString("logintype"));
    edittable.setWindowTitle(tr("Редактирование типов логинов"));
    edittable.exec();
    loginType->clear();
    populateCBox("CodLoginType","logintype","",tr("<Выберите тип логина>"),loginType);
}

void UserImport::on_loginType_currentIndexChanged(int index)
{
    if(index > 0)
        loginTypeStatus->setPixmap(QPixmap(":/16x16/apply/ico/agt_action_success_16x16.png"));
    else
        loginTypeStatus->setPixmap(QPixmap(":/16x16/cancelDel/ico/button_cancel_16x16 (1).png"));
    buttonImport->setEnabled(acceptImport());
}

void UserImport::on_buttonComparisonOrganization_clicked()
{
    UserImportComparisonOrganization *uico = new UserImportComparisonOrganization(this,fileForImport->text(), comparisonOrganization);
    connect(uico,SIGNAL(comparisonResult(QMap<QString,QVariant>)),this,SLOT(setComparisonOrganization(QMap<QString,QVariant>)));
    uico->exec();
}

void UserImport::on_buttonImport_clicked()
{
    QList<QString> attr;
    attr<<loginType->itemData(loginType->currentIndex()).toString();
    UserReportParser  handler(attr,comparisonOrganization);
    QFile             file(fileForImport->text());
    QXmlInputSource   source(&file);
    QXmlSimpleReader  reader;

    reader.setContentHandler(&handler);
    reader.parse(source);
    if (handler.errorString().isEmpty()){
        QMessageBox::information(this, tr("Импорт"), tr("Импорт пользователей выполнен успешно."), tr("Закрыть"));
        fileForImport->clearButton_clicked();
        importFileStatus->setPixmap(QPixmap(":/16x16/cancelDel/ico/button_cancel_16x16 (1).png"));
        loginType->setCurrentIndex(0);
        on_loginType_currentIndexChanged(0);
        loginType->setEnabled(false);
        buttonLoginTypeEdit->setEnabled(false);
        buttonComparisonOrganization->setEnabled(false);
        buttonComparisonOrganization->setIcon(QIcon(":/16x16/cancelDel/ico/button_cancel_16x16 (1).png"));
        comparisonOrganization.clear();
        buttonImport->setEnabled(acceptImport());
    }else
        QMessageBox::warning(this, tr("Ошибка"), tr("Во время импорта произошла ошибка:\n%1")
                             .arg(handler.errorString()), tr("Закрыть"));
}

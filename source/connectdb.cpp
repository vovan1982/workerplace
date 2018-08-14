#include <QDesktopWidget>
#include "headers/connectdb.h"

connectDB::connectDB(QWidget *parent) :
    QDialog(parent){
    setupUi(this);


    // Устанавливаем предыдущие значения для подключения к БД
    QSettings settings( QApplication::applicationDirPath()+"/connection.ini", QSettings::IniFormat );
    dataBase->addItem(settings.value("Connection/DatabaseName","").toString());
    login->setText(settings.value("Connection/UserName","").toString());
    port->setValue(settings.value("Connection/Port",3306).toInt());
    host->setText(settings.value("Connection/HostName","localhost").toString());
    pass->setText(settings.value("Connection/Password","").toString());

    // перемещаем диалог в центр экрана
    setGeometry(QStyle::alignedRect(
                    Qt::LeftToRight,
                    Qt::AlignCenter,
                    size(),
                    qApp->desktop()->availableGeometry()
                ));
    //======================================
}

// Нажата кнопка подключения к базе данных.
void connectDB::on_connect_DB_clicked()
{
    if(dataBase->currentText() == NULL){
        QMessageBox::critical(this, tr("Ошибка"),
                                 tr("Не заполненно поле \"База Данных\"!!!"),
                                 tr("Закрыть"));
        return;
    }
    if(login->text() == NULL){
        QMessageBox::critical(this, tr("Ошибка"),
                              tr("Не заполненно поле \"Логин\"!!!"),
                              tr("Закрыть"));
        return;
    }
    if(pass->text() == NULL){
        QMessageBox::critical(this, tr("Ошибка"),
                              tr("Не заполненно поле \"Пароль\"!!!"),
                              tr("Закрыть"));
        return;
    }
    if(host->text() == NULL){
        QMessageBox::critical(this, tr("Ошибка"),
                              tr("Не указан сервер для подключения!!!"),
                              tr("Закрыть"));
        return;
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName(dataBase->currentText());
    db.setHostName(host->text());
    db.setPort(port->value());
    db.setUserName(login->text());
    db.setPassword(pass->text());
    if (!db.open()) {
        QMessageBox::critical(this, tr("Ошибка"),
                                 tr("ОШИБКА АВТОРИЗАЦИИ!!!:\n %1")
                                 .arg(db.lastError().text()),
                                 tr("Закрыть"));
        return;
    }
    db.exec("SET NAMES utf8;");

    QSettings settings( QApplication::applicationDirPath()+"/connection.ini", QSettings::IniFormat );
    settings.beginGroup( "Connection" );
    settings.setValue( "DatabaseName", dataBase->currentText() );
    settings.setValue( "HostName", host->text() );
    settings.setValue( "Port", port->value() );
    settings.setValue( "UserName", login->text() );
    settings.endGroup();
    connectDB::accept();
}
void connectDB::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

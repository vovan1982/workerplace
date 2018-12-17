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
    QSqlDatabase db;
    if(QSqlDatabase::connectionNames().contains("qt_sql_default_connection")){
        db = QSqlDatabase::database();
    }else{
        db = QSqlDatabase::addDatabase("QMYSQL");
    }
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

    //Проверяем версию базы данных, если при проверки версии ДБ произошла ошибка или версия не соответствует, вход в программу не выполняется
    QSqlQuery query;
    query.exec("SELECT DbVersion FROM Settings");
    if (query.lastError().type() != QSqlError::NoError){
        QMessageBox::critical(this, tr("Ошибка"),
                                 tr("Не удалось получить версию базы данных!!!\n %1")
                                 .arg(query.lastError().text()),
                                 tr("Закрыть"));
        return;
    }
    if(query.size() > 0){
        query.next();
        if(query.value(0).toInt() != 1){
            QMessageBox::critical(this, tr("Ошибка"),
                                     tr("Текущая версия базы данных не соответствует требуемой!!!\nОбновите программу или базу данных."),
                                     tr("Закрыть"));
            return;
        }
    }else{
        QMessageBox::critical(this, tr("Ошибка"),
                                 tr("Не удалось получить версию базы данных!!!"),
                                 tr("Закрыть"));
        return;
    }

    // Сохраняем текущие данные в файл
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

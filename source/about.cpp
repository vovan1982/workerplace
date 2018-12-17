#include "headers/about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    ui->qtVersion->setText(QString("%1%2").arg(ui->qtVersion->text()).arg(qVersion()));
    ui->version->setText(QString("%1%2").arg(ui->version->text()).arg(QApplication::applicationVersion()));
}

About::~About()
{
    delete ui;
}

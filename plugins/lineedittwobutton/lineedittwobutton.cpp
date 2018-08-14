#include <QResizeEvent>
#include "plugins/lineedittwobutton/lineedittwobutton.h"

LineEditTwoButton::LineEditTwoButton(QWidget *parent)
    :QLineEdit(parent)
{
    setStyleSheet("LineEditTwoButton { "
                  " padding-right:50px; "
                  " } ");
    additionalData = QVariant();

    runButton = new QToolButton(this);
    runButton->setGeometry(this->width()-26,1,25,height()-1);
    runButton->setCursor(Qt::ArrowCursor);
    setIcoRunButton(QIcon(":/image/selectall_24x24.png"));
    connect(runButton,SIGNAL(clicked()),SIGNAL(runButtonClicked()));

    clearButton = new QToolButton(this);
    clearButton->setGeometry(this->width()-52,1,25,height());
    setIcoClearButton(QIcon(":/image/clear_left_16x16.png"));
    clearButton->setCursor(Qt::ArrowCursor);
    connect(clearButton,SIGNAL(clicked()),SLOT(clearButton_clicked()));
    connect(clearButton,SIGNAL(clicked()),SIGNAL(clearButtonClicked()));
    clearButton->setStyleSheet("border: 1px;");
}
LineEditTwoButton::LineEditTwoButton(const QString &contents,QWidget *parent)
    :QLineEdit(contents,parent)
{
    setStyleSheet("LineEditTwoButton { "
                  " padding-right:50px; "
                  " } ");
    additionalData = QVariant();

    runButton = new QToolButton(this);
    runButton->setGeometry(this->width()-26,1,25,height()-1);
    runButton->setCursor(Qt::ArrowCursor);
    setIcoRunButton(QIcon(":/image/selectall_24x24.png"));
    connect(runButton,SIGNAL(clicked()),SIGNAL(runButtonClicked()));

    clearButton = new QToolButton(this);
    clearButton->setGeometry(this->width()-52,1,25,height());
    setIcoClearButton(QIcon(":/image/clear_left_16x16.png"));
    clearButton->setCursor(Qt::ArrowCursor);
    connect(clearButton,SIGNAL(clicked()),SLOT(clearButton_clicked()));
    connect(clearButton,SIGNAL(clicked()),SIGNAL(clearButtonClicked()));
    clearButton->setStyleSheet("border: 1px;");
}
void LineEditTwoButton::resizeEvent(QResizeEvent *e)
{
    runButton->setGeometry(e->size().width()-26,1,25,e->size().height()-1);
    clearButton->setGeometry(e->size().width()-52,1,25,e->size().height());
}
void LineEditTwoButton::setData(const QVariant &data)
{
    additionalData = data;
}
void LineEditTwoButton::clearButton_clicked()
{
    setText("");
    additionalData = QVariant();
}
void LineEditTwoButton::setIcoRunButton(const QIcon &ico)
{
    runButton->setIcon(ico);
}
void LineEditTwoButton::setIcoClearButton(const QIcon &ico)
{
    clearButton->setIcon(ico);
}
QVariant LineEditTwoButton::data()
{
    return additionalData;
}
void LineEditTwoButton::setEnabledClearButtron(bool enabled)
{
    clearButton->setEnabled(enabled);
}
void LineEditTwoButton::setDisabledClearButtron(bool disabled)
{
    clearButton->setDisabled(disabled);
}
void LineEditTwoButton::setEnabledRunButtron(bool enabled)
{
    runButton->setEnabled(enabled);
}
void LineEditTwoButton::setDisabledRunButtron(bool disabled)
{
    runButton->setDisabled(disabled);
}
void LineEditTwoButton::setVisibleClearButtron(bool visible)
{
    clearButton->setVisible(visible);
    if(visible)
        setStyleSheet("LineEditTwoButton { "
                      " padding-right:50px; "
                      " } ");
    else
        setStyleSheet("LineEditTwoButton { "
                      " padding-right:25px; "
                      " } ");
}

#include "headers/loadindicator.h"
#include "headers/waitingspinnerwidget.h"

LoadIndicator::LoadIndicator(QObject *parent, const QString &text) :
    QObject(parent),
    m_text(text)
{
    running = false;
    if(parent != nullptr){
        parentWidget = qobject_cast<QWidget *>(parent);
    }else{
        parentWidget = nullptr;
    }
    animation = nullptr;
}

LoadIndicator::~LoadIndicator()
{
}

void LoadIndicator::setParent(QObject *parent)
{
    parentWidget = qobject_cast<QWidget *>(parent);
}

void LoadIndicator::start()
{
    if(parentWidget != nullptr && animation == nullptr){
        animation = new QWidget(parentWidget);
        QHBoxLayout *layout = new QHBoxLayout(animation);
        WaitingSpinnerWidget* spinner = new WaitingSpinnerWidget(animation,false);
        spinner->start();
        layout->addWidget(spinner);
        if(!m_text.isNull() && !m_text.isEmpty()){
            QLabel *text = new QLabel(animation);
            text->setText(m_text);
            text->setStyleSheet("font-weight: bold; color: black");
            layout->addWidget(text);
        }
        animation->setAttribute(Qt::WA_DeleteOnClose);
        animation->adjustSize();
        windowSize = animation->size();
        screenWidth = parentWidget->width();
        screenHeight = parentWidget->height();
        width = windowSize.width();
        height = windowSize.height();
        x = (screenWidth - width) / 2;
        y = (screenHeight - height) / 2;
        animation->move(x,y);
        animation->show();
        running = true;
    }
}

void LoadIndicator::stop()
{
    if(animation != nullptr){
        animation->close();
        animation = nullptr;
        running = false;
    }
}

void LoadIndicator::updatePosition()
{
    if(parentWidget != nullptr && animation != nullptr){
        screenWidth = parentWidget->width();
        screenHeight = parentWidget->height();
        x = (screenWidth - width) / 2;
        y = (screenHeight - height) / 2;
        animation->move(x,y);
    }
}

void LoadIndicator::setText(const QString &text)
{
    m_text = text;
}

bool LoadIndicator::isRunning()
{
    return running;
}

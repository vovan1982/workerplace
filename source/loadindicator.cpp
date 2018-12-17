#include "loadindicator.h"

LoadIndicator::LoadIndicator(QObject *parent, const QString &animationPath, const QString &text) :
    QObject(parent),
    m_animationPath(animationPath),
    m_text(text)
{
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
    if(parentWidget != nullptr){
        animation = new QWidget(parentWidget);
        QHBoxLayout *layout = new QHBoxLayout(animation);
        QMovie *movie = new QMovie(m_animationPath);
        movie->setParent(animation);
        QLabel *gif = new QLabel(animation);
        gif->setMovie(movie);
        movie->start();
        layout->addWidget(gif);
        if(!m_text.isNull() && !m_text.isEmpty()){
            QLabel *text = new QLabel(animation);
            text->setText(m_text);
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
    }
}

void LoadIndicator::stop()
{
    if(animation != nullptr){
        animation->close();
        animation = nullptr;
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

void LoadIndicator::setAnimationPath(const QString &path)
{
    m_animationPath = path;
}

void LoadIndicator::setText(const QString &text)
{
    m_text = text;
}

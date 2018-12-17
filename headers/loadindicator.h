#ifndef LOADINDICATOR_H
#define LOADINDICATOR_H

#include <QObject>
#include <QtWidgets>

class LoadIndicator : public QObject
{
    Q_OBJECT
public:
    explicit LoadIndicator(QObject *parent = nullptr, const QString &animationPath = "", const QString &text = "");
    ~LoadIndicator();
    void setParent(QObject *parent = nullptr);
    void updatePosition();
    void setAnimationPath(const QString &path);
    void setText(const QString &text);

private:
    int screenWidth, width;
    int screenHeight, height;
    int x, y;
    QSize windowSize;
    QWidget *parentWidget, *animation;
    QString m_animationPath, m_text;

signals:

public slots:
    void start();
    void stop();
};

#endif // LOADINDICATOR_H

#ifndef LOADINDICATOR_H
#define LOADINDICATOR_H

#include <QObject>
#include <QtWidgets>

class LoadIndicator : public QObject
{
    Q_OBJECT
public:
    explicit LoadIndicator(QObject *parent = nullptr, const QString &text = "");
    ~LoadIndicator();
    void setParent(QObject *parent = nullptr);
    void updatePosition();
    void setText(const QString &text);
    bool isRunning();

private:
    int screenWidth, width;
    int screenHeight, height;
    int x, y;
    bool running;
    QSize windowSize;
    QWidget *parentWidget, *animation;
    QString m_text;

signals:

public slots:
    void start();
    void stop();
};

#endif // LOADINDICATOR_H

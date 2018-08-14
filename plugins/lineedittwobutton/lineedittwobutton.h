#ifndef LINEEDITTWOBUTTON_H
#define LINEEDITTWOBUTTON_H

#include <QVariant>
#include <QLineEdit>
#include <QToolButton>
#include <QtWidgets/QWidget>

class LineEditTwoButton : public QLineEdit
{
    Q_OBJECT

public:
    explicit LineEditTwoButton(QWidget *parent = 0);
    explicit LineEditTwoButton(const QString & contents = "",QWidget *parent = 0);
    void setData(const QVariant &data);
    void setIcoRunButton(const QIcon &ico);
    void setIcoClearButton(const QIcon &ico);
    QVariant data();
private:
    QToolButton *runButton;
    QToolButton *clearButton;
    QVariant additionalData;
protected:
    void resizeEvent(QResizeEvent *);
signals:
    void clearButtonClicked();
    void runButtonClicked();
public slots:
    void setEnabledClearButtron(bool enabled);
    void setDisabledClearButtron(bool disabled);
    void setEnabledRunButtron(bool enabled);
    void setDisabledRunButtron(bool disabled);
    void setVisibleClearButtron(bool visible);
    void clearButton_clicked();
};

#endif // LINEEDITTWOBUTTON_H

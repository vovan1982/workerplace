#ifndef CEDITTABLEVIEW_H
#define CEDITTABLEVIEW_H

#include <QTableView>

class CeditTableView : public QTableView {
    Q_OBJECT
public:
    CeditTableView(QWidget *parent = 0);
private:
    virtual void resizeEvent(QResizeEvent *event);
};

#endif // CEDITTABLEVIEW_H

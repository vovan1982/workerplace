#ifndef DELEGATS_H
#define DELEGATS_H

#include <QDate>
#include <QDateEdit>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QItemDelegate>
#include <QLineEdit>
#include <QComboBox>
#include <QStyledItemDelegate>

class FileListModel;

class DateEditDelegate : public QItemDelegate {
    Q_OBJECT
public:
    DateEditDelegate(bool calpopup = true,
                 QObject *parent = 0);
    QWidget *createEditor(
            QWidget *parent,
            const QStyleOptionViewItem &option,
            const QModelIndex &index) const;
    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const;
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const;
    void updateEditorGeometry(
            QWidget *editor,
            const QStyleOptionViewItem &option,
            const QModelIndex &index) const;
private:
    bool m_calpopup;
};

//---------------------------------------------------------

class DoubleSpinBoxDelegat : public QItemDelegate {
    Q_OBJECT
public:
    DoubleSpinBoxDelegat(double min=0.00,
                  double max=999999999.99,
                  double step=0.1,
                  int precision=2,
                  QObject *parent = 0);
    QWidget *createEditor(
            QWidget *parent,
            const QStyleOptionViewItem &option,
            const QModelIndex &index) const;
    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const;
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const;
    void updateEditorGeometry(
            QWidget *editor,
            const QStyleOptionViewItem &option,
            const QModelIndex &index) const;
    void paint (
        QPainter *painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index ) const;
private:
    double m_min;
    double m_max;
    double m_step;
    int m_precision;
};

//---------------------------------------------------------

class SpinBoxDelegat : public QItemDelegate {
    Q_OBJECT
public:
    SpinBoxDelegat(int min=0,
                  int max=999999999,
                  int step=1,
                  QObject *parent = 0);
    QWidget *createEditor(
            QWidget *parent,
            const QStyleOptionViewItem &option,
            const QModelIndex &index) const;
    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const;
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const;
    void updateEditorGeometry(
            QWidget *editor,
            const QStyleOptionViewItem &option,
            const QModelIndex &index) const;
    void paint (
        QPainter *painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index ) const;
private:
    int m_min;
    int m_max;
    int m_step;
};

//-----------------------------------------------------------

class TypeDevIconDelegat : public QItemDelegate {
    Q_OBJECT
public:
    TypeDevIconDelegat(QObject *parent = 0);
    QWidget *createEditor(
            QWidget * ,
            const QStyleOptionViewItem &option,
            const QModelIndex &index) const;
    void setEditorData(QWidget *,
                       const QModelIndex &) const;
    void setModelData(QWidget *,
                      QAbstractItemModel *,
                      const QModelIndex &) const;
    void updateEditorGeometry(
            QWidget *editor,
            const QStyleOptionViewItem &option,
            const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const;
    void paint (
        QPainter *painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index ) const;
//private:
//    int m_maxLen;
//    FileListModel *model;
};

//-----------------------------------------------------------

class TypeDevDelegat : public QItemDelegate {
    Q_OBJECT
public:
    TypeDevDelegat(QObject *parent = 0);
    QWidget *createEditor(
            QWidget *parent,
            const QStyleOptionViewItem &option,
            const QModelIndex &index) const;
    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const;
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const;
    void updateEditorGeometry(
            QWidget *editor,
            const QStyleOptionViewItem &option,
            const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const;
    void drawDisplay ( QPainter * painter,
                       const QStyleOptionViewItem & option,
                       const QRect & rect,
                       const QString & text ) const;
};

#endif // DELEGATS_H

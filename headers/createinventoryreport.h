#ifndef CREATEINVENTORYREPORT_H
#define CREATEINVENTORYREPORT_H

#include <QtPrintSupport>
#include <QtWidgets>

class CreateInventoryReport : public QObject
{
    Q_OBJECT
public:
    explicit CreateInventoryReport(QObject *parent = 0);
    ~CreateInventoryReport();
    void createTitle(const QString &firmName);
    void createDepName(const QString &firmName);
    void createTable();
    void addRec(const QString &wpName, const QString &otName, const QString &typeName,
                const QString &iN, const QString &sN);
    void addSubRec(const QString &wpName, const QString &otName, const QString &typeName,
                   const QString &iN, const QString &sN);
    void write(const QString &fileName);
    void print(QPrinter *printer);
    void clear();
    CreateInventoryReport* clone(QObject *parent = 0);
    void setDocument(QTextDocument *doc);
private:
    QTextBlockFormat centerFormat;
    QTextCharFormat boldFormat;
    QTextDocument *m_document;
    QTextCursor m_cursor;
};

#endif // CREATEINVENTORYREPORT_H

#include "headers/createinventoryreport.h"

CreateInventoryReport::CreateInventoryReport(QObject *parent)
    : QObject(parent),
    m_document(new QTextDocument()),
    m_cursor(m_document)
{
}

CreateInventoryReport::~CreateInventoryReport()
{
    delete m_document;
}
void CreateInventoryReport::createTitle(const QString &firmName)
{
    QTextCharFormat defaultFormat;
    defaultFormat.setFontPointSize(10);
    boldFormat = defaultFormat;
    boldFormat.setFontWeight(QFont::Bold);
    QTextCharFormat titleFormat = boldFormat;
    titleFormat.setFontPointSize(18);
    centerFormat.setAlignment(Qt::AlignHCenter);
    m_cursor.movePosition(QTextCursor::NextBlock);
    m_cursor.setBlockFormat(centerFormat);
    m_cursor.insertText(QObject::tr("%1\n").arg(firmName),titleFormat);
    m_cursor.movePosition(QTextCursor::NextBlock);
}
void CreateInventoryReport::createDepName(const QString &dep)
{
    QTextCharFormat titleFormat = boldFormat;
    titleFormat.setFontPointSize(12);
    titleFormat.setFontItalic(true);
    QTextBlockFormat blockFormat;
    blockFormat.setBackground(Qt::lightGray);
    blockFormat.setAlignment(Qt::AlignLeft);
    m_cursor.movePosition(QTextCursor::NextBlock);
    m_cursor.setBlockFormat(blockFormat);
    m_cursor.insertText(QObject::tr("Подразделение: %1").arg(dep),titleFormat);
}
void CreateInventoryReport::createTable()
{
    QTextTableFormat tableFormat;
    tableFormat.setCellPadding(1);
    tableFormat.setCellSpacing(1);
    tableFormat.setHeaderRowCount(1);
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 100));
    tableFormat.setBorderBrush(Qt::black);
    QTextBlockFormat blockFormat;
    blockFormat.setBackground(Qt::gray);
    blockFormat.setAlignment(Qt::AlignHCenter);
    m_cursor.movePosition(QTextCursor::NextBlock);
    m_cursor.insertTable(1, 6, tableFormat);
    m_cursor.setBlockFormat(blockFormat);
    m_cursor.insertText(QObject::tr("Рабочее место/Склад"),boldFormat);
    m_cursor.movePosition(QTextCursor::NextCell);
    m_cursor.setBlockFormat(blockFormat);
    m_cursor.insertText(QObject::tr("Тип устройства"),boldFormat);
    m_cursor.movePosition(QTextCursor::NextCell);
    m_cursor.setBlockFormat(blockFormat);
    m_cursor.insertText(QObject::tr("Наименование"),boldFormat);
    m_cursor.movePosition(QTextCursor::NextCell);
    m_cursor.setBlockFormat(blockFormat);
    m_cursor.insertText(QObject::tr("Инвентаризационный №"),boldFormat);
    m_cursor.movePosition(QTextCursor::NextCell);
    m_cursor.setBlockFormat(blockFormat);
    m_cursor.insertText(QObject::tr("Серийный №"),boldFormat);
    m_cursor.movePosition(QTextCursor::NextCell);
    m_cursor.setBlockFormat(blockFormat);
    m_cursor.insertText(QObject::tr("Соответствие"),boldFormat);
}

void CreateInventoryReport::addRec(const QString &wpName, const QString &typeName, const QString &otName,
                                   const QString &iN, const QString &sN)
{
    QTextTable *table = m_cursor.currentTable();
    if (! table)
        return;
    table->appendRows(1);

    m_cursor.movePosition(QTextCursor::PreviousRow);
    m_cursor.movePosition(QTextCursor::NextCell);
    m_cursor.insertText(QString("%1").arg(wpName));
    m_cursor.movePosition(QTextCursor::NextCell);
    m_cursor.insertText(QString("%1").arg(typeName));
    m_cursor.movePosition(QTextCursor::NextCell);
    m_cursor.insertText(QString("%1").arg(otName));
    m_cursor.movePosition(QTextCursor::NextCell);
    m_cursor.insertText(QString("%1").arg(iN));
    m_cursor.movePosition(QTextCursor::NextCell);
    m_cursor.insertText(QString("%1").arg(sN));
    m_cursor.movePosition(QTextCursor::NextCell);
    m_cursor.insertText("");
}
void CreateInventoryReport::addSubRec(const QString &wpName, const QString &typeName, const QString &otName,
                                      const QString &iN, const QString &sN)
{
    QTextTable *table = m_cursor.currentTable();
    if (! table)
        return;
    QTextBlockFormat blockFormat;
    blockFormat.setBackground(Qt::green);
    table->appendRows(1);

    m_cursor.movePosition(QTextCursor::PreviousRow);
    m_cursor.movePosition(QTextCursor::NextCell);
    m_cursor.setBlockFormat(blockFormat);
    m_cursor.insertText(QString("%1").arg(wpName));
    m_cursor.movePosition(QTextCursor::NextCell);
    m_cursor.setBlockFormat(blockFormat);
    m_cursor.insertText(QString("%1").arg(typeName));
    m_cursor.movePosition(QTextCursor::NextCell);
    m_cursor.setBlockFormat(blockFormat);
    m_cursor.insertText(QString("%1").arg(otName));
    m_cursor.movePosition(QTextCursor::NextCell);
    m_cursor.setBlockFormat(blockFormat);
    m_cursor.insertText(QString("%1").arg(iN));
    m_cursor.movePosition(QTextCursor::NextCell);
    m_cursor.setBlockFormat(blockFormat);
    m_cursor.insertText(QString("%1").arg(sN));
    m_cursor.movePosition(QTextCursor::NextCell);
    m_cursor.setBlockFormat(blockFormat);
    m_cursor.insertText("");
}
void CreateInventoryReport::write(const QString &fileName)
{
    QTextDocumentWriter writer(fileName);
    writer.write(m_document);
}
void CreateInventoryReport::print(QPrinter *printer)
{
    m_document->print(printer);
}
void CreateInventoryReport::clear()
{
    m_document->clear();
}
void CreateInventoryReport::setDocument(QTextDocument *doc)
{
    m_document = doc->clone();
}
CreateInventoryReport* CreateInventoryReport::clone(QObject * parent) {
  CreateInventoryReport* other = new CreateInventoryReport(parent);
  other->setDocument(m_document);
  return other;
}

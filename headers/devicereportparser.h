#ifndef DEVICEREPORTPARSER_H
#define DEVICEREPORTPARSER_H

#include <QXmlDefaultHandler>

class DeviceReportParser : public QXmlDefaultHandler
{
public:
    DeviceReportParser(bool wpMode = false, bool typeDevMode = false, bool typeLicMode = false, bool producerMode = false);
    DeviceReportParser(const QList<QString> attr = QList<QString>(),
                       const QMap<QString,QVariant> comparisonWP = QMap<QString,QVariant>(),
                       const QMap<QString,QVariant> comparisonTypeDev = QMap<QString,QVariant>(),
                       const QMap<QString,QVariant> comparisonTypeLic = QMap<QString,QVariant>(),
                       const QMap<QString,QVariant> comparisonProducer = QMap<QString,QVariant>());
    ~DeviceReportParser();
    bool startElement(const QString&,
                          const QString& ,
                          const QString& teg,
                          const QXmlAttributes& attrs
                         );
    bool characters(const QString& );
    bool endElement(const QString&, const QString&, const QString& /*str*/);
    bool fatalError(const QXmlParseException& exception);
    QString errorString();
    QList<QString> attrValues();
private:
    bool m_wpMode, m_typeDevMode, m_typeLicMode, m_producerMode, importMode;
    QList<QString> attrValuesList;
//    QString m_strText;
    QMap<QString,QVariant> m_comparisonWP;
    QMap<QString,QVariant> m_comparisonTypeDev;
    QMap<QString,QVariant> m_comparisonTypeLic;
    QMap<QString,QVariant> m_comparisonProducer;
    int curWp, curDev;
    QString errorStr;
};

#endif // DEVICEREPORTPARSER_H

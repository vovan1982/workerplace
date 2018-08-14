#ifndef USERREPORTPARSER_H
#define USERREPORTPARSER_H

#include <QXmlDefaultHandler>

class UserReportParser : public QXmlDefaultHandler
{
public:
    UserReportParser(bool organizationMode = false);
    UserReportParser(const QList<QString> attr = QList<QString>(), const QMap<QString,QVariant> comparisonOrganization = QMap<QString,QVariant>());
    ~UserReportParser();
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
    bool m_organizationMode, importMode;
    QList<QString> attrValuesList;
    QMap<QString,QVariant> m_comparisonOrganization;
    int curOrg, curUser;
    QString errorStr;
};

#endif // USERREPORTPARSER_H

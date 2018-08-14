#ifndef REPORTFILEITEM_H
#define REPORTFILEITEM_H

#include <QString>

class ReportFileItem
{
public:
    ReportFileItem();
    ReportFileItem(const QString &name, const QString &description, const QString &path);
    ~ReportFileItem();
    QString getName();
    QString getDescription();
    QString getPath();
    void setName(QString &newName);
    void setDescription(QString &newDescription);
    void setPath(QString &newPath);
private:
    QString m_name;
    QString m_description;
    QString m_path;
};

#endif // REPORTFILEITEM_H

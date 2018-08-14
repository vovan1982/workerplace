#include "headers/reportfileitem.h"

ReportFileItem::ReportFileItem()
{
    m_name = "";
    m_description = "";
    m_path = "";
}
ReportFileItem::ReportFileItem(const QString &name, const QString &description, const QString &path) : m_name(name), m_description(description), m_path(path)
{

}
ReportFileItem::~ReportFileItem()
{

}
QString ReportFileItem::getName()
{
    return m_name;
}
QString ReportFileItem::getDescription()
{
    return m_description;
}
QString ReportFileItem::getPath()
{
    return m_path;
}
void ReportFileItem::setName(QString &newName)
{
    m_name = newName;
}
void ReportFileItem::setDescription(QString &newDescription)
{
    m_description = newDescription;
}
void ReportFileItem::setPath(QString &newPath)
{
    m_path = newPath;
}

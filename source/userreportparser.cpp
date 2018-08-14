#include <QtSql>
#include "headers/userreportparser.h"

UserReportParser::UserReportParser(bool organizationMode) :
    m_organizationMode(organizationMode)
{
    attrValuesList =  QList<QString>();
    importMode = false;
    errorStr = "";
}

UserReportParser::UserReportParser(const QList<QString> attr, const QMap<QString,QVariant> comparisonOrganization)
{
     m_organizationMode = false;
     importMode = true;

     attrValuesList = attr; // 0 - тип логинов
     m_comparisonOrganization = comparisonOrganization;
     curOrg = 0;
     curUser = 0;
     errorStr = "";
}

UserReportParser::~UserReportParser()
{
}

bool UserReportParser::startElement(const QString&,
                                      const QString&,
                                      const QString& teg,
                                      const QXmlAttributes& attrs
                                      )
{
    if(!importMode){
        for(int i = 0; i < attrs.count(); i++) {
            if(m_organizationMode){
                if(teg == "organization"){
                    if(attrs.localName(i) == "name")
                        attrValuesList<<attrs.value(i);
                }
            }
        }
    }else{
        bool ok;
        QSqlQuery query;
        QString queryStr,field,val;
        //---------------------------------
        if(teg == "organization"){
            if(m_comparisonOrganization.isEmpty()) {errorStr = "Comparison Organization is empty!!!"; return false;};
            curOrg = m_comparisonOrganization.value(attrs.value("name")).toInt();
        }
        //---------------------------------
        if(teg == "user"){
            if(attrs.value("displayname").isNull() && attrs.value("displayname").isEmpty()) return true;
            field = "(FIOSummary, CodOrganization, CodPost";
            val = "(\""+attrs.value("displayname")+"\","+QString("%1").arg(curOrg);
            if(!attrs.value("post").isNull() && !attrs.value("post").isEmpty()){
                ok = query.exec("SELECT CodPost FROM post WHERE Name =\""+attrs.value("post")+"\"");
                if(!ok) {errorStr = query.lastError().text(); return false;}
                if(query.size() > 0){
                    query.next();
                    val += ",\""+query.value(0).toString()+"\"";
                }else{
                    ok = query.exec("INSERT INTO post (Name) VALUES (\""+attrs.value("post")+"\")");
                    if(!ok) {errorStr = query.lastError().text(); return false;}
                    val += ",'"+query.lastInsertId().toString()+"'";
                }
            }else{
                ok = query.exec("SELECT CodPost FROM post WHERE Name ='import'");
                if(!ok) {errorStr = query.lastError().text(); return false;}
                if(query.size() > 0){
                    query.next();
                    val += ",'"+query.value(0).toString()+"'";
                }else{
                    ok = query.exec("INSERT INTO post (Name) VALUES ('import')");
                    if(!ok) {errorStr = query.lastError().text(); return false;}
                    val += ",'"+query.lastInsertId().toString()+"'";
                }
            }
            if(!attrs.value("lastname").isNull() && !attrs.value("lastname").isEmpty()){
                field += ",LastName";
                val += ",\""+attrs.value("lastname")+"\"";
            }
            if(!attrs.value("firstname").isNull() && !attrs.value("firstname").isEmpty()){
                field += ",Name";
                val += ",\""+attrs.value("firstname")+"\"";
            }
            if(!attrs.value("middlename").isNull() && !attrs.value("middlename").isEmpty()){
                field += ",MiddleName";
                val += ",\""+attrs.value("middlename")+"\"";
            }
            if(!attrs.value("email").isNull() && !attrs.value("email").isEmpty()){
                field += ",email";
                val += ",\""+attrs.value("email")+"\"";
            }
            if(!attrs.value("additionalemail").isNull() && !attrs.value("additionalemail").isEmpty()){
                field += ",additionalemail";
                val += ",\""+attrs.value("additionalemail")+"\"";
            }
            field += ")";
            val += ")";
            queryStr = "INSERT INTO users "+field+" VALUES "+val;
            ok = query.exec(queryStr);
            if(!ok) {errorStr = query.lastError().text(); return false;}
            curUser = query.lastInsertId().toInt();

            if(!attrs.value("login").isNull() && !attrs.value("login").isEmpty()){
                val = "("+QString("%1").arg(curUser)+","+attrValuesList.value(0)+",\""+attrs.value("login")+"\")";
                queryStr = "INSERT INTO loginpass (CodLoginPass,CodLoginType,Login) VALUES "+val;
                ok = query.exec(queryStr);
                if(!ok) {errorStr = query.lastError().text(); return false;}
            }
        }
    }
    return true;
}

bool UserReportParser::characters(const QString& /*strText*/)
{
    return true;
}

bool UserReportParser::endElement(const QString&, const QString&, const QString& /*str*/)
{
    return true;
}

bool UserReportParser::fatalError(const QXmlParseException& exception)
{
    errorStr = QString("Parser XML File Error:\n Line:%1\n Column:%2\n Message:%3")
            .arg(exception.lineNumber()).arg(exception.columnNumber()).arg(exception.message());
    return false;
}

QString UserReportParser::errorString()
{
    return errorStr;
}

QList<QString> UserReportParser::attrValues()
{
    return attrValuesList;
}

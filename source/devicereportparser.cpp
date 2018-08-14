#include <QtSql>
#include "headers/devicereportparser.h"

DeviceReportParser::DeviceReportParser(bool wpMode, bool typeDevMode, bool typeLicMode, bool producerMode) :
    m_wpMode(wpMode),
    m_typeDevMode(typeDevMode),
    m_typeLicMode(typeLicMode),
    m_producerMode(producerMode)
{
    attrValuesList =  QList<QString>();
    importMode = false;
    errorStr = "";
}

DeviceReportParser::DeviceReportParser(const QList<QString> attr, const QMap<QString,QVariant> comparisonWP,
                   const QMap<QString,QVariant> comparisonTypeDev, const QMap<QString,QVariant> comparisonTypeLic,
                   const QMap<QString,QVariant> comparisonProducer)
{
     m_wpMode = false;
     m_typeDevMode = false;
     m_typeLicMode = false;
     m_producerMode = false;
     importMode = true;

     attrValuesList = attr; // 0 - id фирмы, 1 - код состояния устройств, 2 - код состояния лицензий
     m_comparisonWP = comparisonWP;
     m_comparisonTypeDev = comparisonTypeDev;
     m_comparisonTypeLic = comparisonTypeLic;
     m_comparisonProducer = comparisonProducer;
     curWp = 0;
     curDev = 0;
     errorStr = "";
}

DeviceReportParser::~DeviceReportParser()
{
}

bool DeviceReportParser::startElement(const QString&,
                                      const QString&,
                                      const QString& teg,
                                      const QXmlAttributes& attrs
                                      )
{
    if(!importMode){
        for(int i = 0; i < attrs.count(); i++) {
            if(m_wpMode){
                if(teg == "workplase"){
                    if(attrs.localName(i) == "name")
                        attrValuesList<<attrs.value(i);
                }
            }else if(m_typeDevMode){
                if(teg == "device" || teg == "subdevice"){
                    if(attrs.localName(i) == "type"){
                        if(!attrValuesList.contains(attrs.value(i)))
                            attrValuesList<<attrs.value(i);
                    }
                }
            }else if(m_typeLicMode){
                if(teg == "license"){
                    if(attrs.localName(i) == "name"){
                        if(!attrValuesList.contains(attrs.value(i)))
                            attrValuesList<<attrs.value(i);
                    }
                }
            }else if(m_producerMode){
                if(teg == "device" || teg == "subdevice"){
                    if(attrs.localName(i) == "producer"){
                        if(!attrValuesList.contains(attrs.value(i)))
                            attrValuesList<<attrs.value(i);
                    }
                }
            }
        }
    }else{
        bool ok;
        QSqlQuery query;
        QString queryStr,field,val;
        if(teg == "workplase")
            curWp = m_comparisonWP.value(attrs.value("name")).toInt();
        if(teg == "device"){
            field = "(parent_id, CodOrganization, CodWorkerPlace, CodTypeDevice, Name, CodState, Type";
            val = "(0,"+attrValuesList.value(0)+","+QString("%1").arg(curWp)+","+m_comparisonTypeDev.value(attrs.value("type")).toString();
            val += ",\""+attrs.value("name")+"\","+attrValuesList.value(1)+",1";
            if(!m_comparisonProducer.isEmpty()){
                if(!attrs.value("producer").isNull() && !attrs.value("producer").isEmpty()){
                    field += ",CodProducer";
                    val += ","+m_comparisonProducer.value(attrs.value("producer")).toString();
                }
            }
            if(!attrs.value("mac").isNull() && !attrs.value("mac").isEmpty()){
                field += ",Note";
                val += ",'MAC: "+attrs.value("mac")+"'";
            }
            if(!attrs.value("serialN").isNull() && !attrs.value("serialN").isEmpty()){
                field += ",SerialN";
                val += ",'"+attrs.value("serialN")+"'";
            }
            if(!attrs.value("invN").isNull() && !attrs.value("invN").isEmpty()){
                field += ",InventoryN";
                val += ",'"+attrs.value("invN")+"'";
            }
            field += ")";
            val += ")";
            queryStr = "INSERT INTO device "+field+" VALUES "+val;
            ok = query.exec(queryStr);
            if(!ok) {errorStr = query.lastError().text(); return false;}
            curDev = query.lastInsertId().toInt();
        }
        if(teg == "subdevice"){
            field = "(parent_id, CodOrganization, CodWorkerPlace, CodTypeDevice, Name, CodState, Type";
            val = "("+QString("%1").arg(curDev)+","+attrValuesList.value(0)+","+QString("%1").arg(curWp)+","+m_comparisonTypeDev.value(attrs.value("type")).toString();
            val += ",\""+attrs.value("name")+"\","+attrValuesList.value(1)+",0";
            if(!m_comparisonProducer.isEmpty()){
                if(!attrs.value("producer").isNull() && !attrs.value("producer").isEmpty()){
                    field += ",CodProducer";
                    val += ","+m_comparisonProducer.value(attrs.value("producer")).toString();
                }
            }
            if(!attrs.value("mac").isNull() && !attrs.value("mac").isEmpty()){
                field += ",Note";
                val += ",'MAC: "+attrs.value("mac")+"'";
            }
            if(!attrs.value("serialN").isNull() && !attrs.value("serialN").isEmpty()){
                field += ",SerialN";
                val += ",'"+attrs.value("serialN")+"'";
            }
            if(!attrs.value("invN").isNull() && !attrs.value("invN").isEmpty()){
                field += ",InventoryN";
                val += ",'"+attrs.value("invN")+"'";
            }
            field += ")";
            val += ")";
            queryStr = "INSERT INTO device "+field+" VALUES "+val;
            ok = query.exec(queryStr);
            if(!ok) {errorStr = query.lastError().text(); return false;}
        }
        if(teg == "license"){
            if(m_comparisonTypeLic.isEmpty()) {errorStr = "Comparison Type License is empty!!!"; return false;};
            int codPo;
            ok = query.exec(QString("SELECT CodPO FROM po WHERE Name = '%1'").arg(attrs.value("name")));
            if(!ok) {errorStr = query.lastError().text(); return false;}
            if(query.size() <= 0){
                int codGroup = 0;
                ok = query.exec(QString("SELECT CodPO FROM po WHERE Name = 'import' AND isGroup = 1"));
                if(!ok) {errorStr = query.lastError().text(); return false;}
                if(query.size() <= 0){
                    query.exec("INSERT INTO po (Name, isGroup) VALUES ('import',1)");
                    if(!ok) {errorStr = query.lastError().text(); return false;}
                    codGroup = query.lastInsertId().toInt();
                    query.exec(QString("INSERT INTO po (CodGroupPO, Name) VALUES (%1,'%2')").arg(codGroup).arg(attrs.value("name")));
                    if(!ok) {errorStr = query.lastError().text(); return false;}
                    codPo = query.lastInsertId().toInt();
                }else{
                    query.next();
                    codGroup = query.value(0).toInt();
                    query.exec(QString("INSERT INTO po (CodGroupPO, Name) VALUES (%1,'%2')").arg(codGroup).arg(attrs.value("name")));
                    if(!ok) {errorStr = query.lastError().text(); return false;}
                    codPo = query.lastInsertId().toInt();
                }
            }else{
                query.next();
                codPo = query.value(0).toInt();
            }
            field = "(CodDevice,CodWorkerPlace,CodPO,CodTypeLicense,CodStatePO)";
            val = "("+QString("%1").arg(curDev)+","+QString("%1").arg(curWp)+","+QString("%1").arg(codPo);
            val += ","+m_comparisonTypeLic.value(attrs.value("name")).toString()+","+attrValuesList.value(2);
            val += ")";
            queryStr = "INSERT INTO licenses "+field+" VALUES "+val;
            ok = query.exec(queryStr);
            if(!ok) {errorStr = query.lastError().text(); return false;}
        }
    }
    return true;
}

bool DeviceReportParser::characters(const QString& /*strText*/)
{
    return true;
}

bool DeviceReportParser::endElement(const QString&, const QString&, const QString& /*str*/)
{
    return true;
}

bool DeviceReportParser::fatalError(const QXmlParseException& exception)
{
    errorStr = QString("Parser XML File Error:\n Line:%1\n Column:%2\n Message:%3")
            .arg(exception.lineNumber()).arg(exception.columnNumber()).arg(exception.message());
    return false;
}

QString DeviceReportParser::errorString()
{
    return errorStr;
}

QList<QString> DeviceReportParser::attrValues()
{
    return attrValuesList;
}

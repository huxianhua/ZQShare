#include "sql_record.h"

#include <QVariant>
#include <QStringList>

#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlRecord>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QProcess>
#include <QDir>

#include "config.h"
#include "sql_work.h"




SQL_RECORD::SQL_RECORD(QObject *parent) : QObject(parent)
{
    LOG_DEBUG();

    SQL_WORK *worker = new SQL_WORK;
    worker->readConfig(SQLWORKCOFIG);

    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &SQL_RECORD::operate, worker, &SQL_WORK::doWork);
    connect(worker, &SQL_WORK::resultReady, this, &SQL_RECORD::handleResults);
    workerThread.start();

    LOG_DEBUG();

    emit this->operate("");

    m_json = get_record_400();

}
void SQL_RECORD::handleResults(const QByteArray &results)
{
    LOG_DEBUG("result: (%s)",results.data());
    m_newJsonResult = results;
    if(isValid())
    {
        m_json = m_newJsonResult;
    }
}


QByteArray SQL_RECORD::get_record_400()
{
    QJsonDocument document;
    // 构建 Json 对象
    QJsonObject json;

    QJsonArray pa;

    json.insert("rec", "400");
    json.insert("worklist", QJsonValue(pa));
    json.insert("producer","PC-LinkHIS");
    json.insert("datetime",getDateTimeStamp());

    document.setObject(json);

    QByteArray jsonbuff = document.toJson(QJsonDocument::Indented);

    return jsonbuff;
}

QByteArray SQL_RECORD::get_record_json()
{
    return m_json;
}
QByteArray SQL_RECORD::get_record_demo()
{
    QJsonObject po_1;
    po_1.insert("PatientID", "12012242231");
    po_1.insert("Name",  "胡途");
    po_1.insert("Gender", "F");
    po_1.insert("Birth",  "1980-10-02");

    QJsonObject po_2;
    po_2.insert("PatientID", "12012242232");
    po_2.insert("Name",  "袁大");
    po_2.insert("Gender", "F");
    po_2.insert("Birth",  "1982-10-02");


    QJsonArray pa;
    pa.append(po_1);
    pa.append(po_2);

    // 构建 Json 对象
    QJsonObject json;

    json.insert("rec", "200");
    json.insert("worklist", QJsonValue(pa));
    json.insert("producer","PC-LinkHIS");

    // 构建 Json 文档
    QJsonDocument document;
    document.setObject(json);

    //QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QByteArray byteArray = document.toJson(QJsonDocument::Indented);

    return byteArray;
}

QString SQL_RECORD::getDateTimeStamp()
{
    QDateTime dt = QDateTime::currentDateTime();
    QString time_token = dt.toString("yy-MM-dd hh:mm:ss-zz");
    return time_token;
}



bool SQL_RECORD::isValid()
{
    if(m_newJsonResult.length() < 20)
    {
        LOG_DEBUG("m_newJsonResult: (%s)",m_newJsonResult.data());
        return false;
    }

    QJsonParseError json_error;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(m_newJsonResult, &json_error));

    if(json_error.error != QJsonParseError::NoError)
    {
        LOG_DEBUG("QJsonParseError: %d",(int)json_error.error);
        LOG_DEBUG("m_newJsonResult: (%s)",m_newJsonResult.data());
        return false;
    }

    QJsonObject rootObj = jsonDoc.object();

    int rec = rootObj.value("rec").toInt(400);

    if(rec == 200)
    {
        return true;
    }else
    {
        LOG_DEBUG("m_newJsonResult: (%s)",m_newJsonResult.data());
        return false;
    }
}


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

#include <unistd.h>

#include "config.h"



SQL_RECORD::SQL_RECORD(QObject *parent) : QObject(parent)
{

#if 0
    m_workThread = new QThread;

    /*
    this->moveToThread(m_workThread);
    m_workThread->start();
    connect(m_workThread,SIGNAL(started()),this,SLOT(s_select()));
    //connect(m_workThread,SIGNAL(finished()),m_workThread,SLOT(deleteLater()));
*/

    LOG_DEBUG();
#endif

    m_process = new QProcess();
    connect(m_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finished(int,QProcess::ExitStatus)));



    m_waitSec = 10;

    m_programName = "./WorkList_Cmd";
    m_programConfig = "./mysql.ini";
}
void SQL_RECORD::setOutDir(QString outDir)
{
    m_outDir = outDir;
}
void SQL_RECORD::setWaitSec(int waitSec)
{
    if(waitSec < 10)
    {
        waitSec = 10;
    }
    m_waitSec = waitSec;
}

void SQL_RECORD::setProgram(QString programName,QString programConfig)
{
    m_programName = programName;
    m_programConfig = programConfig;
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
    if(!isValid())
    {
        return get_record_400();
    }else
    {
        return m_newJsonResult;
    }
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
void SQL_RECORD::s_select()
{

    QStringList arguments;

    while(true)
    {
        m_saveFile = QString("%1/%2.json").arg(m_outDir).arg(STRDATETIMEMS);

        arguments.clear();

        arguments << m_programConfig << m_saveFile;

        m_process->start(m_programName,arguments);
        LOG_DEBUG("执行的程序: %s %s %s",qUtf8Printable(m_programName),qUtf8Printable(m_programConfig),qUtf8Printable(m_saveFile));
        //等待睡眠
        sleep(m_waitSec);
    }
}
void SQL_RECORD::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    LOG_DEBUG("exitCode:%d exitStatus:%d",exitCode,exitStatus);

    QFile outFile(m_saveFile);

    if(!outFile.exists())
    {
        LOG_DEBUG("程序执行完成！但输出的文件不存在");
        return;
    }

    if(!outFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        LOG_DEBUG("程序执行完成！但输出的文件 不能读取");
        return;
    }

    m_newJsonResult = outFile.readAll();
    outFile.close();

    //删除文件
    //QFile(m_saveFile).remove();

    LOG_DEBUG("读取到的 查询结果: (%s)",m_newJsonResult.data());

    emit sig_result();
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

SelectThread::SelectThread(QObject* parent):QThread(parent)
{

}


void SelectThread::run()
{

}


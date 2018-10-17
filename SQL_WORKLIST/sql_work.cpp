#include "sql_work.h"
#include <QFile>
#include "config.h"


#include <QVariant>
#include <QStringList>

#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlRecord>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QSqlError>

#include <unistd.h>

SQL_WORK::SQL_WORK(QObject *parent) : QObject (parent)
{

}
void SQL_WORK::readConfig(QString configFile)
{
    QFile config(configFile);

    LOG_DEBUG("读取配置");
    if(!config.exists())
    {
        LOG_FATAL("配置文件不存在  (%s)", qUtf8Printable(configFile));
    }
        LOG_DEBUG("读取配置 -- %s",qUtf8Printable(configFile));

    QSettings settings(configFile,QSettings::IniFormat);

    settings.beginGroup("SYSTINFO");
    m_driverName = settings.value("driverName").toString().trimmed();
    settings.endGroup();

    QStringList driverList;

    driverList << "QOCI" << "QMYSQL" << "QODBC";

    int listCount = driverList.length();

    bool isFind = false;

    for(int index = 0; index < listCount; index++)
    {
        if(0 == driverList.at(index).compare(m_driverName,Qt::CaseInsensitive))
        {
            isFind = true;
            break;
        }
    }

    if(!isFind)
    {
           LOG_FATAL("请检查 %s 类型是否支持",qUtf8Printable(m_driverName));
    }

    settings.beginGroup("Database");

    m_hostname = settings.value("hostname","192.168.0.163").toString();
    m_port = settings.value("port","1521").toInt();
    m_sid = settings.value("sid","xe").toString();
    m_user = settings.value("user","system").toString();
    m_password = settings.value("password","oracle").toString();
    m_sql = settings.value("sql").toString();
    settings.endGroup();

    printCofnig();

}
void SQL_WORK::setConfig(QString driverName,QString hostname,int port,QString sid,QString user,QString password,QString sql)
{
    m_driverName = driverName;
    m_hostname = hostname;
    m_port = port;
    m_sid = sid;
    m_user = user;
    m_password = password;
    m_sql = sql;

    bool isFind = false;
    QStringList driverList;
    driverList << "QOCI" << "QMYSQL" << "QODBC";

    int listCount = driverList.length();
    for(int index = 0; index < listCount; index++)
    {
        if(0 == driverList.at(index).compare(m_driverName,Qt::CaseInsensitive))
        {
            isFind = true;
            break;
        }
    }
    if(!isFind)
    {
           LOG_FATAL("请检查 %s 类型是否支持",qUtf8Printable(m_driverName));
    }

    printCofnig();
}
void SQL_WORK::printCofnig()
{
    QString log;

    log = QString("driverName:(%1) hostname: %1 port:%2 sid:%3 user:%4 password:%5 sql:(%6)")
            .arg(m_driverName)
            .arg(m_hostname)
            .arg(m_port)
            .arg(m_sid)
            .arg(m_user)
            .arg(m_password)
            .arg(m_sql);

    LOG_DEBUG("log:%s",qUtf8Printable(log));
}
QByteArray SQL_WORK::selectSQL()
{
    QByteArray json = "";

    printCofnig();

    m_db = QSqlDatabase::addDatabase(m_driverName,m_driverName);
    m_db.setHostName(m_hostname);
    m_db.setPort(m_port);
    m_db.setDatabaseName(m_sid);
    m_db.setUserName(m_user);
    m_db.setPassword(m_password);

    bool ok = m_db.open();

    if(ok)
    {
        LOG_DEBUG("%s is open",qUtf8Printable(m_driverName));
        if(0 == m_driverName.compare("QMYSQL",Qt::CaseInsensitive))
        {
            LOG_DEBUG("user %s",qUtf8Printable(m_sid));

            QSqlQuery query(this->m_db);
            QString exe_cmd = QString("use %1;").arg(m_sid);
            if(!query.exec(exe_cmd) )
            {
                LOG_FATAL("警告-mysql  执行 use %s; 异常!",m_sid.toUtf8().data());
            }
        }
        json = get_select_json(m_sql);
        m_json = json;
    }else
    {
        LOG_DEBUG("%s is no open, (%s)",qUtf8Printable(m_driverName),qUtf8Printable(m_db.lastError().text()));
    }
    return json;
}
void SQL_WORK::removeDatabase()
{
    QString connection;

    connection = m_db.connectionName();
    m_db.close();
    m_db = QSqlDatabase();
    QSqlDatabase::removeDatabase(connection);
}


QByteArray SQL_WORK::get_select_json(QString sql)
{
    QByteArray json;

    if(0 == m_driverName.compare("QOCI",Qt::CaseInsensitive))
    {
        json = get_record_oracle_180416(sql);

    }else
    if(0 == m_driverName.compare("QMYSQL",Qt::CaseInsensitive))
    {
        json = get_record_mysql(sql);
    }
    else
    {
        LOG_FATAL("请检查 %s 类型是否支持",qUtf8Printable(m_driverName));
    }
    return json;
}

QByteArray SQL_WORK::result()
{
    return m_json;
}




int SQL_WORK::Age_to_Day(QString value)
{
    int multiple = 0;

    if(value.endsWith("Y"))
    {
        multiple = 365;
    }else
    if(value.endsWith("M"))
    {
        multiple = 30;
    }else
    if(value.endsWith("D"))
    {
        multiple = 1;
    }
    int age_number;
    sscanf(value.toLatin1().data(),"%d*",&age_number);

    return multiple * age_number;
}
//"Age":"20", 年龄
//"AgeUnit":"Y", 年龄单位
// n 岁  n 月  n日
void SQL_WORK::HIS_Result_Age(QString value,QString &age,QString &age_unit)
{
    if(value.endsWith("岁"))
    {
        age_unit = "Y";
    }else
    if(value.endsWith("月"))
    {
        age_unit = "M";
    }else
    if(value.endsWith("日"))
    {
        age_unit = "D";
    }
    int age_number;
    sscanf(value.toLatin1().data(),"%d*",&age_number);

   age = QString::number(age_number);
}
void SQL_WORK::parse_age_string(QString age_str,QString &age_number,QString &age_unit)
{
    QString parse_str = age_str.simplified();

    parse_str.replace("岁","Y ");
    parse_str.replace("月","M ");
    parse_str.replace("天","D ");

    QStringList age_list = parse_str.split(" ",QString::SkipEmptyParts);

    int subsection = age_list.length();
    if(subsection == 0)
    {
        return;
    }

    int age_day = 0;

    QString print_string = "";
    for(int index = 0; index < subsection; index++)
    {
        QString out_age;
        QString out_unit;

        age_day = age_day + Age_to_Day(age_list.at(index));
        HIS_Result_Age(age_list.at(index),out_age,out_unit);

        print_string = QString("%1 %2").arg(out_age).arg(out_unit);

        qDebug("\n age_list:(%s) --- %s \n",age_list.at(index).toUtf8().data(),print_string.toUtf8().data());
    }
    //qDebug(" -------------- age_day:%d",age_day);
    int p_age_year  = age_day / 365;
    if(p_age_year >= 1)
    {
        qDebug(" ==== age_day:%d Y",age_day / 365);

        age_number = QString::number(age_day / 365);
        age_unit  = "Y";
    }
    else
    {
        int p_age_month = age_day / 30;
        if(p_age_month >= 1)
        {
            qDebug(" ==== age_day:%d M",age_day / 30);
            age_number = QString::number(age_day / 30) ;
            age_unit  = "M";
        }
        else
        {
            qDebug(" ==== age_day:%d D",age_day );
            age_number = QString::number(age_day) ;
            age_unit  = "D";
        }
    }

#if 0
    if(p_age_year >= 14)
    {
        qDebug(" ==== age_day:%d Y",age_day / 365);

        age_number = QString::number(age_day / 365);
        age_unit  = "Y";
    }else
    if(p_age_year >= 10)
    {
        qDebug(" ==== age_day:%d M",age_day / 30);
        age_number = QString::number(age_day / 30) ;
        age_unit  = "M";
    }
    else
    {
        qDebug(" ==== age_day:%d D",age_day );

        age_number = age_day;
        age_unit  = "D";
    }
#endif
}

QByteArray SQL_WORK::get_record_oracle(QString sql)
{
    QSqlQuery query(m_db);
    QString exe_cmd = sql;

    QJsonDocument document;
    // 构建 Json 对象
    QJsonObject json;

    query.exec(exe_cmd);

    if( query.isActive() )
    {
        //设置字段名
        QSqlRecord record = query.record();
        int field_count = record.count();

        QStringList header_lables;

        int index = 0;

        while(index < field_count)
        {
            header_lables.append(record.fieldName(index));
            index++;
        }

        //取得查询的条数
        int numRows = 0;
        if (m_db.driver()->hasFeature(QSqlDriver::QuerySize)) {
             numRows = query.size();
         } else {
             // this can be very slow
             if(query.last())
             {
                 numRows = query.at();
                 numRows = (numRows < 0) ? 0 : numRows + 1;
                 query.first();
             }
         }

        QJsonArray pa;

        // 设置表内容
        if(numRows > 0)
        {
            int row = 0;
            int column = 0;
            do{
                QJsonObject po;

                column = 0;
                while(column < field_count)
                {
                    QString str = query.value(column).toString();
                    QString head_str = header_lables.at(column);
                    if(0 == head_str.compare("AGE",Qt::CaseInsensitive))
                    {
                        QString age_number = "0";
                        QString age_unit = "Y";
                        parse_age_string(str,age_number,age_unit);
                        QStringList age;
                        age<<age_number<<age_unit;
                        for(int i = 0; i<2; i++)
                        {
                            qDebug(" ==== age:%s",age.at(i).toUtf8().data());
                            po.insert(header_lables.at(column),age.at(i));
                            column++;
                        }
                    }
                    else
                    {
                        if(0 == head_str.compare("PATIENTSEX",Qt::CaseInsensitive))
                        {
                            if(0 == str.compare("男"))
                            {
                                str = "M";
                            }else
                            if(0 == str.compare("女"))
                            {
                                str = "F";
                            }else
                            {
                                str = "O";
                            }
                        }

                        po.insert(header_lables.at(column),str);
                        column++;
                    }

                }
                pa.append(po);

                row++;
            }while (query.next());
        }
        json.insert("rec", "200");
        json.insert("worklist", QJsonValue(pa));
        json.insert("producer","PC-LinkHIS");
        json.insert("datetime",getDateTimeStamp());
    }else
    {
        QJsonArray pa;

        json.insert("rec", "400");
        json.insert("worklist", QJsonValue(pa));
        json.insert("producer","PC-LinkHIS");
        json.insert("datetime",getDateTimeStamp());
    }
    query.clear();

    document.setObject(json);
    //QByteArray byteArray = document.toJson(QJsonDocument::Compact);

    QByteArray jsonBuffer = document.toJson(QJsonDocument::Indented);
    return jsonBuffer;
}
/**
 * 解决: 年龄:  12岁 , 12岁2月, 2月10天,
 *      性别:  Gender_CN   男   女
 * @brief SQL_WORK::get_record_oracle_180416
 * @param db
 * @param sql
 * @return
 */
QByteArray SQL_WORK::get_record_oracle_180416(QString sql)
{
    QSqlQuery query(m_db);
    QString exe_cmd = sql;

    QJsonDocument document;
    // 构建 Json 对象
    QJsonObject json;

    query.exec(exe_cmd);

    if( query.isActive() )
    {
        //设置字段名
        QSqlRecord record = query.record();
        int field_count = record.count();

        QStringList header_lables;

        int index = 0;

        while(index < field_count)
        {
            header_lables.append(record.fieldName(index));
            index++;
        }

        header_lables.append("AGEUNIT");

        //取得查询的条数
        int numRows = 0;
        if (m_db.driver()->hasFeature(QSqlDriver::QuerySize)) {
             numRows = query.size();
         } else {
             // this can be very slow
             if(query.last())
             {
                 numRows = query.at();
                 numRows = (numRows < 0) ? 0 : numRows + 1;
                 query.first();
             }
         }

        QJsonArray pa;

        // 设置表内容
        if(numRows > 0)
        {
            int row = 0;
            int column = 0;
            do{
                //ui->tedit_log->insertPlainText("\n");
                QString age_number = "0";
                QString age_unit = "Y";

                QJsonObject po;

                column = 0;
                while(column < field_count)
                {
                    QString str = query.value(column).toString();
                    //ui->tedit_log->insertPlainText("\t" + str);

                    //ui->tabwid_db->setItem(row,column,new QTableWidgetItem(str));
                    QString head_str = header_lables.at(column);
                    if(0 == head_str.compare("AGE_CN",Qt::CaseInsensitive))
                    {
                        parse_age_string(str,age_number,age_unit);
                        str = age_number;
                        po.insert("AGE",str);
                    }

                    else if(0 == head_str.compare("Gender_CN",Qt::CaseInsensitive))
                    {
                        if(0 == str.compare("男"))
                        {
                            str = "M";
                        }else
                        if(0 == str.compare("女"))
                        {
                            str = "F";
                        }else
                        {
                            str = "O";
                        }
                        po.insert("Gender",str);
                    }
                    else if(0 == head_str.compare("Birth_CN",Qt::CaseInsensitive))
                    {
                        str = str.left(10);
                        po.insert("Birth",str);
                    }
                    else
                    {
                        po.insert(head_str,str);
                    }
                    column++;

                }

                po.insert(header_lables.at(column),age_unit);
                column++;

                pa.append(po);

                row++;
            }while (query.next());
        }
        json.insert("rec", "200");
        json.insert("worklist", QJsonValue(pa));
        json.insert("producer","PC-LinkHIS");
        json.insert("datetime",getDateTimeStamp());
    }else
    {
        QJsonArray pa;

        json.insert("rec", "400");
        json.insert("worklist", QJsonValue(pa));
        json.insert("producer","PC-LinkHIS");
        json.insert("datetime",getDateTimeStamp());
    }
    query.clear();

    document.setObject(json);
    //QByteArray byteArray = document.toJson(QJsonDocument::Compact);

    QByteArray jsonBuffer = document.toJson(QJsonDocument::Indented);
    return jsonBuffer;
}

QByteArray SQL_WORK::get_record_mysql(QString sql)
{
    LOG_DEBUG("sql:(%s)",qUtf8Printable(sql));

    QSqlQuery query(m_db);
    QString exe_cmd = sql;

    QJsonDocument document;
    // 构建 Json 对象
    QJsonObject json;

    query.exec(exe_cmd);

    LOG_DEBUG();

    if( query.isActive() )
    {
        //设置字段名
        QSqlRecord record = query.record();
        int field_count = record.count();

        QStringList header_lables;

        int index = 0;

        while(index < field_count)
        {
            header_lables.append(record.fieldName(index));
            index++;
        }

        //取得查询的条数
        int numRows = 0;
        if (m_db.driver()->hasFeature(QSqlDriver::QuerySize)) {
             numRows = query.size();
         } else {
             // this can be very slow
             if(query.last())
             {
                 numRows = query.at();
                 numRows = (numRows < 0) ? 0 : numRows + 1;
                 query.first();
             }
         }

        QJsonArray pa;

        // 设置表内容
        if(numRows > 0)
        {
            int row = 0;
            int column = 0;
            while (query.next()){
                QJsonObject po;

                column = 0;
                while(column < field_count)
                {
                    QString str = query.value(column).toString();
                    QString head_str = header_lables.at(column);
                    if(0 == head_str.compare("Gender",Qt::CaseInsensitive))
                    {
                        if(0 == str.compare("男"))
                        {
                            str = "M";
                        }else
                        if(0 == str.compare("女"))
                        {
                            str = "F";
                        }else
                        {
                            str = "O";
                        }
                    }
                    po.insert(header_lables.at(column),str);

                    column++;
                }

                pa.append(po);
                row++;
            }
        }
        json.insert("rec", "200");
        json.insert("worklist", QJsonValue(pa));
        json.insert("producer","PC-LinkHIS");
        json.insert("datetime",getDateTimeStamp());
    }else
    {
        QJsonArray pa;

        json.insert("rec", "400");
        json.insert("worklist", QJsonValue(pa));
        json.insert("producer","PC-LinkHIS");
        json.insert("datetime",getDateTimeStamp());
    }
    query.clear();

    document.setObject(json);
    //QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QByteArray jsonBuffer = document.toJson(QJsonDocument::Indented);


    return jsonBuffer;
}


QByteArray SQL_WORK::get_record_demo()
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
    //qDebug() << strJson;

    return byteArray;
}

QString SQL_WORK::getDateTimeStamp()
{
    QDateTime dt = QDateTime::currentDateTime();
    QString time_token = dt.toString("yy-MM-dd hh:mm:ss-zz");
    return time_token;
}

void SQL_WORK::doWork()
{
    LOG_DEBUG();

    while(true)
    {
        selectSQL();
        removeDatabase();

        LOG_DEBUG("json:(%s)",m_json.data());
        //发送查询到的结果
        emit resultReady(m_json);

        //睡眠 5 分钟
        sleep(5 * 60);

    }
}

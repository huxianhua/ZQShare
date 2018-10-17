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


SQL_RECORD::SQL_RECORD(QObject *parent) : QObject(parent)
{
    m_workThread = new QThread(this);

    connect(m_workThread,SIGNAL(started()),this,SLOT(s_select()));

}
int SQL_RECORD::Age_to_Day(QString value)
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
void SQL_RECORD::HIS_Result_Age(QString value,QString &age,QString &age_unit)
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
void SQL_RECORD::parse_age_string(QString age_str,QString &age_number,QString &age_unit)
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

QString SQL_RECORD::get_record_oracle(QSqlDatabase db,QString sql)
{
    QSqlQuery query(db);
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
        if (db.driver()->hasFeature(QSqlDriver::QuerySize)) {
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
                QJsonObject po;

                column = 0;
                while(column < field_count)
                {
                    QString str = query.value(column).toString();
                    //ui->tedit_log->insertPlainText("\t" + str);

                    //ui->tabwid_db->setItem(row,column,new QTableWidgetItem(str));
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
//                    if(0 == head_str.compare("性别",Qt::CaseInsensitive))
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

    m_json = document.toJson(QJsonDocument::Indented);
    return m_json;
}
/**
 * 解决: 年龄:  12岁 , 12岁2月, 2月10天,
 *      性别:  Gender_CN   男   女
 * @brief SQL_RECORD::get_record_oracle_180416
 * @param db
 * @param sql
 * @return
 */
QString SQL_RECORD::get_record_oracle_180416(QSqlDatabase db,QString sql)
{
    QSqlQuery query(db);
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
        if (db.driver()->hasFeature(QSqlDriver::QuerySize)) {
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

    m_json = document.toJson(QJsonDocument::Indented);
    return m_json;
}

QString SQL_RECORD::get_record_mysql(QSqlDatabase db,QString sql)
{
    QSqlQuery query(db);
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
        if (db.driver()->hasFeature(QSqlDriver::QuerySize)) {
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
                //ui->tedit_log->insertPlainText("\n");
                QJsonObject po;

                column = 0;
                while(column < field_count)
                {                                        
                    QString str = query.value(column).toString();
                    //ui->tedit_log->insertPlainText("\t" + str);

                    //ui->tabwid_db->setItem(row,column,new QTableWidgetItem(str));

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
    m_json = document.toJson(QJsonDocument::Indented);
    return m_json;
}
QString SQL_RECORD::get_record_sqlserver(QSqlDatabase db,QString sql)
{
    return get_record_oracle_180416(db,sql);
//    return get_record_oracle(db,sql);
}


QString SQL_RECORD::get_record_demo()
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

    QString strJson(byteArray);

    //qDebug() << strJson;

    return strJson;
}

QString SQL_RECORD::getDateTimeStamp()
{
    QDateTime dt = QDateTime::currentDateTime();
    QString time_token = dt.toString("yy-MM-dd hh:mm:ss-zz");
    return time_token;
}
void SQL_RECORD::s_select()
{

}

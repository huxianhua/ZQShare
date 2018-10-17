#ifndef SQL_RECORD_H
#define SQL_RECORD_H

#include <QObject>
#include <QJsonDocument>
#include <QSqlDatabase>
#include <QThread>

class SQL_RECORD : public QObject
{
    Q_OBJECT
public:
    explicit SQL_RECORD(QObject *parent = 0);

    static SQL_RECORD* instance() {
        static SQL_RECORD* instance = NULL;
           if(NULL == instance)
           {
               instance = new SQL_RECORD;
           }
           return instance;
      }

    void set_db(QSqlDatabase &db,QString sql){
        m_db = db;
        m_sql = sql;
    }

    QString get_record_oracle(QSqlDatabase db,QString sql);
    QString get_record_mysql(QSqlDatabase db,QString sql);
    QString get_record_sqlserver(QSqlDatabase db,QString sql);

    QString get_record_demo();

    //QSqlDatabase m_db;
    void parse_age_string(QString age_str, QString &age_number, QString &age_unit);
    int Age_to_Day(QString value);
    void HIS_Result_Age(QString value, QString &age, QString &age_unit);
    QString get_record_oracle_180416(QSqlDatabase db, QString sql);
private:
     QString m_json;

     QString getDateTimeStamp();

     QSqlDatabase m_db;
     QString m_sql;

     QThread *m_workThread;
     QString m_bufferJson;


signals:

public slots:
     void s_select();
};

#endif // SQL_RECORD_H

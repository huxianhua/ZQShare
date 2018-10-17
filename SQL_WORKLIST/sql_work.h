#ifndef SQL_WORK_H
#define SQL_WORK_H

#include <QObject>
#include <QVariantMap>
#include <QSettings>
#include <QSqlDatabase>

#ifndef  nullptr
#define nullptr 0
#endif

class SQL_WORK : public QObject
{
    Q_OBJECT
public:
    explicit SQL_WORK(QObject *parent = nullptr);

    /*
    static SQL_WORK* instance(QString config) {
        static SQL_WORK* instance = nullptr;
           if(nullptr == instance)
           {
               instance = new SQL_WORK;
               instance->setConfig(config);
           }
           return instance;
    }*/



    void readConfig(QString configFile);
    void setConfig(QString driverName,QString hostname,int port,QString sid,QString user,QString password,QString sql);
    void printCofnig();

    QByteArray selectSQL();
    void removeDatabase();

    QByteArray result();

private:
    QString m_driverName;
    QString m_hostname;
    int m_port;
    QString m_sid;
    QString m_user;
    QString m_password;
    QString m_sql;

    QSqlDatabase m_db;

    QByteArray m_json;


    int Age_to_Day(QString value);
    void HIS_Result_Age(QString value,QString &age,QString &age_unit);

    void parse_age_string(QString age_str,QString &age_number,QString &age_unit);
    QString getDateTimeStamp();

    QByteArray get_record_demo();

    QByteArray get_select_json(QString sql);
    QByteArray get_record_oracle(QString sql);
    QByteArray get_record_oracle_180416(QString sql);
    QByteArray get_record_mysql(QString sql);

public slots:
    void doWork();

signals:
  void resultReady(const QByteArray &result);


};

#endif // SQL_WORK_H

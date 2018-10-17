#ifndef SQL_RECORD_H
#define SQL_RECORD_H

#include <QObject>
#include <QVariantMap>
#include <QSettings>
#include <QSqlDatabase>

class SQL_RECORD : public QObject
{
    Q_OBJECT
public:
    explicit SQL_RECORD(QObject *parent = nullptr);

    static SQL_RECORD* instance(QString config) {
        static SQL_RECORD* instance = nullptr;
           if(nullptr == instance)
           {
               instance = new SQL_RECORD;
               instance->setConfig(config);
           }
           return instance;
    }


    void setConfig(QString path){
        m_config_filename = path;
    }


    QString runSQL();

    QString result();
    void saveResult(QString path = "");

    void removeDatabase();




private:
    QString m_config_filename;
   // QString m_outfile;

    QSettings *m_settings;
    QString m_driverName;

    QSqlDatabase m_db;

    QString m_json;


    int Age_to_Day(QString value);
    void HIS_Result_Age(QString value,QString &age,QString &age_unit);

    void parse_age_string(QString age_str,QString &age_number,QString &age_unit);
    QString getDateTimeStamp();

    QString get_record_demo();


    QString get_select_json(QString sql);
    QString get_record_oracle(QString sql);
    QString get_record_oracle_180416(QString sql);


};

#endif // SQL_RECORD_H

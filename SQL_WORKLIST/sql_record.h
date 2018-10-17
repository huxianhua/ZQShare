#ifndef SQL_RECORD_H
#define SQL_RECORD_H

#include <QObject>
#include <QJsonDocument>
#include <QSqlDatabase>
#include <QThread>

#include <QSettings>
#include <QProcess>

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

    ~SQL_RECORD(){
        workerThread.quit();
        workerThread.wait();
    }

    /**
     * @brief isValid 判定查询结果是否是有效的
     * @return
     */
    bool isValid();

    QByteArray get_record_400();
    QByteArray get_record_json();
    QByteArray get_record_demo();

public slots:

private:
     QByteArray m_json;

     QByteArray m_newJsonResult;
     QString m_bufferJson;
     QThread workerThread;


     QString getDateTimeStamp();

signals:
    void sig_result();


public slots:
      void handleResults(const QByteArray &);
signals:
      void operate(const QString &);


};

#endif // SQL_RECORD_H

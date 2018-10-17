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

    void setOutDir(QString outDir);
    void setWaitSec(int waitSec);
    void setProgram(QString programName,QString programConfig);

    /**
     * @brief isValid 判定查询结果是否是有效的
     * @return
     */
    bool isValid();

    QByteArray get_record_400();
    QByteArray get_record_json();
    QByteArray get_record_demo();

public slots:

    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void s_select();

private:
     QString m_json;

     QByteArray m_newJsonResult;

     QString m_outDir;
     int m_waitSec;

     QString m_programName;
     QString m_programConfig;
     QString m_saveFile;

     QString getDateTimeStamp();

     QThread *m_workThread;
     QString m_bufferJson;

     QProcess *m_process;


signals:
    void sig_result();
public slots:

};




class SelectThread:public QThread
{
    Q_OBJECT
public:
    SelectThread(QObject* parent=0);

private:
    QProcess *m_process;

public slots:
    //void s_xxx();
signals:
    //void sig();
protected:
    void run();
};

#endif // SQL_RECORD_H

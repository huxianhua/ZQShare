
#include <QCoreApplication>
#include <QApplication>

#include <QObject>

#include <QUrl>
#include <QDesktopServices>

#include <QFileInfo>
#include <QDebug>
#include <QProcess>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>


#include <QSysInfo>
#include <QSettings>
#include <QFile>
#include <QDateTime>
#include <QDir>
#include <QtNetwork>
#include <QHostInfo>
#include <QDesktopServices>
#include <QMessageBox>
#include <QIcon>
#include <QFileInfo>
#include <QHostAddress>
#include <QDesktopWidget>

#include <QSettings>
#include <QFile>

#include "mainwindow.h"



#include "QSingleApplication.h"
#include "http_worklist.h"

#include "sql_record.h"

#include "config.h"

//#if _MSC_VER >= 1600
//#pragma execution_character_set("utf-8")    dddxxx
//#endif


//HTTP_WORKLIST *m_wl_server;
QFile *global_logfile;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString & str)
{
    QString txt=str;
#else
void customMessageHandler(QtMsgType type, const char *msg)
{
    QString txt(msg);
#endif

    if(!global_logfile && !(global_logfile->isOpen()))
    {
        return;
    }

    static QMutex mutex;

    mutex.lock();
    QString log;

    switch(type)
    {
    case QtDebugMsg:
        log = QString("Debug: ");
        break;
    case QtWarningMsg:
        log = QString("Error: ");
        break;
    case QtCriticalMsg:
        log = QString("Critical Error: ");
        break;
    case QtFatalMsg:
        log = QString("Fatal Error: ");
        abort();
        break;
    default:
        log = QString("Unknow Msg Type : ");
        break;

    };


    log += QTime::currentTime().toString("hh:mm:ss.zzz, File: ") + QString(context.file) ;

    log += QString(", Line: " ) + QString::number(context.line) + QString(" : %1\n").arg(str);
    global_logfile->write(log.toStdString().c_str());
    global_logfile->flush();


//    log += QString("%1-%2 \n").arg(QTime::currentTime().toString("hh:mm:ss.zzz")).arg(QString(msg).toUtf8().data());
//    global_logfile->write(log.toUtf8());
//    global_logfile->flush();

    mutex.unlock();

}

#define LOG_DIR QString("%1/log").arg(qApp->applicationDirPath())



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(!QFile(HTTPJSON_CONFIG).exists())
    {
        LOG_FATAL("配置文件不存在！ (%s) ",qUtf8Printable(HTTPJSON_CONFIG));
    }


    QDir dir(LOG_DIR);

    if(dir.exists())
    {
        QString path = QString("%1/log%2.log").arg(LOG_DIR).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH_mm_ss"));
        global_logfile = new QFile(path);

        if (!global_logfile->open(QIODevice::ReadWrite | QIODevice::Append))
        {
            QString erinfo = global_logfile->errorString();
            printf("%s",erinfo.toUtf8().data());
        }

        #if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        qInstallMessageHandler(customMessageHandler);
        #else
        qInstallMsgHandler(customMessageHandler);
        #endif
    }else
    {
        qDebug("日志目录: %s  不存在，所以也不会生成日记",qUtf8Printable(LOG_DIR));
    }


    QString uniqueKey = QString(argv[0]);

    qDebug("path:(%s)",argv[0]);
    qDebug("path:(%s)",uniqueKey.toUtf8().data());
    uniqueKey= QCryptographicHash::hash(uniqueKey.toUtf8(), QCryptographicHash::Md5).toHex().constData();
    qDebug("uniqueKey:(%s)",uniqueKey.toLocal8Bit().data());

    uniqueKey = "SQL_WORKLIST";

    /*
    SingleApplication app(argc, argv, uniqueKey);
    QDir::setCurrent(qApp->applicationDirPath());
    qDebug("applicationFilePath:(%s)",qApp->applicationFilePath().toUtf8().data());

    if (app.isRunning())
    {
        app.sendMessage("程序已经运行了!");
        return EXIT_SUCCESS;
    }
*/
    MainWindow *mw = new MainWindow();

    mw->show();








    int rc = a.exec();

    if(global_logfile->isOpen())
    {
        global_logfile->close();
    }



    return rc;
}


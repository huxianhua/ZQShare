#include "mainwindow.h"
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

#include "QSingleApplication.h"

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

    SingleApplication app(argc, argv, uniqueKey);

    QDir::setCurrent(qApp->applicationDirPath());

    qDebug("applicationFilePath:(%s)",qApp->applicationFilePath().toUtf8().data());

    if (app.isRunning())
    {
        app.sendMessage("程序已经运行了!");
//        QMessageBox::information(NULL, "iMac信息录入系统",
//                                   "iMac信息录入系统正在运行...",
//                           QMessageBox::Ok);
        return EXIT_SUCCESS;
    }

    //主界面
    MainWindow *w  = MainWindow::instance();
    w->show();

    QObject::connect(&app, SIGNAL(messageAvailable(QString)), w, SLOT(s_singleApp_ReceiveMessage(QString)));

    //http 服务
    HTTP_WORKLIST *wl_server = new HTTP_WORKLIST (qApp);


    QObject::connect(w,SIGNAL(sig_start(int)),wl_server,SLOT(s_start(int)));
    QObject::connect(w,SIGNAL(sig_stop()),wl_server,SLOT(s_stop()));


    int rc = a.exec();

    if(global_logfile->isOpen())
    {
        global_logfile->close();
    }

    return rc;
}


int _x_main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //QApplication::addLibraryPath("D:/qtcode/build-Hello_QT-Desktop_Qt_5_7_0_MSVC2015_32bit-Debug/debug/sqldrivers");
    //QApplication::addLibraryPath("C:\\qt\\instantclient_12_2");

//    QProcess process;
//    QProcessEnvironment env;// = QProcessEnvironment::systemEnvironment();
//    env.insert("PATH", "$PATH;D:\qtcode\build-Hello_QT-Desktop_Qt_5_7_0_MSVC2015_32bit-Release\release\oracle_test_pub\oracle"); // Add an environment variable
//    process.setProcessEnvironment(env);
//    //process.start("myapp");



//    QString ip_test = "192.168.7.219";

//    QString suffer = ip_test.left(ip_test.lastIndexOf("."));

//    qDebug("suffer:(%s)",suffer.toUtf8().data());


   //  QDesktopServices::openUrl(QUrl("http://www.baidu.com"));

   MainWindow w;
   w.show();


/*
   // 构建 Json 对象 - Page
   QJsonObject po_1;
   po_1.insert("PatientID", "12012242231");
   po_1.insert("Name",  "胡途");
   po_1.insert("Gender", "F");
   po_1.insert("Birth",  "2080-10-02");

   QJsonObject po_2;
   po_2.insert("PatientID", "12012242232");
   po_2.insert("Name",  "袁大");
   po_2.insert("Gender", "F");
   po_2.insert("Birth",  "2070-10-02");


   QJsonArray pa;
   pa.append(po_1);
   pa.append(po_2);

   // 构建 Json 对象
   QJsonObject json;

   json.insert("rec", "200");
   json.insert("worklist", QJsonValue(pa));
   json.insert("producer","PC-LinkHIS");
   */

   QJsonArray pa;

   // 构建 Json 对象
   QJsonObject json;

   json.insert("rec", "400");
   json.insert("worklist", QJsonValue(pa));
   json.insert("producer","PC-LinkHIS");


   // 构建 Json 文档
   QJsonDocument document;
   document.setObject(json);

   //QByteArray byteArray = document.toJson(QJsonDocument::Compact);
   QByteArray byteArray = document.toJson(QJsonDocument::Indented);



   QString strJson(byteArray);

   qDebug() << strJson;

/*
   {
       "Company": "Digia",
       "From": 1991,
       "Name": "Qt",
       "Page": {
           "Developers": "https://www.qt.io/developers/",
           "Download": "https://www.qt.io/download/",
           "Home": "https://www.qt.io/"
       },
       "Version": [4.7999999999999998, 5.2000000000000002, 5.7000000000000002]
   }

{
    "rec":"200",
    "worklist":[
        {
            "PatientID":"12012242231",
            "Name":"胡途",
            "Gender":"F",
            "Age":"7",
            "AgeUnit":"M",
            "Birth":"2010-10-02"
        },
        {
            "PatientID":"12012242231",
            "Name":"袁大",
            "Gender":"F",
            "Age":"7",
            "AgeUnit":"M",
            "Birth":"2010-10-02"
        }
        ]
        }

*/


/*
    QString value = "18岁" ;

    int age;
    sscanf(value.toLatin1().data(),"%d*",&age);

    qDebug("age:(%d)", age);

    //C:\Users\xianhua\Desktop\dcm4che-master\dcm4che-tool\dcm4che-tool-mppsscp\src\main\java\org\dcm4che3\tool\mppsscp\MppsSCP.java
    QFileInfo file_info= QFileInfo("D:\Program\dcm4che-5.11.0\README.md");


//    qDebug() << file_info.absoluteDir();
    qDebug("%s",file_info.absoluteFilePath().toUtf8().data());
    qDebug("%s",file_info.absolutePath().toUtf8().data());

*/


    return a.exec();
}

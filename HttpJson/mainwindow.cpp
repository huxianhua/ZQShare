#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QSettings>

#include "config.h"
#include "http_worklist.h"
#include "sql_record.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    LOG_DEBUG("");




    LOG_DEBUG("");
   /*
    m_settings = new QSettings (HTTPJSON_CONFIG,QSettings::IniFormat);

    m_settings->beginGroup("HttpJson");
    int port = m_settings->value("port").toInt();
    QString recordOutDir = m_settings->value("recordOutDir").toString();
    int waitSec = m_settings->value("waitSec").toInt();
    m_settings->endGroup();



    m_settings->beginGroup("WorkList_Cmd");
    QString programName = m_settings->value("programName").toString();
    QString programConfig = m_settings->value("programConfig").toString();


    m_settings->endGroup();

    LOG_DEBUG("port:%d , recordOutDir:(%s) ", port,qUtf8Printable(recordOutDir));

    QDir outDir(recordOutDir);

    if(!outDir.exists())
    {
        outDir.mkpath(recordOutDir);
    }

    //http 服务
    m_wl = new HTTP_WORKLIST ();

    //设置导出的目录
    SQL_RECORD::instance()->setOutDir(recordOutDir);
    SQL_RECORD::instance()->setWaitSec(waitSec);
    SQL_RECORD::instance()->setProgram(programName,programConfig);


#if 0

    QThread workThread;
    LOG_DEBUG();
    SQL_RECORD::instance()->moveToThread(&workThread);

    LOG_DEBUG();
    QObject::connect(&workThread,SIGNAL(started()),SQL_RECORD::instance(),SLOT(s_select()));
    //connect(m_workThread,SIGNAL(finished()),&workThread,SLOT(deleteLater()));

    workThread.start();
    LOG_DEBUG();
#endif

    LOG_DEBUG("");
    //m_wl->s_start(port);
    LOG_DEBUG("");

    */

    /*

    m_wl->s_stop();
    m_wl->deleteLater();
    settings->deleteLater();

*/

}

MainWindow::~MainWindow()
{
    LOG_DEBUG("");

    delete ui;

    LOG_DEBUG("");
/*
    m_wl->s_stop();
    m_wl->deleteLater();
    m_settings->deleteLater();
*/
    LOG_DEBUG();

}
/*
void MainWindow::closeEvent(QCloseEvent *event)
{
    LOG_DEBUG();


}
*/

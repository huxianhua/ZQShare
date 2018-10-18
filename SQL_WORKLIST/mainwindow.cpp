#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTime>
#include<QSqlDriver>
#include<QSqlDatabase>
#include<QDebug>
#include <QSqlError>
#include <QTime>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>

#include <QHostInfo>
#include <QHostAddress>

#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <QSettings>

#include <QUrl>
#include <QDesktopServices>
#include <QProcess>
#include <QDesktopWidget>
#include <QInputDialog>

#include "sql_record.h"
#include "sliderpicture.h"
//
#include <QMenu>

#include "dialog_sql.h"

#include "config.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->m_app_version = VERSION;
    this->m_config_filename = QString("%1/%2-%3.ini")
            .arg(qApp->applicationDirPath())
            .arg(qApp->applicationName())
            .arg(this->m_app_version);


    qDebug("config_file:(%s)",this->m_config_filename.toUtf8().data());



    m_tabwidCurrentIndex = 0;
    ui->tabWidget_main->setCurrentIndex(m_tabwidCurrentIndex);

   // connect(ui->menu_options,SIGNAL(triggered(QAction*)),this,SLOT(s_open_options()));

  //void triggered(QAction *action);

   connect(ui->tabWidget_main,SIGNAL(tabBarClicked(int)),this,SLOT(s_tabbarClicked(int)));
   connect(ui->tabWidget_main,SIGNAL(currentChanged(int)),this,SLOT(s_currentChanged(int)));



    connect(ui->pbtn_link,SIGNAL(released()),this,SLOT(s_pbtn_link()));
    connect(ui->pbtn_run,SIGNAL(released()),this,SLOT(s_pbtn_run()));

    connect(this,SIGNAL(sig_connect()),this,SLOT(s_pbtn_link()));

    //oracle
    connect(ui->pbtn_tables,SIGNAL(pressed()),this,SLOT(s_pbtn_tables()));
    connect(ui->pbtn_views,SIGNAL(pressed()),this,SLOT(s_pbtn_views()));
    connect(ui->pbtn_default,SIGNAL(pressed()),this,SLOT(s_pbtn_default()));
    //mysql
    connect(ui->pbtn_tables_mysql,SIGNAL(pressed()),this,SLOT(s_pbtn_tables_mysql()));
    connect(ui->pbtn_views_mysql,SIGNAL(pressed()),this,SLOT(s_pbtn_views_mysql()));
    connect(ui->pbtn_default_mysql,SIGNAL(pressed()),this,SLOT(s_pbtn_default_mysql()));
    //sqlserver
    connect(ui->pbtn_tables_sqlserver,SIGNAL(pressed()),this,SLOT(s_pbtn_tables_sqlserver()));
    connect(ui->pbtn_views_sqlserver,SIGNAL(pressed()),this,SLOT(s_pbtn_views_sqlserver()));
    connect(ui->pbtn_default_sqlserver,SIGNAL(pressed()),this,SLOT(s_pbtn_default_sqlserver()));

    connect(ui->pbtn_close,SIGNAL(pressed()),this,SLOT(s_pbtn_close()));

    connect(ui->pbtn_http_start_stop,SIGNAL(released()),this,SLOT(s_pbtn_http_start_stop()));

    connect(ui->pbtn_http_open,SIGNAL(released()),this,SLOT(s_open_url()));

    connect(ui->pbtn_odbc_config,SIGNAL(released()),this,SLOT(s_profile_help()));

    connect(ui->pbtn_http_format,SIGNAL(released()),this,SLOT(s_http_format()));

    //connect(qApp,SIGNAL(lastWindowClosed()),this,SLOT(s_writeConfig()));
    connect(qApp,SIGNAL(lastWindowClosed()),this,SLOT(deleteLater()));


    connect(ui->tedit_sql,SIGNAL(textChanged()),this,SLOT(s_sql_textChanged()));



    ui->lab_status->setText(getColorString("red","Not connected!"));
    ui->pbtn_close->setEnabled(false);
    //ui->pbtn_run->setEnabled(false);
    ui->lab_elapsed->setText("");


    ui->statusBar->addWidget(ui->lab_status);
    ui->statusBar->addWidget(ui->lab_elapsed);


    m_isAutoStart = false;

    setIPComboBox();
    readConfig();

    createActions();
    createTrayIcon();

    m_trayIcon->setVisible(true);


    m_timeID = this->startTimer(500);

}
void MainWindow::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_timeID)
    {
        if(m_isAutoStart)
        {
            //加入自动连接
            s_pbtn_link();

            //自动执行语句
            s_pbtn_run();

            //启动worklist服务
#if 0
            connect(this,SIGNAL(sig_start_stop_released()),ui->pbtn_http_start_stop,SIGNAL(released()));
#else
            ui->pbtn_http_start_stop->setText("Stop");
            emit sig_start(ui->ledit_port->text().toInt());
            //background:
            ui->lab_http_title->setStyleSheet("background-color: rgb(85, 255, 0)");
            qDebug("[%s -- %d] sig_start",__FUNCTION__,__LINE__);
#endif

        }
        killTimer(m_timeID);
        m_timeID = 0;
    }
}

void MainWindow::readConfig()
{
    QFile config(this->m_config_filename);

    LOG_DEBUG("读取配置");

    if(config.exists())
    {
        LOG_DEBUG("读取配置 -- %s",qUtf8Printable(this->m_config_filename));

        //cbox_os cbox_bit cbox_auto
        QSettings *settings = new QSettings (this->m_config_filename,QSettings::IniFormat);
        settings->beginGroup("SYSTINFO");

        int para;
        QString para_str;
        //QString tab_name = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
        para = settings->value("driver_index",0).toInt();
        ui->tabWidget->setCurrentIndex(para);
        int tabWidgetIndex = para;

        para = settings->value("connect_state",0).toInt();
        if(para == 1)
        {
            emit sig_connect();
            qDebug("\n %s %d sig_connect \n",__FUNCTION__,__LINE__);
        }
        para = settings->value("cbox_auto",0).toInt();
        ui->cbox_auto->setCurrentIndex(para);
        setAutoStart((para == 0)? false:true);

        m_isAutoStart = (para == 0)? false:true;


        settings->endGroup();

        settings->beginGroup("oracle");
        //oracle 设置项
        para_str = settings->value("hostname_oracle","192.168.0.163").toString();
        ui->ledt_hostname_oracle->setText(para_str);

        para_str = settings->value("port_oracle","1521").toString();
        ui->ledt_port_oracle->setText(para_str);

        para_str = settings->value("sid_oracle","xe").toString();
        ui->ledt_sid_oracle->setText(para_str);

        para_str = settings->value("user_oracle","system").toString();
        ui->ledt_user_oracle->setText(para_str);

        para_str = settings->value("password_oracle","oracle").toString();
        ui->ledt_password_oracle->setText(para_str);

        m_oracle_sql = settings->value("defual_sql_oracle","select * from V_TJ_XDT where rownum < 100").toString();
        settings->endGroup();


        settings->beginGroup("sqlserver");
        //sqlserver
        para_str = settings->value("hostname_sqlserver","192.168.0.163").toString();
        ui->ledt_hostname_sqlserver->setText(para_str);

        para_str = settings->value("sid_sqlserver","QTDSM").toString();
        ui->ledt_sid_sqlserver->setText(para_str);

        para_str = settings->value("user_sqlserver","sa").toString();
        ui->ledt_user_sqlserver->setText(para_str);

        para_str = settings->value("password_sqlserver","root").toString();
        ui->ledt_password_sqlserver->setText(para_str);

        m_sqlserver_sql = settings->value("defual_sql_sqlserver","SELECT * FROM [SampleDB].[dbo].[TiJianH]").toString();
        settings->endGroup();

        settings->beginGroup("mysql");
        //mysql
        para_str = settings->value("hostname_mysql","192.168.7.16").toString();
        ui->ledt_hostname_mysql->setText(para_str);

        para_str = settings->value("port_mysql","3306").toString();
        ui->ledt_port_mysql->setText(para_str);

        para_str = settings->value("sid_mysql","zq_sql").toString();
        ui->ledt_sid_mysql->setText(para_str);

        para_str = settings->value("user_mysql","test").toString();
        ui->ledt_user_mysql->setText(para_str);

        para_str = settings->value("password_mysql","test").toString();
        ui->ledt_password_mysql->setText(para_str);
        m_mysql_sql = settings->value("defual_sql_mysql","SELECT * FROM users").toString();
        settings->endGroup();

        qDebug()<<"tabWidgetIndex: "<< tabWidgetIndex;
        qDebug()<<"m_oracle_sql: "<<m_oracle_sql;
        qDebug()<<"m_sqlserver_sql: "<<m_sqlserver_sql;
        qDebug()<<"m_mysql_sql: "<<m_mysql_sql;
        switch(tabWidgetIndex)
        {
        case 0:
            ui->tedit_sql->setText(m_oracle_sql);
            break;
        case 1:
            ui->tedit_sql->setText(m_sqlserver_sql);
            break;
        case 2:
            ui->tedit_sql->setText(m_mysql_sql);
            break;
        }

        delete settings;



    }else
    {
        LOG_DEBUG("读取配置 -- %s  文件不存在，使用默认参数",qUtf8Printable(this->m_config_filename));

        //默认的参数
        int para;
        //QString tab_name = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
        para = 0;
        ui->tabWidget->setCurrentIndex(para);

        para = 0;
        if(para == 1)
        {
            emit sig_connect();
            qDebug("\n %s %d sig_connect \n",__FUNCTION__,__LINE__);
        }
        para = 0;
        ui->cbox_auto->setCurrentIndex(para);
        setAutoStart((para == 0)? false:true);

        //oracle 设置项
        ui->ledt_hostname_oracle->setText("192.168.0.163");

        ui->ledt_port_oracle->setText("1521");
        ui->ledt_sid_oracle->setText("xe");
        ui->ledt_user_oracle->setText("system");
        ui->ledt_password_oracle->setText("oracle");
        m_oracle_sql = "select * from V_TJ_XDT where rownum < 100";

        //sqlserver
        ui->ledt_hostname_sqlserver->setText("192.168.0.163");
        ui->ledt_sid_sqlserver->setText("QTDSM");
        ui->ledt_user_sqlserver->setText("sa");
        ui->ledt_password_sqlserver->setText("root");
        m_sqlserver_sql = "SELECT * FROM [SampleDB].[dbo].[TiJianH]";

        //mysql
        ui->ledt_hostname_mysql->setText("192.168.7.16");
        ui->ledt_port_mysql->setText("3306");
        ui->ledt_sid_mysql->setText("zq_sql");
        ui->ledt_user_mysql->setText("test");
        ui->ledt_password_mysql->setText("test");
        m_mysql_sql = "SELECT * FROM users";
    }
}
void MainWindow::writeConfig()
{
    LOG_DEBUG("写入配置 -- %s",qUtf8Printable(this->m_config_filename));

    QSettings *settings = new QSettings (this->m_config_filename,QSettings::IniFormat);
    settings->beginGroup("SYSTINFO");

    settings->setValue("driver_index",ui->tabWidget->currentIndex());

    if(!m_db.isOpen())
    {
        settings->setValue("connect_state",0);
    }else
    {
        settings->setValue("connect_state",1);
    }
    settings->setValue("cbox_auto",ui->cbox_auto->currentIndex());
    settings->endGroup();

    settings->beginGroup("oracle");
    settings->setValue("hostname_oracle",ui->ledt_hostname_oracle->text());
    settings->setValue("port_oracle",ui->ledt_port_oracle->text());
    settings->setValue("sid_oracle",ui->ledt_sid_oracle->text());
    settings->setValue("user_oracle",ui->ledt_user_oracle->text());
    settings->setValue("password_oracle",ui->ledt_password_oracle->text());
    settings->setValue("defual_sql_oracle",m_oracle_sql);
    settings->endGroup();

    settings->beginGroup("sqlserver");
    settings->setValue("hostname_sqlserver",ui->ledt_hostname_sqlserver->text());
    settings->setValue("sid_sqlserver",ui->ledt_sid_sqlserver->text());
    settings->setValue("user_sqlserver",ui->ledt_user_sqlserver->text());
    settings->setValue("password_sqlserver",ui->ledt_password_sqlserver->text());
    settings->setValue("defual_sql_sqlserver",m_sqlserver_sql);
    settings->endGroup();

    settings->beginGroup("mysql");
    settings->setValue("hostname_mysql",ui->ledt_hostname_mysql->text());
    settings->setValue("port_mysql",ui->ledt_port_mysql->text());
    settings->setValue("sid_mysql",ui->ledt_sid_mysql->text());
    settings->setValue("user_mysql",ui->ledt_user_mysql->text());
    settings->setValue("password_mysql",ui->ledt_password_mysql->text());
    settings->setValue("defual_sql_mysql",m_mysql_sql);
    settings->endGroup();

    delete settings;
}




QString MainWindow::getColorString(QString color,QString contents)
{
    QString htmlString = QString("<font color=%1> %2 </font>").arg(color).arg(contents);
    return htmlString;
}
QString MainWindow::getDateTimeStamp()
{
    QDateTime dt = QDateTime::currentDateTime();
    QString time_token = dt.toString("[yy-MM-dd hh:mm:ss-zz]");
    return time_token;
}


QString MainWindow::get_select_json()
{
    QString json;

    if(0 == m_driverName.compare("QMYSQL",Qt::CaseInsensitive))
    {
        //json = SQL_RECORD::instance()->get_record_mysql(this->m_db,ui->tedit_sql->toPlainText());

    }else
    if(0 == m_driverName.compare("QOCI",Qt::CaseInsensitive))
    {
        //json = SQL_RECORD::instance()->get_record_oracle_180416(this->m_db,ui->tedit_sql->toPlainText());
//        json = SQL_RECORD::instance()->get_record_oracle(this->m_db,ui->tedit_sql->toPlainText());
    }else
    if(0 == m_driverName.compare("QODBC",Qt::CaseInsensitive))
    {
        //json = SQL_RECORD::instance()->get_record_sqlserver(this->m_db,ui->tedit_sql->toPlainText());
    }else
    {
        QMessageBox::information(this, tr("警告"),
                                 QString("请检查 %1 类型是否支持").arg(m_driverName));
    }
    return json;
}
QString MainWindow::get_json_demo()
{
    QString json;
    json = SQL_RECORD::instance()->get_record_demo();

    return json;
}




void MainWindow::test_oracle()
{
    qDebug()<<"Available drivers:";
    QStringList drivers= QSqlDatabase::drivers();
    foreach(QString driver,drivers)
    qDebug()<<driver;
}
void MainWindow::test_sqlserver_connect()
{
    qWarning("add Database....====");

    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

    qWarning("setHostName....====");
    db.setHostName("192.168.0.163");
    //this->db.setPort(this->port.toInt());
    db.setDatabaseName("QTDSM");
    db.setUserName("sa");
    db.setPassword("root");
    qWarning("db.open()....====");
    if(!db.open())
    {
        qWarning("\n connect failed \n");
        qWarning() << db.lastError().databaseText() ;
        //return 0;
    }else
    {
        qDebug("\n connect success \n");
        qDebug("\n connect success \n");
    }
    db.close();
}



int MainWindow::test_open_oracle()
{

    QSqlDatabase db = QSqlDatabase::addDatabase("QOCI");

    db.setHostName("192.168.0.163");
    db.setPort(1521);
    db.setDatabaseName("xe");
    db.setUserName("system");
    db.setPassword("oracle");
    qWarning("db.open()....====");
    if(!db.open())
    {
        qWarning("\n connect failed \n");
        qWarning() << db.lastError().databaseText() ;
        return 0;
    }else
    {
        qDebug("\n connect success \n");
        qDebug("\n connect success \n");
    }
    return 1;
}
void MainWindow::s_tabbarClicked(int index)
{
    qDebug("");
    LOG_DEBUG("index:(%d)",index);

    if(SETTING_LOCK)
    {
        if(index == 1)
        {

            bool isok;
            QString text = QInputDialog::getText(this,"设置修改","请输入密码",QLineEdit::Password,"",&isok);

            if(isok)
            {
                if(text.compare("pw") == 0 || text.compare(PASSWORD) == 0)
                {
                    m_tabwidCurrentIndex = index;
                    LOG_DEBUG("密码正确！");
                }else
                {
                    QMessageBox::warning(this,"警告","密码错误",QMessageBox::Ok);
                }

            }else
            {
                QMessageBox::warning(this,"警告","如要修改请联系维护人员-huxianhua@zoncare.cn",QMessageBox::Ok);

            }
        }else
        {
            m_tabwidCurrentIndex = index;
        }
    }else
    {
        m_tabwidCurrentIndex = index;
    }


}
void MainWindow::s_currentChanged(int index)
{
    LOG_DEBUG("index:(%d)",index);
    ui->tabWidget_main->setCurrentIndex(m_tabwidCurrentIndex);
}

//连接操作得放入线程中。。。。
void MainWindow::s_pbtn_link()
{    

   LOG_DEBUG("进入连接数据接口");

    if( m_db.isOpen())
    {
        LOG_DEBUG("数据是打开的");
        return;
    }

    QString tab_name = ui->tabWidget->tabText(ui->tabWidget->currentIndex());   

    if(0 == tab_name.compare("mysql",Qt::CaseInsensitive))
    {
        m_driverName = "QMYSQL";

        m_db = QSqlDatabase::addDatabase(m_driverName);
        m_db.setHostName(ui->ledt_hostname_mysql->text());
        m_db.setPort(ui->ledt_port_mysql->text().toInt());
        m_db.setDatabaseName(ui->ledt_sid_mysql->text());
        m_db.setUserName(ui->ledt_user_mysql->text());
        m_db.setPassword(ui->ledt_password_mysql->text());

        m_mysql_sql = ui->tedit_sql->toPlainText();
    }else
    if(0 == tab_name.compare("oracle",Qt::CaseInsensitive))
    {
        m_driverName = "QOCI";

        m_db = QSqlDatabase::addDatabase(m_driverName);
        m_db.setHostName(ui->ledt_hostname_oracle->text());
        m_db.setPort(ui->ledt_port_oracle->text().toInt());
        m_db.setDatabaseName(ui->ledt_sid_oracle->text());
        m_db.setUserName(ui->ledt_user_oracle->text());
        m_db.setPassword(ui->ledt_password_oracle->text());

        m_oracle_sql = ui->tedit_sql->toPlainText();

    }else
    if(0 == tab_name.compare("sqlServer",Qt::CaseInsensitive))
    {
        m_driverName = "QODBC";
        m_db = QSqlDatabase::addDatabase(m_driverName);
        m_db.setHostName(ui->ledt_hostname_sqlserver->text());
        m_db.setDatabaseName(ui->ledt_sid_sqlserver->text());
        m_db.setUserName(ui->ledt_user_sqlserver->text());
        m_db.setPassword(ui->ledt_password_sqlserver->text());

        m_sqlserver_sql = ui->tedit_sql->toPlainText();
    }else
    if(0 == tab_name.compare("sqlite",Qt::CaseInsensitive))
    {
        m_driverName = "QSQLITE";
        qWarning("数据库名称:(%s) 不支持,请联系开发人员改进!", qPrintable(tab_name));

        QMessageBox::information(this, tr("警告"),
                                 tr("请检查 Tab 上的名称是否与数据库类型名一致</b>."));

        return ;

    }else
    {
        qWarning("数据库名称:(%s) 不支持,请联系开发人员改进!", qPrintable(tab_name));

        QMessageBox::information(this, tr("警告"),
                                 tr("请检查 Tab 上的名称是否与数据库类型名一致</b>."));

        return ;
    }

    //m_db = QSqlDatabase::addDatabase("QOCI");

    qDebug("driverName:(%s)",m_driverName.toUtf8().data());

    LOG_DEBUG("数据连接时 -- 写入配置");
    writeConfig();


    if(!m_db.isOpen())
    {
        LOG_DEBUG("!m_db.isOpen()");
    }else
    {
        LOG_DEBUG("m_db.isOpen");
    }

    if(!m_db.isValid())
    {
        LOG_DEBUG("!m_db.isValid");
    }else
    {
        LOG_DEBUG("m_db.isValid");
    }

    if(!m_db.isOpen())
    {

        bool ok = m_db.open();

        if(ok)
        {
            ui->tedit_log->insertPlainText("\n" + getDateTimeStamp() + " connect success");

            ui->lab_status->setText(getColorString("green",QString("%1 -- Already connected!").arg(this->m_driverName)));

            ui->pbtn_link->setEnabled(false);
            ui->pbtn_close->setEnabled(true);
            ui->pbtn_run->setEnabled(true);

            if(0 == m_driverName.compare("QMYSQL",Qt::CaseInsensitive))
            {
                QSqlQuery query(this->m_db);
                QString exe_cmd = QString("use %1;").arg(ui->ledt_sid_mysql->text());
                if(!query.exec(exe_cmd) )
                {
                    QMessageBox::information(this, tr("警告-mysql"),
                                             QString("执行 use %1; 异常! ").arg(ui->ledt_sid_mysql->text()));
                }

            }
        }else
        {
            ui->tedit_log->insertPlainText("\n" + getDateTimeStamp() + m_db.lastError().databaseText());
        }
    }

    ui->tedit_log->moveCursor(QTextCursor::End);


}
void MainWindow::s_pbtn_close()
{
    if(m_db.isOpen())
    {
        m_db.close();
        ui->lab_status->setText(getColorString("red","Not connected!"));
        ui->pbtn_close->setEnabled(false);
        ui->pbtn_run->setEnabled(false);
        ui->pbtn_link->setEnabled(true);

        ui->lab_elapsed->setText("");

        QDateTime dt = QDateTime::currentDateTime();
        QString time_token = dt.toString("yy-MM-dd hh:mm:ss-zz");

        ui->tedit_log->insertPlainText("\n" + getDateTimeStamp() + " database close");

    }
}
void MainWindow::s_pbtn_http_start_stop()
{
    QPushButton *btn_http = qobject_cast<QPushButton *>(sender());

    if(0 == btn_http->text().compare("Start",Qt::CaseInsensitive))
    {
        btn_http->setText("Stop");

//        m_wl_server->setPort(ui->ledit_port->text().toInt());
//        m_wl_server->s_start();

        emit sig_start(ui->ledit_port->text().toInt());

        //background:
        ui->lab_http_title->setStyleSheet("background-color: rgb(85, 255, 0)");
        qDebug("[%s -- %d] sig_start",__FUNCTION__,__LINE__);
    }else
    if(0 == btn_http->text().compare("Stop",Qt::CaseInsensitive))
    {
        btn_http->setText("Start");

        //m_wl_server->s_stop();

        emit sig_stop();

        //ui->lab_0_0_0_1->setText("");
        ui->lab_http_title->setStyleSheet("");
        qDebug("[%s -- %d] sig_stop",__FUNCTION__,__LINE__);
    }else
    {
        QMessageBox msgBox;
        msgBox.setText(QString("逻辑异常! [%1 -- %2]  %3").arg(__FUNCTION__).arg(__LINE__).arg(btn_http->text()) );
        msgBox.exec();
    }

}

void MainWindow::s_open_url()
{
    QString open_url = QString("http://%1:%2").arg(ui->cbox_ip->currentText()).arg(ui->ledit_port->text());

//    QMessageBox msgBox;
//    msgBox.setText(QString("打开! [%1 -- %2]  %3").arg(__FUNCTION__).arg(__LINE__).arg(open_url) );
//    msgBox.exec();


    QDesktopServices::openUrl(QUrl(open_url));

}


/*
void MainWindow::s_pbtn_run()
{
//    m_db = QSqlDatabase::addDatabase("QOCI");
//    m_db.setHostName(ui->ledt_hostname->text());
//    m_db.setPort(ui->ledt_port->text().toInt());
//    m_db.setDatabaseName(ui->ledt_sid->text());
//    m_db.setUserName(ui->ledt_user->text());
//    m_db.setPassword(ui->ledt_password->text());

    QString driverName = "QMYSQL";

    m_db = QSqlDatabase::addDatabase(driverName);
    m_db.setHostName(ui->ledt_hostname_mysql->text());
    m_db.setPort(ui->ledt_port_mysql->text().toInt());
    m_db.setDatabaseName(ui->ledt_sid_mysql->text());
    m_db.setUserName(ui->ledt_user_mysql->text());
    m_db.setPassword(ui->ledt_password_mysql->text());


    bool ok = m_db.open();

    QDateTime dt = QDateTime::currentDateTime();
    QString time_token = dt.toString("yy-MM-dd hh:mm:ss-zz");

    if(ok)
    {
        ui->tedit_log->insertPlainText("\n [" + time_token + "] connect success");
    }else
    {
        ui->tedit_log->insertPlainText("\n[" + time_token + "]" + m_db.lastError().databaseText());
    }
    ui->tedit_log->moveCursor(QTextCursor::End);

    QSqlQuery query(this->m_db);
    //表
    //query.exec("SELECT * FROM user_tables ORDER by TABLE_NAME ASC");

    QString exe_cmd = QString("use %1;").arg(ui->ledt_sid_mysql->text());
    if(! query.exec(exe_cmd) )
    {
        QMessageBox::information(this, tr("警告"),
                                 tr(" use  db !"));

    }


    query.exec(ui->tedit_sql->toPlainText());

    if( query.isActive() )
    {
        //设置字段名
        QSqlRecord record = query.record();
        int field_count = record.count();

        int index = 0;
        while(index < field_count)
        {
           //t.setVariable(QString("field%1.name").arg(index),record.fieldName(index));

            QString str = record.fieldName(index);
            ui->tedit_log->insertPlainText("\n" + str);

           index++;
        }
        int numRows = 0;
        if (this->m_db.driver()->hasFeature(QSqlDriver::QuerySize)) {
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
        ui->tedit_log->insertPlainText("\n --------------------------------");

        // 设置表内容
        //t.loop("rows",numRows);
        if(numRows > 0)
        {
            do{
                index = 0;
                while(index < field_count)
                {
                    //t.setVariableSingle(QString("field%1.para").arg(index),query.value(index).toString());
                    QString str = query.value(index).toString();
                    ui->tedit_log->insertPlainText("\n" + str);

                    ++index;
                }
            }while (query.next());

        }
       // response.write(t.toUtf8(),true);
    }else
    {
//        QByteArray body("./oracle.html?warning=");
//        body.append(exe_cmd);
//        body.append("--Can not find results!");
//        response.redirect(body);
    }

    m_db.close();
}

*/


void MainWindow::run_oracle(QString sql)
{
    QTime t;
    t.start();

    QSqlQuery query(this->m_db);

    QString exe_cmd = sql;
    query.exec(exe_cmd);

    if( query.isActive() )
    {
        //设置字段名
        QSqlRecord record = query.record();
        int field_count = record.count();

        QStringList header_lables;

        int index = 0;
        ui->tedit_log->insertPlainText("\n");

        while(index < field_count)
        {
            QString str = record.fieldName(index);
            ui->tedit_log->insertPlainText("\t" + str);

            header_lables.append(record.fieldName(index));
            index++;
        }

        ui->tabwid_db->clear();
        int rowCount = ui->tabwid_db->rowCount();
        for(index = rowCount -1 ;index >= 0;index--)
        {
            ui->tabwid_db->removeRow(index);
        }

        //设置表标题
        ui->tabwid_db->setColumnCount(field_count);
        ui->tabwid_db->setHorizontalHeaderLabels(header_lables);

        //取得查询的条数
        int numRows = 0;
        if (this->m_db.driver()->hasFeature(QSqlDriver::QuerySize)) {
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
        ui->tabwid_db->setRowCount(numRows);

        ui->tedit_log->insertPlainText("\n --------------------------------");

        // 设置表内容
        if(numRows > 0)
        {
            int row = 0;
            int column = 0;
            do{
                ui->tedit_log->insertPlainText("\n");
                column = 0;
                while(column < field_count)
                {
                    QString str = query.value(column).toString();
                    ui->tedit_log->insertPlainText("\t" + str);

                    ui->tabwid_db->setItem(row,column,new QTableWidgetItem(str));
                    column++;
                }
                row++;
            }while (query.next());
        }
        ui->lab_elapsed->setText(QString("查询 %1 条记录, 耗时: %2 ms").arg(numRows).arg(t.elapsed() ));
    }else
    {
        ui->tedit_log->insertPlainText("\n" + getDateTimeStamp() + "Can not find results!");
        ui->tedit_log->moveCursor(QTextCursor::End);
    }

    query.clear();

}

void MainWindow::run_mysql(QString sql)
{
    QTime t;
    t.start();

    QSqlQuery query(this->m_db);

    QString exe_cmd = sql;
    query.exec(exe_cmd);

    if( query.isActive() )
    {
        //设置字段名
        QSqlRecord record = query.record();
        int field_count = record.count();

        QStringList header_lables;

        int index = 0;
        ui->tedit_log->insertPlainText("\n");

        while(index < field_count)
        {
            QString str = record.fieldName(index);
            ui->tedit_log->insertPlainText("\t" + str);
            header_lables.append(record.fieldName(index));
           index++;
        }

        ui->tabwid_db->clear();
        int rowCount = ui->tabwid_db->rowCount();
        for(index = rowCount -1 ;index >= 0;index--)
        {
            ui->tabwid_db->removeRow(index);
        }

        //设置表标题
        ui->tabwid_db->setColumnCount(field_count);
        ui->tabwid_db->setHorizontalHeaderLabels(header_lables);

        //取得查询的条数
        int numRows = 0;
        if (this->m_db.driver()->hasFeature(QSqlDriver::QuerySize)) {
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
        ui->tabwid_db->setRowCount(numRows);
        ui->tedit_log->insertPlainText("\n --------------------------------");

        // 设置表内容
        if(numRows > 0)
        {
            int row = 0;
            int column = 0;
            while (query.next()){
                ui->tedit_log->insertPlainText("\n");
                column = 0;
                while(column < field_count)
                {
                    QString str = query.value(column).toString();
                    ui->tedit_log->insertPlainText("\t" + str);

                    ui->tabwid_db->setItem(row,column,new QTableWidgetItem(str));
                    column++;
                }
                row++;
            }
        }
        ui->lab_elapsed->setText(QString("查询 %1 条记录, 耗时: %2 ms").arg(numRows).arg(t.elapsed() ));
    }else
    {
        ui->tedit_log->insertPlainText("\n" + getDateTimeStamp() + "Can not find results!");
        ui->tedit_log->moveCursor(QTextCursor::End);
    }
    query.clear();

}
void MainWindow::run_sqlserver(QString sql)
{
    run_oracle(sql);
}

void MainWindow::s_pbtn_run()
{
//    m_db = QSqlDatabase::addDatabase("QOCI");
//    m_db.setHostName(ui->ledt_hostname->text());
//    m_db.setPort(ui->ledt_port->text().toInt());
//    m_db.setDatabaseName(ui->ledt_sid->text());
//    m_db.setUserName(ui->ledt_user->text());
//    m_db.setPassword(ui->ledt_password->text());
    //bool ok = m_db.open();

    QDateTime dt = QDateTime::currentDateTime();
    QString time_token = dt.toString("yy-MM-dd hh:mm:ss-zz");

    ui->tedit_log->clear();

    if(m_db.isOpen())
    {
        ui->tedit_log->insertPlainText("\n" + getDateTimeStamp() + " connect success");
        ui->tedit_log->moveCursor(QTextCursor::End);
    }else
    {
        ui->tedit_log->insertPlainText("\n" + getDateTimeStamp() + m_db.lastError().databaseText());
        ui->tedit_log->moveCursor(QTextCursor::End);

        ui->lab_status->setText(getColorString("red","Not connected!"));
        ui->pbtn_close->setEnabled(false);
        ui->pbtn_run->setEnabled(false);
        ui->pbtn_link->setEnabled(true);

        ui->lab_elapsed->setText("");
        return;
    }
    if(0 == m_driverName.compare("QMYSQL",Qt::CaseInsensitive))
    {
        run_mysql(ui->tedit_sql_test->toPlainText());

    }else
    if(0 == m_driverName.compare("QOCI",Qt::CaseInsensitive))
    {
        run_oracle(ui->tedit_sql_test->toPlainText());

    }else
    if(0 == m_driverName.compare("QODBC",Qt::CaseInsensitive))
    {
        run_sqlserver(ui->tedit_sql_test->toPlainText());
    }else
    {
        QMessageBox::information(this, tr("警告"),
                                 QString("请检查 %1 类型是否支持").arg(m_driverName));

        return;
    }


    QString json = get_select_json();
    ui->tedit_log->insertPlainText(QString("\n %1 %2").arg(getDateTimeStamp()).arg(json));
    ui->tedit_log->moveCursor(QTextCursor::End);
}

void MainWindow::s_pbtn_tables()
{
    ui->tedit_sql->setPlainText("SELECT * FROM user_tables ORDER by TABLE_NAME ASC");
}
void MainWindow::s_pbtn_views()
{
    ui->tedit_sql->setPlainText("SELECT * FROM user_views ORDER by VIEW_NAME ASC");
}

void MainWindow::s_pbtn_default()
{
    ui->tedit_sql->setPlainText(m_oracle_sql);
}
void MainWindow::s_pbtn_tables_mysql()
{
    ui->tedit_sql->setPlainText("show table status where comment !='view'");
}
void MainWindow::s_pbtn_views_mysql()
{
    ui->tedit_sql->setPlainText("show table status where comment ='view'");
}
void MainWindow::s_pbtn_default_mysql()
{
    ui->tedit_sql->setPlainText(m_mysql_sql);
}
//sqlserver
void MainWindow::s_pbtn_tables_sqlserver()
{
    ui->tedit_sql->setPlainText("SELECT * FROM sys.objects WHERE type = 'U' ORDER by name ASC");
}
void MainWindow::s_pbtn_views_sqlserver()
{
    ui->tedit_sql->setPlainText("SELECT * FROM sys.all_views WHERE type = 'V' ORDER by name ASC");
}
void MainWindow::s_pbtn_default_sqlserver()
{
    ui->tedit_sql->setPlainText(m_sqlserver_sql);
}
void MainWindow::s_sql_textChanged()
{

}
void MainWindow::s_writeConfig()
{

    LOG_DEBUG("s_writeConfig时 -- 写入配置");
    writeConfig();
}

void MainWindow::s_auto_run_currentIndexChanged(int index)
{
    setAutoStart((index == 0)? false:true);
}


void MainWindow::setAutoStart(bool is_auto_start)
{
    QString reg_auto_run = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";

    QString application_name = QApplication::applicationName();
    QSettings *settings = new QSettings(reg_auto_run, QSettings::NativeFormat);

    QString set_auto_run_log;

    QString application_path = QApplication::applicationFilePath().replace("/", "\\");

    if(is_auto_start)
    {
        if(0 != application_path.compare(settings->value(application_name,"null").toString()))
        {
            settings->setValue(application_name, application_path.replace("/", "\\"));
        }
        set_auto_run_log = QString("增加 自动启动！ %1").arg(application_path);
        qDebug("%s",set_auto_run_log.toUtf8().data());
        ui->tedit_log->insertPlainText("\n" +  set_auto_run_log + "\n" );
        ui->tedit_log->moveCursor(QTextCursor::End);

    }
    else
    {
        set_auto_run_log = QString("取消 自动启动！ %1").arg(application_path);
        qDebug("%s",set_auto_run_log.toUtf8().data());
        ui->tedit_log->insertPlainText("\n" +  set_auto_run_log + "\n" );
        ui->tedit_log->moveCursor(QTextCursor::End);

        settings->remove(application_name);
    }
    delete settings;

}

void MainWindow::setIPComboBox()
{
    QString localHostName = QHostInfo::localHostName();
    qDebug() <<"localHostName:"<<localHostName; //计算机名

    QHostInfo info = QHostInfo::fromName(localHostName);
    qDebug() << "IP Address:" << info.addresses();


    ui->cbox_ip->clear();

    foreach(QHostAddress address, info.addresses())
    {
        if (address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            qDebug() << "IPv4 Address:" << address.toString();

            ui->cbox_ip->addItem(address.toString());
        }
    }
}


void MainWindow::s_profile_help()
{
    QString profile = QString("%1/%2").arg(QDir::currentPath()).arg("/resource/HisLink_SQLServer.html");

    //QString profile = ":/help/resource/HisLink_SQLServer.html";
    QFile help_file(profile);

    if(help_file.exists())
    {
        showHelp(profile);
    }else
    {
        showHelp("C:/Users/root/Documents/Hello_QT/resource/HisLink_SQLServer.html");
    }
}

void MainWindow::s_http_format()
{
    QString profile = QString("%1/%2").arg(QDir::currentPath()).arg("/resource/json.html");

    //QString profile = ":/help/resource/HisLink_SQLServer.html";
    QFile help_file(profile);

    if(help_file.exists())
    {
        showHelp(profile);
    }else
    {
        showHelp("F:/work/QT/iMac_Tools/SQL_WORKLIST/resource/json.html");
    }
}


void MainWindow::showHelp(QString htmlPath)
{
    QFileInfo absolutePath(htmlPath);

    QTextEdit *tedit = new QTextEdit("空的帮助内容");
    QString currentDir = QDir::currentPath();
    QDir::setCurrent(absolutePath.absolutePath());

    //QFile help_firewall("firewall.html");
    QFile help_firewall(htmlPath);
    //
    if (help_firewall.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      tedit->setHtml(help_firewall.readAll());
      help_firewall.close();
    }

    tedit->resize(900,600);

    tedit->show();
    QDir::setCurrent(currentDir);
}

//tabwid_db

/*
ui->tabwid_7_1_2->setRowCount(list_size);
ui->tabwid_7_1_2->setColumnCount(5);

QStringList header_lables;
header_lables << "转换类型" << "传输方式" << "命名规则" << "传输目录" << "文件";
ui->tabwid_7_1_2->setHorizontalHeaderLabels(header_lables);

for ( int i=0; i!= list_size; ++i )
{
   QVariantMap task_info = result_list.at(i).toMap();
   //print_map(task_info);
   ui->tabwid_7_1_2->setItem(i,0,new QTableWidgetItem(BusCommon::get_Bus_mode(task_info["BusMode"].toInt())  ));
   ui->tabwid_7_1_2->setItem(i,1,new QTableWidgetItem(BusCommon::get_TransferMode(task_info["TransferMode"].toInt())  ));
   ui->tabwid_7_1_2->setItem(i,2,new QTableWidgetItem(BusCommon::get_NameRule(task_info["NameRule"].toInt())  ));
   ui->tabwid_7_1_2->setItem(i,3,new QTableWidgetItem(task_info["directory"].toString()  ));
   ui->tabwid_7_1_2->setItem(i,4,new QTableWidgetItem(QFileInfo(task_info["FileName"].toString()).fileName()  ));
}
*/
void MainWindow::createActions()
{
    m_action_Show = new QAction(tr("显示"), this);
    connect(m_action_Show, SIGNAL(triggered()), this, SLOT(show()));

    m_action_Hide = new QAction(tr("隐藏"), this);
    connect(m_action_Hide, SIGNAL(triggered()), this, SLOT(hide()));

    m_action_Restore = new QAction(tr("恢复"), this);
    connect(m_action_Restore, SIGNAL(triggered()), this, SLOT(showNormal()));

    m_action_Quit = new QAction(tr("退出"), this);
    connect(m_action_Quit, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWindow::createTrayIcon()
{
    m_trayIconMenu = new QMenu(this);
    m_trayIconMenu->addAction(m_action_Show);
    m_trayIconMenu->addAction(m_action_Hide);
    m_trayIconMenu->addAction(m_action_Restore);
    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addAction(m_action_Quit);

    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setContextMenu(m_trayIconMenu);

    QIcon icon = QIcon(":/images/logo.ico");
    m_trayIcon->setIcon(icon);
//    setWindowIcon(icon);

    m_trayIcon->setToolTip("HISSQL数据库查询与JSON服务-iMac");


    connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(s_trayIcon_activated(QSystemTrayIcon::ActivationReason)));

}

void MainWindow::s_trayIcon_activated(QSystemTrayIcon::ActivationReason)
{
    this->show();
}




void MainWindow::s_singleApp_ReceiveMessage(QString message)
{
    this->setGeometry(
                (QApplication::desktop()->width() - this->width()) / 2,
                (QApplication::desktop()->height() - this->height()) / 2,
                this->width(),this->height());
    this->show();

    ui->tedit_log->insertPlainText("\n" + message);
    ui->tedit_log->moveCursor(QTextCursor::End);

    qDebug("s_singleApp_ReceiveMessage:(%s)",message.toUtf8().data());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    /*
    QMessageBox::information(this, tr("系统托盘"),
                             tr("该程序将继续运行在系统托盘中。要终止程序，在系统托盘条目的上下文菜单中选择 <b> 退出 </b>."));
*/

    QMessageBox msgBox;
    msgBox.setText("此程序会在系统托盘显示.");
    msgBox.setInformativeText("是否要终止程序?"
                              "\n Yes \t终止程序"
                              "\n No  \t隐藏程序"
                              "\n Cancle \t取消");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();

    switch (ret) {
      case QMessageBox::Yes:
            LOG_DEBUG("退出程序时 -- 写入配置");
            writeConfig();
            exit(0);
          break;
      case QMessageBox::No:
            hide();
            event->ignore();
            break;
      case QMessageBox::Cancel:
            event->ignore();
            break;
      default:
          // should never be reached
          break;
    }
}

void MainWindow::on_pbtn_http_format_2_clicked()
{
    SliderPicture *s = new SliderPicture();
    s->show();
}

MainWindow::~MainWindow()
{
    LOG_DEBUG("退出程序时 -- 写入配置");
    writeConfig();

    if(m_db.isOpen())
    {
        m_db.close();
        ui->lab_status->setText(getColorString("red","Not connected!"));
        ui->pbtn_close->setEnabled(false);
        ui->pbtn_run->setEnabled(false);
        ui->pbtn_link->setEnabled(true);

        ui->lab_elapsed->setText("");

        QDateTime dt = QDateTime::currentDateTime();
        QString time_token = dt.toString("yy-MM-dd hh:mm:ss-zz");

        ui->tedit_log->insertPlainText("\n" + getDateTimeStamp() + " database close");

    }

    qDebug("~MainWindow");

    delete ui;
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    QFile config(this->m_config_filename);

    if(config.exists())
    {
        switch(index)
        {
        case 0:
            ui->tedit_sql->setText(m_oracle_sql);
            break;
        case 1:
            ui->tedit_sql->setText(m_sqlserver_sql);
            break;
        case 2:
            ui->tedit_sql->setText(m_mysql_sql);
            break;
        }
    }
    else
    {
        switch(index)
        {
        case 0:
            ui->tedit_sql->setText("select * from V_TJ_XDT where rownum < 100");
            break;
        case 1:
            ui->tedit_sql->setText("SELECT * FROM [SampleDB].[dbo].[TiJianH]");
            break;
        case 2:
            ui->tedit_sql->setText("SELECT * FROM users");
            break;
        }
    }
}

void MainWindow::s_open_options()
{
    qDebug("%s %s %d",__FILE__,__FUNCTION__,__LINE__);
    //确认密码: QMessageBox 输入窗口,   zoncare_V0.0.2   zoncare_V0.0.n
    Dialog_SQL *dia = new Dialog_SQL(this);
    dia->open();
}

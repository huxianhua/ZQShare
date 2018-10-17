#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "http_worklist.h"
#include <QMainWindow>
#include <QSqlDatabase>
#include <QCloseEvent>
#include <QSystemTrayIcon>

#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    static MainWindow* instance() {
        static MainWindow* instance = NULL;
           if(NULL == instance)
           {
               instance = new MainWindow;
           }
           return instance;
      }


    ~MainWindow();

    void readConfig();
    void writeConfig();

    void test_oracle();

    int test_open_oracle();

    void test_sqlserver_connect();

    QString getColorString(QString color,QString contents);
    QString getDateTimeStamp();


    QString get_select_json();
    QString get_json_demo();


    void createTrayIcon();
    void createActions();
protected:
    void timerEvent(QTimerEvent *);

private:
    Ui::MainWindow *ui;
    QSqlDatabase m_db;
    QString m_driverName;

    HTTP_WORKLIST *m_wl_server;

    QString m_app_version;
    QString m_config_filename;

    QString m_oracle_sql;
    QString m_sqlserver_sql;
    QString m_mysql_sql;

    QAction *m_action_Show;
    QAction *m_action_Hide;
    QAction *m_action_Restore;
    QAction *m_action_Quit;

    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayIconMenu;

    bool  m_isAutoStart;
    int   m_timeID;

    int  m_tabwidCurrentIndex;

    QThread *m_workThread;

    QString m_bufferJson;

    void run_oracle(QString sql);
    void run_mysql(QString sql);
    void run_sqlserver(QString sql);

    void setIPComboBox();

    void showHelp(QString htmlPath);

    void setAutoStart(bool is_auto_start);

signals:
    void sig_start(int port);
    void sig_stop();

    void sig_connect();

    void sig_start_stop_released();



private slots:

    void s_tabbarClicked(int index);
    void s_currentChanged(int index);

    void s_pbtn_link();
    void s_pbtn_close();


    void s_pbtn_http_start_stop();
    void s_open_url();

    void s_profile_help();
    void s_http_format();

    void s_pbtn_run();

    void s_pbtn_tables();
    void s_pbtn_views();
    void s_pbtn_default();


    void s_pbtn_tables_mysql();
    void s_pbtn_views_mysql();
    void s_pbtn_default_mysql();

    void s_pbtn_tables_sqlserver();
    void s_pbtn_views_sqlserver();
    void s_pbtn_default_sqlserver();

    void s_sql_textChanged();


    void s_writeConfig();
    //

    void s_auto_run_currentIndexChanged(int index);

    void s_trayIcon_activated(QSystemTrayIcon::ActivationReason);

    void s_singleApp_ReceiveMessage(QString message);

    void on_pbtn_http_format_2_clicked();

    void on_tabWidget_currentChanged(int index);




    void s_open_options();


protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

};

#endif // MAINWINDOW_H

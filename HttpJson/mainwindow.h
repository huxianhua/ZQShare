#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "http_worklist.h"
#include "sql_record.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
   // void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;

    QSettings *m_settings;

    HTTP_WORKLIST *m_wl;
};

#endif // MAINWINDOW_H

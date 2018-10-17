#include "dialog_sql.h"
#include "ui_dialog_sql.h"

Dialog_SQL::Dialog_SQL(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_SQL)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)),ui->stackedWidget,SLOT(setCurrentIndex(int)));



}

Dialog_SQL::~Dialog_SQL()
{
    delete ui;
}

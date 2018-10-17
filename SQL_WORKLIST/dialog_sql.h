#ifndef DIALOG_SQL_H
#define DIALOG_SQL_H

#include <QDialog>

namespace Ui {
class Dialog_SQL;
}

class Dialog_SQL : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_SQL(QWidget *parent = 0);
    ~Dialog_SQL();

private:
    Ui::Dialog_SQL *ui;
};

#endif // DIALOG_SQL_H

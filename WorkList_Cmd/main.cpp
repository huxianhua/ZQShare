#include <QCoreApplication>
#include <QFile>

#include "config.h"
#include "sql_record.h"


#include <QFile>
#include <QString>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if(argc >= 2)
    {
        if(QFile(argv[1]).exists())
        {
             //设置配置文件
            SQL_RECORD *sql_recode = SQL_RECORD::instance(argv[1]);
            //查询
            QString json = sql_recode->runSQL();
            //结果存入文件中
            sql_recode->saveResult(argv[2]);
            //关闭数据
            sql_recode->removeDatabase();
            return 0;
        }else
        {
            LOG_DEBUG("配置文件不存在, (%s)",argv[1]);
            return -1;
        }
    }else
    {
        qDebug("程序格式: %s config.ini out.json",argv[0]);
        return -1;
    }


    return a.exec();
}



#ifndef CONFIG_H
#define CONFIG_H

#include <QTime>
#include <QDate>
#include <QDateTime>



#define LOG_FFL qDebug("[%s - %s - %d]",__FILE__,__FUNCTION__,__LINE__)
#define LOG_FATAL_FFL qDebug("[%s - %s - %d]",__FILE__,__FUNCTION__,__LINE__)

#define LOG_DEBUG(format,...)    qDebug("[%s - %s - %d]\t"#format,__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define LOG_FATAL(format,...)    qFatal("[%s - %s - %d]\t"#format,__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)

#define COMPANY  "zoncare"
#define VERSION  "V0.0.3"


#define PASSWORD QString("%1_%2").arg(COMPANY).arg(VERSION)





#define TIMEMS qPrintable (QTime::currentTime().toString("HH:mm:ss zzz"))
#define TIME qPrintable (QTime::currentTime().toString("HH:mm:ss"))
#define QDATE qPrintable (QDate::currentDate().toString("yyyy-MM-dd"))
#define QTIME qPrintable (QTime::currentTime().toString("HH-mm-ss"))
#define DATETIME qPrintable (QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
#define STRDATETIME qPrintable (QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))
#define STRDATETIMEMS qPrintable (QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss-zzz"))



#endif // CONFIG_H


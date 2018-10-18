#ifndef CONFIG_H
#define CONFIG_H


#define LOG_FFL qDebug("[%s - %s - %d]",__FILE__,__FUNCTION__,__LINE__)
#define LOG_FATAL_FFL qDebug("[%s - %s - %d]",__FILE__,__FUNCTION__,__LINE__)

#define LOG_DEBUG(format,...)    qDebug("[%s - %s - %d]\t"#format,__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define LOG_FATAL(format,...)    qFatal("[%s - %s - %d]\t"#format,__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)

#define COMPANY  "zoncare"
#define VERSION  "V1.0"


#define SETTING_LOCK         false
#define PASSWORD            QString("%1_%2").arg(COMPANY).arg(VERSION)

#if 1
#define SQLWORKCOFIG        "./config/mysql.ini"
#else
//#define SQLWORKCOFIG        "/Users/huxianhua/develop/git/ZQShare/SQL_WORKLIST/config/mysql.ini"
#define SQLWORKCOFIG          "D:/Git/ZQShare/SQL_WORKLIST/config/mysql.ini"
#endif



#endif // CONFIG_H


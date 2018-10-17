#ifndef CONFIG_H
#define CONFIG_H


#define LOG_FFL qDebug("[%s - %s - %d]",__FILE__,__FUNCTION__,__LINE__)
#define LOG_FATAL_FFL qDebug("[%s - %s - %d]",__FILE__,__FUNCTION__,__LINE__)

#define LOG_DEBUG(format,...)    qDebug("[%s - %s - %d]\t"#format,__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)
#define LOG_FATAL(format,...)    qFatal("[%s - %s - %d]\t"#format,__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__)

#define COMPANY  "zoncare"
#define VERSION  "V0.0.3"


#define PASSWORD QString("%1_%2").arg(COMPANY).arg(VERSION)

#endif // CONFIG_H


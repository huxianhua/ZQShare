#-------------------------------------------------
#
# Project created by QtCreator 2017-12-21T14:28:04
#
#-------------------------------------------------

QT       += core gui network sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SQL_WORKLIST_V1.0
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sql_record.cpp \
    http_worklist.cpp \
    sliderpicture.cpp \
    dialog_sql.cpp \
    sql_work.cpp

HEADERS  += mainwindow.h \
    sql_record.h \
    http_worklist.h \
    sliderpicture.h \
    dialog_sql.h \
    config.h \
    sql_work.h

FORMS    += mainwindow.ui \
    dialog_sql.ui


include(./HttpServer/lib/libQt5HttpServer.pri)
include(./HttpServer/static-files/static-files.pri)
include(./HttpServer/request-parsing/request-parsing.pri)
include(./QSingleApplication/QSingleApplication.pri)

INCLUDEPATH += ./QSingleApplication
DEPENDPATH += ./QSingleApplication

RESOURCES     = resource.qrc

win32:RC_FILE = resources.rc

#LIBS += -LC:\qt\instantclient_12_2
#LIBS += -LD:\qtcode\build-Hello_QT-Desktop_Qt_5_7_0_MSVC2015_32bit-Debug\debug\sqldrivers

DISTFILES += \
    config/config.ini \
    config/mysql.ini \
    config/oracle.ini \
    config/sqlserver.ini

#-------------------------------------------------
#
# Project created by QtCreator 2017-12-21T14:28:04
#
#-------------------------------------------------

QT       += core gui network sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WorkList-Json
TEMPLATE = app


SOURCES += main.cpp\
    sql_record.cpp \
    http_worklist.cpp \
    mainwindow.cpp

HEADERS  += \
    sql_record.h \
    http_worklist.h \
    config.h \
    mainwindow.h

FORMS    += \
    mainwindow.ui


include(./HttpServer/lib/libQt5HttpServer.pri)
include(./HttpServer/static-files/static-files.pri)
include(./HttpServer/request-parsing/request-parsing.pri)
include(./QSingleApplication/QSingleApplication.pri)

INCLUDEPATH += ./QSingleApplication
DEPENDPATH += ./QSingleApplication

RESOURCES     = resource.qrc

win32:RC_FILE = resources.rc

DISTFILES += \
    HttpJson.ini

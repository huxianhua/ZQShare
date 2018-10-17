#-------------------------------------------------
#
# Project created by QtCreator 2018-03-29T14:30:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HttpServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

include(./lib/libQt5HttpServer.pri)

include(./static-files/static-files.pri)

include(./request-parsing/request-parsing.pri)



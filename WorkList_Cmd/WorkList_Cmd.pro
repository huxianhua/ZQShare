#-------------------------------------------------
#
# Project created by QtCreator 2018-10-17T07:09:58
#
#-------------------------------------------------

QT       += core network sql

QT       -= gui

TARGET = WorkList_Cmd
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp config.h \
    sql_record.cpp

HEADERS += \
    sql_record.h

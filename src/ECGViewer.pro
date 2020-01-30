#-------------------------------------------------
#
# Project created by QtCreator 2019-12-24T23:13:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ECGViewer
TEMPLATE = app


SOURCES += main.cpp\
        EDFlib/edflib.c \
        graphicareawidget.cpp \
        mainwindow.cpp

HEADERS  += mainwindow.h \
    EDFlib/edflib.h \
    graphicareawidget.h

FORMS    += mainwindow.ui

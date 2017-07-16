#-------------------------------------------------
#
# Project created by QtCreator 2017-01-20T14:47:03
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AudioPlayer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
    wavbuffer.cpp \
    signalplot.cpp

HEADERS  += mainwindow.h \
    wavbuffer.h \
    signalplot.h

RESOURCES += application.qrc

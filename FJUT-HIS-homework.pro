#-------------------------------------------------
#
# Project created by QtCreator 2018-10-03T10:50:04
#
#-------------------------------------------------

QT       += core gui sql
QT       += network
QT       += charts
FORMS += login.ui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FJUT-HIS-homework
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    login.cpp \
        main.cpp \
        mainwindow.cpp \
    employeeinfo.cpp \
    employeedbutils.cpp \
    techused.cpp \
    about.cpp

HEADERS += \
    login.h \
        mainwindow.h \
    employeeinfo.h \
    techused.h \
    about.h

FORMS += \
    login.ui \
        mainwindow.ui \
    employeeinfo.ui \
    techused.ui \
    about.ui

RESOURCES += \
    resources.qrc

DISTFILES +=

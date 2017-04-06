#-------------------------------------------------
#
# Project created by QtCreator 2016-08-29T13:07:48
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = DAQ_GUI
TEMPLATE = app

#have to use winNT because the compiler WinGW is 32
win32: LIBS += "C:/Program Files (x86)/IVI Foundation/VISA/WinNT/lib/msc/visa32.lib"

win32:RC_ICONS += icon.ico

SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    wavefromgenerator.cpp \
    oscilloscope.cpp \
    wfg_dialog.cpp \
    osc_dialog.cpp \
    qscpi.cpp \
    qfileio.cpp \
    dmm.cpp \
    analysis.cpp \
    matrix.cpp \
    constant.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    wavefromgenerator.h \
    constant.h \
    oscilloscope.h \
    wfg_dialog.h \
    osc_dialog.h \
    qscpi.h \
    qfileio.h \
    exception.h \
    dmm.h \
    analysis.h \
    matrix.h


FORMS    += mainwindow.ui \
    wfg_dialog.ui \
    osc_dialog.ui

DISTFILES += \
    icon.ico

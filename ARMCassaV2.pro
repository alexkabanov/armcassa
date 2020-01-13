#-------------------------------------------------
#
# Project created by QtCreator 2015-11-28T19:40:28
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -lm -lqaxcontainer

TARGET = ARMCassaV2
TEMPLATE = app


SOURCES += \
    countdialog.cpp \
    keypresseater.cpp \
    main.cpp \
    mainsales.cpp \
    productdialog.cpp \
    readportthread.cpp \
    saleitemtableview.cpp \
    selectproduct.cpp \
    settingsdialog.cpp \
    kkm.cpp

HEADERS  += \
    countdialog.h \
    keypresseater.h \
    mainsales.h \
    productdialog.h \
    readportthread.h \
    saleitemtableview.h \
    selectproduct.h \
    settingsdialog.h \
    kkm.h

FORMS    += \
    countdialog.ui \
    mainsales.ui \
    productdialog.ui \
    selectproduct.ui \
    settingsdialog.ui

DISTFILES += \
    2312.txt

# -------------------------------------------------
# Project created by QtCreator 2010-04-01T04:36:37
# -------------------------------------------------
QT += core \
    dbus
QT -= gui
TARGET = qconnman
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    qconnmanservice_linux.cpp \
    qconnman.cpp
HEADERS += qconnmanservice_linux_p.h \
    qconnman.h

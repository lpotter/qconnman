# -------------------------------------------------
# Project created by QtCreator 2010-04-01T04:36:37
# -------------------------------------------------
QT += core dbus gui
TARGET = qconnman
CONFIG += console
TEMPLATE = app
SOURCES += main.cpp \
    qconnmanservice_linux.cpp \
    qofonoservice_linux.cpp \
    qconnman.cpp \
    window.cpp \
    newmessage.cpp \
    qsysteminfodbushelper.cpp \
    agentadaptor.cpp

HEADERS += qconnmanservice_linux_p.h \
    qofonoservice_linux_p.h \
    qconnman.h \
    window.h \
    newmessage.h \
    qsysteminfodbushelper_p.h \
    agentadaptor.h

RESOURCES = systray.qrc
QT += xml svg

FORMS += \
    wizard.ui \
    form.ui \
    sendSmsDialog.ui \
    smsmessagebox.ui
MOC_DIR=.moc
OBJECTS_DIR=.obj


# -------------------------------------------------
# Project created by QtCreator 2010-04-01T04:36:37
# -------------------------------------------------
QT += core dbus gui widgets
TARGET = qconnman
CONFIG += console
TEMPLATE = app

LIBS += -lconnman-qt5
INCLUDEPATH += /usr/include/connman-qt5

SOURCES += main.cpp \
    qofonoservice_linux.cpp \
    qconnman.cpp \
    window.cpp \
    newmessage.cpp \
    qsysteminfodbushelper.cpp \
    messagebox.cpp

HEADERS += \
    qofonoservice_linux_p.h \
    qconnman.h \
    window.h \
    newmessage.h \
    qsysteminfodbushelper_p.h \
    messagebox.h

RESOURCES = systray.qrc
QT += xml svg

FORMS += \
    wizard.ui \
    form.ui \
    sendSmsDialog.ui \
    smsmessagebox.ui

MOC_DIR=.moc
OBJECTS_DIR=.obj


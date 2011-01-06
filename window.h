/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef WINDOW_H
#define WINDOW_H

#include <QtGui/QSystemTrayIcon>
#include <QtGui/QDialog>
#include "qconnmanservice_linux_p.h"
#include "qofonoservice_linux_p.h"

#include "ui_form.h"
//#include "ui_trayapp.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
QT_END_NAMESPACE

class QMenu;
class Window : public QSystemTrayIcon
{
    Q_OBJECT

public:
    Window();
    ~Window();

//    void setVisible(bool visible);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void messageClicked();
    void showWifi();
    void menuTriggered(QAction *action);

    void connManPropertyChanged(const QString &str, const QDBusVariant &var);
    void connmanPropertyChangedContext(const QString &,const QString &, const QDBusVariant &);

    void techPropertyChanged(const QString &str, const QDBusVariant &var);

    void itemClicked(QTreeWidgetItem*,int);
    void subItemClicked(QTreeWidgetItem*,int);

    void updateTree();

    void networkPropertyChanged(const QString &, const QDBusVariant &);
    void networkPropertyChangedContext(const QString &,const QString &, const QDBusVariant &);

    void servicesPropertyChanged(const QString &, const QDBusVariant &);
    void servicesPropertyChangedContext(const QString &,const QString &, const QDBusVariant &);

    void technologyPropertyChanged(const QString &, const QDBusVariant &);
    void technologyPropertyChangedContext(const QString &,const QString &, const QDBusVariant &);

    void connmanStateChanged(const QString &);


    void ofonoPropertyChangedContext(const QString &path,const QString &item, const QDBusVariant &value);
    void ofonoNetworkPropertyChangedContext(const QString &path,const QString &item, const QDBusVariant &value);
    void ofonoModemPropertyChangedContext(const QString &path,const QString &item, const QDBusVariant &value);

    void immediateMessage(const QString &msg,const QVariantMap &map);
    void incomingMessage(const QString &msg,const QVariantMap &map);

    void cellItemClicked(QTreeWidgetItem*,int);
    void sendMessage();


    void contextMenu(const QPoint &);
    void appletContextMenu(const QPoint &);

    void deviceContextMenuClicked();

    void connectService();
    void disconnectService();
    void removeService();
    void scan();
    void sendMessage(const QString &address);

private:
    void createActions();
    void createTrayIcon();
    void doTrigger();

    QConnmanManagerInterface *connman;
    QList<QConnmanTechnologyInterface *> connmanTech;
    QList<QConnmanServiceInterface *> connmanServices;
    QList<QConnmanServiceInterface *> connmanNetworks;

    QConnmanServiceInterface *wifiService;

    QAction *quitAction;
    QAction *wifiAction;

    QMenu *trayIconMenu;
    QWidget *trayWidget;
    Ui_qconnmanApp *mw;
    void connectToService(const QString &service);

    QStringList knownModems;
    QOfonoManagerInterface *ofonoManager;

};

class QConnectThread : public QThread
{
    Q_OBJECT

public:
    QConnectThread(QObject *parent = 0);
    ~QConnectThread();
    bool keepRunning;
    void quit();
    QString service;

Q_SIGNALS:
    void connected(const QString &path);
    void disconnected();

protected:
    void run();

private:
    QMutex mutex;

private Q_SLOTS:
};

#endif

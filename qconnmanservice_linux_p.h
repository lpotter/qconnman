/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef QCONNMANSERVICE_H
#define QCONNMANSERVICE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtDBus/QtDBus>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusReply>

#include <QtDBus/QDBusPendingCallWatcher>
#include <QtDBus/QDBusObjectPath>
#include <QtDBus/QDBusContext>
#include <QMap>

//#include "qnmdbushelper.h"

#include <connman/dbus.h>
#include <connman/device.h>

#ifndef __CONNMAN_DBUS_H

#define	CONNMAN_SERVICE     "org.moblin.connman"
#define	CONNMAN_PATH        "/org/moblin/connman"

#define CONNMAN_DEBUG_INTERFACE		CONNMAN_SERVICE ".Debug"
#define CONNMAN_ERROR_INTERFACE		CONNMAN_SERVICE ".Error"
#define CONNMAN_AGENT_INTERFACE		CONNMAN_SERVICE ".Agent"
#define CONNMAN_COUNTER_INTERFACE	CONNMAN_SERVICE ".Counter"

#define CONNMAN_MANAGER_INTERFACE	CONNMAN_SERVICE ".Manager"
#define CONNMAN_MANAGER_PATH		"/"

#define CONNMAN_TASK_INTERFACE		CONNMAN_SERVICE ".Task"
#define CONNMAN_PROFILE_INTERFACE	CONNMAN_SERVICE ".Profile"
#define CONNMAN_SERVICE_INTERFACE	CONNMAN_SERVICE ".Service"
#define CONNMAN_DEVICE_INTERFACE	CONNMAN_SERVICE ".Device"
#define CONNMAN_NETWORK_INTERFACE	CONNMAN_SERVICE ".Network"
#define CONNMAN_PROVIDER_INTERFACE	CONNMAN_SERVICE ".Provider"
#define CONNMAN_TECHNOLOGY_INTERFACE	CONNMAN_SERVICE ".Technology"
#endif

#ifndef __CONNMAN_DEVICE_H
enum connman_device_type {
	CONNMAN_DEVICE_TYPE_UNKNOWN   = 0,
	CONNMAN_DEVICE_TYPE_ETHERNET  = 1,
	CONNMAN_DEVICE_TYPE_WIFI      = 2,
	CONNMAN_DEVICE_TYPE_WIMAX     = 3,
	CONNMAN_DEVICE_TYPE_BLUETOOTH = 4,
	CONNMAN_DEVICE_TYPE_CELLULAR  = 5,
	CONNMAN_DEVICE_TYPE_GPS       = 6,
	CONNMAN_DEVICE_TYPE_VENDOR    = 10000,
} connamn_device;

enum connman_device_mode {
	CONNMAN_DEVICE_MODE_UNKNOWN          = 0,
	CONNMAN_DEVICE_MODE_NETWORK_SINGLE   = 1,
	CONNMAN_DEVICE_MODE_NETWORK_MULTIPLE = 2,
};
#endif

QT_BEGIN_NAMESPACE

typedef QMap< QString, QVariant > QConnmanMap;
typedef QMap<QString,QString>  QConnmanStringMap;

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QT_PREPEND_NAMESPACE(QConnmanMap))
Q_DECLARE_METATYPE(QT_PREPEND_NAMESPACE(QConnmanStringMap))


QT_BEGIN_NAMESPACE
//QDBusAbstractInterface

class QConnmanInterface : public  QDBusAbstractInterface
{
    Q_OBJECT

public:

    QConnmanInterface( QObject *parent = 0);
    ~QConnmanInterface();

     QDBusObjectPath path() const;

    QConnmanMap getProperties();
    bool setProperty(const QString &name, const QDBusVariant &value);
    QDBusObjectPath createProfile(const QString &name);
    bool removeProfile(QDBusObjectPath path);
    bool requestScan(const QString &type);
    bool enableTechnology(const QString &type);
    bool disableTechnology(const QString &type);
    QDBusObjectPath connectService(QConnmanMap &map);
    void registerAgent(QDBusObjectPath &path);
    void unregisterAgent(QDBusObjectPath path);
    void registerCounter(QDBusObjectPath path, quint32 interval);
    void unregisterCounter(QDBusObjectPath path);

    QString requestSession(const QString &bearerName);
    void releaseSession();
    
      // properties
    QString getState();
    QStringList getAvailableTechnologies();
    QStringList getEnabledTechnologies();
    QStringList getConnectedTechnologies();
    QString getDefaultTechnology();
    bool getOfflineMode();
    QString getActiveProfile();
    QStringList getProfiles();
    QStringList  getTechnologies();
    QStringList getServices();

    QString getPathForTechnology(const QString &tech);


Q_SIGNALS:
    void propertyChanged(const QString &, const QDBusVariant &value);
    void stateChanged(const QString &);

private Q_SLOTS:
private:
    //QConnmanInterfacePrivate *d;
//    QNmDBusHelper *nmDBusHelper;

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);
    QVariant getProperty(const QString &);
};


class QConnmanNetworkInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:

    QConnmanNetworkInterface(const QString &dbusPathName, QObject *parent = 0);
    ~QConnmanNetworkInterface();

    QConnmanMap getProperties();

    //properties
    QString getName();
    bool isConnected();
    quint8 getSignalStrength();
    QString getDevice();
    QString getWifiSsid();
    QString getWifiMode();
    QString getWifiSecurity();
    QString getWifiPassphrase();

Q_SIGNALS:
    void propertyChanged(const QString &, const QDBusVariant &value);
private:
    //QConnmanNetworkInterfacePrivate *d;
protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);
    QVariant getProperty(const QString &);
};

class QConnmanProfileInterfacePrivate;
class QConnmanProfileInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:

    QConnmanProfileInterface(const QString &dbusPathName,QObject *parent = 0);
    ~QConnmanProfileInterface();

    QConnmanMap getProperties();
// properties
    QString getName();
    bool isOfflineMode();
    QStringList getServices();

Q_SIGNALS:
    void propertyChanged(const QString &, const QDBusVariant &value);
private:
    QConnmanProfileInterfacePrivate *d;

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);
    QVariant getProperty(const QString &);
};

class QConnmanServiceInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:

    QConnmanServiceInterface(const QString &dbusPathName,QObject *parent = 0);
    ~QConnmanServiceInterface();

    QConnmanMap getProperties();
      // setProperty
      // clearProperty
    void connect();
    void disconnect();
    void remove();
      // void moveBefore(QDBusObjectPath &service);
      // void moveAfter(QDBusObjectPath &service);

// properties
    QString getState();
    QString getError();
    QString getName();
    QString getType();
    QString getMode();
    QString getSecurity();
    QString getPassphrase();
    bool isPassphraseRequired();
    quint8 getSignalStrength();
    bool isFavorite();
    bool isImmutable();
    bool isAutoConnect();
    bool isSetupRequired();
    QString getAPN();
    QString getMCC();
    QString getMNC();
    bool isRoaming();
    QStringList getNameservers();
    QStringList getDomains();
    QConnmanStringMap getIPv4();
    QConnmanStringMap getIPv4Configuration();
    QConnmanStringMap getProxy();
    QConnmanStringMap getEthernet();

    bool isOfflineMode();
    QStringList getServices();

Q_SIGNALS:
    void propertyChanged(const QString &, const QDBusVariant &value);

private:
//    QConnmanServiceInterfacePrivate *d;
protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);
    QVariant getProperty(const QString &);
};

class QConnmanTechnologyInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:

    QConnmanTechnologyInterface(const QString &dbusPathName,QObject *parent = 0);
    ~QConnmanTechnologyInterface();

    QConnmanMap getProperties();
// properties
    QString getState();
    QString getName();
    QString getType();

    QStringList getDevices();

Q_SIGNALS:
    void propertyChanged(const QString &, const QDBusVariant &value);
private:
protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);
    QVariant getProperty(const QString &);

};

class QConnmanAgentInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:

    QConnmanAgentInterface(const QString &dbusPathName,QObject *parent = 0);
    ~QConnmanAgentInterface();

    void release();
    void reportError(QDBusObjectPath &path, const QString &error);
//    dict requestInput(QDBusObjectPath &path, dict fields);
    void cancel();
private:
  //  QConnmanAgentInterfacePrivate *d;
protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);
};

//class QConnmanCounterInterfacePrivate;
//class QConnmanCounterInterface : public QDBusAbstractInterface
//{
//    Q_OBJECT
//
//public:
//
//    QConnmanCounterInterface(QObject *parent = 0);
//    ~QConnmanCounterInterface();
//
//    void release();
//private:
//    QConnmanCounterInterfacePrivate *d;
//};

class QConnmanDeviceInterface : public QDBusAbstractInterface
{
    Q_OBJECT

public:

    QConnmanDeviceInterface(const QString &dbusPathName,QObject *parent = 0);
    ~QConnmanDeviceInterface();

    QConnmanMap getProperties();

//properties
    QString getAddress();
    QString getName();
    QString getType();
    QString getInterface();
    bool isPowered();
    quint16 getScanInterval();
    bool isScanning();
    QStringList getNetworks();
    bool setEnabled(bool powered);

Q_SIGNALS:
    void propertyChanged(const QString &, const QDBusVariant &value);
private:
protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);
    QVariant getProperty(const QString &);

};


QT_END_NAMESPACE

#endif //QCONNMANSERVICE_H

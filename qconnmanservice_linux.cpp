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

#include <QObject>
#include <QList>
#include <QtDBus/QtDBus>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusPendingCallWatcher>
#include <QtDBus/QDBusObjectPath>
#include <QtDBus/QDBusPendingCall>

#include "qconnmanservice_linux_p.h"
//#include "qnmdbushelper.h"

QT_BEGIN_NAMESPACE
static QDBusConnection dbusConnection = QDBusConnection::systemBus();


QConnmanInterface::QConnmanInterface( QObject *parent)
        : QDBusAbstractInterface(QLatin1String(CONNMAN_SERVICE),
                                 QLatin1String(CONNMAN_MANAGER_PATH),
                                 CONNMAN_MANAGER_INTERFACE,
                                 QDBusConnection::systemBus(), parent)
{
}

QConnmanInterface::~QConnmanInterface()
{
}

void QConnmanInterface::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QVariant))) {
        dbusConnection.connect(QLatin1String(CONNMAN_SERVICE),
                               QLatin1String(CONNMAN_MANAGER_PATH),
                               QLatin1String(CONNMAN_MANAGER_INTERFACE),
                               QLatin1String("PropertiesChanged"),
                               this,SIGNAL(propertyChanged(const QString &, QVariant &)));
    }

    if (QLatin1String(signal) == SIGNAL(stateChanged(QString))) {
        if (!dbusConnection.connect(QLatin1String(CONNMAN_SERVICE),
                                    QLatin1String(CONNMAN_MANAGER_PATH),
                                    QLatin1String(CONNMAN_MANAGER_INTERFACE),
                                    QLatin1String("StateChanged"),
                                    this,SIGNAL(stateChanged(const QString &)))) {

        }
    }
}

void QConnmanInterface::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString, QVariant))) {

    }
}

QVariant QConnmanInterface::getProperty(const QString &property)
{
    QVariant var;
    QConnmanMap map = getProperties();
    if (map.contains(property)) {
        var = map.value(property);
    }
    return var;
}

QConnmanMap QConnmanInterface::getProperties()
{
    QDBusReply<QConnmanMap > reply =  this->call(QLatin1String("GetProperties"));
    return reply.value();
}

QString QConnmanInterface::getState()
{
    QDBusReply<QString > reply =  this->call("GetState");
    return reply.value();
}

bool QConnmanInterface::setProperty(const QString &name, QVariant value)
{
}

QDBusObjectPath QConnmanInterface::createProfile(const QString &name)
{
}

bool QConnmanInterface::removeProfile(QDBusObjectPath path)
{
}

bool QConnmanInterface::requestScan(const QString &type)
{
    QDBusReply<QString> reply =  this->call(QLatin1String("RequestScan"), QVariant::fromValue(type));

    bool ok = true;
    if(reply.error().type() == QDBusError::InvalidArgs) {
        qWarning() << reply.error().message();
        ok = false;
    }
    return ok;
}

bool QConnmanInterface::enableTechnology(const QString &type)
{
    QDBusReply<QList<QDBusObjectPath> > reply =  this->call(QLatin1String("EnableTechnology"), QVariant::fromValue(type));
    bool ok = true;
    if(reply.error().type() == QDBusError::InvalidArgs) {
        qWarning() << reply.error().message();
        ok = false;
    }
    return ok;
}

bool QConnmanInterface::disableTechnology(const QString &type)
{
    QDBusReply<QList<QDBusObjectPath> > reply =  this->call(QLatin1String("DisableTechnology"), QVariant::fromValue(type));
    bool ok = true;
    if(reply.error().type() == QDBusError::InvalidArgs) {
        qWarning() << reply.error().message();
        ok = false;
    }
    return ok;
}

QDBusObjectPath QConnmanInterface::connectService(QConnmanMap &map)
{
    QDBusReply<QList<QDBusObjectPath> > reply =  this->call(QLatin1String("ConnectService"), QVariant::fromValue(map));
}

void QConnmanInterface::registerAgent(QDBusObjectPath &path)
{
}

void QConnmanInterface::unregisterAgent(QDBusObjectPath path)
{
}

void QConnmanInterface::registerCounter(QDBusObjectPath path, quint32 interval)
{
}

void QConnmanInterface::unregisterCounter(QDBusObjectPath path)
{
}

QString QConnmanInterface::requestSession(const QString &bearerName)
{
    QDBusReply<QList<QDBusObjectPath> > reply =  this->call(QLatin1String("RequestSession"), QVariant::fromValue(bearerName));
}

void  QConnmanInterface::releaseSession()
{
    QDBusReply<QList<QDBusObjectPath> > reply =  this->call(QLatin1String("ReleaseSession"));

}

// properties

QStringList QConnmanInterface::getAvailableTechnologies()
{
    QVariant var = getProperty("AvailableTechnologies");
    return qdbus_cast<QStringList>(var);
}

QStringList QConnmanInterface::getEnabledTechnologies()
{
    QVariant var = getProperty("EnabledTechnologies");
    return qdbus_cast<QStringList>(var);
}

QStringList QConnmanInterface::getConnectedTechnologies()
{
    QVariant var = getProperty("ConnectedTechnologies");
    return qdbus_cast<QStringList>(var);
}

QString QConnmanInterface::getDefaultTechnology()
{
    QVariant var = getProperty("DefaultTechnology");
    return qdbus_cast<QString>(var);
}

bool QConnmanInterface::getOfflineMode()
{
    QVariant var = getProperty("OfflineMode");
    return qdbus_cast<bool>(var);
}

QString QConnmanInterface::getActiveProfile()
{
    QVariant var = getProperty("ActiveProfile");
    return qdbus_cast<QString>(var);
}

QStringList QConnmanInterface::getProfiles()
{
    QVariant var = getProperty("Profiles");
    return qdbus_cast<QStringList>(var);
}

QStringList QConnmanInterface::getTechnologies()
{
    QVariant var = getProperty("Technologies");
    return qdbus_cast<QStringList >(var);

}

QStringList QConnmanInterface::getServices()
{
    QVariant var = getProperty("Services");
    return qdbus_cast<QStringList >(var);

}

QString QConnmanInterface::getPathForTechnology(const QString &name)
{
    foreach(const QString path, getTechnologies()) {
        if(path.contains(name)) {
            return path;
        }
    }
    return "";
}


/////////////
QConnmanNetworkInterface::QConnmanNetworkInterface(const QString &dbusPathName, QObject *parent)
    : QDBusAbstractInterface(QLatin1String(CONNMAN_SERVICE),
                             dbusPathName,
                             CONNMAN_NETWORK_INTERFACE,
                             QDBusConnection::systemBus(), parent)
{
}

QConnmanNetworkInterface::~QConnmanNetworkInterface()
{
}

void QConnmanNetworkInterface::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QVariant))) {
        dbusConnection.connect(QLatin1String(CONNMAN_SERVICE),
                               this->path(),
                               QLatin1String(CONNMAN_NETWORK_INTERFACE),
                               QLatin1String("PropertiesChanged"),
                               this,SIGNAL(propertyChanged(const QString &, QVariant &)));
    }
}

void QConnmanNetworkInterface::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString, QVariant))) {

    }
}

QConnmanMap QConnmanNetworkInterface::getProperties()
{
    QDBusReply<QConnmanMap > reply =  this->call(QLatin1String("GetProperties"));
    return reply.value();
}

QVariant QConnmanNetworkInterface::getProperty(const QString &property)
{
    QVariant var;
    QConnmanMap map = getProperties();
    if (map.contains(property)) {
        var = map.value(property);
    }
    return var;
}
//properties

QString QConnmanNetworkInterface::getName()
{
    QVariant var = getProperty("Name");
    return qdbus_cast<QString>(var);
}

bool QConnmanNetworkInterface::isConnected()
{
    QVariant var = getProperty("Connected");
    return qdbus_cast<bool>(var);
}

quint8 QConnmanNetworkInterface::getSignalStrength()
{
    QVariant var = getProperty("Strength");
    return qdbus_cast<quint8>(var);
}

QString QConnmanNetworkInterface::getDevice()
{
    QVariant var = getProperty("Device");
    return qdbus_cast<QString>(var);

}

QString QConnmanNetworkInterface::getWifiSsid()
{
    QVariant var = getProperty("WiFi.SSID");
    return qdbus_cast<QString>(var);
}

QString QConnmanNetworkInterface::getWifiMode()
{
    QVariant var = getProperty("WiFi.Mode");
    return qdbus_cast<QString>(var);
}

QString QConnmanNetworkInterface::getWifiSecurity()
{
    QVariant var = getProperty("WiFi.Security");
    return qdbus_cast<QString>(var);
}

QString QConnmanNetworkInterface::getWifiPassphrase()
{
    QVariant var = getProperty("WiFi.Passphrase");
    return qdbus_cast<QString>(var);
}


//////////////////////////

QConnmanProfileInterface::QConnmanProfileInterface(const QString &dbusPathName,QObject *parent)
    : QDBusAbstractInterface(QLatin1String(CONNMAN_SERVICE),
                             dbusPathName,
                             CONNMAN_NETWORK_INTERFACE,
                             QDBusConnection::systemBus(), parent)
{
}

QConnmanProfileInterface::~QConnmanProfileInterface()
{
}

void QConnmanProfileInterface::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QVariant))) {
        dbusConnection.connect(QLatin1String(CONNMAN_SERVICE),
                               this->path(),
                               QLatin1String(CONNMAN_PROFILE_INTERFACE),
                               QLatin1String("PropertiesChanged"),
                               this,SIGNAL(propertyChanged(const QString &, QVariant &)));
    }
}

void QConnmanProfileInterface::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString, QVariant))) {

    }
}

QConnmanMap QConnmanProfileInterface::getProperties()
{
    QDBusReply<QConnmanMap > reply =  this->call(QLatin1String("GetProperties"));
    return reply.value();
}

QVariant QConnmanProfileInterface::getProperty(const QString &property)
{
    QVariant var;
    QConnmanMap map = getProperties();
    if (map.contains(property)) {
        var = map.value(property);
    }
    return var;
}

// properties
QString QConnmanProfileInterface::getName()
{

    QVariant var = getProperty("Name");
    return qdbus_cast<QString>(var);
}

bool QConnmanProfileInterface::isOfflineMode()
{
    QVariant var = getProperty("OfflineMode");
    return qdbus_cast<bool>(var);
}

QStringList QConnmanProfileInterface::getServices()
{
    QVariant var = getProperty("Services");
    return qdbus_cast<QStringList>(var);
}

///////////////////////////
QConnmanServiceInterface::QConnmanServiceInterface(const QString &dbusPathName,QObject *parent)
    : QDBusAbstractInterface(QLatin1String(CONNMAN_SERVICE),
                             dbusPathName,
                             CONNMAN_SERVICE_INTERFACE,
                             QDBusConnection::systemBus(), parent)
{
}

QConnmanServiceInterface::~QConnmanServiceInterface()
{
}

void QConnmanServiceInterface::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QVariant))) {
        dbusConnection.connect(QLatin1String(CONNMAN_SERVICE),
                               this->path(),
                               QLatin1String(CONNMAN_SERVICE_INTERFACE),
                               QLatin1String("PropertiesChanged"),
                               this,SIGNAL(propertyChanged(const QString &, QVariant &)));
    }
}

void QConnmanServiceInterface::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QVariant))) {

    }
}

QConnmanMap QConnmanServiceInterface::getProperties()
{
    QDBusReply<QConnmanMap> reply =  this->call(QLatin1String("GetProperties"));
    return reply.value();
}

QVariant QConnmanServiceInterface::getProperty(const QString &property)
{
    QVariant var;
    QConnmanMap map = getProperties();
    if (map.contains(property)) {
        var = map.value(property);
    }
    return var;
}

// setProperty
// clearProperty
void QConnmanServiceInterface::connect()
{
    QDBusReply<QConnmanMap> reply =  this->call(QLatin1String("Connect"));
}

void QConnmanServiceInterface::disconnect()
{
    QDBusReply<QConnmanMap> reply =  this->call(QLatin1String("Disconnect"));
}

void QConnmanServiceInterface::remove()
{
    QDBusReply<QConnmanMap> reply =  this->call(QLatin1String("Remove"));
}

// void moveBefore(QDBusObjectPath &service);
// void moveAfter(QDBusObjectPath &service);

// properties
QString QConnmanServiceInterface::getState()
{
    QVariant var = getProperty("State");
    return qdbus_cast<QString>(var);
}

QString QConnmanServiceInterface::getError()
{
    QVariant var = getProperty("Error");
    return qdbus_cast<QString>(var);
}

QString QConnmanServiceInterface::getName()
{
    QVariant var = getProperty("Name");
    return qdbus_cast<QString>(var);
}

QString QConnmanServiceInterface::getType()
{
    QVariant var = getProperty("Type");
    return qdbus_cast<QString>(var);
}

QString QConnmanServiceInterface::getMode()
{
    QVariant var = getProperty("Mode");
    return qdbus_cast<QString>(var);
}

QString QConnmanServiceInterface::getSecurity()
{
    QVariant var = getProperty("Security");
    return qdbus_cast<QString>(var);
}

QString QConnmanServiceInterface::getPassphrase()
{
    QVariant var = getProperty("Passphrase");
    return qdbus_cast<QString>(var);
}

bool QConnmanServiceInterface::isPassphraseRequired()
{
    QVariant var = getProperty("PassphraseRequired");
    return qdbus_cast<bool>(var);
}

quint8 QConnmanServiceInterface::getSignalStrength()
{
    QVariant var = getProperty("Strength");
    return qdbus_cast<quint8>(var);
}

bool QConnmanServiceInterface::isFavorite()
{
    QVariant var = getProperty("Favorite");
    return qdbus_cast<bool>(var);
}

bool QConnmanServiceInterface::isImmutable()
{
    QVariant var = getProperty("Immutable");
    return qdbus_cast<bool>(var);
}

bool QConnmanServiceInterface::isAutoConnect()
{
    QVariant var = getProperty("AutoConnect");
    return qdbus_cast<bool>(var);
}

bool QConnmanServiceInterface::isSetupRequired()
{
    QVariant var = getProperty("SetupRequired");
    return qdbus_cast<bool>(var);
}

QString QConnmanServiceInterface::getAPN()
{
    QVariant var = getProperty("APN");
    return qdbus_cast<QString>(var);
}

QString QConnmanServiceInterface::getMCC()
{
    QVariant var = getProperty("MCC");
    return qdbus_cast<QString>(var);
}

QString QConnmanServiceInterface::getMNC()
{
    QVariant var = getProperty("MNC");
    return qdbus_cast<QString>(var);
}

bool QConnmanServiceInterface::isRoaming()
{
    QVariant var = getProperty("Roaming");
    return qdbus_cast<bool>(var);
}

QStringList QConnmanServiceInterface::getNameservers()
{
    QVariant var = getProperty("NameServers");
    return qdbus_cast<QStringList>(var);
}

QStringList QConnmanServiceInterface::getDomains()
{
    QVariant var = getProperty("Domains");
    return qdbus_cast<QStringList>(var);
}

QConnmanStringMap QConnmanServiceInterface::getIPv4()
{
    QVariant var = getProperty("Ipv4");
    return qdbus_cast<QConnmanStringMap >(var);
}

QConnmanStringMap QConnmanServiceInterface::getIPv4Configuration()
{
    QVariant var = getProperty("Ip4.Configuration");
    return qdbus_cast<QConnmanStringMap >(var);
}

QConnmanStringMap QConnmanServiceInterface::getProxy()
{
    QVariant var = getProperty("Proxy");
    return qdbus_cast<QConnmanStringMap >(var);
}

QConnmanStringMap QConnmanServiceInterface::getEthernet()
{
    QVariant var = getProperty("Ethernet");
    return qdbus_cast<QConnmanStringMap >(var);
}

bool QConnmanServiceInterface::isOfflineMode()
{
    QVariant var = getProperty("OfflineMode");
    return qdbus_cast<bool>(var);
}

QStringList QConnmanServiceInterface::getServices()
{
    QVariant var = getProperty("Services");
    return qdbus_cast<QStringList>(var);
}


//////////////////////////
QConnmanTechnologyInterface::QConnmanTechnologyInterface(const QString &dbusPathName,QObject *parent)
    : QDBusAbstractInterface(QLatin1String(CONNMAN_SERVICE),
                             dbusPathName,
                             CONNMAN_TECHNOLOGY_INTERFACE,
                             QDBusConnection::systemBus(), parent)
{
}

QConnmanTechnologyInterface::~QConnmanTechnologyInterface()
{
}

void QConnmanTechnologyInterface::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QVariant))) {
        dbusConnection.connect(QLatin1String(CONNMAN_SERVICE),
                               this->path(),
                               QLatin1String(CONNMAN_TECHNOLOGY_INTERFACE),
                               QLatin1String("PropertiesChanged"),
                               this,SIGNAL(propertyChanged(const QString &, QVariant &)));
    }
}

void QConnmanTechnologyInterface::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QVariant))) {

    }
}

QConnmanMap QConnmanTechnologyInterface::getProperties()
{
    QDBusReply<QConnmanMap> reply =  this->call(QLatin1String("GetProperties"));
    return reply.value();
}

QVariant QConnmanTechnologyInterface::getProperty(const QString &property)
{
    QVariant var;
    QConnmanMap map = getProperties();
    if (map.contains(property)) {
        var = map.value(property);
    }
    return var;
}

// properties
QString QConnmanTechnologyInterface::getState()
{
    QVariant var = getProperty("State");
    return qdbus_cast<QString>(var);
}

QString QConnmanTechnologyInterface::getName()
{
    QVariant var = getProperty("Name");
    return qdbus_cast<QString>(var);
}

QString QConnmanTechnologyInterface::getType()
{
    QVariant var = getProperty("Type");
    return qdbus_cast<QString>(var);
}


QStringList QConnmanTechnologyInterface::getDevices()
{
    QVariant var = getProperty("Devices");
    return qdbus_cast<QStringList>(var);
}


//////////////////////////////////
QConnmanAgentInterface::QConnmanAgentInterface(const QString &dbusPathName, QObject *parent)
    : QDBusAbstractInterface(QLatin1String(CONNMAN_SERVICE),
                             dbusPathName,
                             CONNMAN_AGENT_INTERFACE,
                             QDBusConnection::systemBus(), parent)
{
}

QConnmanAgentInterface::~QConnmanAgentInterface()
{
}

void QConnmanAgentInterface::connectNotify(const char *signal)
{
//    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QVariant))) {
//        dbusConnection.connect(QLatin1String(CONNMAN_SERVICE),
//                               this->path(),
//                               QLatin1String(CONNMAN_NETWORK_INTERFACE),
//                               QLatin1String("PropertiesChanged"),
//                               this,SIGNAL(propertyChanged(const QString &, QVariant &)));
//    }
}

void QConnmanAgentInterface::disconnectNotify(const char *signal)
{
//    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString, QVariant))) {
//
//    }
}


void QConnmanAgentInterface::release()
{
}

void QConnmanAgentInterface::reportError(QDBusObjectPath &path, const QString &error)
{
}

//dict QConnmanAgentInterface::requestInput(QDBusObjectPath &path, dict fields)
//{
//}

void QConnmanAgentInterface::cancel()
{
}


/////////////////////////////////////////

QConnmanDeviceInterface::QConnmanDeviceInterface(const QString &dbusPathName,QObject *parent)
    : QDBusAbstractInterface(QLatin1String(CONNMAN_SERVICE),
                             dbusPathName,
                             CONNMAN_DEVICE_INTERFACE,
                             QDBusConnection::systemBus(), parent)
{
}

QConnmanDeviceInterface::~QConnmanDeviceInterface()
{
}

void QConnmanDeviceInterface::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QVariant))) {
        dbusConnection.connect(QLatin1String(CONNMAN_SERVICE),
                               this->path(),
                               QLatin1String(CONNMAN_DEVICE_INTERFACE),
                               QLatin1String("PropertiesChanged"),
                               this,SIGNAL(propertyChanged(const QString &, QVariant &)));
    }
}

void QConnmanDeviceInterface::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QVariant))) {

    }
}

QConnmanMap QConnmanDeviceInterface::getProperties()
{
    QDBusReply<QConnmanMap> reply =  this->call(QLatin1String("GetProperties"));
    return reply.value();
}

QVariant QConnmanDeviceInterface::getProperty(const QString &property)
{
    QVariant var;
    QConnmanMap map = getProperties();
    if (map.contains(property)) {
        var = map.value(property);
    }
    return var;
}

//properties
QString QConnmanDeviceInterface::getAddress()
{
    QVariant var = getProperty("Address");
    return qdbus_cast<QString>(var);
}

QString QConnmanDeviceInterface::getName()
{
    QVariant var = getProperty("Name");
    return qdbus_cast<QString>(var);
}

QString QConnmanDeviceInterface::getType()
{
    QVariant var = getProperty("Type");
    return qdbus_cast<QString>(var);
}

QString QConnmanDeviceInterface::getInterface()
{
    QVariant var = getProperty("Interface");
    return qdbus_cast<QString>(var);
}

bool QConnmanDeviceInterface::isPowered()
{
    QVariant var = getProperty("Powered");
    return qdbus_cast<bool>(var);
}

quint16 QConnmanDeviceInterface::getScanInterval()
{
    QVariant var = getProperty("ScanInterval");
    return qdbus_cast<quint16>(var);
}

bool QConnmanDeviceInterface::isScanning()
{
    QVariant var = getProperty("Scanning");
     return qdbus_cast<bool>(var);
}

QStringList QConnmanDeviceInterface::getNetworks()
{
    QVariant var = getProperty("Networks");
    return qdbus_cast<QStringList>(var);
}

/////////////////
QT_END_NAMESPACE

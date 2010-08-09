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

#include "qofonoservice_linux_p.h"


QT_BEGIN_NAMESPACE
static QDBusConnection dbusConnection = QDBusConnection::systemBus();


QOfonoManagerInterface::QOfonoManagerInterface( QObject *parent)
        : QDBusAbstractInterface(QLatin1String(OFONO_SERVICE),
                                 QLatin1String(OFONO_MANAGER_PATH),
                                 OFONO_MANAGER_INTERFACE,
                                 QDBusConnection::systemBus(), parent)
{
}

QOfonoManagerInterface::~QOfonoManagerInterface()
{
}

QList <QDBusObjectPath> QOfonoManagerInterface::getModems()
{
    QVariant var = getProperty("Modems");
    return qdbus_cast<QList<QDBusObjectPath> >(var);
}

QDBusObjectPath QOfonoManagerInterface::currentModem()
{
    QList<QDBusObjectPath> modems = getModems();
    foreach(const QDBusObjectPath modem, modems) {
        QOfonoModemInterface device(modem.path());
        if(device.isPowered() && device.isOnline())
        return modem;;
    }
    return QDBusObjectPath();
}


void QOfonoManagerInterface::connectNotify(const char *signal)
{
if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QDBusVariant))) {
        if(!connection().connect(QLatin1String(OFONO_SERVICE),
                               QLatin1String(OFONO_MANAGER_PATH),
                               QLatin1String(OFONO_MANAGER_INTERFACE),
                               QLatin1String("PropertyChanged"),
                               this,SIGNAL(propertyChanged(const QString &, const QDBusVariant & )))) {
            qWarning() << "PropertyCHanged not connected";
        }
    }

    if (QLatin1String(signal) == SIGNAL(propertyChangedContext(QString,QString,QDBusVariant))) {
        QOfonoDBusHelper *helper;
        helper = new QOfonoDBusHelper(this);

        dbusConnection.connect(QLatin1String(OFONO_SERVICE),
                               QLatin1String(OFONO_MANAGER_PATH),
                               QLatin1String(OFONO_MANAGER_INTERFACE),
                               QLatin1String("PropertyChanged"),
                               helper,SLOT(propertyChanged(QString,QDBusVariant)));


        QObject::connect(helper,SIGNAL(propertyChangedContext(const QString &,const QString &,const QDBusVariant &)),
                this,SIGNAL(propertyChangedContext(const QString &,const QString &,const QDBusVariant &)));
    }
}

void QOfonoManagerInterface::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QVariant))) {

    }
}

QVariant QOfonoManagerInterface::getProperty(const QString &property)
{
    QVariant var;
    QVariantMap map = getProperties();
    if (map.contains(property)) {
        var = map.value(property);
    } else {
        qDebug() << Q_FUNC_INFO << "does not contain" << property;
    }
    return var;
}

QVariantMap QOfonoManagerInterface::getProperties()
{
    QDBusReply<QVariantMap > reply = this->call(QLatin1String("GetProperties"));
    return reply.value();
}

QOfonoDBusHelper::QOfonoDBusHelper(QObject * parent)
        : QObject(parent)
{
}

QOfonoDBusHelper::~QOfonoDBusHelper()
{
}

void QOfonoDBusHelper::propertyChanged(const QString &item, const QDBusVariant &var)
{
    QDBusMessage msg = this->message();
    Q_EMIT propertyChangedContext(msg.path() ,item, var);
}


QOfonoModemInterface::QOfonoModemInterface(const QString &dbusPathName, QObject *parent)
    : QDBusAbstractInterface(QLatin1String(OFONO_SERVICE),
                             dbusPathName,
                             OFONO_MODEM_INTERFACE,
                             QDBusConnection::systemBus(), parent)
{
}

QOfonoModemInterface::~QOfonoModemInterface()
{
}

bool QOfonoModemInterface::isPowered()
{
    QVariant var = getProperty("Powered");
    return qdbus_cast<bool>(var);
}

bool QOfonoModemInterface::isOnline()
{
    QVariant var = getProperty("Online");
    return qdbus_cast<bool>(var);
}

QString QOfonoModemInterface::getName()
{
    QVariant var = getProperty("Name");
    return qdbus_cast<QString>(var);
}

QStringList QOfonoModemInterface::getFeatures()
{
    //sms, sim
    QVariant var = getProperty("Features");
    return qdbus_cast<QStringList>(var);
}

QStringList QOfonoModemInterface::getInterfaces()
{
    QVariant var = getProperty("Interfaces");
    return qdbus_cast<QStringList>(var);
}

QString QOfonoModemInterface::defaultInterface()
{
    foreach(const QString &modem,getInterfaces()) {
     return modem;
    }
    return QString();
}


void QOfonoModemInterface::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QDBusVariant))) {
            if(!connection().connect(QLatin1String(OFONO_SERVICE),
                                   this->path(),
                                   QLatin1String(OFONO_MODEM_INTERFACE),
                                   QLatin1String("PropertyChanged"),
                                   this,SIGNAL(propertyChanged(const QString &, const QDBusVariant & )))) {
                qWarning() << "PropertyCHanged not connected";
            }
        }

        if (QLatin1String(signal) == SIGNAL(propertyChangedContext(QString,QString,QDBusVariant))) {
            QOfonoDBusHelper *helper;
            helper = new QOfonoDBusHelper(this);

            dbusConnection.connect(QLatin1String(OFONO_SERVICE),
                                   this->path(),
                                   QLatin1String(OFONO_MODEM_INTERFACE),
                                   QLatin1String("PropertyChanged"),
                                   helper,SLOT(propertyChanged(QString,QDBusVariant)));


            QObject::connect(helper,SIGNAL(propertyChangedContext(const QString &,const QString &,const QDBusVariant &)),
                    this,SIGNAL(propertyChangedContext(const QString &,const QString &,const QDBusVariant &)));
        }}

void QOfonoModemInterface::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QVariant))) {

    }
}

QVariantMap QOfonoModemInterface::getProperties()
{
    QDBusReply<QVariantMap > reply = this->call(QLatin1String("GetProperties"));
    return reply.value();
}

QVariant QOfonoModemInterface::getProperty(const QString &property)
{
    QVariant var;
    QVariantMap map = getProperties();
    if (map.contains(property)) {
        var = map.value(property);
    } else {
        qDebug() << Q_FUNC_INFO << "does not contain" << property;
    }
    return var;
}


QOfonoNetworkInterface::QOfonoNetworkInterface(const QString &dbusPathName, QObject *parent)
    : QDBusAbstractInterface(QLatin1String(OFONO_SERVICE),
                             dbusPathName,
                             OFONO_NETWORK_REGISTRATION_INTERFACE,
                             QDBusConnection::systemBus(), parent)
{
}

QOfonoNetworkInterface::~QOfonoNetworkInterface()
{
}

QString QOfonoNetworkInterface::getStatus()
{
    /*
                "unregistered"  Not registered to any network
                "registered"    Registered to home network
                "searching"     Not registered, but searching
                "denied"        Registration has been denied
                "unknown"       Status is unknown
                "roaming"       Registered, but roaming*/
    QVariant var = getProperty("Status");
    return qdbus_cast<QString>(var);
}

quint16 QOfonoNetworkInterface::getLac()
{
    QVariant var = getProperty("LocationAreaCode");
    return qdbus_cast<quint16>(var);
}


quint16 QOfonoNetworkInterface::getCellId()
{
    QVariant var = getProperty("CellId");
    return qdbus_cast<quint16>(var);
}

QString QOfonoNetworkInterface::getTechnology()
{
    // "gsm", "edge", "umts", "hspa","lte"
    QVariant var = getProperty("Technology");
    return qdbus_cast<QString>(var);
}

QString QOfonoNetworkInterface::getOperatorName()
{
    QVariant var = getProperty("Name");
    return qdbus_cast<QString>(var);
}

int QOfonoNetworkInterface::getSignalStrength()
{
    QVariant var = getProperty("Strength");
    return qdbus_cast<int>(var);

}

QString QOfonoNetworkInterface::getBaseStation()
{
    QVariant var = getProperty("BaseStation");
    return qdbus_cast<QString>(var);
}

QList <QDBusObjectPath> QOfonoNetworkInterface::getOperators()
{
    QVariant var = getProperty("Operators");
    return qdbus_cast<QList <QDBusObjectPath> >(var);
}

void QOfonoNetworkInterface::connectNotify(const char *signal)
{
if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QDBusVariant))) {
        if(!connection().connect(QLatin1String(OFONO_SERVICE),
                               this->path(),
                               QLatin1String(OFONO_NETWORK_REGISTRATION_INTERFACE),
                               QLatin1String("PropertyChanged"),
                               this,SIGNAL(propertyChanged(const QString &, const QDBusVariant & )))) {
            qWarning() << "PropertyCHanged not connected";
        }
    }

    if (QLatin1String(signal) == SIGNAL(propertyChangedContext(QString,QString,QDBusVariant))) {
        QOfonoDBusHelper *helper;
        helper = new QOfonoDBusHelper(this);

        dbusConnection.connect(QLatin1String(OFONO_SERVICE),
                               this->path(),
                               QLatin1String(OFONO_NETWORK_REGISTRATION_INTERFACE),
                               QLatin1String("PropertyChanged"),
                               helper,SLOT(propertyChanged(QString,QDBusVariant)));


        QObject::connect(helper,SIGNAL(propertyChangedContext(const QString &,const QString &,const QDBusVariant &)),
                this,SIGNAL(propertyChangedContext(const QString &,const QString &,const QDBusVariant &)));
    }
}

void QOfonoNetworkInterface::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QVariant))) {

    }
}

QVariant QOfonoNetworkInterface::getProperty(const QString &property)
{
    QVariant var;
    QVariantMap map = getProperties();
    if (map.contains(property)) {
        var = map.value(property);
    } else {
        qDebug() << Q_FUNC_INFO << "does not contain" << property;
    }
    return var;
}

QVariantMap QOfonoNetworkInterface::getProperties()
{
    QDBusReply<QVariantMap > reply =  this->call(QLatin1String("GetProperties"));
    return reply.value();
}



QOfonoNetworkOperatorInterface::QOfonoNetworkOperatorInterface(const QString &dbusPathName, QObject *parent)
    : QDBusAbstractInterface(QLatin1String(OFONO_SERVICE),
                             dbusPathName,
                             OFONO_NETWORK_OPERATOR_INTERFACE,
                             QDBusConnection::systemBus(), parent)
{
}

QOfonoNetworkOperatorInterface::~QOfonoNetworkOperatorInterface()
{
}

QString QOfonoNetworkOperatorInterface::getName()
{
    QVariant var = getProperty("Name");
    return qdbus_cast<QString>(var);
}

QString QOfonoNetworkOperatorInterface::getStatus()
{
    // "unknown", "available", "current" and "forbidden"
    QVariant var = getProperty("Status");
    return qdbus_cast<QString>(var);
}

QString QOfonoNetworkOperatorInterface::getMcc()
{
    QVariant var = getProperty("MobileCountryCode");
    return qdbus_cast<QString>(var);
}

QString QOfonoNetworkOperatorInterface::getMnc()
{
    QVariant var = getProperty("MobileNetworkCode");
    return qdbus_cast<QString>(var);
}

QStringList QOfonoNetworkOperatorInterface::getTechnologies()
{
    QVariant var = getProperty("Technologies");
    return qdbus_cast<QStringList>(var);
}

void QOfonoNetworkOperatorInterface::connectNotify(const char *signal)
{
if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QDBusVariant))) {
        if(!connection().connect(QLatin1String(OFONO_SERVICE),
                               this->path(),
                               QLatin1String(OFONO_NETWORK_OPERATOR_INTERFACE),
                               QLatin1String("PropertyChanged"),
                               this,SIGNAL(propertyChanged(const QString &, const QDBusVariant & )))) {
            qWarning() << "PropertyCHanged not connected";
        }
    }

    if (QLatin1String(signal) == SIGNAL(propertyChangedContext(QString,QString,QDBusVariant))) {
        QOfonoDBusHelper *helper;
        helper = new QOfonoDBusHelper(this);

        dbusConnection.connect(QLatin1String(OFONO_SERVICE),
                               this->path(),
                               QLatin1String(OFONO_NETWORK_OPERATOR_INTERFACE),
                               QLatin1String("PropertyChanged"),
                               helper,SLOT(propertyChanged(QString,QDBusVariant)));


        QObject::connect(helper,SIGNAL(propertyChangedContext(const QString &,const QString &,const QDBusVariant &)),
                this,SIGNAL(propertyChangedContext(const QString &,const QString &,const QDBusVariant &)));
    }
}

void QOfonoNetworkOperatorInterface::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QVariant))) {

    }
}

QVariant QOfonoNetworkOperatorInterface::getProperty(const QString &property)
{
    QVariant var;
    QVariantMap map = getProperties();
    if (map.contains(property)) {
        var = map.value(property);
    } else {
        qDebug() << Q_FUNC_INFO << "does not contain" << property;
    }
    return var;
}

QVariantMap QOfonoNetworkOperatorInterface::getProperties()
{
    QDBusReply<QVariantMap > reply =  this->call(QLatin1String("GetProperties"));
    return reply.value();
}

QOfonoSimInterface::QOfonoSimInterface(const QString &dbusPathName, QObject *parent)
    : QDBusAbstractInterface(QLatin1String(OFONO_SERVICE),
                             dbusPathName,
                             OFONO_SIM_MANAGER_INTERFACE,
                             QDBusConnection::systemBus(), parent)
{
}

QOfonoSimInterface::~QOfonoSimInterface()
{
}

bool QOfonoSimInterface::isPresent()
{
    QVariant var = getProperty("Present");
    return qdbus_cast<bool>(var);
}

QString QOfonoSimInterface::getHomeMcc()
{
    QVariant var = getProperty("MobileCountryCode");
    return qdbus_cast<QString>(var);
}

QString QOfonoSimInterface::getHomeMnc()
{
    QVariant var = getProperty("MobileNetworkCode");
    return qdbus_cast<QString>(var);
}

//    QStringList subscriberNumbers();
//    QMap<QString,QString> serviceNumbers();
QString QOfonoSimInterface::pinRequired()
{
    QVariant var = getProperty("PinRequired");
    return qdbus_cast<QString>(var);
}

QString QOfonoSimInterface::lockedPins()
{
    QVariant var = getProperty("LockedPins");
    return qdbus_cast<QString>(var);
}

QString QOfonoSimInterface::cardIdentifier()
{
    QVariant var = getProperty("CardIdentifier");
    return qdbus_cast<QString>(var);
}

void QOfonoSimInterface::connectNotify(const char *signal)
{
if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QDBusVariant))) {
        if(!connection().connect(QLatin1String(OFONO_SERVICE),
                               this->path(),
                               QLatin1String(OFONO_SIM_MANAGER_INTERFACE),
                               QLatin1String("PropertyChanged"),
                               this,SIGNAL(propertyChanged(const QString &, const QDBusVariant & )))) {
            qWarning() << "PropertyCHanged not connected";
        }
    }

    if (QLatin1String(signal) == SIGNAL(propertyChangedContext(QString,QString,QDBusVariant))) {
        QOfonoDBusHelper *helper;
        helper = new QOfonoDBusHelper(this);

        dbusConnection.connect(QLatin1String(OFONO_SERVICE),
                               this->path(),
                               QLatin1String(OFONO_SIM_MANAGER_INTERFACE),
                               QLatin1String("PropertyChanged"),
                               helper,SLOT(propertyChanged(QString,QDBusVariant)));


        QObject::connect(helper,SIGNAL(propertyChangedContext(const QString &,const QString &,const QDBusVariant &)),
                this,SIGNAL(propertyChangedContext(const QString &,const QString &,const QDBusVariant &)));
    }
}

void QOfonoSimInterface::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QVariant))) {

    }
}

QVariant QOfonoSimInterface::getProperty(const QString &property)
{
    QVariant var;
    QVariantMap map = getProperties();
    if (map.contains(property)) {
        var = map.value(property);
    } else {
        qDebug() << Q_FUNC_INFO << "does not contain" << property;
    }
    return var;
}

QVariantMap QOfonoSimInterface::getProperties()
{
    QDBusReply<QVariantMap > reply =  this->call(QLatin1String("GetProperties"));
    return reply.value();
}

QOfonoDataConnectionInterface::QOfonoDataConnectionInterface(const QString &dbusPathName, QObject *parent)
    : QDBusAbstractInterface(QLatin1String(OFONO_SERVICE),
                             dbusPathName,
                             OFONO_DATA_CONNECTION_MANAGER_INTERFACE,
                             QDBusConnection::systemBus(), parent)
{
}

QOfonoDataConnectionInterface::~QOfonoDataConnectionInterface()
{
}

QList<QDBusObjectPath> QOfonoDataConnectionInterface::getPrimaryContexts()
{
    QVariant var = getProperty("PrimaryContexts");
    return qdbus_cast<QList<QDBusObjectPath> >(var);
}

bool QOfonoDataConnectionInterface::isAttached()
{
    QVariant var = getProperty("Attached");
    return qdbus_cast<bool>(var);
}

bool QOfonoDataConnectionInterface::roamingAttached()
{
    QVariant var = getProperty("Roaming");
    return qdbus_cast<bool>(var);
}

bool QOfonoDataConnectionInterface::isPowered()
{
    QVariant var = getProperty("Powered");
    return qdbus_cast<bool>(var);
}

void QOfonoDataConnectionInterface::connectNotify(const char *signal)
{
if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QDBusVariant))) {
        if(!connection().connect(QLatin1String(OFONO_SERVICE),
                               this->path(),
                               QLatin1String(OFONO_DATA_CONNECTION_MANAGER_INTERFACE),
                               QLatin1String("PropertyChanged"),
                               this,SIGNAL(propertyChanged(const QString &, const QDBusVariant & )))) {
            qWarning() << "PropertyCHanged not connected";
        }
    }

    if (QLatin1String(signal) == SIGNAL(propertyChangedContext(QString,QString,QDBusVariant))) {
        QOfonoDBusHelper *helper;
        helper = new QOfonoDBusHelper(this);

        dbusConnection.connect(QLatin1String(OFONO_SERVICE),
                               this->path(),
                               QLatin1String(OFONO_DATA_CONNECTION_MANAGER_INTERFACE),
                               QLatin1String("PropertyChanged"),
                               helper,SLOT(propertyChanged(QString,QDBusVariant)));


        QObject::connect(helper,SIGNAL(propertyChangedContext(const QString &,const QString &,const QDBusVariant &)),
                this,SIGNAL(propertyChangedContext(const QString &,const QString &,const QDBusVariant &)));
    }
}

void QOfonoDataConnectionInterface::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QVariant))) {

    }
}

QVariant QOfonoDataConnectionInterface::getProperty(const QString &property)
{
    QVariant var;
    QVariantMap map = getProperties();
    if (map.contains(property)) {
        var = map.value(property);
    } else {
        qDebug() << Q_FUNC_INFO << "does not contain" << property;
    }
    return var;
}

QVariantMap QOfonoDataConnectionInterface::getProperties()
{
    QDBusReply<QVariantMap > reply =  this->call(QLatin1String("GetProperties"));
    return reply.value();
}

QOfonoPrimaryDataContextInterface::QOfonoPrimaryDataContextInterface(const QString &dbusPathName, QObject *parent)
    : QDBusAbstractInterface(QLatin1String(OFONO_SERVICE),
                             dbusPathName,
                             OFONO_DATA_CONTEXT_INTERFACE,
                             QDBusConnection::systemBus(), parent)
{
}

QOfonoPrimaryDataContextInterface::~QOfonoPrimaryDataContextInterface()
{
}

bool QOfonoPrimaryDataContextInterface::isActive()
{
    QVariant var = getProperty("Active");
    return qdbus_cast<bool>(var);
}

QString QOfonoPrimaryDataContextInterface::getApName()
{
    QVariant var = getProperty("AccessPointName");
    return qdbus_cast<QString>(var);
}

QString QOfonoPrimaryDataContextInterface::getType()
{
    QVariant var = getProperty("Type");
    return qdbus_cast<QString>(var);
}

QString QOfonoPrimaryDataContextInterface::getName()
{
    QVariant var = getProperty("Name");
    return qdbus_cast<QString>(var);
}

QVariantMap QOfonoPrimaryDataContextInterface::getSettings()
{
    QVariant var = getProperty("Settings");
    return qdbus_cast<QVariantMap>(var);
}

QString QOfonoPrimaryDataContextInterface::getInterface()
{
    QVariant var = getProperty("Interace");
    return qdbus_cast<QString>(var);
}

void QOfonoPrimaryDataContextInterface::connectNotify(const char *signal)
{
if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QDBusVariant))) {
        if(!connection().connect(QLatin1String(OFONO_SERVICE),
                               this->path(),
                               QLatin1String(OFONO_DATA_CONTEXT_INTERFACE),
                               QLatin1String("PropertyChanged"),
                               this,SIGNAL(propertyChanged(const QString &, const QDBusVariant & )))) {
            qWarning() << "PropertyCHanged not connected";
        }
    }

    if (QLatin1String(signal) == SIGNAL(propertyChangedContext(QString,QString,QDBusVariant))) {
        QOfonoDBusHelper *helper;
        helper = new QOfonoDBusHelper(this);

        dbusConnection.connect(QLatin1String(OFONO_SERVICE),
                               this->path(),
                               QLatin1String(OFONO_DATA_CONTEXT_INTERFACE),
                               QLatin1String("PropertyChanged"),
                               helper,SLOT(propertyChanged(QString,QDBusVariant)));


        QObject::connect(helper,SIGNAL(propertyChangedContext(const QString &,const QString &,const QDBusVariant &)),
                this,SIGNAL(propertyChangedContext(const QString &,const QString &,const QDBusVariant &)));
    }
}

void QOfonoPrimaryDataContextInterface::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QVariant))) {

    }
}

QVariant QOfonoPrimaryDataContextInterface::getProperty(const QString &property)
{
    QVariant var;
    QVariantMap map = getProperties();
    if (map.contains(property)) {
        var = map.value(property);
    } else {
        qDebug() << Q_FUNC_INFO << "does not contain" << property;
    }
    return var;
}

QVariantMap QOfonoPrimaryDataContextInterface::getProperties()
{
    QDBusReply<QVariantMap > reply =  this->call(QLatin1String("GetProperties"));
    return reply.value();
}


QOfonoSmsInterface::QOfonoSmsInterface(const QString &dbusPathName, QObject *parent)
    : QDBusAbstractInterface(QLatin1String(OFONO_SERVICE),
                             dbusPathName,
                             OFONO_SMS_MANAGER_INTERFACE,
                             QDBusConnection::systemBus(), parent)
{
}

QOfonoSmsInterface::~QOfonoSmsInterface()
{
}

void QOfonoSmsInterface::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QDBusVariant))) {
        if(!connection().connect(QLatin1String(OFONO_SERVICE),
                                 this->path(),
                                 QLatin1String(OFONO_SMS_MANAGER_INTERFACE),
                                 QLatin1String("PropertyChanged"),
                                 this,SIGNAL(propertyChanged(const QString &, const QDBusVariant & )))) {
            qWarning() << "PropertyCHanged not connected";
        }
    }

    if (QLatin1String(signal) == SIGNAL(propertyChangedContext(QString,QString,QDBusVariant))) {
        QOfonoDBusHelper *helper;
        helper = new QOfonoDBusHelper(this);

        dbusConnection.connect(QLatin1String(OFONO_SERVICE),
                               this->path(),
                               QLatin1String(OFONO_SMS_MANAGER_INTERFACE),
                               QLatin1String("PropertyChanged"),
                               helper,SLOT(propertyChanged(QString,QDBusVariant)));


        QObject::connect(helper,SIGNAL(propertyChangedContext(const QString &,const QString &,const QDBusVariant &)),
                         this,SIGNAL(propertyChangedContext(const QString &,const QString &,const QDBusVariant &)));
    }

    if (QLatin1String(signal) == SIGNAL(immediateMessage(QString,QVariantMap))) {
        if(!connection().connect(QLatin1String(OFONO_SERVICE),
                                 this->path(),
                                 QLatin1String(OFONO_SMS_MANAGER_INTERFACE),
                                 QLatin1String("ImmediateMessage"),
                                 this,SIGNAL(immediateMessage(QString,QVariantMap )))) {
            qWarning() << "PropertyCHanged not connected";
        }
    }

    if (QLatin1String(signal) == SIGNAL(incomingMessage(QString,QVariantMap))) {
        if(!connection().connect(QLatin1String(OFONO_SERVICE),
                                 this->path(),
                                 QLatin1String(OFONO_SMS_MANAGER_INTERFACE),
                                 QLatin1String("IncomingMessage"),
                                 this,SIGNAL(incomingMessage(QString,QVariantMap)))) {
            qWarning() << "PropertyCHanged not connected";
        }
    }
}

void QOfonoSmsInterface::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QVariant))) {

    }
}

QVariant QOfonoSmsInterface::getProperty(const QString &property)
{
    QVariant var;
    QVariantMap map = getProperties();
    if (map.contains(property)) {
        var = map.value(property);
    } else {
        qDebug() << Q_FUNC_INFO << "does not contain" << property;
    }
    return var;
}

QVariantMap QOfonoSmsInterface::getProperties()
{
    QDBusReply<QVariantMap > reply = this->call(QLatin1String("GetProperties"));
    return reply.value();
}

void QOfonoSmsInterface::sendMessage(const QString &to, const QString &message)
{
    QDBusReply<QString> reply =  this->call(QLatin1String("SendMessage"),
                                            QVariant::fromValue(to),
                                            QVariant::fromValue(message));
    bool ok = true;
    if(reply.error().type() == QDBusError::InvalidArgs) {
        qWarning() << reply.error().message();
        ok = false;
    }
}

QT_END_NAMESPACE
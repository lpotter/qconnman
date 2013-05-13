/****************************************************************************
**
** Copyright (C) 2010 Lorn Potter.
** All rights reserved.
** Contact: lorn.potter@gmail.com
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
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qconnman.h"
#include "qconnmanservice_linux_p.h"

#include <connman-qt5/networkmanager.h>

QConnman::QConnman(QObject *parent) :
    QObject(parent)
{
    getInfo();
}

void QConnman::getInfo()
{
    NetworkManager *connman = NetworkManagerFactory::createInstance();
    if(!connman->isAvailable()) {
        //qWarning() << connman->lastError().message();
        emit done();
        return;
    }
    connect(connman, SIGNAL(stateChanged(QString)),this,SLOT(stateChanged(QString)));
    connect(connman,SIGNAL(propertyChanged(QString,QVariant&)),this,SLOT(propertyChanged(QString,QVariant&)));

    qWarning() << "State" << connman->state();
    qWarning() << "available technologies" << connman->technologiesList();
//    qWarning() << "enabled technologies" << connman->getEnabledTechnologies();
//    qWarning() << "connected technologies" << connman->getConnectedTechnologies();

//    qWarning() << "default technology" << connman->getDefaultTechnology();

//    qWarning() << "active profile" << connman->getActiveProfile();
  //  qWarning() << "technologies" << connman->getTechnologies();
//    qWarning() << "services" << connman->servicesList();
//    qWarning() << "profiles" << connman->getProfiles();

//    if(!connman->getEnabledTechnologies().contains("wifi")
//        && connman->enableTechnology("wifi")) {
//        qWarning() << "enabled technologies" << connman->getEnabledTechnologies();

//    }

//    QString wifiPath = connman->getPathForTechnology("/net/connman/technology/wifi");
//    QConnmanTechnologyInterface techInterface(wifiPath, this);

//    qWarning() << techInterface.getName() << techInterface.getPowerState();
//    qWarning() << techInterface.getType();

//    emit done();
}

void QConnman::stateChanged(const QString &state)
{
    qWarning() << Q_FUNC_INFO << state;
}


void QConnman::propertyChanged(const QString &property, QVariant &var)
{
    qWarning() << Q_FUNC_INFO << property << var;

}

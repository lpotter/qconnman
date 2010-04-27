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

#include <QtGui/QtGui>

#include "window.h"
#include <QtGui/QDesktopWidget>
#include <QTreeWidgetItem>
#include <QStringList>

//#include "qconnman.h"
#include "qconnmanservice_linux_p.h"
#include "ui_trayapp.h"


Window::Window()
{
    connman = new QConnmanInterface(this);
    connect(connman,SIGNAL(propertyChanged(const QString &,const QDBusVariant&)),this,SLOT(connManPropertyChanged(const QString &,const QDBusVariant&)));

trayWidget = new QWidget;
    mw = new Ui_TrayApp;
     mw->setupUi(trayWidget);
    // mw->treeWidget->resizeColumnToContents(1);
//     mw->treeWidget->header()->setHidden(true);
//     mw->treeWidget->setAlternatingRowColors(true);

    createActions();
    createTrayIcon();
    connect(this, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    QIcon icon = QIcon(":/images/tray.svg");
    this->setIcon(icon);
    this->setToolTip("foobar");
    this->show();
}
Window::~Window()
{
    delete trayIconMenu;
}

void Window::closeEvent(QCloseEvent *event)
{
    if (this->isVisible()) {
        event->ignore();
    }
}

void Window::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        {
            qWarning() << "trigger click";
            doTrigger();
        }
        break;
    case QSystemTrayIcon::DoubleClick:
        qWarning() << "double click";
        break;
    case QSystemTrayIcon::MiddleClick:
        qWarning() << "middle click";
        break;
    default:
        ;
    }
}


void Window::messageClicked()
{
    qWarning() << __FUNCTION__;
}

void Window::createActions()
{
    trayIconMenu = new QMenu(0);

    if(!connman->isValid()) {
        qWarning() << "not valid";
    } else {
updateTree();
}
    connect(mw->treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this,SLOT(itemClicked(QTreeWidgetItem*,int)));
    quitAction = new QAction(tr("Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    connect(trayIconMenu,SIGNAL(triggered(QAction*)),this,SLOT(menuTriggered(QAction*)));

        this->setContextMenu(trayIconMenu);
}

void Window::createTrayIcon()
{
}

void Window::itemClicked(QTreeWidgetItem* item,int)
{
    qWarning() << item->text(0) << item->text(1);
    if(item->childCount() < 1) {
        if(item->text(1) == "available") {
            QConnmanTechnologyInterface tech(connman->getPathForTechnology(item->text(0)));
            QConnmanDeviceInterface device(tech.getDevices().at(0));
            device.setEnabled(true);

        } else if(item->text(1) == "enabled") {
            if(item->text(0) == "wifi") {
                QStringList knownNetworkList;
                QStringList networkList;
                //   qWarning() << connman->getServices();
                QConnmanTechnologyInterface tech(connman->getPathForTechnology(item->text(0)));
                QConnmanDeviceInterface device(tech.getDevices().at(0));
                QList<QTreeWidgetItem *> treeList;
                item->takeChildren();
                foreach(QString service,connman->getServices()) {
                    QConnmanServiceInterface serv(service,this);
                    if(serv.getType() == item->text(0) ) {
                        knownNetworkList << serv.getName();
                        qWarning() << serv.getName();
                        QTreeWidgetItem *subItem;
                        QStringList columns;
                        QString num;
                        num.setNum(serv.getSignalStrength());
                        columns << serv.getName()
                                << serv.getState()
                                << num
                                << serv.getSecurity();
                        subItem = new QTreeWidgetItem(item, columns);
                        treeList.append(subItem);
                    }
                }
                foreach(QString net, device.getNetworks()) {
                    QConnmanNetworkInterface network(net, this);
//                    if(!networkList.contains(network.getWifiSsid())
//                        && !knownNetworkList.contains(network.getWifiSsid())) {
                        networkList << network.getWifiSsid();
                        //                    qWarning() << "Networks" << network.getWifiSsid();
                        QString num;
                        num.setNum(network.getSignalStrength());
                        QTreeWidgetItem *subItem;
                        subItem = new QTreeWidgetItem(item,QStringList()
                                                      << network.getWifiSsid()
                                                      << (network.isConnected() ? "Connected" : "Disconnected")
                                                      << num
                                                      << network.getWifiSecurity());
                        treeList.append(subItem);
                        if(connect(&network,SIGNAL(propertyChanged(QString,QDBusVariant)),
                                    this,SLOT(networkPropertyChanged(QString,QDBusVariant)))) {
                            qWarning() << "connected";

                        }

//                    }
                }
                item->addChildren(treeList);
            }
            if(item->text(0) == "ethernet") {


            }
        } else {

            qWarning() << "have a child";

        }
    }
}

void Window::subItemClicked(QTreeWidgetItem* item,int)
{

}

void Window::showWifi()
{
    if(!connman->isValid()) {
        qWarning() << "not valid";
    } else {
        qWarning() << "State" << connman->getState();
        qWarning() <<"available tech" << connman->getAvailableTechnologies()
                << "connected tech" << connman->getConnectedTechnologies()
                << "Services" << connman->getServices()
                << "default tech" << connman->getDefaultTechnology()
                ;
    }

}

void Window::menuTriggered(QAction *action)
{
    QStringList menuText = action->text().split(" ");
    qWarning() << action->text();
    QString tech = menuText.at(0);
    QString av = menuText.at(1);

    if(av == "available") {
     QConnmanTechnologyInterface tech(connman->getPathForTechnology(menuText.at(0)));
     QConnmanDeviceInterface device(tech.getDevices().at(0));
     device.setEnabled(true);

 } else if(av == "enabled") {

//    QConnmanTechnologyInterface tech(connman->getPathForTechnology(menuText.at(0)));
//    QConnmanDeviceInterface device(tech.getDevices().at(0));
//     device.setEnabled(false);
 }
}


void Window::doTrigger()
{
    if(trayWidget->isHidden())
        trayWidget->show();
    else
        trayWidget->hide();
}

void Window::connManPropertyChanged(const QString &str, const QDBusVariant &var)
{
    qWarning() << __FUNCTION__ << str << var.variant()
            << "Technologies" << connman->getAvailableTechnologies()
            ;
    updateTree();
}

void Window::techPropertyChanged(const QString &str, const QDBusVariant &var)
{
    qWarning() << __FUNCTION__ << str << var.variant()
            << "Services" << connman->getServices()
            ;

}

void Window::updateTree()
{
    mw->treeWidget->clear();
    foreach(QString techname, connman->getAvailableTechnologies()) {

        QConnmanTechnologyInterface *tech = new QConnmanTechnologyInterface(connman->getPathForTechnology(techname));
        connmanTech.append(tech);

        connect(tech,SIGNAL(propertyChanged(QString,QDBusVariant)),this,SLOT(techPropertyChanged(QString,QDBusVariant)));

        //techname +=" "+ tech->getState();

        //if(tech->getState()) == "enabled"
        QAction *action = new QAction(techname, this );

        trayIconMenu->addAction(action);
        trayIconMenu->addSeparator();
        QTreeWidgetItem *item;
        QStringList list;
        list << techname << tech->getState();
        item = new QTreeWidgetItem(list);

        mw->treeWidget->addTopLevelItem(item);
    }
}

void Window::networkPropertyChanged(const QString &item, const QDBusVariant &value)
{
qWarning() << __FUNCTION__ << item << value.variant();
}


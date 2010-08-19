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

#include "qconnmanservice_linux_p.h"
#include "qofonoservice_linux_p.h"

//#include "connmancloud.h"
#include "ui_form.h"
#include "ui_wizard.h"
#include "ui_sendSmsDialog.h"
#include "ui_smsmessagebox.h"


Window::Window()
{
    connman = new QConnmanManagerInterface(this);

    connect(connman,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
            this,SLOT(connmanPropertyChangedContext(QString,QString,QDBusVariant)));

    connect(connman,SIGNAL(stateChanged(const QString &)),
            this,SLOT(connmanStateChanged(const QString &)));

    trayWidget = new QWidget;
    mw = new Ui_qconnmanApp;
    mw->setupUi(trayWidget);

    mw->servicesTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    mw->networksTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    mw->cellTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(mw->connectButton,SIGNAL(clicked()),this,SLOT(connectService()));
    connect(mw->disconnectButton,SIGNAL(clicked()),this,SLOT(disconnectService()));

    connect(mw->servicesTreeWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(contextMenu(QPoint)));
    connect(mw->networksTreeWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(contextMenu(QPoint)));
    connect(mw->cellTreeWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(contextMenu(QPoint)));

    this->setToolTip(connman->getState());
    createActions();
    createTrayIcon();
    connect(this, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    QIcon icon = QIcon(":/images/tray.svg");
    this->setIcon(icon);
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
    qWarning() << __FUNCTION__ << reason;
    switch (reason) {
    case QSystemTrayIcon::Context:
        {
            appletContextMenu(QPoint(0,0));
        }
        break;
    case QSystemTrayIcon::Trigger:
        {
            doTrigger();
        }
        break;
    case QSystemTrayIcon::DoubleClick:
        break;
    case QSystemTrayIcon::MiddleClick:
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
        qDebug() << "connman not found";
        this->setToolTip(tr("Connman not found"));
    } else {
        updateTree();
    }
    connect(mw->servicesTreeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this,SLOT(itemClicked(QTreeWidgetItem*,int)));
    connect(mw->networksTreeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this,SLOT(itemClicked(QTreeWidgetItem*,int)));
    connect(mw->cellTreeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this,SLOT(cellItemClicked(QTreeWidgetItem*,int)));

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
    qWarning() << __FUNCTION__<< item->text(0) << item->text(1);

}

void Window::subItemClicked(QTreeWidgetItem* /*item*/,int)
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
    if(trayWidget->isHidden()) {
        trayWidget->move(QPoint(QCursor::pos().x()-trayWidget->width(),QCursor::pos().y()+20));
        trayWidget->show();
    } else {
        trayWidget->hide();
    }
}


void Window::updateTree()
{
    qWarning() << __FUNCTION__;
    mw->servicesTreeWidget->clear();
    mw->networksTreeWidget->clear();
    mw->cellTreeWidget->clear();

    foreach(QString technology,connman->getTechnologies()) {
        qDebug() << technology;
        QConnmanTechnologyInterface *tech;
        tech = new QConnmanTechnologyInterface(connman->getPathForTechnology(technology),this);

        connect(tech,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                this,SLOT(technologyPropertyChangedContext(QString,QString,QDBusVariant)));

        QStringList knownNetworkList;
        QStringList networkList;

        if(tech->getDevices().count() > 0) {
            QConnmanDeviceInterface device(tech->getDevices().at(0));
            foreach(QString service,connman->getServices()) {
                QConnmanServiceInterface *serv;
                serv = new QConnmanServiceInterface(service,this);

                if(serv->getType() == tech->getType() ) {
                    knownNetworkList << serv->getName();
                    QTreeWidgetItem *item;
                    QStringList columns;
                    QString num;
                    num.setNum(serv->getSignalStrength());

                    columns << serv->getName()
                            << serv->getState()
                            << num
                            << serv->getSecurity()
                            << serv->getIPv4().value("Address").toString();

                    item = new QTreeWidgetItem( columns);
                    item->setData(0,Qt::UserRole,QVariant(service));
                    item->setData(1,Qt::UserRole,QVariant(serv->getType()));

                    if(!connmanServices.contains(serv))
                        connmanServices.append(serv);
                    connect(connmanServices.at(connmanServices.count() - 1)/*serv*/,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                            this,SLOT(servicesPropertyChangedContext(QString,QString,QDBusVariant)));

                    mw->servicesTreeWidget->addTopLevelItem(item);
                    item->setExpanded(true);
                }
            }
        }
        if(technology.contains("wifi")) {
            // setup wifi networks tree
            if(tech->getDevices().count() > 0) {
                QConnmanDeviceInterface device(tech->getDevices().at(0));
                foreach(const QString net, device.getNetworks()) {

                    QConnmanNetworkInterface *network;
                    network = new QConnmanNetworkInterface(net, this);

                    if(!connmanNetworks.contains(network))
                        connmanNetworks.append(network);

                    networkList << network->getWifiSsid();
                    QString num;
                    num.setNum(network->getSignalStrength());
                    QTreeWidgetItem *netItem;
                    //                QString address;
                    //                QVariantMap map2 = serv->getIPv4();

                    //                qWarning() << map2.count() << qdbus_cast<QVariantMap>(map2.value("IPv4"));
                    //                qWarning() << map2.value("Address").toString();
                    //                QMapIterator<QString,QVariant> it(map2);
                    //                while(it.hasNext()) {
                    //                    it.next();
                    //                    qWarning() << it.key() << it.value();

                    //                }

                    QString service = getServiceForNetwork(net);
                    QConnmanServiceInterface serv(service,this);
                    QString wifissid = network->getWifiSsid();
                    if(wifissid.isEmpty()) {
                        wifissid = "Hidden Network";
                    }

                    netItem = new QTreeWidgetItem(QStringList()
                                                  << wifissid
                                                  << (network->isConnected() ? "Connected" : "Disconnected")
                                                  << num
                                                  << network->getWifiSecurity()
                                                  << network->getAddress());
                    netItem->setData(0,Qt::UserRole,QVariant(service));
                    netItem->setData(1,Qt::UserRole,QVariant(serv.getType()));

                    mw->networksTreeWidget->addTopLevelItem(netItem);
                    netItem->setExpanded(true);
                    connect(network,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                            this,SLOT(networkPropertyChangedContext(QString,QString,QDBusVariant)));

                }
            }
        }
        if(technology.contains("cellular")) {
            ofonoManager = new QOfonoManagerInterface(this);
            connect(ofonoManager,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                    this,SLOT(ofonoPropertyChangedContext(QString,QString,QDBusVariant)));

            Q_FOREACH(QDBusObjectPath path, ofonoManager->getModems()) {
//                initModem(path.path());
                if(!knownModems.contains(path.path())) {
                    QOfonoModemInterface *modemIface;
                    modemIface = new QOfonoModemInterface(path.path(),this);
                    knownModems << path.path();

                    connect(modemIface,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                            this,SLOT(ofonoModemPropertyChangedContext(QString,QString,QDBusVariant)));

                    QOfonoNetworkRegistrationInterface *ofonoNetworkInterface;
                    ofonoNetworkInterface = new QOfonoNetworkRegistrationInterface(path.path(),this);
                    connect(ofonoNetworkInterface,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                            this,SLOT(ofonoNetworkPropertyChangedContext(QString,QString,QDBusVariant)));

                    QTreeWidgetItem *netItem;
                    netItem = new QTreeWidgetItem(QStringList()
                                                  << ofonoNetworkInterface->getOperatorName()
                                                  << ofonoNetworkInterface->getStatus());
                    mw->cellTreeWidget->addTopLevelItem(netItem);
                    netItem->setExpanded(true);
                    netItem->setData(0,Qt::UserRole,QVariant(path.path()));

                    QOfonoDataConnectionInterface dc(path.path(),this);
                    foreach(const QDBusObjectPath dcPath,dc.getPrimaryContexts()) {
                        QOfonoPrimaryDataContextInterface context(dcPath.path(),this);
                        //QTreeWidgetItem *netItem;
                        netItem = new QTreeWidgetItem(QStringList()
                                                      << context.getName()
                                                      << (context.isActive() ? "Connected":"Not Connected")
                                                      << context.getApName()
                                                      << (dc.isPowered() ?"On":"Off"));

                        mw->cellTreeWidget->addTopLevelItem(netItem);
                        netItem->setExpanded(true);
                        netItem->setData(0,Qt::UserRole,QVariant(dcPath.path()));
                    }



                    QOfonoSmsInterface *smsIface;
                    smsIface = new QOfonoSmsInterface(path.path(), this);

                    connect(smsIface,SIGNAL(immediateMessage(QString, QVariantMap)),
                            this,SLOT(immediateMessage(QString,QVariantMap)));

                    connect(smsIface,SIGNAL(incomingMessage(QString, QVariantMap)),
                            this,SLOT(incomingMessage(QString,QVariantMap)));
                }
            }
        }
    }
}

QString Window::getServiceForNetwork(const QString &netPath)
{
///    QMutexLocker locker(&mutex);
    QConnmanNetworkInterface network(netPath, this);
    foreach(const QString service,connman->getServices()) {
        QConnmanServiceInterface serv(service,this);
        if(serv.getName() == network.getName()
            && network.getSignalStrength() == serv.getSignalStrength()) {
            return service;
        }
    }
    return QString();
}
void Window::connManPropertyChanged(const QString &str, const QDBusVariant &var)
{
    qWarning() << __FUNCTION__ << str << var.variant()
//            << "Technologies" << connman->getAvailableTechnologies()
            << connman->getState();

}

void Window::connmanPropertyChangedContext(const QString &path, const QString &item, const QDBusVariant &value)
{
    qWarning() << __FUNCTION__ << item << value.variant();
    if(item == "Services") {
//        QDBusArgument arg = qvariant_cast<QDBusArgument>(value.variant());
//        QStringList list = qdbus_cast<QStringList>(arg);
//        qWarning() << list;
        updateTree();
    }
}

void Window::connmanStateChanged(const QString &/*state*/)
{
  //  qWarning() << __FUNCTION__ << state;
}


void Window::techPropertyChanged(const QString &/*str*/, const QDBusVariant &/*var*/)
{
 //   qWarning() << __FUNCTION__ << str << var.variant()
  //          << "Services" << connman->getServices()
            ;
}

void Window::technologyPropertyChangedContext(const QString & /*path*/, const QString &/*item*/, const QDBusVariant &/*value*/)
{
   // qWarning() << __FUNCTION__ << path << item << value.variant();

}

void Window::networkPropertyChanged(const QString &/*item*/, const QDBusVariant &/*value*/)
{
   // qWarning() << __FUNCTION__ << item << value.variant();
}

void Window::networkPropertyChangedContext(const QString & /*path*/, const QString &/*item*/, const QDBusVariant &/*value*/)
{
//    qWarning() << __FUNCTION__ << path << item << value.variant();

}

void Window::servicesPropertyChanged(const QString &item, const QDBusVariant &value)
{
 //   qWarning() << __FUNCTION__ << item << value.variant() << sender();
    if(value.variant() == "failure") {
    }
    if(item == "Strength") {
     qWarning() << value.variant().toUInt();
    }
    if(item == "State")
        this->setToolTip(connman->getState());
}

void Window::servicesPropertyChangedContext(const QString & /*path*/, const QString &/*item*/, const QDBusVariant &/*value*/)
{
//    qWarning() << __FUNCTION__ << path << item << value.variant();

}

void Window::technologyPropertyChanged(const QString &/*item*/, const QDBusVariant &/*value*/)
{
   // qWarning() << __FUNCTION__ << item << value.variant();
}

void Window::connectToService(const QString &service)
{
    //    QConnectThread *thread;
    //    thread = new QConnectThread(this);
    //    thread->service = service;
    //    thread->start();

    QConnmanServiceInterface *serv;
    serv = new QConnmanServiceInterface(service,this);
    qWarning() << serv->getName() << serv->getType()
            << serv->getSignalStrength()
            << "\npassphrase required:" << serv->isPassphraseRequired()
            << "\nsetup required:" << serv->isSetupRequired();

    QVariantMap map;// = serv->getProperties();

        map.insert("Type", serv->getType());
        if(serv->getType() == "wifi") {
            map.insert("Mode", "managed");
            map.insert("SSID", serv->getName());
            map.insert("Security", serv->getSecurity());
    //        map.insert("Passphrase",serv->getPassphrase());
        }
    if(serv->isPassphraseRequired()) {
        QWizard *wid = new QWizard();
        Ui_Wizard *wiz = new Ui_Wizard();
        wiz->setupUi(wid);
        int result = wid->exec();
        if(result == QDialog::Accepted) {
            map.insert("Passphrase",wiz->passphraseLineEdit->text());
            qWarning() << wiz->passphraseLineEdit->text();
        }
        delete wiz;
        delete wid;
    }

    QDBusObjectPath path = connman->connectService(map);
    qWarning() <<"Service path is"<< path.path();
    if(!path.path().isEmpty()) {
        //Q_EMIT connected(path.path());
    }
}

void Window::connectService()
{
    trayWidget->update();
    if(mw->tabWidget->currentIndex() == 0
        && mw->servicesTreeWidget->currentItem() != NULL) {
        connectToService(mw->servicesTreeWidget->currentItem()->data(0,Qt::UserRole).toString());
    } else if(mw->tabWidget->currentIndex() == 1
               && mw->networksTreeWidget->currentItem() != NULL){
        connectToService(mw->networksTreeWidget->currentItem()->data(0,Qt::UserRole).toString());
    }
    trayWidget->update();
}

void Window::disconnectService()
{
    QString serviceStr;
    if(mw->tabWidget->currentIndex() == 0
       && mw->servicesTreeWidget->currentItem() != NULL) {
        serviceStr = mw->servicesTreeWidget->currentItem()->data(0,Qt::UserRole).toString();
    } else if(mw->tabWidget->currentIndex() == 1
              && mw->networksTreeWidget->currentItem() != NULL){
        serviceStr = mw->networksTreeWidget->currentItem()->data(0,Qt::UserRole).toString();
    }
    QConnmanServiceInterface serv(serviceStr,this);
    serv.disconnect();
}

void Window::removeService()
{
    QString serviceStr = mw->servicesTreeWidget->currentItem()->data(0,Qt::UserRole).toString();
    QConnmanServiceInterface serv(serviceStr,this);
    serv.remove();
    updateTree();
}

void Window::scan()
{
    qWarning() << __FUNCTION__;
    connman->requestScan("wifi");
    foreach(QString technology,connman->getTechnologies()) {
        QConnmanTechnologyInterface tech(connman->getPathForTechnology(technology),this);
        if(tech.getType() == "wifi") {
            QConnmanDeviceInterface device(tech.getDevices().at(0));
            device.scan();
        }
    }
}


void Window::contextMenu(const QPoint &point)
{
    QMenu menu;
    QAction *action;

    if(mw->tabWidget->currentIndex() == 2) {
        if(mw->cellTreeWidget->currentItem() != NULL) {
            action = new QAction("Send SMS",this);
            connect(action,SIGNAL(triggered()),this,SLOT(sendMessage()));
            menu.addAction(action);
        }
    } else {

        action = new QAction("Connect",this);
        connect(action,SIGNAL(triggered()),this,SLOT(connectService()));
        menu.addAction(action);

        action = new QAction("Disconnect",this);
        connect(action,SIGNAL(triggered()),this,SLOT(disconnectService()));
        menu.addAction(action);

        action = new QAction("Remove",this);
        connect(action,SIGNAL(triggered()),this,SLOT(removeService()));
        menu.addAction(action);

        action = new QAction("Scan",this);
        connect(action,SIGNAL(triggered()),this,SLOT(scan()));
        menu.addAction(action);
    }

    menu.exec(mw->servicesTreeWidget->mapToGlobal(point));
}

void  Window::appletContextMenu(const QPoint &/*point*/)
{
//    ConnmanCloud *conncloud;
//    conncloud = new ConnmanCloud(this);
//    CloudView view(conncloud);
//    view.show();

    QMenu menu;
    QAction *action;

    foreach(QString technology,connman->getTechnologies()) {
        qDebug() <<__FUNCTION__<< technology;

        QConnmanTechnologyInterface *tech;
        tech = new QConnmanTechnologyInterface(connman->getPathForTechnology(technology),this);
        bool on = false;
        if(tech->getState() == "enabled") {
            on = true;
        }
        QString textItem = tech->getName() +": "+tech->getState();

        action = new QAction(textItem,this);
        action->setDisabled(true);
        menu.addAction(action);

        if(on) {
            action = new QAction(QString("Disable "+tech->getName()),this);
        } else {
            action = new QAction(QString("Enable "+tech->getName()),this);
        }
        connect(action,SIGNAL(triggered()),this,SLOT(deviceContextMenuClicked()));
        menu.addAction(action);
        menu.addSeparator();
    }

    Q_FOREACH(QDBusObjectPath path, ofonoManager->getModems()) {
        qDebug() << path.path();
        QOfonoModemInterface modemIface(path.path(),this);
        qDebug() << modemIface.getName() << modemIface.isPowered();
        QString str = modemIface.getName();
        if(str.isEmpty()) {
            str = modemIface.getManufacturer() +" "+modemIface.getModel();
        }
        QString textItem = str +": "+(modemIface.isPowered() ? "On":"Off");

        action = new QAction(textItem,this);
        action->setDisabled(true);
        menu.addAction(action);

        if(modemIface.isPowered()) {
            action = new QAction(QString("Disable "+str),this);
        } else {
            action = new QAction(QString("Enable "+str),this);
        }
        connect(action,SIGNAL(triggered()),this,SLOT(deviceContextMenuClicked()));
        menu.addAction(action);
        menu.addSeparator();
    }


    action = new QAction("Quit", this);
    connect(action,SIGNAL(triggered()),qApp,SLOT(quit()));
    menu.addAction(action);
    menu.exec(QCursor::pos());
}

QConnectThread::QConnectThread(QObject *parent)
    :QThread(parent)
{
}

QConnectThread::~QConnectThread()
{
}

void QConnectThread::quit()
{
    mutex.lock();
    keepRunning = false;
    mutex.unlock();
    wait();
}

void QConnectThread::run()
{
    QConnmanManagerInterface *connman;
    connman = new QConnmanManagerInterface();

//    qWarning() << service << "Services" << connman->getServices() <<"\n";
    QString servicepath = service;

    QConnmanServiceInterface *serv;
    serv = new QConnmanServiceInterface(servicepath);

    qWarning() << service << serv->getType() << serv->getName();

    if(serv->getName() == service || serv->getType() == service) {

        qWarning() << serv->getName() << serv->getType()
                << serv->getSignalStrength()
                << "\npassphrase required:" << serv->isPassphraseRequired()
                << serv->getIPv4Configuration();

        QVariantMap map;

        if(serv->isPassphraseRequired()) {
//            QWizard *wid = new QWizard();
//            Ui_Wizard *wiz = new Ui_Wizard();
//            wiz->setupUi(wid);
//            int result = wid->exec();
//            if(result == QDialog::Accepted) {
//                map.insert("Passphrase",wiz->passphraseLineEdit->text());
//                qWarning() << wiz->passphraseLineEdit->text();
//            }
//            delete wiz;
//            delete wid;

            map.insert("Type", serv->getType());
            if(serv->getType() == "wifi") {
                map.insert("Mode", "managed");
                map.insert("SSID", serv->getName());
                map.insert("Security", serv->getSecurity());
                //                map.insert("IPv4.Configuration", "Method=dhcp");
                //                        map.insert("IPv4", "Method=dhcp");
            }

            QDBusObjectPath path = connman->connectService(map);
            qWarning() <<"Service path is"<< path.path();
            if(!path.path().isEmpty()) {
                Q_EMIT connected(path.path());
            }
        } else {
            serv->connect();
        }
    }
    delete serv;
    delete connman;
}

void Window::deviceContextMenuClicked()
{
    QAction *action = static_cast<QAction*>(sender());
    bool doEnable = false;
   if(action->text().contains("Disable")) {
        // disable tech
 //       qWarning() << action->text().split(" ").at(1);
    } else {
        doEnable = true;
        // enable tech
   //     qWarning() << action->text().split(" ").at(1);
    }
qDebug() << Q_FUNC_INFO;
    foreach(QString technology,connman->getTechnologies()) {
        qDebug() << technology;

        QConnmanTechnologyInterface tech(connman->getPathForTechnology(technology),this);
        QConnmanDeviceInterface device(tech.getDevices().at(0));
        if(action->text().split(" ").at(1) == tech.getName()) {
            device.setEnabled(doEnable);
        }
    }
}


void Window::ofonoPropertyChangedContext(const QString &path,const QString &item, const QDBusVariant &value)
{
    qDebug() << __FUNCTION__ << path << item << value.variant();

}

void Window::ofonoNetworkPropertyChangedContext(const QString &path,const QString &item, const QDBusVariant &value)
{
    qDebug() << __FUNCTION__ << path << item << value.variant();

}

void Window::ofonoModemPropertyChangedContext(const QString &path,const QString &item, const QDBusVariant &value)
{
    qDebug() << __FUNCTION__ << path << item << value.variant();

}

void Window::immediateMessage(const QString& msg,const QVariantMap &map)
{
    qDebug() <<__FUNCTION__ << msg << map["Info"] << map["LocalSentTime"];
}

void Window::incomingMessage(const QString& msg,const QVariantMap &map)
{
    qDebug() <<__FUNCTION__ << msg << map["Sender"] << map["LocalSentTime"];
    QDialog *wid = new QDialog();
    Ui::SmsMessageBox d;
    d.setupUi(wid);
    d.fromLineEdit->setText(map["Sender"].toString());
    d.messageTextEdit->setText( msg);
    d.timeSentLabel->setText(map["LocalSentTime"].toString());
    int result = wid->exec();
    qDebug() << result;
    if(result != 0) {

    }
    delete wid;

}

void Window::cellItemClicked(QTreeWidgetItem *item,int)
{
    qDebug() << item->text(0) << item->data(0,Qt::UserRole).toString();
}

void Window::sendMessage()
{
    QDialog *wid = new QDialog();
    Ui::Dialog d;
    d.setupUi(wid);
    int result = wid->exec();
    if(result != 0) {
        QString to = d.toLineEdit->text();
        QString msg = d.messageText->toPlainText();

        QOfonoSmsInterface smsIface(mw->cellTreeWidget->currentItem()->data(0,Qt::UserRole).toString(),this);

        smsIface.sendMessage(to,msg);
    }
    delete wid;
}

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

#include <QtGui/QtGui>

#include "window.h"
#include "agentadaptor.h"
#include <QtGui/QDesktopWidget>
#include <QTreeWidgetItem>
#include <QStringList>

#include "qconnmanservice_linux_p.h"
#include "qofonoservice_linux_p.h"

//#include "connmancloud.h"
#include "ui_form.h"
#include "ui_wizard.h"
#include "ui_sendSmsDialog.h"
#include "newmessage.h"
//#include "ui_smsmessagebox.h"

static const char AGENT_PATH[] = "/ConnmanAgent";

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

    ua = new UserAgent(connman, this);

    connect(ua,SIGNAL(userInputRequested(QString,QVariantList)),
            this,SLOT(userInputRequested(QString,QVariantList)));


    QIcon icon = QIcon(":/images/tray.svg");
    this->setIcon(icon);
    this->show();
}

Window::~Window()
{
    QDBusObjectPath obPath("/ConnmanAgent");
    connman->unregisterAgent(obPath);
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
    connectToService(item->data(0,Qt::UserRole).toString());
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
        qDebug() << "Tech" << tech->getName();
        connect(tech,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                this,SLOT(technologyPropertyChangedContext(QString,QString,QDBusVariant)));
    }

        QStringList knownNetworkList;
        QStringList networkList;

        foreach(QString service,connman->getServices()) {
            QConnmanServiceInterface *serv;
            serv = new QConnmanServiceInterface(service,this);
            qDebug() << "Service" << serv->getName();

                knownNetworkList << serv->getName();
                QTreeWidgetItem *item;
                QStringList columns;
                QString num;
                num.setNum(serv->getSignalStrength());

                columns << serv->getName()
                        << ((serv->getState() =="ready")  ? "online": serv->getState())
                        << num
                        << serv->getSecurity()
                        << serv->getIPv4().value("Address").toString();

                item = new QTreeWidgetItem( columns);
                item->setData(0,Qt::UserRole,QVariant(service));
                item->setData(1,Qt::UserRole,QVariant(serv->getType()));

                if(!connmanServices.contains(serv))
                    connmanServices.append(serv);

                qDebug() << Q_FUNC_INFO << connmanServices.at(connmanServices.count() - 1)
                            << serv->getState();

                connect(/*connmanServices.at(connmanServices.count() - 1)*/serv,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                        this,SLOT(servicesPropertyChangedContext(QString,QString,QDBusVariant)));

                mw->servicesTreeWidget->addTopLevelItem(item);
                item->setExpanded(true);
            if(serv->getType().contains("wifi")) {
                // setup wifi networks tree

                if(!connmanNetworks.contains(serv))
                    connmanNetworks.append(serv);

                networkList << serv->getName();
                QString num;
                num.setNum(serv->getSignalStrength());
                QTreeWidgetItem *netItem;
                QString address;
                QVariantMap map2 = serv->getIPv4();

                qWarning() << map2.count() << qdbus_cast<QVariantMap>(map2.value("IPv4"));
                qWarning() << map2.value("Address").toString();
                address = map2.value("Address").toString();
                QMapIterator<QString,QVariant> it(map2);
                while(it.hasNext()) {
                    it.next();
                    qWarning() << it.key() << it.value();

                }

                QString wifissid = serv->getName();
                if(wifissid.isEmpty()) {
                    wifissid = "Hidden Network";
                }

                netItem = new QTreeWidgetItem(QStringList()
                                              << wifissid
                                              << ((serv->getState() =="ready")  ? "online": serv->getState())
                                              << num
                                              << serv->getSecurity()
                                              <<address);
                netItem->setData(0,Qt::UserRole,QVariant(service));
                netItem->setData(1,Qt::UserRole,QVariant(serv->getType()));

                mw->networksTreeWidget->addTopLevelItem(netItem);
                netItem->setExpanded(true);
            }
        if(serv->getType().contains("cellular")) {
            ofonoManager = new QOfonoManagerInterface(this);
            connect(ofonoManager,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                    this,SLOT(ofonoPropertyChangedContext(QString,QString,QDBusVariant)));

            Q_FOREACH(QDBusObjectPath path, ofonoManager->getModems()) {
//                initModem(path.path());
                    QOfonoModemInterface *modemIface;
                    modemIface = new QOfonoModemInterface(path.path(),this);
                    if(!knownModems.contains(path.path()))
                        knownModems << path.path();
                    if(!modemIface->isPowered())
                        continue;
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


                    // get any 3G connection types
                    QOfonoDataConnectionManagerInterface dc(path.path(),this);
                    foreach(const QDBusObjectPath dcPath,dc.getPrimaryContexts()) {
                        QOfonoPrimaryDataContextInterface context(dcPath.path(),this);
                        if(context.isValid() && !context.getName().isEmpty()) {
                            netItem = new QTreeWidgetItem(QStringList()
                                                          << context.getName()
                                                          << (context.isActive() ? "Connected":"Not Connected")
                                                          << context.getApName()
                                                          << (dc.isPowered() ?"On":"Off"));

                            mw->cellTreeWidget->addTopLevelItem(netItem);
                            netItem->setExpanded(true);
                            netItem->setData(0,Qt::UserRole,QVariant(dcPath.path()));
                        }
                    QOfonoMessageManagerInterface *smsIface;
                    smsIface = new QOfonoMessageManagerInterface(path.path(), this);

                    connect(smsIface,SIGNAL(immediateMessage(QString, QVariantMap)),
                            this,SLOT(immediateMessage(QString,QVariantMap)));

                    connect(smsIface,SIGNAL(incomingMessage(QString, QVariantMap)),
                            this,SLOT(incomingMessage(QString,QVariantMap)));
                }
            }
        }
    }
}

void Window::connManPropertyChanged(const QString &str, const QDBusVariant &var)
{
    qWarning() << __FUNCTION__ << str << var.variant()
//            << "Technologies" << connman->getAvailableTechnologies()
            << connman->getState();

}

void Window::connmanPropertyChangedContext(const QString &/*path*/, const QString &item, const QDBusVariant &value)
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
    qWarning() << __FUNCTION__ << item << value.variant() << sender();
    if(value.variant() == "failure") {
    }
    if(item == "Strength") {
     qWarning() << value.variant().toUInt();
    }
    if(item == "State")
        this->setToolTip(connman->getState());
}

void Window::servicesPropertyChangedContext(const QString & path, const QString &item, const QDBusVariant &value)
{
  //  updateTree();
   qWarning() << __FUNCTION__ << item << value.variant();
   if (item == "State") {
//       QConnmanServiceInterface *serv;
//       serv = new QConnmanServiceInterface(path,this);

       QTreeWidgetItemIterator it( mw->servicesTreeWidget);
       while (*it) {
           qDebug() << (*it)->data(0,Qt::UserRole).toString()
                       << path;
           if ((*it)->data(0,Qt::UserRole).toString() == path)
           {
               QString state = value.variant().toString();
               if (state == "ready" )
                   state == "online";
               (*it)->setText(1,state);
               break;
           }
           ++it;
       }
       QTreeWidgetItemIterator it2(mw->networksTreeWidget);
       while (*it2) {
           qDebug() << (*it2)->data(0,Qt::UserRole).toString()
                       << path;
           if ((*it2)->data(0,Qt::UserRole).toString() == path)
           {
               QString state = value.variant().toString();
               if (state == "ready" )
                   state == "online";
               (*it2)->setText(1,state);
               break;
           }
           ++it2;
       }

   } else if (item == "Strength") {
         qWarning() << value.variant().toUInt();
//       QConnmanServiceInterface *serv;
//       serv = new QConnmanServiceInterface(path,this);
       QTreeWidgetItemIterator it( mw->servicesTreeWidget);
       while (*it) {
           qDebug() << (*it)->data(0,Qt::UserRole).toString()
                       << path;
           if ((*it)->data(0,Qt::UserRole).toString() == path) {
               (*it)->setText(2, QString::number(value.variant().toUInt()));
               break;
           }
           ++it;
       }
       QTreeWidgetItemIterator it2(mw->networksTreeWidget);
       while (*it2) {
           qDebug() << (*it2)->data(0,Qt::UserRole).toString()
                       << path;
           if ((*it2)->data(0,Qt::UserRole).toString() == path) {
               (*it2)->setText(2, QString::number(value.variant().toUInt()));
               break;
           }
           ++it2;
       }
   }

}

void Window::technologyPropertyChanged(const QString &item, const QDBusVariant &value)
{
 //   updateTree();
    qWarning() << __FUNCTION__ << item << value.variant();
}

void Window::connectToService(const QString &service)
{

    QConnmanServiceInterface *serv;
    serv = new QConnmanServiceInterface(service,this);
    qWarning() << Q_FUNC_INFO
                  << service
               << serv->getName()
               << serv->getType()
               << serv->getSignalStrength();
    serv->connect();
}

void Window::connectService()
{
    trayWidget->update();
    if(mw->tabWidget->currentIndex() == 0
        && mw->servicesTreeWidget->currentItem() != NULL) {
        connectToService(mw->servicesTreeWidget->currentItem()->data(0,Qt::UserRole).toString());
    } else if(mw->tabWidget->currentIndex() == 1
               && mw->networksTreeWidget->currentItem() != NULL) {
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
//    foreach(QString technology,connman->getTechnologies()) {
//        QConnmanTechnologyInterface tech(connman->getPathForTechnology(technology),this);
//        if(tech.getType() == "wifi") {
//            QConnmanDeviceInterface device(tech.getDevices().at(0));
//            device.scan();
//        }
//    }
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
        if(tech->getPowerState() == "true") {
            on = true;
        }
        QString textItem = tech->getName() +": "+tech->getPowerState();

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

    if(connman->getTechnologies().contains("cellular")) {
        Q_FOREACH(QDBusObjectPath path, ofonoManager->getModems()) {
            if(!path.path().isEmpty() || !path.path().isNull()) {
                QOfonoModemInterface modemIface(path.path(),this);
                if(!modemIface.isValid()) {
                    continue;
                }
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
        }
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
                << serv->getIPv4Configuration();

        QVariantMap map;

            serv->connect();
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
//qDebug() << Q_FUNC_INFO;
//    foreach(QString technology,connman->getTechnologies()) {
//        qDebug() << technology;

////        QConnmanTechnologyInterface tech(connman->getPathForTechnology(technology),this);
////        QConnmanDeviceInterface device(tech.getDevices().at(0));
////        if(action->text().split(" ").at(1) == tech.getName()) {
////            device.setEnabled(doEnable);
////        }
//    }
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
    NewMessage *newmessagebox = new NewMessage();
    newmessagebox->setAddress(map["Sender"].toString());
    newmessagebox->setBody( msg);
    newmessagebox->setTimestamp(map["LocalSentTime"].toString());
    connect(newmessagebox,SIGNAL(replyTo(QString)),
            this,SLOT(sendMessage(QString)));

    newmessagebox->exec();
//    qDebug() << result;
//    if(result != 0) {

//    }
    delete newmessagebox;

}

void Window::cellItemClicked(QTreeWidgetItem *item,int)
{
    qDebug() << item->text(0) << item->data(0,Qt::UserRole).toString();
}

void Window::sendMessage()
{
    sendMessage("");
}

void Window::sendMessage(const QString &address)
{
    QDialog *wid = new QDialog();
    Ui::Dialog d;
    d.setupUi(wid);
    wid->setWindowTitle("Send New Message");
    d.toLineEdit->setText(address);

    int result = wid->exec();
    if(result != 0) {
        QString to = d.toLineEdit->text();
        QString msg = d.messageText->toPlainText();

        QOfonoMessageManagerInterface smsIface(mw->cellTreeWidget->currentItem()->data(0,Qt::UserRole).toString(), 0);
        smsIface.sendMessage(to,msg);
    }
    delete wid;
}

void Window::userInputRequested(const QString &servicePath, const QVariantList &fields)
{
    qDebug() << Q_FUNC_INFO << servicePath << fields;
    QConnmanServiceInterface *serv;
    serv = new QConnmanServiceInterface(servicePath);

    QVariantMap map;
    QWizard *wid = new QWizard();
    Ui_Wizard *wiz = new Ui_Wizard();
    wiz->setupUi(wid);
    int result = wid->exec();
    if(result == QDialog::Accepted) {
        map.insert("Passphrase",wiz->passphraseLineEdit->text());
        qWarning() << wiz->passphraseLineEdit->text();
    }

    map.insert("Type", serv->getType());
    if(serv->getType() == "wifi") {
        map.insert("Mode", "managed");
        map.insert("SSID", serv->getName());
        map.insert("Security", serv->getSecurity());
        //                map.insert("IPv4.Configuration", "Method=dhcp");
        //                        map.insert("IPv4", "Method=dhcp");
    }

    ua->sendUserReply(map);
    delete wiz;
    delete wid;
}


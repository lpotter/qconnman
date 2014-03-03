/****************************************************************************
**
** Copyright (C) 2010-2013 Lorn Potter.
** All rights reserved.
** Contact: lorn.potter@gmail.com
**
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**
****************************************************************************/


#include <QtWidgets/QDesktopWidget>
#include <QMessageBox>
#include <QtGui>
#include <QMenu>
#include <QTreeWidgetItem>
#include <QStringList>

#include "window.h"

#include "qofonoservice_linux_p.h"

#include "ui_form.h"
#include "ui_wizard.h"
#include "ui_sendSmsDialog.h"
#include "newmessage.h"

static const char AGENT_PATH[] = "/ConnmanAgent";

Window::Window() :
    connman(0)
  , ua(0)
  , watcher(0),
    connmanAvailable(0)
{
    trayWidget = new QWidget(this);
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


    watcher = new QDBusServiceWatcher("net.connman",QDBusConnection::systemBus(),
                                      QDBusServiceWatcher::WatchForRegistration |
                                      QDBusServiceWatcher::WatchForUnregistration, this);

    connect(watcher, SIGNAL(serviceRegistered(QString)),
            this, SLOT(connectToConnman(QString)));
    connect(watcher, SIGNAL(serviceUnregistered(QString)),
            this, SLOT(connmanUnregistered(QString)));

    if (QDBusConnection::systemBus().interface()->isServiceRegistered("net.connman")) {
        qDebug() << "connman service ok, connect";
        initNetworkManager();
    }
    createActions();
    createTrayIcon();
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    this->setWindowTitle("QConnman the connection tray");
//    QIcon icon = QIcon(":/images/preferences-system-network-2.svg");
    QIcon icon = QIcon(":/images/tray.svg");
    trayIcon->setIcon(icon);
    trayIcon->show();

}


Window::~Window()
{
    connman->unregisterAgent("/ConnmanAgent");
    delete trayIconMenu;
}

void Window::initNetworkManager()
{
    connman = NetworkManagerFactory::createInstance();

    connect(connman,SIGNAL(technologiesChanged()),this,SLOT(techChanged()));
    connect(connman,SIGNAL(servicesChanged()),this,SLOT(servicesChanged()));

    connect(connman,SIGNAL(availabilityChanged(bool)),
            this,SLOT(connmanAvailableChanged(bool)));

    connect(connman,SIGNAL(stateChanged(const QString &)),
            this,SLOT(connmanStateChanged(const QString &)));

    if (!ua)
        ua = new UserAgent(this);

    connect(ua,SIGNAL(userInputRequested(QString,QVariantMap)),
            this,SLOT(userInputRequested(QString,QVariantMap)));
    connect(ua,SIGNAL(userConnectRequested(QDBusMessage)),
            this,SLOT(requestConnect(QDBusMessage)));
    connmanAvailable = connman->isAvailable();
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
    trayIconMenu = new QMenu(this);

    if(!connman) {
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
}

void Window::createTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void Window::itemClicked(QTreeWidgetItem* item,int)
{
    qWarning() << __FUNCTION__<< item->text(0) << item->text(1);
    connectToService(item->data(0,Qt::UserRole).toString());
}

void Window::subItemClicked(QTreeWidgetItem* /*item*/,int)
{

}

void Window::menuTriggered(QAction *action)
{
    QStringList menuText = action->text().split(" ");
    qWarning() << action->text();
    QString tech = menuText.at(0);
    QString av = menuText.at(1);
    qDebug() << Q_FUNC_INFO;
    if(av == "available") {
    } else if(av == "enabled") {

        //    QConnmanTechnologyInterface tech(connman->getPathForTechnology(menuText.at(0)));
        //    QConnmanDeviceInterface device(tech.getDevices().at(0));
        //     device.setEnabled(false);
    }
}


void Window::doTrigger()
{
    qDebug() << Q_FUNC_INFO << this->isHidden();
    if(this->isHidden()) {
        this->move(QPoint(QCursor::pos().x()-trayWidget->width(),QCursor::pos().y()+20));
        this->showNormal();
        //   trayWidget->showNormal();
    } else {
        this->hide();
        //   trayWidget->hide();
    }
}

void Window::updateTree()
{
    qWarning() << __FUNCTION__;
    mw->servicesTreeWidget->clear();
    mw->networksTreeWidget->clear();
    mw->cellTreeWidget->clear();

    QVector<NetworkTechnology *> techs = connman->getTechnologies();
    for (int i = 0; i < techs.size(); ++i) {
        NetworkTechnology *tech = techs.at(i);

        QStringList knownNetworkList;
        QStringList networkList;

        foreach(NetworkService* serv, connman->getServices(tech->type())) {

            knownNetworkList << serv->name();
            QTreeWidgetItem *item;
            QStringList columns;
            QString num;
            num.setNum(serv->strength());

            columns << (serv->name().isEmpty() ? "Hidden" : serv->name())
                    << serv->state()
                    << num
                    << (serv->security().isEmpty() ? "none" : serv->security().join(", "))
                    << serv->ipv4().value("Address").toString()
                    << serv->ethernet().value("Interface").toString()
                       << serv->ethernet().value("Address").toString();

            item = new QTreeWidgetItem(columns);
            item->setData(0,Qt::UserRole,QVariant(serv->path()));
            item->setData(1,Qt::UserRole,QVariant(serv->type()));

            if (serv->state() == "online" && connman->defaultRoute()->name() == serv->name()) {
                for (int i = 0; i < 5; i++) {
                    item->setTextColor(i, QColor("red"));
                    item->setBackground(i,*(new QBrush(Qt::lightGray,Qt::SolidPattern)));
                }
            }
            connect(serv,SIGNAL(stateChanged(QString)),
                    this,SLOT(serviceStateChanged(QString)));

            connect(serv,SIGNAL(strengthChanged(uint)),
                    this,SLOT(serviceStrengthChanged(uint)));

            connect(serv,SIGNAL(ipv4ConfigChanged(QVariantMap)),
                    this,SLOT(servicesIpv4ConfigChanged(QVariantMap)));

            mw->servicesTreeWidget->addTopLevelItem(item);
            item->setExpanded(true);


            if(serv->type().contains("wifi")) {

                networkList << serv->name();
                QString num;
                num.setNum(serv->strength());
                QTreeWidgetItem *netItem;
                QString address;
                QVariantMap map2 = serv->ipv4();

                address = map2.value("Address").toString();

                QString wifissid = serv->name();
                if(wifissid.isEmpty()) {
                    wifissid = "Hidden Network";
                }

                netItem = new QTreeWidgetItem(QStringList()
                                              << wifissid
                                              << ((serv->state() =="ready")  ? "online": serv->state())
                                              << num
                                              << (serv->security().isEmpty() ? "none" : serv->security().join(", "))
                                              << address);
                netItem->setData(0,Qt::UserRole,QVariant(serv->path()));
                netItem->setData(1,Qt::UserRole,QVariant(serv->type()));

                mw->networksTreeWidget->addTopLevelItem(netItem);
                netItem->setExpanded(true);
            }

            if(serv->type().contains("cellular")) {

                ofonoManager = new QOfonoManagerInterface(this);
                connect(ofonoManager,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                        this,SLOT(ofonoPropertyChangedContext(QString,QString,QDBusVariant)));
                qDebug() << Q_FUNC_INFO <<  ofonoManager->getModems().count();
                Q_FOREACH(QDBusObjectPath path, ofonoManager->getModems()) {
                    //                initModem(path.path());
                    QOfonoModemInterface *modemIface;
                    modemIface = new QOfonoModemInterface(path.path(),this);
                    qDebug() << Q_FUNC_INFO << path.path();

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
}

void Window::connmanStateChanged(const QString &state)
{
    qWarning() << __FUNCTION__ << state;
    this->setToolTip(connman->state());
    if (state == "idle") {
        QIcon icon = QIcon(":/images/tray.svg");
        trayIcon->setIcon(icon);
     //  trayIcon->icon().
        trayIcon->show();
    }
    if (state == "online"  || state == "ready") {
        QIcon icon = QIcon(":/images/preferences-system-network-2.svg");
        trayIcon->setIcon(icon);
        trayIcon->show();
    }
}

void Window::serviceStrengthChanged(uint value)
{
    NetworkService *service = qobject_cast<NetworkService *>(sender());

    QTreeWidgetItemIterator it( mw->servicesTreeWidget);
    while (*it) {
        if ((*it)->data(0,Qt::UserRole).toString() == service->path()) {
            (*it)->setText(2, QString::number(value));
            break;
        }
        ++it;
    }
    QTreeWidgetItemIterator it2(mw->networksTreeWidget);
    while (*it2) {
        if ((*it2)->data(0,Qt::UserRole).toString() == service->path()) {
            (*it2)->setText(2, QString::number(value));
            break;
        }
        ++it2;
    }
}

void Window::servicesIpv4ConfigChanged(QVariantMap map)
{
    NetworkService *service = qobject_cast<NetworkService *>(sender());

    QString address;
    address = map.value("Address").toString();

    QTreeWidgetItemIterator it( mw->servicesTreeWidget);
    while (*it) {
        if ((*it)->data(0,Qt::UserRole).toString() == service->path()) {
            (*it)->setText(4, address);
            break;
        }
        ++it;
    }
    QTreeWidgetItemIterator it2(mw->networksTreeWidget);
    while (*it2) {
        if ((*it2)->data(0,Qt::UserRole).toString() == service->path()) {
            (*it2)->setText(4, address);
            break;
        }
        ++it2;
    }
}

void Window::serviceStateChanged(const QString &state)
{
    NetworkService *service = qobject_cast<NetworkService *>(sender());

    QTreeWidgetItemIterator it( mw->servicesTreeWidget);
    while (*it) {
        if ((*it)->data(0,Qt::UserRole).toString() == service->path()) {
            if (state == "ready" )
                state == "online";
            (*it)->setText(1,state);
            break;
        }
        ++it;
    }
    QTreeWidgetItemIterator it2(mw->networksTreeWidget);
    while (*it2) {
        if ((*it2)->data(0,Qt::UserRole).toString() == service->path()) {
            if (state == "ready" )
                state == "online";
            (*it2)->setText(1,state);
            break;
        }
        ++it2;
    }
}


void Window::connectToService(const QString &service)
{
    qDebug() << connmanAvailable;
    if (!connmanAvailable)
        return;
    NetworkService *serv;
    serv = new NetworkService(this);
    serv->setPath(service);
    connect(serv,SIGNAL(connectRequestFailed(QString)),this,SLOT(connectRequestFailed(QString)));
    serv->requestConnect();
}

void Window::connectService()
{
    if (!connmanAvailable)
        return;
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
    if (!connmanAvailable)
        return;
    QString serviceStr;

    if(mw->tabWidget->currentIndex() == 0
            && mw->servicesTreeWidget->currentItem() != NULL) {
        serviceStr = mw->servicesTreeWidget->currentItem()->data(0,Qt::UserRole).toString();
    } else if(mw->tabWidget->currentIndex() == 1
              && mw->networksTreeWidget->currentItem() != NULL){
        serviceStr = mw->networksTreeWidget->currentItem()->data(0,Qt::UserRole).toString();
    }
    NetworkService serv(this);
    serv.setPath(serviceStr);
    qDebug() << Q_FUNC_INFO << serv.name();

    serv.requestDisconnect();
}

void Window::removeService()
{
    if (!connmanAvailable)
        return;
    QString serviceStr = mw->servicesTreeWidget->currentItem()->data(0,Qt::UserRole).toString();
    NetworkService serv(this);
    serv.setPath(serviceStr);
    serv.remove();
    updateTree();
}

void Window::scan()
{
    if (!connmanAvailable)
        return;
    NetworkTechnology* technology = connman->getTechnology("wifi");
    technology->scan();
}

void Window::doAutoConnect()
{
    if (!connmanAvailable)
        return;
    QString serviceStr = mw->servicesTreeWidget->currentItem()->data(0,Qt::UserRole).toString();
    NetworkService serv(this);
    serv.setPath(serviceStr);
    serv.setAutoConnect(true);
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

        action = new QAction("Autoconnect", this);
        connect(action,SIGNAL(triggered()),this,SLOT(doAutoConnect()));
        menu.addAction(action);
    }

    menu.exec(mw->servicesTreeWidget->mapToGlobal(point));
}

void  Window::appletContextMenu(const QPoint &/*point*/)
{
    QMenu menu;
    QAction *action;
    if (connman) {
        foreach(NetworkTechnology *tech,connman->getTechnologies()) {

            bool on = false;
            qDebug() <<Q_FUNC_INFO << tech->powered();
            if(tech->powered()) {
                on = true;
            }
            QString textItem = tech->type() +": "+tech->powered();

            action = new QAction(textItem,this);
            action->setDisabled(true);
            menu.addAction(action);

            if(on) {
                action = new QAction(QString("Disable "+tech->type()),this);
            } else {
                action = new QAction(QString("Enable "+tech->type()),this);
            }
            connect(action,SIGNAL(triggered()),this,SLOT(deviceContextMenuClicked()));
            menu.addAction(action);
            menu.addSeparator();

            if(tech->type().contains("cellular")) {
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
    NetworkManager *connman = NetworkManagerFactory::createInstance();
    //   connman = new QConnmanManagerInterface();

    //    qWarning() << service << "Services" << connman->getServices() <<"\n";
    QString servicepath = service;

    NetworkService *serv;
    serv = new NetworkService();
    serv->setPath(servicepath);

    qWarning() << service << serv->type() << serv->name();

    if(serv->name() == service || serv->type() == service) {

        qWarning() << serv->name() << serv->type()
                   << serv->strength();
        QVariantMap map;

        serv->requestConnect();
    }
    delete serv;
    delete connman;
}

void Window::deviceContextMenuClicked()
{
    QAction *action = static_cast<QAction*>(sender());
    bool doEnable = false;
    qDebug() << Q_FUNC_INFO << action->text();

    if(action->text().contains("Disable")) {
        // disable tech
    } else {
        doEnable = true;
        // enable tech
    }

    foreach(NetworkTechnology *technology,connman->getTechnologies()) {

        if(action->text().contains(technology->type())) {

            qDebug() <<Q_FUNC_INFO << technology->type() << doEnable << "powered" << technology->powered();

            technology->setPowered(doEnable);
            if (doEnable)
                technology->scan();

            qDebug() <<Q_FUNC_INFO << technology->type() << "powered" << technology->powered();
            updateTree();
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

void Window::userInputRequested(const QString &servicePath, const QVariantMap &fields)
{
    qDebug() << Q_FUNC_INFO << servicePath << fields;
    NetworkService *serv;
    serv = new NetworkService();

    serv->setPath(servicePath);

    QVariantMap map;
    QWizard *wid = new QWizard();
    Ui_Wizard *wiz = new Ui_Wizard();
    wiz->setupUi(wid);
    int result = wid->exec();
    if(result == QDialog::Accepted) {
        map.insert("Passphrase",wiz->passphraseLineEdit->text());
        qWarning() << wiz->passphraseLineEdit->text();

        map.insert("Type", serv->type());
        if(serv->type() == "wifi") {
            map.insert("Mode", "managed");
            map.insert("SSID", serv->name());
            map.insert("Security", serv->security());
            //                map.insert("IPv4.Configuration", "Method=dhcp");
            //                        map.insert("IPv4", "Method=dhcp");
        }
    }

    ua->sendUserReply(map);
    delete wiz;
    delete wid;
}

void Window::requestConnect(const QDBusMessage &/*msg*/)
{
    qDebug() << Q_FUNC_INFO;
    QMessageBox msgBox;
    msgBox.setText("Connection Request.");
    msgBox.setInformativeText("Do you want to connect to the internet?");
    msgBox.setStandardButtons(QMessageBox::Yes |  QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Yes) {
        //      sessionAgent->requestConnect(); //connect to this session
        ua->sendConnectReply(QLatin1String("Clear"), 0);
    } else {
        ua->sendConnectReply(QLatin1String("Supress"), 15);
    }
}

void Window::connmanAvailableChanged(bool b)
{
    qDebug() << Q_FUNC_INFO << b;
    connmanAvailable = b;
    if (b) {
        this->setToolTip(connman->state());
        updateTree();
    }
}

void Window::connmanUnregistered(const QString &/*something*/)
{
    connmanAvailable = false;

    disconnect(connman,SIGNAL(stateChanged(const QString &)),
               this,SLOT(connmanStateChanged(const QString &)));

    disconnect(ua,SIGNAL(userInputRequested(QString,QVariantList)),
               this,SLOT(userInputRequested(QString,QVariantList)));

    disconnect(ua,SIGNAL(userConnectRequested(QDBusMessage)),
               this,SLOT(requestConnect(QDBusMessage)));
}

void Window::techChanged()
{
    updateTree();
    qDebug() << Q_FUNC_INFO;
}

void Window::servicesChanged()
{
    qDebug() << Q_FUNC_INFO;
    updateTree();
}

void Window::connectRequestFailed(const QString &error)
{
    trayIcon->showMessage("QConnman connection error", error/*, QSystemTrayIcon::Information, 4000*/);
}

void Window::connectToConnman(QString)
{
    connmanAvailable = true;
    initNetworkManager();
    updateTree();
}

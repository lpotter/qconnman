#include "qconnman.h"
#include "qconnmanservice_linux_p.h"

QConnman::QConnman(QObject *parent) :
    QObject(parent)
{
}

void QConnman::getInfo()
{
    QConnmanInterface *connman = new QConnmanInterface();
    if(!connman->isValid()) {
        qWarning() << connman->lastError().message();
        emit done();
        return;
    }
    connect(connman, SIGNAL(stateChanged(QString)), this,SLOT(stateChanged(QString)));
    connect(connman,SIGNAL(propertyChanged(QString,QVariant&)), this,SLOT(propertyChanged(QString,QVariant&)));

    qWarning() << "State" << connman->getState();
    qWarning() << "available technologies" << connman->getAvailableTechnologies();
    qWarning() << "enabled technologies" << connman->getEnabledTechnologies();
    qWarning() << "connected technologies" << connman->getConnectedTechnologies();

    qWarning() << "default technology" << connman->getDefaultTechnology();

    qWarning() << "active profile" << connman->getActiveProfile();
    qWarning() << "technologies" << connman->getTechnologies();
    qWarning() << "services" << connman->getServices();
    qWarning() << "profiles" << connman->getProfiles();

    if(!connman->getEnabledTechnologies().contains("wifi")
        && connman->enableTechnology("wifi")) {
        qWarning() << "enabled technologies" << connman->getEnabledTechnologies();

    }

    QString wifiPath = connman->getPathForTechnology("/org/moblin/connman/technology/wifi");
    QConnmanTechnologyInterface techInterface(wifiPath, this);

    QStringList devices = techInterface.getDevices();
    qWarning() <<"devices" << devices;

    qWarning() << techInterface.getName() << techInterface.getState();
//    qWarning() << techInterface.getType();

    if(!devices.isEmpty()) {
        QConnmanDeviceInterface devInterface(devices.at(0), this);

         qWarning() << "wifi is powered" << devInterface.isPowered();

        connman->requestScan("wifi");

        // qWarning() << devInterface.getNetworks();
        foreach(const QString &net, devInterface.getNetworks()) {
            QConnmanNetworkInterface netInterface(net);
            qWarning() <<"SSID"
                    << netInterface.getWifiSsid()
                    << netInterface.getSignalStrength()
                    << netInterface.getWifiSecurity()
                    << netInterface.isConnected();
        }
    }

//    emit done();
}

void QConnman::stateChanged(const QString &state)
{
    qWarning() << Q_FUNC_INFO;
}


void QConnman::propertyChanged(const QString &property, QVariant &var)
{
    qWarning() << Q_FUNC_INFO;

}

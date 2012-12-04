#include "qconnman.h"
#include "qconnmanservice_linux_p.h"

QConnman::QConnman(QObject *parent) :
    QObject(parent)
{
}

void QConnman::getInfo()
{
    QConnmanManagerInterface *connman = new QConnmanManagerInterface();
    if(!connman->isValid()) {
        qWarning() << connman->lastError().message();
        emit done();
        return;
    }
    connect(connman, SIGNAL(stateChanged(QString)),this,SLOT(stateChanged(QString)));
    connect(connman,SIGNAL(propertyChanged(QString,QVariant&)),this,SLOT(propertyChanged(QString,QVariant&)));

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

    QString wifiPath = connman->getPathForTechnology("/net/connman/technology/wifi");
    QConnmanTechnologyInterface techInterface(wifiPath, this);

    qWarning() << techInterface.getName() << techInterface.getState();
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

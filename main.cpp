#include <QtCore/QCoreApplication>
#include <QDBusConnection>
#include <QTimer>
#include "qconnman.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QConnman qcon;
    QTimer::singleShot(0, &qcon, SLOT(getInfo()));
    QObject::connect(&qcon, SIGNAL(done()), &a, SLOT(quit()));
    return a.exec();
}

//#include <QtCore/QCoreApplication>
//#include <QDBusConnection>
//#include <QTimer>
//#include "qconnman.h"
//
//int main(int argc, char *argv[])
//{
//    QCoreApplication a(argc, argv);
//    QConnman qcon;
//    QTimer::singleShot(0, &qcon, SLOT(getInfo()));
//    QObject::connect(&qcon, SIGNAL(done()), &a, SLOT(quit()));
//    return a.exec();
//}
//
//****************************************************************************/

#include <QtGui/QtGui>
#include "window.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(systray);

    QApplication app(argc, argv);

    QApplication::setQuitOnLastWindowClosed(false);

    Window window;
    //window.show();
    return app.exec();
}

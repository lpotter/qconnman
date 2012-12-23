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

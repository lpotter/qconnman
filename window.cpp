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
#include "qconnman.h"

Window::Window()
{
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
            QConnman *conn = new QConnman(this);
            conn->getInfo();
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
//    minimizeAction = new QAction(tr("General"), this);
//   // connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));
//
//    maximizeAction = new QAction(tr("Network"), this);
//   // connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));
//
//    restoreAction = new QAction(tr("Device"), this);
//    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("Quit"), this);
   connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void Window::createTrayIcon()
{
    trayIconMenu = new QMenu(0);
//    trayIconMenu->addAction(minimizeAction);
//    trayIconMenu->addAction(maximizeAction);
    //trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
//    trayIcon = new QSystemTrayIcon(this);
    this->setContextMenu(trayIconMenu);
}

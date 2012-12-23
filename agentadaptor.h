/****************************************************************************
**
** Copyright (C) 2012 Lorn Potter
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSensors module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef AGENTADAPTOR_H
#define AGENTADAPTOR_H

#include <QDBusAbstractAdaptor>
#include <QObject>
#include "qconnmanservice_linux_p.h"

struct ServiceRequestData
{
    QString objectPath;
    QVariantMap fields;
    QDBusMessage reply;
    QDBusMessage msg;
};

class UserAgent : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(UserAgent)

public:
    explicit UserAgent(QConnmanManagerInterface *manager,QObject* parent = 0);
    virtual ~UserAgent();

    Q_INVOKABLE void sendUserReply(const QVariantMap &input);

signals:
    void userInputRequested(const QString &servicePath, const QVariantList &fields);
    void userInputCanceled();
    void errorReported(const QString &error);

private slots:
    void updateMgrAvailability(bool &available);

private:
    void requestUserInput(ServiceRequestData* data);
    void cancelUserInput();
    void reportError(const QString &error);

    ServiceRequestData* m_req_data;
    QConnmanManagerInterface* m_manager;

    friend class AgentAdaptor;
};


class AgentAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "net.connman.Agent")

 public:
    explicit AgentAdaptor(UserAgent* parent);
     virtual ~AgentAdaptor();

 public slots:
     void Release();
     void ReportError(const QDBusObjectPath &service_path, const QString &error);
     void RequestBrowser(const QDBusObjectPath &service_path, const QString &url);
     Q_NOREPLY void RequestInput(const QDBusObjectPath &service_path,
                                 const QVariantMap &fields,
                                 const QDBusMessage &message);
     void Cancel();

 private:
     UserAgent* m_userAgent;
};

#endif // AGENTADAPTOR_H

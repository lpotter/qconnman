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

#include "agentadaptor.h"


UserAgent::UserAgent(QConnmanManagerInterface *manager,QObject* parent) :
    QObject(parent),
    m_req_data(NULL),
    m_manager(manager)
{
    qDebug() << Q_FUNC_INFO;
    new AgentAdaptor(this);
    if (!QDBusConnection::systemBus().registerObject("/ConnmanAgent", this))
        qDebug() << "Not registered agent";

    QDBusObjectPath obpath("/ConnmanAgent");
    m_manager->unregisterAgent(obpath);
    m_manager->registerAgent(obpath);

//    connect(m_manager, SIGNAL(availabilityChanged(bool)),
//            this, SLOT(updateMgrAvailability(bool)));
}

UserAgent::~UserAgent()
{
    QDBusObjectPath obpath("/ConnmanAgent");
    m_manager->unregisterAgent(obpath);
}

void UserAgent::requestUserInput(ServiceRequestData* data)
{
    m_req_data = data;
    QVariantList fields;

    foreach (const QString &key, data->fields.keys()) {
        QVariantMap field;

        field.insert("name", key);
        field.insert("type", data->fields.value(key).toMap().value("Type"));
        fields.append(QVariant(field));
    }

    emit userInputRequested(data->objectPath, fields);
}

void UserAgent::cancelUserInput()
{
    delete m_req_data;
    m_req_data = NULL;
    emit userInputCanceled();
}

void UserAgent::reportError(const QString &error) {
    emit errorReported(error);
}

void UserAgent::sendUserReply(const QVariantMap &input) {
    if (m_req_data == NULL) {
        qWarning("Got reply for non-existing request");
        return;
    }

    if (!input.isEmpty()) {
        QDBusMessage &reply = m_req_data->reply;
        reply << input;
        QDBusConnection::systemBus().send(reply);
    } else {
        QDBusMessage error = m_req_data->msg.createErrorReply(
                    QString("net.connman.Agent.Error.Canceled"),
                    QString("canceled by user"));
        QDBusConnection::systemBus().send(error);
    }
    delete m_req_data;
    m_req_data = NULL;
}

void UserAgent::updateMgrAvailability(bool &available)
{
    if (available) {
        QDBusObjectPath obpath("/ConnmanAgent");
        m_manager->registerAgent(obpath);
    }
}


AgentAdaptor::AgentAdaptor(UserAgent* parent)
    : QDBusAbstractAdaptor(parent),
      m_userAgent(parent)
{
}

AgentAdaptor::~AgentAdaptor()
{
}

void AgentAdaptor::Release()
{
}

void AgentAdaptor::ReportError(const QDBusObjectPath &service_path, const QString &error)
{
    qDebug() << "From " << service_path.path() << " got this error:\n" << error;
    m_userAgent->reportError(error);
}

void AgentAdaptor::RequestBrowser(const QDBusObjectPath &service_path, const QString &url)
{
    qDebug() << "Service " << service_path.path() << " wants browser to open hotspot's url " << url;
}

void AgentAdaptor::RequestInput(const QDBusObjectPath &service_path,
                                const QVariantMap &fields,
                                const QDBusMessage &message)
{
    qDebug() << "Service " << service_path.path() << " wants user input";

    QVariantMap json;
    foreach (const QString &key, fields.keys()){
        QVariantMap payload = qdbus_cast<QVariantMap>(fields[key]);
        json.insert(key, payload);
    }

    message.setDelayedReply(true);

    ServiceRequestData *reqdata = new ServiceRequestData;
    reqdata->objectPath = service_path.path();
    reqdata->fields = json;
    reqdata->reply = message.createReply();
    reqdata->msg = message;

    m_userAgent->requestUserInput(reqdata);
}

void AgentAdaptor::Cancel()
{
    qDebug() << "WARNING: request to agent got canceled";
    m_userAgent->cancelUserInput();
}

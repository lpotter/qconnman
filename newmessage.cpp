/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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


#include "newmessage.h"

NewMessage::NewMessage(QWidget *parent) :
    QDialog(parent)
{
    ui.setupUi(this);
    connect(ui.replyPushButton,SIGNAL(clicked()),this,SLOT(replyClicked()));
}

QString NewMessage::getAddress()
{
    return ui.fromLineEdit->text();
}

QString NewMessage::getTimestamp()
{
    return ui.timeSentLabel->text();
}

QString NewMessage::getBody()
{
    return ui.messageTextEdit->toPlainText();
}

void NewMessage::replyClicked()
{
    Q_EMIT replyTo(toAddress);
    this->accept();
}

void NewMessage::setAddress(const QString &ad)
{
    toAddress = ad;
    ui.fromLineEdit->setText(toAddress);
}

void NewMessage::setTimestamp(const QString &ts)
{
    timestamp = ts;
    ui.timeSentLabel->setText(timestamp);
}

void NewMessage::setBody(const QString &bd)
{
    message = bd;
    ui.messageTextEdit->setText(message);
}

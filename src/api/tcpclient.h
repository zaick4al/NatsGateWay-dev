// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef CLIENT_H
#define CLIENT_H

#include <QDataStream>
#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include "singletones/settings.h"

namespace WebApi
{
class TcpClient : public QObject
{
    Q_OBJECT

public:
    explicit TcpClient(QObject *parent = nullptr);

    QHash<QString, quint16> servicesPorts() const;
    void setServicesPorts(const QHash<QString, quint16> &newServicesPorts);

    QHash<QString, QString> servicesUrls() const;
    void setServicesUrls(const QHash<QString, QString> &newServicesUrls);

public slots:
    void onNatsMessage(QString &p_uuid, QByteArray &p_messageText);
    void onAccessPointConnected(const QString &uuid, const QString &service);
    void onAccessPointDisconnected(const QString &uuid);
    void onNatsDisconnected();
signals:
    void tcpMessageReceived(QString &uuid, QByteArray &messageData);
private slots:
    void displayError(QAbstractSocket::SocketError socketError);
    void onTcpMessage();
    void onConnectedSocket();
    void onDisconnectedSocket();
private:
    Singletones::Settings* m_settings;
    QHash<QTcpSocket*, QString> m_sockets;
    QHash<QString, quint16> m_servicesPorts;
    QHash<QString, QString> m_servicesUrls;
    QByteArray m_message;
};
}

#endif

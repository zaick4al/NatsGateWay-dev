#ifndef NATSHANDLER_H
#define NATSHANDLER_H

#include <QObject>
#include <QTimer>
#include <qtnats.h>
#include "singletones/settings.h"
#include "QJsonDocument"
#include "QJsonObject"

namespace WebApi
{
class NatsHandler : public QObject
{
    Q_OBJECT
public:
    NatsHandler();

public slots:
    void connectToServer();
    void disconnectFromServer();
    void onTcpMessage(const QString &uuid, const QByteArray &messageText);
signals:
    void messageReceived(QString &uuid, QByteArray &msg);
    void errorThrown(QString errortxt);
    void connectedClient(QString &clientId, QString &service);
    void disconnectedClient(QString &clientId);
    void gwUpdateReceived(QJsonObject &obj);
    void natsDisconnected();
    void debugSignal(QString &data);
protected:
    void publish(const QtNats::Message &message);

private slots:
    void onMessage(const QtNats::Message &message);
    void OnQueueMessage(const QtNats::Message &message);
    void onConnect();
    void onError(natsStatus error, const QString& text);

private:
    QtNats::Subscription *m_listenSubscription;
    QtNats::Subscription *m_queueGroupSubscription;
    QtNats::Client m_client;
    QList<QUrl> m_natsIps;
    int m_currentNatsSrv;
};
}

#endif // NATSHANDLER_H

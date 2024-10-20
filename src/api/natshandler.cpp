#include "natshandler.h"

namespace WebApi
{
NatsHandler::NatsHandler() : m_currentNatsSrv(-1)
{
    QObject::connect(&m_client, &QtNats::Client::statusChanged, this, &NatsHandler::onConnect);
    QObject::connect(&m_client, &QtNats::Client::errorOccurred, this, &NatsHandler::onError);
    m_natsIps = Singletones::Settings::instance().natsServerIps();
    connectToServer();
}

void NatsHandler::connectToServer()
{
    QString debugData = QString("\n") + "Nats is trying to connect" + QString("\n") + "to" + QString("\n");
    for(QUrl url : m_natsIps)
        debugData += url.toString() + QString("\n");
    debugData += QString("\n");
    emit debugSignal(debugData);
    QtNats::Options connectOpts;
    connectOpts.servers = m_natsIps;
    try{
        m_client.connectToServer(connectOpts);
    }
    catch(QtNats::Exception &except)
    {
        QString debugData = except.what();
        emit debugSignal(debugData);
        QTimer::singleShot(10000, this, &NatsHandler::connectToServer);
    }
}

void NatsHandler::disconnectFromServer()
{
    m_client.disconnect();
}

void NatsHandler::onTcpMessage(const QString &uuid, const QByteArray &messageText)
{
    QJsonObject mainobj;
    mainobj.insert("uuid", uuid);
    mainobj.insert("content", QString::fromUtf8(messageText));
    QtNats::Message messageFromTcp;
    messageFromTcp.subject = "bar";
    messageFromTcp.data = QJsonDocument(mainobj).toJson(QJsonDocument::Compact);
    publish(messageFromTcp);
}

void NatsHandler::publish(const QtNats::Message &message)
{
    try{
        m_client.publish(message);
    }
    catch(QtNats::Exception &except){
        QString debugData = except.what();
        emit debugSignal(debugData);
    }
}
void NatsHandler::onMessage(const QtNats::Message &message)
{
    QByteArray msg = message.data;
    QByteArray msg2 = QByteArray::fromBase64(msg, QByteArray::Base64Encoding);
    if(msg.contains("content"))
    {
        QJsonObject uuid = QJsonDocument::fromJson(msg).object();
        QString uuidDecoded = uuid.value("uuid").toString();
        QByteArray contentsDecoded = uuid.value("content").toString().toUtf8();
        emit messageReceived(uuidDecoded, contentsDecoded);
    }
    else if(msg.contains("gwUpd"))
    {
        QJsonObject updateObj = QJsonDocument::fromJson(msg).object();
        emit gwUpdateReceived(updateObj);
    }
    else if(msg.contains("client disconnected"))
    {
        msg.replace("client disconnected:", "");
        QString client = QString::fromUtf8(msg);
        emit disconnectedClient(client);
    }
}

void NatsHandler::OnQueueMessage(const QtNats::Message &message)
{
    QByteArray msg = message.data;
    QByteArray msg2 = QByteArray::fromBase64(msg, QByteArray::Base64Encoding);
    if(msg.contains("client connection"))
    {
        msg.replace("client connection:", "");
        QString msgTostr = msg;
        QStringList msgToStrL = msgTostr.split("/");
        emit connectedClient(msgToStrL[0], msgToStrL[1]);
    }
    else if(msg.contains("accessPointRequest"))
    {
        QJsonObject jobj = Singletones::Settings::instance().jsonJsonData();
        jobj.insert("acpConfig", "");
        QJsonDocument doc(jobj);
        QtNats::Message accessPointAnswer;
        accessPointAnswer.subject = "bar";
        accessPointAnswer.data = doc.toJson();
        publish(accessPointAnswer);
    }
}

void NatsHandler::onConnect()
{
    if(m_client.status() != QtNats::ConnectionStatus::Connected)
    {
        QString debugData = "Not connected";
        emit debugSignal(debugData);
        if(m_client.status() == QtNats::ConnectionStatus::Disconnected  ||
            m_client.status() == QtNats::ConnectionStatus::Closed ||
            m_client.status() == QtNats::ConnectionStatus::Reconnecting)
            emit natsDisconnected();
        return;
    }
    m_listenSubscription = m_client.subscribe("foo");
    m_queueGroupSubscription = m_client.subscribe("foobar", "job.workers");
    QObject::connect(m_listenSubscription, &QtNats::Subscription::received, this, &NatsHandler::onMessage);
    QObject::connect(m_queueGroupSubscription, &QtNats::Subscription::received, this, &NatsHandler::OnQueueMessage);
    QJsonObject jobj = Singletones::Settings::instance().jsonJsonData();
    jobj.insert("gwUpd", "");
    QJsonDocument doc(jobj);
    QtNats::Message sendToGws;
    QtNats::Message sendToAPs;
    sendToGws.subject = "foo";
    sendToAPs.subject = "bar";
    sendToAPs.data = doc.toJson();
    sendToGws.data = sendToAPs.data;
    publish(sendToGws);
    publish(sendToAPs);
    QString debugData = "Nats Connected!";
    emit debugSignal(debugData);
}

void NatsHandler::onError(natsStatus error, const QString &text)
{
    qDebug() << error;
    emit errorThrown(text);
}

}

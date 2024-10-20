#include "apihandler.h"

namespace Api {

ApiHandler::ApiHandler(QObject *parent)
    : QObject{parent}
{
    m_settings = &Singletones::Settings::instance();
    m_tcpClient = new WebApi::TcpClient();
    m_natsHandler = new WebApi::NatsHandler();
    connect(m_natsHandler, &WebApi::NatsHandler::debugSignal, this, &ApiHandler::natsDebug);
    connect(m_natsHandler, &WebApi::NatsHandler::connectedClient, m_tcpClient, &WebApi::TcpClient::onAccessPointConnected);
    connect(m_natsHandler, &WebApi::NatsHandler::disconnectedClient, m_tcpClient, &WebApi::TcpClient::onAccessPointDisconnected);
    connect(m_tcpClient, &WebApi::TcpClient::tcpMessageReceived, m_natsHandler, &WebApi::NatsHandler::onTcpMessage);
    connect(m_natsHandler, &WebApi::NatsHandler::messageReceived, m_tcpClient, &WebApi::TcpClient::onNatsMessage);
    connect(m_natsHandler, &WebApi::NatsHandler::natsDisconnected, m_tcpClient, &WebApi::TcpClient::onNatsDisconnected);
    connect(m_natsHandler, &WebApi::NatsHandler::gwUpdateReceived, this, &ApiHandler::updateSettings);
}

void ApiHandler::reconnect()
{
    m_natsHandler->connectToServer();
}

void ApiHandler::updateSettings(QJsonObject newSettings)
{
    qDebug() << "update received!";
    m_settings->updateSettings(newSettings);
}

} // namespace Api

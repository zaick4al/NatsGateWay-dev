#include "tcpclient.h"

namespace WebApi {

    TcpClient::TcpClient(QObject *parent)
        : QObject{parent}
    {
        m_settings = &Singletones::Settings::instance();
        setServicesPorts(m_settings->servicesPorts());
        setServicesUrls(m_settings->serviceUrls());
    }

    void TcpClient::displayError(QAbstractSocket::SocketError socketError)
    {
        switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
            break;
        case QAbstractSocket::HostNotFoundError:
            qDebug() << "The host was not found. Please check the host name and port settings.";
            break;
        case QAbstractSocket::ConnectionRefusedError:
            qDebug() << "The connection was refused by the peer. "
                        "Make sure the server is running, "
                        "and check that the host name and port "
                        "settings are correct.";
            break;
        default:
            qDebug() << "The following error occurred: " << socketError;
        }
    }

    void TcpClient::onTcpMessage()
    {
        QTcpSocket* connection = qobject_cast<QTcpSocket*>(this->sender());
        QString uuid;
        QList<QTcpSocket*> socketsKeys = m_sockets.keys();
        QList<QTcpSocket*>::Iterator iterator = std::find_if(socketsKeys.begin(), socketsKeys.end(), [=](const QTcpSocket* socket){
            return (socket == connection);
        });
        int index = iterator - socketsKeys.begin();
        uuid = m_sockets.value(socketsKeys.value(index));
        QStringList data;
        while (true) {
            QByteArray message = connection->read(1024).toBase64(QByteArray::Base64Encoding);
            if (message.isEmpty())
                break;
            emit tcpMessageReceived(uuid, message);
        }
    }

    void TcpClient::onConnectedSocket()
    {
        QTcpSocket* tcpSocket = qobject_cast<QTcpSocket*>(this->sender());
        for(const QTcpSocket *socket : m_sockets.keys())
        {
            if(socket == tcpSocket)
                qDebug() << "SOCKET CONNECTED:" << m_sockets.value(tcpSocket) << socket->peerPort();
        }
    }

    void TcpClient::onDisconnectedSocket()
    {
        QTcpSocket* tcpSocket = qobject_cast<QTcpSocket*>(this->sender());
        for(const QTcpSocket *socket : m_sockets.keys())
        {
            if(socket == tcpSocket)
                qDebug() << "SOCKET DISCONNECTED:" << m_sockets.value(tcpSocket) << socket->peerPort();
        }
    }

    QHash<QString, QString> TcpClient::servicesUrls() const
    {
        return m_servicesUrls;
    }

    void TcpClient::setServicesUrls(const QHash<QString, QString> &newServicesUrls)
    {
        m_servicesUrls = newServicesUrls;
    }

    QHash<QString, quint16> TcpClient::servicesPorts() const
    {
        return m_servicesPorts;
    }

    void TcpClient::setServicesPorts(const QHash<QString, quint16> &newServicesPorts)
    {
        m_servicesPorts = newServicesPorts;
    }

    void TcpClient::onNatsMessage(QString &p_uuid, QByteArray &p_messageText)
    {
        QByteArray messageDecoded = QByteArray::fromBase64(p_messageText, QByteArray::Base64Encoding);
        for(QTcpSocket *socket : m_sockets.keys())
        {
            if(m_sockets.value(socket) == p_uuid)
            {
                if(socket->state() != QAbstractSocket::ConnectedState)
                {
                    qDebug() << "SOCKET NOT CONNECTED" << socket->peerPort();
                    socket->write(messageDecoded);
                    return;
                }
                socket->write(messageDecoded);
                return;
            }
        }
    }

    void TcpClient::onAccessPointConnected(const QString &uuid, const QString &service)
    {
        QUrl serverIp = m_servicesUrls.value(service);
        quint16 serverPort = m_servicesPorts.value(service);
        QTcpSocket* tcpSocket = new QTcpSocket(this);
        connect(tcpSocket, &QIODevice::readyRead, this, &TcpClient::onTcpMessage);
        connect(tcpSocket, &QAbstractSocket::errorOccurred,
                this, &TcpClient::displayError);
        m_sockets.insert(tcpSocket, uuid);
        connect(tcpSocket, &QTcpSocket::connected, this, &TcpClient::onConnectedSocket);
        connect(tcpSocket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnectedSocket);
        tcpSocket->connectToHost(serverIp.toString(), serverPort);
    }

    void TcpClient::onAccessPointDisconnected(const QString &uuid)
    {
        QList<QTcpSocket*> socketsList = m_sockets.keys();
        QList<QTcpSocket*>::Iterator socketIterator = std::find_if(socketsList.begin(), socketsList.end(), [=](QTcpSocket* currentSocket){return m_sockets.value(currentSocket) == uuid;});
        if((socketIterator - socketsList.begin()) > socketsList.length())
            return;
        int index = socketIterator - socketsList.begin();
        QTcpSocket* socket = socketsList.value(index);
        if(!socket)
            return;
        socket->disconnect();
        m_sockets.remove(socket);
        socket->close();
        socket->deleteLater();
        qDebug() << "connected sockets:" << m_sockets.count() << "sockets:" << m_sockets.values();
    }

    void TcpClient::onNatsDisconnected()
    {
        for(QTcpSocket *sock : m_sockets.keys())
        {
            m_sockets.remove(sock);
            sock->deleteLater();
        }
        qDebug() << "purged sockets, remaining:" << m_sockets.count();
    }

} // namespace WebApi

#ifndef API_APIHANDLER_H
#define API_APIHANDLER_H

#include <QObject>
#include "api/natshandler.h"
#include "api/tcpclient.h"

namespace Api {

class ApiHandler : public QObject
{
    Q_OBJECT
public:
    explicit ApiHandler(QObject *parent = nullptr);

public slots:
    void reconnect();
signals:
    void natsDebug(QString &data);
private:
    Singletones::Settings* m_settings;
    WebApi::NatsHandler *m_natsHandler;
    WebApi::TcpClient *m_tcpClient;
private slots:
    void updateSettings(QJsonObject newSettings);
};

} // namespace Api

#endif // API_APIHANDLER_H

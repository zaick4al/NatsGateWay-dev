#ifndef SETTINGS_H
#define SETTINGS_H

#include "qurl.h"
#include <QObject>
#include <QSettings>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDateTime>
#include <QFileInfo>

namespace Singletones
{
class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = nullptr);

    void updateSettings();
    void updateSettings(QJsonObject obj);

    static Settings &instance();

    QList<QUrl> natsServerIps() const;
    void setNatsServerIps(const QList<QUrl> &newNatsServerIp);
    void setNatsServerIps(const QString &p_ip);

    QPair<qint64, QHash<QString, QString> > jsonData() const;
    void setJsonData(const QPair<qint64, QHash<QString, QString> > &newJsonData);

    QHash<QString, quint16> servicesPorts() const;

    QJsonObject jsonJsonData() const;

    QHash<QString, QString> serviceUrls() const;
    void setServiceUrls(const QHash<QString, QString> &newServiceUrls);

    qint64 date() const;
    void setDate(const qint64 &newDate);

signals:
    void jsonConfigReceived(QHash<QString, quint16> servicesPorts);
    void ready();

private:
    QSettings* m_settings;
    QHash<QString, QString> m_currentSettings;
    QHash<QString, quint16> m_portsPairs;
    QHash<QString, QString> m_serviceUrls;
    QPair<quint64, QHash<QString, QString>> m_jsonData;
    qint64 m_date = 0;
    QList<QUrl> m_natsServerIps;
};
}
#endif // SETTINGS_H

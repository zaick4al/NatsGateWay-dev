#include "settings.h"

namespace Singletones
{
    Settings::Settings(QObject *parent)
        : QObject{parent}
    {
        const QString settingsFile = QCoreApplication::applicationDirPath() + "/configs/" + "NatsGateWay.ini";
        m_settings = new QSettings(settingsFile, QSettings::IniFormat, this);

        if(m_settings->value("Nats/ServerIp").toString().isEmpty())
        {
            QString newNatsIp("192.168.102.1:4222, 192.168.102.2:4222");
            m_settings->setValue("Nats/ServerIp", newNatsIp);
        }
        setNatsServerIps(m_settings->value("Nats/ServerIp").toString());
        updateSettings();
    }

    void Settings::updateSettings()
    {
        QFile file;
        file.setFileName(QCoreApplication::applicationDirPath() + "/configs/" + "NatsServices.json");
        if(!file.exists())
        {
            qDebug() << "file doesn't exist!";
            file.open(QFile::NewOnly);
            QJsonObject jobj;
            QJsonObject internalJobj;
            QJsonObject services;
            services.insert("sourcePort", 8010);
            services.insert("destinationPort", 8010);
            services.insert("destinationUrl", "192.168.102.12");
            internalJobj.insert("serviceHttp", services);
            services.insert("sourcePort", 8080);
            services.insert("destinationPort", 8080);
            services.insert("destinationUrl", "192.168.102.12");
            internalJobj.insert("serviceWs", services);
            jobj.insert("services", internalJobj);
            QJsonDocument docObj(jobj);
            QByteArray jsonArr = docObj.toJson();
            QTextStream out(&file);
            out << jsonArr;
            file.close();
            qint64 currTime = 0;
            setDate(currTime);
        }
        else
        {
            QFileInfo info(file.fileName());
            file.open(QFile::ReadOnly);
            qint64 lastUpdated = info.lastModified().toSecsSinceEpoch();
            QByteArray data = file.readAll();
            QJsonObject docObj = QJsonDocument::fromJson(data).object();
            docObj.insert("date", lastUpdated);
            updateSettings(docObj);
        }
    }

    void Settings::updateSettings(QJsonObject mainObj)
    {
        qint64 newDate = mainObj.value("date").toInt();
        qDebug() << mainObj;
        QHash<QString, QString> servicesHash;
        if(newDate >= date())
        {
            bool updated = false;
            if(newDate > date())
                updated = true;
            qDebug() << newDate;
            qDebug() << date();
            qDebug() << updated;
            setDate(newDate);
            QJsonObject jobj = mainObj.value("services").toObject();
            for(const QString &key : jobj.keys())
            {
                QJsonObject internalJobj = jobj.value(key).toObject();
                QVariant srcPort = internalJobj.value("sourcePort");
                QVariant destPort = internalJobj.value("destinationPort");
                QVariant destUrl = internalJobj.value("destinationUrl");
                servicesHash.insert(key, QString::number(srcPort.toInt()));
                m_portsPairs.insert(key, destPort.toInt());
                m_serviceUrls.insert(key, destUrl.toString());
            }
            QPair<qint64, QHash<QString, QString>> pair;
            pair.first = newDate;
            pair.second = servicesHash;
            setJsonData(pair);
            emit jsonConfigReceived(m_portsPairs);
            if(updated)
            {
                qDebug() << "updated!!";
                QFile file;
                file.setFileName(m_settings->fileName().replace("NatsGateWay", "NatsServices").replace(".ini", ".json"));
                file.open(QFile::WriteOnly);
                file.resize(0);
                QByteArray data = QJsonDocument(jsonJsonData()).toJson();
                file.write(data);
                file.close();
            }
        }
        emit ready();
    }

    Settings &Settings::instance()
    {
        static Settings SettingsInstance;
        return SettingsInstance;
    }

    QList<QUrl> Settings::natsServerIps() const
    {
        return m_natsServerIps;
    }

    void Settings::setNatsServerIps(const QList<QUrl> &newNatsServerIp)
    {
        if(m_natsServerIps == newNatsServerIp)
            return;
        m_natsServerIps = newNatsServerIp;
        QString newSettingsVal = "";
        for(QUrl url : m_natsServerIps)
        {
            QString comma = ",";
            if(newSettingsVal.isEmpty())
                comma = "";
            newSettingsVal = newSettingsVal + comma + url.toString();
        }
        m_settings->setValue("Nats/ServerIp", newSettingsVal);
    }

    void Settings::setNatsServerIps(const QString &p_ip)
    {
        QList<QUrl> urlList;
        QStringList ips = p_ip.split(QRegularExpression("\\s*,\\s*"), Qt::SkipEmptyParts);
        for(const QString &urlStr : ips)
            urlList << urlStr;
        qDebug() << urlList;
        qDebug() << p_ip;
        setNatsServerIps(urlList);
    }

    QPair<qint64, QHash<QString, QString> > Settings::jsonData() const
    {
        return m_jsonData;
    }

    void Settings::setJsonData(const QPair<qint64, QHash<QString, QString> > &newJsonData)
    {
        m_jsonData = newJsonData;
    }

    QHash<QString, quint16> Settings::servicesPorts() const
    {
        return m_portsPairs;
    }

    QJsonObject Settings::jsonJsonData() const
    {
        QJsonObject retVal;
        QHash<QString, QString> serviceHash = jsonData().second;
        QHash<QString, quint16> ports = servicesPorts();
        QJsonObject services;
        for(const QString &key : serviceHash.keys())
        {
            QJsonObject jobj;
            quint16 destPort = ports.value(key);
            QString destUrl = serviceUrls().value(key);
            jobj.insert("sourcePort", serviceHash.value(key).toInt());
            jobj.insert("destinationPort", destPort);
            jobj.insert("destinationUrl", destUrl);
            services.insert(key, jobj);
        }
        retVal.insert("services", services);
        retVal["date"] = QJsonValue(jsonData().first);
        return retVal;
    }

    QHash<QString, QString> Settings::serviceUrls() const
    {
        return m_serviceUrls;
    }

    void Settings::setServiceUrls(const QHash<QString, QString> &newServiceUrls)
    {
        m_serviceUrls = newServiceUrls;
    }

    qint64 Settings::date() const
    {
        return m_date;
    }

    void Settings::setDate(const qint64 &newDate)
    {
        m_date = newDate;
    }

}

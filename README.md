# NatsGateWay

Для запуска и корректной работы системы необходимо создать файл NatsServices.json в папке %appdata%/Inbis/ текущего пользователя. Там же при первом запуске программы создастся файл NatsGateWay.ini, через который можно настроить подключение к NATS.io

#Структура Json настроек системы
```Json
{
  "services": {
    "serviceFirst": {
      "sourcePort": 8010,
      "destinationPort": 8010,
      "destinationUrl": "192.168.102.12"
    },
    "serviceSecond": {
      "sourcePort": 8080,
      "destinationPort": 8080,
      "destinationUrl": "192.168.102.12"
    },
    "serviceThird": {
      "sourcePort": 4040,
      "destinationPort": 443,
      "destinationUrl": "ya.ru"
    }
  },
  "date": "last edit date"
}
```

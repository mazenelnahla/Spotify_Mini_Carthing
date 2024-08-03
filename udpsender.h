#ifndef UDPSENDER_H
#define UDPSENDER_H

#include <QObject>
#include <QUdpSocket>
#include <QJsonDocument>
#include <QJsonObject>

class UDPSender : public QObject
{
    Q_OBJECT
public:
    explicit UDPSender(QObject *parent = nullptr);
    Q_INVOKABLE void sendPlayback(bool isPlay);
    Q_INVOKABLE void sendForward();
    Q_INVOKABLE void sendReverse();

private:
    QUdpSocket *udpSocket;
    QJsonObject commandObject;  // Object to hold commands

    void sendCommand();  // Private function to send commands

};

#endif // UDPSENDER_H

#include "udpsender.h"
#include <QHostAddress>

UDPSender::UDPSender(QObject *parent) : QObject(parent)
{
    udpSocket = new QUdpSocket(this);
}

void UDPSender::sendPlayback(bool isPlay)
{
    if(isPlay){
        commandObject["pause"] = "pause";
        sendCommand();
    }else{
        commandObject["play"] = "play";
        sendCommand();
    }
}

void UDPSender::sendForward()
{
    commandObject["forward"] = true;
    sendCommand();
}

void UDPSender::sendReverse()
{
    commandObject["reverse"] = true;
    sendCommand();
}

void UDPSender::sendCommand()
{
    QJsonDocument jsonDoc(commandObject);
    QByteArray jsonData = jsonDoc.toJson();
    udpSocket->writeDatagram(jsonData, QHostAddress("127.0.0.1"), 12456); // Send to specific IP and port

    // Clear the command object after sending
    commandObject = QJsonObject();
}

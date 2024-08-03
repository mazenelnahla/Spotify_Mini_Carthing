#include "SpotifyReceiver.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>


SpotifyReceiver::SpotifyReceiver(QObject *parent) : QObject(parent)
{
    udpSocket.bind(QHostAddress::Any, 12356); // Adjust the port as needed

    connect(&udpSocket, &QUdpSocket::readyRead, this, &SpotifyReceiver::processPendingDatagrams);
}

void SpotifyReceiver::processPendingDatagrams() {
    while (udpSocket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket.pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        udpSocket.readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        // Debug the received raw datagram

        // Decode the received data as a JSON object
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(datagram, &jsonError);

        // Debug JSON parse errors
        if (jsonError.error != QJsonParseError::NoError) {
            qDebug() << "Error decoding JSON data:" << jsonError.errorString();
            continue;
        }

        if (jsonDoc.isObject()) {
            QJsonObject jsonObj = jsonDoc.object();
            QString Track_Name = jsonObj["trackName"].toString();
            QString Artist_Name = jsonObj["artistName"].toString();
            QString Album_Name = jsonObj["albumName"].toString();
            QString Album_Img_URL = jsonObj["albumURL"].toString();
            bool is_Playing = jsonObj["isPlaying"].toBool();
            double currentTime = jsonObj["currentTime"].toDouble(); // Use toDouble for precision
            double duration = jsonObj["duration"].toDouble();

            // Format time
            int minutes_currentTime = static_cast<int>(currentTime) / 60;
            int seconds_currentTime = static_cast<int>(currentTime) % 60;
            QString currentTimeformatted = QString("%1:%2").arg(minutes_currentTime, 2, 10, QChar('0')).arg(seconds_currentTime, 2, 10, QChar('0'));
            int minutes_duration = static_cast<int>(duration) / 60;
            int seconds_duration = static_cast<int>(duration) % 60;
            QString durationformatted = QString("%1:%2").arg(minutes_duration, 2, 10, QChar('0')).arg(seconds_duration, 2, 10, QChar('0'));

            // Emit data
            emit spotifiyReceivedData(Track_Name, Artist_Name, Album_Name, Album_Img_URL, is_Playing, currentTimeformatted, durationformatted, currentTime, duration);

        }
    }
}


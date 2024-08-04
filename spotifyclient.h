#ifndef SPOTIFYCLIENT_H
#define SPOTIFYCLIENT_H

#include <QObject>
#include <QJsonObject>
#include <QTcpServer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

class OAuthServer : public QTcpServer {
    Q_OBJECT
public:
    QString authCode;
protected:
    void incomingConnection(qintptr socketDescriptor) override;
};

class SpotifyClient : public QObject {
    Q_OBJECT
public:
    explicit SpotifyClient(QObject *parent = nullptr);
    Q_INVOKABLE void updateCurrentTrack();
    Q_INVOKABLE void stopUpdate();
    // Public slot so it can be called directly from QML or other C++ code
// public slots:
//     void updateCurrentTrack();

signals:
    // Signal to notify QML about Spotify data
    void spotifyReceivedData(const QString &trackName, const QString &artistName, const QString &albumName, const QString &albumImgUrl, bool isPlaying,double currentTime,double duration,QString currentTimeformatted,QString durationformatted);

private:
    QTimer *updateTimer;
    QString getAuthorizationCode();
    QJsonObject getAccessToken(const QString &authCode);
    QString readAccessToken();
    QString refreshAccessToken(const QString &refreshToken);
    QJsonObject getCurrentTrack(const QString &token);

    QNetworkAccessManager networkManager; // Ensure networkManager is a member if used in multiple methods

};

#endif // SPOTIFYCLIENT_H

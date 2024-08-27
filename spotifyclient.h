#ifndef SPOTIFYCLIENT_H
#define SPOTIFYCLIENT_H

#include <QObject>
#include <QJsonObject>
#include <QTcpServer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QNetworkConfigurationManager>

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
    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void nextTrack();
    Q_INVOKABLE void previousTrack();
    Q_INVOKABLE void checkNetworkConnectivity();
    Q_INVOKABLE void close();


signals:
    void spotifyReceivedData(const QString &trackName, const QString &artistName, const QString &albumName, const QString &albumImgUrl, bool isPlaying, double currentTime, double duration, QString currentTimeformatted, QString durationformatted);
    void isConnectedChanged(bool isConnected);
private:
    bool isConnected;
    QTimer *updateTimer;
    QTimer *networkCheckTimer;
    QString token;
    QString refreshToken;
    QString getAuthorizationCode();
    QJsonObject getAccessToken(const QString &authCode);
    QString readAccessToken();
    QString refreshAccessToken(const QString &refreshToken);
    QJsonObject getCurrentTrack(const QString &token);
    void saveTokens(const QString &accessToken, const QString &refreshToken);

    QNetworkAccessManager networkManager;
};

#endif // SPOTIFYCLIENT_H

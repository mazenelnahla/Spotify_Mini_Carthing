#include "spotifyclient.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUrl>
#include <QDateTime>
#include <QDebug>
#include <QUdpSocket>
#include <QCoreApplication>
#include <QProcess>
#include <QThread>
#include <QTimer>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
static const QString clientId = "3fb73f276ec048b78eff8151cee5563c";
static const QString clientSecret = "e8d229b5cc704e4d9c29bbd62957f93d";
static const QString redirectUri = "http://localhost:8888/callback";
static const QString scope = "user-read-currently-playing user-modify-playback-state user-read-playback-state";
static const QString tokenFilePath = QDir::current().filePath("spotify_tokens.json");

void openUrlInBrowser(const QString &url) {
#ifdef Q_OS_WIN
    // Windows
    QString program = "rundll32";
    QStringList arguments;
    arguments << "url.dll,FileProtocolHandler" << url;
    QProcess::startDetached(program, arguments);
#elif defined(Q_OS_MACOS)
    // macOS
    QProcess::startDetached("open", QStringList() << url);
#elif defined(Q_OS_LINUX)
    // Linux
    QProcess::startDetached("xdg-open", QStringList() << url);
#endif
}

void OAuthServer::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket *socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
        QByteArray data = socket->readAll();
        QString response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
                           "<html><body>"
                           "<script>window.close();</script>"
                           "<p>Authorization successful! You can close this window.</p>"
                           "</body></html>";
        socket->write(response.toUtf8());
        socket->flush();
        socket->waitForBytesWritten(3000);
        socket->close();
        QString request(data);
        int codeIndex = request.indexOf("code=");
        if (codeIndex != -1) {
            int startIndex = codeIndex + 5;
            int endIndex = request.indexOf(' ', startIndex);
            authCode = request.mid(startIndex, endIndex - startIndex);
            close();
        }
    });
}

SpotifyClient::SpotifyClient(QObject *parent) : QObject(parent)
{
    updateTimer = new QTimer(this);
    networkCheckTimer = new QTimer(this);

    // Check network connectivity every 5 seconds
    connect(networkCheckTimer, &QTimer::timeout, this, &SpotifyClient::checkNetworkConnectivity);
    networkCheckTimer->start(5000);
}

void SpotifyClient::checkNetworkConnectivity() {
    QNetworkConfigurationManager mgr;
    isConnected = mgr.isOnline();

    if (isConnected) {
        // qDebug() << "Network is connected";
        // Optionally: Restart track updates if the network was previously disconnected
        emit isConnectedChanged(isConnected);
        if (!updateTimer->isActive()) {
            updateCurrentTrack();
        }
    } else {
        // qDebug() << "Network is disconnected";
        // Stop track updates if the network is disconnected
        emit isConnectedChanged(isConnected);
        stopUpdate();
    }
}

QString SpotifyClient::getAuthorizationCode() {
    QString authUrl = QString("https://accounts.spotify.com/authorize?response_type=code&client_id=%1&scope=%2&redirect_uri=%3")
                          .arg(clientId, scope, redirectUri);

    // Open the URL in the default web browser
    openUrlInBrowser(authUrl);

    OAuthServer server;
    server.listen(QHostAddress::LocalHost, 8888);
    while (server.authCode.isEmpty()) {
        QCoreApplication::processEvents();
    }
    return server.authCode;
}

QJsonObject SpotifyClient::getAccessToken(const QString &authCode) {
    QNetworkRequest request(QUrl("https://accounts.spotify.com/api/token"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QByteArray data;
    data.append("grant_type=authorization_code&");
    data.append("code=" + QUrl::toPercentEncoding(authCode) + "&");
    data.append("redirect_uri=" + QUrl::toPercentEncoding(redirectUri) + "&");
    data.append("client_id=" + QUrl::toPercentEncoding(clientId) + "&");
    data.append("client_secret=" + QUrl::toPercentEncoding(clientSecret));

    QNetworkReply *reply = networkManager.post(request, data);
    while (!reply->isFinished()) {
        QCoreApplication::processEvents();
    }
    QByteArray response = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Error in getting access token:" << reply->errorString();
        reply->deleteLater();
        return QJsonObject();
    }
    QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
    QJsonObject tokenInfo = jsonResponse.object();
    tokenInfo["expires_at"] = QDateTime::currentSecsSinceEpoch() + tokenInfo["expires_in"].toInt();
    reply->deleteLater();

    saveTokens(tokenInfo["access_token"].toString(), tokenInfo["refresh_token"].toString());

    return tokenInfo;
}

void SpotifyClient::saveTokens(const QString &accessToken, const QString &refreshToken) {
    QFile file(tokenFilePath);

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open token file for writing.";
        return;
    }

    QJsonObject jsonObj;
    jsonObj["access_token"] = accessToken;
    jsonObj["refresh_token"] = refreshToken;
    jsonObj["expires_at"] = QDateTime::currentSecsSinceEpoch() + 3600; // Example: Token expires in 1 hour

    QJsonDocument jsonDoc(jsonObj);
    file.write(jsonDoc.toJson());
    file.close();
}

QString SpotifyClient::readAccessToken() {
    // qDebug() <<tokenFilePath;
    QFile file(tokenFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open token file for reading.";
        file.close();
        return QString();
    }
    QByteArray data = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qWarning() << "Invalid token file format.";
        return QString();
    }
    QJsonObject jsonObj = jsonDoc.object();
    token = jsonObj["access_token"].toString();
    refreshToken = jsonObj["refresh_token"].toString();
    qint64 expiresAt = jsonObj["expires_at"].toVariant().toLongLong();

    if (QDateTime::currentSecsSinceEpoch() > expiresAt) {
        token = refreshAccessToken(refreshToken);
    }
    return token;
}

QString SpotifyClient::refreshAccessToken(const QString &refreshToken) {
    QNetworkRequest request(QUrl("https://accounts.spotify.com/api/token"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QByteArray data;
    data.append("grant_type=refresh_token&");
    data.append("refresh_token=" + QUrl::toPercentEncoding(refreshToken) + "&");
    data.append("client_id=" + QUrl::toPercentEncoding(clientId) + "&");
    data.append("client_secret=" + QUrl::toPercentEncoding(clientSecret));

    QNetworkReply *reply = networkManager.post(request, data);
    while (!reply->isFinished()) {
        QCoreApplication::processEvents();
    }
    QByteArray response = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Error in refreshing access token:" << reply->errorString();
        reply->deleteLater();
        return QString();
    }
    QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
    QJsonObject tokenInfo = jsonResponse.object();
    tokenInfo["expires_at"] = QDateTime::currentSecsSinceEpoch() + tokenInfo["expires_in"].toInt();
    reply->deleteLater();

    saveTokens(tokenInfo["access_token"].toString(), tokenInfo["refresh_token"].toString());

    return tokenInfo["access_token"].toString();
}

QJsonObject SpotifyClient::getCurrentTrack(const QString &token) {
    QNetworkRequest request(QUrl("https://api.spotify.com/v1/me/player/currently-playing"));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());

    QNetworkReply *reply = networkManager.get(request);
    while (!reply->isFinished()) {
        QCoreApplication::processEvents();
    }
    QByteArray response = reply->readAll();
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Error in getting current track:" << reply->errorString();
        reply->deleteLater();
        return QJsonObject();
    }
    QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
    reply->deleteLater();
    return jsonResponse.object();
}

void SpotifyClient::updateCurrentTrack() {
    qDebug()<<tokenFilePath;
    connect(updateTimer, &QTimer::timeout, this, [=]() {
        QString token = readAccessToken();
        if (token.isEmpty()) {
            QString authCode = getAuthorizationCode();
            if (!authCode.isEmpty()) {
                QJsonObject tokenInfo = getAccessToken(authCode);
                token = tokenInfo["access_token"].toString();
            }
        }
        QJsonObject currentTrack = getCurrentTrack(token);
        QJsonObject jsonData;
        if (!currentTrack.isEmpty() && currentTrack.contains("item")) {
            QJsonObject trackInfo = currentTrack["item"].toObject();
            int durationMs = trackInfo["duration_ms"].toInt();
            int progressMs = currentTrack["progress_ms"].toInt();
            // Format time
            int minutes_currentTime = static_cast<int>(progressMs/1000) / 60;
            int seconds_currentTime = static_cast<int>(progressMs/1000) % 60;
            QString currentTimeformatted = QString("%1:%2").arg(minutes_currentTime, 2, 10, QChar('0')).arg(seconds_currentTime, 2, 10, QChar('0'));
            int minutes_duration = static_cast<int>(durationMs/1000) / 60;
            int seconds_duration = static_cast<int>(durationMs/1000) % 60;
            QString durationformatted = QString("%1:%2").arg(minutes_duration, 2, 10, QChar('0')).arg(seconds_duration, 2, 10, QChar('0'));
            jsonData = {
                {"trackName", trackInfo["name"].toString()},
                {"artistName", trackInfo["artists"].toArray().first().toObject()["name"].toString()},
                {"albumName", trackInfo["album"].toObject()["name"].toString()},
                {"albumURL", trackInfo["album"].toObject()["images"].toArray().first().toObject()["url"].toString()},
                {"isPlaying", currentTrack["is_playing"].toBool()},
                {"currentTime", progressMs},
                {"duration", durationMs}
            };

            // Emit the signal with the data
            emit spotifyReceivedData(
                trackInfo["name"].toString(),
                trackInfo["artists"].toArray().first().toObject()["name"].toString(),
                trackInfo["album"].toObject()["name"].toString(),
                trackInfo["album"].toObject()["images"].toArray().first().toObject()["url"].toString(),
                currentTrack["is_playing"].toBool(),
                progressMs,
                durationMs,
                currentTimeformatted,
                durationformatted
                );
        } else {
            jsonData = {
                {"trackName", ""},
                {"artistName", ""},
                {"albumName", ""},
                {"albumURL", ""},
                {"isPlaying", false},
                {"currentTime", 0},
                {"duration", 0}
            };

            // Emit the signal with no song information
            emit spotifyReceivedData("", "", "", "", false,0,0,"00:00","00:00");
        }
    });
    updateTimer->start(1000); // Update every 1 seconds, for example
}

void SpotifyClient::stopUpdate()
{
    if (updateTimer->isActive()) {
        updateTimer->stop();
    }
}

void SpotifyClient::pause() {
    QString token = readAccessToken();
    if (token.isEmpty()) return;
    QNetworkRequest request(QUrl("https://api.spotify.com/v1/me/player/pause"));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());

    QNetworkReply *reply = networkManager.put(request, QByteArray());
    while (!reply->isFinished()) {
        QCoreApplication::processEvents();
    }
    reply->deleteLater();
}

void SpotifyClient::play() {
    QString token = readAccessToken();
    if (token.isEmpty()) return;

    QNetworkRequest request(QUrl("https://api.spotify.com/v1/me/player/play"));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());

    QNetworkReply *reply = networkManager.put(request, QByteArray());
    while (!reply->isFinished()) {
        QCoreApplication::processEvents();
    }
    reply->deleteLater();
}

void SpotifyClient::nextTrack() {
    QString token = readAccessToken();
    if (token.isEmpty()) return;

    QNetworkRequest request(QUrl("https://api.spotify.com/v1/me/player/next"));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());

    QNetworkReply *reply = networkManager.post(request, QByteArray());
    while (!reply->isFinished()) {
        QCoreApplication::processEvents();
    }
    reply->deleteLater();
}

void SpotifyClient::previousTrack() {
    QString token = readAccessToken();
    if (token.isEmpty()) return;

    QNetworkRequest request(QUrl("https://api.spotify.com/v1/me/player/previous"));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());

    QNetworkReply *reply = networkManager.post(request, QByteArray());
    while (!reply->isFinished()) {
        QCoreApplication::processEvents();
    }
    reply->deleteLater();
}

void SpotifyClient::close()
{
    if (networkCheckTimer->isActive()) {
        networkCheckTimer->stop();
    }
    if (updateTimer->isActive()) {
        updateTimer->stop();
    }
}

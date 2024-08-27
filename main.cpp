#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include "spotifyclient.h"


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/mainaction.png"));

    qmlRegisterType<SpotifyClient>("com.example.spotifyclient", 1, 0, "SpotifyClient");

    QQmlApplicationEngine engine;

    SpotifyClient spotifyReceiver;
    engine.rootContext()->setContextProperty("spotifyReceiver", &spotifyReceiver);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl){
                QCoreApplication::exit(-1);
            }
        }, Qt::QueuedConnection);

    engine.load(url);

    spotifyReceiver.updateCurrentTrack();

    // Connect the aboutToQuit signal to stop the update process
    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&spotifyReceiver]() {
        spotifyReceiver.close();
    });

    return app.exec();
}

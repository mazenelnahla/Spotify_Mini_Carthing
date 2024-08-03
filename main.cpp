#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "udpsender.h"
#include "SpotifyReceiver.h"
#include <QQmlContext>
#include "pythonrunner.h"
#include <QIcon>


int main(int argc, char *argv[])
{
    if (qEnvironmentVariableIsEmpty("QTGLESSTREAM_DISPLAY")) {
        qputenv("QT_QPA_EGLFS_PHYSICAL_WIDTH", QByteArray("213"));
        qputenv("QT_QPA_EGLFS_PHYSICAL_HEIGHT", QByteArray("120"));

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    }
    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/mainaction.png"));

    qmlRegisterType<UDPSender>("UDPsender", 1, 0, "UDPSender");

    QQmlApplicationEngine engine;

    SpotifyReceiver spotifyReceiver;
    engine.rootContext()->setContextProperty("spotifyReceiver", &spotifyReceiver);
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);
    PythonRunner runner;
    runner.runSpotifyScript();

    return app.exec();
}

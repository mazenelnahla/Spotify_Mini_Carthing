#ifndef PYTHONRUNNER_H
#define PYTHONRUNNER_H

#include <QObject>
#include <QProcess>

class PythonRunner : public QObject {
    Q_OBJECT

public:
    explicit PythonRunner(QObject *parent = nullptr);

public slots:
    void runSpotifyScript();

private slots:
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QProcess spotifyProcess;
};

#endif // PYTHONRUNNER_H

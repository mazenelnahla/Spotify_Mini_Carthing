#include "pythonrunner.h"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QTextCodec>

PythonRunner::PythonRunner(QObject *parent) : QObject(parent) {
    connect(&spotifyProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &PythonRunner::processFinished);
}

void PythonRunner::runSpotifyScript() {
    QString appDir = QCoreApplication::applicationDirPath();
    QString scriptRelativePath = QStringLiteral("../../Spotify.py");
    QString scriptPath = QDir(appDir).filePath(scriptRelativePath);

    QFileInfo fileInfo(scriptPath);
    QString absoluteScriptPath = fileInfo.absoluteFilePath();
    qDebug() << "Script Path:" << absoluteScriptPath;

    spotifyProcess.setWorkingDirectory(fileInfo.absolutePath());
    spotifyProcess.setProgram("python");
    spotifyProcess.setArguments(QStringList() << absoluteScriptPath);

    // Ensure encoding is handled properly
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    spotifyProcess.start();

    if (!spotifyProcess.waitForStarted()) {
        qDebug() << "Failed to start the Spotify script process:" << spotifyProcess.errorString();
        return;
    }
}

void PythonRunner::processFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::CrashExit) {
        qDebug() << "Script crashed with exit code:" << exitCode;
    } else {
        qDebug() << "Script execution finished with exit code:" << exitCode;
    }
}

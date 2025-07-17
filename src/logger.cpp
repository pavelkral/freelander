#include "logger.h"
#include <QDateTime>
#include <cstdio>

Logger::Logger()
    : logFile("log.txt")
{
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        logStream.setDevice(&logFile);
    } else {
        fprintf(stderr, "Failed to open log file!\n");
    }
}

Logger::~Logger() {
    if (logFile.isOpen()) {
        logFile.close();
    }
}

Logger& Logger::instance() {
    static Logger loggerInstance;
    return loggerInstance;
}

void Logger::setEnabled(bool value) {
    QMutexLocker locker(&mutex);
    enabled = value;
}

bool Logger::isEnabled() const {
    return enabled;
}

void Logger::log(const QString& message) {
    QMutexLocker locker(&mutex);
    if (!enabled) return;

    QString timeStampedMessage = QDateTime::currentDateTime()
        .toString("yyyy-MM-dd HH:mm:ss.zzz") + " - " + message;

    const qint64 maxSize = 1024 * 1024;
    if (logFile.size() > maxSize) {
        rotateLogFile();
    }

    fprintf(stdout, "%s\n", timeStampedMessage.toUtf8().constData());
    fflush(stdout);

    if (logFile.isOpen()) {
        logStream << timeStampedMessage << '\n';
        logStream.flush();
    }
}

void Logger::rotateLogFile() {
    if (logFile.isOpen()) {
        logStream.flush();
        logFile.close();
    }

    QFile::remove("log_old.txt");
    QFile::rename("log.txt", "log_old.txt");

    logFile.setFileName("log.txt");
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        logStream.setDevice(&logFile);
    } else {
        fprintf(stderr, "Failed to open log file!\n");
    }
}

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (!Logger::instance().isEnabled())
        return;

    QString level;
    switch (type) {
    case QtDebugMsg:    level = "[DEBUG]"; break;
    case QtInfoMsg:     level = "[INFO]"; break;
    case QtWarningMsg:  level = "[WARNING]"; break;
    case QtCriticalMsg: level = "[CRITICAL]"; break;
    case QtFatalMsg:    level = "[FATAL]"; break;
    }

    QString file = context.file ? context.file : "unknown";
    QString function = context.function ? context.function : "unknown";

    QString fullMessage = QString("%1 %2 (%3:%4, %5)")
        .arg(level)
        .arg(msg)
        .arg(file)
        .arg(context.line)
        .arg(function);

    Logger::instance().log(fullMessage);

    if (type == QtFatalMsg) {
        abort();
    }
}

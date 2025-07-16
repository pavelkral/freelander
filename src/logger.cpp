#include "logger.h"
#include <QDebug>

Logger::Logger()
    : logFile("log.txt")
{
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        logStream.setDevice(&logFile);
    }
    else {
        qWarning() << "Cannot open log.txt for writing!";
    }
}

Logger::~Logger() {
    logFile.close();
}

Logger& Logger::instance() {
    static Logger loggerInstance;
    return loggerInstance;
}

void Logger::log(const QString& message) {
    QMutexLocker locker(&mutex);
    QString timeStampedMessage = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz") + " - " + message;

    qDebug().noquote() << timeStampedMessage; // Optional: log to console
    if (logFile.isOpen()) {
        logStream << timeStampedMessage << '\n';
        logStream.flush();
    }
}

// Qt message handler
void myMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QString level;

    switch (type) {
    case QtDebugMsg:
        level = "[DEBUG]";
        break;
    case QtInfoMsg:
        level = "[INFO]";
        break;
    case QtWarningMsg:
        level = "[WARNING]";
        break;
    case QtCriticalMsg:
        level = "[CRITICAL]";
        break;
    case QtFatalMsg:
        level = "[FATAL]";
        break;
    }

    QString fullMessage = QString("%1 %2 (%3:%4, %5)")
        .arg(level)
        .arg(msg)
        .arg(QString(context.file ? context.file : "unknown"))
        .arg(context.line)
        .arg(QString(context.function ? context.function : "unknown"));

    Logger::instance().log(fullMessage);

    if (type == QtFatalMsg) {
        abort();
    }
}

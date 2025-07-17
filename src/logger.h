#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>

class Logger {
public:
    static Logger& instance();

    void log(const QString& message);
    void setEnabled(bool enabled);
    bool isEnabled() const;
	void setDebug(bool debug) { this->debug = debug; }
    bool isDebug() const { return debug; }
 

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void rotateLogFile();

    QFile logFile;
    QTextStream logStream;
    QMutex mutex;
    bool enabled = true;
    bool debug = false;
};

void myMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

#endif // LOGGER_H


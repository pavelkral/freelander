#include "utils.h"
#include <QFile>
#include <QIODevice>
#include <QJsonParseError>
#include <QDebug>
#include <QColor>

QJsonDocument Utils::loadJsonDocumentFromFile(const QString &filePath)
{

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Couldn't open file for reading:" << filePath << "Error:" << file.errorString();
        return QJsonDocument(); //QJsonDocument::isNull()
    }
    QByteArray jsonData = file.readAll();

    file.close();
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {

        qWarning() << "Failed to parse JSON file:" << filePath << "Error:" << parseError.errorString() << "at offset" << parseError.offset;
        return QJsonDocument(); //  QJsonDocument::isNull()
    }

    return jsonDoc;
}

bool Utils::saveJsonDocumentToFile(const QString &filePath, const QJsonDocument &jsonDocument, QJsonDocument::JsonFormat format)
{

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {

        qWarning() << "Couldn't open file for writing:" << filePath << "Error:" << file.errorString();
        return false;
    }

    QByteArray jsonData = jsonDocument.toJson(format);
    qint64 bytesWritten = file.write(jsonData);
    file.close();

    if (bytesWritten != jsonData.size()) {
        qWarning() << "Failed to write all data to file:" << filePath << "Expected:" << jsonData.size() << "Written:" << bytesWritten;
        return false;
    }

    return true;
}

void Utils::Log(const QString &str,const QColor &col)
{

    if (col == Qt::red) {
        // Červená  (ANSI kód 31)
        qDebug().noquote() << "\033[1;31m"<< str << " \033[0m";
    } else if (col == Qt::green) {
        // Zelená  (ANSI kód 32)
        qDebug().noquote() << "\033[1;32m" << str << " \033[0m";
    } else if (col == Qt::blue) {
        // Modrá barva v terminálu (ANSI kód 34)
        qDebug().noquote() << "\033[1;34m" << str << " \033[0m";
    } else if (col == Qt::yellow) {
        // Žlutá  (ANSI kód 33)
        qDebug().noquote() << "\033[1;33m" << str << " \033[0m";
    } else if (col == Qt::cyan) {
        // Azurová  (ANSI kód 36)
        qDebug().noquote() << "\033[1;36m" << str << " \033[0m";
    } else if (col == Qt::magenta) {
        // Purpurová (ANSI kód 35)
        qDebug().noquote() << "\033[1;35m" << str << " \033[0m";
    }

    else {
  // (bílá/šedá ANSI kód 37)
        qDebug().noquote() << "\033[1;37m" << str << " \033[0m";
    }
}

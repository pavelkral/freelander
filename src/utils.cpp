#include "utils.h"
#include <QFile>
#include <QIODevice>
#include <QJsonParseError>
#include <QDebug>

QJsonDocument Utils::loadJsonDocumentFromFile(const QString &filePath)
{

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Couldn't open file for reading:" << filePath << "Error:" << file.errorString();
        return QJsonDocument(); //QJsonDocument::isNull()
    }
    QByteArray jsonData = file.readAll();
    //  zavřeme ho (i když by se zavřel automaticky na konci funkce)
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

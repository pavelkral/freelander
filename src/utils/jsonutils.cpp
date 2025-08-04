#include "jsonutils.h"
#include <QFile>
#include <QIODevice>
#include <QJsonParseError>
#include <QDebug>
#include <QColor>

QJsonDocument JsonUtils::loadJsonDocumentFromFile(const QString &filePath)
{

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Couldn't open file :" << filePath << "Error:" << file.errorString();
        return QJsonDocument(); //QJsonDocument::isNull()
    }
    QByteArray jsonData = file.readAll();

    file.close();
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {

        qWarning() << "Failed to parse json :" << filePath << "Error:" << parseError.errorString() << "at offset" << parseError.offset;
        return QJsonDocument(); //  QJsonDocument::isNull()
        
    }

    return jsonDoc;
}

bool JsonUtils::saveJsonDocumentToFile(const QString &filePath, const QJsonDocument &jsonDocument, QJsonDocument::JsonFormat format)
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
        qWarning() << "Failed to write file into disk:" << filePath << "Expected:" << jsonData.size() << "Written:" << bytesWritten;
        return false;
    }

    return true;
}



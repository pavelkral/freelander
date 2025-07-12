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
        qWarning() << "Failed to write file into disk:" << filePath << "Expected:" << jsonData.size() << "Written:" << bytesWritten;
        return false;
    }

    return true;
}

void Utils::Log(const QString &str,const QColor &col)
{

    if (col == Qt::red) {
       
        qDebug().noquote() << "\033[1;31m"<< str << " \033[0m";
    } else if (col == Qt::green) {
        
        qDebug().noquote() << "\033[1;32m" << str << " \033[0m";
    } else if (col == Qt::blue) {
        
        qDebug().noquote() << "\033[1;34m" << str << " \033[0m";
    } else if (col == Qt::yellow) {
      
        qDebug().noquote() << "\033[1;33m" << str << " \033[0m";
    } else if (col == Qt::cyan) {
     
        qDebug().noquote() << "\033[1;36m" << str << " \033[0m";
    } else if (col == Qt::magenta) {
        
        qDebug().noquote() << "\033[1;35m" << str << " \033[0m";
    }

    else {

        qDebug().noquote() << "\033[1;37m" << str << " \033[0m";
    }
}

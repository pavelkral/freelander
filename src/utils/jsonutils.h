#ifndef JSONUTILS_H
#define JSONUTILS_H

#include <QObject>
#include <QJsonDocument>
#include <QString>


class JsonUtils : public QObject
{
    Q_OBJECT

public:

    JsonUtils() = delete;
    static QJsonDocument loadJsonDocumentFromFile(const QString &filePath);
    static bool saveJsonDocumentToFile(const QString &filePath,const QJsonDocument &jsonDocument,QJsonDocument::JsonFormat format = QJsonDocument::Indented);
 
private:


};

#endif // JSONUTILS_H



#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QJsonDocument>
#include <QString>


class Utils : public QObject
{
    Q_OBJECT

public:

    Utils() = delete;
    static QJsonDocument loadJsonDocumentFromFile(const QString &filePath);
    static bool saveJsonDocumentToFile(const QString &filePath,const QJsonDocument &jsonDocument,QJsonDocument::JsonFormat format = QJsonDocument::Indented);

private:


};

#endif // UTILS_H



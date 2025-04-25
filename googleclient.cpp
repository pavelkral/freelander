#include "googleclient.h"
#include "utils.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QDebug>
#include <QTextCharFormat>
#include <QMessageBox>
#include <QCalendarWidget>

GoogleClient::GoogleClient(QObject *parent)
    : QObject(parent), m_manager(new QNetworkAccessManager(this)) {}

void GoogleClient::setAccessToken(const QString &token) {
    m_token = token;
}

QPair<QDateTime,QDateTime> GoogleClient::monthRange(const QDate &monthDate) const {
    QDate first(monthDate.year(), monthDate.month(), 1);
    QDate last = first.addMonths(1).addDays(-1);
    QDateTime start(first, QTime(0, 0));
    QDateTime end(last, QTime(23, 59, 59));
    return { start, end };
}

void GoogleClient::fetchEvents(const QDate &monthDate, QCalendarWidget *calendar) {
    if (m_token.isEmpty()) return;

    //qDebug() << "Token:" << m_token;
    auto [start, end] = monthRange(monthDate);

    QUrl url("https://www.googleapis.com/calendar/v3/calendars/primary/events");
    QUrlQuery query;
    query.addQueryItem("timeMin", start.toUTC().toString(Qt::ISODate));
    query.addQueryItem("timeMax", end.toUTC().toString(Qt::ISODate));
    query.addQueryItem("singleEvents", "true");
    query.addQueryItem("orderBy", "startTime");
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer " + m_token.toUtf8());
    //qDebug() << "Google req:" << request.url();
const QString TOKEN_FILE = "result.json";

    QNetworkReply *reply = m_manager->get(request);

    connect(reply, &QNetworkReply::finished, [=]() {
        QByteArray data = reply->readAll();


        QJsonDocument doc = QJsonDocument::fromJson(data);

        bool ok = Utils::saveJsonDocumentToFile(TOKEN_FILE,doc,QJsonDocument::Indented);
        if(ok){
              // qDebug() << "Json:" << doc.toJson();
        }
        QJsonArray items = doc.object().value("items").toArray();

        QSet<QDate> highlightDates;
        QSet<QDate> dates;
        QStringList lines;
        eventIdMap.clear();

        for (const QJsonValue &item : items) {
            QJsonObject obj = item.toObject();
            QString summary = obj["summary"].toString("Bez názvu");
            QString id = obj["id"].toString();
            QJsonObject startObj = obj["start"].toObject();
            QDate eventDate;
            QString startStr;

            if (startObj.contains("dateTime")) {
                QDateTime dt = QDateTime::fromString(startObj["dateTime"].toString(), Qt::ISODate);
                eventDate = dt.date();
                startStr = dt.toString("dd.MM.yyyy HH:mm");
            } else {
                eventDate = QDate::fromString(startObj["date"].toString(), Qt::ISODate);
                startStr = eventDate.toString("dd.MM.yyyy");
            }

            QString displayText = startStr + " - " + summary + "";
            eventIdMap[displayText] = id;
            highlightDates.insert(eventDate);
            lines << displayText;
            dates.insert(eventDate);
        }

        emit eventsFetched(lines.join("\n"), dates);
        reply->deleteLater();
    });
}

void GoogleClient::fetchEventDetails(const QString &eventId) {

    if (m_token.isEmpty()) return;

    QUrl url("https://www.googleapis.com/calendar/v3/calendars/primary/events/"+eventId);
    QNetworkRequest req(url);
    req.setRawHeader("Authorization", "Bearer "+m_token.toUtf8());
    auto *reply = m_manager->get(req);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        if (reply->error()==QNetworkReply::NoError) {
            auto obj = QJsonDocument::fromJson(reply->readAll()).object();
            QString sum = obj["summary"].toString();
            QDateTime st = QDateTime::fromString(obj["start"].toObject()["dateTime"].toString(), Qt::ISODate);
            QDateTime en = QDateTime::fromString(obj["end"].toObject()["dateTime"].toString(), Qt::ISODate);
            emit eventDetailsFetched(sum, st, en);
        }
        reply->deleteLater();
    });
}

void GoogleClient::createEvent(const QString &summary, const QDateTime &start, const QDateTime &end,QCalendarWidget *calendar) {
    if (m_token.isEmpty()) return;

    QUrl url("https://www.googleapis.com/calendar/v3/calendars/primary/events");
    QNetworkRequest req(url);
    req.setRawHeader("Authorization", "Bearer "+m_token.toUtf8());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject event;
    event["summary"] = summary;
    QJsonObject startObj;
    startObj["dateTime"] = start.toUTC().toString(Qt::ISODate);
    startObj["timeZone"] = "UTC";
    QJsonObject endObj;
    endObj["dateTime"] = end.toUTC().toString(Qt::ISODate);
    endObj["timeZone"] = "UTC";
    event["start"] = startObj;
    event["end"] = endObj;

    auto *reply = m_manager->post(req, QJsonDocument(event).toJson());

    connect(reply,&QNetworkReply::finished,this,[=](){
        bool ok = reply->error()==QNetworkReply::NoError;

        int year = calendar->yearShown();
        int month = calendar->monthShown();
        QDate firstDateOfMonth(year, month, 1);
        fetchEvents(firstDateOfMonth, calendar);
        reply->deleteLater();
    });
}

void GoogleClient::updateEvent(const QString &eventId, const QString &summary, const QDateTime &start, const QDateTime &end,QCalendarWidget *calendar) {
    if (m_token.isEmpty()) return;

    QUrl url("https://www.googleapis.com/calendar/v3/calendars/primary/events/"+eventId);
    QNetworkRequest req(url);
    req.setRawHeader("Authorization", "Bearer "+m_token.toUtf8());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject event;
    event["summary"] = summary;

    QJsonObject startObj;
    startObj["dateTime"] = start.toUTC().toString(Qt::ISODate);
    startObj["timeZone"] = "UTC";

    QJsonObject endObj;
    endObj["dateTime"] = end.toUTC().toString(Qt::ISODate);
    endObj["timeZone"] = "UTC";

    event["start"] = startObj;
    event["end"] = endObj;

    auto *reply = m_manager->sendCustomRequest(req, "PATCH", QJsonDocument(event).toJson());
    connect(reply,&QNetworkReply::finished,this,[=](){
        bool ok = reply->error()==QNetworkReply::NoError;
       // emit operationFinished(ok, ok?"Updated":"Update failed: "+reply->errorString());
      //  QMessageBox::information(nullptr, ok?"OK":"Error", reply->errorString());
        int year = calendar->yearShown();
        int month = calendar->monthShown();
        QDate firstDateOfMonth(year, month, 1);
        fetchEvents(firstDateOfMonth, calendar);
        reply->deleteLater();
    });
}

void GoogleClient::deleteEvent(const QString &eventId,QCalendarWidget *calendar) {

    if (m_token.isEmpty()) return;
    QUrl url("https://www.googleapis.com/calendar/v3/calendars/primary/events/"+eventId);
    QNetworkRequest req(url);
    req.setRawHeader("Authorization", "Bearer "+m_token.toUtf8());
    auto *reply = m_manager->deleteResource(req);
    connect(reply,&QNetworkReply::finished,this,[=](){

        bool ok = reply->error()==QNetworkReply::NoError;
       // QMessageBox::information(nullptr, ok?"OK":"Error", reply->errorString());
        int year = calendar->yearShown();
        int month = calendar->monthShown();
        QDate firstDateOfMonth(year, month, 1);
        fetchEvents(firstDateOfMonth, calendar);
        reply->deleteLater();
    });
}

void GoogleClient::fetchEventsForDate(const QDate &date, std::function<void(const QList<QPair<QString, QString>>&)> callback) {

    QNetworkAccessManager *manager = new QNetworkAccessManager;

    QDateTime startOfDay(date, QTime(0, 0, 0));
    QDateTime endOfDay(date, QTime(23, 59, 59));

    QUrl url("https://www.googleapis.com/calendar/v3/calendars/primary/events");
    QUrlQuery query;
    query.addQueryItem("timeMin", startOfDay.toUTC().toString(Qt::ISODate));
    query.addQueryItem("timeMax", endOfDay.toUTC().toString(Qt::ISODate));
    query.addQueryItem("singleEvents", "true");
    query.addQueryItem("orderBy", "startTime");
    url.setQuery(query);

    QNetworkRequest request(url);

    request.setRawHeader("Authorization", "Bearer "+m_token.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject event;
    QNetworkReply *reply = manager->get(request);
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        QList<QPair<QString, QString>> events;
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonArray items = doc.object()["items"].toArray();
            for (const QJsonValue &item : items) {
                QJsonObject obj = item.toObject();
                QString summary = obj["summary"].toString("Bez názvu");
                QString id = obj["id"].toString();
                events.append(qMakePair(summary, id));
            }
        } else {
            qDebug() << "Error fetching events for date:" << reply->errorString();
        }
        reply->deleteLater();
        callback(events);
    });
}

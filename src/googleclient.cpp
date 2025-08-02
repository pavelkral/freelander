#include "googleclient.h"
//#include "utils.h"
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
#include "utils/utils.h"

GoogleClient::GoogleClient(QObject *parent)
    : QObject(parent), m_manager(new QNetworkAccessManager(this)) {

    parentWidget = qobject_cast<QWidget*>(this->parent());

    if (!parentWidget) {
        parentWidget = nullptr;
        qDebug() << "No parent widget";
    }
}

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
    const QString RESULT_FILE = "result.json";
    QNetworkReply *reply = m_manager->get(request);

    connect(reply, &QNetworkReply::finished, [=]() {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        //bool ok = Utils::saveJsonDocumentToFile(TOKEN_FILE,doc,QJsonDocument::Indented);
       // if(ok){
              // qDebug() << "Json:" << doc.toJson();
       // }
        QJsonArray items = doc.object().value("items").toArray();

        QSet<QDate> highlightDates;
        QSet<QDate> dates;
        QList<QPair<QDate, QString>> upcomingEvents;
        eventIdMap.clear();

        for (const QJsonValue &item : items) {
            QJsonObject obj = item.toObject();
            QString summary = obj["summary"].toString("Bez názvu");
            QString id = obj["id"].toString();

            QJsonObject startObj = obj["start"].toObject();
            QJsonObject endObj = obj["end"].toObject();

            QDate startDate, endDate;
            QString startStr;

            if (startObj.contains("dateTime")) {
                QDateTime startDT = QDateTime::fromString(startObj["dateTime"].toString(), Qt::ISODate);
                QDateTime endDT = QDateTime::fromString(endObj["dateTime"].toString(), Qt::ISODate);
                startDate = startDT.date();
                endDate = endDT.date();
                startStr = startDT.toString("dd.MM.yyyy HH:mm");
            } else {
                startDate = QDate::fromString(startObj["date"].toString(), Qt::ISODate);
                endDate = QDate::fromString(endObj["date"].toString(), Qt::ISODate);
                startStr = startDate.toString("dd.MM.yyyy");
            }

            QString displayText = "📌  " + startStr + " - " + summary;
            eventIdMap[displayText] = id;

            // current or futur
            if (startDate >= QDate::currentDate()) {
                upcomingEvents.append({startDate, displayText});
            }

            //start to endDate highlite
            for (QDate d = startDate; d <= endDate; d = d.addDays(1)) {
                dates.insert(d);
            }
        }

        //date filter
        std::sort(upcomingEvents.begin(), upcomingEvents.end(), [](const auto &a, const auto &b) {
            return a.first < b.first;
        });

        // first 5
        QStringList lines;
        int count = 0;
        for (const auto &pair : upcomingEvents) {
            lines << pair.second;
            if (++count >= 5) break;
        }


        if (reply->error() == QNetworkReply::NoError) {  
            emit eventsFetched(lines.join("\n"), dates);               
            emit apiRequestSucceeded(QString("call Fetch successful!"));
          //  qDebug() << "Response:" << reply->readAll();
          
        } else {  
       
            emit apiRequestFailed(reply->errorString(), reply->error());
        } 
        
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
            emit eventDetailsFetched(sum, st, en, eventId);
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

    connect(reply, &QNetworkReply::finished, this, [=]() {  

           if (reply->error() == QNetworkReply::NoError) {  
               int year = calendar->yearShown();  
               int month = calendar->monthShown();  
               QDate firstDateOfMonth(year, month, 1);  
               fetchEvents(firstDateOfMonth, calendar);
              
           } else {  
               qDebug() << "Error occurred:" << reply->errorString();  
           }  
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
		/*if (ok) {
			Utils::Log("API call successful! Data received", Qt::red);
			emit apiRequestSucceeded(reply->readAll());
		}
		else {
			Utils::Log("API call failed: " + reply->errorString(), Qt::red);
			emit apiRequestFailed(reply->errorString());
		}*/
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
  
        if (reply->error() == QNetworkReply::NoError) {   
            int year = calendar->yearShown();  
            int month = calendar->monthShown();  
            QDate firstDateOfMonth(year, month, 1);  
            fetchEvents(firstDateOfMonth, calendar);  
        } else {  
           
        } 

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
            qDebug() << "Error fetching for date:" << reply->errorString();
        }
        reply->deleteLater();
        callback(events);
    });
}

void GoogleClient::setTokenManager(TokenManager* tokenManager)
{
	m_tokenManager = tokenManager;
}

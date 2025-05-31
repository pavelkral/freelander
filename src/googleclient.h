#ifndef GOOGLECLIENT_H
#define GOOGLECLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QDate>
#include <QDateTime>
#include <QSet>
#include <QMap>
#include <QOAuth2AuthorizationCodeFlow>
#include "tokenmanager.h"

class QTextEdit;
class QCalendarWidget;

class GoogleClient : public QObject {
    Q_OBJECT
public:
    explicit GoogleClient(QObject *parent = nullptr);
    QMap<QString, QString> eventIdMap;
    QString getToken() { return m_token; }
    void setAccessToken(const QString& token);

    void fetchEvents(const QDate &date, QCalendarWidget *calendar);
    void fetchEventDetails(const QString &eventId);
    void createEvent(const QString &summary, const QDateTime &start, const QDateTime &end ,QCalendarWidget *calendar);
    void updateEvent(const QString &eventId, const QString &summary, const QDateTime &start, const QDateTime &end,QCalendarWidget *calendar);
    void deleteEvent(const QString &eventId,QCalendarWidget *calendar);
    void fetchEventsForDate(const QDate &date, std::function<void(const QList<QPair<QString, QString>>&)> callback) ;
    void setTokenManager(TokenManager* tokenManager);
 
    
signals:
    void eventsFetched(const QString &text, const QSet<QDate> &dates);
    void eventDetailsFetched(const QString &summary, const QDateTime &start, const QDateTime &end,const QString &enventId);

private:
    QWidget* parentWidget = nullptr;
    QString m_token;
    QNetworkAccessManager *m_manager = nullptr;
	TokenManager* m_tokenManager = nullptr;
    QPair<QDateTime,QDateTime> monthRange(const QDate &monthDate) const;
};


#endif // GOOGLECLIENT_H

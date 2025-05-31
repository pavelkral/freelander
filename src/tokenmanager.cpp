#include "tokenmanager.h"
#include <QAbstractOAuth2>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QMessageBox>
#include "utils.h"
#include <chrono>
TokenManager::TokenManager(QObject *parent)
    : QObject(parent),
    m_oauth(new QOAuth2AuthorizationCodeFlow(this)),
    m_replyHandler(new QOAuthHttpServerReplyHandler(8080, this))
{

    QJsonDocument configDoc = Utils::loadJsonDocumentFromFile(CONFIG_FILE);
    QString clientId;
    QString clientSecret;

    if (configDoc.isNull()) {
        qWarning() << "Error: Configuration file is null:" << CONFIG_FILE;

    } else if (!configDoc.isObject()) {
        qWarning() << "Error: Root element is not a JSON object:" << CONFIG_FILE;
    } else {
        QJsonObject rootObj = configDoc.object();
        if (rootObj.contains("oauth_settings") && rootObj["oauth_settings"].isObject()) {
            QJsonObject oauthSettings = rootObj["oauth_settings"].toObject();

            if (oauthSettings.contains("client_id") && oauthSettings["client_id"].isString()) {
                clientId = oauthSettings["client_id"].toString();
            } else {
                qWarning() << "'client_id' is missing or not a string in 'oauth_settings' in file:" << CONFIG_FILE;
                 // QMessageBox::information(nullptr,"Client","not saved",QMessageBox::Ok);
                QMessageBox::critical(0, qApp->tr("Cannot read client id"),
                                      qApp->tr("Please setup your id in config.json.\n"
                                               "Click Cancel to exit."), QMessageBox::Cancel);
            }

            if (oauthSettings.contains("client_secret") && oauthSettings["client_secret"].isString()) {
                clientSecret = oauthSettings["client_secret"].toString();
            } else {
                qWarning() << "'client_secret' is missing or not a string in 'oauth_settings' in file:" << CONFIG_FILE;
                QMessageBox::critical(0, qApp->tr("Cannot read client secret"),
                                      qApp->
                                      qApp->tr("Please setup your id in config.json.\n"
                                               "Click Cancel to exit."), QMessageBox::Cancel);
            }

        } else {
            qWarning() << "Error: Object 'oauth_settings' is missing in configuration file:" << CONFIG_FILE;
        }
    }

    m_oauth->setReplyHandler(m_replyHandler);
    m_oauth->setAuthorizationUrl(QUrl("https://accounts.google.com/o/oauth2/auth"));
    m_oauth->setTokenUrl(QUrl("https://oauth2.googleapis.com/token"));

    if (!clientId.isEmpty() && !clientSecret.isEmpty()) {
        m_oauth->setClientIdentifier(clientId);
        m_oauth->setClientIdentifierSharedKey(clientSecret);
       // qInfo() << "OAuth loaded" << CONFIG_FILE <<"OAuth idetnification " << clientId << clientSecret;

    } else {
        qCritical() << "OAuth not loaded.";
        QMessageBox::critical(0, qApp->tr("Cannot read client id or secret"),
                              qApp->
                              qApp->tr("Please setup your config.json.\n"
                                       "Click Cancel to exit."), QMessageBox::Cancel);
    }

    m_oauth->setScope("https://www.googleapis.com/auth/calendar");
    m_oauth->setProperty("redirectUri", "http://localhost:8080");
    m_oauth->setAutoRefresh(true); 
    m_oauth->setRefreshLeadTime(std::chrono::seconds(300));

    connect(m_oauth, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
            [](const QUrl &url){ QDesktopServices::openUrl(url); });
    connect(m_oauth, &QOAuth2AuthorizationCodeFlow::granted,
            this, &TokenManager::onGranted);

    connect(m_oauth, &QOAuth2AuthorizationCodeFlow::tokenChanged,
        [](const QString& accessToken) {
            qDebug() << "Access Token changed (likely refreshed):" << accessToken;
        //    QMessageBox::critical(this->parentWidget(), "Error", "Fetch failed: " + r));
            // Update client objects that use the access token here
        });

    loadTokens();
}

void TokenManager::initialize() {
    if (!m_refreshToken.isEmpty()) {
        m_oauth->setRefreshToken(m_refreshToken);
        m_oauth->refreshTokens();
    } else {
        m_oauth->grant();
    }
}

QString TokenManager::accessToken() const {
    return m_accessToken;
}

void TokenManager::onGranted() {
    m_accessToken = m_oauth->token();
    m_refreshToken = m_oauth->refreshToken();
    saveTokens();
    emit tokenReady(m_accessToken);
}

void TokenManager::loadTokens() {

    auto doc = Utils::loadJsonDocumentFromFile(TOKEN_FILE);
    if (!doc.isNull()) {
        auto obj = doc.object();
        m_accessToken  = obj.value("access_token").toString();
        m_refreshToken = obj.value("refresh_token").toString();
    }
}

void TokenManager::saveTokens() {

    QJsonObject obj;
    obj["access_token"]  = m_accessToken;
    obj["refresh_token"] = m_refreshToken;
    QJsonDocument jsonDocument(obj);
    bool success = Utils::saveJsonDocumentToFile(TOKEN_FILE, jsonDocument, QJsonDocument::Indented);

    if (success) {
        //QMessageBox::information(nullptr,"Token"," saved",QMessageBox::Ok);
        qDebug() << "Tokens saved:" << TOKEN_FILE;
    } else {
       // QMessageBox::information(nullptr,"Token","not saved",QMessageBox::Ok);
        qWarning() << "Tokens not saved:" << TOKEN_FILE;
    }


}

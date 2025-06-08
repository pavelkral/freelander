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
#include <QHostInfo>

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
                qWarning() << "'client_id' is missing or not a string :" << CONFIG_FILE;
                QMessageBox::critical(0, qApp->tr("Cannot read client id"),
                                      qApp->tr("Please setup your id in config.json.\n"), QMessageBox::Cancel);
            }

            if (oauthSettings.contains("client_secret") && oauthSettings["client_secret"].isString()) {
                clientSecret = oauthSettings["client_secret"].toString();
            } else {
                qWarning() << "'client_secret' is missing or not a string :" << CONFIG_FILE;
                QMessageBox::critical(0, qApp->tr("Cannot read client secret"),
                                      qApp->tr("Please setup your id in config.json.\n"), QMessageBox::Cancel);
            }

        } else {
            qWarning() << "Error: Object 'oauth_settings' is missing in configuration file:" << CONFIG_FILE;
        }
    }

    checkHostAvailability("oauth2.googleapis.com");

    // Initialize the OAuth2 flow
    m_oauth->setReplyHandler(m_replyHandler);
    m_oauth->setAuthorizationUrl(QUrl("https://accounts.google.com/o/oauth2/auth"));
    m_oauth->setTokenUrl(QUrl("https://oauth2.googleapis.com/token"));

    if (!clientId.isEmpty() && !clientSecret.isEmpty()) {
        m_oauth->setClientIdentifier(clientId);
        m_oauth->setClientIdentifierSharedKey(clientSecret);
    } else {
        qCritical() << "OAuth not loaded.";
        QMessageBox::critical(0, qApp->tr("Cannot read client id or secret"),
                              qApp->tr("Please setup your config.json.\n"), QMessageBox::Cancel);
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
            qDebug() << "Access Token refreshed";
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

void TokenManager::refreshTokens()
{
    if (!m_refreshToken.isEmpty()) {
        m_oauth->setRefreshToken(m_refreshToken);
        m_oauth->refreshTokens();
        m_accessToken = m_oauth->token();
        m_refreshToken = m_oauth->refreshToken();
        saveTokens();
        emit tokenReady(m_accessToken);
    }
    else {
        qWarning() << "No refresh token available!";
    }
}

void TokenManager::checkHostAvailability(const QString& hostname) {
    QHostInfo::lookupHost(hostname, nullptr, [](const QHostInfo& info) {
        if (info.error() != QHostInfo::NoError) {
            qWarning() << "DNS lookup failed for" << info.hostName() << ":" << info.errorString();
            QMessageBox::critical(0, qApp->tr("Error"),
                qApp->tr("DNS lookup failed.\n"
                    "Click Cancel to exit."), QMessageBox::Cancel);
            qApp->quit();

        }
        else {
            qDebug() << "Host resolved:" << info.hostName() << "->" << info.addresses();
        }
        });
}

void TokenManager::onGranted() {
    m_accessToken = m_oauth->token();
    m_refreshToken = m_oauth->refreshToken();
    saveTokens();
    emit tokenReady(m_accessToken);
	//emit authenticationFailed(QString("on grant called")); // Clear any previous error message
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
       // qDebug() << "Tokens saved:" << TOKEN_FILE;
    } else {
       
      //  qWarning() << "Tokens not saved:" << TOKEN_FILE;
    }


}

#include "tokenmanager.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include "utils.h"

TokenManager::TokenManager(QObject *parent)
    : QObject(parent),
    m_oauth(new QOAuth2AuthorizationCodeFlow(this)),
    m_replyHandler(new QOAuthHttpServerReplyHandler(8080, this))
{

    QString configFilePath = "config.json"; // Nebo QCoreApplication::applicationDirPath() + "/config.json"
    QJsonDocument configDoc = Utils::loadJsonDocumentFromFile(configFilePath);
    QString clientId;
    QString clientSecret;

    if (configDoc.isNull()) {
        qWarning() << "Chyba: konfigurační soubor:" << configFilePath;
        //  zpracovat chybu - např. ukončit aplikaci nebo použít výchozí hodnoty
    } else if (!configDoc.isObject()) {
        qWarning() << "Chyba: Kořenový element není JSON objekt:" << configFilePath;
    } else {
        QJsonObject rootObj = configDoc.object();
        if (rootObj.contains("oauth_settings") && rootObj["oauth_settings"].isObject()) {
            QJsonObject oauthSettings = rootObj["oauth_settings"].toObject();

            if (oauthSettings.contains("client_id") && oauthSettings["client_id"].isString()) {
                clientId = oauthSettings["client_id"].toString();
            } else {
                qWarning() << " 'client_id' chybí nebo není řetězec v 'oauth_settings' v souboru:" << configFilePath;
            }

            if (oauthSettings.contains("client_secret") && oauthSettings["client_secret"].isString()) {
                clientSecret = oauthSettings["client_secret"].toString();
            } else {
                qWarning() << " 'client_secret' chybí nebo není řetězec v 'oauth_settings' v souboru:" << configFilePath;
            }

        } else {
            qWarning() << "Chyba: Objekt 'oauth_settings' chybí v konfiguračním souboru:" << configFilePath;
        }
    }

    m_oauth->setReplyHandler(m_replyHandler);
    m_oauth->setAuthorizationUrl(QUrl("https://accounts.google.com/o/oauth2/auth"));
    m_oauth->setTokenUrl(QUrl("https://oauth2.googleapis.com/token"));

    if (!clientId.isEmpty() && !clientSecret.isEmpty()) {
        m_oauth->setClientIdentifier(clientId);
        m_oauth->setClientIdentifierSharedKey(clientSecret);
        qInfo() << "OAuth identifikátory načteny z" << configFilePath <<"OAuth identifikátory " << clientId << clientSecret;

    } else {
        qCritical() << "Kritická chyba: Nepodařilo se načíst OAuth identifikátory z konfigurace. Aplikace nemůže pokračovat.";
        // QCoreApplication::exit(-1); nebo zobrazení chybové zprávy uživateli.
    }

   // m_oauth->setClientIdentifier("4373465227-k1nnjiou0l2kk9ndoj5cuivinchkdok5.apps.googleusercontent.com");
   // m_oauth->setClientIdentifierSharedKey("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");
    m_oauth->setScope("https://www.googleapis.com/auth/calendar");
    m_oauth->setProperty("redirectUri", "http://localhost:8080");


    connect(m_oauth, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
            [](const QUrl &url){ QDesktopServices::openUrl(url); });
    connect(m_oauth, &QOAuth2AuthorizationCodeFlow::granted,
            this, &TokenManager::onGranted);

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
    QFile f(TOKEN_FILE);
    if (f.open(QIODevice::ReadOnly)) {
        auto doc = QJsonDocument::fromJson(f.readAll());
        f.close();
        auto obj = doc.object();
        m_accessToken  = obj.value("access_token").toString();
        m_refreshToken = obj.value("refresh_token").toString();
    }
}

void TokenManager::saveTokens() {
    QJsonObject obj;
    obj["access_token"]  = m_accessToken;
    obj["refresh_token"] = m_refreshToken;
    QFile f(TOKEN_FILE);
    if (f.open(QIODevice::WriteOnly)) {
        f.write(QJsonDocument(obj).toJson());
        f.close();
    }
}

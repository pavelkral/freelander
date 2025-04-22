#include "tokenmanager.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

TokenManager::TokenManager(QObject *parent)
    : QObject(parent),
    m_oauth(new QOAuth2AuthorizationCodeFlow(this)),
    m_replyHandler(new QOAuthHttpServerReplyHandler(8080, this))
{
    m_oauth->setReplyHandler(m_replyHandler);
    m_oauth->setAuthorizationUrl(QUrl("https://accounts.google.com/o/oauth2/auth"));
    m_oauth->setAccessTokenUrl(QUrl("https://oauth2.googleapis.com/token"));
    m_oauth->setClientIdentifier("4373465227-k1nnjiou0l2kk9ndoj5cuivinchkdok5.apps.googleusercontent.com");
    m_oauth->setClientIdentifierSharedKey("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");
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
        m_oauth->refreshAccessToken();
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

#ifndef TOKENMANAGER_H
#define TOKENMANAGER_H
#include <QObject>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QCoreApplication>
#include <QDir>

class TokenManager : public QObject {
    Q_OBJECT
public:
    explicit TokenManager(QObject *parent = nullptr);
    void initialize();
    QString accessToken() const;

signals:
    void tokenReady(const QString &accessToken);
    void authenticationFailed(const QString &error);

private slots:
    void onGranted();

private:
    void loadTokens();
    void saveTokens();

    QOAuth2AuthorizationCodeFlow *m_oauth;
    QOAuthHttpServerReplyHandler *m_replyHandler;
    QString m_accessToken;
    QString m_refreshToken;
    const QString TOKEN_FILE = QCoreApplication::applicationDirPath() + QDir::separator() + "tokens.json";
    const QString CONFIG_FILE = QCoreApplication::applicationDirPath() + QDir::separator() + "config.json";
};

#endif // TOKENMANAGER_H

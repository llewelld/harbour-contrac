#include <QDebug>

#include "settings.h"

Settings * Settings::instance = nullptr;

Settings::Settings(QObject *parent) : QObject(parent),
    settings(this)
{
    m_downloadServer = settings.value(QStringLiteral("servers/downloadServer"), QStringLiteral("127.0.0.1:8003")).toString();
    m_uploadServer = settings.value(QStringLiteral("servers/uploadServer"), QStringLiteral("127.0.0.1:8000")).toString();
    m_verificationServer = settings.value(QStringLiteral("servers/verificationServer"), QStringLiteral("127.0.0.1:8004")).toString();

    qDebug() << "Settings created";
}

Settings::~Settings()
{
    settings.setValue(QStringLiteral("servers/downloadServer"), m_downloadServer);
    settings.setValue(QStringLiteral("servers/uploadServer"), m_uploadServer);
    settings.setValue(QStringLiteral("servers/verificationServer"), m_verificationServer);

    qDebug() << "Settings deleted";
}

void Settings::instantiate(QObject *parent) {
    if (instance == nullptr) {
        instance = new Settings(parent);
    }
}

Settings & Settings::getInstance() {
    return *instance;
}

QObject * Settings::provider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return instance;
}

QString Settings::downloadServer() const
{
    return m_downloadServer;
}

void Settings::setDownloadServer(QString const &downloadServer)
{
    if (m_downloadServer != downloadServer) {
        m_downloadServer = downloadServer;
        emit downloadServerChanged();
    }
}

QString Settings::uploadServer() const
{
    return m_uploadServer;
}

void Settings::setUploadServer(QString const &uploadServer)
{
    if (m_uploadServer != uploadServer) {
        m_uploadServer = uploadServer;
        emit uploadServerChanged();
    }
}

QString Settings::verificationServer() const
{
    return m_verificationServer;
}

void Settings::setVerificationServer(QString const &verificationServer)
{
    if (m_verificationServer != verificationServer) {
        m_verificationServer = verificationServer;
        emit verificationServerChanged();
    }
}

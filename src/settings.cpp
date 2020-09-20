#include <QDebug>
#include <mlite5/MGConfItem>
#include <sailfishapp.h>

#include "../contracd/src/exposuresummary.h"

#include "settings.h"

#define SETTINGS_MAX_VERSION (1)

Settings * Settings::instance = nullptr;

Settings::Settings(QObject *parent) : QObject(parent),
    m_settings(this)
{
    m_downloadServer = m_settings.value(QStringLiteral("servers/downloadServer"), QStringLiteral("https://svc90.main.px.t-online.de")).toString();
    m_uploadServer = m_settings.value(QStringLiteral("servers/uploadServer"), QStringLiteral("https://submission.coronawarn.app")).toString();
    m_verificationServer = m_settings.value(QStringLiteral("servers/verificationServer"), QStringLiteral("https://verification.coronawarn.app")).toString();
    m_latestSummary = m_settings.value(QStringLiteral("update/latestSummary"), QVariant::fromValue<ExposureSummary>(ExposureSummary())).value<ExposureSummary>();
    m_summaryUpdated = m_settings.value(QStringLiteral("update/date"), QDateTime()).toDateTime();

    // Figure out where we're going to find our images
    QScopedPointer<MGConfItem> ratioItem(new MGConfItem("/desktop/sailfish/silica/theme_pixel_ratio"));
    double pixelRatio = ratioItem->value(1.0).toDouble();

    double const threshold[] = {1.75, 1.5, 1.25, 1.0};
    QString const dir[] = {"2.0", "1.75", "1.5", "1.25", "1.0"};

    Q_ASSERT((sizeof(dir) / sizeof(QString)) == ((sizeof(threshold) / sizeof(double)) + 1));

    qDebug() << "Pixel ration: " << pixelRatio;
    size_t pos;
    for (pos = 0; pos < (sizeof(threshold) - 1) && pixelRatio <= threshold[pos]; ++pos) {
        // Just carry on looping
    }

    m_imageDir = SailfishApp::pathTo("qml/images/z" + dir[pos]).toString(QUrl::RemoveScheme) + "/";
    qDebug() << "Image folder: " << m_imageDir;

    qDebug() << "Settings created: " << m_settings.fileName();

    upgrade();
}

Settings::~Settings()
{
    m_settings.setValue(QStringLiteral("servers/downloadServer"), m_downloadServer);
    m_settings.setValue(QStringLiteral("servers/uploadServer"), m_uploadServer);
    m_settings.setValue(QStringLiteral("servers/verificationServer"), m_verificationServer);
    m_settings.setValue(QStringLiteral("update/latestSummary"), QVariant::fromValue<ExposureSummary>(m_latestSummary));
    m_settings.setValue(QStringLiteral("update/date"), m_summaryUpdated);

    qDebug() << "Deleted settings";
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

ExposureSummary *Settings::latestSummary()
{
    return &m_latestSummary;
}

void Settings::setLatestSummary(ExposureSummary const *latestSummary)
{
    if (m_latestSummary != *latestSummary) {
        m_latestSummary = *latestSummary;
        emit latestSummaryChanged();
    }
}

QDateTime Settings::summaryUpdated() const
{
    return m_summaryUpdated;
}

void Settings::setSummaryUpdated(QDateTime summaryUpdated)
{
    if (m_summaryUpdated != summaryUpdated) {
        m_summaryUpdated = summaryUpdated;
        emit summaryUpdatedChanged();
    }
}

QString Settings::getImageDir() const {
    return m_imageDir;
}

QString Settings::getImageUrl(QString const &id) const {
    return m_imageDir + id + ".png";
}

bool Settings::upgrade()
{
    quint32 version;
    bool success = true;

    if (m_settings.allKeys().size() == 0) {
        version = SETTINGS_MAX_VERSION;
        qDebug() << "Creating new settings file with version: " << SETTINGS_MAX_VERSION;
    }
    else {
        version = m_settings.value(QStringLiteral("application/settingsVersion"), 0).toUInt();
        qDebug() << "Existing settings file version: " << version;
    }

    switch (version) {
    case 0:
        if (success) {
            success = upgradeToVersion1();
        }
        // Fallthrough
        [[clang::fallthrough]];
    default:
    case SETTINGS_MAX_VERSION:
        // File upgraded
        // Do nothing
        break;
    }

    if (success) {
        m_settings.setValue(QStringLiteral("application/version"), VERSION);
        m_settings.setValue(QStringLiteral("application/settingsVersion"), SETTINGS_MAX_VERSION);
    }

    return success;
}

bool Settings::upgradeToVersion1()
{
    bool success = true;

    qDebug() << "Upgrading settings to version 1";

    // Update from the test servers to the real Corona-Warn-App servers
    m_downloadServer = QStringLiteral("https://svc90.main.px.t-online.de");
    m_uploadServer = QStringLiteral("https://submission.coronawarn.app");
    m_verificationServer = QStringLiteral("https://verification.coronawarn.app");

    m_settings.setValue(QStringLiteral("servers/downloadServer"), m_downloadServer);
    m_settings.setValue(QStringLiteral("servers/uploadServer"), m_uploadServer);
    m_settings.setValue(QStringLiteral("servers/verificationServer"), m_verificationServer);

    m_settings.sync();
    success = (m_settings.status() == QSettings::NoError);

    return success;
}

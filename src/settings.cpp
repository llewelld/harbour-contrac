#include <QDebug>
#include <mlite5/MGConfItem>
#include <sailfishapp.h>

#include "../contracd/src/exposuresummary.h"

#include "settings.h"

Settings * Settings::instance = nullptr;

Settings::Settings(QObject *parent) : QObject(parent),
    settings(this)
{
    m_downloadServer = settings.value(QStringLiteral("servers/downloadServer"), QStringLiteral("127.0.0.1:8003")).toString();
    m_uploadServer = settings.value(QStringLiteral("servers/uploadServer"), QStringLiteral("127.0.0.1:8000")).toString();
    m_verificationServer = settings.value(QStringLiteral("servers/verificationServer"), QStringLiteral("127.0.0.1:8004")).toString();
    m_latestSummary = settings.value(QStringLiteral("update/latestSummary"), QVariant::fromValue<ExposureSummary>(ExposureSummary())).value<ExposureSummary>();
    m_summaryUpdated = settings.value(QStringLiteral("update/date"), QDateTime()).toDateTime();

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

    qDebug() << "Settings created: " << settings.fileName();
}

Settings::~Settings()
{
    settings.setValue(QStringLiteral("servers/downloadServer"), m_downloadServer);
    settings.setValue(QStringLiteral("servers/uploadServer"), m_uploadServer);
    settings.setValue(QStringLiteral("servers/verificationServer"), m_verificationServer);
    settings.setValue(QStringLiteral("update/latestSummary"), QVariant::fromValue<ExposureSummary>(m_latestSummary));
    settings.setValue(QStringLiteral("update/date"), m_summaryUpdated);

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


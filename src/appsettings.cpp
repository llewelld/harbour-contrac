#include <mlite5/MGConfItem>
#include <sailfishapp.h>
#include <QDebug>

#include "../contracd/src/exposuresummary.h"
#include "../contracd/src/version.h"

#include "appsettings.h"

#define SETTINGS_MAX_VERSION (1)

AppSettings *AppSettings::instance = nullptr;

AppSettings::AppSettings(QObject *parent)
    : QObject(parent)
    , m_settings(this)
{
    m_downloadServer = m_settings.value(QStringLiteral("servers/downloadServer"), QStringLiteral("https://svc90.main.px.t-online.de")).toString();
    m_uploadServer = m_settings.value(QStringLiteral("servers/uploadServer"), QStringLiteral("https://submission.coronawarn.app")).toString();
    m_verificationServer = m_settings.value(QStringLiteral("servers/verificationServer"), QStringLiteral("https://verification.coronawarn.app")).toString();
    m_latestSummary = m_settings.value(QStringLiteral("update/latestSummary"), QVariant::fromValue<ExposureSummary>(ExposureSummary())).value<ExposureSummary>();
    m_summaryUpdated = m_settings.value(QStringLiteral("update/date"), QDateTime()).toDateTime();
    m_infoViewed = m_settings.value(QStringLiteral("application/infoViewed"), 0).toUInt();
    m_countryCode = m_settings.value(QStringLiteral("update/countryCode"), QStringLiteral("DE")).toString();

    // Set
    m_riskWeights.append(m_settings.value(QStringLiteral("combinedRisk/riskWeightLow"), 1.0).toDouble());
    m_riskWeights.append(m_settings.value(QStringLiteral("combinedRisk/riskWeightMid"), 1.0).toDouble());
    m_riskWeights.append(m_settings.value(QStringLiteral("combinedRisk/riskWeightHigh"), 1.0).toDouble());

    m_defaultBuckeOffset = m_settings.value(QStringLiteral("combinedRisk/defaultBuckeOffset"), 0.0).toInt();
    m_normalizationDivisor = m_settings.value(QStringLiteral("combinedRisk/normalizationDivisor"), 1.0).toInt();

    m_riskScoreClasses.clear();
    int size = m_settings.beginReadArray(QStringLiteral("combinedRisk/riskScoreClasses"));
    for (int pos = 0; pos < size; ++pos) {
        m_settings.setArrayIndex(pos);
        RiskScoreClass riskScoreClass = m_settings.value(QStringLiteral("class")).value<RiskScoreClass>();
        m_riskScoreClasses.append(riskScoreClass);
    }
    m_settings.endArray();

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

AppSettings::~AppSettings()
{
    m_settings.setValue(QStringLiteral("servers/downloadServer"), m_downloadServer);
    m_settings.setValue(QStringLiteral("servers/uploadServer"), m_uploadServer);
    m_settings.setValue(QStringLiteral("servers/verificationServer"), m_verificationServer);
    m_settings.setValue(QStringLiteral("update/latestSummary"), QVariant::fromValue<ExposureSummary>(m_latestSummary));
    m_settings.setValue(QStringLiteral("update/date"), m_summaryUpdated);
    m_settings.setValue(QStringLiteral("application/infoViewed"), m_infoViewed);
    m_settings.setValue(QStringLiteral("update/countryCode"), m_countryCode);

    // Store
    while (m_riskWeights.size() < 3) {
        m_riskWeights.append(1.0);
    }
    m_settings.setValue(QStringLiteral("combinedRisk/riskWeightLow"), m_riskWeights[0]);
    m_settings.setValue(QStringLiteral("combinedRisk/riskWeightMid"), m_riskWeights[1]);
    m_settings.setValue(QStringLiteral("combinedRisk/riskWeightHigh"), m_riskWeights[2]);
    m_settings.setValue(QStringLiteral("combinedRisk/defaultBuckeOffset"), m_defaultBuckeOffset);
    m_settings.setValue(QStringLiteral("combinedRisk/normalizationDivisor"), m_normalizationDivisor);

    m_settings.beginWriteArray(QStringLiteral("combinedRisk/riskScoreClasses"), m_riskScoreClasses.size());
    for (int pos = 0; pos < m_riskScoreClasses.size(); ++pos) {
        m_settings.setArrayIndex(pos);
        m_settings.setValue(QStringLiteral("class"), QVariant::fromValue<RiskScoreClass>(m_riskScoreClasses[pos]));
    }
    m_settings.endArray();

    instance = nullptr;
    qDebug() << "Deleted settings";
}

void AppSettings::instantiate(QObject *parent)
{
    if (instance == nullptr) {
        instance = new AppSettings(parent);
    }
}

AppSettings &AppSettings::getInstance()
{
    return *instance;
}

QObject *AppSettings::provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return instance;
}

QString AppSettings::downloadServer() const
{
    return m_downloadServer;
}

void AppSettings::setDownloadServer(QString const &downloadServer)
{
    if (m_downloadServer != downloadServer) {
        m_downloadServer = downloadServer;
        emit downloadServerChanged();
    }
}

QString AppSettings::uploadServer() const
{
    return m_uploadServer;
}

void AppSettings::setUploadServer(QString const &uploadServer)
{
    if (m_uploadServer != uploadServer) {
        m_uploadServer = uploadServer;
        emit uploadServerChanged();
    }
}

QString AppSettings::verificationServer() const
{
    return m_verificationServer;
}

void AppSettings::setVerificationServer(QString const &verificationServer)
{
    if (m_verificationServer != verificationServer) {
        m_verificationServer = verificationServer;
        emit verificationServerChanged();
    }
}

ExposureSummary *AppSettings::latestSummary()
{
    return &m_latestSummary;
}

void AppSettings::setLatestSummary(ExposureSummary const *latestSummary)
{
    if (m_latestSummary != *latestSummary) {
        m_latestSummary = *latestSummary;
        emit latestSummaryChanged();
    }
}

QDateTime AppSettings::summaryUpdated() const
{
    return m_summaryUpdated;
}

void AppSettings::setSummaryUpdated(QDateTime summaryUpdated)
{
    if (m_summaryUpdated != summaryUpdated) {
        m_summaryUpdated = summaryUpdated;
        emit summaryUpdatedChanged();
    }
}

quint32 AppSettings::infoViewed() const
{
    return m_infoViewed;
}

void AppSettings::setInfoViewed(quint32 infoViewed)
{
    if (m_infoViewed != infoViewed) {
        m_infoViewed = infoViewed;
        emit infoViewedChanged();
    }
}

QString AppSettings::countryCode() const
{
    return m_countryCode;
}

void AppSettings::setCountryCode(QString countryCode)
{
    if (m_countryCode != countryCode) {
        m_countryCode = countryCode;
        emit countryCodeChanged();
    }
}

QString AppSettings::getImageDir() const
{
    return m_imageDir;
}

QString AppSettings::getImageUrl(QString const &id) const
{
    return m_imageDir + id + ".png";
}

bool AppSettings::upgrade()
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

bool AppSettings::upgradeToVersion1()
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

QList<double> AppSettings::riskWeights() const
{
    return m_riskWeights;
}

void AppSettings::setRiskWeights(QList<double> riskWeights)
{
    if (riskWeights.length() == 3) {
        if (m_riskWeights != riskWeights) {
            m_riskWeights = riskWeights;
            emit riskWeightsChanged();
        }
    }
    else {
        qDebug() << "Risk weights must come in threes, but there are only " << riskWeights.length();
    }
}

qint32 AppSettings::defaultBuckeOffset() const
{
    return m_defaultBuckeOffset;
}

void AppSettings::setDefaultBuckeOffset(qint32 defaultBuckeOffset)
{
    if (m_defaultBuckeOffset != defaultBuckeOffset) {
        m_defaultBuckeOffset = defaultBuckeOffset;
        emit defaultBuckeOffsetChanged();
    }
}

qint32 AppSettings::normalizationDivisor() const
{
    return m_normalizationDivisor;
}

void AppSettings::setNormalizationDivisor(qint32 normalizationDivisor)
{
    if (m_normalizationDivisor != normalizationDivisor) {
        m_normalizationDivisor = normalizationDivisor;
        emit normalizationDivisorChanged();
    }
}

QList<RiskScoreClass> AppSettings::riskScoreClasses() const
{
    return m_riskScoreClasses;
}

void AppSettings::setRiskScoreClasses(QList<RiskScoreClass> riskScoreClasses)
{
    if (m_riskScoreClasses != riskScoreClasses) {
        m_riskScoreClasses = riskScoreClasses;
        emit riskScoreClassesChanged();
    }
}

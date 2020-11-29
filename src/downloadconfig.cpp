#include <QStandardPaths>
#include <QDir>

#include "../contracd/src/exposureconfiguration.h"
#include "../contracd/src/zipistreambuffer.h"

#include "appsettings.h"
#include "serveraccess.h"
#include "applicationConfiguration.pb.h"

#include "downloadconfig.h"

#define CONFIG_DIRECTORY QStringLiteral("/download/")
#define CONFIG_LEAFNAME QStringLiteral("file.config")

DownloadConfig::DownloadConfig(QObject *parent) : QObject(parent)
  , m_serverAccess(new ServerAccess(this))
  , m_downloading(false)
  , m_status(StatusIdle)
  , m_configuration(new ExposureConfiguration(this))
{
    m_serverAccess->setId("accessKey1");
    m_serverAccess->setSecret("verySecretKey1");
    m_serverAccess->setBaseUrl(AppSettings::getInstance().downloadServer());
    m_serverAccess->setBucket("cwa");

    connect(m_configuration, &ExposureConfiguration::minimumRiskScoreChanged, this, &DownloadConfig::configChanged);
    connect(m_configuration, &ExposureConfiguration::attenuationScoresChanged, this, &DownloadConfig::configChanged);
    connect(m_configuration, &ExposureConfiguration::daysSinceLastExposureScoresChanged, this, &DownloadConfig::configChanged);
    connect(m_configuration, &ExposureConfiguration::durationScoresChanged, this, &DownloadConfig::configChanged);
    connect(m_configuration, &ExposureConfiguration::transmissionRiskScoresChanged, this, &DownloadConfig::configChanged);
    connect(m_configuration, &ExposureConfiguration::attenuationWeightChanged, this, &DownloadConfig::configChanged);
    connect(m_configuration, &ExposureConfiguration::daysSinceLastExposureWeightChanged, this, &DownloadConfig::configChanged);
    connect(m_configuration, &ExposureConfiguration::durationWeightChanged, this, &DownloadConfig::configChanged);
    connect(m_configuration, &ExposureConfiguration::transmissionRiskWeightChanged, this, &DownloadConfig::configChanged);
    connect(m_configuration, &ExposureConfiguration::durationAtAttenuationThresholdsChanged, this, &DownloadConfig::configChanged);

    qDebug() << "DownloadConfig created";
}

Q_INVOKABLE void DownloadConfig::downloadLatest()
{
    qDebug() << "Requesting configuration";

    if (!m_downloading) {
        m_serverAccess->setBaseUrl(AppSettings::getInstance().downloadServer());
        setStatus(StatusDownloading);

        QString key = "version/v1/configuration/country/DE/app_config";
        QString filename = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + CONFIG_DIRECTORY;
        QDir dir;
        qDebug() << "Creating directory:" << filename;
        dir.mkpath(filename);
        filename += CONFIG_LEAFNAME;
        qDebug() << "Saving to:" << filename;

        ServerGetFileResult *result = m_serverAccess->getFile(key, filename);
        connect(result, &ServerResult::finished, this, [this, result, filename]() {
            qDebug() << "Finished downloading configuration";
            finalise();
            switch (result->error()) {
            case QNetworkReply::NoError:
                qDebug() << "DownloadConfig NoError";
                setStatus(StatusIdle);
                loadConfig();
                break;
            default:
                qDebug() << "Network error while downloading keys: " << result->error();
                setStatusError(ErrorNetwork);
                break;
            }

            emit downloadComplete(filename);
            result->deleteLater();
        });

        m_downloading = true;
        emit downloadingChanged();
    }
}

void DownloadConfig::finalise()
{
    if (m_downloading) {
        m_downloading = false;
        qDebug() << "DownloadConfig finalised";

        emit downloadingChanged();
    }
}

DownloadConfig::Status DownloadConfig::status() const
{
    return m_status;
}

void DownloadConfig::setStatus(Status status)
{
    if (m_status != status) {
        qDebug() << "Setting status: " << status;
        m_status = status;

        if (m_error != ErrorNone) {
            m_error = ErrorNone;
            emit errorChanged();
        }
        emit statusChanged();
    }
}

DownloadConfig::ErrorType DownloadConfig::error() const
{
    return m_error;
}

void DownloadConfig::setStatusError(ErrorType error)
{
    if (m_error != error) {
        qDebug() << "Setting error: " << error;
        m_error = error;

        if (m_status != StatusError) {
            m_status = StatusError;
            emit statusChanged();
        }
        emit errorChanged();
    }
}

bool DownloadConfig::downloading() const
{
    return m_downloading;
}

bool DownloadConfig::loadConfig()
{
    QString filename = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + CONFIG_DIRECTORY + CONFIG_LEAFNAME;

    QuaZip quazip(filename);
    bool result = true;
    QStringList files;
    diagnosis::ApplicationConfiguration appConfig;

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    result = quazip.open(QuaZip::mdUnzip, nullptr);

    if (result) {
        // The zip archive must contain exactly two entries
        files = quazip.getFileNameList();
        if ((files.size() != 2) || !files.contains("export.sig") || !files.contains("export.bin")) {
            result = false;
        }
    }

    //TODO: Check signature file

    if (result) {
        ZipIStreamBuffer streambuf(quazip, "export.bin");
        qDebug() << "Stream is good: " << streambuf.success();
        std::istream stream(&streambuf);

        result = appConfig.ParseFromIstream(&stream);
    }

    if (result) {
        applyConfiguration(appConfig);
    }
    else {
        qDebug() << "ApplicationConfiguration proto file failed to load";
    }

    return result;
}

void DownloadConfig::applyConfiguration(diagnosis::ApplicationConfiguration const &appConfig)
{
    bool sendAttenuationDurationConfigChanged = false;

    m_configuration->setMinimumRiskScore(appConfig.minriskscore());
    qDebug() << "Config min risk score: " << appConfig.minriskscore();
    AppSettings &settings = AppSettings::getInstance();

    if (appConfig.has_exposureconfig()) {
        const ::diagnosis::RiskScoreParameters& params = appConfig.exposureconfig();
        if (params.has_transmission()) {
            QList<qint32> transmissionRiskScores;
            transmissionRiskScores.append(params.transmission().appdefined_1());
            transmissionRiskScores.append(params.transmission().appdefined_2());
            transmissionRiskScores.append(params.transmission().appdefined_3());
            transmissionRiskScores.append(params.transmission().appdefined_4());
            transmissionRiskScores.append(params.transmission().appdefined_5());
            transmissionRiskScores.append(params.transmission().appdefined_6());
            transmissionRiskScores.append(params.transmission().appdefined_7());
            transmissionRiskScores.append(params.transmission().appdefined_8());
            m_configuration->setTransmissionRiskScores(transmissionRiskScores);
            qDebug() << "Config transmission risks scores:" << transmissionRiskScores;
        }
        m_configuration->setTransmissionRiskWeight(params.transmissionweight());
        qDebug() << "Config transmission risk weight:" << params.transmissionweight();

        if (params.has_duration()) {
            QList<qint32> durationScores;
            durationScores.append(params.duration().eq_0_min());
            durationScores.append(params.duration().gt_0_le_5_min());
            durationScores.append(params.duration().gt_5_le_10_min());
            durationScores.append(params.duration().gt_10_le_15_min());
            durationScores.append(params.duration().gt_15_le_20_min());
            durationScores.append(params.duration().gt_20_le_25_min());
            durationScores.append(params.duration().gt_25_le_30_min());
            durationScores.append(params.duration().gt_30_min());
            m_configuration->setDurationScores(durationScores);
            qDebug() << "Config duration scores:" << durationScores;
        }

        m_configuration->setDurationWeight(params.durationweight());
        qDebug() << "Config duration weight:" << params.durationweight();

        if (params.has_dayssincelastexposure()) {
            QList<qint32> daysSinceLastExposureScores;
            daysSinceLastExposureScores.append(params.dayssincelastexposure().ge_14_days());
            daysSinceLastExposureScores.append(params.dayssincelastexposure().ge_12_lt_14_days());
            daysSinceLastExposureScores.append(params.dayssincelastexposure().ge_10_lt_12_days());
            daysSinceLastExposureScores.append(params.dayssincelastexposure().ge_8_lt_10_days());
            daysSinceLastExposureScores.append(params.dayssincelastexposure().ge_6_lt_8_days());
            daysSinceLastExposureScores.append(params.dayssincelastexposure().ge_4_lt_6_days());
            daysSinceLastExposureScores.append(params.dayssincelastexposure().ge_2_lt_4_days());
            daysSinceLastExposureScores.append(params.dayssincelastexposure().ge_0_lt_2_days());
            m_configuration->setDaysSinceLastExposureScores(daysSinceLastExposureScores);
            qDebug() << "Config days since last exposure scores:" << daysSinceLastExposureScores;
        }

        m_configuration->setDaysSinceLastExposureWeight(params.daysweight());
        qDebug() << "Config days weight:" << params.daysweight();

        if (params.has_attenuation()) {
            QList<qint32> attenuationScores;
            attenuationScores.append(params.attenuation().gt_73_dbm());
            attenuationScores.append(params.attenuation().gt_63_le_73_dbm());
            attenuationScores.append(params.attenuation().gt_51_le_63_dbm());
            attenuationScores.append(params.attenuation().gt_33_le_51_dbm());
            attenuationScores.append(params.attenuation().gt_27_le_33_dbm());
            attenuationScores.append(params.attenuation().gt_15_le_27_dbm());
            attenuationScores.append(params.attenuation().gt_10_le_15_dbm());
            attenuationScores.append(params.attenuation().lt_10_dbm());
            m_configuration->setAttenuationScores(attenuationScores);
            qDebug() << "Config attenuation scores:" << attenuationScores;
        }

        m_configuration->setAttenuationWeight(params.attenuationweight());
        qDebug() << "Config attenuation weight:" << params.attenuationweight();
    }

    if (appConfig.has_attenuationduration()) {
        const ::diagnosis::AttenuationDuration &attenuation = appConfig.attenuationduration();
        QList<qint32> thresholds;
        thresholds.append(attenuation.thresholds().lower());
        thresholds.append(attenuation.thresholds().upper());
        m_configuration->setDurationAtAttenuationThresholds(thresholds);
        qDebug() << "Config attentuation duration thresholds:" << thresholds;

        if (attenuation.has_weights()) {

            QList<double> riskWeights;
            riskWeights.append(attenuation.weights().low());
            riskWeights.append(attenuation.weights().mid());
            riskWeights.append(attenuation.weights().high());

            if (settings.riskWeights() != riskWeights) {
                settings.setRiskWeights(riskWeights);
                sendAttenuationDurationConfigChanged = true;
            }

            qDebug() << "Config attentuation duration risk weights:" << settings.riskWeights();
        }
        if (settings.defaultBuckeOffset() != attenuation.defaultbucketoffset()) {
            settings.setDefaultBuckeOffset(attenuation.defaultbucketoffset());
            sendAttenuationDurationConfigChanged = true;
        }
        if (settings.normalizationDivisor() != attenuation.riskscorenormalizationdivisor()) {
            settings.setNormalizationDivisor(attenuation.riskscorenormalizationdivisor());
            sendAttenuationDurationConfigChanged = true;
        }
    }

    if (appConfig.has_riskscoreclasses()) {
        QList<RiskScoreClass> riskScoreClasses;
        const ::diagnosis::RiskScoreClassification &riskScores = appConfig.riskscoreclasses();

        for (int pos = 0; pos < riskScores.risk_classes_size(); ++pos) {
            RiskScoreClass riskScoreClass(riskScores.risk_classes(pos));
            riskScoreClasses.append(riskScoreClass);
        }

        if (settings.riskScoreClasses() != riskScoreClasses) {
            settings.setRiskScoreClasses(riskScoreClasses);
            sendAttenuationDurationConfigChanged = true;
        }
    }

    if (sendAttenuationDurationConfigChanged) {
        emit attenuationDurationConfigChanged();
    }

}

ExposureConfiguration *DownloadConfig::config() const
{
    return m_configuration;
}

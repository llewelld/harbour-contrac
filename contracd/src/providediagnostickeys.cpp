#include <QDebug>
#include <math.h>
#include <QMutexLocker>

#include "exposurenotification_p.h"
#include "contrac.pb.h"

#include "providediagnostickeys.h"

// In milliseconds
#define CONTIGUOUS_PERIOD_THRESHOLD (10 * 60 * 1000)

#define MAX_BATCH_SIZE (1024)

namespace { // anonymous namespace

inline qint32 attenuationCalculation(qint16 rssiMeasured, qint8 rssiCorrection, qint8 txPower)
{
    // Attenuation = TX_power - (RSSI_measured + RSSI_correction)
    // See https://developers.google.com/android/exposure-notifications/ble-attenuation-overview
    qint32 attenuation;

    attenuation = txPower - (rssiMeasured + rssiCorrection);

    return attenuation;
}

inline qint32 checkLowerThreshold(qint32 thresholds[8], QList<qint32> scores, qint32 value)
{
    qint8  pos = 7;
    while ((pos > 0) && (value < thresholds[pos - 1])) {
        --pos;
    }

    return scores[pos];
}

inline qint32 checkGreaterThreshold(qint32 thresholds[8], QList<qint32> scores, qint32 value)
{
    qint8 pos = 7;
    while ((pos > 0) && (value > thresholds[pos - 1])) {
        --pos;
    }

    return scores[pos];
}

template<class T>
inline T clamp(T value, T min, T max)
{
    if (value < min) {
        value = min;
    }
    else {
        if (value > max) {
            value = max;
        }
    }
    return value;
}

inline bool compareIntervals(RpiDataItem const &rpi1, RpiDataItem const & rpi2)
{
    return rpi1.m_interval < rpi2.m_interval;
}

qint32 calculateRiskScore(ExposureConfiguration const &configuration, qint32 transmissionRisk, qint32 duration, qint32 days_ago, qint32 attenuationValue)
{
    qint32 riskScore;
    qint32 attenuationScore;
    qint32 daysSinceLastExposureScore;
    qint32 durationScore;
    qint32 transmissionRiskScore;
    qint8 pos;

    qint32 attenuationThresholds[8] = {73, 63, 51, 33, 27, 15, 10};
    qint32 daysThresholds[8] = {14, 12, 10, 8, 6, 4, 2};
    qint32 durationThreshold[8] = {1, 6, 11, 16, 21, 26, 31};

    attenuationScore = checkGreaterThreshold(attenuationThresholds, configuration.attenuationScores(), attenuationValue);
    daysSinceLastExposureScore = checkGreaterThreshold(daysThresholds, configuration.daysSinceLastExposureScores(), days_ago);
    durationScore = checkLowerThreshold(durationThreshold, configuration.durationScores(), duration);

    pos = clamp(static_cast<qint8>(transmissionRisk), static_cast<qint8>(0), static_cast<qint8>(7));
    transmissionRiskScore = configuration.transmissionRiskScores()[pos];

    riskScore = attenuationScore * daysSinceLastExposureScore * durationScore * transmissionRiskScore;

    return riskScore;
}

} // anonymous namespace

QList<ExposureInformation> ProvideDiagnosticKeys::aggregateExposureData(quint32 dayNumber, ExposureConfiguration const &configuration, QList<ContactMatch> matches, qint32 const days_ago) const
{
    qDebug() << "MinimumRiskScore: " << configuration.minimumRiskScore();
    qDebug() << "AttenuationScores: " << configuration.attenuationScores();
    qDebug() << "DaysSinceLastExposureScores: " << configuration.daysSinceLastExposureScores();
    qDebug() << "DurationScores: " << configuration.durationScores();
    qDebug() << "TransmissionRiskScores: " << configuration.transmissionRiskScores();
    qDebug() << "DurationAtAttenuationThresholds: " << configuration.durationAtAttenuationThresholds();

    QList<ExposureInformation> exposures;
    qint32 attenuationThreshold[2];
    attenuationThreshold[0] = configuration.durationAtAttenuationThresholds()[0];
    attenuationThreshold[1] = configuration.durationAtAttenuationThresholds()[1];

    qint32 transmissionRisk;
    qint32 totalDuration;
    //qint32 days_ago;
    qint32 attenuationValue;

    Metadata metadata;
    qint8 txPower;
    qint32 attenuation;

    for (ContactMatch match : matches) {
        metadata.setDtk(match.m_dtk->m_dtk);

        // The aggregation process assumes that rpi intervals are increasing
        // So order the RPIs if necessary to ensure this
        // This is just for safety, by far the most likely situation is that they'll already be ordered
        if (!std::is_sorted(match.m_rpis.begin(), match.m_rpis.end(), compareIntervals)) {
            qDebug() << "Reordering RPIs to have increasing interval values";
            std::sort(match.m_rpis.begin(), match.m_rpis.end(), compareIntervals);
        }

        int rpi_pos = 0;
        qDebug() << "Aggregating data for rpi matches:" << match.m_rpis.size();
        while (rpi_pos < match.m_rpis.size()) {
            ExposureInformation exposure;

            quint64 const dateMillisSinceEpoch = dayNumber * 24 * 60 * 60 * 1000;
            exposure.setDateMillisSinceEpoch(dateMillisSinceEpoch);
            transmissionRisk = static_cast<qint32>(match.m_dtk->m_transmissionRiskLevel);
            exposure.setTransmissionRiskLevel(transmissionRisk);

            qint64 interval = match.m_rpis[rpi_pos].m_interval;
            totalDuration = 0;
            qint32 totalRiskScore;
            qint32 attenuationDurations[3] = {0, 0, 0};
            qint64 attenuationSum = 0;
            while (rpi_pos < match.m_rpis.size() && (match.m_rpis[rpi_pos].m_interval < interval + (CONTIGUOUS_PERIOD_THRESHOLD / CTINTERVAL_DURATION) + 1)) {
                RpiDataItem const &rpi = match.m_rpis[rpi_pos];
                // The timeDelta value should be measured in minutes and rounded upwards
                qint64 timeDelta = qint64(ceil((qint64(1 + rpi.m_interval) - interval) / double(60 * 1000 / CTINTERVAL_DURATION)));
                // Make use of the txPower stored in the AEM
                metadata.setEncryptedMetadata(rpi.m_aem);
                metadata.setRpi(rpi.m_rpi);
                txPower = metadata.txPower();
                // The metadata must also be valid
                if (metadata.valid()) {
                    attenuation = attenuationCalculation(rpi.m_rssi, m_rssiCorrection, txPower);

                    // Attenuation durations in minutes
                    // From the specification:
                    // "Array of durations in minutes at certain radio signal attenuations."
                    if (attenuation < attenuationThreshold[0]) {
                        attenuationDurations[0] += timeDelta;
                    }
                    else {
                        if (attenuation < attenuationThreshold[1]) {
                            attenuationDurations[1] += timeDelta;
                        }
                        else {
                            attenuationDurations[2] += timeDelta;
                        }
                    }
                    attenuationSum += (attenuation * timeDelta);

                    interval = rpi.m_interval;
                    totalDuration += timeDelta;
                }
                else {
                    qDebug() << "Invalid metadata";
                }

                ++rpi_pos;
            }
            // Duration to the nearest 5 minutes
            // From the specification:
            // "Length of exposure in 5 minute increments, with a 30 minute maximum."
            qint32 durationMinutes = 5 * qint32(ceil(totalDuration / 5.0));
            if (durationMinutes > 30) {
                durationMinutes = 30;
            }
            exposure.setDurationMinutes(durationMinutes);
            QList<qint32> attenuationDurationList;
            attenuationDurationList.append(attenuationDurations[0]);
            attenuationDurationList.append(attenuationDurations[1]);
            attenuationDurationList.append(attenuationDurations[2]);
            exposure.setAttenuationDurations(attenuationDurationList);
            attenuationValue = totalDuration > 0 ? qint32(attenuationSum / totalDuration) : 0;
            exposure.setAttenuationValue(attenuationValue);

            // We no longer calculate days_ago to allow unit tests to work correctly
            //days_ago = static_cast<qint32>(dayNumber) - static_cast<qint32>(m_contrac->dayNumber());
            totalRiskScore = calculateRiskScore(configuration, transmissionRisk, totalDuration, days_ago, attenuationValue);

            exposure.setTotalRiskScore(totalRiskScore);

            exposures.append(exposure);
        }
    }

    return exposures;
}

ProvideDiagnosticKeys::ProvideDiagnosticKeys(ExposureNotificationPrivate * exposureNotificationPrivate, QVector<QString> const &keyFiles, ExposureConfiguration const &configuration, QString &token, qint8 rssiCorrection)
    : QObject(exposureNotificationPrivate)
    , m_d(exposureNotificationPrivate)
    , m_keyFiles(keyFiles)
    , m_configuration(configuration)
    , m_token(token)
    , m_rssiCorrection(rssiCorrection)
    , m_terminate(false)
{
    m_currentDayNumber = m_d->m_contrac->dayNumber();
    m_startTime = QDateTime::currentDateTime();
}

void ProvideDiagnosticKeys::run()
{
    bool result;
    int pos;
    QList<DiagnosisKey> diagnosisKeys[DAYS_TO_STORE];
    bool terminate = false;

    for (QVector<QString>::const_iterator iter = m_keyFiles.begin(); (iter != m_keyFiles.end()) && !terminate; ++iter) {
        QString const &file = *iter;
        // TODO: Check region
        // TODO: Check batch numbering
        // TODO: Check start/end timestamps
        diagnosis::TemporaryExposureKeyExport keyExport;
        result = ExposureNotificationPrivate::loadDiagnosisKeys(file, &keyExport);
        if (result) {
            for(pos = 0; pos < keyExport.keys_size(); ++pos) {
                diagnosis::TemporaryExposureKey const &key = keyExport.keys(pos);
                QByteArray dtk(key.key_data().data(), static_cast<int>(key.key_data().length()));
                DiagnosisKey diagnosisKey(dtk, static_cast<DiagnosisKey::RiskLevel>(key.transmission_risk_level()), static_cast<quint32>(key.rolling_start_interval_number()), static_cast<quint32>(key.rolling_period()));
                qint64 day = m_currentDayNumber - (key.rolling_start_interval_number() / 144);
                if (day >= 0 && day < DAYS_TO_STORE) {
                    diagnosisKeys[day].append(diagnosisKey);
                }
//                else {
//                    qDebug() << "Day falls outside 14 day range: " << day;
//                }

            }
        }
        else {
            qDebug() << "Error loading diagnosis key file: " << file;
        }
        terminate = shouldTerminate();
    }

    for (qint32 day = 0; (day < DAYS_TO_STORE) && !terminate; ++day) {
        qDebug() << "Aggregating day: " << day;
        if (diagnosisKeys[day].length() > 0) {
            quint32 const dayNumber = static_cast<quint32>(m_currentDayNumber + day);
            qDebug() << "Calling findDtkMatches with" << diagnosisKeys[day].count() << " keys";

            QList<ContactMatch> matches;
            qint32 batchPos = 0;
            while (batchPos < diagnosisKeys[day].size() && !terminate) {
                QList<DiagnosisKey> diagnosticKeyBatch;
                diagnosticKeyBatch = diagnosisKeys[day].mid(batchPos, MAX_BATCH_SIZE);
                batchPos += MAX_BATCH_SIZE;
                matches.append(m_d->m_contacts->findDtkMatches(dayNumber, diagnosticKeyBatch));
                terminate = shouldTerminate();
            }

            terminate = shouldTerminate();
            if (!terminate) {
                qDebug() << "Calling aggregateExposureData with" << matches.count() << "matches";
                m_exposureInfoList.append(aggregateExposureData(dayNumber, m_configuration, matches, day));
            }
        }
        terminate = shouldTerminate();
    }

    emit taskFinished(m_token);
}

bool ProvideDiagnosticKeys::shouldTerminate()
{
    QMutexLocker locker(&m_terminateMutex);

    return m_terminate;
}

void ProvideDiagnosticKeys::requestTerminate()
{
    qDebug() << "Requesting termination";

    QMutexLocker locker(&m_terminateMutex);

    m_terminate = true;
}

QDateTime const ProvideDiagnosticKeys::startTime() const
{
    return m_startTime;
}

QList<ExposureInformation> const &ProvideDiagnosticKeys::exposureInfoList() const
{
    return m_exposureInfoList;
}

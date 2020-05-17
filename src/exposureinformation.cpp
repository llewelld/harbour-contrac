#include "temporaryexposurekey.h"

#include "exposureinformation.h"

ExposureInformation::ExposureInformation(QObject *parent)
    : QObject(parent)
    , m_dateMillisSinceEpoch(0)
    , m_durationMinutes(0)
    , m_attenuationValue(0)
    , m_transmissionRiskLevel(TemporaryExposureKey::RiskLevelMedium)
    , m_totalRiskScore(0)
    , m_attenuationDurations()
{
}

ExposureInformation::ExposureInformation(ExposureInformation const &exposureInformation)
    : QObject(exposureInformation.parent())
    , m_dateMillisSinceEpoch(exposureInformation.dateMillisSinceEpoch())
    , m_durationMinutes(exposureInformation.durationMinutes())
    , m_attenuationValue(exposureInformation.attenuationValue())
    , m_transmissionRiskLevel(exposureInformation.transmissionRiskLevel())
    , m_totalRiskScore(exposureInformation.totalRiskScore())
    , m_attenuationDurations(exposureInformation.attenuationDurations())
{
}

quint64 ExposureInformation::dateMillisSinceEpoch() const
{
    return m_dateMillisSinceEpoch;
}

quint32 ExposureInformation::durationMinutes() const
{
    return m_durationMinutes;
}

quint32 ExposureInformation::attenuationValue() const
{
    return m_attenuationValue;
}

quint32 ExposureInformation::transmissionRiskLevel() const
{
    return m_transmissionRiskLevel;
}

quint32 ExposureInformation::totalRiskScore() const
{
    return m_totalRiskScore;
}

QList<quint32> ExposureInformation::attenuationDurations() const
{
    return m_attenuationDurations;
}

void ExposureInformation::setDateMillisSinceEpoch(quint64 dateMillisSinceEpoch)
{
    if (m_dateMillisSinceEpoch != dateMillisSinceEpoch) {
        m_dateMillisSinceEpoch = dateMillisSinceEpoch;
        emit dateMillisSinceEpochChanged();
    }
}

void ExposureInformation::setDurationMinutes(quint32 durationMinutes)
{
    if (m_durationMinutes != durationMinutes) {
        m_durationMinutes = durationMinutes;
        emit durationMinutesChanged();
    }
}

void ExposureInformation::setAttenuationValue(quint32 attenuationValue)
{
    if (m_attenuationValue != attenuationValue) {
        m_attenuationValue = attenuationValue;
        emit attenuationValueChanged();
    }
}

void ExposureInformation::setTransmissionRiskLevel(quint32 transmissionRiskLevel)
{
    if (m_transmissionRiskLevel != transmissionRiskLevel) {
        m_transmissionRiskLevel = transmissionRiskLevel;
        emit transmissionRiskLevelChanged();
    }
}

void ExposureInformation::setTotalRiskScore(quint32 totalRiskScore)
{
    if (m_totalRiskScore != totalRiskScore) {
        m_totalRiskScore = totalRiskScore;
        emit totalRiskScoreChanged();
    }
}

void ExposureInformation::setAttenuationDurations(QList<quint32> attenuationDurations)
{
    if (m_attenuationDurations != attenuationDurations) {
        m_attenuationDurations = attenuationDurations;
        emit attenuationDurationsChanged();
    }
}


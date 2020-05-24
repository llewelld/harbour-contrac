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

ExposureInformation& ExposureInformation::operator=( const ExposureInformation &other)
{
    if (this != &other) {
        m_dateMillisSinceEpoch = other.m_dateMillisSinceEpoch;
        m_durationMinutes = other.m_durationMinutes;
        m_attenuationValue = other.m_attenuationValue;
        m_transmissionRiskLevel = other.m_transmissionRiskLevel;
        m_totalRiskScore = other.m_totalRiskScore;
        m_attenuationDurations = other.m_attenuationDurations;
    }

    return *this;
}

quint64 ExposureInformation::dateMillisSinceEpoch() const
{
    return m_dateMillisSinceEpoch;
}

qint32 ExposureInformation::durationMinutes() const
{
    return m_durationMinutes;
}

qint32 ExposureInformation::attenuationValue() const
{
    return m_attenuationValue;
}

qint32 ExposureInformation::transmissionRiskLevel() const
{
    return m_transmissionRiskLevel;
}

qint32 ExposureInformation::totalRiskScore() const
{
    return m_totalRiskScore;
}

QList<qint32> ExposureInformation::attenuationDurations() const
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

void ExposureInformation::setDurationMinutes(qint32 durationMinutes)
{
    if (m_durationMinutes != durationMinutes) {
        m_durationMinutes = durationMinutes;
        emit durationMinutesChanged();
    }
}

void ExposureInformation::setAttenuationValue(qint32 attenuationValue)
{
    if (m_attenuationValue != attenuationValue) {
        m_attenuationValue = attenuationValue;
        emit attenuationValueChanged();
    }
}

void ExposureInformation::setTransmissionRiskLevel(qint32 transmissionRiskLevel)
{
    if (m_transmissionRiskLevel != transmissionRiskLevel) {
        m_transmissionRiskLevel = transmissionRiskLevel;
        emit transmissionRiskLevelChanged();
    }
}

void ExposureInformation::setTotalRiskScore(qint32 totalRiskScore)
{
    if (m_totalRiskScore != totalRiskScore) {
        m_totalRiskScore = totalRiskScore;
        emit totalRiskScoreChanged();
    }
}

void ExposureInformation::setAttenuationDurations(QList<qint32> attenuationDurations)
{
    if (m_attenuationDurations != attenuationDurations) {
        m_attenuationDurations = attenuationDurations;
        emit attenuationDurationsChanged();
    }
}


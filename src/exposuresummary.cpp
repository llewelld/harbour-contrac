#include "exposuresummary.h"

ExposureSummary::ExposureSummary(QObject *parent) : QObject(parent)
{

}

ExposureSummary::ExposureSummary(ExposureSummary const &exposureSummary)
    : QObject(exposureSummary.parent())
    , m_daysSinceLastExposure(exposureSummary.daysSinceLastExposure())
    , m_matchedKeyCount(exposureSummary.daysSinceLastExposure())
    , m_maximumRiskScore(exposureSummary.maximumRiskScore())
    , m_attenuationDurations(exposureSummary.attenuationDurations())
    , m_summationRiskScore(exposureSummary.summationRiskScore())
{
}

quint32 ExposureSummary::daysSinceLastExposure() const
{
    return m_daysSinceLastExposure;
}

quint32 ExposureSummary::matchedKeyCount() const
{
    return m_matchedKeyCount;
}

quint32 ExposureSummary::maximumRiskScore() const
{
    return m_maximumRiskScore;
}

QList<quint32> ExposureSummary::attenuationDurations() const
{
    return m_attenuationDurations;
}

quint32 ExposureSummary::summationRiskScore() const
{
    return m_summationRiskScore;
}


void ExposureSummary::setDaysSinceLastExposure(quint32 daysSinceLastExposure)
{
    if (m_daysSinceLastExposure != daysSinceLastExposure) {
        m_daysSinceLastExposure = daysSinceLastExposure;
        emit daysSinceLastExposureChanged();
    }
}

void ExposureSummary::setMatchedKeyCount(quint32 matchedKeyCount)
{
    if (m_matchedKeyCount != matchedKeyCount) {
        m_matchedKeyCount = matchedKeyCount;
        emit matchedKeyCountChanged();
    }
}

void ExposureSummary::setMaximumRiskScore(quint32 maximumRiskScore)
{
    if (m_maximumRiskScore != maximumRiskScore) {
        m_maximumRiskScore = maximumRiskScore;
        emit maximumRiskScoreChanged();
    }
}

void ExposureSummary::setAttenuationDurations(QList<quint32> attenuationDurations)
{
    if (m_attenuationDurations != attenuationDurations) {
        m_attenuationDurations = attenuationDurations;
        emit attenuationDurationsChanged();
    }
}

void ExposureSummary::setSummationRiskScore(quint32 summationRiskScore)
{
    if (m_summationRiskScore != summationRiskScore) {
        m_summationRiskScore = summationRiskScore;
        emit summationRiskScoreChanged();
    }
}



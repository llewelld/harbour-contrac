#include "exposuresummary.h"

ExposureSummary::ExposureSummary(QObject *parent) : QObject(parent)
{

}

ExposureSummary::ExposureSummary(ExposureSummary const &exposureSummary)
    : QObject(exposureSummary.parent())
    , m_daysSinceLastExposure(exposureSummary.m_daysSinceLastExposure)
    , m_matchedKeyCount(exposureSummary.m_matchedKeyCount)
    , m_maximumRiskScore(exposureSummary.m_maximumRiskScore)
    , m_attenuationDurations(exposureSummary.m_attenuationDurations)
    , m_summationRiskScore(exposureSummary.m_summationRiskScore)
{
}

ExposureSummary& ExposureSummary::operator=( const ExposureSummary &other)
{
    if (this != &other) {
        m_daysSinceLastExposure = other.m_daysSinceLastExposure;
        m_matchedKeyCount = other.m_matchedKeyCount;
        m_maximumRiskScore = other.m_maximumRiskScore;
        m_attenuationDurations = other.m_attenuationDurations;
        m_summationRiskScore = other.m_summationRiskScore;
    }

    return *this;
}

QDBusArgument &operator<<(QDBusArgument &argument, const ExposureSummary &exposureSummary)
{
    argument.beginStructure();
    argument << exposureSummary.daysSinceLastExposure();
    argument << exposureSummary.matchedKeyCount();
    argument << exposureSummary.maximumRiskScore();
    argument << exposureSummary.attenuationDurations();
    argument << exposureSummary.summationRiskScore();
    argument.endStructure();

    return argument;
}

QDBusArgument const &operator>>(const QDBusArgument &argument, ExposureSummary &exposureSummary)
{
    quint32 valueUnsignedInt;
    qint32 valueSignedInt;
    QList<qint32> valueList;

    argument.beginStructure();
    argument >> valueUnsignedInt;
    exposureSummary.setDaysSinceLastExposure(valueUnsignedInt);
    argument >> valueUnsignedInt;
    exposureSummary.setMatchedKeyCount(valueUnsignedInt);
    argument >> valueSignedInt;
    exposureSummary.setMaximumRiskScore(valueSignedInt);
    argument >> valueList;
    exposureSummary.setAttenuationDurations(valueList);
    argument >> valueSignedInt;
    exposureSummary.setSummationRiskScore(valueSignedInt);
    argument.endStructure();

    return argument;
}

quint32 ExposureSummary::daysSinceLastExposure() const
{
    return m_daysSinceLastExposure;
}

quint32 ExposureSummary::matchedKeyCount() const
{
    return m_matchedKeyCount;
}

qint32 ExposureSummary::maximumRiskScore() const
{
    return m_maximumRiskScore;
}

QList<qint32> ExposureSummary::attenuationDurations() const
{
    return m_attenuationDurations;
}

qint32 ExposureSummary::summationRiskScore() const
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

void ExposureSummary::setMaximumRiskScore(qint32 maximumRiskScore)
{
    if (m_maximumRiskScore != maximumRiskScore) {
        m_maximumRiskScore = maximumRiskScore;
        emit maximumRiskScoreChanged();
    }
}

void ExposureSummary::setAttenuationDurations(QList<qint32> attenuationDurations)
{
    if (m_attenuationDurations != attenuationDurations) {
        m_attenuationDurations = attenuationDurations;
        emit attenuationDurationsChanged();
    }
}

void ExposureSummary::setSummationRiskScore(qint32 summationRiskScore)
{
    if (m_summationRiskScore != summationRiskScore) {
        m_summationRiskScore = summationRiskScore;
        emit summationRiskScoreChanged();
    }
}



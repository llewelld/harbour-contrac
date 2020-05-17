#include "exposureconfiguration.h"

ExposureConfiguration::ExposureConfiguration(QObject *parent) : QObject(parent)
{

}

QList<quint32> ExposureConfiguration::attenuationScores() const
{
    return m_attenuationScores;
}

QList<quint32> ExposureConfiguration::daysSinceLastExposureScores() const
{
    return m_daysSinceLastExposureScores;
}

QList<quint32> ExposureConfiguration::durationScores() const
{
    return m_durationScores;
}

QList<quint32> ExposureConfiguration::transmissionRiskScores() const
{
    return m_transmissionRiskScores;
}

double ExposureConfiguration::attenuationWeight() const
{
    return m_attenuationWeight;
}

double ExposureConfiguration::daysSinceLastExposureWeight() const
{
    return m_daysSinceLastExposureWeight;
}

double ExposureConfiguration::durationWeight() const
{
    return m_durationWeight;
}

double ExposureConfiguration::transmissionRiskWeight() const
{
    return m_transmissionRiskWeight;
}

void ExposureConfiguration::setAttenuationScores(QList<quint32> attenuationScores)
{
    if (m_attenuationScores != attenuationScores) {
        m_attenuationScores = attenuationScores;
        emit attenuationScoresChanged();
    }
}

void ExposureConfiguration::setDaysSinceLastExposureScores(QList<quint32> daysSinceLastExposureScores)
{
    if (m_daysSinceLastExposureScores != daysSinceLastExposureScores) {
        m_daysSinceLastExposureScores = daysSinceLastExposureScores;
        emit daysSinceLastExposureScoresChanged();
    }
}

void ExposureConfiguration::setDurationScores(QList<quint32> durationScores)
{
    if (m_durationScores != durationScores) {
        m_durationScores = durationScores;
        emit durationScoresChanged();
    }
}

void ExposureConfiguration::setTransmissionRiskScores(QList<quint32> transmissionRiskScores)
{
    if (m_transmissionRiskScores != transmissionRiskScores) {
        m_transmissionRiskScores = transmissionRiskScores;
        emit transmissionRiskScoresChanged();
    }
}

void ExposureConfiguration::setAttenuationWeight(double attenuationWeight)
{
    if (m_attenuationWeight != attenuationWeight) {
        m_attenuationWeight = attenuationWeight;
        emit attenuationWeightChanged();
    }
}

void ExposureConfiguration::setDaysSinceLastExposureWeight(double daysSinceLastExposureWeight)
{
    if (m_daysSinceLastExposureWeight != daysSinceLastExposureWeight) {
        m_daysSinceLastExposureWeight = daysSinceLastExposureWeight;
        emit daysSinceLastExposureWeightChanged();
    }
}

void ExposureConfiguration::setDurationWeight(double durationWeight)
{
    if (m_durationWeight != durationWeight) {
        m_durationWeight = durationWeight;
        emit durationWeightChanged();
    }
}

void ExposureConfiguration::setTransmissionRiskWeight(double transmissionRiskWeight)
{
    if (m_transmissionRiskWeight != transmissionRiskWeight) {
        m_transmissionRiskWeight = transmissionRiskWeight;
        emit transmissionRiskWeightChanged();
    }
}


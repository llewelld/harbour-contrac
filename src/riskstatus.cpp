#include "settings.h"

#include "riskstatus.h"

RiskStatus::RiskStatus(QObject *parent)
    : QObject(parent)
    , m_combinedRiskScore(0.0)
    , m_maximumRiskScore(0)
{
    Settings &settings = Settings::getInstance();

    connect(&settings, &Settings::riskWeightsChanged, this, &RiskStatus::recalculate);
    connect(&settings, &Settings::defaultBuckeOffsetChanged, this, &RiskStatus::recalculate);
    connect(&settings, &Settings::normalizationDivisorChanged, this, &RiskStatus::recalculate);
    connect(&settings, &Settings::riskScoreClassesChanged, this, &RiskStatus::recalculate);
    connect(&settings, &Settings::latestSummaryChanged, this, &RiskStatus::updateExposureSummary);
}

void RiskStatus::recalculate()
{
    double combinedRiskScore = calculateRisk(m_maximumRiskScore, m_attenuationDurations);
    qint32 prevRiskClassIndex = riskClassIndex();
    if (m_combinedRiskScore != combinedRiskScore) {
        m_combinedRiskScore = combinedRiskScore;
        emit combinedRiskScoreChanged();
    }
    if (riskClassIndex() != prevRiskClassIndex) {
        emit riskClassChanged();
    }
}

double RiskStatus::combinedRiskScore() const
{
    return m_combinedRiskScore;
}

qint32 RiskStatus::riskClassIndex() const
{
    return calculateRiskClassIndex(m_combinedRiskScore);
}

QString RiskStatus::riskClassLabel() const
{
    return calculateRiskClassLabel(m_combinedRiskScore);
}

void RiskStatus::updateExposureSummary()
{
    ExposureSummary const *exposureSummary = Settings::getInstance().latestSummary();

    if (exposureSummary) {
        m_maximumRiskScore = exposureSummary->maximumRiskScore();
        m_attenuationDurations = exposureSummary->attenuationDurations();
        recalculate();
    }
}

double RiskStatus::calculateRisk(qint32 riskScore, QList<qint32> const &attenuationDurations) const
{
    double risk = 0.0;
    Settings &settings = Settings::getInstance();

    if ((attenuationDurations.size() == 3) && (settings.riskWeights().size() == 3) && (settings.normalizationDivisor() > 0)) {
        double exposureScore = 0.0;
        double normalizedScore = 0.0;
        exposureScore += attenuationDurations[0] * settings.riskWeights()[0];
        exposureScore += attenuationDurations[1] * settings.riskWeights()[1];
        exposureScore += attenuationDurations[2] * settings.riskWeights()[2];
        normalizedScore = double(riskScore) / double(settings.normalizationDivisor());

        risk = exposureScore * normalizedScore;
    }

    return risk;
}

qint32 RiskStatus::calculateRiskClassIndex(double combinedRiskScore) const
{
    qint32 pos = 0;
    qint32 result = -1;
    QList<RiskScoreClass> const &riskScoreClasses = Settings::getInstance().riskScoreClasses();
    while ((pos < riskScoreClasses.size()) && (result < 0)) {
        if ((combinedRiskScore >= riskScoreClasses[pos].min()) && (combinedRiskScore < riskScoreClasses[pos].max())) {
            result = pos;
        }
        ++pos;
    }
    return result;
}

QString RiskStatus::calculateRiskClassLabel(double combinedRiskScore) const
{
    qint32 index = calculateRiskClassIndex(combinedRiskScore);
    QString label;
    QList<RiskScoreClass> const &riskScoreClasses = Settings::getInstance().riskScoreClasses();

    if ((index >= 0) && (index < riskScoreClasses.size())) {
        label = riskScoreClasses[index].label().toLower();
        // Sentence case
        label[0] = label[0].toUpper();
    }

    return label;
}


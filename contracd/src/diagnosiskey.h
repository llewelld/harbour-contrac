#ifndef DIAGNOSISKEY_H
#define DIAGNOSISKEY_H

#include <QByteArray>

class DiagnosisKey
{
public:
    enum RiskLevel : quint32 {
        RiskLevelInvalid = 0,
        RiskLevelLowest = 1,
        RiskLevelLow = 2,
        RiskLevelLowMedium = 3,
        RiskLevelMedium = 4,
        RiskLevelMediumHigh = 5,
        RiskLevelHigh = 6,
        RiskLevelVeryHigh = 7,
        RiskLevelHighest = 8
    };

    DiagnosisKey();
    DiagnosisKey(QByteArray const &dtk, RiskLevel transmissionRiskLevel, quint32 rollingStartIntervalNumber, quint32 rollingPeriod);

    QByteArray m_dtk;
    RiskLevel m_transmissionRiskLevel;
    quint32 m_rollingStartIntervalNumber;
    quint32 m_rollingPeriod;
};

#endif // DIAGNOSISKEY_H

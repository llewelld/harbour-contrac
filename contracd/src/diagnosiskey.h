#ifndef DIAGNOSISKEY_H
#define DIAGNOSISKEY_H

#include <QByteArray>

class DiagnosisKey
{
public:
    DiagnosisKey();
    DiagnosisKey(QByteArray const &dtk, quint32 transmissionRiskLevel, quint32 rollingStartIntervalNumber, quint32 rollingPeriod);

    QByteArray m_dtk;
    quint32 m_transmissionRiskLevel;
    quint32 m_rollingStartIntervalNumber;
    quint32 m_rollingPeriod;
};

#endif // DIAGNOSISKEY_H

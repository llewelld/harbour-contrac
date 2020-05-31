#include "diagnosiskey.h"

DiagnosisKey::DiagnosisKey()
    : m_dtk()
    , m_transmissionRiskLevel(0)
    , m_rollingStartIntervalNumber(0)
    , m_rollingPeriod(0)
{
}

DiagnosisKey::DiagnosisKey(QByteArray const &dtk, quint32 transmissionRiskLevel, quint32 rollingStartIntervalNumber, quint32 rollingPeriod)
    : m_dtk(dtk)
    , m_transmissionRiskLevel(transmissionRiskLevel)
    , m_rollingStartIntervalNumber(rollingStartIntervalNumber)
    , m_rollingPeriod(rollingPeriod)
{
}

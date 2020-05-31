#include "temporaryexposurekey.h"

TemporaryExposureKey::TemporaryExposureKey(QObject *parent)
    : QObject(parent)
    , m_rollingStartNumber(0)
    , m_rollingPeriod(0)
    , m_transmissionRiskLevel(0)

{
}

TemporaryExposureKey::TemporaryExposureKey(TemporaryExposureKey const& temporaryExopsureKey)
    : QObject(temporaryExopsureKey.parent())
    , m_rollingStartNumber(temporaryExopsureKey.rollingStartNumber())
    , m_rollingPeriod(temporaryExopsureKey.rollingPeriod())
    , m_transmissionRiskLevel(temporaryExopsureKey.transmissionRiskLevel())
{
}

QByteArray TemporaryExposureKey::keyData() const
{
    return m_keyData;
}

quint32 TemporaryExposureKey::rollingStartNumber() const
{
    return m_rollingStartNumber;
}

quint32 TemporaryExposureKey::rollingPeriod() const
{
    return m_rollingPeriod;
}

quint8 TemporaryExposureKey::transmissionRiskLevel() const
{
    return m_transmissionRiskLevel;
}

void TemporaryExposureKey::setKeyData(QByteArray const &keyData)
{
    if (m_keyData != keyData) {
        m_keyData = keyData;
        emit keyDataChanged();
    }
}

void TemporaryExposureKey::setRollingStartNumber(quint32 rollingStartNumber)
{
    if (m_rollingStartNumber != rollingStartNumber) {
        m_rollingStartNumber = rollingStartNumber;
        emit rollingStartNumberChanged();
    }
}

void TemporaryExposureKey::setRollingPeriod(quint32 rollingPeriod)
{
    if (m_rollingPeriod != rollingPeriod) {
        m_rollingPeriod = rollingPeriod;
        emit rollingPeriodChanged();
    }
}

void TemporaryExposureKey::setTransmissionRiskLevel(quint8 transmissionRiskLevel)
{
    if (m_transmissionRiskLevel != transmissionRiskLevel) {
        m_transmissionRiskLevel = transmissionRiskLevel;
        emit transmissionRiskLevelChanged();
    }
}



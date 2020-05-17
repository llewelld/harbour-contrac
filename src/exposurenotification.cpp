#include <QList>

#include "temporaryexposurekey.h"
#include "contrac.pb.h"

#include "exposurenotification.h"

#define MAX_DIAGNOSIS_KEYS (1024)

ExposureNotification::ExposureNotification(QObject *parent)
    : QObject(parent)
    , m_status(Success)
{
    bool result;

    m_contrac = new Contrac(this);
    m_scanner = new BleScanner(this);
    m_controller = new Controller(this);
    m_contacts = new ContactStorage(m_contrac);

    result = m_contrac->loadTracingKey();
    if (!result) {
        m_contrac->generateTracingKey();
        m_contrac->saveTracingKey();
    }
}

ExposureNotification::~ExposureNotification()
{
}

bool ExposureNotification::isEnabled() const
{
    return m_scanner->scan();
}

quint32 ExposureNotification::maxDiagnosisKeys() const
{
    return MAX_DIAGNOSIS_KEYS;
}

ExposureNotification::Status ExposureNotification::status() const
{
    return m_status;
}

void ExposureNotification::start()
{
    if (!m_scanner->scan()) {
        m_scanner->setScan(true);
        m_controller->registerAdvert();

        connect(m_scanner, &BleScanner::beaconDiscovered, this, &ExposureNotification::beaconDiscovered);
    }
}

void ExposureNotification::stop()
{
    if (m_scanner->scan()) {
        disconnect(m_scanner, &BleScanner::beaconDiscovered, this, &ExposureNotification::beaconDiscovered);

        m_scanner->setScan(false);
        m_controller->unRegisterAdvert();
    }
}

// Return the last 14 days of history
QList<TemporaryExposureKey> ExposureNotification::getTemporaryExposureKeyHistory()
{
    QList<TemporaryExposureKey>  keys;
    quint32 today;
    quint32 day;

    today = m_contrac->dayNumber();
    day = today < 14 ? 0 : today - 14;
    while ( day < today) {
        QByteArray dtk;
        dtk = m_contrac->dailyTracingKey(m_contrac->tk(), day);
        TemporaryExposureKey key(this);
        key.setKeyData(dtk);
        key.setRollingStartNumber(day * 144);
        key.setRollingPeriod(144);
        // TODO: Figure out where this is supposed to come from
        key.setTransmissionRiskLevel(TemporaryExposureKey::RiskLevelMedium);
        keys.append(key);
    }

    return keys;
}

void ExposureNotification::provideDiagnosisKeys(QVector<QString> const &keyFiles, ExposureConfiguration const &configuration, QString token)
{
}

void ExposureNotification::loadDiagnosisKeys(QString const &keyFile, ExposureConfiguration const &configuration, QString token)
{



}

ExposureSummary const &ExposureNotification::getExposureSummary(QString const &token)
{
}

QList<ExposureInformation> ExposureNotification::getExposureInformation(QString const &token)
{
}

quint32 ExposureNotification::getMaxDiagnosisKeys() const
{
    return maxDiagnosisKeys();
}

void ExposureNotification::resetAllData()
{
    m_contrac->generateTracingKey();
    m_contrac->saveTracingKey();
    m_contacts->clearAllDataFiles();
}

void ExposureNotification::beaconDiscovered(const QString &address, const QByteArray &rpi, qint16 rssi)
{
    m_contacts->addContact(m_contrac->timeIntervalNumber(), rpi, rssi);
}

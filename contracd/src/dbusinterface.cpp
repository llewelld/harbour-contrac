#include <QDebug>

#include "settings.h"

#include "dbusinterface.h"

DBusInterface::DBusInterface(QObject *parent)
    : QObject(parent)
    , m_connection(QDBusConnection::sessionBus())
{
    bool result;

    qDebug() << "CONTRAC: Initialising the dbus interface";

    Settings &settings = Settings::getInstance();

    qDBusRegisterMetaType<TemporaryExposureKey>();
    qDBusRegisterMetaType<ExposureInformation>();
    qDBusRegisterMetaType<ExposureSummary>();
    qDBusRegisterMetaType<ExposureConfiguration>();
    qDBusRegisterMetaType<QList<TemporaryExposureKey>>();
    qDBusRegisterMetaType<QList<ExposureInformation>>();

    connect(&m_exposureNotification, &ExposureNotification::statusChanged, this, &DBusInterface::statusChanged);
    connect(&m_exposureNotification, &ExposureNotification::isEnabledChanged, this, &DBusInterface::isEnabledChanged);
    connect(&m_exposureNotification, &ExposureNotification::beaconSent, this, &DBusInterface::incrementSentCount);
    connect(&m_exposureNotification, &ExposureNotification::beaconReceived, this, &DBusInterface::incrementReceiveCount);
    connect(&m_exposureNotification, &ExposureNotification::isBusyChanged, this, &DBusInterface::isBusyChanged);
    connect(&m_exposureNotification, &ExposureNotification::actionExposureStateUpdated, this, &DBusInterface::actionExposureStateUpdated);

    connect(&settings, &Settings::txPowerChanged, this, &DBusInterface::txPowerChanged);
    connect(&settings, &Settings::rssiCorrectionChanged, this, &DBusInterface::rssiCorrectionChanged);

    result = m_connection.registerService(QStringLiteral(SERVICE_NAME));
    qDebug() << "CONTRAC: service registration: " << result;

    if (result) {
        result = m_connection.registerObject(SERVICE_PATH, this, QDBusConnection::ExportAllInvokables | QDBusConnection::ExportAllSignals | QDBusConnection::ExportAllProperties);
        qDebug() << "CONTRAC: object registration: " << result;
    }

    qDebug() << "CONTRAC: DBus registered: " << result;

    if (!result) {
        qDebug() << "CONTRAC: Error initialising dbus interface";
    }

    if (settings.enabled()) {
        qDebug() << "Starting automatically";
        m_exposureNotification.start();
    }
}

DBusInterface::~DBusInterface()
{
    bool result;

    disconnect(&m_exposureNotification, &ExposureNotification::statusChanged, this, &DBusInterface::statusChanged);
    disconnect(&m_exposureNotification, &ExposureNotification::isEnabledChanged, this, &DBusInterface::isEnabledChanged);
    disconnect(&m_exposureNotification, &ExposureNotification::beaconSent, this, &DBusInterface::incrementSentCount);
    disconnect(&m_exposureNotification, &ExposureNotification::beaconReceived, this, &DBusInterface::incrementReceiveCount);
    disconnect(&m_exposureNotification, &ExposureNotification::isBusyChanged, this, &DBusInterface::isBusyChanged);
    disconnect(&m_exposureNotification, &ExposureNotification::actionExposureStateUpdated, this, &DBusInterface::actionExposureStateUpdated);

    Settings &settings = Settings::getInstance();
    disconnect(&settings, &Settings::txPowerChanged, this, &DBusInterface::txPowerChanged);
    disconnect(&settings, &Settings::rssiCorrectionChanged, this, &DBusInterface::rssiCorrectionChanged);

    m_connection.unregisterObject(SERVICE_PATH);
    result = m_connection.unregisterService(QStringLiteral(SERVICE_NAME));
    qDebug() << "CONTRAC: DBus unregistration: " << result;

    if (!result) {
        qDebug() << "CONTRAC: Error unregistering service";
    }
}

void DBusInterface::start()
{
    qDebug() << "CONTRAC: start()";
    m_exposureNotification.start();
}

void DBusInterface::stop()
{
    qDebug() << "CONTRAC: stop()";
    m_exposureNotification.stop();
}

ExposureNotification::Status DBusInterface::status() const
{
    qDebug() << "CONTRAC: statis()";
    return m_exposureNotification.status();
}

bool DBusInterface::isEnabled() const
{
    qDebug() << "CONTRAC: isEnabled()";
    return m_exposureNotification.isEnabled();
}

bool DBusInterface::isBusy() const
{
    qDebug() << "CONTRAC: isBusy()";
    return m_exposureNotification.isBusy();
}

quint32 DBusInterface::maxDiagnosisKeys() const
{
    qDebug() << "CONTRAC: maxDiagnosisKeys()";
    return m_exposureNotification.maxDiagnosisKeys();
}

QList<TemporaryExposureKey> DBusInterface::getTemporaryExposureKeyHistory()
{
    qDebug() << "CONTRAC: getTemporaryExposureKeyHistory()";
    QList<TemporaryExposureKey> keys = m_exposureNotification.getTemporaryExposureKeyHistory();
    qDebug() << "CONTRAC: Sending " << keys.size() << "keys";

    return keys;
}

// This is potentially very long-running. According to the D-bus docs, the method
// should still return the result in the normal way, with the client waiting for
// the result asynchronously. However, to better match the GAEN API, we signal
// that the result is available on completion using the actionExposureStateUpdated()
// signal instead.
void DBusInterface::provideDiagnosisKeys(QStringList const &keyFiles, ExposureConfiguration const &configuration, QString token)
{
    qDebug() << "CONTRAC: provideDiagnosisKeys()";

    qDebug() << "Minimum risk score set to: " << configuration.minimumRiskScore();
    qDebug() << "Attenuation scores set to: " << configuration.attenuationScores();
    qDebug() << "Days Since Last Exposure scores set to: " << configuration.daysSinceLastExposureScores();
    qDebug() << "Duration scores set to: " << configuration.durationScores();
    qDebug() << "Transmission Risk scores set to: " << configuration.transmissionRiskScores();
    qDebug() << "Duration At Attenuation Thresholds set to: " << configuration.durationAtAttenuationThresholds();

    m_exposureNotification.provideDiagnosisKeys(keyFiles.toVector(), configuration, token);
}

ExposureSummary DBusInterface::getExposureSummary(QString const &token)
{
    qDebug() << "CONTRAC: getExposureSummary()";
    return m_exposureNotification.getExposureSummary(token);
}

QList<ExposureInformation> DBusInterface::getExposureInformation(QString const &token)
{
    qDebug() << "CONTRAC: getExposureInformation()";
    return m_exposureNotification.getExposureInformation(token);
}

quint32 DBusInterface::getMaxDiagnosisKeys() const
{
    qDebug() << "CONTRAC: getMaxDiagnosisKeys()";
    return m_exposureNotification.getMaxDiagnosisKeys();
}

void DBusInterface::resetAllData()
{
    qDebug() << "CONTRAC: resetAllData()";
    m_exposureNotification.resetAllData();
}

quint32 DBusInterface::receivedCount() const
{
    return Settings::getInstance().received();
}

quint32 DBusInterface::sentCount() const
{
    return Settings::getInstance().sent();
}

void DBusInterface::incrementReceiveCount()
{
    qDebug() << "CONTRAC: incrementReceiveCount()";
    Settings &settings = Settings::getInstance();
    settings.setReceived(settings.received() + 1);
    emit receivedCountChanged();
}

void DBusInterface::incrementSentCount()
{
    qDebug() << "CONTRAC: incrementSentCount()";
    Settings &settings = Settings::getInstance();
    settings.setSent(settings.sent() + 1);
    emit sentCountChanged();
}

qint32 DBusInterface::txPower() const
{
    return qint32(Settings::getInstance().txPower());
}

void DBusInterface::setTxPower(qint32 txPower)
{
    txPower = qBound(INT8_MIN, txPower, INT8_MAX);
    Settings::getInstance().setTxPower(qint8(txPower));
}

qint32 DBusInterface::rssiCorrection() const
{
    return qint16(Settings::getInstance().rssiCorrection());
}

void DBusInterface::setRssiCorrection(qint32 rssiCorrection)
{
    rssiCorrection = qBound(INT8_MIN, rssiCorrection, INT8_MAX);
    Settings::getInstance().setRssiCorrection(qint8(rssiCorrection));
}


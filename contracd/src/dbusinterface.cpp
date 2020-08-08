#include <QDebug>

#include "dbusinterface.h"

DBusInterface::DBusInterface(QObject *parent)
    : QObject(parent)
    , m_connection(QDBusConnection::sessionBus())
    , m_sentCount(0)
    , m_receivedCount(0)
{
    bool result;

    qDebug() << "CONTRAC: Initialising the dbus interface";

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
}

DBusInterface::~DBusInterface()
{
    bool result;

    disconnect(&m_exposureNotification, &ExposureNotification::statusChanged, this, &DBusInterface::statusChanged);
    disconnect(&m_exposureNotification, &ExposureNotification::isEnabledChanged, this, &DBusInterface::isEnabledChanged);
    disconnect(&m_exposureNotification, &ExposureNotification::beaconSent, this, &DBusInterface::incrementSentCount);
    disconnect(&m_exposureNotification, &ExposureNotification::beaconReceived, this, &DBusInterface::incrementReceiveCount);
    disconnect(&m_exposureNotification, &ExposureNotification::isBusyChanged, this, &DBusInterface::isBusyChanged);

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

void DBusInterface::provideDiagnosisKeys(QStringList const &keyFiles, ExposureConfiguration const &configuration, QString token)
{
    qDebug() << "CONTRAC: provideDiagnosisKeys()";
    m_exposureNotification.provideDiagnosisKeys(keyFiles.toVector(), configuration, token);
}

ExposureSummary DBusInterface::getExposureSummary(QString const &token) const
{
    qDebug() << "CONTRAC: getExposureSummary()";
    return m_exposureNotification.getExposureSummary(token);
}

QList<ExposureInformation> DBusInterface::getExposureInformation(QString const &token) const
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
    return m_receivedCount;
}

quint32 DBusInterface::sentCount() const
{
    return m_sentCount;
}

void DBusInterface::incrementReceiveCount()
{
    qDebug() << "CONTRAC: incrementReceiveCount()";
    ++m_receivedCount;
    qDebug() << "CONTRAC: receiveCount:" << m_receivedCount;
    emit receivedCountChanged();
}

void DBusInterface::incrementSentCount()
{
    qDebug() << "CONTRAC: incrementSentCount()";
    ++m_sentCount;
    qDebug() << "CONTRAC: receiveCount:" << m_sentCount;
    emit sentCountChanged();
}



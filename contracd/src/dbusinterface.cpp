#include <QDebug>

#include "dbusinterface.h"

DBusInterface::DBusInterface(QObject *parent)
    : QObject(parent)
    , m_connection(QDBusConnection::sessionBus())
{
    bool result;

    qDebug() << "CONTRAC: Initialising the dbus interface";

    connect(&m_exposureNotification, &ExposureNotification::statusChanged, this, &DBusInterface::statusChanged);
    connect(&m_exposureNotification, &ExposureNotification::isEnabledChanged, this, &DBusInterface::isEnabledChanged);

    result = m_connection.registerService(QStringLiteral(SERVICE_NAME));
    qDebug() << "CONTRAC: service registration: " << result;

    if (result) {
        result = m_connection.registerObject(SERVICE_PATH, this, QDBusConnection::ExportAllInvokables | QDBusConnection::ExportAllSignal |QDBusConnection::ExportAllProperties);
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

quint32 DBusInterface::maxDiagnosisKeys() const
{
    qDebug() << "CONTRAC: maxDiagnosisKeys()";
    return m_exposureNotification.maxDiagnosisKeys();
}

QList<TemporaryExposureKey> DBusInterface::getTemporaryExposureKeyHistory()
{
    qDebug() << "CONTRAC: getTemporaryExposureKeyHistory()";
    return m_exposureNotification.getTemporaryExposureKeyHistory();
}

void DBusInterface::provideDiagnosisKeys(QVector<QString> const &keyFiles, ExposureConfiguration const &configuration, QString token)
{
    qDebug() << "CONTRAC: provideDiagnosisKeys()";
    m_exposureNotification.provideDiagnosisKeys(keyFiles, configuration, token);
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


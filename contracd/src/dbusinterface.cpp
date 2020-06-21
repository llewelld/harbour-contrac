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
    qDBusRegisterMetaType<TemporaryExposureKeyList>();
    qDBusRegisterMetaType<ExposureInformationList>();

    connect(&m_exposureNotification, &ExposureNotification::statusChanged, this, &DBusInterface::statusChanged);
    connect(&m_exposureNotification, &ExposureNotification::isEnabledChanged, this, &DBusInterface::isEnabledChanged);
    connect(&m_exposureNotification, &ExposureNotification::beaconSent, this, &DBusInterface::incrementSentCount);
    connect(&m_exposureNotification, &ExposureNotification::beaconReceived, this, &DBusInterface::incrementReceiveCount);

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

QDBusArgument &operator<<(QDBusArgument &argument, const ExposureInformationList &exposureInformationList)
{
    argument.beginArray();
    for (ExposureInformation const & exposureInformation : exposureInformationList) {
        argument << exposureInformation;
    }
    argument.endArray();

    return argument;
}

QDBusArgument const &operator>>(const QDBusArgument &argument, ExposureInformationList &exposureInformationList)
{
    argument.beginArray();
    while (!argument.atEnd()) {
        ExposureInformation exposureInformation;
        argument >> exposureInformation;
        exposureInformationList.append(exposureInformation);
    }
    argument.endArray();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const TemporaryExposureKeyList &temporaryExposureKeyList)
{
    argument.beginArray();
    for (TemporaryExposureKey const & temporaryExposureKey : temporaryExposureKeyList) {
        argument << temporaryExposureKey;
    }
    argument.endArray();

    return argument;
}

QDBusArgument const &operator>>(const QDBusArgument &argument, TemporaryExposureKeyList &temporaryExposureKeyList)
{
    argument.beginArray();
    while (!argument.atEnd()) {
        TemporaryExposureKey temporaryExposureKey;
        argument >> temporaryExposureKey;
        temporaryExposureKeyList.append(temporaryExposureKey);
    }
    argument.endArray();

    return argument;
}

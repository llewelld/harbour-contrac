#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDBusReply>
#include <QDebug>

#include "dbusproxy.h"

#define SERVICE_NAME "uk.co.flypig.contrac"

// D-bus timeout in milliseconds
// Some of the calls such as provideDiagnosisKeys() are potentially very long running
#define DBUS_PROXY_TIMEOUT (10 * 60 * 1000)

DBusProxy::DBusProxy(QObject *parent)
    : QObject(parent)
    , m_receivedCount(0)
    , m_sentCount(0)
{
    bool result;

    qDBusRegisterMetaType<TemporaryExposureKey>();
    qDBusRegisterMetaType<ExposureInformation>();
    qDBusRegisterMetaType<ExposureSummary>();
    qDBusRegisterMetaType<ExposureConfiguration>();
    qDBusRegisterMetaType<QList<TemporaryExposureKey>>();
    qDBusRegisterMetaType<QList<ExposureInformation>>();

    m_interface = new QDBusInterface(QStringLiteral(SERVICE_NAME), QStringLiteral("/"), QString(), QDBusConnection::sessionBus(), this);
    m_interface->setTimeout(DBUS_PROXY_TIMEOUT);

    QStringList argumentMatch;
    //argumentMatch.append("uk.co.flypig.contrac");
    QString signature;

    qDebug() << "Connecting property change";
    result = QDBusConnection::sessionBus().connect("uk.co.flypig.contrac", "/", "uk.co.flypig.contrac", "receivedCountChanged", argumentMatch, signature, this, SIGNAL(receivedCountChanged()));
    qDebug() << "Connection receivedCountChanged result: " << result;

    result = QDBusConnection::sessionBus().connect("uk.co.flypig.contrac", "/", "uk.co.flypig.contrac", "sentCountChanged", argumentMatch, signature, this, SIGNAL(sentCountChanged()));
    qDebug() << "Connection sentCountChanged result: " << result;

    result = QDBusConnection::sessionBus().connect("uk.co.flypig.contrac", "/", "uk.co.flypig.contrac", "statusChanged", argumentMatch, signature, this, SIGNAL(statusChanged()));
    qDebug() << "Connection statusChanged result: " << result;

    result = QDBusConnection::sessionBus().connect("uk.co.flypig.contrac", "/", "uk.co.flypig.contrac", "isEnabledChanged", argumentMatch, signature, this, SIGNAL(isEnabledChanged()));
    qDebug() << "Connection isEnabledChanged result: " << result;

    result = QDBusConnection::sessionBus().connect("uk.co.flypig.contrac", "/", "uk.co.flypig.contrac", "isBusyChanged", argumentMatch, signature, this, SIGNAL(isBusyChanged()));
    qDebug() << "Connection isBusyChanged result: " << result;

    result = QDBusConnection::sessionBus().connect("uk.co.flypig.contrac", "/", "uk.co.flypig.contrac", "txPowerChanged", argumentMatch, signature, this, SIGNAL(txPowerChanged()));
    qDebug() << "Connection txPowerChanged result: " << result;

    result = QDBusConnection::sessionBus().connect("uk.co.flypig.contrac", "/", "uk.co.flypig.contrac", "rssiCorrectionChanged", argumentMatch, signature, this, SIGNAL(rssiCorrectionChanged()));
    qDebug() << "Connection rssiCorrectionChanged result: " << result;

    argumentMatch.append(QStringLiteral("s"));
    signature = QStringLiteral("s");
    result = QDBusConnection::sessionBus().connect("uk.co.flypig.contrac", "/", "uk.co.flypig.contrac", "actionExposureStateUpdated", this, SIGNAL(actionExposureStateUpdated(QString)));
    qDebug() << "Connection actionExposureStateUpdated result: " << result;

    result = QDBusConnection::sessionBus().connect("uk.co.flypig.contrac", "/", "uk.co.flypig.contrac", "exposureStateChanged", this, SIGNAL(exposureStateChanged(QString)));
    qDebug() << "Connection exposureStateChanged result: " << result;
}

DBusProxy::~DBusProxy()
{
}

void DBusProxy::start()
{
    QDBusPendingCall async = m_interface->asyncCall("start");

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [](QDBusPendingCallWatcher *call) {
        qDebug() << "DBus start returned";

        QDBusPendingReply<> reply = *call;
        if (reply.isError()) {
            if ((reply.error().type() == QDBusError::ErrorType::NotSupported) || (reply.error().type() == QDBusError::ErrorType::Other)) {
                qDebug() << "Error calling start";
            }
            else {
                qDebug() << "Error: " << reply.error().message();
            }
        }
        else {
            qDebug() << "Success calling start";
        }
        call->deleteLater();
    });
}

void DBusProxy::stop()
{
    QDBusPendingCall async = m_interface->asyncCall("stop");

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [](QDBusPendingCallWatcher *call) {
        qDebug() << "DBus stop returned";

        QDBusPendingReply<> reply = *call;
        if (reply.isError()) {
            if ((reply.error().type() == QDBusError::ErrorType::NotSupported) || (reply.error().type() == QDBusError::ErrorType::Other)) {
                qDebug() << "Error calling stop";
            }
            else {
                qDebug() << "Error: " << reply.error().message();
            }
        }
        else {
            qDebug() << "Success calling stop";
        }
        call->deleteLater();
    });
}

quint32 DBusProxy::receivedCount() const
{
    QDBusReply<quint32> reply = m_interface->call("receivedCount");
    return reply;
}

quint32 DBusProxy::sentCount() const
{
    QDBusReply<quint32> reply = m_interface->call("sentCount");
    return reply;
}

DBusProxy::Status DBusProxy::status() const
{
    QDBusReply<qint32> reply = m_interface->call("status");
    return DBusProxy::Status(reply.value());
}

bool DBusProxy::isEnabled() const
{
    QDBusReply<bool> reply = m_interface->call("isEnabled");
    return reply;
}

bool DBusProxy::isBusy() const
{
    QDBusReply<bool> reply = m_interface->call("isBusy");
    return reply;
}

quint32 DBusProxy::getMaxDiagnosisKeys() const
{
    QDBusReply<quint32> reply = m_interface->call("maxDiagnosisKeys");
    return reply;
}

void DBusProxy::resetAllData()
{
    m_interface->call("resetAllData");
}

ExposureSummary *DBusProxy::getExposureSummary(QString const &token) const
{
    QDBusReply<ExposureSummary> reply = m_interface->call("getExposureSummary", token);
    ExposureSummary *result = new ExposureSummary(reply);
    result->setParent(nullptr);

    return result;
}

QList<TemporaryExposureKey> DBusProxy::getTemporaryExposureKeyHistory()
{
    qDebug() << "Calling dbus";
    QDBusReply<QList<TemporaryExposureKey>> reply = m_interface->call("getTemporaryExposureKeyHistory");
    QList<TemporaryExposureKey> result(reply);
    qDebug() << "Returning " << result.size() << " keys";

    return result;
}

void DBusProxy::provideDiagnosisKeys(QStringList const &keyFiles, ExposureConfiguration *configuration, QString token)
{
    qDebug() << "Calling provideDiagnosisKeys";
    QVariant configurationVariant;
    configurationVariant.setValue(*configuration);

    qDebug() << "Minimum risk score set to: " << configuration->minimumRiskScore();
    qDebug() << "Attenuation scores set to: " << configuration->attenuationScores();
    qDebug() << "Days Since Last Exposure scores set to: " << configuration->daysSinceLastExposureScores();
    qDebug() << "Duration scores set to: " << configuration->durationScores();
    qDebug() << "Transmission Risk scores set to: " << configuration->transmissionRiskScores();
    qDebug() << "Duration At Attenuation Thresholds set to: " << configuration->durationAtAttenuationThresholds();

    QDBusMessage reply = m_interface->call("provideDiagnosisKeys", keyFiles, configurationVariant, token);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "DBus error providing diagnosis keys: " << reply.errorMessage();
    }
}

QList<ExposureInformation> *DBusProxy::getExposureInformation(QString const &token) const
{
    QDBusReply<QList<ExposureInformation>> reply = m_interface->call("getExposureInformation", token);
    QList<ExposureInformation> *result = new QList<ExposureInformation>(reply);

    return result;
}

qint32 DBusProxy::txPower() const
{
    QDBusReply<qint32> reply = m_interface->call("txPower");
    return reply;
}

void DBusProxy::setTxPower(qint32 txPower)
{
    QDBusMessage reply = m_interface->call("setTxPower", txPower);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "DBus error setting txPower: " << reply.errorMessage();
    }
}

qint32 DBusProxy::rssiCorrection() const
{
    QDBusReply<qint32> reply = m_interface->call("rssiCorrection");
    return reply;
}

void DBusProxy::setRssiCorrection(qint32 rssiCorrection)
{
    QDBusMessage reply = m_interface->call("setRssiCorrection", rssiCorrection);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "DBus error setting rssiCorrection: " << reply.errorMessage();
    }
}

DBusProxy::ExposureState DBusProxy::exposureState(QString const &token) const
{
    QDBusReply<qint32> reply = m_interface->call("exposureState", token);
    return DBusProxy::ExposureState(reply.value());
}

QDateTime DBusProxy::lastProcessTime(QString const token) const
{
    QDBusReply<QDateTime> reply = m_interface->call("lastProcessTime", token);
    return reply;
}

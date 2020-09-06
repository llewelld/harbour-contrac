#include <QDebug>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusReply>
#include <QDBusMetaType>
#include <QDBusPendingCallWatcher>

#include "dbusproxy.h"

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
}

DBusProxy::~DBusProxy()
{
}

void DBusProxy::start()
{
    QDBusPendingCall async = m_interface->asyncCall("start");

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [](QDBusPendingCallWatcher* call) {
        qDebug() << "DBus start returned";

        QDBusPendingReply<> reply = *call;
        if (reply.isError()) {
            if ((reply.error().type() == QDBusError::ErrorType::NotSupported)
                    || (reply.error().type() == QDBusError::ErrorType::Other)) {
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

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [](QDBusPendingCallWatcher* call) {
        qDebug() << "DBus stop returned";

        QDBusPendingReply<> reply = *call;
        if (reply.isError()) {
            if ((reply.error().type() == QDBusError::ErrorType::NotSupported)
                    || (reply.error().type() == QDBusError::ErrorType::Other)) {
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
    QDBusReply<DBusProxy::Status> reply = m_interface->call("status");
    return reply;
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

void DBusProxy::provideDiagnosisKeys(QStringList const &keyFiles, ExposureConfiguration const &configuration, QString token)
{
    qDebug() << "Calling provideDiagnosisKeys";
    QVariant configurationVariant;
    configurationVariant.setValue(configuration);

    QList<QVariant> args;
    args << keyFiles << configurationVariant << token;

    QDBusPendingCall async = m_interface->asyncCall("provideDiagnosisKeys", keyFiles, configurationVariant, token);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, token](QDBusPendingCallWatcher *call){
        QDBusPendingReply<> reply = *call;
        if (reply.isError()) {
            qDebug() << "D-Bus returned failure:" << reply.error().message();
            emit provideDiagnosisKeysResult(FailedInternal, token);
        }
        else {
            qDebug() << "D-Bus returned success:" << reply.error().message();
            emit provideDiagnosisKeysResult(Success, token);
        }

        call->deleteLater();
    });
}

QList<ExposureInformation> *DBusProxy::getExposureInformation(QString const &token) const
{
    QDBusReply<QList<ExposureInformation>> reply = m_interface->call("getExposureInformation", token);
    QList<ExposureInformation> *result = new QList<ExposureInformation>(reply);

    return result;
}


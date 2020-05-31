#include <QDebug>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusReply>

#include "dbusproxy.h"

DBusProxy::DBusProxy(QObject *parent)
    : QObject(parent)
    , m_receivedCount(0)
    , m_sentCount(0)
{
    bool result;
    m_interface = new QDBusInterface(QStringLiteral(SERVICE_NAME), QStringLiteral("/"), QString(), QDBusConnection::sessionBus(), this);

    QStringList argumentMatch;
    //argumentMatch.append("uk.co.flypig.contrac");
    QString signature;

    qDebug() << "Connecting property change";
    result = QDBusConnection::sessionBus().connect("uk.co.flypig.contrac", "/", "uk.co.flypig.contrac", "receivedCountChanged", argumentMatch, signature, this, SLOT(onReceiveCountChanged()));
    qDebug() << "Connection receivedCountChanged result: " << result;

    result = QDBusConnection::sessionBus().connect("uk.co.flypig.contrac", "/", "uk.co.flypig.contrac", "sentCountChanged", argumentMatch, signature, this, SLOT(onSentCountChanged()));
    qDebug() << "Connection sentCountChanged result: " << result;
}

DBusProxy::~DBusProxy()
{
}

void DBusProxy::onReceiveCountChanged()
{
    qDebug() << "onReceiveCountChanged";
    QDBusReply<quint32> reply = m_interface->call("receivedCount");
    m_receivedCount = reply;
    qDebug() << "onReceiveCountChanged: " << m_receivedCount;
    emit receivedCountChanged();
}

void DBusProxy::onSentCountChanged()
{
    qDebug() << "onSentCountChanged";
    QDBusReply<quint32> reply = m_interface->call("sentCount");
    m_sentCount = reply;
    qDebug() << "onSentCountChanged: " << m_sentCount;
    emit sentCountChanged();
}

void DBusProxy::onPropertiesChanged(const QString &interface, const QVariantMap &changed, const QStringList &invalidated)
{
    qDebug() << "Property changed";
    qDebug() << "   Interface: " << interface;
    for (const QString &key : changed.keys()) {
        qDebug() << "   Changed: " << key << " = " << changed[key];
        if (key == "receivedCount") {
            m_receivedCount = changed[key].toUInt();
            emit receivedCountChanged();
        }
        if (key == "sentCount") {
            m_sentCount = changed[key].toUInt();
            emit sentCountChanged();
        }
    }
    for (const QString &invalid : invalidated) {
        qDebug() << "   Invalidated: " << invalid;
    }
}

void DBusProxy::start()
{
    QDBusPendingCall async = m_interface->asyncCall("start");

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher* call) {
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

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher* call) {
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
    return m_receivedCount;
}

quint32 DBusProxy::sentCount() const
{
    return m_sentCount;
}


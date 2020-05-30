#include <QDebug>

#include "dbusinterface.h"

DBusInterface::DBusInterface(QObject *parent)
    : QObject(parent)
    , m_connection(QDBusConnection::sessionBus())
{
    bool result;

    qDebug() << "CONTRAC: Initialising the dbus interface";

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
}

void DBusInterface::stop()
{
    qDebug() << "CONTRAC: stop()";
}


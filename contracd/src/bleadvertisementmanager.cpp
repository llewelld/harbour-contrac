/*
 * Copyright 2020, David Llewellyn-Jones <david@flypig.co.uk>
 * Released under the GPLv2
 *
 * More info at: https://www.flypig.co.uk/contrac
 *
 */

#include <QDBusConnection>
#include <QDebug>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QDBusObjectPath>

#include "bleadvertisement.h"
#include "bleadvertisementmanager.h"

BleAdvertisementManager::BleAdvertisementManager(QObject *parent)
    : QObject(parent)
    , m_interface(nullptr)
{
}

void BleAdvertisementManager::connectDBus()
{
    QString path = QStringLiteral("/org/bluez/hci0");

    qDebug() << "Registering interface";
    m_interface = new QDBusInterface("org.bluez", path, "org.bluez.LEAdvertisingManager1", QDBusConnection::systemBus(), this);

    QStringList argumentMatch;
    argumentMatch.append("org.bluez.LEAdvertisingManager1");
    QString signature;

    qDebug() << "Connecting property change";
    QDBusConnection::systemBus().connect("org.bluez", path, "org.freedesktop.DBus.Properties", "PropertiesChanged", argumentMatch, signature, this, SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));
}

BleAdvertisementManager::~BleAdvertisementManager()
{
    qDebug() << "~BleAdvertisementManager() start";
    QString path = QStringLiteral("/org/bluez/hci0");
    QStringList argumentMatch;
    argumentMatch.append("org.bluez.LEAdvertisingManager1");
    QString signature;

    QDBusConnection::systemBus().disconnect("org.bluez", path, "org.freedesktop.DBus.Properties", "PropertiesChanged", argumentMatch, signature, this, SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));
    qDebug() << "~BleAdvertisementManager() finish";
}

void BleAdvertisementManager::onPropertiesChanged(const QString &interface, const QVariantMap &changed, const QStringList &invalidated)
{
    qDebug() << "Property changed";
    qDebug() << "   Interface: " << interface;
    for (const QString &key : changed.keys()) {
        qDebug() << "   Changed: " << key << " = " << changed[key];
    }
    for (const QString &invalid : invalidated) {
        qDebug() << "   Invalidated: " << invalid;
    }
}

void BleAdvertisementManager::registerAdvertisement(BleAdvertisement * advertisement)
{
    qDebug() << "Registering advertisement: " << advertisement->getPath();
    QVariantMap options;

    QDBusObjectPath path(advertisement->getPath());
    QVariant argument = QVariant::fromValue(path);
    QDBusPendingCall async = m_interface->asyncCall("RegisterAdvertisement", argument, options);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher* call) {
        qDebug() << "DBus RegisterAdvertisement call returned";

        QDBusPendingReply<> reply = *call;
        if (reply.isError()) {
            qDebug() << "Error: " << reply.error().message();
        }
        else {
            qDebug() << "No error";
        }
        call->deleteLater();
    });
}

void BleAdvertisementManager::unRegisterAdvertisement(BleAdvertisement * advertisement)
{
    qDebug() << "Unregistering advertisement: " << advertisement->getPath();
    QVariantMap options;

    QDBusObjectPath path(advertisement->getPath());
    QVariant argument = QVariant::fromValue(path);
    QDBusPendingCall async = m_interface->asyncCall("UnregisterAdvertisement", argument);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher* call) {
        qDebug() << "DBus UnregisterAdvertisement call returned";

        QDBusPendingReply<> reply = *call;
        if (reply.isError()) {
            qDebug() << "Error: " << reply.error().message();
        }
        else {
            qDebug() << "No error";
        }
        call->deleteLater();
    });
}


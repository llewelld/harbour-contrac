/*
 * Copyright 2020, David Llewellyn-Jones <david@flypig.co.uk>
 * Released under the GPLv2
 *
 * More info at: https://www.flypig.co.uk/contrac
 *
 */

#include <QDBusConnection>
#include <QDebug>

#include "bleadvertisement.h"

#define EXPORT_OPTIONS (QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllInvokables | QDBusConnection::ExportAllSignals | QDBusConnection::ExportAllProperties)
#define BLE_APPEARANCE_GENERIC_PHONE (64)

BleAdvertisement::BleAdvertisement(QObject * parent)
    : QDBusAbstractAdaptor(parent)
    , m_path("/")
    , m_registered(false)
    , m_type("broadcast")
    , m_serviceUUIDs()
    , m_manufacturerData()
    , m_solicitUUIDs()
    , m_serviceData()
    , m_includes()
    , m_localName("SailfishOS")
    , m_appearance(BLE_APPEARANCE_GENERIC_PHONE)
    , m_duration(2)
    , m_timeout(60)
{
}

BleAdvertisement::~BleAdvertisement()
{
    qDebug() << "~BleAdvertisement()";
}

void BleAdvertisement::registerDBus(const QString &path)
{
    bool result;

    if (!m_registered) {
        qDebug() << "Registering: " << path;
        m_path = path;

        qDebug() << "Connecting";
        result = QDBusConnection::sessionBus().isConnected();

        if (!result) {
            qDebug() << "Not connected";
        }

        qDebug() << "Registering service";
        result = QDBusConnection::systemBus().registerService(BLE_SERVICE_NAME);
        if (!result) {
            qDebug() << "Service registration failed";
        }

        qDebug() << "Registering object";
        result = QDBusConnection::systemBus().registerObject(m_path, QStringLiteral(BLE_OBJECT_INTERFACE), this, EXPORT_OPTIONS);
        if (!result) {
            qDebug() << "Object registration failed";
        }
        m_registered = true;
    }
}

void BleAdvertisement::unRegisterDBus()
{
    if (m_registered) {
        QDBusConnection::systemBus().unregisterObject(m_path);

        QDBusConnection::systemBus().unregisterService(BLE_SERVICE_NAME);

        m_registered = false;
    }
}

QString BleAdvertisement::getPath() const
{
    return m_path;
}

void BleAdvertisement::Release()
{
    // Do nothing
    qDebug() << "Object released";
}

void BleAdvertisement::Ping() const
{
    qDebug() << "Pong!";
}

QString BleAdvertisement::type() const
{
    return m_type;
}

QStringList BleAdvertisement::serviceUUIDs() const
{
    return m_serviceUUIDs;
}

QVariantMap BleAdvertisement::manufacturerData() const
{
    return m_manufacturerData;
}

QStringList BleAdvertisement::solicitUUIDs() const
{
    return m_solicitUUIDs;
}

QVariantMap BleAdvertisement::serviceData() const
{
    return m_serviceData;
}

QStringList BleAdvertisement::includes() const
{
    return m_includes;
}

QString BleAdvertisement::localName() const
{
    return m_localName;
}

ushort BleAdvertisement::appearance() const
{
    return m_appearance;
}

ushort BleAdvertisement::duration() const
{
    return m_duration;
}

ushort BleAdvertisement::timeout() const
{
    return m_timeout;
}

void BleAdvertisement::setType(const QString &type)
{
    if (m_type != type) {
        m_type = type;
        emit typeChanged();
    }
}

void BleAdvertisement::setServiceUUIDs(const QStringList &serviceUUIDs)
{
    if (m_serviceUUIDs != serviceUUIDs) {
        m_serviceUUIDs = serviceUUIDs;
        emit serviceUUIDsChanged();
    }
}

void BleAdvertisement::setManufacturerData(const QVariantMap &manufacturerData)
{
    if (m_manufacturerData != manufacturerData) {
        m_manufacturerData = manufacturerData;
        emit manufacturerDataChanged();
    }
}

void BleAdvertisement::setSolicitUUIDs(const QStringList &solicitUUIDs)
{
    if (m_solicitUUIDs != solicitUUIDs) {
        m_solicitUUIDs = solicitUUIDs;
        emit solicitUUIDsChanged();
    }
}

void BleAdvertisement::setServiceData(const QVariantMap &serviceData)
{
    if (m_serviceData != serviceData) {
        m_serviceData = serviceData;
        qDebug() << "Service data value: " << serviceData.value(serviceData.keys()[0]).toByteArray().toHex();
        emit serviceDataChanged();
    }
}

void BleAdvertisement::setIncludes(const QStringList &includes)
{
    if (m_includes != includes) {
        m_includes = includes;
        emit includesChanged();
    }
}

void BleAdvertisement::setLocalName(const QString &localName)
{
    if (m_localName != localName) {
        m_localName = localName;
        emit localNameChanged();
    }
}

void BleAdvertisement::setAppearance(ushort appearance)
{
    if (m_appearance != appearance) {
        m_appearance = appearance;
        emit appearanceChanged();
    }
}

void BleAdvertisement::setDuration(ushort duration)
{
    if (m_duration != duration) {
        m_duration = duration;
        emit durationChanged();
    }
}

void BleAdvertisement::setTimeout(ushort timeout)
{
    if (m_timeout != timeout) {
        m_timeout = timeout;
        emit timeoutChanged();
    }
}

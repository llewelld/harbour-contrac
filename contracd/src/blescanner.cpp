#include <QDebug>
#include <QDBusPendingReply>
#include <QDBusArgument>
#include <QDBusMetaType>

#include "blescanner.h"

BleScanner::BleScanner(QObject *parent) : QObject(parent)
  , m_adapterInterface(nullptr)
  , m_scanState(Inactive)
  , m_target(Inactive)
  , m_error(false)
  , m_filteringSupported(true)
{
    QString path = QStringLiteral("/org/bluez/hci0");

    qDebug() << "Registering interface";
    m_adapterInterface = new QDBusInterface("org.bluez", path, "org.bluez.Adapter1", QDBusConnection::systemBus(), this);

    qDBusRegisterMetaType<ObjectStuff>();

    // dbus-send --system --dest=org.bluez --print-reply / org.freedesktop.DBus.ObjectManager.GetManagedObjects
    // dbus-monitor --system "type='signal',sender='org.bluez', interface='org.freedesktop.DBus.ObjectManager',member='InterfacesAdded', path='/'"
    QDBusConnection::systemBus().connect("org.bluez", "/", "org.freedesktop.DBus.ObjectManager", "InterfacesAdded", this, SLOT(onInterfaceAdded(QDBusObjectPath, ObjectStuff)));
}

BleScanner::~BleScanner()
{
    QDBusConnection::systemBus().disconnect("org.bluez", "/", "org.freedesktop.DBus.ObjectManager", "InterfacesAdded", this, SLOT(onInterfaceAdded(QDBusObjectPath, ObjectStuff)));
}

void BleScanner::onInterfaceAdded(QDBusObjectPath, ObjectStuff interfaces_and_properties)
{
    bool result = true;
    QVariantMap properties;
    QString address;
    QByteArray rpi;
    qint16 rssi = 0;

    result = interfaces_and_properties.contains("org.bluez.Device1");

    if (result) {
        properties = interfaces_and_properties.value("org.bluez.Device1");
        result = properties.contains("Address");
    }

    if (result) {
        address = properties.value("Address").toString();
        result = properties.contains("UUIDs");
    }

    if (result) {
        QStringList uuids = properties.value("UUIDs").toStringList();
        result = uuids.contains("0000fd6f-0000-1000-8000-00805f9b34fb");
    }

    if (result) {
        result = properties.contains("ServiceData");
    }

    if (result) {
        // Demarshall the RPI data
        result = false;
        const QDBusArgument serviceData = properties.value("ServiceData").value<QDBusArgument>();
        if (serviceData.currentType() == QDBusArgument::MapType) {
            serviceData.beginMap();
            while (!serviceData.atEnd() && !result) {
                QString key;
                QVariant value;
                serviceData.beginMapEntry();
                serviceData >> key >> value;
                serviceData.endMapEntry();
                if (key == "0000fd6f-0000-1000-8000-00805f9b34fb") {
                    rpi = value.toByteArray();
                    result = true;
                }
            }
            serviceData.endMap();
        }
    }

    if (result) {
        if (properties.contains("RSSI")) {
            rssi = properties.value("RSSI").toInt(&result);
        }

        qDebug() << "Beacon found: " << rpi.toHex();
        emit beaconDiscovered(address, rpi, rssi);
    }
}

void BleScanner::applyDiscoveryFilter()
{
    QVariantMap filter;

    if (m_filteringSupported) {
        qDebug() << "Setting up discovery filter";

        filter.insert("UUIDs", QStringList("0000fd6f-0000-1000-8000-00805f9b34fb"));
        filter.insert("Transport", "le");

        QDBusPendingCall async = m_adapterInterface->asyncCall("SetDiscoveryFilter", filter);

        m_scanState = ApplyingFilter;
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

        connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher* call) {
            qDebug() << "DBus SetDiscoveryFilter returned";

            QDBusPendingReply<> reply = *call;
            if (reply.isError()) {
                if ((reply.error().type() == QDBusError::ErrorType::NotSupported)
                        || (reply.error().type() == QDBusError::ErrorType::Other)) {
                    m_filteringSupported = false;
                    qDebug() << "Filtering not supported";
                    nextState();
                }
                else {
                    dbusError(reply.error());
                }
            }
            else {
                qDebug() << "No error";
                nextState();
            }
            call->deleteLater();
        });
    }
    else {
        qDebug() << "Filtering not supported, moving straight to start scan";
        nextState();
    }
}

bool BleScanner::scan() const
{
    return (m_scanState == Active);
}

bool BleScanner::busy() const
{
    return (m_scanState != m_target);
}

void BleScanner::startDiscovery()
{
    qDebug() << "Starting discovery";

    QDBusPendingCall async = m_adapterInterface->asyncCall("StartDiscovery");

    m_scanState = StartingDiscovery;
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher* call) {
        qDebug() << "DBus StartDiscovery returned";

        QDBusPendingReply<> reply = *call;
        if (reply.isError()) {
            dbusError(reply.error());
        }
        else {
            qDebug() << "No error";
            nextState();
        }
        call->deleteLater();
    });
}

void BleScanner::removeDiscoveryFilter()
{
    qDebug() << "Removing discovery filter";
    m_scanState = RemovingFilter;

    if (m_filteringSupported) {
        QDBusPendingCall async = m_adapterInterface->asyncCall("SetDiscoveryFilter", QVariantMap());

        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

        connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher* call) {
            qDebug() << "DBus SetDiscoveryFilter returned";

            QDBusPendingReply<> reply = *call;
            if (reply.isError()) {
                dbusError(reply.error());
            }
            else {
                qDebug() << "No error";
                nextState();
            }
            call->deleteLater();
        });
    }
    else {
        qDebug() << "No discovery filter to remove, moving straight to stop discovery";
        nextState();
    }
}

void BleScanner::stopDiscovery()
{
    qDebug() << "Stopping discovery";

    QDBusPendingCall async = m_adapterInterface->asyncCall("StopDiscovery");

    m_scanState = StoppingDiscovery;
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher* call) {
        qDebug() << "DBus StopDiscovery returned";

        QDBusPendingReply<> reply = *call;
        if (reply.isError()) {
            dbusError(reply.error());
        }
        else {
            qDebug() << "No error";
            nextState();
        }
        call->deleteLater();
    });
}

void BleScanner::setScan(bool scan)
{
    if (scan) {
        if (m_target != Active) {
            m_target = Active;
            if (m_scanState == Inactive) {
                emit busyChanged();
            }
            nextState();
        }
    }
    else {
        if (m_target != Inactive) {
            m_target = Inactive;
            if (m_scanState == Inactive) {
                emit busyChanged();
            }
            nextState();
        }
    }
}

void BleScanner::nextState()
{
    bool scanning = scan();
    if (m_target == Active) {
        switch (m_scanState) {
        case Inactive:
            applyDiscoveryFilter();
            break;
        case ApplyingFilter:
            startDiscovery();
            break;
        case StartingDiscovery:
            // Success
            m_scanState = Active;
            emit busyChanged();
            break;
        case Active:
            // Do nothing
            break;
        case StoppingDiscovery:
            startDiscovery();
            break;
        case RemovingFilter:
            applyDiscoveryFilter();
            break;
        }
    }
    else {
        switch (m_scanState) {
        case Inactive:
            // Do nothing
            break;
        case ApplyingFilter:
            removeDiscoveryFilter();
            break;
        case StartingDiscovery:
            stopDiscovery();
            break;
        case Active:
            stopDiscovery();
            break;
        case StoppingDiscovery:
            removeDiscoveryFilter();
            break;
        case RemovingFilter:
            // Success
            m_scanState = Inactive;
            emit busyChanged();
            break;
        }
    }

    if (scanning != scan()) {
        emit scanChanged();
    }
}

void BleScanner::dbusError(QDBusError error)
{
    qDebug() << "DBus returned error: " << error.message() << " (" << error.type() << ")";
    if (!m_error) {
        // Attempt to climb down gracefully
        m_target = Inactive;
        nextState();
    }
    else {
        // Multiple errors, giving up
        m_target = Inactive;
        m_scanState = Inactive;
    }
}


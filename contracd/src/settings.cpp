#include <QDebug>

#include "settings.h"
#include "version.h"

#define SETTINGS_MAX_VERSION (0)

Settings *Settings::instance = nullptr;

Settings::Settings(QObject *parent)
    : QObject(parent)
    , m_settings(this)
    , m_dirty(false)
{
    m_tracingKey = m_settings.value(QStringLiteral("keys/tracingKey"), QVariant(QByteArray())).toByteArray();
    m_enabled = m_settings.value(QStringLiteral("state/enabled"), false).toBool();
    m_sent = m_settings.value(QStringLiteral("state/sent"), 0).toUInt();
    m_received = m_settings.value(QStringLiteral("state/received"), 0).toUInt();
    m_txPower = static_cast<qint8>(m_settings.value(QStringLiteral("configuration/txPower"), -30).toInt());
    m_rssiCorrection = static_cast<qint8>(m_settings.value(QStringLiteral("configuration/rssiCorretion"), 5).toInt());

    qDebug() << "Settings created: " << m_settings.fileName();

    upgrade();
}

Settings::~Settings()
{
    writeSettingsToDisk();
    instance = nullptr;
    qDebug() << "Deleted settings";
}

void Settings::instantiate(QObject *parent)
{
    if (instance == nullptr) {
        instance = new Settings(parent);
    }
}

Settings &Settings::getInstance()
{
    return *instance;
}

QByteArray Settings::tracingKey() const
{
    return m_tracingKey;
}

void Settings::setTracingKey(QByteArray const &tracingKey)
{
    if (m_tracingKey != tracingKey) {
        m_tracingKey = tracingKey;
        m_dirty = true;

        emit tracingKeyChanged();
    }
}

bool Settings::enabled() const
{
    return m_enabled;
}

void Settings::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        m_dirty = true;

        emit enabledChanged();
    }
}

quint32 Settings::sent() const
{
    return m_sent;
}

void Settings::setSent(quint32 sent)
{
    if (m_sent != sent) {
        m_sent = sent;
        m_dirty = true;

        emit sentChanged();
    }
}

quint32 Settings::received() const
{
    return m_received;
}

void Settings::setReceived(quint32 received)
{
    if (m_received != received) {
        m_received = received;
        m_dirty = true;

        emit receivedChanged();
    }
}

qint8 Settings::txPower() const
{
    // The txPower is an empirically-determined device-specific value
    // See https://developers.google.com/android/exposure-notifications/ble-attenuation-overview

    return m_txPower;
}

void Settings::setTxPower(qint8 txPower)
{
    if (m_txPower != txPower) {
        m_txPower = txPower;
        m_dirty = true;

        emit txPowerChanged();
    }
}

qint8 Settings::rssiCorrection() const
{
    // The rssiCorrection is an empirically-determined device-specific value
    // See https://developers.google.com/android/exposure-notifications/ble-attenuation-overview

    return m_rssiCorrection;
}

void Settings::setRssiCorrection(qint8 rssiCorrection)
{
    if (m_rssiCorrection != rssiCorrection) {
        m_rssiCorrection = rssiCorrection;
        m_dirty = true;

        emit rssiCorrectionChanged();
    }
}

void Settings::writeSettingsToDisk(bool force)
{
    if (m_dirty || force) {
        m_settings.setValue(QStringLiteral("keys/tracingKey"), m_tracingKey);
        m_settings.setValue(QStringLiteral("state/enabled"), m_enabled);
        m_settings.setValue(QStringLiteral("state/sent"), m_sent);
        m_settings.setValue(QStringLiteral("state/received"), m_received);
        m_settings.setValue(QStringLiteral("configuration/txPower"), m_txPower);
        m_settings.setValue(QStringLiteral("configuration/rssiCorrection"), m_rssiCorrection);
        m_dirty = false;
    }
}

bool Settings::upgrade()
{
    quint32 version;
    bool success = true;

    if (m_settings.allKeys().size() == 0) {
        version = SETTINGS_MAX_VERSION;
        qDebug() << "Creating new settings file with version: " << SETTINGS_MAX_VERSION;
    }
    else {
        version = m_settings.value(QStringLiteral("application/settingsVersion"), 0).toUInt();
        qDebug() << "Existing settings file version: " << version;
    }

    switch (version) {
    default:
    case SETTINGS_MAX_VERSION:
        // File upgraded
        // Do nothing
        break;
    }

    if (success) {
        m_settings.setValue(QStringLiteral("application/version"), VERSION);
        m_settings.setValue(QStringLiteral("application/settingsVersion"), SETTINGS_MAX_VERSION);
    }

    return success;
}

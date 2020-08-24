#include <QDebug>

#include "settings.h"

Settings * Settings::instance = nullptr;

Settings::Settings(QObject *parent) : QObject(parent),
    settings(this)
{
    m_tracingKey = settings.value(QStringLiteral("keys/tracingKey"), QVariant(QByteArray())).toByteArray();
    m_enabled = settings.value(QStringLiteral("state/enabled"), false).toBool();
    m_sent = settings.value(QStringLiteral("state/sent"), 0).toUInt();
    m_received = settings.value(QStringLiteral("state/received"), 0).toUInt();
    m_txPower = static_cast<qint8>(settings.value(QStringLiteral("configuration/txPower"), -30).toInt());
    m_rssiCorrection = static_cast<qint8>(settings.value(QStringLiteral("configuration/rssiCorretion"), 5).toInt());

    qDebug() << "Settings created: " << settings.fileName();
}

Settings::~Settings()
{
    settings.setValue(QStringLiteral("keys/tracingKey"), m_tracingKey);
    settings.setValue(QStringLiteral("state/enabled"), m_enabled);
    settings.setValue(QStringLiteral("state/sent"), m_sent);
    settings.setValue(QStringLiteral("state/received"), m_received);
    settings.setValue(QStringLiteral("configuration/txPower"), m_txPower);
    settings.setValue(QStringLiteral("configuration/rssiCorrection"), m_rssiCorrection);

    instance = nullptr;
    qDebug() << "Deleted settings";
}

void Settings::instantiate(QObject *parent) {
    if (instance == nullptr) {
        instance = new Settings(parent);
    }
}

Settings & Settings::getInstance() {
    return *instance;
}

Q_INVOKABLE QByteArray Settings::tracingKey() const
{
    return m_tracingKey;
}

void Settings::setTracingKey(QByteArray const &tracingKey)
{
    if (m_tracingKey != tracingKey) {
        m_tracingKey = tracingKey;

        emit tracingKeyChanged();
    }
}

Q_INVOKABLE bool Settings::enabled() const
{
    return m_enabled;
}

void Settings::setEnabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;

        emit enabledChanged();
    }
}

Q_INVOKABLE quint32 Settings::sent() const
{
    return m_sent;
}

void Settings::setSent(quint32 sent)
{
    if (m_sent != sent) {
        m_sent = sent;

        emit sentChanged();
    }
}

Q_INVOKABLE quint32 Settings::received() const
{
    return m_received;
}

void Settings::setReceived(quint32 received)
{
    if (m_received != received) {
        m_received = received;

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

        emit rssiCorrectionChanged();
    }
}

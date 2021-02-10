#ifndef CONTRACD_SETTINGS_H
#define CONTRACD_SETTINGS_H

#include <QObject>
#include <QSettings>

class Settings : public QObject
{
    Q_OBJECT

    // General properties
    Q_PROPERTY(QByteArray tracingKey READ tracingKey WRITE setTracingKey NOTIFY tracingKeyChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(quint32 sent READ sent WRITE setSent NOTIFY sentChanged)
    Q_PROPERTY(quint32 received READ received WRITE setReceived NOTIFY receivedChanged)
    Q_PROPERTY(qint8 txPower READ txPower WRITE setTxPower NOTIFY txPowerChanged)
    Q_PROPERTY(qint8 rssiCorrection READ rssiCorrection WRITE setRssiCorrection NOTIFY rssiCorrectionChanged)

public:
    explicit Settings(QObject *parent = nullptr);
    ~Settings();

    static void instantiate(QObject *parent = nullptr);
    static Settings & getInstance();

    Q_INVOKABLE QByteArray tracingKey() const;
    Q_INVOKABLE bool enabled() const;
    Q_INVOKABLE quint32 sent() const;
    Q_INVOKABLE quint32 received() const;
    Q_INVOKABLE qint8 txPower() const;
    Q_INVOKABLE qint8 rssiCorrection() const;

    void writeSettingsToDisk(bool force);

signals:
    void tracingKeyChanged();
    void enabledChanged();
    void sentChanged();
    void receivedChanged();
    void txPowerChanged();
    void rssiCorrectionChanged();

public slots:
    void setTracingKey(QByteArray const &tracingKey);
    void setEnabled(bool enabled);
    void setSent(quint32 sent);
    void setReceived(quint32 received);
    void setTxPower(qint8 txPower);
    void setRssiCorrection(qint8 rssiCorrection);

private:
    bool upgrade();

private:
    static Settings * instance;
    QSettings m_settings;
    bool m_dirty;

    QByteArray m_tracingKey;
    bool m_enabled;
    quint32 m_sent;
    quint32 m_received;
    qint8 m_txPower;
    qint8 m_rssiCorrection;
};

#endif // CONTRACD_SETTINGS_H

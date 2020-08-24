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

public:
    explicit Settings(QObject *parent = nullptr);
    ~Settings();

    static void instantiate(QObject *parent = nullptr);
    static Settings & getInstance();

    Q_INVOKABLE QByteArray tracingKey() const;
    Q_INVOKABLE bool enabled() const;
    Q_INVOKABLE quint32 sent() const;
    Q_INVOKABLE quint32 received() const;

signals:
    void tracingKeyChanged();
    void enabledChanged();
    void sentChanged();
    void receivedChanged();

public slots:
    void setTracingKey(QByteArray const &tracingKey);
    void setEnabled(bool enabled);
    void setSent(quint32 sent);
    void setReceived(quint32 received);

private:
    static Settings * instance;
    QSettings settings;

    QByteArray m_tracingKey;
    bool m_enabled;
    quint32 m_sent;
    quint32 m_received;
};

#endif // CONTRACD_SETTINGS_H

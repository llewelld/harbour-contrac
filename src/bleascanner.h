#ifndef BLESCANNER_H
#define BLESCANNER_H

#include <QObject>
#include <QDBusInterface>
#include <QDBusPendingCallWatcher>

typedef QMap<QString, QVariantMap> ObjectStuff;
Q_DECLARE_METATYPE(ObjectStuff)

class BleScanner : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool scan READ scan WRITE setScan NOTIFY scanChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

public:
    explicit BleScanner(QObject *parent = nullptr);
    ~BleScanner();

    bool scan() const;
    void setScan(bool scan);
    bool busy() const;

signals:
    void scanChanged();
    void busyChanged();
    void beaconDiscovered(const QString &address, const QByteArray &rpi, qint16 rssi);

private:
    enum ScanState {
        Inactive,
        ApplyingFilter,
        StartingDiscovery,
        Active,
        StoppingDiscovery,
        RemovingFilter,
    };

private:
    void applyDiscoveryFilter();
    void startDiscovery();

    void stopDiscovery();
    void removeDiscoveryFilter();

    void nextState();
    void dbusError(QDBusError error);

public slots:
    void onInterfaceAdded(QDBusObjectPath, ObjectStuff interfaces_and_properties);

private:
    QDBusInterface *m_adapterInterface;
    ScanState m_scanState;
    ScanState m_target;
    bool m_error;
};

#endif // BLESCANNER_H

#ifndef DBUSPROXY_H
#define DBUSPROXY_H

#include <QObject>
#include <QDBusPendingCall>

#include "../contracd/src/exposuresummary.h"
#include "../contracd/src/exposureinformation.h"
#include "../contracd/src/temporaryexposurekey.h"
#include "../contracd/src/exposureconfiguration.h"

class QDBusInterface;

#define SERVICE_NAME "uk.co.flypig.contrac"

class DBusProxy : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool isEnabled READ isEnabled NOTIFY isEnabledChanged)
    Q_PROPERTY(quint32 maxDiagnosisKeys READ getMaxDiagnosisKeys CONSTANT)

    // Non-standard additions
    Q_PROPERTY(quint32 receivedCount READ receivedCount NOTIFY receivedCountChanged)
    Q_PROPERTY(quint32 sentCount READ sentCount NOTIFY sentCountChanged)
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)
    Q_PROPERTY(qint32 txPower READ txPower WRITE setTxPower NOTIFY txPowerChanged)
    Q_PROPERTY(qint32 rssiCorrection READ rssiCorrection WRITE setRssiCorrection NOTIFY rssiCorrectionChanged)

public:
    explicit DBusProxy(QObject *parent = nullptr);
    ~DBusProxy();

    enum Status
    {
        Success = 0,
        FailedRejectedOptIn,
        FailedServiceDisabled,
        FailedBluetoothScanningDisabled,
        FailedTemporarilyDisabled,
        FailedInsufficientStorage,
        FailedInternal
    };
    Q_ENUM(Status)

    enum ExposureState
    {
        Idle = 0,
        Working
    };
    Q_ENUM(ExposureState)

    Status status() const;
    bool isEnabled() const;
    quint32 getMaxDiagnosisKeys() const;

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE QList<TemporaryExposureKey> getTemporaryExposureKeyHistory();
    Q_INVOKABLE void provideDiagnosisKeys(QStringList const &keyFiles, ExposureConfiguration *configuration, QString token);
    Q_INVOKABLE ExposureSummary *getExposureSummary(QString const &token) const;
    Q_INVOKABLE QList<ExposureInformation> *getExposureInformation(QString const &token) const;
    Q_INVOKABLE void resetAllData();

    // Non-standard additions
    Q_INVOKABLE ExposureState exposureState(QString const &token);
    quint32 receivedCount() const;
    quint32 sentCount() const;
    bool isBusy() const;
    qint32 txPower() const;
    qint32 rssiCorrection() const;
    void setTxPower(qint32 txPower);
    void setRssiCorrection(qint32 rssiCorrection);

signals:
    void statusChanged();
    void isEnabledChanged();

    // Non-standard additions
    void receivedCountChanged();
    void sentCountChanged();
    void isBusyChanged();
    void txPowerChanged();
    void rssiCorrectionChanged();
    void exposureStateChanged(QString const &token);

    // Async responses
    void actionExposureStateUpdated(QString token);

public slots:

private:
    QDBusInterface *m_interface;
    quint32 m_receivedCount;
    quint32 m_sentCount;
};

Q_DECLARE_METATYPE(DBusProxy::Status)

#endif // DBUSPROXY_H

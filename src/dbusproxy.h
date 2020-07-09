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
    Q_ENUMS(Status)

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool isEnabled READ isEnabled NOTIFY isEnabledChanged)
    Q_PROPERTY(quint32 maxDiagnosisKeys READ getMaxDiagnosisKeys CONSTANT)

    // Non-standard additions
    Q_PROPERTY(quint32 receivedCount READ receivedCount NOTIFY receivedCountChanged)
    Q_PROPERTY(quint32 sentCount READ sentCount NOTIFY sentCountChanged)
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)
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

    Status status() const;
    bool isEnabled() const;
    quint32 getMaxDiagnosisKeys() const;

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE QList<TemporaryExposureKey> getTemporaryExposureKeyHistory();
    Q_INVOKABLE void provideDiagnosisKeys(QStringList const &keyFiles, ExposureConfiguration const &configuration, QString token);
    Q_INVOKABLE ExposureSummary *getExposureSummary(QString const &token) const;
    Q_INVOKABLE QList<ExposureInformation> *getExposureInformation(QString const &token) const;
    Q_INVOKABLE void resetAllData();

    // Non-standard additions
    quint32 receivedCount() const;
    quint32 sentCount() const;
    bool isBusy() const;

signals:
    void statusChanged();
    void isEnabledChanged();

    // Non-standard additions
    void receivedCountChanged();
    void sentCountChanged();
    void isBusyChanged();

public slots:

private:
    QDBusInterface *m_interface;
    quint32 m_receivedCount;
    quint32 m_sentCount;
};

Q_DECLARE_METATYPE(DBusProxy::Status)

#endif // DBUSPROXY_H

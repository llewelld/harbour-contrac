#ifndef EXPOSURENOTIFICATION_H
#define EXPOSURENOTIFICATION_H

// Based on Expsure Notification API
// Version 1.3.1
// May 2020

#include <QObject>
#include <QFile>

#include "exposuresummary.h"
#include "exposureinformation.h"
#include "exposureconfiguration.h"
#include "temporaryexposurekey.h"
#include "contrac.h"
#include "blescanner.h"
#include "controller.h"
#include "contactstorage.h"

class ExposureNotificationPrivate;

class ExposureNotification : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool isEnabled READ isEnabled NOTIFY isEnabledChanged)
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)
    Q_PROPERTY(quint32 maxDiagnosisKeys READ maxDiagnosisKeys CONSTANT)
public:
    explicit ExposureNotification(QObject *parent = nullptr);
    ~ExposureNotification();

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
    bool isBusy() const;
    quint32 maxDiagnosisKeys() const;

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE QList<TemporaryExposureKey> getTemporaryExposureKeyHistory();
    Q_INVOKABLE void provideDiagnosisKeys(QVector<QString> const &keyFiles, ExposureConfiguration const &configuration, QString token);
    Q_INVOKABLE ExposureSummary getExposureSummary(QString const &token);
    Q_INVOKABLE QList<ExposureInformation> getExposureInformation(QString const &token);
    Q_INVOKABLE quint32 getMaxDiagnosisKeys() const;
    Q_INVOKABLE void resetAllData();

signals:
    void statusChanged();
    void isEnabledChanged();
    void isBusyChanged();

    void beaconSent();
    void beaconReceived();

    void actionExposureStateUpdated(QString token);

public slots:
    void beaconDiscovered(const QString &address, const QByteArray &data, qint16 rssi);
    void intervalUpdate();
    void onRpiChanged();

private:
    ExposureNotificationPrivate *d_ptr;

    Q_DECLARE_PRIVATE(ExposureNotification)
};

#endif // EXPOSURENOTIFICATION_H

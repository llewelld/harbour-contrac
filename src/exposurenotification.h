#ifndef EXPOSURENOTIFICATION_H
#define EXPOSURENOTIFICATION_H

// Based on Expsure Notification API
// Version 1.2
// April 2020

#include <QObject>
#include <QFile>

#include "exposuresummary.h"
#include "exposureinformation.h"
#include "exposureconfiguration.h"
#include "temporaryexposurekey.h"
#include "contrac.h"
#include "bleascanner.h"
#include "controller.h"
#include "contactstorage.h"

class ExposureNotification : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool isEnabled READ isEnabled NOTIFY isEnabledChanged)
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

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE QList<TemporaryExposureKey> getTemporaryExposureKeyHistory();
    Q_INVOKABLE void provideDiagnosisKeys(QVector<QString> const &keyFiles, ExposureConfiguration const &configuration, QString token);
    Q_INVOKABLE ExposureSummary const &getExposureSummary(QString const &token);
    Q_INVOKABLE QList<ExposureInformation> getExposureInformation(QString const &token);
    Q_INVOKABLE quint32 getMaxDiagnosisKeys() const;
    Q_INVOKABLE void resetAllData();

    Status status() const;
    bool isEnabled() const;
    quint32 maxDiagnosisKeys() const;

signals:
    void statusChanged();
    void isEnabledChanged();

public slots:
    void beaconDiscovered(const QString &address, const QByteArray &rpi, qint16 rssi);

private:
    void loadDiagnosisKeys(QString const &keyFile, ExposureConfiguration const &configuration, QString token);

private:
    Status m_status;
    Contrac *m_contrac;
    BleScanner *m_scanner;
    Controller *m_controller;
    ContactStorage *m_contacts;
};

#endif // EXPOSURENOTIFICATION_H

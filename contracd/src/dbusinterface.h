#ifndef DBUSINTERFACE_H
#define DBUSINTERFACE_H

#include <QObject>
#include <QtDBus>

#include "exposurenotification.h"

#define SERVICE_NAME "uk.co.flypig.contrac"
#define SERVICE_PATH "/"

class DBusInterface : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", SERVICE_NAME)

    Q_PROPERTY(qint32 status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool isEnabled READ isEnabled NOTIFY isEnabledChanged)
    Q_PROPERTY(quint32 maxDiagnosisKeys READ maxDiagnosisKeys CONSTANT)

    // Non-standard additions
    Q_PROPERTY(quint32 receivedCount READ receivedCount NOTIFY receivedCountChanged)
    Q_PROPERTY(quint32 sentCount READ sentCount NOTIFY sentCountChanged)
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)
    Q_PROPERTY(qint32 txPower READ txPower WRITE setTxPower NOTIFY txPowerChanged)
    Q_PROPERTY(qint32 rssiCorrection READ rssiCorrection WRITE setRssiCorrection NOTIFY rssiCorrectionChanged)

public:
    explicit DBusInterface(bool &result, QObject *parent = nullptr);
    ~DBusInterface();

    Q_INVOKABLE qint32 status() const;
    Q_INVOKABLE bool isEnabled() const;
    Q_INVOKABLE quint32 maxDiagnosisKeys() const;

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE QList<TemporaryExposureKey> getTemporaryExposureKeyHistory();
    Q_INVOKABLE void provideDiagnosisKeys(QStringList const &keyFiles, ExposureConfiguration const &configuration, QString token);
    Q_INVOKABLE ExposureSummary getExposureSummary(QString const &token);
    Q_INVOKABLE QList<ExposureInformation> getExposureInformation(QString const &token);
    Q_INVOKABLE quint32 getMaxDiagnosisKeys() const;
    Q_INVOKABLE void resetAllData();

    // Non-standard additions
    Q_INVOKABLE qint32 exposureState(QString const token) const;
    Q_INVOKABLE quint32 receivedCount() const;
    Q_INVOKABLE quint32 sentCount() const;
    Q_INVOKABLE bool isBusy() const;
    Q_INVOKABLE qint32 txPower() const;
    Q_INVOKABLE qint32 rssiCorrection() const;
    Q_INVOKABLE void setTxPower(qint32 txPower);
    Q_INVOKABLE void setRssiCorrection(qint32 rssiCorrection);
    Q_INVOKABLE QDateTime lastProcessTime(QString const token) const;

signals:
    void statusChanged();
    void isEnabledChanged();
    void actionExposureStateUpdated(QString const token);

    // Non-standard additions
    void receivedCountChanged();
    void sentCountChanged();
    void isBusyChanged();
    void txPowerChanged();
    void rssiCorrectionChanged();
    void exposureStateChanged(QString const &token);

public slots:

private slots:
    void incrementReceiveCount();
    void incrementSentCount();

private:
    QDBusConnection m_connection;
    ExposureNotification m_exposureNotification;
};



#endif // DBUSINTERFACE_H

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

    Q_PROPERTY(ExposureNotification::Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool isEnabled READ isEnabled NOTIFY isEnabledChanged)
    Q_PROPERTY(quint32 maxDiagnosisKeys READ maxDiagnosisKeys CONSTANT)
public:
    explicit DBusInterface(QObject *parent = nullptr);
    ~DBusInterface();

    ExposureNotification::Status status() const;
    bool isEnabled() const;
    quint32 maxDiagnosisKeys() const;

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE QList<TemporaryExposureKey> getTemporaryExposureKeyHistory();
    Q_INVOKABLE void provideDiagnosisKeys(QVector<QString> const &keyFiles, ExposureConfiguration const &configuration, QString token);
    Q_INVOKABLE ExposureSummary getExposureSummary(QString const &token) const;
    Q_INVOKABLE QList<ExposureInformation> getExposureInformation(QString const &token) const;
    Q_INVOKABLE quint32 getMaxDiagnosisKeys() const;
    Q_INVOKABLE void resetAllData();

signals:
    void statusChanged();
    void isEnabledChanged();

public slots:

private:
    QDBusConnection m_connection;
    ExposureNotification m_exposureNotification;
};

#endif // DBUSINTERFACE_H

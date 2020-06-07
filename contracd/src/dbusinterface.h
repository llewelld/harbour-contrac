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

    // Non-standard additions
    Q_PROPERTY(quint32 receivedCount READ receivedCount NOTIFY receivedCountChanged)
    Q_PROPERTY(quint32 sentCount READ sentCount NOTIFY sentCountChanged)

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

    // Non-standard additions
    Q_INVOKABLE quint32 receivedCount() const;
    Q_INVOKABLE quint32 sentCount() const;

signals:
    void statusChanged();
    void isEnabledChanged();

    // Non-standard additions
    void receivedCountChanged();
    void sentCountChanged();

public slots:

private slots:
    void incrementReceiveCount();
    void incrementSentCount();

private:
    QDBusConnection m_connection;
    ExposureNotification m_exposureNotification;

    quint32 m_sentCount;
    quint32 m_receivedCount;
};

typedef QList<TemporaryExposureKey> TemporaryExposureKeyList;

QDBusArgument &operator<<(QDBusArgument &argument, const TemporaryExposureKeyList &temporaryExposureKeyList);
QDBusArgument const &operator>>(const QDBusArgument &argument, TemporaryExposureKeyList &temporaryExposureKeyList);

Q_DECLARE_METATYPE(TemporaryExposureKeyList)

typedef QList<ExposureInformation> ExposureInformationList;

QDBusArgument &operator<<(QDBusArgument &argument, const ExposureInformationList &exposureInformationList);
QDBusArgument const &operator>>(const QDBusArgument &argument, ExposureInformationList &exposureInformationList);

Q_DECLARE_METATYPE(ExposureInformationList)


#endif // DBUSINTERFACE_H

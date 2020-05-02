/*
 * Copyright 2020, David Llewellyn-Jones <david@flypig.co.uk>
 * Released under the GPLv2
 *
 * More info at: https://www.flypig.co.uk/contrac
 *
 */

#ifndef BLEADVERTISEMENT_H
#define BLEADVERTISEMENT_H

#include <QObject>
#include <QVariantMap>
#include <QDBusAbstractAdaptor>

#define SERVICE_NAME "org.bluez"
#define OBJECT_INTERFACE "org.bluez.LEAdvertisement1"

class BleAdvertisement : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", OBJECT_INTERFACE)

    Q_PROPERTY(QString Type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QStringList ServiceUUIDs READ serviceUUIDs WRITE setServiceUUIDs NOTIFY serviceUUIDsChanged)
    Q_PROPERTY(QVariantMap ManufacturerData READ manufacturerData WRITE setManufacturerData NOTIFY manufacturerDataChanged)
    Q_PROPERTY(QStringList SolicitUUIDs READ solicitUUIDs WRITE setSolicitUUIDs NOTIFY solicitUUIDsChanged)
    Q_PROPERTY(QVariantMap ServiceData READ serviceData WRITE setServiceData NOTIFY serviceDataChanged)
    Q_PROPERTY(QStringList Includes READ includes WRITE setIncludes NOTIFY includesChanged)
    Q_PROPERTY(QString LocalName READ localName WRITE setLocalName NOTIFY localNameChanged)
    Q_PROPERTY(ushort Appearance READ appearance WRITE setAppearance NOTIFY appearanceChanged)
    Q_PROPERTY(ushort Duration READ duration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(ushort Timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)

public:
    explicit BleAdvertisement(QObject * parent = nullptr);
    ~BleAdvertisement();
    void registerDBus(const QString &path);
    void unRegisterDBus();
    QString getPath() const;

    QString type() const;
    QStringList serviceUUIDs() const;
    QVariantMap manufacturerData() const;
    QStringList solicitUUIDs() const;
    QVariantMap serviceData() const;
    QStringList includes() const;
    QString localName() const;
    ushort appearance() const;
    ushort duration() const;
    ushort timeout() const;

    void setType(const QString &type);
    void setServiceUUIDs(const QStringList &serviceUUIDs);
    void setManufacturerData(const QVariantMap &manufacturerData);
    void setSolicitUUIDs(const QStringList &solicitUUIDs);
    void setServiceData(const QVariantMap &serviceData);
    void setIncludes(const QStringList &includes);
    void setLocalName(const QString &localName);
    void setAppearance(ushort appearance);
    void setDuration(ushort duration);
    void setTimeout(ushort timeout);

public slots:
    Q_NOREPLY void Release();
private:
    void Ping() const;

signals:
    void typeChanged();
    void serviceUUIDsChanged();
    void manufacturerDataChanged();
    void solicitUUIDsChanged();
    void serviceDataChanged();
    void includesChanged();
    void localNameChanged();
    void appearanceChanged();
    void durationChanged();
    void timeoutChanged();

private:
    QString m_path;
    bool m_registered;

    QString m_type;
    QStringList m_serviceUUIDs;
    QVariantMap m_manufacturerData;
    QStringList m_solicitUUIDs;
    QVariantMap m_serviceData;
    QStringList m_includes;
    QString m_localName;
    ushort m_appearance;
    ushort m_duration;
    ushort m_timeout;
};

#endif // BLEADVERTISEMENT_H
